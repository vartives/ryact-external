

#include "../cache/actorloop.h"
#include "../../imgui/imgui_impl_win32.h"
#include "../../imgui/imgui_impl_dx11.h"
#include <d3d11.h>
#include <vector>
#include <string>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <tchar.h>
#include "../../imgui/imgui_internal.h"
#include "menu/drawing.h"
#include "menu/menu.h"
//#include "../../imgui/burbank.hpp"
#include <iostream>
#include "../../imgui/examples/example_win32_directx11/main_settings.h"
#include "../../imgui/examples/example_win32_directx11/logo.h"
#include "../../imgui/examples/example_win32_directx11/Font.h"
#include "../../imgui/examples/example_win32_directx11/imgui_settings.h"

#include <D3D11.h>
#include <D3DX11core.h>
#include <D3DX11.h>
#include <D3DX11tex.h>
#include <thread>
#include <chrono>
#include <random>
using namespace std::chrono;
using namespace std;

#include <filesystem>

#define HJWND_PROGRAM _(L"mmc.exe")
#define MAX_CLASSNAME 255
#define MAX_WNDNAME 255

#define ALPHA ( ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoBorder )
ImFont* icons;
ID3D11ShaderResourceView* Zlogo = nullptr;

#pragma comment(lib, "D3DX11.lib")



inline  auto ToVec4(float r, float g, float b, float a = 255.f) -> ImVec4
{
    return ImVec4(r, g, b, a);
}
DWORD picker_flags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_InputRGB;



float WIDTH = 700; // Loader Size X
float HEIGHT = 450; // Loader Size Y
RECT rc;

ImVec2 menu_size = { WIDTH, HEIGHT };


namespace image
{
    ID3D11ShaderResourceView* Recoil = nullptr;
    ID3D11ShaderResourceView* FishBot = nullptr;
    ID3D11ShaderResourceView* Misc = nullptr;
    ID3D11ShaderResourceView* KeyBinds = nullptr;
    ID3D11ShaderResourceView* Settings = nullptr;
    ID3D11ShaderResourceView* Configs = nullptr;
    ID3D11ShaderResourceView* exit = nullptr;
    ID3D11ShaderResourceView* Key = nullptr;
    ID3D11ShaderResourceView* icon_color = nullptr;
    ID3D11ShaderResourceView* settings_checkbox = nullptr;
    ID3D11ShaderResourceView* arrows = nullptr;
    ID3D11ShaderResourceView* Audio = nullptr;
    ID3D11ShaderResourceView* texture = nullptr;

}

using namespace std;
struct WindowsFinderParams {
    DWORD pidOwner = NULL;
    std::wstring wndClassName = _(L"");
    std::wstring wndName = _(L"");
    RECT pos = { 0, 0, 0, 0 };
    POINT res = { 0, 0 };
    float percentAllScreens = 0.0f;
    float percentMainScreen = 0.0f;
    DWORD style = NULL;
    DWORD styleEx = NULL;
    bool satisfyAllCriteria = false;
    std::vector<HWND> hwnds;
};
HWND HiJackNotepadWindow();
std::vector<DWORD> GetPIDs(std::wstring targetProcessName);
std::vector<HWND> WindowsFinder(WindowsFinderParams params);
BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam);
void TerminateNotepad();
HWND HiJackNotepadWindow()
{
    HWND hwndHiHjacked = NULL;
    std::vector<DWORD> existingNotepads = GetPIDs(HJWND_PROGRAM);
    if (!existingNotepads.empty()) {
        for (int i(0); i < existingNotepads.size(); ++i) {
            HANDLE hOldProcess = OpenProcess(PROCESS_TERMINATE, FALSE, existingNotepads[i]);
            TerminateProcess(hOldProcess, 0);
            CloseHandle(hOldProcess);
        }
    }
    system(_("start mmc.exe"));
    std::vector<DWORD> notepads = GetPIDs(HJWND_PROGRAM);
    if (notepads.empty() || notepads.size() > 1)
    {
        return hwndHiHjacked;
    }
    WindowsFinderParams params;
    params.pidOwner = notepads[0];
    params.style = WS_VISIBLE;
    params.satisfyAllCriteria = true;
    std::vector<HWND> hwnds;
    int attempt = 0;
    while (hwndHiHjacked == NULL || attempt > 50000) {
        Sleep(7500);
        hwnds = WindowsFinder(params);
        if (hwnds.size() > 1)
        {
            return hwndHiHjacked;
        }
        hwndHiHjacked = hwnds[0];
        ++attempt;
    }
    if (!hwndHiHjacked)
    {
        return hwndHiHjacked;
    }
    SetMenu(hwndHiHjacked, NULL);
    return hwndHiHjacked;
}
void TerminateNotepad()
{
    std::vector<DWORD> existingNotepads = GetPIDs(HJWND_PROGRAM);
    if (!existingNotepads.empty()) {
        for (int i(0); i < existingNotepads.size(); ++i) {
            HANDLE hOldProcess = OpenProcess(PROCESS_TERMINATE, FALSE, existingNotepads[i]);
            TerminateProcess(hOldProcess, 0);
            CloseHandle(hOldProcess);
        }
    }
}
std::vector<DWORD> GetPIDs(std::wstring targetProcessName) {
    std::vector<DWORD> pids;
    if (targetProcessName == L"")
        return pids;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32W entry;
    entry.dwSize = sizeof entry;
    if (!Process32FirstW(snap, &entry)) {
        CloseHandle(snap);
        return pids;
    }
    do {
        if (std::wstring(entry.szExeFile) == targetProcessName) {
            pids.emplace_back(entry.th32ProcessID);
        }
    } while (Process32NextW(snap, &entry));
    CloseHandle(snap);
    return pids;
}
std::vector<HWND> WindowsFinder(WindowsFinderParams params) {
    EnumWindows(EnumWindowsCallback, (LPARAM)&params);
    return params.hwnds;
}
BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam) {
    WindowsFinderParams& params = *(WindowsFinderParams*)lParam;

    unsigned char satisfiedCriteria = 0, unSatisfiedCriteria = 0;

    // If looking for windows of a specific PID
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (params.pidOwner != NULL)
        if (params.pidOwner == pid)
            ++satisfiedCriteria; // Doesn't belong to the process targeted
        else
            ++unSatisfiedCriteria;

    // If looking for windows of a specific class
    wchar_t className[MAX_CLASSNAME] = L"";
    GetClassName(hwnd, (LPWSTR)className, MAX_CLASSNAME);
    std::wstring classNameWstr = className;
    if (params.wndClassName != L"")
        if (params.wndClassName == classNameWstr)
            ++satisfiedCriteria; // Not the class targeted
        else
            ++unSatisfiedCriteria;

    // If looking for windows with a specific name
    wchar_t windowName[MAX_WNDNAME] = L"";
    GetWindowText(hwnd, (LPWSTR)windowName, MAX_CLASSNAME);
    std::wstring windowNameWstr = windowName;
    if (params.wndName != L"")
        if (params.wndName == windowNameWstr)
            ++satisfiedCriteria; // Not the class targeted
        else
            ++unSatisfiedCriteria;

    // If looking for window at a specific position
    RECT pos;
    GetWindowRect(hwnd, &pos);
    if (params.pos.left || params.pos.top || params.pos.right || params.pos.bottom)
        if (params.pos.left == pos.left && params.pos.top == pos.top && params.pos.right == pos.right && params.pos.bottom == pos.bottom)
            ++satisfiedCriteria;
        else
            ++unSatisfiedCriteria;

    // If looking for window of a specific size
    POINT res = { pos.right - pos.left, pos.bottom - pos.top };
    if (params.res.x || params.res.y)
        if (res.x == params.res.x && res.y == params.res.y)
            ++satisfiedCriteria;
        else
            ++unSatisfiedCriteria;

    // If looking for windows taking more than a specific percentage of all the screens
    LONG ratioAllScreensX = res.x / GetSystemMetrics(SM_CXSCREEN);
    LONG ratioAllScreensY = res.y / GetSystemMetrics(SM_CYSCREEN);
    float percentAllScreens = (float)ratioAllScreensX * (float)ratioAllScreensY * 450;
    if (params.percentAllScreens != 0.0f)
        if (percentAllScreens >= params.percentAllScreens)
            ++satisfiedCriteria;
        else
            ++unSatisfiedCriteria;

    // If looking for windows taking more than a specific percentage or the main screen
    RECT desktopRect;
    GetWindowRect(GetDesktopWindow(), &desktopRect);
    POINT desktopRes = { desktopRect.right - desktopRect.left, desktopRect.bottom - desktopRect.top };
    LONG ratioMainScreenX = res.x / desktopRes.x;
    LONG ratioMainScreenY = res.y / desktopRes.y;
    float percentMainScreen = (float)ratioMainScreenX * (float)ratioMainScreenY * 450;
    if (params.percentMainScreen != 0.0f)
        if (percentAllScreens >= params.percentMainScreen)
            ++satisfiedCriteria;
        else
            ++unSatisfiedCriteria;

    // Looking for windows with specific styles
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    if (params.style)
        if (params.style & style)
            ++satisfiedCriteria;
        else
            ++unSatisfiedCriteria;

    // Looking for windows with specific extended styles
    LONG_PTR styleEx = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if (params.styleEx)
        if (params.styleEx & styleEx)
            ++satisfiedCriteria;
        else
            ++unSatisfiedCriteria;

    if (satisfiedCriteria == 0)
    {
        return TRUE;

    }

    if (params.satisfyAllCriteria == 1 && unSatisfiedCriteria > 0)
    {
        return TRUE;
    }

    // If looking for multiple windows
    params.hwnds.push_back(hwnd);
    return TRUE;
}

namespace SetUp {

    inline HWND gWnd{}, DrawWnd{};

#define MAX_CLASSNAME 255
#define MAX_WNDNAME 255
#define TRANSPARENCY_COLOR RGB(0, 254, 0)

    struct WindowsFinderParams {
        DWORD pidOwner = NULL;
        string wndClassName = "";
        string wndName = "";
        RECT pos = { 0, 0, 0, 0 };
        POINT res = { 0, 0 };
        float percentAllScreens = 0.0f;
        float percentMainScreen = 0.0f;
        DWORD style = NULL;
        DWORD styleEx = NULL;
        bool satisfyAllCriteria = false;
        vector<HWND> hwnds;
    };

    // Prototypes
    inline std::vector<HWND> WindowsFinder(WindowsFinderParams params);
    inline BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam);
    inline HWND HiJackNotepadWindow();
    inline std::vector<DWORD> GetPIDs(wstring targetProcessName);

    inline int amain() {
        HWND hwnd = HiJackNotepadWindow();
        if (!hwnd) {
            return EXIT_FAILURE;
        }

        HDC hdc = GetDC(hwnd);

        // Getting settings of back buffer bitmap
        DEVMODE devMode;
        devMode.dmSize = sizeof(devMode);
        EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);
        BITMAPINFO backBufferBmpInfo;
        backBufferBmpInfo = { 0 };
        backBufferBmpInfo.bmiHeader.biBitCount = devMode.dmBitsPerPel;
        backBufferBmpInfo.bmiHeader.biHeight = GetSystemMetrics(SM_CYSCREEN);
        backBufferBmpInfo.bmiHeader.biWidth = GetSystemMetrics(SM_CXSCREEN);
        backBufferBmpInfo.bmiHeader.biPlanes = 1;
        backBufferBmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

        void* backBufferPixels = nullptr;
        POINT res = { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
        HBRUSH bgTransparencyColor = CreateSolidBrush(TRANSPARENCY_COLOR);

        int i = -1;

        while (true) {

            i = (i > res.x) ? 0 : ++i; // To simulate movement

            // Frame preparation
            HDC hdcBackBuffer = CreateCompatibleDC(hdc); // Create back buffer
            HBITMAP hbmBackBuffer = CreateDIBSection(hdcBackBuffer, (BITMAPINFO*)&backBufferBmpInfo, DIB_RGB_COLORS, (void**)&backBufferPixels, NULL, 0); // Create back buffer bitmap
            DeleteObject(SelectObject(hdcBackBuffer, hbmBackBuffer));
            DeleteObject(SelectObject(hdcBackBuffer, bgTransparencyColor));
            Rectangle(hdcBackBuffer, 0, 0, res.x, res.y);


            // Frame presentation
            BitBlt(hdc, 0, 0, res.x, res.y, hdcBackBuffer, 0, 0, SRCCOPY);

            // Cleanup
            DeleteDC(hdcBackBuffer); // Delete back buffer device context
            DeleteObject(hbmBackBuffer); // Delete back buffer bitmap
            backBufferPixels = nullptr;
        }

        return EXIT_SUCCESS;
    }

    inline HWND HiJackNotepadWindow() {
        HWND hwnd = NULL;

        // Remove previous windows
        vector<DWORD> existingNotepads = GetPIDs(_(L"mmc.exe"));
        if (!existingNotepads.empty()) {
            for (int i(0); i < existingNotepads.size(); ++i) {
                // Terminating processes
                HANDLE hOldProcess = OpenProcess(PROCESS_TERMINATE, FALSE, existingNotepads[i]);
                TerminateProcess(hOldProcess, 0);
                CloseHandle(hOldProcess);
            }
        }

        system(_("start mmc.exe")); // Start notepad, and not as child process, so easy :)

        // Finding notepad's window (we could just use FindWindow but then it would be OS language dependent)
        vector<DWORD> notepads = GetPIDs(_(L"mmc.exe"));
        if (notepads.empty() || notepads.size() > 1) // Should check if more than one to be more strict
            return hwnd;
        WindowsFinderParams params;
        params.pidOwner = notepads[0];
        params.style = WS_VISIBLE;
        params.satisfyAllCriteria = true;
        vector<HWND> hwnds;
        int attempt = 0; // The process takes a bit of time to initialise and spawn the window, will try during 5 sec before time out
        while (hwnd == NULL || attempt > 50) {
            Sleep(7500);
            hwnds = WindowsFinder(params);
            if (hwnds.size() > 1)
                return hwnd;
            hwnd = hwnds[0];
            ++attempt;
        }
        if (!hwnd)
            return hwnd;

        // Making the window usable for overlay puposes

        SetMenu(hwnd, NULL);
        SetWindowLongPtr(hwnd, GWL_STYLE, WS_VISIBLE);
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW); // WS_EX_NOACTIVATE  and WS_EX_TOOLWINDOW removes it from taskbar

        SetWindowPos(hwnd, NULL, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);
        globals.ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
        globals.ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
        return hwnd;
    }

    inline std::vector<DWORD> GetPIDs(wstring targetProcessName) {
        vector<DWORD> pids;
        if (targetProcessName == L"")
            return pids;
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32W entry;
        entry.dwSize = sizeof entry;
        if (!Process32FirstW(snap, &entry)) {
            CloseHandle(snap);
            return pids;
        }
        do {
            if (wstring(entry.szExeFile) == targetProcessName) {
                pids.emplace_back(entry.th32ProcessID);
            }
        } while (Process32NextW(snap, &entry));
        CloseHandle(snap);
        return pids;
    }

    inline BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam) {
        WindowsFinderParams& params = *(WindowsFinderParams*)lParam;

        unsigned char satisfiedCriteria = 0, unSatisfiedCriteria = 0;

        // If looking for windows of a specific PDI
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (params.pidOwner != NULL)
            if (params.pidOwner == pid)
                ++satisfiedCriteria; // Doesn't belong to the process targeted
            else
                ++unSatisfiedCriteria;

        // If looking for windows of a specific class
        char className[MAX_CLASSNAME] = "";
        GetClassName(hwnd, (LPWSTR)className, MAX_CLASSNAME);
        string classNameWstr = className;
        if (params.wndClassName != "")
            if (params.wndClassName == classNameWstr)
                ++satisfiedCriteria; // Not the class targeted
            else
                ++unSatisfiedCriteria;

        // If looking for windows with a specific name
        char windowName[MAX_WNDNAME] = "";
        GetWindowText(hwnd, (LPWSTR)windowName, MAX_CLASSNAME);
        string windowNameWstr = windowName;
        if (params.wndName != "")
            if (params.wndName == windowNameWstr)
                ++satisfiedCriteria; // Not the class targeted
            else
                ++unSatisfiedCriteria;

        // If looking for window at a specific position
        RECT pos;
        GetWindowRect(hwnd, &pos);
        if (params.pos.left || params.pos.top || params.pos.right || params.pos.bottom)
            if (params.pos.left == pos.left && params.pos.top == pos.top && params.pos.right == pos.right && params.pos.bottom == pos.bottom)
                ++satisfiedCriteria;
            else
                ++unSatisfiedCriteria;

        // If looking for window of a specific size
        POINT res = { pos.right - pos.left, pos.bottom - pos.top };
        if (params.res.x || params.res.y)
            if (res.x == params.res.x && res.y == params.res.y)
                ++satisfiedCriteria;
            else
                ++unSatisfiedCriteria;

        // If looking for windows taking more than a specific percentage of all the screens
        float ratioAllScreensX = res.x / GetSystemMetrics(SM_CXSCREEN);
        float ratioAllScreensY = res.y / GetSystemMetrics(SM_CYSCREEN);
        float percentAllScreens = ratioAllScreensX * ratioAllScreensY * 450;
        if (params.percentAllScreens != 0.0f)
            if (percentAllScreens >= params.percentAllScreens)
                ++satisfiedCriteria;
            else
                ++unSatisfiedCriteria;

        // If looking for windows taking more than a specific percentage or the main screen
        RECT desktopRect;
        GetWindowRect(GetDesktopWindow(), &desktopRect);
        POINT desktopRes = { desktopRect.right - desktopRect.left, desktopRect.bottom - desktopRect.top };
        float ratioMainScreenX = res.x / desktopRes.x;
        float ratioMainScreenY = res.y / desktopRes.y;
        float percentMainScreen = ratioMainScreenX * ratioMainScreenY * 450;
        if (params.percentMainScreen != 0.0f)
            if (percentAllScreens >= params.percentMainScreen)
                ++satisfiedCriteria;
            else
                ++unSatisfiedCriteria;

        // Looking for windows with specific styles
        LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
        if (params.style)
            if (params.style & style)
                ++satisfiedCriteria;
            else
                ++unSatisfiedCriteria;

        // Looking for windows with specific extended styles
        LONG_PTR styleEx = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        if (params.styleEx)
            if (params.styleEx & styleEx)
                ++satisfiedCriteria;
            else
                ++unSatisfiedCriteria;

        if (!satisfiedCriteria)
            return TRUE;

        if (params.satisfyAllCriteria && unSatisfiedCriteria)
            return TRUE;

        // If looking for multiple windows
        params.hwnds.push_back(hwnd);
        return TRUE;
    }

    inline std::vector<HWND> WindowsFinder(WindowsFinderParams params) {
        EnumWindows(EnumWindowsCallback, (LPARAM)&params);
        return params.hwnds;
    }
}
typedef HRESULT(__stdcall* Td3D11Present)(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags);
Td3D11Present              OPresent = nullptr;

void Error()
{
    getchar();
    exit(-1);
}

ImVec4 clear_color2 = ImVec4(0.45f, 0.55f, 0.60f, 0.f);
float menu_color[4] = { 1.00f, 0.00f, 0.20f, 0.80f };

int menu_tab = menus.tab;

ImFont* ico = nullptr;
ImFont* ico_combo = nullptr;
ImFont* ico_button = nullptr;
ImFont* ico_grande = nullptr;
ImFont* segu = nullptr;
ImFont* default_segu = nullptr;
ImFont* bold_segu = nullptr;
HWND FortniteWindow = NULL;

ID3D11Device* d3d_device;
ID3D11DeviceContext* d3d_device_ctx;
IDXGISwapChain* d3d_swap_chain;
ID3D11RenderTargetView* d3d_render_target;
D3DPRESENT_PARAMETERS d3d_present_params;
typedef struct _Header
{
    UINT Magic;
    UINT FrameCount;
    UINT NoClue;
    UINT Width;
    UINT Height;
    BYTE Buffer[1]; // B8G8R8A8
} Header;
#pragma comment(lib, "d3d11.lib")
#include "other/fonts/burbank.hpp"
//#include "../../imgui/imgui_freetype.h"
#include "../../utilities/cache/rankicons.h"

ImFont* Inter_Medium = nullptr;
ImFont* Inter_Bold = nullptr;
ImFont* Inter_Bold_1 = nullptr;
ImFont* Inter_SemmiBold = nullptr;
ImFont* Inter_SemmiBold_1 = nullptr;
ImFont* Icon_Arrow;


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



namespace GUI {
    float color[4] = { 230, 99, 240, 255 };
    bool hide_gui = false; /* */ int hide_bind;
    bool hide_menu = false;

    float menu_scale = 1.f;

}

namespace cfg {

    bool check_creater = false;
    std::string path_name = "C:\\FN-CONFIG-SAVING-LOC\\";
    std::string line_checker;
    std::string line;
    std::string text;
    std::string s;
    int findnum;
    int findpos;
    const char* delete_file;
    char add_cfg[16] = { "" };
    char name_cfg[16] = { "" };
    char search_cfg[16] = { "" };
    char selected_cfg[16] = { "" };
    std::string plus = path_name + add_cfg + ".ini";
    char buf[100];
}

bool readfile(std::string& s, std::string filename) {
    std::ifstream fp(filename);
    if (!fp.is_open())
        return false;

    char nil = '\0';
    std::getline(fp, s, nil);
    fp.close();
    return (s.length() > 0);
}

void cleaning() {
    memset(&cfg::add_cfg[0], 0, sizeof(cfg::add_cfg));
    memset(&cfg::search_cfg[0], 0, sizeof(cfg::search_cfg));
}

namespace config_base {

    void load_cfg() {
        std::ifstream file(cfg::path_name + cfg::name_cfg + ".ini");
        if (!file.is_open()) return;

        std::string line;
        int section = 0;

        while (std::getline(file, line)) {
            if (line.empty()) continue;
            if (line[0] == '[') {
                if (line == "[GUI]") section = 1;
                else if (line == "[Menu]") section = 2;
                else if (line == "[Aimbot]") section = 3;
                else if (line == "[Triggerbot]") section = 4;
                else if (line == "[Visuals]") section = 5;
                else if (line == "[WeaponCFG]") section = 6;
                else if (line == "[Radar]") section = 7;
                else if (line == "[Colors]") section = 8;
                else if (line == "[Exploits]") section = 9;
                else if (line == "[Globals]") section = 10;

                // add other sections
                continue;
            }

            std::istringstream iss(line);
            switch (section) {
            case 1: { static int i = 0; iss >> GUI::color[i++]; if (i >= 4) i = 0; break; }
            case 2: {
                static int i = 0;
                switch (i++) {
                case 0: iss >> menus.ShowMenu; break;
                case 1: iss >> menus.fontsize; break;
                case 2: iss >> menus.menu_cursor; break;
                case 3: iss >> menus.menu_index; break;
                case 4: iss >> menus.tab; i = 0; break;
                }
                break;
            }
            case 3: {
                static int i = 0;
                switch (i++) {
                case 0: iss >> aimbot.enable; break;
                case 1: iss >> aimbot.target_line; break;
                case 2: iss >> aimbot.target_text; break;
                case 3: iss >> aimbot.method; break;
                case 4: iss >> aimbot.prediction; break;
                case 5: iss >> aimbot.predictiondot; break;
                case 6: iss >> aimbot.vischeck; break;
                case 7: iss >> aimbot.drawfov; break;
                case 8: iss >> aimbot.fov_arrows; break;
                case 9: iss >> aimbot.fov_drawdistance; break;
                case 10: iss >> aimbot.fovsize; break;
                case 11: iss >> aimbot.ADS_Fov_Size; break;
                case 12: iss >> aimbot.smoothsize; break;
                case 13: iss >> aimbot.Hitbox; break;
                case 14: iss >> aimbot.aimkey; break;
                case 15: iss >> aimbot.booty; break;
                case 16: iss >> aimbot.mouse; break;
                case 17: iss >> aimbot.controller; break;
                case 18: iss >> aimbot.shotgunonly; break;
                case 19: iss >> aimbot.ignore_knocked; break;
                case 20: iss >> aimbot.weaponcfg; i = 0; break;
                }
                break;
            }
            case 4: {
                static int i = 0;
                switch (i++) {
                case 0: iss >> triggerbot.triggerbot_enable; break;
                case 1: iss >> triggerbot.delay; break;
                case 2: iss >> triggerbot.triggerbot_distance; break;
                case 3: iss >> triggerbot.triggerbot_key; i = 0; break;
                }
                break;
            }
            case 5: {
                static int i = 0;
                switch (i++) {
                case 0: iss >> visuals.box; break;
                case 1: iss >> visuals.head_esp; break;
                case 2: iss >> visuals.skeleton; break;
                case 3: iss >> visuals.distance; break;
                case 4: iss >> visuals.username; break;
                case 5: iss >> visuals.platform; break;
                case 6: iss >> visuals.rank; break;
                case 7: iss >> visuals.weapon; break;
                case 8: iss >> aimbot.fov_arrows; break;
                case 9: iss >> visuals.filled; break;
                case 10: iss >> visuals.box_outline; break;
                case 11: iss >> visuals.skel_outline; break;
                case 12: iss >> visuals.outlinetext; break;
                case 13: iss >> visuals.renderDistance; break;
                case 14: iss >> visuals.textsize; break;
                case 15: iss >> visuals.box_thick; break;
                case 16: iss >> visuals.skeleton_thickness; break;
                case 17: iss >> config.target_bone; break;
                case 18: iss >> config.filledStyles; break;
                case 19: iss >> config.selected_config; i = 0; break;
                }
                break;
            }
            }
        }

        file.close();
    }


    void add_cfg() {
        std::ofstream file(cfg::path_name + cfg::add_cfg + ".ini");
        if (!file.is_open()) return;

        // GUI
        file << "[GUI]\n";
        for (int i = 0; i < 4; ++i)
            file << GUI::color[i] << "\n";

        // Menu
        file << "\n[Menu]\n";
        file << menus.ShowMenu << "\n";
        file << menus.fontsize << "\n";
        file << menus.menu_cursor << "\n";
        file << menus.menu_index << "\n";
        file << menus.tab << "\n";

        // Aimbot
        file << "\n[Aimbot]\n";
        file << aimbot.enable << "\n";
        file << aimbot.target_line << "\n";
        file << aimbot.target_text << "\n";
        file << aimbot.method << "\n";
        file << aimbot.prediction << "\n";
        file << aimbot.predictiondot << "\n";
        file << aimbot.vischeck << "\n";
        file << aimbot.drawfov << "\n";
        file << aimbot.fov_arrows << "\n";
        file << aimbot.fov_drawdistance << "\n";
        file << aimbot.fovsize << "\n";
        file << aimbot.ADS_Fov_Size << "\n";
        file << aimbot.smoothsize << "\n";
        file << aimbot.Hitbox << "\n";
        file << aimbot.aimkey << "\n";
        file << aimbot.booty << "\n";
        file << aimbot.mouse << "\n";
        file << aimbot.controller << "\n";
        file << aimbot.shotgunonly << "\n";
        file << aimbot.ignore_knocked << "\n";
        file << aimbot.weaponcfg << "\n";

        // Triggerbot
        file << "\n[Triggerbot]\n";
        file << triggerbot.triggerbot_enable << "\n";
        file << triggerbot.delay << "\n";
        file << triggerbot.triggerbot_distance << "\n";
        file << triggerbot.triggerbot_key << "\n";

        // Visuals
        file << "\n[Visuals]\n";
        file << visuals.box << "\n";
        file << visuals.head_esp << "\n";
        file << visuals.skeleton << "\n";
        file << visuals.distance << "\n";
        file << visuals.username << "\n";
        file << visuals.platform << "\n";
        file << visuals.rank << "\n";
        file << visuals.weapon << "\n";
        file << aimbot.fov_arrows << "\n";
        file << visuals.filled << "\n";
        file << visuals.box_outline << "\n";
        file << visuals.skel_outline << "\n";
        file << visuals.outlinetext << "\n";
        file << visuals.renderDistance << "\n";
        file << visuals.textsize << "\n";
        file << visuals.box_thick << "\n";
        file << visuals.skeleton_thickness << "\n";
        file << config.target_bone << "\n";
        file << config.filledStyles << "\n";
        file << config.selected_config << "\n";

        // Add more: visuals, triggerbot, world, etc.

        file.close();
    }


    void reset_cfg() {

        GUI::color[0] = 230; GUI::color[1] = 99; GUI::color[2] = 240; GUI::color[3] = 255;


    }

    namespace delete_file {

        void eraseFileLine(std::string path, std::string eraseLine) {
            std::string line;
            std::ifstream fin;
            fin.open(path);

            std::ofstream temp;
            temp.open("temp.ini");

            while (getline(fin, line)) {

                if (line != eraseLine)
                    temp << line << std::endl;
            }

            temp.close();
            fin.close();

            const char* p = path.c_str();
            remove(p);
            rename("temp.ini", p);
        }
    }

    namespace create_config {

        void create() {

            for (int i = 0; cfg::add_cfg[i]; i++) {
                if (cfg::add_cfg[i] == ' ') {
                    cfg::add_cfg[i] = *"_";
                }
            }

            std::ifstream general_path("C:\\FN-CONFIG-SAVING-LOC\\Config_List.ini");

            std::ofstream data;

            using In = std::istream_iterator<std::string>;

            auto pos = std::find(In(general_path), In(), cfg::add_cfg);
            if (pos != In())
                cfg::check_creater = false;
            else
                cfg::check_creater = true;

            general_path.close();

            if (cfg::add_cfg[0] && cfg::check_creater) {

                std::ofstream outfile(cfg::path_name + cfg::add_cfg + ".ini");

                config_base::add_cfg();

                data.open(cfg::path_name + "Config_List.ini", std::ios_base::app);
                data << cfg::add_cfg << "\n";
                data.close();

                cleaning();
            }
        }
    }

}

void config_list() {

    SetConsoleOutputCP(65001);

    setlocale(LC_ALL, "English");

    CreateDirectory(L"C:\\FN-CONFIG-SAVING-LOC", NULL);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 8));

    ImGui::InputTextEx("##0", NULL, "Search Config", cfg::search_cfg, 16, ImVec2(ImGui::GetWindowWidth() - (ImGui::GetStyle().WindowPadding.x * 2) - 20, 20), ImGuiInputTextFlags_None);
    if (ImGui::IsItemClicked()) PostMessage(GetForegroundWindow(), WM_INPUTLANGCHANGEREQUEST, INPUTLANGCHANGE_SYSCHARSET, 0x409);

    ImGui::InputTextEx("##1", NULL, "Config Name", cfg::add_cfg, 16, ImVec2(ImGui::GetWindowWidth() - (ImGui::GetStyle().WindowPadding.x * 2) - 20, 20), ImGuiInputTextFlags_None);

    if (ImGui::IsItemClicked()) PostMessage(GetForegroundWindow(), WM_INPUTLANGCHANGEREQUEST, INPUTLANGCHANGE_SYSCHARSET, 0x409);

    if (ImGui::Button("Create", ImVec2(ImGui::GetWindowWidth() - (ImGui::GetStyle().WindowPadding.x * 2) - 20, 40))) {
        config_base::create_config::create();
    };

    if (ImGui::Button("Delete", ImVec2(ImGui::GetWindowWidth() - (ImGui::GetStyle().WindowPadding.x * 2) - 20, 40))) {

        cfg::plus = cfg::path_name + cfg::add_cfg + ".ini";
        cfg::delete_file = cfg::plus.c_str();
        remove(cfg::delete_file);

        config_base::delete_file::eraseFileLine("C:\\FN-CONFIG-SAVING-LOC\\Config_List.ini", cfg::add_cfg);
        cleaning();
    };

    if (GetAsyncKeyState(VK_DELETE) & 0x01) {
        cfg::plus = cfg::path_name + cfg::add_cfg + ".ini";
        cfg::delete_file = cfg::plus.c_str();
        remove(cfg::delete_file);

        config_base::delete_file::eraseFileLine("C:\\FN-CONFIG-SAVING-LOC\\Config_List.ini", cfg::add_cfg);
        cleaning();
    }

    if (ImGui::Button("Load", ImVec2(ImGui::GetWindowWidth() - (ImGui::GetStyle().WindowPadding.x * 2) - 20, 40))) {
        config_base::load_cfg();
        cleaning();
    };

    if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
        config_base::load_cfg();
        cleaning();
    }

    if (ImGui::Button("Save", ImVec2(ImGui::GetWindowWidth() - (ImGui::GetStyle().WindowPadding.x * 2) - 20, 40))) {
        config_base::add_cfg();
        cleaning();
    };

    if (ImGui::Button("Reset", ImVec2(ImGui::GetWindowWidth() - (ImGui::GetStyle().WindowPadding.x * 2) - 20, 40)) && cfg::add_cfg[0]) {
        config_base::reset_cfg();
        config_base::add_cfg();
        cleaning();
    };
    ImGui::PopStyleVar();
}

void config_add() {

    std::ifstream in("C:\\FN-CONFIG-SAVING-LOC\\Config_List.ini");
    if (in.is_open())
    {
        while (getline(in, cfg::line))
        {
            if (cfg::line.find(cfg::search_cfg) != std::string::npos && ImGui::config_selector(cfg::line.c_str(), ImVec2(ImGui::GetWindowWidth() - (ImGui::GetStyle().WindowPadding.x * 2), 45))) {

                cleaning();
                memset(&cfg::name_cfg[0], 0, sizeof(cfg::name_cfg));

                char* myChar = new char(cfg::line.length());
                for (int i = 0; i < cfg::line.length(); ++i) {
                    cfg::add_cfg[i] = cfg::line[i];
                    cfg::name_cfg[i] = cfg::line[i];
                }
            };
        }
    }
}

namespace overlay
{
    bool InitImgui()
    {
        DXGI_SWAP_CHAIN_DESC swap_chain_description;
        ZeroMemory(&swap_chain_description, sizeof(swap_chain_description));
        swap_chain_description.BufferCount = 2;
        swap_chain_description.BufferDesc.Width = 0;
        swap_chain_description.BufferDesc.Height = 0;
        swap_chain_description.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swap_chain_description.BufferDesc.RefreshRate.Numerator = 60;
        swap_chain_description.BufferDesc.RefreshRate.Denominator = 1;
        swap_chain_description.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        swap_chain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swap_chain_description.OutputWindow = globals.window_handle;
        swap_chain_description.SampleDesc.Count = 1;
        swap_chain_description.SampleDesc.Quality = 0;
        swap_chain_description.Windowed = 1;
        swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        D3D_FEATURE_LEVEL d3d_feature_lvl;

        const D3D_FEATURE_LEVEL d3d_feature_array[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

        D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, d3d_feature_array, 2, D3D11_SDK_VERSION, &swap_chain_description, &d3d_swap_chain, &d3d_device, &d3d_feature_lvl, &d3d_device_ctx);

        ID3D11Texture2D* pBackBuffer;
        D3DX11_IMAGE_LOAD_INFO info;
        D3DX11_IMAGE_LOAD_INFO image;
        ID3DX11ThreadPump* pump{ nullptr };

        d3d_swap_chain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

        d3d_device->CreateRenderTargetView(pBackBuffer, NULL, &d3d_render_target);

        pBackBuffer->Release();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
        ImFontConfig icons_config;

        ImFontConfig CustomFont;
        CustomFont.FontDataOwnedByAtlas = false;

        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        icons_config.OversampleH = 2.5;
        icons_config.OversampleV = 2.5;

        ImGui::StyleColorsDark();

        ImGui_ImplWin32_Init(globals.window_handle);
        ImGui_ImplDX11_Init(d3d_device, d3d_device_ctx);
        
        ImFontConfig font_config;
        font_config.OversampleH = 1;
        font_config.OversampleV = 1;
        font_config.PixelSnapH = 1;

        static const ImWchar ranges[] =
        {
            0x0020, 0x00FF,
            0x0400, 0x044F,
            0,
        };

       



 

      
        if (Bronze1 == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, bronze1, sizeof(bronze1), &info, pump, &Bronze1, 0);
        if (Bronze2 == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, bronze2, sizeof(bronze2), &info, pump, &Bronze2, 0);
        if (Bronze3 == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, bronze3, sizeof(bronze3), &info, pump, &Bronze1, 0);
        if (Gold1 == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, gold1, sizeof(gold1), &info, pump, &Gold1, 0);
        if (Gold2 == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, gold2, sizeof(gold2), &info, pump, &Gold2, 0);
        if (Gold3 == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, gold3, sizeof(gold3), &info, pump, &Gold3, 0);
        if (Platinum1 == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, platin1, sizeof(platin1), &info, pump, &Platinum1, 0);
        if (Platinum2 == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, platin2, sizeof(platin2), &info, pump, &Platinum2, 0);
        if (Platinum3 == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, platin3, sizeof(platin3), &info, pump, &Platinum3, 0);
        if (Diamond1 == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, diamond1, sizeof(diamond1), &info, pump, &Diamond1, 0);
        if (Diamond2 == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, diamond2, sizeof(diamond2), &info, pump, &Diamond2, 0);
        if (Diamond3 == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, diamond3, sizeof(diamond3), &info, pump, &Diamond3, 0);
        if (Elite == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, elite, sizeof(elite), &info, pump, &Elite, 0);
        if (Champion == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, champion, sizeof(champion), &info, pump, &Champion, 0);
        if (Unreal == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, unreal, sizeof(unreal), &info, pump, &Unreal, 0);
        if (Unranked == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, unranked, sizeof(unranked), &info, pump, &Unranked, 0);

        if (ms::ImgConfigs == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, Recoil_icon, sizeof(Recoil_icon), &info, pump, &ms::ImgConfigs, 0);
        
        if (ms::ImgVisuals == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, eyeIcon, sizeof(eyeIcon), &info, pump, &ms::ImgVisuals, 0);

        if (ms::ImgWeaponConfig == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, weaponIcon, sizeof(weaponIcon), &info, pump, &ms::ImgWeaponConfig, 0);

        if(ms::ImgExploits == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, explosionIcon, sizeof(explosionIcon), &info, pump, &ms::ImgExploits, 0);

        if(ms::ImgPc == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, pcIcon, sizeof(pcIcon), &info, pump, &ms::ImgPc, 0);

        if(ms::ImgConfig == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, configgIcon, sizeof(configgIcon), &info, pump, &ms::ImgConfig, 0);

        if (ms::ImgColor == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, colorIcon, sizeof(colorIcon), &info, pump, &ms::ImgColor, 0);

        if (ms::ImgExit == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, exit_icon, sizeof(exit_icon), &info, pump, &ms::ImgExit, 0);

        if (ms::ImgKey == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, Key_icon, sizeof(Key_icon), &info, pump, &ms::ImgKey, 0);

        if (ms::ImgTime == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, timeIcon, sizeof(timeIcon), &info, pump, &ms::ImgTime, 0);

        if (ms::ImgUser == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, userIcon, sizeof(userIcon), &info, pump, &ms::ImgUser, 0);

        if (ms::ImgFilled == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, filledImage, sizeof(filledImage), &info, pump, &ms::ImgFilled, 0);

        Inter_Medium = io.Fonts->AddFontFromMemoryTTF(&Inter_M, sizeof Inter_M, 15.f, NULL, io.Fonts->GetGlyphRangesCyrillic());

        Inter_Bold = io.Fonts->AddFontFromMemoryTTF(&Inter_B, sizeof Inter_B, 32.f, NULL, io.Fonts->GetGlyphRangesCyrillic());

        Inter_Bold_1 = io.Fonts->AddFontFromMemoryTTF(&Inter_B, sizeof Inter_B, 13.f, NULL, io.Fonts->GetGlyphRangesCyrillic());

        Inter_SemmiBold = io.Fonts->AddFontFromMemoryTTF(&Inter_S, sizeof Inter_S, 15.f, NULL, io.Fonts->GetGlyphRangesCyrillic());

        Inter_SemmiBold_1 = io.Fonts->AddFontFromMemoryTTF(&Inter_S, sizeof Inter_S, 17.f, NULL, io.Fonts->GetGlyphRangesCyrillic());

        Icon_Arrow = io.Fonts->AddFontFromMemoryTTF(&Arrows, sizeof Arrows, 6.f, NULL, io.Fonts->GetGlyphRangesCyrillic());

        if (image::Audio == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, Audio, sizeof(Audio), &info, pump, &image::Audio, 0);

        if (image::texture == nullptr) D3DX11CreateShaderResourceViewFromMemory(d3d_device, texture_byte, sizeof(texture_byte), &info, pump, &image::texture, 0);


        d3d_device->Release();

        return true;
    }

    void CreateRenderTarget()
    {
        ID3D11Texture2D* pBackBuffer;
        d3d_swap_chain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        d3d_device->CreateRenderTargetView(pBackBuffer, NULL, &d3d_render_target);
        pBackBuffer->Release();
    }

    void CleanupRenderTarget()
    {
        if (d3d_render_target) { d3d_render_target->Release(); d3d_render_target = NULL; }
    }

   


    LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        switch (msg)
        {
        case WM_SIZE:
            if (d3d_device != NULL && wParam != SIZE_MINIMIZED)
            {
                CleanupRenderTarget();
                d3d_swap_chain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                CreateRenderTarget();
            }
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
        }
        return ::DefWindowProc(hWnd, msg, wParam, lParam);
    }


    bool renderwin()
    {
        WNDCLASSEXA wcex = {
                sizeof(WNDCLASSEXA),
                0,
                WndProc,
                0,
                0,
                nullptr,
                LoadIcon(nullptr, IDI_APPLICATION),
                LoadCursor(nullptr, IDC_ARROW),
                nullptr,
                nullptr,
                ("Discord"),
                LoadIcon(nullptr, IDI_APPLICATION)
        };

        RECT Rect;

        GetWindowRect(GetDesktopWindow(), &Rect);

        RegisterClassExA(&wcex);

        globals.window_handle = CreateWindowExA(NULL, ("Discord"), ("DiscordOverlayClass"), WS_POPUP, Rect.left, Rect.top, Rect.right, Rect.bottom, NULL, NULL, wcex.hInstance, NULL);

        SetWindowPos(globals.window_handle, HWND_TOPMOST, 0, 0, globals.ScreenWidth, globals.ScreenHeight, SWP_SHOWWINDOW);

        SetWindowLongA(globals.window_handle, GWL_EXSTYLE,
            GetWindowLong(globals.window_handle, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);
        MARGINS margin = { -1 };
        DwmExtendFrameIntoClientArea(globals.window_handle, &margin);

        SetLayeredWindowAttributes(globals.window_handle, RGB(0, 0, 0), 255, LWA_ALPHA);
        MARGINS margins = { -1 };
        DwmExtendFrameIntoClientArea(globals.window_handle, &margins);
        ShowWindow(globals.window_handle, SW_SHOWDEFAULT);
        UpdateWindow(globals.window_handle);
        return true;




    }



    void CleanupOverlay()
    {
        if (!globals.window_handle)
            return;

        // Remove layered and transparency
        LONG_PTR style = GetWindowLongPtrA(globals.window_handle, GWL_EXSTYLE);
        SetWindowLongPtrA(globals.window_handle, GWL_EXSTYLE, style & ~WS_EX_LAYERED);

        // Restore default margins
        MARGINS margins = { 0, 0, 0, 0 };
        DwmExtendFrameIntoClientArea(globals.window_handle, &margins);

        // Optionally hide the window again
        ShowWindow(globals.window_handle, SW_HIDE);
    }



    bool HijackWindow()
    {

        /*  while (!globals.window_handle) {
              globals.window_handle = SetUp::HiJackNotepadWindow();
              Sleep(10);
          }
          MARGINS margin = { -1 };
          DwmExtendFrameIntoClientArea(globals.window_handle, &margin);
          SetMenu(globals.window_handle, NULL);
          SetWindowLongPtrA(globals.window_handle, GWL_STYLE, WS_VISIBLE);
          SetWindowLongPtrA(globals.window_handle, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);
          ShowWindow(globals.window_handle, SW_SHOW);
          UpdateWindow(globals.window_handle);

          SetWindowLongA(globals.window_handle, GWL_EXSTYLE, GetWindowLong(globals.window_handle, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);

          return true;*/
#define E _
        //globals.window_handle = FindWindowA(("Chrome_WidgetWin_1"), ("CrosshairX")); // CrosshairX
        //globals.window_handle = FindWindowA_Spoofed(E("MedalOverlayClass"), E("MedalOverlay")); //medal
        globals.window_handle = FindWindowA(E("Afx:00400000:0:00010005:00000000:00000000"), E("RTSS")); //riva tuner

        if (!globals.window_handle)
        {
            MessageBoxA(0, ("Code -> Error 0xCFFF (RTSS NOT FOUND)"), ("Process Not Open! "), MB_ICONINFORMATION);
            return false;
        }



        SetWindowPos(globals.window_handle, HWND_TOPMOST, 0, 0, globals.ScreenWidth, globals.ScreenHeight, SWP_SHOWWINDOW);
        SetLayeredWindowAttributes(globals.window_handle, RGB(0, 0, 0), 255, LWA_ALPHA);
        SetWindowLongA(globals.window_handle, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);

        MARGINS margin = { -1 };
        DwmExtendFrameIntoClientArea(globals.window_handle, &margin);

        ShowWindow(globals.window_handle, SW_SHOW);
        UpdateWindow(globals.window_handle);

        ShowWindow(globals.window_handle, SW_HIDE);

        return true;
    }
    ImVec2 menu_pos = { (float)(globals.ScreenWidth - WIDTH) * 0.5f, (float)(globals.ScreenHeight - HEIGHT) * 0.5f };


    void move_window()
    {
        static bool dragging = false;
        static ImVec2 drag_start_cursor{};
        static ImVec2 drag_start_pos{};

        ImGui::SetCursorPos({ 0, 0 });
        ImGui::InvisibleButton("Move_detector", menu_size);

        if (ImGui::IsItemActive() && ImGui::IsMouseClicked(0))
        {
            dragging = true;
            drag_start_cursor = ImGui::GetIO().MousePos;
            drag_start_pos = menu_pos;
        }

        if (dragging && ImGui::IsMouseDown(0))
        {
            ImVec2 current = ImGui::GetIO().MousePos;
            ImVec2 delta = ImVec2(current.x - drag_start_cursor.x, current.y - drag_start_cursor.y);
            menu_pos = ImVec2(drag_start_pos.x + delta.x, drag_start_pos.y + delta.y);
        }

        if (dragging && ImGui::IsMouseReleased(0))
        {
            dragging = false;
        }
    }






    ImVec2 leadingWindowSize;
    ImVec2 leadingWindowPos;


    static float tab_alpha = 0.f; /* */ static float tab_add; /* */ static int active_tabs = 0;
    int pagess = 0;
    const char* subtabs[] = { "Aimbot","Triggerbot" };


    void menu_loop()
    {
        if (GetAsyncKeyState(menus.menu_key) & 1)
            menus.ShowMenu = !menus.ShowMenu;

        bool visible = menus.ShowMenu;
        static bool wasVisible = true;

        LONG exStyle = GetWindowLong(globals.window_handle, GWL_EXSTYLE);

        // Toggle click-through
        if (!visible && !(exStyle & WS_EX_TRANSPARENT)) {
            SetWindowLong(globals.window_handle, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);
        }
        else if (visible && (exStyle & WS_EX_TRANSPARENT)) {
            SetWindowLong(globals.window_handle, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT);
        }

        // DO NOT HIDE THE WINDOW — Just update z-order
        if (visible != wasVisible)
        {
            SetWindowPos(
                globals.window_handle,
                HWND_TOPMOST,
                0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW
            );
            UpdateWindow(globals.window_handle);
            wasVisible = visible;
        }

      
      




        if (menus.ShowMenu)
        {

            ImGuiContext& g = *GImGui;
            ImGuiStyle* style = &ImGui::GetStyle();
            ImGuiStyle& s = ImGui::GetStyle();

            s.Colors[ImGuiCol_Border] = ImColor(0, 0, 0, 0);
            s.Colors[ImGuiCol_PopupBg] = ImColor(26, 25, 33, 255);

            s.ChildRounding = 4.f;
            s.WindowRounding = 8.f;
            s.WindowPadding = ImVec2(0, 0);

            s.Colors[ImGuiCol_ChildBg] = ImColor(26, 25, 33, 255);
            s.Colors[ImGuiCol_WindowBg] = ImColor(21, 20, 26, 255);
            ImGui::SetNextWindowSize(ImVec2(WIDTH, HEIGHT));

            static bool centered = false;
            if (!centered && globals.ScreenWidth > 0 && globals.ScreenHeight > 0)
            {
                menu_pos = { (globals.ScreenWidth - WIDTH) * 0.5f, (globals.ScreenHeight - HEIGHT) * 0.5f };
                centered = true;
            }

            ImGui::SetNextWindowPos(menu_pos, ImGuiCond_Always);


            ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = false;
            ImGui::Begin("General", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);
            {
                leadingWindowPos = ImGui::GetWindowPos();
                leadingWindowSize = ImGui::GetWindowSize();

                auto draw = ImGui::GetWindowDrawList();

                const auto& p = ImGui::GetWindowPos();

                const ImVec2& region = ImGui::GetContentRegionMax();

                tab_alpha = ImLerp(tab_alpha, (pagess == active_tabs) ? 1.f : 0.f, 20.f * ImGui::GetIO().DeltaTime);
                if (tab_alpha < 0.01f && tab_add < 0.01f) active_tabs = pagess;

                ImGui::SetCursorPos(ImVec2(8, 30));
                ImGui::BeginGroup();
                {
                    ImGui::BeginTabs(ImVec2(250.f, 0.f));
                    {
                        if (ImGui::Tab("Aim Assist", ms::ImgConfigs, subtabs, ARRAYSIZE(subtabs), 0 == pagess)) pagess = 0;
                        if (ImGui::Tab("Visuals", ms::ImgVisuals, subtabs, ARRAYSIZE(subtabs), 1 == pagess)) pagess = 1;
                        if (ImGui::Tab("Colors", ms::ImgColor, subtabs, ARRAYSIZE(subtabs), 6 == pagess)) pagess = 6;
                        if (ImGui::Tab("Exploits", ms::ImgExploits, subtabs, ARRAYSIZE(subtabs), 3 == pagess)) pagess = 3;
                        if (ImGui::Tab("Misc", ms::ImgPc, subtabs, ARRAYSIZE(subtabs), 4 == pagess)) pagess = 4;
                        if (ImGui::Tab("Configs", ms::ImgConfig, subtabs, ARRAYSIZE(subtabs), 5 == pagess)) pagess = 5;
                    }
                    ImGui::EndTabs();

                }ImGui::EndGroup();

                ImGui::GetWindowDrawList()->AddText(Inter_Bold, 32.f, ImVec2(p.x + 36, p.y + 36), ImGui::GetColorU32(c::text_active), userinformations::menuname);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, tab_alpha * style->Alpha);
                {
                    if (active_tabs == 0)
                    {
                        ImGui::SetCursorPos(ImVec2(251, 110 - (tab_alpha * 100)));
                        ImGui::BeginChild_1("scroll", ImVec2(403, 400), false);
                        {
                            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 10));

      
                            ImGui::BeginChild("1", ImVec2(383, 440), false);
                            {

                                ImGui::Checkbox_Custom("Enable Aimbot", "", "", &aimbot.mouse);
                                ImGui::Separator();
                                if (aimbot.mouse)
                                {
                                    ImGui::Checkbox_Custom("Enable Prediction", "", "", &aimbot.prediction);
                                    ImGui::Checkbox_Custom("Enable FOV", "", "", &aimbot.drawfov);
                                    ImGui::Checkbox_Custom("Enable Visible Check", "", "", &aimbot.vischeck);
                                    ImGui::Checkbox_Custom("Ignore Team (Visual too)", "", "", &aimbot.ignore_team);
                                    ImGui::Keybind("Hotkey", "", ".", &aimbot.aimkey);

                                    const char* hitboxes[5]{ "Head", "Neck", "Chest", "Pelvis", "Random"};

                                    ImGui::Combo("Target Hitbox", &aimbot.Hitbox, hitboxes, IM_ARRAYSIZE(hitboxes), 2);

                                    ImGui::Separator();
                                    if (!aimbot.weaponcfg)
                                    {
                                        ImGui::SliderInt("1", "FOV Size", "", "", &aimbot.fovsize, 1, 500, "%d");
                                        ImGui::SliderInt("2", "Smoothness (Keep high or else very snappy)", "", "", &aimbot.smoothsize, 1, 30, "%d");
                                    }
                                    

                                    
                                }
                        
                            }ImGui::EndChild();

                            ImGui::BeginChild("2", ImVec2(383, 230), false);
                            {

                                ImGui::Checkbox_Custom("Enable Triggerbot", "", "", &triggerbot.triggerbot_enable);
                                ImGui::Separator();
                                if (triggerbot.triggerbot_enable)
                                {
                                    ImGui::SliderInt("3", "Trigger Delay", "", "", &triggerbot.delay, 0, 10, "%ds%");
                                    ImGui::SliderInt("4", "Max Trigger Distance", "", "", &triggerbot.triggerbot_distance, 10, 100, "%dm%");
                                    ImGui::Keybind("Trigger Hotkey", "", ".", &triggerbot.triggerbot_key);
                                    ImGui::Checkbox_Custom("Shotgun only", "", "", &aimbot.shotgunonly);
                                }

                            }ImGui::EndChild();

                            ImGui::BeginChild("3", ImVec2(383, 45), false);
                            {
                                ImGui::Checkbox_Custom("Show Aim Target Line (Red)", "", "", &aimbot.target_line);
                                

                            }ImGui::EndChild();
                          

                            ImGui::PopStyleVar();
                        }ImGui::EndChild();
                    }
                    if (active_tabs == 1)
                    {
                        ImGui::SetCursorPos(ImVec2(251, 110 - (tab_alpha * 100)));
                        ImGui::BeginChild_1("scroll", ImVec2(403, 400), false);
                        {
                            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 10));


                            ImGui::BeginChild("4", ImVec2(383, 480), false);
                            {

                                ImGui::Checkbox_Custom("Enable Box", "", "", &visuals.box);
                                ImGui::Checkbox_Custom("Enable Head Box", "", "", &visuals.head_esp);
                                ImGui::Checkbox_Custom("Enable Skeleton", "", "", &visuals.skeleton);
                                ImGui::Checkbox_Custom("Enable Crosshair", "", "", &visuals.crosshair);
                                ImGui::Checkbox_Custom("Enable Distance", "", "", &visuals.distance);
                                ImGui::Checkbox_Custom("Enable Username", "", "", &visuals.username);
                                ImGui::Checkbox_Custom("Enable Platform", "", "", &visuals.platform);
                                ImGui::Checkbox_Custom("Enable Rank", "", "", &visuals.rank);
                                ImGui::Checkbox_Custom("Enable Weapon", "", "", &visuals.weapon);
                                ImGui::Checkbox_Custom("Enable FOV Arrows", "", "", &aimbot.fov_arrows);

                                ImGui::Separator();
                                
                                ImGui::Checkbox_Custom("Fill Box", "", "", &visuals.filled);
                                ImGui::Checkbox_Custom("Outline Box", "", "", &visuals.box_outline);
                                ImGui::Checkbox_Custom("Outline Skeleton", "", "", &visuals.skel_outline);
                                ImGui::Checkbox_Custom("Outline Text", "", "", &visuals.outlinetext);

                            }ImGui::EndChild();

                         
                            ImGui::BeginChild("OtherVislaStuff", ImVec2(383, 470), false);
                            {
                                ImGui::SliderInt("363", "Max Render Distance", "", "", &visuals.renderDistance, 5, 2000, "%d");
                                ImGui::SliderInt("5", "Text Size", "", "", &visuals.textsize, 5, 30, "%d");
                                ImGui::SliderInt("6", "Box Size", "", "", &visuals.box_thick, 1, 20, "%d");
                                ImGui::SliderInt("7", "Skeleton Size", "", "", &visuals.skeleton_thickness, 1, 20, "%d");
                                const char* boxtypes[2]{ "Normal", "Corned" };
                                ImGui::Combo("Box Styles", &config.target_bone, boxtypes, IM_ARRAYSIZE(boxtypes), 2);
                                const char* filledtypes[2]{ "Normal", "Yeee" };
                                ImGui::Combo("Filled Styles (Booster Only)", &config.filledStyles, filledtypes, IM_ARRAYSIZE(filledtypes), 2);
                                const char* skeltypes[2]{ "Normal", "Wavy"};
                                ImGui::Combo("Skeleton Styles (Booster Only)", &config.selected_config, skeltypes, IM_ARRAYSIZE(skeltypes), 2);

                            }ImGui::EndChild();

                            ImGui::BeginChild("RadarStuff", ImVec2(383, 270), false);
                            {

                                ImGui::Checkbox_Custom("Enable Radar", "", "", &visuals.radar);
                                if (visuals.radar)
                                {
                                    ImGui::SliderFloat("8", "Radar Left/Right", "", "", &radar_position_x, 1.f, 2000.f);
                                    ImGui::SliderFloat("9", "Radar Up/Down", "", "", &radar_position_y, 1.f, 2000.f);
                                    ImGui::SliderFloat("10", "Radar Distance", "", "", &RadarDistance, 10.f, 400.f);
                                    ImGui::SliderFloat("11", "Radar Size", "", "", &radar_size, 100.f, 500.f);
                                }





                            }ImGui::EndChild();

                            ImGui::PopStyleVar();
                        }ImGui::EndChild();
                    }
                    if (active_tabs == 6)
                    {
                        ImGui::SetCursorPos(ImVec2(251, 110 - (tab_alpha * 100)));
                        ImGui::BeginChild_1("scroll", ImVec2(403, 400), false);
                        {
                            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 10));


                            ImGui::BeginChild("Colors", ImVec2(383, 180), false);
                            {

                                ImGui::HueSlider("Text Color", vscolors.text_color, picker_flags);
                                ImGui::HueSlider("Visible Color", vscolors.box_visible, picker_flags);
                                ImGui::HueSlider("Invisible Color", vscolors.box_invisible, picker_flags);

                            }ImGui::EndChild();

                            ImGui::PopStyleVar();
                        }ImGui::EndChild();
                    }
                    if (active_tabs == 3)
                    {
                        ImGui::SetCursorPos(ImVec2(251, 110 - (tab_alpha * 100)));
                        ImGui::BeginChild_1("scroll", ImVec2(403, 400), false);
                        {
                            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 10));


                            ImGui::BeginChild("7", ImVec2(383, 45), false);
                            {

                                ImGui::Checkbox_Custom("Coming soon!", "", "", &exploits.no_recoil);
                               




                            }ImGui::EndChild();

                            




                            ImGui::PopStyleVar();
                        }ImGui::EndChild();
                    }
                    if (active_tabs == 4)
                    {
                        ImGui::SetCursorPos(ImVec2(251, 110 - (tab_alpha * 100)));
                        ImGui::BeginChild_1("scroll", ImVec2(403, 400), false);
                        {
                            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 10));


                            ImGui::BeginChild("8", ImVec2(383, 75), false);
                            {

                                ImGui::Checkbox_Custom("Enable Watermark (Booster Only)", "", "", &globals.showfps);
                                ImGui::Checkbox_Custom("Enable V-Sync", "", "", &globals.vsync);




                            }ImGui::EndChild();






                            ImGui::PopStyleVar();
                        }ImGui::EndChild();
                    }
                    if (active_tabs == 5)
                    {
                        ImGui::SetCursorPos(ImVec2(251, 110 - (tab_alpha * 100)));
                        ImGui::BeginChild_1("scroll1", ImVec2(403, 400), false);
                        {
                            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 15));

                            ImGui::BeginChild("Config Add", ImVec2(383, 279), false);
                            {
                                ImGui::SetCursorPos(ImVec2(10, 10));
                                ImGui::BeginGroup();
                                {

                                    config_add();

                                }ImGui::EndGroup();
                            }ImGui::EndChild();

                            ImGui::BeginChild("Config List", ImVec2(383, 350), false);
                            {
                                ImGui::SetCursorPos(ImVec2(10, 10));
                                ImGui::BeginGroup();
                                {

                                    config_list();

                                }ImGui::EndGroup();

                            }ImGui::EndChild();
                            ImGui::PopStyleVar();
                        }ImGui::EndChild();

                    }

                }ImGui::PopStyleVar();
                move_window();
            }ImGui::End();
        }
    }

    void draw()
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        actorloop();
        menu_loop();

       

        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color2.x * clear_color2.w, clear_color2.y * clear_color2.w, clear_color2.z * clear_color2.w, clear_color2.w };
        d3d_device_ctx->OMSetRenderTargets(1, &d3d_render_target, nullptr);
        d3d_device_ctx->ClearRenderTargetView(d3d_render_target, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        d3d_swap_chain->Present(globals.vsync, 0);
    }

    bool render()
    {
        MSG msg = { NULL };
        ZeroMemory(&msg, sizeof(MSG));

        while (msg.message != WM_QUIT)
        {

            UpdateWindow(globals.window_handle);
            ShowWindow(globals.window_handle, SW_SHOW);

            if (PeekMessageA(&msg, globals.window_handle, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            ImGuiIO& io = ImGui::GetIO();
            io.DeltaTime = 1.0f / 60.0f;


            POINT p_cursor;
            GetCursorPos(&p_cursor);
            io.MousePos.x = p_cursor.x;
            io.MousePos.y = p_cursor.y;

            if (GetAsyncKeyState(VK_LBUTTON)) {
                io.MouseDown[0] = true;
                io.MouseClicked[0] = true;
                io.MouseClickedPos[0].x = io.MousePos.x;
                io.MouseClickedPos[0].x = io.MousePos.y;
            }
            else
                io.MouseDown[0] = false;

            draw();
        }

        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        DestroyWindow(globals.window_handle);

        return true;
    }

    void start()
    {
        // HijackWindow();
        renderwin();
        InitImgui();

        CreateThread(0, 0, LPTHREAD_START_ROUTINE(render()), 0, 0, 0);
        std::thread([&]() { game_helper.get_camera(); }).detach();
    }
}

