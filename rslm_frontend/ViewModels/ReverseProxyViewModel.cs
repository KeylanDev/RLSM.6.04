using System;
using System.Threading.Tasks;
using System.Windows.Input;
using Newtonsoft.Json.Linq;
using rslm_frontend.Services;

namespace rslm_frontend.ViewModels
{
    public class ReverseProxyViewModel : ViewModelBase
    {
        private readonly RslmTcpClient _tcpClient;
        private readonly Action<string, string> _log;
        private string _host = "127.0.0.1";
        private int _port = 1080;
        private bool _isRunning;
        private string _statusText = "Ready";
        private string _targetAgentId;
        private bool _isBusy;

        public ReverseProxyViewModel(RslmTcpClient tcpClient, string targetAgentId, Action<string, string> log)
        {
            _tcpClient = tcpClient;
            _targetAgentId = targetAgentId ?? "";
            _log = log;

            StartCommand = new RelayCommand(_ => _ = StartAsync(), _ => !IsRunning && _tcpClient.IsConnected && !IsBusy);
            StopCommand = new RelayCommand(_ => _ = StopAsync(), _ => IsRunning);
        }

        public string Host
        {
            get => _host;
            set => SetProperty(ref _host, value);
        }

        public int Port
        {
            get => _port;
            set => SetProperty(ref _port, value);
        }

        public bool IsRunning
        {
            get => _isRunning;
            set
            {
                if (SetProperty(ref _isRunning, value))
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

        public async Task StartAsync()
        {
            if (string.IsNullOrEmpty(_targetAgentId))
            {
                StatusText = "No agent selected";
                return;
            }

            IsBusy = true;
            StatusText = "Starting proxy...";

            try
            {
                var payload = new JObject
                {
                    ["host"] = Host,
                    ["port"] = Port
                };
                var response = await _tcpClient.SendMessageAsync("proxy-start", _targetAgentId, payload);

                if (response["payload"]?["status"]?.ToString() == "running" ||
                    response["type"]?.ToString() == "proxy-start-response")
                {
                    IsRunning = true;
                    StatusText = $"Proxy running on {Host}:{Port}";
                    _log("Reverse Proxy", $"Started proxy on {Host}:{Port}");
                }
                else
                {
                    StatusText = "Failed to start: " + (response["payload"]?["error"] ?? response["error"]);
                    _log("Reverse Proxy", "Failed to start: " + StatusText);
                }
            }
            catch (Exception ex)
            {
                StatusText = "Error: " + ex.Message;
                _log("Reverse Proxy", "Error: " + ex.Message);
            }
            finally
            {
                IsBusy = false;
            }
        }

        public async Task StopAsync()
        {
            if (!string.IsNullOrEmpty(_targetAgentId))
                await _tcpClient.SendMessageAsync("proxy-stop", _targetAgentId, new JObject());

            IsRunning = false;
            StatusText = "Proxy stopped";
            _log("Reverse Proxy", "Stopped proxy");
        }

        public void Dispose()
        {
        }
    }
}
