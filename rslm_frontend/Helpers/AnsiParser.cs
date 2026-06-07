using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Media;

namespace rslm_frontend.Helpers
{
	public static class AnsiParser
	{
		private static readonly Regex AnsiRegex = new Regex(
			@"\x1B\[([0-9;]*)m",
			RegexOptions.Compiled);

		private static readonly Dictionary<int, Brush> ColorMap = new Dictionary<int, Brush>
		{
			{ 30, Brushes.Black },
			{ 31, Brushes.Red },
			{ 32, Brushes.LimeGreen },
			{ 33, Brushes.Yellow },
			{ 34, Brushes.DodgerBlue },
			{ 35, Brushes.Magenta },
			{ 36, Brushes.Cyan },
			{ 37, Brushes.White },
			{ 90, Brushes.Gray },
			{ 91, Brushes.IndianRed },
			{ 92, Brushes.LightGreen },
			{ 93, Brushes.Khaki },
			{ 94, Brushes.LightSkyBlue },
			{ 95, Brushes.Plum },
			{ 96, Brushes.PaleTurquoise },
			{ 97, Brushes.WhiteSmoke }
		};

		public static string StripAnsi(string text)
		{
			if (string.IsNullOrEmpty(text)) return string.Empty;
			return AnsiRegex.Replace(text, string.Empty);
		}

		public static void AppendToRichTextBox(RichTextBox box, string text, Brush defaultBrush)
		{
			if (box == null || string.IsNullOrEmpty(text)) return;

			var paragraph = box.Document.Blocks.LastBlock as Paragraph;
			if (paragraph == null)
			{
				paragraph = new Paragraph { Margin = new Thickness(0) };
				box.Document.Blocks.Add(paragraph);
			}

			var currentBrush = defaultBrush ?? Brushes.LimeGreen;
			int index = 0;

			foreach (Match match in AnsiRegex.Matches(text))
			{
				if (match.Index > index)
				{
					string chunk = text.Substring(index, match.Index - index);
					paragraph.Inlines.Add(new Run(chunk) { Foreground = currentBrush });
				}

				currentBrush = ResolveColor(match.Groups[1].Value, currentBrush, defaultBrush);
				index = match.Index + match.Length;
			}

			if (index < text.Length)
			{
				paragraph.Inlines.Add(new Run(text.Substring(index)) { Foreground = currentBrush });
			}
		}

		private static Brush ResolveColor(string codes, Brush current, Brush defaultBrush)
		{
			if (string.IsNullOrEmpty(codes)) return defaultBrush ?? Brushes.LimeGreen;

			var parts = codes.Split(';');
			foreach (var part in parts)
			{
				if (!int.TryParse(part, out int code)) continue;

				if (code == 0) return defaultBrush ?? Brushes.LimeGreen;
				if (ColorMap.TryGetValue(code, out var brush)) return brush;
			}

			return current;
		}
	}
}
