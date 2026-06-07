using System.Windows.Controls;
using rslm_frontend.ViewModels;

namespace rslm_frontend.Views
{
    public partial class SystemInfoView : UserControl
    {
        private SystemInfoViewModel ViewModel => DataContext as SystemInfoViewModel;

        public SystemInfoView()
        {
            InitializeComponent();
            Loaded += SystemInfoView_Loaded;
            Unloaded += SystemInfoView_Unloaded;
        }

        private void SystemInfoView_Loaded(object sender, System.Windows.RoutedEventArgs e)
        {
            if (ViewModel != null)
            {
                ViewModel.RefreshAsync();
            }
        }

        private void SystemInfoView_Unloaded(object sender, System.Windows.RoutedEventArgs e)
        {
            ViewModel?.Dispose();
        }
    }
}
