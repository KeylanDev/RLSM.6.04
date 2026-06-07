using System;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media.Imaging;
using Newtonsoft.Json.Linq;
using rslm_frontend.Services;

namespace rslm_frontend.ViewModels
{
    public class RemoteDesktopViewModel : ViewModelBase
    {
        private readonly RslmTcpClient _tcpClient;
        private readonly Action<string, string> _log;
        private string _targetAgentId;
        private BitmapSource _currentFrame;
        private bool _isStreaming;
        private string _statusText = "Not connected";
        private int _quality = 50;
        private int _fps = 5;

        public RemoteDesktopViewModel(RslmTcpClient tcpClient, string targetAgentId, Action<string, string> log)
        {
            _tcpClient = tcpClient;
            _targetAgentId = targetAgentId;
            _log = log;

            StartCommand = new RelayCommand(_ => _ = StartAsync(), _ => !_isStreaming && _tcpClient.IsConnected);
            StopCommand = new RelayCommand(_ => _ = StopAsync(), _ => _isStreaming);

            _tcpClient.MessageReceived += OnTcpMessage;
        }

        public event Action<BitmapSource> FrameReceived;

        public string TargetAgentId
        {
            get => _targetAgentId;
            set => SetProperty(ref _targetAgentId, value);
        }

        public BitmapSource CurrentFrame
        {
            get => _currentFrame;
            set => SetProperty(ref _currentFrame, value);
        }

        public bool IsStreaming
        {
            get => _isStreaming;
            set
            {
                if (SetProperty(ref _isStreaming, value))
                {
                    ((RelayCommand)StartCommand).RaiseCanExecuteChanged();
                    ((RelayCommand)StopCommand).RaiseCanExecuteChanged();
                }
            }
        }

        public string StatusText
        {
            get => _statusText;
            set => SetProperty(ref _statusText, value);
        }

        public int Quality
        {
            get => _quality;
            set => SetProperty(ref _quality, value);
        }

        public int Fps
        {
            get => _fps;
            set => SetProperty(ref _fps, value);
        }

        public ICommand StartCommand { get; }
        public ICommand StopCommand { get; }

        public async System.Threading.Tasks.Task StartAsync()
        {
            if (string.IsNullOrEmpty(_targetAgentId))
            {
                StatusText = "No agent selected";
                _log?.Invoke("Remote Desktop", "No agent selected");
                return;
            }

            if (!_tcpClient.IsConnected)
            {
                StatusText = "Not connected to server";
                _log?.Invoke("Remote Desktop", "Not connected to server");
                return;
            }

            StatusText = "Starting stream...";
            _log?.Invoke("Remote Desktop", "Starting stream...");

            var payload = new JObject
            {
                ["quality"] = _quality,
                ["fps"] = _fps
            };

            var response = await _tcpClient.SendMessageAsync("desktop-start", _targetAgentId, payload);
            if (response["payload"]?["status"]?.ToString() == "streaming")
            {
                IsStreaming = true;
                StatusText = "Streaming started";
                _log?.Invoke("Remote Desktop", "Stream started!");
            }
            else
            {
                IsStreaming = false;
                StatusText = response["error"]?.ToString() ?? "Failed to start";
                _log?.Invoke("Remote Desktop", $"Failed to start: {StatusText}");
            }
        }

        public async System.Threading.Tasks.Task StopAsync()
        {
            if (!string.IsNullOrEmpty(_targetAgentId) && _tcpClient.IsConnected)
            {
                await _tcpClient.SendMessageAsync("desktop-stop", _targetAgentId, new JObject());
            }

            IsStreaming = false;
            StatusText = "Stream stopped";
            _log?.Invoke("Remote Desktop", "Stream stopped");
        }

        public void SendMouseInput(int x, int y, int action, int wheelDelta = 0)
        {
            if (!_tcpClient.IsConnected || string.IsNullOrEmpty(_targetAgentId) || !_isStreaming)
                return;

            var payload = new JObject
            {
                ["x"] = x,
                ["y"] = y,
                ["action"] = action,
                ["wheelDelta"] = wheelDelta
            };

            _ = _tcpClient.SendFireAndForgetAsync("desktop-mouse", _targetAgentId, payload);
        }

        public void SendKeyboardInput(int keyCode, bool keyDown)
        {
            if (!_tcpClient.IsConnected || string.IsNullOrEmpty(_targetAgentId) || !_isStreaming)
                return;

            var payload = new JObject
            {
                ["keyCode"] = keyCode,
                ["keyDown"] = keyDown
            };

            _ = _tcpClient.SendFireAndForgetAsync("desktop-keyboard", _targetAgentId, payload);
        }

        private void OnTcpMessage(JObject msg)
        {
            if (msg.Value<string>("type") != "desktop-frame")
                return;

            _log?.Invoke("Remote Desktop", $"Got desktop-frame! Msg: {msg.ToString()}");

            var base64 = msg["payload"]?["data"]?.ToString();
            if (!string.IsNullOrEmpty(base64))
            {
                try
                {
                    var data = Convert.FromBase64String(base64);
                    using (var ms = new System.IO.MemoryStream(data))
                    {
                        var image = new BitmapImage();
                        image.BeginInit();
                        image.StreamSource = ms;
                        image.CacheOption = BitmapCacheOption.OnLoad;
                        image.EndInit();
                        image.Freeze();

                        Application.Current.Dispatcher.Invoke(() =>
                        {
                            CurrentFrame = image;
                            FrameReceived?.Invoke(image);
                            _log?.Invoke("Remote Desktop", "Set CurrentFrame!");
                        });
                    }
                }
                catch (Exception ex)
                {
                    _log?.Invoke("Remote Desktop", $"Error decoding frame: {ex}");
                }
            }
            else
            {
                _log?.Invoke("Remote Desktop", "No data in desktop-frame payload!");
            }
        }

        public void Dispose()
        {
            _tcpClient.MessageReceived -= OnTcpMessage;
            if (IsStreaming)
                _ = StopAsync();
        }
    }
}
