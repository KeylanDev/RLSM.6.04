using System;
using System.Threading.Tasks;
using System.Windows.Input;
using Newtonsoft.Json.Linq;
using rslm_frontend.Services;

namespace rslm_frontend.ViewModels
{
    public class SystemInfoViewModel : ViewModelBase
    {
        private readonly RslmTcpClient _tcpClient;
        private readonly Action<string, string> _log;
        private string _computerName;
        private string _username;
        private string _osName;
        private string _osVersion;
        private string _architecture;
        private int _cpuCores;
        private long _totalRam;
        private long _availableRam;
        private string _gpu;
        private bool _isAdmin;
        private string _statusText = "Ready";
        private string _targetAgentId;
        private bool _isBusy;

        public SystemInfoViewModel(RslmTcpClient tcpClient, string targetAgentId, Action<string, string> log)
        {
            _tcpClient = tcpClient;
            _targetAgentId = targetAgentId ?? "";
            _log = log;

            RefreshCommand = new RelayCommand(_ => _ = RefreshAsync(), _ => _tcpClient.IsConnected && !IsBusy);
        }

        public string ComputerName
        {
            get => _computerName;
            set => SetProperty(ref _computerName, value);
        }

        public string Username
        {
            get => _username;
            set => SetProperty(ref _username, value);
        }

        public string OsName
        {
            get => _osName;
            set => SetProperty(ref _osName, value);
        }

        public string OsVersion
        {
            get => _osVersion;
            set => SetProperty(ref _osVersion, value);
        }

        public string Architecture
        {
            get => _architecture;
            set => SetProperty(ref _architecture, value);
        }

        public int CpuCores
        {
            get => _cpuCores;
            set => SetProperty(ref _cpuCores, value);
        }

        public long TotalRam
        {
            get => _totalRam;
            set => SetProperty(ref _totalRam, value);
        }

        public string TotalRamDisplay
        {
            get
            {
                string[] sizes = { "B", "KB", "MB", "GB", "TB" };
                int order = 0;
                double size = TotalRam;
                while (size >= 1024 && order < sizes.Length - 1)
                {
                    order++;
                    size /= 1024;
                }
                return $"{size:0.##} {sizes[order]}";
            }
        }

        public long AvailableRam
        {
            get => _availableRam;
            set => SetProperty(ref _availableRam, value);
        }

        public string AvailableRamDisplay
        {
            get
            {
                string[] sizes = { "B", "KB", "MB", "GB", "TB" };
                int order = 0;
                double size = AvailableRam;
                while (size >= 1024 && order < sizes.Length - 1)
                {
                    order++;
                    size /= 1024;
                }
                return $"{size:0.##} {sizes[order]}";
            }
        }

        public string Gpu
        {
            get => _gpu;
            set => SetProperty(ref _gpu, value);
        }

        public bool IsAdmin
        {
            get => _isAdmin;
            set => SetProperty(ref _isAdmin, value);
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
                    ((RelayCommand)RefreshCommand).RaiseCanExecuteChanged();
                }
            }
        }

        public ICommand RefreshCommand { get; }

        public async Task RefreshAsync()
        {
            if (string.IsNullOrEmpty(_targetAgentId))
            {
                StatusText = "No agent selected";
                return;
            }

            IsBusy = true;
            StatusText = "Loading system info...";

            try
            {
                var response = await _tcpClient.SendMessageAsync("system-info", _targetAgentId, new JObject());

                if (response["payload"] != null)
                {
                    ComputerName = response["payload"]["computerName"]?.ToString() ?? "";
                    Username = response["payload"]["username"]?.ToString() ?? "";
                    OsName = response["payload"]["osName"]?.ToString() ?? "";
                    OsVersion = response["payload"]["osVersion"]?.ToString() ?? "";
                    Architecture = response["payload"]["architecture"]?.ToString() ?? "";
                    CpuCores = response["payload"]["cpuCores"]?.ToObject<int>() ?? 0;
                    TotalRam = response["payload"]["totalRam"]?.ToObject<long>() ?? 0;
                    AvailableRam = response["payload"]["availableRam"]?.ToObject<long>() ?? 0;
                    Gpu = response["payload"]["gpu"]?.ToString() ?? "";
                    IsAdmin = response["payload"]["isAdmin"]?.ToObject<bool>() ?? false;

                    OnPropertyChanged(nameof(TotalRamDisplay));
                    OnPropertyChanged(nameof(AvailableRamDisplay));

                    StatusText = "System info loaded";
                    _log("System Info", "Loaded system information");
                }
                else
                {
                    StatusText = "Failed to load system info";
                    _log("System Info", "Failed to load system info");
                }
            }
            catch (Exception ex)
            {
                StatusText = "Error: " + ex.Message;
                _log("System Info", "Error: " + ex.Message);
            }
            finally
            {
                IsBusy = false;
            }
        }

        public void Dispose()
        {
        }
    }
}
