#pragma warning(disable:4996)
#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "ScreenHelper.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <vector>
#include <memory>

#include "../stb_image_write.h"

namespace rslm {
    namespace client {
        namespace helper {

            bool ScreenHelper::GetScreenSize(int& width, int& height) {
#ifdef _WIN32
                width = GetSystemMetrics(SM_CXSCREEN);
                height = GetSystemMetrics(SM_CYSCREEN);
                return true;
#else
                width = 1920;
                height = 1080;
                return true;
#endif
            }

            std::vector<ScreenHelper::ScreenInfo> ScreenHelper::GetAllScreens() {
                std::vector<ScreenHelper::ScreenInfo> screens;
#ifdef _WIN32
                ScreenInfo primary;
                GetScreenSize(primary.width, primary.height);
                primary.x = 0;
                primary.y = 0;
                primary.isPrimary = true;
                screens.push_back(primary);
#endif
                return screens;
            }

            // Callback pour stb_image_write : accumule les bytes dans un vector
            static void stbi_write_callback(void* context, void* data, int size) {
                auto* buf = reinterpret_cast<std::vector<uint8_t>*>(context);
                auto* ptr = reinterpret_cast<uint8_t*>(data);
                buf->insert(buf->end(), ptr, ptr + size);
            }

            std::vector<uint8_t> ScreenHelper::CaptureScreen(int x, int y, int width, int height) {
#ifdef _WIN32
                HDC hdcScreen = GetDC(NULL);
                if (!hdcScreen) return {};

                HDC hdcMem = CreateCompatibleDC(hdcScreen);
                if (!hdcMem) { ReleaseDC(NULL, hdcScreen); return {}; }

                HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
                if (!hBitmap) { DeleteDC(hdcMem); ReleaseDC(NULL, hdcScreen); return {}; }

                HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hBitmap);
                BitBlt(hdcMem, 0, 0, width, height, hdcScreen, x, y, SRCCOPY);
                SelectObject(hdcMem, hOld);

                // Extraire les pixels en RGB
                BITMAPINFOHEADER bi = {};
                bi.biSize = sizeof(BITMAPINFOHEADER);
                bi.biWidth = width;
                bi.biHeight = -height; // top-down
                bi.biPlanes = 1;
                bi.biBitCount = 24;
                bi.biCompression = BI_RGB;

                std::vector<uint8_t> pixels(width * height * 3);
                GetDIBits(hdcMem, hBitmap, 0, height, pixels.data(),
                    (BITMAPINFO*)&bi, DIB_RGB_COLORS);

                // GDI retourne BGR, convertir en RGB
                for (int i = 0; i < width * height; i++) {
                    std::swap(pixels[i * 3 + 0], pixels[i * 3 + 2]);
                }

                DeleteObject(hBitmap);
                DeleteDC(hdcMem);
                ReleaseDC(NULL, hdcScreen);

                return pixels;
#else
                return {};
#endif
            }

            std::vector<uint8_t> ScreenHelper::CaptureScreenJpeg(int x, int y, int width, int height, int quality) {
                auto pixels = CaptureScreen(x, y, width, height);
                if (pixels.empty()) return {};

                std::vector<uint8_t> jpeg;
                jpeg.reserve(width * height / 4); // estimation

                stbi_write_jpg_to_func(stbi_write_callback, &jpeg,
                    width, height,
                    3,           // RGB
                    pixels.data(),
                    quality);    // 0-100

                return jpeg;
            }

            std::vector<uint8_t> ScreenHelper::CaptureScreenBmp(int x, int y, int width, int height) {
                // Non impl�ment� pour l'instant
                return {};
            }

        } // namespace helper
    } // namespace client
} // namespace rslm