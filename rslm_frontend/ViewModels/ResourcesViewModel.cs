using System.Collections.ObjectModel;
using rslm_frontend.Models;
using rslm_frontend.Services;

namespace rslm_frontend.ViewModels
{
    public class ResourcesViewModel : ViewModelBase
    {
        private ResourceNode _selectedNode;

        public ResourcesViewModel()
        {
            Nodes = new ObservableCollection<ResourceNode>(ResourceDataService.CreateResourceTree());
        }

        public ObservableCollection<ResourceNode> Nodes { get; }

        public ResourceNode SelectedNode
        {
            get => _selectedNode;
            set => SetProperty(ref _selectedNode, value);
        }
    }
}
