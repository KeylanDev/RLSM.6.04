using System;
using System.Collections.Concurrent;
using System.IO;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace rslm_frontend.Services
{
	public class RslmTcpClient
	{
		private TcpClient _client;
		private NetworkStream _stream;
		private readonly string _host;
		private readonly int _port;
		private bool _connected;
		private string _clientId = "admin";
		private readonly StringBuilder _lineBuffer = new StringBuilder();
		private CancellationTokenSource _receiveCts;
		private Task _receiveTask;
		private readonly ConcurrentDictionary<uint, TaskCompletionSource<JObject>> _pending =
			new ConcurrentDictionary<uint, TaskCompletionSource<JObject>>();
		private int _nextTagInt = 1;

		public RslmTcpClient(string host = "127.0.0.1", int port = 4782)
		{
			_host = host;
			_port = port;
		}

		public bool IsConnected => _connected && _client?.Connected == true;
		public string ClientId => _clientId;

		public event Action<JObject> MessageReceived;
		public event Action<string, string> AgentConnected;

		public async Task<bool> ConnectAsync()
		{
			try
			{
				_client = new TcpClient();
				await _client.ConnectAsync(_host, _port);
				_stream = _client.GetStream();
				_connected = true;

				StartReceiveLoop();

				var hello = new JObject
				{
					["type"] = "hello",
					["senderId"] = "",
					["targetId"] = "server",
					["tag"] = 0,
					["timestamp"] = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds(),
					["payload"] = new JObject { ["role"] = "admin", ["tag"] = "RSLM-Admin" }
				};

				await SendRawAsync(hello);
				return true;
			}
			catch
			{
				_connected = false;
				return false;
			}
		}

		public void Disconnect()
		{
			_connected = false;
			_receiveCts?.Cancel();
			_stream?.Close();
			_client?.Close();

			foreach (var pending in _pending.Values)
				pending.TrySetCanceled();
			_pending.Clear();
		}

		public async Task<JObject> SendMessageAsync(string type, string targetId, JObject payload, uint tag = 0)
		{
			if (!IsConnected)
			{
				var reconnected = await ConnectAsync();
				if (!reconnected) return new JObject { ["error"] = "Not connected" };
			}

			if (tag == 0)
				tag = (uint)Interlocked.Increment(ref _nextTagInt);

			var message = new JObject
			{
				["type"] = type,
				["senderId"] = _clientId,
				["targetId"] = targetId ?? "",
				["tag"] = tag,
				["timestamp"] = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds(),
				["payload"] = payload ?? new JObject()
			};

			var tcs = new TaskCompletionSource<JObject>(TaskCreationOptions.RunContinuationsAsynchronously);
			_pending[tag] = tcs;

			try
			{
				await SendRawAsync(message);

				var completed = await Task.WhenAny(tcs.Task, Task.Delay(15000));
				if (completed != tcs.Task)
				{
					_pending.TryRemove(tag, out _);
					return new JObject { ["error"] = "Timeout" };
				}

				return await tcs.Task;
			}
			catch (Exception ex)
			{
				_pending.TryRemove(tag, out _);
				_connected = false;
				return new JObject { ["error"] = ex.Message };
			}
		}

		public async Task SendFireAndForgetAsync(string type, string targetId, JObject payload)
		{
			if (!IsConnected) return;

			var message = new JObject
			{
				["type"] = type,
				["senderId"] = _clientId,
				["targetId"] = targetId ?? "",
				["tag"] = 0,
				["timestamp"] = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds(),
				["payload"] = payload ?? new JObject()
			};

			await SendRawAsync(message);
		}

		[Obsolete("Use SendMessageAsync with protocol types")]
		public async Task<string> SendCommandAsync(string module, string action, string target = "", string data = "")
		{
			var payload = new JObject { ["module"] = module, ["data"] = data };
			var result = await SendMessageAsync(action, target, payload);
			return result.ToString(Formatting.None);
		}

		private async Task SendRawAsync(JObject message)
		{
			string json = message.ToString(Formatting.None) + "\n";
			byte[] buffer = Encoding.UTF8.GetBytes(json);
			await _stream.WriteAsync(buffer, 0, buffer.Length);
		}

		private void StartReceiveLoop()
		{
			_receiveCts = new CancellationTokenSource();
			var token = _receiveCts.Token;

			_receiveTask = Task.Run(async () =>
			{
				var buffer = new byte[65536];
				while (_connected && !token.IsCancellationRequested)
				{
					try
					{
						int bytesRead = await _stream.ReadAsync(buffer, 0, buffer.Length, token);
						if (bytesRead <= 0)
						{
							_connected = false;
							break;
						}

						ProcessReceivedChunk(Encoding.UTF8.GetString(buffer, 0, bytesRead));
					}
					catch (OperationCanceledException)
					{
						break;
					}
					catch
					{
						_connected = false;
						break;
					}
				}
			}, token);
		}

		private void ProcessReceivedChunk(string chunk)
		{
			_lineBuffer.Append(chunk);

			while (true)
			{
				string text = _lineBuffer.ToString();
				int newline = text.IndexOf('\n');
				if (newline < 0) break;

				string line = text.Substring(0, newline).Trim('\r', ' ');
				_lineBuffer.Remove(0, newline + 1);

				if (string.IsNullOrWhiteSpace(line)) continue;

				try
				{
					var msg = JObject.Parse(line);
					HandleIncomingMessage(msg);
				}
				catch
				{
					// ignore malformed lines
				}
			}
		}

		private void HandleIncomingMessage(JObject msg)
		{
			var type = msg.Value<string>("type") ?? "";

			if (type == "hello_ack")
			{
				_clientId = msg["payload"]?.Value<string>("clientId") ?? "admin";
				return;
			}

			if (type == "agent-connected")
			{
				var id = msg["payload"]?.Value<string>("id");
				var agentTag = msg["payload"]?.Value<string>("tag");
				if (!string.IsNullOrEmpty(id))
					AgentConnected?.Invoke(id, agentTag ?? "");
				return;
			}

			uint msgTag = msg.Value<uint?>("tag") ?? 0;
			if (msgTag > 0 && _pending.TryRemove(msgTag, out var tcs))
			{
				tcs.TrySetResult(msg);
				return;
			}

			if (type.EndsWith("-response") || type.EndsWith("-error"))
			{
				MessageReceived?.Invoke(msg);
				return;
			}

			MessageReceived?.Invoke(msg);
		}
	}
}
