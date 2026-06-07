using System.Windows.Controls;
using rslm_frontend.ViewModels;

namespace rslm_frontend.Views
{
    public partial class PasswordRecoveryView : UserControl
    {
        private PasswordRecoveryViewModel ViewModel => DataContext as PasswordRecoveryViewModel;

        public PasswordRecoveryView()
        {
            InitializeComponent();
            Loaded += PasswordRecoveryView_Loaded;
            Unloaded += PasswordRecoveryView_Unloaded;
        }

        private void PasswordRecoveryView_Loaded(object sender, System.Windows.RoutedEventArgs e)
        {
        }

        private void PasswordRecoveryView_Unloaded(object sender, System.Windows.RoutedEventArgs e)
        {
            ViewModel?.Dispose();
        }
    }
}
