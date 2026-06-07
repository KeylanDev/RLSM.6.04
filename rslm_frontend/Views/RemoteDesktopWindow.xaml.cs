using System.Windows;
using System.Windows.Input;

namespace rslm_frontend.Views
{
    public partial class RemoteDesktopWindow : Window
    {
        public RemoteDesktopWindow()
        {
            InitializeComponent();
        }

        private void TitleBar_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (e.ChangedButton == MouseButton.Left)
                DragMove();
        }

        private void CloseButton_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }
    }
}
