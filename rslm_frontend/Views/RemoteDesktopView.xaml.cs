using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media.Imaging;
using rslm_frontend.ViewModels;

namespace rslm_frontend.Views
{
    public partial class RemoteDesktopView : UserControl
    {
        private RemoteDesktopViewModel ViewModel => DataContext as RemoteDesktopViewModel;
        private bool _leftButtonDown;
        private bool _rightButtonDown;
        private bool _middleButtonDown;

        // NOUVEAU : Gestion de la capture souris
        private bool _isMouseCaptured = false;
        private Cursor _previousCursor;

        public RemoteDesktopView()
        {
            InitializeComponent();
            Loaded += RemoteDesktopView_Loaded;
            Unloaded += RemoteDesktopView_Unloaded;

            // NOUVEAU : Capturer la touche Échap au niveau de la fenêtre parente
            this.PreviewKeyDown += OnPreviewKeyDown;
        }

        private void RemoteDesktopView_Loaded(object sender, RoutedEventArgs e)
        {
            DesktopImage.Focus();

            if (ViewModel != null)
            {
                ViewModel.FrameReceived += (bmp) =>
                {
                    Dispatcher.Invoke(() =>
                    {
                        DesktopImage.Source = bmp;
                    });
                };
            }
        }

        private void RemoteDesktopView_Unloaded(object sender, RoutedEventArgs e)
        {
            ViewModel?.Dispose();
            ReleaseMouseCapture(); // Nettoyer
        }

        // NOUVEAU : Gestion de la touche Échap pour relâcher la souris
        private void OnPreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Escape && _isMouseCaptured)
            {
                ReleaseMouseCapture();
                ShowTemporaryMessage("Contrôle relâché - Cliquez sur l'écran pour reprendre");
                e.Handled = true;
            }
        }

        // NOUVEAU : Capturer la souris
        private void CaptureMouseForRemote()
        {
            if (_isMouseCaptured) return;

            _isMouseCaptured = true;
            _previousCursor = DesktopImage.Cursor;
            DesktopImage.Cursor = Cursors.None; // Cache le curseur local
            DesktopImage.CaptureMouse();

            ShowTemporaryMessage("Contrôle activé - Appuyez sur Échap pour relâcher");
        }

        // NOUVEAU : Relâcher la souris
        private void ReleaseMouseCapture()
        {
            if (!_isMouseCaptured) return;

            _isMouseCaptured = false;
            DesktopImage.Cursor = _previousCursor ?? Cursors.Arrow;
            DesktopImage.ReleaseMouseCapture();
        }

        // NOUVEAU : Afficher un message temporaire
        private void ShowTemporaryMessage(string message)
        {
            // Tu peux utiliser ton système de notification existant
            // Ou un simple ToolTip temporaire
            var tooltip = new ToolTip { Content = message, IsOpen = true };
            tooltip.Closed += (s, e) => tooltip.IsOpen = false;
            DesktopImage.ToolTip = tooltip;
            tooltip.IsOpen = true;

            // Fermer après 2 secondes
            var timer = new System.Timers.Timer(2000);
            timer.Elapsed += (s, e) => Dispatcher.Invoke(() => tooltip.IsOpen = false);
            timer.AutoReset = false;
            timer.Start();
        }

        private Point GetMousePosition(MouseEventArgs e)
        {
            var pos = e.GetPosition(DesktopImage);
            if (DesktopImage.Source is BitmapSource bmp)
            {
                var scaleX = bmp.PixelWidth / DesktopImage.ActualWidth;
                var scaleY = bmp.PixelHeight / DesktopImage.ActualHeight;
                return new Point(pos.X * scaleX, pos.Y * scaleY);
            }
            return new Point(pos.X, pos.Y);
        }

        private void OnMouseDown(object sender, MouseButtonEventArgs e)
        {
            if (ViewModel == null) return;

            DesktopImage.Focus();

            // NOUVEAU : Capturer la souris au premier clic si pas déjà capturée
            if (!_isMouseCaptured)
            {
                CaptureMouseForRemote();
            }

            var pos = GetMousePosition(e);
            int action = 0;
            if (e.ChangedButton == MouseButton.Left)
            {
                _leftButtonDown = true;
                action = 1; // Left down
            }
            else if (e.ChangedButton == MouseButton.Right)
            {
                _rightButtonDown = true;
                action = 3; // Right down
            }
            else if (e.ChangedButton == MouseButton.Middle)
            {
                _middleButtonDown = true;
                action = 5; // Middle down
            }

            ViewModel.SendMouseInput((int)Math.Round(pos.X), (int)Math.Round(pos.Y), action);
        }

        private void OnMouseUp(object sender, MouseButtonEventArgs e)
        {
            if (ViewModel == null) return;

            var pos = GetMousePosition(e);
            int action = 0;
            if (e.ChangedButton == MouseButton.Left)
            {
                _leftButtonDown = false;
                action = 2; // Left up
            }
            else if (e.ChangedButton == MouseButton.Right)
            {
                _rightButtonDown = false;
                action = 4; // Right up
            }
            else if (e.ChangedButton == MouseButton.Middle)
            {
                _middleButtonDown = false;
                action = 6; // Middle up
            }

            ViewModel.SendMouseInput((int)Math.Round(pos.X), (int)Math.Round(pos.Y), action);
        }

        private void OnMouseMove(object sender, MouseEventArgs e)
        {
            // MODIFIÉ : Envoyer les mouvements SEULEMENT si la souris est capturée
            if (ViewModel == null) return;

            // Si la souris n'est pas capturée, on ne transmet PAS les mouvements
            // (la souris locale bouge normalement)
            if (!_isMouseCaptured) return;

            var pos = GetMousePosition(e);
            ViewModel.SendMouseInput((int)Math.Round(pos.X), (int)Math.Round(pos.Y), 0); // Move
        }

        private void OnMouseWheel(object sender, MouseWheelEventArgs e)
        {
            if (ViewModel == null) return;

            // La molette fonctionne même sans capture (pratique)
            var pos = GetMousePosition(e);
            ViewModel.SendMouseInput((int)Math.Round(pos.X), (int)Math.Round(pos.Y), 7, e.Delta); // Wheel
        }

        private void OnMouseEnter(object sender, MouseEventArgs e)
        {
            DesktopImage.Focus();
        }

        // NOUVEAU : Quand la souris quitte l'image, on peut optionnellement relâcher
        private void OnMouseLeave(object sender, MouseEventArgs e)
        {
            // Optionnel : Relâcher automatiquement quand on quitte l'image
            // Décommente si tu veux ce comportement
            // ReleaseMouseCapture();
        }

        private void OnKeyDown(object sender, KeyEventArgs e)
        {
            if (ViewModel == null) return;

            // NOUVEAU : Ne pas envoyer la touche Échap (elle sert à relâcher)
            if (e.Key == Key.Escape) return;

            ViewModel.SendKeyboardInput((int)KeyInterop.VirtualKeyFromKey(e.Key), true);
        }

        private void OnKeyUp(object sender, KeyEventArgs e)
        {
            if (ViewModel == null) return;

            // NOUVEAU : Ne pas envoyer la touche Échap
            if (e.Key == Key.Escape) return;

            ViewModel.SendKeyboardInput((int)KeyInterop.VirtualKeyFromKey(e.Key), false);
        }
    }
}