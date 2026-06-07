using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using rslm_frontend.Models;

namespace rslm_frontend.Services
{
    public static class ExplorerDataService
    {
        private static List<ExplorerNode> _agents = new List<ExplorerNode>();

        public static IReadOnlyList<ExplorerNode> CreateRootAssemblies()
        {
            return new List<ExplorerNode>
            {
                CreateHeader("CONNECTED AGENTS", "🖥")
            };
        }

        public static void AddAgent(string agentId, string tag)
        {
            var agent = new ExplorerNode
            {
                Name = tag ?? agentId,
                FullName = agentId,
                Icon = "💻",
                NodeType = ExplorerNodeType.Assembly,
                IsLoaded = true,
                DocumentContent = $"Agent: {agentId}\nTag: {tag}"
            };

            _agents.Add(agent);

            // Ajoute les modules à cet agent
            agent.Children.Add(CreateModule(agent, "Remote Shell", "⚡", "Execute commands remotely."));
            agent.Children.Add(CreateModule(agent, "File Manager", "📁", "Browse and transfer files."));
            agent.Children.Add(CreateModule(agent, "Remote Desktop", "🖥", "View and control desktop."));
            agent.Children.Add(CreateModule(agent, "Keylogger", "⌨", "Capture keystrokes."));
            agent.Children.Add(CreateModule(agent, "Task Manager", "📊", "View and manage processes."));
            agent.Children.Add(CreateModule(agent, "System Info", "ℹ", "Hardware and OS info."));
        }

        public static List<ExplorerNode> GetAgents() => _agents;

        private static ExplorerNode CreateHeader(string name, string icon)
        {
            return new ExplorerNode
            {
                Name = name,
                Icon = icon,
                NodeType = ExplorerNodeType.Assembly,
                IsLoaded = true,
                Children = { }
            };
        }

        private static ExplorerNode CreateModule(ExplorerNode agent, string name, string icon, string desc)
        {
            return new ExplorerNode
            {
                Name = name,
                FullName = $"{agent.FullName}/{name}",
                Icon = icon,
                NodeType = ExplorerNodeType.Namespace,
                IsLoaded = true,
                DocumentContent = $"{name}\nAgent: {agent.Name}\n{desc}"
            };
        }

        public static async Task<IReadOnlyList<ExplorerNode>> LoadChildrenAsync(ExplorerNode parent)
        {
            await Task.Delay(10);
            return new List<ExplorerNode>();
        }
    }
}