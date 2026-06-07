using System;
using rslm_frontend.ViewModels;

namespace rslm_frontend.Models
{
    public class LogEntry : ViewModelBase
    {
        private DateTime _timestamp;
        private string _message;
        private string _source;

        public DateTime Timestamp
        {
            get => _timestamp;
            set
            {
                if (SetProperty(ref _timestamp, value))
                    OnPropertyChanged(nameof(FormattedLine));
            }
        }

        public string Message
        {
            get => _message;
            set
            {
                if (SetProperty(ref _message, value))
                    OnPropertyChanged(nameof(FormattedLine));
            }
        }

        public string Source
        {
            get => _source;
            set
            {
                if (SetProperty(ref _source, value))
                    OnPropertyChanged(nameof(FormattedLine));
            }
        }

        public string FormattedLine => $"[{Timestamp:HH:mm:ss.fff}] {Source}: {Message}";
    }
}
