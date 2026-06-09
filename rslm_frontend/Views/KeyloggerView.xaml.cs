using System.Windows.Controls;
using rslm_frontend.ViewModels;

namespace rslm_frontend.Views
{
    public partial class KeyloggerView : UserControl
    {
        private KeyloggerViewModel ViewModel => DataContext as KeyloggerViewModel;

        public KeyloggerView()
        {
            InitializeComponent();
            Loaded += KeyloggerView_Loaded;
            Unloaded += KeyloggerView_Unloaded;
        }

        private void KeyloggerView_Loaded(object sender, System.Windows.RoutedEventArgs e)
        {
            if (ViewModel != null)
            {
                // Pour TextBox, on utilise TextProperty
                var binding = new System.Windows.Data.Binding("LogText")
                {
                    Mode = System.Windows.Data.BindingMode.OneWay,
                    UpdateSourceTrigger = System.Windows.Data.UpdateSourceTrigger.PropertyChanged
                };
                LogTextBox.SetBinding(TextBox.TextProperty, binding);
            }
        }

        private void KeyloggerView_Unloaded(object sender, System.Windows.RoutedEventArgs e)
        {
            ViewModel?.Dispose();
        }
    }
}