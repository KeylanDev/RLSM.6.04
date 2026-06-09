using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using rslm_frontend.ViewModels;
using rslm_frontend.Views;
using AvalonDock.Layout;

namespace rslm_frontend.Views
{
    public partial class FileManagerView : UserControl
    {
        private FileManagerViewModel ViewModel => DataContext as FileManagerViewModel;
        private LayoutDocumentPane _documentPane;

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
                // S'abonner à l'événement d'ouverture de fichier
                ViewModel.FileOpenRequested += OnFileOpenRequested;
            }

            _documentPane = FindDocumentPane(this);
        }

        private void FileManagerView_Unloaded(object sender, RoutedEventArgs e)
        {
            if (ViewModel != null)
            {
                ViewModel.FileOpenRequested -= OnFileOpenRequested;
            }
            ViewModel?.Dispose();
        }

        private void OnFileOpenRequested(string remotePath)
        {
            OpenFileInViewer(remotePath);
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
                OpenFileInViewer(selected.Path);
            }
        }

        private void FilesListView_KeyDown(object sender, KeyEventArgs e)
        {
            if (ViewModel == null) return;

            if (e.Key == Key.Enter)
            {
                var selected = ViewModel.SelectedFile;
                if (selected != null)
                {
                    if (selected.IsDirectory)
                    {
                        ViewModel.NavigateToPath(selected.Path);
                    }
                    else
                    {
                        OpenFileInViewer(selected.Path);
                    }
                    e.Handled = true;
                }
            }
            else if (e.Key == Key.Back)
            {
                ViewModel.ParentCommand.Execute(null);
                e.Handled = true;
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

        private void OpenFileInViewer(string remotePath)
        {
            if (ViewModel == null) return;

            string fileName = System.IO.Path.GetFileName(remotePath);

            // Créer le viewer
            var viewer = new FileViewer();
            var viewModel = new FileViewerViewModel(
                ViewModel,  // Passer le ViewModel directement
                remotePath
            );
            viewer.DataContext = viewModel;

            // Créer un nouveau document dans AvalonDock
            var doc = new LayoutDocument
            {
                Title = fileName,
                Content = viewer,
                ToolTip = remotePath
            };

            if (_documentPane != null)
            {
                _documentPane.Children.Add(doc);
                doc.IsActive = true;
            }
        }

        private LayoutDocumentPane FindDocumentPane(DependencyObject parent)
        {
            if (parent == null) return null;

            for (int i = 0; i < VisualTreeHelper.GetChildrenCount(parent); i++)
            {
                var child = VisualTreeHelper.GetChild(parent, i);
                if (child is LayoutDocumentPane documentPane)
                    return documentPane;

                var result = FindDocumentPane(child);
                if (result != null)
                    return result;
            }
            return null;
        }
    }
}