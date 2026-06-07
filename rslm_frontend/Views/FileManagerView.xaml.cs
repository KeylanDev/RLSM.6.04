using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using rslm_frontend.ViewModels;

namespace rslm_frontend.Views
{
    public partial class FileManagerView : UserControl
    {
        private FileManagerViewModel ViewModel => DataContext as FileManagerViewModel;

        public FileManagerView()
        {
            InitializeComponent();
            Loaded += FileManagerView_Loaded;
            Unloaded += FileManagerView_Unloaded;
        }

        private void FileManagerView_Loaded(object sender, RoutedEventArgs e)
        {
            if (ViewModel != null)
            {
                ViewModel.RefreshAsync();
            }
        }

        private void FileManagerView_Unloaded(object sender, RoutedEventArgs e)
        {
            ViewModel?.Dispose();
        }

        private void FilesListView_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (ViewModel == null) return;

            var selected = ViewModel.SelectedFile;
            if (selected == null) return;

            if (selected.IsDirectory)
            {
                ViewModel.NavigateToPath(selected.Path);
            }
            else
            {
                ViewModel.DownloadAsync();
            }
        }

        private void PathTextBox_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter && ViewModel != null)
            {
                e.Handled = true;
                ViewModel.NavigateToPath(PathTextBox.Text);
            }
        }
    }
}
