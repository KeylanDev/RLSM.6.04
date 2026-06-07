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

		public RemoteDesktopView()
		{
			InitializeComponent();
			Loaded += RemoteDesktopView_Loaded;
			Unloaded += RemoteDesktopView_Unloaded;
		}

		private void RemoteDesktopView_Loaded(object sender, RoutedEventArgs e)
		{
			// Focus the image so we can get keyboard events
			DesktopImage.Focus();
		}

		private void RemoteDesktopView_Unloaded(object sender, RoutedEventArgs e)
		{
			ViewModel?.Dispose();
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
			if (ViewModel == null) return;
			var pos = GetMousePosition(e);
			ViewModel.SendMouseInput((int)Math.Round(pos.X), (int)Math.Round(pos.Y), 0); // Move
		}

		private void OnMouseWheel(object sender, MouseWheelEventArgs e)
		{
			if (ViewModel == null) return;
			var pos = GetMousePosition(e);
			ViewModel.SendMouseInput((int)Math.Round(pos.X), (int)Math.Round(pos.Y), 7, e.Delta); // Wheel
		}

		private void OnMouseEnter(object sender, MouseEventArgs e)
		{
			DesktopImage.Focus();
		}

		private void OnKeyDown(object sender, KeyEventArgs e)
		{
			if (ViewModel == null) return;
			ViewModel.SendKeyboardInput((int)KeyInterop.VirtualKeyFromKey(e.Key), true);
		}

		private void OnKeyUp(object sender, KeyEventArgs e)
		{
			if (ViewModel == null) return;
			ViewModel.SendKeyboardInput((int)KeyInterop.VirtualKeyFromKey(e.Key), false);
		}
	}
}
