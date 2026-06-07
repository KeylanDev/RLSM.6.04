using rslm_frontend.ViewModels;

namespace rslm_frontend.Models
{
    public class DocumentModel : ViewModelBase
    {
        private string _title;
        private string _content;
        private string _contentId;
        private bool _isSelected;
        private string _moduleKey;
        private object _moduleViewModel;

        public string Title
        {
            get => _title;
            set => SetProperty(ref _title, value);
        }

        public string Content
        {
            get => _content;
            set => SetProperty(ref _content, value);
        }

        public string ContentId
        {
            get => _contentId;
            set => SetProperty(ref _contentId, value);
        }

        public bool IsSelected
        {
            get => _isSelected;
            set => SetProperty(ref _isSelected, value);
        }

        public string ModuleKey
        {
            get => _moduleKey;
            set => SetProperty(ref _moduleKey, value);
        }

        public object ModuleViewModel
        {
            get => _moduleViewModel;
            set => SetProperty(ref _moduleViewModel, value);
        }
    }
}
