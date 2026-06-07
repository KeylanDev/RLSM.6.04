using System.Windows.Controls;
using rslm_frontend.ViewModels;

namespace rslm_frontend.Views
{
    public partial class ReverseProxyView : UserControl
    {
        private ReverseProxyViewModel ViewModel => DataContext as ReverseProxyViewModel;

        public ReverseProxyView()
        {
            InitializeComponent();
            Loaded += ReverseProxyView_Loaded;
            Unloaded += ReverseProxyView_Unloaded;
        }

        private void ReverseProxyView_Loaded(object sender, System.Windows.RoutedEventArgs e)
        {
        }

        private void ReverseProxyView_Unloaded(object sender, System.Windows.RoutedEventArgs e)
        {
            ViewModel?.Dispose();
        }
    }
}
