using System.Windows;
using System.Windows.Media;
using ICSharpCode.AvalonEdit;
using ICSharpCode.AvalonEdit.Highlighting;
using rslm_frontend.Models;

namespace rslm_frontend.Helpers
{
    public static class CodeEditorFactory
    {
        public static TextEditor CreateReadOnlyEditor(DocumentModel document)
        {
            var editor = new TextEditor
            {
                Document = { Text = document?.Content ?? string.Empty },
                IsReadOnly = true,
                ShowLineNumbers = true,
                FontFamily = new FontFamily("Consolas"),
                FontSize = 13,
                Background = (Brush)Application.Current.FindResource("VsBackgroundBrush"),
                Foreground = (Brush)Application.Current.FindResource("VsTextBrush"),
                LineNumbersForeground = (Brush)Application.Current.FindResource("VsSecondaryTextBrush"),
                HorizontalScrollBarVisibility = System.Windows.Controls.ScrollBarVisibility.Auto,
                VerticalScrollBarVisibility = System.Windows.Controls.ScrollBarVisibility.Auto,
                SyntaxHighlighting = HighlightingManager.Instance.GetDefinition("C#"),
                Padding = new Thickness(4, 0, 0, 0)
            };

            editor.TextArea.SelectionBrush = (Brush)Application.Current.FindResource("VsSelectionBrush");
            editor.TextArea.SelectionForeground = (Brush)Application.Current.FindResource("VsTextBrush");
            editor.TextArea.Caret.CaretBrush = (Brush)Application.Current.FindResource("VsAccentBrush");

            return editor;
        }

        public static TextEditor CreateOutputEditor(string text)
        {
            var editor = new TextEditor
            {
                Document = { Text = text ?? string.Empty },
                IsReadOnly = true,
                ShowLineNumbers = false,
                FontFamily = new FontFamily("Consolas"),
                FontSize = 12,
                Background = (Brush)Application.Current.FindResource("VsPanelBrush"),
                Foreground = (Brush)Application.Current.FindResource("VsTextBrush"),
                HorizontalScrollBarVisibility = System.Windows.Controls.ScrollBarVisibility.Auto,
                VerticalScrollBarVisibility = System.Windows.Controls.ScrollBarVisibility.Auto,
                BorderThickness = new Thickness(0),
                Padding = new Thickness(4, 2, 4, 2)
            };

            return editor;
        }
    }
}
