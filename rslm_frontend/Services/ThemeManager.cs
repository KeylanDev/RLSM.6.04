using Microsoft.Win32;
using rslm_frontend.Views;
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Controls.Primitives;

namespace rslm_frontend.Services
{
    public enum AppTheme
    {
        Dark,
        Light,
        System
    }

    public static class ThemeManager
    {
        private static AppTheme _currentTheme = AppTheme.Dark;
        private static readonly string ThemeRegistryKey = @"Software\RSLM\Settings";
        private static readonly string ThemeValueName = "Theme";

        public static event Action<AppTheme> ThemeChanged;

        public static AppTheme CurrentTheme => _currentTheme;

        static ThemeManager()
        {
            LoadThemeFromSettings();
        }

        public static void SetTheme(AppTheme theme)
        {
            _currentTheme = theme;
            ApplyTheme(_currentTheme);
            SaveThemeToSettings();
            ThemeChanged?.Invoke(_currentTheme);
        }

        private static void LoadThemeFromSettings()
        {
            try
            {
                using (var key = Registry.CurrentUser.OpenSubKey(ThemeRegistryKey))
                {
                    if (key != null)
                    {
                        int savedTheme = (int)key.GetValue(ThemeValueName, 0);
                        if (savedTheme >= 0 && savedTheme <= 2)
                            _currentTheme = (AppTheme)savedTheme;
                    }
                }
            }
            catch { }
            ApplyTheme(_currentTheme);
        }

        private static void SaveThemeToSettings()
        {
            try
            {
                using (var key = Registry.CurrentUser.CreateSubKey(ThemeRegistryKey))
                {
                    if (key != null)
                    {
                        key.SetValue(ThemeValueName, (int)_currentTheme);
                    }
                }
            }
            catch { }
        }

        private static void ApplyTheme(AppTheme theme)
        {
            var mainWindow = Application.Current.MainWindow as MainWindow;
            if (mainWindow == null) return;

            bool useDark = (theme == AppTheme.Dark) || (theme == AppTheme.System && IsSystemDarkMode());

            Brush backgroundBrush;
            Brush panelBackgroundBrush;
            Brush foregroundBrush;
            Brush borderBrush;
            Brush accentBrush;
            Brush hoverBrush;
            Brush selectionBrush;
            Brush windowBorderBrush;

            if (useDark)
            {
                // THÈME SOMBRE
                backgroundBrush = new SolidColorBrush(Color.FromRgb(0x1E, 0x1E, 0x1E));
                panelBackgroundBrush = new SolidColorBrush(Color.FromRgb(0x25, 0x25, 0x26));
                foregroundBrush = new SolidColorBrush(Color.FromRgb(0xD4, 0xD4, 0xD4));
                borderBrush = new SolidColorBrush(Color.FromRgb(0x3F, 0x3F, 0x46));
                accentBrush = new SolidColorBrush(Color.FromRgb(0xC5, 0x86, 0xC0));
                hoverBrush = new SolidColorBrush(Color.FromRgb(0x2A, 0x2D, 0x2E));
                selectionBrush = new SolidColorBrush(Color.FromRgb(0x37, 0x37, 0x3D));
                windowBorderBrush = new SolidColorBrush(Color.FromRgb(0xC5, 0x86, 0xC0)); // Rose pour la bordure active
            }
            else
            {
                // THÈME CLAIR
                backgroundBrush = new SolidColorBrush(Color.FromRgb(0xFE, 0xFE, 0xE2));  // ← MODIFIE ICI
                panelBackgroundBrush = new SolidColorBrush(Color.FromRgb(0xFF, 0xFF, 0xFF));
                foregroundBrush = new SolidColorBrush(Color.FromRgb(0x20, 0x20, 0x20));
                borderBrush = new SolidColorBrush(Color.FromRgb(0xD0, 0xD0, 0xD0));
                accentBrush = new SolidColorBrush(Color.FromRgb(0x7B, 0x2D, 0x8E));
                hoverBrush = new SolidColorBrush(Color.FromRgb(0xE8, 0xE8, 0xE8));
                selectionBrush = new SolidColorBrush(Color.FromRgb(0xE0, 0xE0, 0xE0));
                windowBorderBrush = new SolidColorBrush(Color.FromRgb(0x88, 0x88, 0x88));
            }

            // Mettre à jour les ressources
            Application.Current.Resources["BackgroundBrush"] = backgroundBrush;
            Application.Current.Resources["PanelBackgroundBrush"] = panelBackgroundBrush;
            Application.Current.Resources["ForegroundBrush"] = foregroundBrush;
            Application.Current.Resources["BorderBrush"] = borderBrush;
            Application.Current.Resources["AccentPurpleBrush"] = accentBrush;
            Application.Current.Resources["HoverBrush"] = hoverBrush;
            Application.Current.Resources["SelectionBrush"] = selectionBrush;
            Application.Current.Resources["WindowBorderBrush"] = windowBorderBrush;

            // APPLIQUER DIRECTEMENT AUX CONTROLES
            try
            {
                var border = mainWindow.FindName("WindowBorder") as Border;
                if (border != null)
                {
                    border.Background = backgroundBrush;
                }

                var titleBar = mainWindow.FindName("TitleBar") as Grid;
                if (titleBar != null)
                {
                    titleBar.Background = panelBackgroundBrush;
                }

                var toolbar = mainWindow.FindName("ToolBar") as ToolBar;
                if (toolbar != null)
                {
                    toolbar.Background = panelBackgroundBrush;
                }

                var dockManager = mainWindow.FindName("DockManager") as AvalonDock.DockingManager;
                if (dockManager != null)
                {
                    dockManager.Background = backgroundBrush;
                }

                var explorerTree = mainWindow.FindName("ExplorerTree") as TreeView;
                if (explorerTree != null)
                {
                    explorerTree.Background = backgroundBrush;
                    explorerTree.Foreground = foregroundBrush;
                }

                var outputBox = mainWindow.FindName("OutputBox") as RichTextBox;
                if (outputBox != null)
                {
                    outputBox.Background = backgroundBrush;
                    outputBox.Foreground = foregroundBrush;
                }

                var statusBar = mainWindow.FindName("StatusBar1") as StatusBar;
                if (statusBar != null)
                {
                    statusBar.Background = accentBrush;
                }
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine($"Theme apply error: {ex.Message}");
            }

            mainWindow.InvalidateVisual();
        }

        private static bool IsSystemDarkMode()
        {
            try
            {
                using (var key = Registry.CurrentUser.OpenSubKey(@"Software\Microsoft\Windows\CurrentVersion\Themes\Personalize"))
                {
                    if (key != null)
                    {
                        var value = key.GetValue("AppsUseLightTheme");
                        if (value != null)
                        {
                            return (int)value == 0;
                        }
                    }
                }
            }
            catch { }
            return false;
        }

        public static void Initialize()
        {
            ApplyTheme(_currentTheme);
        }
    }
}