using rslm_frontend.ViewModels;

namespace rslm_frontend.Models
{
    public class PropertyItem : ViewModelBase
    {
        private string _name;
        private string _value;
        private string _category;

        public string Name
        {
            get => _name;
            set => SetProperty(ref _name, value);
        }

        public string Value
        {
            get => _value;
            set => SetProperty(ref _value, value);
        }

        public string Category
        {
            get => _category;
            set => SetProperty(ref _category, value);
        }
    }
}
