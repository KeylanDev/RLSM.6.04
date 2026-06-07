using System.Collections.ObjectModel;
using rslm_frontend.ViewModels;

namespace rslm_frontend.Models
{
    public class ExplorerNode : ViewModelBase
    {
        private string _name;
        private string _icon;
        private bool _isExpanded;
        private bool _isLoaded;
        private bool _isLoading;
        private ExplorerNodeType _nodeType;
        private string _documentContent;
        private string _fullName;

        public ExplorerNode()
        {
            Children = new ObservableCollection<ExplorerNode>();
        }

        public string Name
        {
            get => _name;
            set => SetProperty(ref _name, value);
        }

        public string FullName
        {
            get => _fullName;
            set => SetProperty(ref _fullName, value);
        }

        public string Icon
        {
            get => _icon;
            set => SetProperty(ref _icon, value);
        }

        public ExplorerNodeType NodeType
        {
            get => _nodeType;
            set => SetProperty(ref _nodeType, value);
        }

        public bool IsExpanded
        {
            get => _isExpanded;
            set => SetProperty(ref _isExpanded, value);
        }

        public bool IsLoaded
        {
            get => _isLoaded;
            set => SetProperty(ref _isLoaded, value);
        }

        public bool IsLoading
        {
            get => _isLoading;
            set => SetProperty(ref _isLoading, value);
        }

        public string DocumentContent
        {
            get => _documentContent;
            set => SetProperty(ref _documentContent, value);
        }

        public bool HasChildren => NodeType != ExplorerNodeType.Method && NodeType != ExplorerNodeType.Property;

        public ObservableCollection<ExplorerNode> Children { get; }
    }
}
