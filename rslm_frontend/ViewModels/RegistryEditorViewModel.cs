using System;
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using System.Windows.Input;
using Newtonsoft.Json.Linq;
using rslm_frontend.Services;

namespace rslm_frontend.ViewModels
{
    public class RegistryEditorViewModel : ViewModelBase
    {
        private readonly RslmTcpClient _tcpClient;
        private readonly Action<string, string> _log;
        private readonly ObservableCollection<RegistryKeyNode> _rootKeys = new ObservableCollection<RegistryKeyNode>();
        private readonly ObservableCollection<RegistryValueItem> _values = new ObservableCollection<RegistryValueItem>();
        private RegistryKeyNode _selectedKey;
        private RegistryValueItem _selectedValue;
        private string _statusText = "Ready";
        private string _targetAgentId;
        private bool _isBusy;

        public RegistryEditorViewModel(RslmTcpClient tcpClient, string targetAgentId, Action<string, string> log)
        {
            _tcpClient = tcpClient;
            _targetAgentId = targetAgentId ?? "";
            _log = log;

            RefreshCommand = new RelayCommand(_ => _ = RefreshAsync(), _ => _tcpClient.IsConnected && !IsBusy);
            DeleteValueCommand = new RelayCommand(_ => _ = DeleteValueAsync(), _ => _tcpClient.IsConnected && !IsBusy && SelectedValue != null);

            // Add root keys
            _rootKeys.Add(new RegistryKeyNode { Name = "HKEY_LOCAL_MACHINE", Path = "HKEY_LOCAL_MACHINE" });
            _rootKeys.Add(new RegistryKeyNode { Name = "HKEY_CURRENT_USER", Path = "HKEY_CURRENT_USER" });
            _rootKeys.Add(new RegistryKeyNode { Name = "HKEY_CLASSES_ROOT", Path = "HKEY_CLASSES_ROOT" });
            _rootKeys.Add(new RegistryKeyNode { Name = "HKEY_CURRENT_CONFIG", Path = "HKEY_CURRENT_CONFIG" });
            _rootKeys.Add(new RegistryKeyNode { Name = "HKEY_USERS", Path = "HKEY_USERS" });
        }

        public ObservableCollection<RegistryKeyNode> RootKeys => _rootKeys;
        public ObservableCollection<RegistryValueItem> Values => _values;

        public RegistryKeyNode SelectedKey
        {
            get => _selectedKey;
            set => SetProperty(ref _selectedKey, value);
        }

        public RegistryValueItem SelectedValue
        {
            get => _selectedValue;
            set
            {
                if (SetProperty(ref _selectedValue, value))
                {
                    ((RelayCommand)DeleteValueCommand).RaiseCanExecuteChanged();
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
                    ((RelayCommand)RefreshCommand).RaiseCanExecuteChanged();
                    ((RelayCommand)DeleteValueCommand).RaiseCanExecuteChanged();
                }
            }
        }

        public ICommand RefreshCommand { get; }
        public ICommand DeleteValueCommand { get; }

        public async Task RefreshAsync()
        {
            if (string.IsNullOrEmpty(_targetAgentId))
            {
                StatusText = "No agent selected";
                return;
            }

            if (SelectedKey == null)
            {
                StatusText = "Select a key first";
                return;
            }

            IsBusy = true;
            StatusText = "Loading values...";

            try
            {
                var payload = new JObject { ["path"] = SelectedKey.Path };
                var response = await _tcpClient.SendMessageAsync("reg-enum", _targetAgentId, payload);

                if (response["payload"]?["values"] is JArray valuesArray)
                {
                    _values.Clear();
                    foreach (var v in valuesArray)
                    {
                        _values.Add(new RegistryValueItem
                        {
                            Name = v["name"]?.ToString() ?? "",
                            Type = v["type"]?.ToString() ?? "",
                            Data = v["data"]?.ToString() ?? ""
                        });
                    }
                    StatusText = $"Loaded {_values.Count} values";
                    _log("Registry Editor", $"Loaded {_values.Count} values from {SelectedKey.Path}");
                }
                else
                {
                    StatusText = "Failed to load values";
                    _log("Registry Editor", "Failed to load values");
                }
            }
            catch (Exception ex)
            {
                StatusText = "Error: " + ex.Message;
                _log("Registry Editor", "Error: " + ex.Message);
            }
            finally
            {
                IsBusy = false;
            }
        }

        public async Task DeleteValueAsync()
        {
            if (SelectedValue == null || string.IsNullOrEmpty(_targetAgentId)) return;

            IsBusy = true;
            StatusText = "Deleting value...";

            try
            {
                var payload = new JObject
                {
                    ["path"] = SelectedKey.Path,
                    ["name"] = SelectedValue.Name
                };
                await _tcpClient.SendMessageAsync("reg-delete", _targetAgentId, payload);
                StatusText = "Value deleted";
                _log("Registry Editor", $"Deleted value {SelectedValue.Name} from {SelectedKey.Path}");
                await RefreshAsync();
            }
            catch (Exception ex)
            {
                StatusText = "Error: " + ex.Message;
                _log("Registry Editor", "Error: " + ex.Message);
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

    public class RegistryKeyNode : ViewModelBase
    {
        private string _name;
        private string _path;

        public string Name
        {
            get => _name;
            set => SetProperty(ref _name, value);
        }

        public string Path
        {
            get => _path;
            set => SetProperty(ref _path, value);
        }
    }

    public class RegistryValueItem : ViewModelBase
    {
        private string _name;
        private string _type;
        private string _data;

        public string Name
        {
            get => _name;
            set => SetProperty(ref _name, value);
        }

        public string Type
        {
            get => _type;
            set => SetProperty(ref _type, value);
        }

        public string Data
        {
            get => _data;
            set => SetProperty(ref _data, value);
        }
    }
}
