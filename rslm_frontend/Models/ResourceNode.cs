using System.Collections.ObjectModel;
using rslm_frontend.ViewModels;

namespace rslm_frontend.Models
{
    public class ResourceNode : ViewModelBase
    {
        private string _name;
        private string _icon;
        private string _value;

        public ResourceNode()
        {
            Children = new ObservableCollection<ResourceNode>();
        }

        public string Name
        {
            get => _name;
            set => SetProperty(ref _name, value);
        }

        public string Icon
        {
            get => _icon;
            set => SetProperty(ref _icon, value);
        }

        public string Value
        {
            get => _value;
            set => SetProperty(ref _value, value);
        }

        public ObservableCollection<ResourceNode> Children { get; }
    }
}
