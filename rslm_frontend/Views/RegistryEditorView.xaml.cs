using System.Windows;
using System.Windows.Controls;
using rslm_frontend.ViewModels;

namespace rslm_frontend.Views
{
    public partial class RegistryEditorView : UserControl
    {
        private RegistryEditorViewModel ViewModel => DataContext as RegistryEditorViewModel;

        public RegistryEditorView()
        {
            InitializeComponent();
            Loaded += RegistryEditorView_Loaded;
            Unloaded += RegistryEditorView_Unloaded;
        }

        private void RegistryEditorView_Loaded(object sender, System.Windows.RoutedEventArgs e)
        {
        }

        private void RegistryEditorView_Unloaded(object sender, System.Windows.RoutedEventArgs e)
        {
            ViewModel?.Dispose();
        }

        private void TreeView_SelectedItemChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            if (ViewModel != null && e.NewValue is RegistryKeyNode keyNode)
            {
                ViewModel.SelectedKey = keyNode;
                ViewModel.RefreshAsync();
            }
        }
    }
}
