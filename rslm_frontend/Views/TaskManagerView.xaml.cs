using System.Windows.Controls;
using rslm_frontend.ViewModels;

namespace rslm_frontend.Views
{
    public partial class TaskManagerView : UserControl
    {
        private TaskManagerViewModel ViewModel => DataContext as TaskManagerViewModel;

        public TaskManagerView()
        {
            InitializeComponent();
            Loaded += TaskManagerView_Loaded;
            Unloaded += TaskManagerView_Unloaded;
        }

        private void TaskManagerView_Loaded(object sender, System.Windows.RoutedEventArgs e)
        {
            if (ViewModel != null)
            {
                ViewModel.RefreshAsync();
            }
        }

        private void TaskManagerView_Unloaded(object sender, System.Windows.RoutedEventArgs e)
        {
            ViewModel?.Dispose();
        }
    }
}
