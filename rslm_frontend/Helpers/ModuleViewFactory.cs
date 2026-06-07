using System.Windows;
using System.Windows.Controls;
using ICSharpCode.AvalonEdit;
using rslm_frontend.Helpers;
using rslm_frontend.Models;
using rslm_frontend.ViewModels;
using rslm_frontend.Views;

namespace rslm_frontend.Helpers
{
	public static class ModuleViewFactory
	{
		public static UIElement CreateContent(DocumentModel document)
		{
			if (document == null)
				return new TextBlock { Text = "Empty document" };

			if (document.ModuleKey == "Remote Desktop" && document.ModuleViewModel is RemoteDesktopViewModel rdVm)
				return new RemoteDesktopView { DataContext = rdVm };

			if (document.ModuleKey == "Remote Shell" && document.ModuleViewModel is RemoteShellViewModel shellVm)
				return new RemoteShellView { DataContext = shellVm };

			return CreateTextEditor(document);
		}

		public static TextEditor CreateTextEditor(DocumentModel document)
		{
			return CodeEditorFactory.CreateReadOnlyEditor(document);
		}
	}
}
