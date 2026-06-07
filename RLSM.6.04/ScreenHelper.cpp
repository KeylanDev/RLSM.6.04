#include "ScreenHelper.h"
#include "Windows.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#endif

#include <memory>
#include <vector>

namespace rslm {
    namespace client {
        namespace helper {

#ifdef _WIN32
            namespace {
                int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
                    UINT num = 0;
                    UINT size = 0;
                    Gdiplus::GetImageEncodersSize(&num, &size);
                    if (size == 0) return -1;

                    auto pImageCodecInfo = std::make_unique<Gdiplus::ImageCodecInfo[]>(num);
                    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo.get());

                    for (UINT i = 0; i < num; ++i) {
                        if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0) {
                            *pClsid = pImageCodecInfo[i].Clsid;
                            return static_cast<int>(i);
                        }
                    }
                    return -1;
                }
            }
#endif

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
                DISPLAY_DEVICEA device = {};
                device.cb = sizeof(device);
                int index = 0;

                while (EnumDisplayDevicesA(nullptr, index, &device, 0)) {
                    if (device.StateFlags & DISPLAY_DEVICE_ACTIVE) {
                        DEVMODEA mode = {};
                        mode.dmSize = sizeof(mode);

                        if (EnumDisplaySettingsA(device.DeviceName, ENUM_CURRENT_SETTINGS, &mode)) {
                            ScreenHelper::ScreenInfo info;
                            info.x = mode.dmPosition.x;
                            info.y = mode.dmPosition.y;
                            info.width = mode.dmPelsWidth;
                            info.height = mode.dmPelsHeight;
                            info.isPrimary = (device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) != 0;
                            screens.push_back(info);
                        }
                    }
                    index++;
                }
#endif

                if (screens.empty()) {
                    ScreenHelper::ScreenInfo primary;
                    GetScreenSize(primary.width, primary.height);
                    primary.isPrimary = true;
                    screens.push_back(primary);
                }

                return screens;
            }

            std::vector<uint8_t> ScreenHelper::CaptureScreen(int x, int y, int width, int height) {
                std::vector<uint8_t> pixels(static_cast<size_t>(width) * height * 3);

#ifdef _WIN32
                HDC hdcScreen = GetDC(nullptr);
                if (!hdcScreen) return pixels;

                HDC hdcMem = CreateCompatibleDC(hdcScreen);
                HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
                HBITMAP hOld = static_cast<HBITMAP>(SelectObject(hdcMem, hBitmap));

                BitBlt(hdcMem, 0, 0, width, height, hdcScreen, x, y, SRCCOPY);

                BITMAPINFO bmi = {};
                bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                bmi.bmiHeader.biWidth = width;
                bmi.bmiHeader.biHeight = -height;
                bmi.bmiHeader.biPlanes = 1;
                bmi.bmiHeader.biBitCount = 24;
                bmi.bmiHeader.biCompression = BI_RGB;

                GetDIBits(hdcMem, hBitmap, 0, height, pixels.data(), &bmi, DIB_RGB_COLORS);

                SelectObject(hdcMem, hOld);
                DeleteObject(hBitmap);
                DeleteDC(hdcMem);
                ReleaseDC(nullptr, hdcScreen);
#endif

                return pixels;
            }

            std::vector<uint8_t> ScreenHelper::CaptureScreenJpeg(int x, int y, int width, int height, int quality) {
                std::vector<uint8_t> jpeg;

#ifdef _WIN32
                auto bgr = CaptureScreen(x, y, width, height);
                if (bgr.empty()) return jpeg;

                Gdiplus::Bitmap bitmap(width, height, width * 3, PixelFormat24bppRGB, bgr.data());
                if (bitmap.GetLastStatus() != Gdiplus::Ok) return jpeg;

                CLSID clsid;
                if (GetEncoderClsid(L"image/jpeg", &clsid) < 0) return jpeg;

                Gdiplus::EncoderParameters params;
                params.Count = 1;
                params.Parameter[0].Guid = Gdiplus::EncoderQuality;
                params.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
                params.Parameter[0].NumberOfValues = 1;
                ULONG q = static_cast<ULONG>(quality);
                params.Parameter[0].Value = &q;

                IStream* stream = nullptr;
                if (CreateStreamOnHGlobal(nullptr, TRUE, &stream) != S_OK)
                    return jpeg;

                if (bitmap.Save(stream, &clsid, &params) == Gdiplus::Ok) {
                    STATSTG stat = {};
                    if (stream->Stat(&stat, STATFLAG_NONAME) == S_OK) {
                        ULONG size = static_cast<ULONG>(stat.cbSize.QuadPart);
                        jpeg.resize(size);
                        LARGE_INTEGER li = {};
                        stream->Seek(li, STREAM_SEEK_SET, nullptr);
                        ULONG read = 0;
                        stream->Read(jpeg.data(), size, &read);
                        jpeg.resize(read);
                    }
                }

                stream->Release();
#endif

                return jpeg;
            }

            std::vector<uint8_t> ScreenHelper::CaptureScreenBmp(int x, int y, int width, int height) {
                std::vector<uint8_t> bmp;

#ifdef _WIN32
                auto pixels = CaptureScreen(x, y, width, height);
                if (pixels.empty()) return bmp;

                int pixelArraySize = width * height * 3;
                int fileSize = 54 + pixelArraySize;
                bmp.resize(fileSize);

                bmp[0] = 'B';
                bmp[1] = 'M';
                bmp[2] = (fileSize) & 0xFF;
                bmp[3] = (fileSize >> 8) & 0xFF;
                bmp[4] = (fileSize >> 16) & 0xFF;
                bmp[5] = (fileSize >> 24) & 0xFF;
                bmp[6] = 0;
                bmp[7] = 0;
                bmp[8] = 0;
                bmp[9] = 0;
                bmp[10] = 54;
                bmp[11] = 0;
                bmp[12] = 0;
                bmp[13] = 0;

                bmp[14] = 40;
                bmp[15] = 0;
                bmp[16] = 0;
                bmp[17] = 0;
                bmp[18] = (width) & 0xFF;
                bmp[19] = (width >> 8) & 0xFF;
                bmp[20] = (width >> 16) & 0xFF;
                bmp[21] = (width >> 24) & 0xFF;
                bmp[22] = (-height) & 0xFF;
                bmp[23] = (-height >> 8) & 0xFF;
                bmp[24] = (-height >> 16) & 0xFF;
                bmp[25] = (-height >> 24) & 0xFF;
                bmp[26] = 1;
                bmp[27] = 0;
                bmp[28] = 24;
                bmp[29] = 0;
                bmp[30] = 0;
                bmp[31] = 0;
                bmp[32] = 0;
                bmp[33] = 0;
                bmp[34] = (pixelArraySize) & 0xFF;
                bmp[35] = (pixelArraySize >> 8) & 0xFF;
                bmp[36] = (pixelArraySize >> 16) & 0xFF;
                bmp[37] = (pixelArraySize >> 24) & 0xFF;
                bmp[38] = 0x13;
                bmp[39] = 0x0B;
                bmp[40] = 0;
                bmp[41] = 0;
                bmp[42] = 0x13;
                bmp[43] = 0x0B;
                bmp[44] = 0;
                bmp[45] = 0;
                bmp[46] = 0;
                bmp[47] = 0;
                bmp[48] = 0;
                bmp[49] = 0;
                bmp[50] = 0;
                bmp[51] = 0;
                bmp[52] = 0;
                bmp[53] = 0;

                std::memcpy(bmp.data() + 54, pixels.data(), pixels.size());
#endif

                return bmp;
            }

        } // namespace helper
    } // namespace client
} // namespace rslm
