using System;
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using System.Windows.Input;
using rslm_frontend.Models;
using rslm_frontend.Services;

namespace rslm_frontend.ViewModels
{
    public class ExplorerViewModel : ViewModelBase
    {
        private readonly MainViewModel _mainViewModel;
        private ExplorerNode _selectedNode;

        public ExplorerViewModel(MainViewModel mainViewModel)
        {
            _mainViewModel = mainViewModel;
            Nodes = new ObservableCollection<ExplorerNode>(ExplorerDataService.CreateRootAssemblies());
            NodeExpandedCommand = new RelayCommand(async p => await OnNodeExpandedAsync(p as ExplorerNode));
            NodeSelectedCommand = new RelayCommand(OnNodeSelected);
        }

        public ObservableCollection<ExplorerNode> Nodes { get; }

        public ExplorerNode SelectedNode
        {
            get => _selectedNode;
            set => SetProperty(ref _selectedNode, value);
        }

        public ICommand NodeExpandedCommand { get; }
        public ICommand NodeSelectedCommand { get; }

        private async Task OnNodeExpandedAsync(ExplorerNode node)
        {
            if (node == null || node.IsLoaded || node.IsLoading || !node.HasChildren)
                return;

            node.IsLoading = true;
            _mainViewModel.AppendLog("Assembly Explorer", "Loading " + node.FullName + "...");

            try
            {
                var children = await ExplorerDataService.LoadChildrenAsync(node);
                foreach (var child in children)
                    node.Children.Add(child);

                node.IsLoaded = true;
                _mainViewModel.AppendLog("Assembly Explorer", "Loaded " + children.Count + " items from " + node.Name);
            }
            catch (Exception ex)
            {
                _mainViewModel.AppendLog("Assembly Explorer", "Error: " + ex.Message);
            }
            finally
            {
                node.IsLoading = false;
            }
        }

        private void OnNodeSelected(object parameter)
        {
            var node = parameter as ExplorerNode ?? SelectedNode;
            if (node == null)
                return;

            _mainViewModel.UpdatePropertiesForNode(node);

            if (node.NodeType == ExplorerNodeType.Method ||
                node.NodeType == ExplorerNodeType.Property ||
                node.NodeType == ExplorerNodeType.Class)
            {
                _mainViewModel.OpenDocument(node);
            }
        }
    }
}
