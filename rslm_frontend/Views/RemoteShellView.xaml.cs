using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using rslm_frontend.ViewModels;

namespace rslm_frontend.Views
{
    public partial class RemoteShellView : UserControl
    {
        private RemoteShellViewModel ViewModel => DataContext as RemoteShellViewModel;

        public RemoteShellView()
        {
            InitializeComponent();
            Loaded += RemoteShellView_Loaded;
            Unloaded += RemoteShellView_Unloaded;
        }

        private void RemoteShellView_Loaded(object sender, RoutedEventArgs e)
        {
            if (ViewModel != null)
            {
                ViewModel.OutputAppended += OnOutputAppended;
                ViewModel.OutputCleared += OnOutputCleared;
                InputBox.Focus();
            }
        }

        private void RemoteShellView_Unloaded(object sender, RoutedEventArgs e)
        {
            if (ViewModel != null)
            {
                ViewModel.OutputAppended -= OnOutputAppended;
                ViewModel.OutputCleared -= OnOutputCleared;
                ViewModel.Dispose();
            }
        }

        private void OnOutputAppended(string text)
        {
            Dispatcher.Invoke(() =>
            {
                ShellOutput.AppendText(text);
                ShellOutput.ScrollToEnd();
            });
        }

        private void OnOutputCleared()
        {
            Dispatcher.Invoke(() =>
            {
                ShellOutput.Clear();
            });
        }

        private async void InputBox_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (ViewModel == null) return;

            if (e.Key == Key.Enter)
            {
                e.Handled = true;
                await ViewModel.SendCommandAsync();
            }
            else if (e.Key == Key.Up)
            {
                e.Handled = true;
                ViewModel.NavigateHistoryUp();
                InputBox.CaretIndex = InputBox.Text.Length;
            }
            else if (e.Key == Key.Down)
            {
                e.Handled = true;
                ViewModel.NavigateHistoryDown();
                InputBox.CaretIndex = InputBox.Text.Length;
            }
        }
    }
}
