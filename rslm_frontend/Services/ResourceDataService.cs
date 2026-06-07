using System.Collections.Generic;
using rslm_frontend.Models;

namespace rslm_frontend.Services
{
    public static class ResourceDataService
    {
        public static IReadOnlyList<ResourceNode> CreateResourceTree()
        {
            var root = new ResourceNode { Name = "Resources", Icon = "\uD83D\uDCC1" };

            var strings = new ResourceNode { Name = "Strings", Icon = "\uD83D\uDCC1" };
            strings.Children.Add(new ResourceNode { Name = "AppTitle", Icon = "\uD83D\uDD27", Value = "RLSM Decompiler" });
            strings.Children.Add(new ResourceNode { Name = "AssemblyExplorer", Icon = "\uD83D\uDD27", Value = "Assembly Explorer" });
            strings.Children.Add(new ResourceNode { Name = "OutputWindow", Icon = "\uD83D\uDD27", Value = "Output" });

            var icons = new ResourceNode { Name = "Icons", Icon = "\uD83D\uDCC1" };
            icons.Children.Add(new ResourceNode { Name = "Run", Icon = "\u2699", Value = "Run_16x" });
            icons.Children.Add(new ResourceNode { Name = "Stop", Icon = "\u2699", Value = "Stop_16x" });
            icons.Children.Add(new ResourceNode { Name = "Open", Icon = "\u2699", Value = "Open_16x" });
            icons.Children.Add(new ResourceNode { Name = "Settings", Icon = "\u2699", Value = "Settings_16x" });

            var images = new ResourceNode { Name = "Images", Icon = "\uD83D\uDCC1" };
            images.Children.Add(new ResourceNode { Name = "Logo.png", Icon = "\uD83D\uDCC4", Value = "pack://application:,,,/Images/Logo.png" });
            images.Children.Add(new ResourceNode { Name = "Splash.bmp", Icon = "\uD83D\uDCC4", Value = "pack://application:,,,/Images/Splash.bmp" });

            root.Children.Add(strings);
            root.Children.Add(icons);
            root.Children.Add(images);

            return new List<ResourceNode> { root };
        }
    }
}
