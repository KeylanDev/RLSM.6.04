using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Input;
using Newtonsoft.Json.Linq;
using rslm_frontend.Services;

namespace rslm_frontend.ViewModels
{
    public class TaskManagerViewModel : ViewModelBase
    {
        private readonly RslmTcpClient _tcpClient;
        private readonly Action<string, string> _log;
        private readonly ObservableCollection<TaskItem> _allTasks = new ObservableCollection<TaskItem>();
        private readonly ObservableCollection<TaskItem> _tasks = new ObservableCollection<TaskItem>();
        private TaskItem _selectedTask;
        private string _statusText = "Ready";
        private string _targetAgentId;
        private bool _isBusy;
        private string _searchText = "";

        public TaskManagerViewModel(RslmTcpClient tcpClient, string targetAgentId, Action<string, string> log)
        {
            _tcpClient = tcpClient;
            _targetAgentId = targetAgentId ?? "";
            _log = log;

            RefreshCommand = new RelayCommand(_ => _ = RefreshAsync(), _ => _tcpClient.IsConnected && !IsBusy);
            KillCommand = new RelayCommand(_ => _ = KillAsync(), _ => _tcpClient.IsConnected && !IsBusy && SelectedTask != null);
        }

        public ObservableCollection<TaskItem> Tasks => _tasks;

        public TaskItem SelectedTask
        {
            get => _selectedTask;
            set
            {
                if (SetProperty(ref _selectedTask, value))
                {
                    ((RelayCommand)KillCommand).RaiseCanExecuteChanged();
                }
            }
        }

        public string SearchText
        {
            get => _searchText;
            set
            {
                if (SetProperty(ref _searchText, value))
                {
                    FilterTasks();
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
                    ((RelayCommand)KillCommand).RaiseCanExecuteChanged();
                }
            }
        }

        public ICommand RefreshCommand { get; }
        public ICommand KillCommand { get; }

        private void FilterTasks()
        {
            _tasks.Clear();
            var searchLower = SearchText?.ToLower() ?? "";
            foreach (var task in _allTasks)
            {
                if (string.IsNullOrWhiteSpace(searchLower) || task.Name?.ToLower().Contains(searchLower) == true)
                {
                    _tasks.Add(task);
                }
            }
        }

        public async Task RefreshAsync()
        {
            if (string.IsNullOrEmpty(_targetAgentId))
            {
                StatusText = "No agent selected";
                return;
            }

            IsBusy = true;
            StatusText = "Loading processes...";

            try
            {
                var response = await _tcpClient.SendMessageAsync("task-list", _targetAgentId, new JObject());

                if (response["payload"]?["processes"] is JArray processesArray)
                {
                    _allTasks.Clear();
                    foreach (var proc in processesArray)
                    {
                        _allTasks.Add(new TaskItem
                        {
                            Pid = proc["pid"]?.ToObject<int>() ?? 0,
                            Name = proc["name"]?.ToString() ?? "",
                            MemoryUsage = proc["memoryUsage"]?.ToObject<long>() ?? 0,
                            Threads = proc["threads"]?.ToObject<int>() ?? 0
                        });
                    }
                    FilterTasks();
                    StatusText = $"Loaded {_allTasks.Count} processes";
                    _log("Task Manager", $"Loaded {_allTasks.Count} processes");
                }
                else
                {
                    StatusText = "Failed to load processes";
                    _log("Task Manager", "Failed to load processes");
                }
            }
            catch (Exception ex)
            {
                StatusText = "Error: " + ex.Message;
                _log("Task Manager", "Error: " + ex.Message);
            }
            finally
            {
                IsBusy = false;
            }
        }

        public async Task KillAsync()
        {
            if (SelectedTask == null || string.IsNullOrEmpty(_targetAgentId)) return;

            IsBusy = true;
            StatusText = "Killing process...";

            try
            {
                var payload = new JObject { ["pid"] = SelectedTask.Pid };
                await _tcpClient.SendMessageAsync("task-kill", _targetAgentId, payload);
                StatusText = "Process killed";
                _log("Task Manager", $"Killed process {SelectedTask.Pid} ({SelectedTask.Name})");
                await RefreshAsync();
            }
            catch (Exception ex)
            {
                StatusText = "Error: " + ex.Message;
                _log("Task Manager", "Error: " + ex.Message);
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

    public class TaskItem : ViewModelBase
    {
        private int _pid;
        private string _name;
        private long _memoryUsage;
        private int _threads;

        public int Pid
        {
            get => _pid;
            set => SetProperty(ref _pid, value);
        }

        public string Name
        {
            get => _name;
            set => SetProperty(ref _name, value);
        }

        public long MemoryUsage
        {
            get => _memoryUsage;
            set => SetProperty(ref _memoryUsage, value);
        }

        public string MemoryDisplay
        {
            get
            {
                string[] sizes = { "B", "KB", "MB", "GB", "TB" };
                int order = 0;
                double size = MemoryUsage;
                while (size >= 1024 && order < sizes.Length - 1)
                {
                    order++;
                    size /= 1024;
                }
                return $"{size:0.##} {sizes[order]}";
            }
        }

        public int Threads
        {
            get => _threads;
            set => SetProperty(ref _threads, value);
        }
    }
}
