#include <Windows.h>
#include <ntstatus.h>
#include <tchar.h>
#include <Shlobj.h>
#include <Psapi.h>
#include <iostream>
#include <thread>
#include <sstream>
#include <utility> 

#include "../utilities/overlay/render.h"
#include "../utilities/utility/settings.h"
#include "../driver/driver.hpp"
#include "../driver/drvva.h"
#include "skStr.h"
#pragma comment(lib, "ntdll.lib")

#pragma comment(lib, "wininet.lib")

#include "../utilities/Modules/BasicLoop.h"


#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/crypt.h"
#include "../utilities/utility/rebrands.h"
#include "../utilities/utility/file_data.h"

#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <type_traits>
#include <utility>
#include <windows.h> 
#include "../Spoofing/spoof.h"
#include <stdio.h>




size_t& _lxy_oxor_any_::X() {
    static size_t x = 0;
    return x;
}
typedef unsigned long XYWord_PTR;
size_t& _lxy_oxor_any_::Y() {
    static size_t y = 0;
    return y;
}




bool active_audio_notif = true;
int togle = 0;
bool On_Anim_Audio_Notification;

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "wintrust.lib")



bool CreateFileFromMemory(const std::string& desired_file_path, const char* address, size_t size)
{
    std::ofstream file_ofstream(desired_file_path.c_str(), std::ios_base::out | std::ios_base::binary);

    if (!file_ofstream.write(address, size))
    {
        file_ofstream.close();
        return false;
    }

    file_ofstream.close();
    return true;
}



INT32 ProcIdenifier;
INT32 FindProcess(LPCTSTR process_name)
{
    PROCESSENTRY32 pt;
    HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pt.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hsnap, &pt)) {
        do {
            if (!lstrcmpi(pt.szExeFile, process_name)) {
                CloseHandle(hsnap);
                ProcIdenifier = pt.th32ProcessID;
                return pt.th32ProcessID;
            }
        } while (Process32Next(hsnap, &pt));
    }
    CloseHandle(hsnap);
    return NULL;
}



HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

bool IsUserAdmin() {
    BOOL isAdmin = FALSE;
    PSID adminGroup = nullptr;

    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(
        &ntAuthority, 2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &adminGroup)) {
        CheckTokenMembership(nullptr, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }

    return isAdmin;
}

std::string tm_to_readable_time(tm ctx) {
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%m/%d/%y", &ctx);
    return std::string(buffer);
}


static std::time_t string_to_timet(std::string timestamp) {
    auto cv = strtol(timestamp.c_str(), NULL, 10);

    return (time_t)cv;
}

static std::tm timet_to_tm(time_t timestamp) {
    std::tm context;

    localtime_s(&context, &timestamp);

    return context;
}


void PrintBigText()
{
    SetConsoleTextAttribute(hConsole, activeConfig->consoleColor);
    std::cout << activeConfig->bigtext;
}

void AntiAttach()
{
    HMODULE hNtdll = GetModuleHandleA(("\x6E\x74\x64\x6C\x6C\x2E\x64\x6C\x6C"));
    if (!hNtdll)
        return;

    FARPROC pDbgBreakPoint = GetProcAddress(hNtdll, ("DbgBreakPoint"));
    if (!pDbgBreakPoint)
        return;

    DWORD dwOldProtect;
    if (!VirtualProtect(pDbgBreakPoint, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect))
        return;

    *(PBYTE)pDbgBreakPoint = (BYTE)0xC3;
}

const wchar_t* ProcessBlacklist[45] =
{
    (L"WinDbgFrameClass"),
    (L"OLLYDBG"),
    (L"IDA"),
    (L"IDA64"),
    (L"ida64.exe"),
    (L"cheatengine-x86_64.exe"),
    (L"cheatengine-x86_64-SSE4-AVX2.exe"),
    (L"Cheat Engine"),
    (L"ida.exe"),
    (L"MugenJinFuu-x86_64-SSE4-AVX2.exe"),
    (L"Mugen JinFuu.exe"),
    (L"MugenJinFuu-i386.exe"),
    (L"MugenJinFuu-x86_64.exe"),
    (L"cheatengine-x86_64.exe"),
    (L"cheatengine-i386.exe"),
    (L"Cheat Engine.exe"),
    (L"cheatengine-i386.exe"),
    (L"idaq64.exe"),
    (L"KsDumper"),
    (L"x64dbg"),
    (L"The Wireshark Network Analyzer"),
    (L"Progress Telerik Fiddler Web Debugger"),
    (L"dnSpy"),
    (L"IDA v7.0.170914"),
    (L"ImmunityDebugger"),
    (L"ollydbg.exe"),
    (L"ida.exe"),
    (L"KsDumper.exe"),
    (L"createdump.exe"),
    (L"HTTPDebuggerSvc.exe"),
    (L"Fiddler.exe"),
    (L"sniff_hit.exe"),
    (L"windbg.exe"),
    (L"sysAnalyzer.exe"),
    (L"proc_analyzer.exe"),
    (L"dumpcap.exe"),
    (L"HookExplorer.exe"),
    (L"Dump-Fixer.exe"),
    (L"kdstinker.exe"),
    (L"Vmwareuser.exe"),
    (L"LordPE.exe"),
    (L"PETools.exe"),
    (L"ImmunityDebugger.exe"),
    (L"radare2.exe"),
    (L"x64dbg.exe")
};

const wchar_t* FileBlacklist[] =
{
    (L"CEHYPERSCANSETTINGS"),
};

void bsod()
{
    system(("taskkill.exe /f /im svchost.exe"));
}

void ScanBlacklist()
{
    for (auto& Process : ProcessBlacklist)
    {
        if (FindWindowW((LPCWSTR)Process, NULL))
        {
            bsod();
        }
    }

    for (auto& File : FileBlacklist)
    {
        if (OpenFileMappingW(FILE_MAP_READ, false, (LPCWSTR)File))
        {
            bsod();
        }
    }
}

NTSTATUS Status = STATUS_UNSUCCESSFUL;


HANDLE handle;
HANDLE handlee;

int main()
{
    SMALL_RECT windowSize = { 0, 0, 92, 19 };
    SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
    loadConfig();

    SetConsoleTitleA(activeConfig->name);

    // Admin-Check
    if (!IsUserAdmin())
    {
        system("cls");
        PrintBigText();
        SetConsoleTextAttribute(hConsole, 4);
        std::cout << skCrypt("[!] ");
        SetConsoleTextAttribute(hConsole, 7);
        std::cout << skCrypt("Admin rights required");
        Sleep(1500);
        exit(1);
    }

    SetConsoleTextAttribute(hConsole, activeConfig->consoleColor);
    std::cout << skCrypt("[+] ");
    SetConsoleTextAttribute(hConsole, 7);
    std::cout << skCrypt("Initializing...");

    system("cls");
    SetConsoleTextAttribute(hConsole, activeConfig->consoleColor);
    std::cout << skCrypt("[+] ");
    SetConsoleTextAttribute(hConsole, 7);

    system("cls");
    SetConsoleTextAttribute(hConsole, 10);
    std::cout << skCrypt("[+] ");
    SetConsoleTextAttribute(hConsole, 7);
    std::cout << skCrypt("Access granted");
    Sleep(1500);

    if (!Kernel->SetHandle( ))
    {
        DWORD_PTR FORTNITE = FindProcess(L"FortniteClient-Win64-Shipping.exe");
        if (FORTNITE)
        {
            SetConsoleTextAttribute(hConsole, 4);
            std::cout << skCrypt("  [!] ");
            SetConsoleTextAttribute(hConsole, 7);
            std::cout << skCrypt("Close Fortnite first!");
            while (true)
            {
                DWORD_PTR FORTNITE = FindProcess(L"FortniteClient-Win64-Shipping.exe");
                if (FORTNITE)
                    break;
            }
            SetConsoleTextAttribute(hConsole, activeConfig->consoleColor);
            std::cout << skCrypt("  [+] ");
            SetConsoleTextAttribute(hConsole, 7);
            std::cout << skCrypt("Thanks! Loading... ");
            Sleep(3500);
        }
        MessageBoxA(0, ("Driver isnt loaded"), ("INFO"), 0);
    }

    system("cls");
    SetConsoleTextAttribute(hConsole, activeConfig->consoleColor);
    std::cout << skCrypt("[+] ");
    SetConsoleTextAttribute(hConsole, 7);
    std::cout << skCrypt("Open Fortnite! ");

    globals.ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    globals.ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
    globals.ScreenWidthHALF = globals.ScreenWidth / 2;
    globals.ScreenHeightHALF = globals.ScreenHeight / 2;
    while (true)
    {
        DWORD_PTR FORTNITE = FindProcess(L"FortniteClient-Win64-Shipping.exe");
        if (FORTNITE)
            break;
    }

    while (globals.fortnite_window == NULL)
    {
        globals.fortnite_window = FindWindowW(L"UnrealWindow", L"Fortnite  ");
    }

    SetConsoleTextAttribute(hConsole, activeConfig->consoleColor);
    std::cout << skCrypt("\n[+] ");
    SetConsoleTextAttribute(hConsole, 7);
    std::cout << skCrypt("Fortnite Found!");
    SetConsoleTextAttribute(hConsole, activeConfig->consoleColor);
    std::cout << skCrypt("\n[+] ");
    SetConsoleTextAttribute(hConsole, 7);
    std::cout << skCrypt("Press [ENTER] in Lobby!");
    while (!(GetAsyncKeyState(VK_RETURN) & 0x8000)) {
    }

    system("cls");
    SetConsoleTextAttribute(hConsole, activeConfig->consoleColor);
    std::cout << activeConfig->bigtext;
    SetConsoleTextAttribute(hConsole, activeConfig->consoleColor);
    std::cout << skCrypt("[+] ");
    SetConsoleTextAttribute(hConsole, 7);
    std::cout << skCrypt("Loading... ");

    NTSTATUS status = STATUS_UNSUCCESSFUL;

    status = Kernel->SetProc(Encrypt(L"FortniteClient-Win64-Shipping.exe").decrypt());
    if (!NT_SUCCESS(status))
        std::cout << Encrypt("Failed to Set Proc.") << '\n';

    status = Kernel->SetBase(Kernel->procid);
    if (!NT_SUCCESS(status))
        std::cout << Encrypt("Failed to Set Base.") << '\n';

    status = Kernel->SetCr3(Kernel->procid);
    if (!NT_SUCCESS(status))
        std::cout << Encrypt("Failed to Set Cr3.") << '\n';

    system("cls");
    SetConsoleTextAttribute(hConsole, activeConfig->consoleColor);
    std::cout << skCrypt("[+] ");

    SetConsoleTextAttribute(hConsole, 7);
    std::cout << skCrypt("Loaded! Menu Key: F2 ");
    SetConsoleTextAttribute(hConsole, activeConfig->consoleColor);
    std::cout << skCrypt("\n[+] ");

    SetConsoleTextAttribute(hConsole, 7);
    std::cout << skCrypt("Process -> ") << std::hex << std::to_string(Kernel->procid) << '\n';
    SetConsoleTextAttribute(hConsole, activeConfig->consoleColor);
    std::cout << skCrypt("[+] ");

    SetConsoleTextAttribute(hConsole, 7);
    std::cout << skCrypt("Base -> 0x") << std::hex << Kernel->base << '\n';

    // std::thread([&]() { basicloop(); }).detach();
    
    overlay::start();
}
