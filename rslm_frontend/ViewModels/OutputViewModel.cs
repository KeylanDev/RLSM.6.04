using System;
using System.Collections.ObjectModel;
using System.Linq;
using rslm_frontend.Models;

namespace rslm_frontend.ViewModels
{
    public class OutputViewModel : ViewModelBase
    {
        private string _selectedSource;
        private string _fullText;

        public OutputViewModel()
        {
            Logs = new ObservableCollection<LogEntry>();
            Sources = new ObservableCollection<string> { "Build", "Debug", "Assembly Explorer", "Decompiler" };
            SelectedSource = "Debug";
            Append("Debug", "Output window initialized.");
            Append("Assembly Explorer", "Ready.");
        }

        public ObservableCollection<LogEntry> Logs { get; }
        public ObservableCollection<string> Sources { get; }

        public string SelectedSource
        {
            get => _selectedSource;
            set
            {
                if (SetProperty(ref _selectedSource, value))
                    RefreshFullText();
            }
        }

        public string FullText
        {
            get => _fullText;
            private set => SetProperty(ref _fullText, value);
        }

        public void Append(string source, string message)
        {
            var entry = new LogEntry
            {
                Timestamp = DateTime.Now,
                Source = source,
                Message = message
            };

            Logs.Add(entry);
            RefreshFullText();
        }

        public void Clear()
        {
            Logs.Clear();
            RefreshFullText();
        }

        private void RefreshFullText()
        {
            var lines = Logs
                .Where(l => string.IsNullOrEmpty(SelectedSource) || l.Source == SelectedSource)
                .Select(l => l.FormattedLine);

            FullText = string.Join(Environment.NewLine, lines);
            if (!string.IsNullOrEmpty(FullText))
                FullText += Environment.NewLine;
        }
    }
}
