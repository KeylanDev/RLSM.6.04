using System.Collections.ObjectModel;
using rslm_frontend.Models;

namespace rslm_frontend.ViewModels
{
    public class PropertiesViewModel : ViewModelBase
    {
        private PropertyItem _selectedProperty;

        public PropertiesViewModel()
        {
            Properties = new ObservableCollection<PropertyItem>();
        }

        public ObservableCollection<PropertyItem> Properties { get; }

        public PropertyItem SelectedProperty
        {
            get => _selectedProperty;
            set => SetProperty(ref _selectedProperty, value);
        }

        public void SetPropertiesForNode(ExplorerNode node)
        {
            Properties.Clear();

            if (node == null)
                return;

            Properties.Add(new PropertyItem { Category = "Identity", Name = "Name", Value = node.Name });
            Properties.Add(new PropertyItem { Category = "Identity", Name = "Full Name", Value = node.FullName });
            Properties.Add(new PropertyItem { Category = "Metadata", Name = "Type", Value = node.NodeType.ToString() });
            Properties.Add(new PropertyItem { Category = "Metadata", Name = "Icon", Value = node.Icon });
            Properties.Add(new PropertyItem { Category = "State", Name = "Is Loaded", Value = node.IsLoaded.ToString() });
            Properties.Add(new PropertyItem { Category = "State", Name = "Has Children", Value = node.HasChildren.ToString() });
            Properties.Add(new PropertyItem { Category = "State", Name = "Child Count", Value = node.Children.Count.ToString() });
        }
    }
}
