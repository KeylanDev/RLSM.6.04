using System;
using System.Collections.Generic;
using System.Windows.Input;
using Newtonsoft.Json.Linq;
using rslm_frontend.Services;

namespace rslm_frontend.ViewModels
{
	public class RemoteShellViewModel : ViewModelBase
	{
		private readonly RslmTcpClient _tcpClient;
		private readonly Action<string, string> _log;
		private readonly List<string> _history = new List<string>();
		private int _historyIndex = -1;
		private string _inputLine = "";
		private string _prompt = ">";
		private string _statusText = "Not connected";
		private string _targetAgentId;
		private bool _isShellOpen;
		private bool _isBusy;

		public RemoteShellViewModel(RslmTcpClient tcpClient, string targetAgentId, Action<string, string> log)
		{
			_tcpClient = tcpClient;
			_targetAgentId = targetAgentId;
			_log = log;

			ClearCommand = new RelayCommand(_ => OutputCleared?.Invoke());
			OpenShellCommand = new RelayCommand(_ => _ = OpenShellAsync(), _ => _tcpClient.IsConnected && !_isShellOpen);
			CloseShellCommand = new RelayCommand(_ => _ = CloseShellAsync(), _ => _isShellOpen);

			_tcpClient.MessageReceived += OnTcpMessage;

			// Try to open shell automatically if connected
			if (_tcpClient.IsConnected)
			{
				_ = OpenShellAsync();
			}
		}

		public event Action<string> OutputAppended;
		public event Action OutputCleared;

		public string InputLine
		{
			get => _inputLine;
			set => SetProperty(ref _inputLine, value);
		}

		public string Prompt
		{
			get => _prompt;
			set => SetProperty(ref _prompt, value);
		}

		public string StatusText
		{
			get => _statusText;
			set => SetProperty(ref _statusText, value);
		}

		public string TargetAgentId
		{
			get => _targetAgentId;
			set => SetProperty(ref _targetAgentId, value);
		}

		public bool IsShellOpen
		{
			get => _isShellOpen;
			set
			{
				if (SetProperty(ref _isShellOpen, value))
				{
					((RelayCommand)OpenShellCommand).RaiseCanExecuteChanged();
					((RelayCommand)CloseShellCommand).RaiseCanExecuteChanged();
				}
			}
		}

		public ICommand ClearCommand { get; }
		public ICommand OpenShellCommand { get; }
		public ICommand CloseShellCommand { get; }

		public async System.Threading.Tasks.Task OpenShellAsync()
		{
			if (string.IsNullOrEmpty(_targetAgentId))
			{
				StatusText = "No agent selected";
				return;
			}

			if (!_tcpClient.IsConnected)
			{
				StatusText = "Not connected to server";
				return;
			}

			StatusText = "Opening shell...";
			var response = await _tcpClient.SendMessageAsync("shell-open", _targetAgentId, new JObject());

			if (response["payload"]?["status"]?.ToString() == "open")
			{
				IsShellOpen = true;
				Prompt = response["payload"]?["prompt"]?.ToString() ?? ">";
				StatusText = "Shell connected";
				AppendOutput("Shell connected to agent\n");
			}
			else
			{
				IsShellOpen = false;
				StatusText = response["error"]?.ToString() ?? "Failed to open shell";
			}
		}

		public async System.Threading.Tasks.Task CloseShellAsync()
		{
			if (!string.IsNullOrEmpty(_targetAgentId))
			{
				await _tcpClient.SendMessageAsync("shell-close", _targetAgentId, new JObject());
			}
			IsShellOpen = false;
			StatusText = "Shell closed";
		}

		public async System.Threading.Tasks.Task SendCommandAsync()
		{
			var command = InputLine?.Trim();
			if (string.IsNullOrEmpty(command) || _isBusy)
				return;

			if (!IsShellOpen)
			{
				await OpenShellAsync();
				if (!IsShellOpen)
					return;
			}

			_history.Add(command);
			_historyIndex = _history.Count;

			AppendOutput($"{Prompt} {command}\n");
			InputLine = "";

			_isBusy = true;
			try
			{
				var payload = new JObject { ["command"] = command };
				await _tcpClient.SendMessageAsync("shell-command", _targetAgentId, payload);
			}
			catch
			{
				// Ignore
			}
			finally
			{
				_isBusy = false;
			}
		}

		public void NavigateHistoryUp()
		{
			if (_history.Count == 0)
				return;

			if (_historyIndex < 0 || _historyIndex >= _history.Count)
			{
				_historyIndex = _history.Count - 1;
			}
			else if (_historyIndex > 0)
			{
				_historyIndex--;
			}

			InputLine = _history[_historyIndex];
		}

		public void NavigateHistoryDown()
		{
			if (_history.Count == 0)
				return;

			if (_historyIndex >= _history.Count - 1)
			{
				_historyIndex = _history.Count;
				InputLine = "";
				return;
			}

			_historyIndex++;
			InputLine = _history[_historyIndex];
		}

		private void OnTcpMessage(JObject msg)
		{
			if (msg.Value<string>("type") != "shell-output")
				return;

			var output = msg["payload"]?["output"]?.ToString();
			if (!string.IsNullOrEmpty(output))
				AppendOutput(output);
		}

		private void AppendOutput(string text)
		{
			OutputAppended?.Invoke(text);
		}

		public void Dispose()
		{
			_tcpClient.MessageReceived -= OnTcpMessage;
			if (IsShellOpen)
				_ = CloseShellAsync();
		}
	}
}
