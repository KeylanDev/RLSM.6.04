using System;
using System.ComponentModel;
using System.IO;
using System.Runtime.CompilerServices;
using System.Text;
using System.Windows.Media.Imaging;
using rslm_frontend.ViewModels;

namespace rslm_frontend.ViewModels
{
    public class FileViewerViewModel : INotifyPropertyChanged
    {
        private readonly FileManagerViewModel _fileManager;
        private readonly string _remotePath;

        private string _fileContent;
        private BitmapImage _imageSource;
        private string _hexContent;
        private string _status;
        private string _fileInfo;

        public event PropertyChangedEventHandler PropertyChanged;

        public string FileContent
        {
            get => _fileContent;
            set { _fileContent = value; OnPropertyChanged(); }
        }

        public BitmapImage ImageSource
        {
            get => _imageSource;
            set { _imageSource = value; OnPropertyChanged(); }
        }

        public string HexContent
        {
            get => _hexContent;
            set { _hexContent = value; OnPropertyChanged(); }
        }

        public string Status
        {
            get => _status;
            set { _status = value; OnPropertyChanged(); }
        }

        public string FileInfo
        {
            get => _fileInfo;
            set { _fileInfo = value; OnPropertyChanged(); }
        }

        public FileViewerViewModel(FileManagerViewModel fileManager, string remotePath)
        {
            _fileManager = fileManager;
            _remotePath = remotePath;

            LoadFile();
        }

        private async void LoadFile()
        {
            Status = "Downloading file...";

            try
            {
                // Utiliser la méthode existante du FileManager pour télécharger
                var fileData = await _fileManager.DownloadFileDataAsync(_remotePath);

                if (fileData != null)
                {
                    FileInfo = $"{Path.GetFileName(_remotePath)} ({FormatFileSize(fileData.Length)})";

                    string extension = Path.GetExtension(_remotePath).ToLower();

                    if (IsImageFile(extension))
                    {
                        LoadImage(fileData);
                    }
                    else if (IsTextFile(extension))
                    {
                        LoadText(fileData);
                    }
                    else
                    {
                        LoadHex(fileData);
                    }

                    Status = "Ready";
                }
                else
                {
                    Status = "Failed to load file";
                }
            }
            catch (Exception ex)
            {
                Status = $"Error: {ex.Message}";
            }
        }

        private void LoadImage(byte[] data)
        {
            var bitmap = new BitmapImage();
            using (var stream = new MemoryStream(data))
            {
                bitmap.BeginInit();
                bitmap.CacheOption = BitmapCacheOption.OnLoad;
                bitmap.StreamSource = stream;
                bitmap.EndInit();
            }
            ImageSource = bitmap;
        }

        private void LoadText(byte[] data)
        {
            FileContent = Encoding.UTF8.GetString(data);
        }

        private void LoadHex(byte[] data)
        {
            var sb = new StringBuilder();
            for (int i = 0; i < data.Length; i += 16)
            {
                sb.Append($"{i:X8}  ");
                for (int j = 0; j < 16 && i + j < data.Length; j++)
                {
                    sb.Append($"{data[i + j]:X2} ");
                    if (j == 7) sb.Append(" ");
                }
                sb.Append("  ");
                for (int j = 0; j < 16 && i + j < data.Length; j++)
                {
                    char c = (char)data[i + j];
                    sb.Append(char.IsControl(c) ? '.' : c);
                }
                sb.AppendLine();
            }
            HexContent = sb.ToString();
        }

        private bool IsImageFile(string ext) => ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".gif";
        private bool IsTextFile(string ext) => ext == ".txt" || ext == ".cs" || ext == ".cpp" || ext == ".h" || ext == ".xml" || ext == ".json" || ext == ".js" || ext == ".html" || ext == ".css" || ext == ".ini" || ext == ".cfg" || ext == ".log";

        private string FormatFileSize(long bytes)
        {
            if (bytes < 1024) return $"{bytes} B";
            if (bytes < 1024 * 1024) return $"{bytes / 1024.0:F1} KB";
            if (bytes < 1024 * 1024 * 1024) return $"{bytes / (1024.0 * 1024.0):F1} MB";
            return $"{bytes / (1024.0 * 1024.0 * 1024.0):F1} GB";
        }

        protected void OnPropertyChanged([CallerMemberName] string name = null) =>
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
    }
}