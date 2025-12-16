// EnhancedProtection.cpp
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <TlHelp32.h>
#include <algorithm>
#include <cctype>
#include <mutex>
#include <atomic>
#include <chrono>
#include <winhttp.h>
#include <psapi.h>
#include <tchar.h>
#include <stdio.h>
#include <winternl.h>

// Link necessary libraries
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "psapi.lib")

// Disable specific warnings
#pragma warning(push)
#pragma warning(disable: 4996)

// Namespace for Process Monitoring and Protection
namespace ProcessMonitor {

    // Target window classes to monitor and terminate
    const std::vector<std::string> targetWindowClasses = {
        "Qt5QWindowIcon",
        "MainWindowClassName"
    };

    // Utility function to convert std::string to std::wstring
    std::wstring StringToWString(const std::string& s) {
        if (s.empty()) return std::wstring();
        int len;
        int slength = static_cast<int>(s.length()) + 1;
        len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, nullptr, 0);
        std::wstring r(len, L'\0');
        MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, &r[0], len);
        if (!r.empty() && r.back() == L'\0') {
            r.pop_back();
        }
        return r;
    }

    // Utility function to convert std::wstring to std::string
    std::string WStringToString(const std::wstring& s) {
        if (s.empty()) return std::string();
        int len;
        int slength = static_cast<int>(s.length());
        len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, nullptr, 0, nullptr, nullptr);
        std::string r(len, '\0');
        WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, nullptr, nullptr);
        return r;
    }

    // Callback function for EnumWindows to terminate target processes
    BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
        const int classNameSize = 256;
        wchar_t className[classNameSize];
        if (GetClassNameW(hwnd, className, classNameSize) == 0) {
            return TRUE;
        }

        std::wstring wClassName(className);
        std::string sClassName = WStringToString(wClassName);
        std::transform(sClassName.begin(), sClassName.end(), sClassName.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        for (const auto& targetClass : targetWindowClasses) {
            std::string targetClassLower = targetClass;
            std::transform(targetClassLower.begin(), targetClassLower.end(), targetClassLower.begin(),
                [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

            if (sClassName == targetClassLower) {
                DWORD processID;
                GetWindowThreadProcessId(hwnd, &processID);

                if (processID == GetCurrentProcessId()) {
                    continue;
                }

                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processID);
                if (hProcess != NULL) {
                    if (!TerminateProcess(hProcess, 1)) {
                        // Optional: Log termination failure
                    }
                    else {
                        // Optional: Log successful termination
                    }
                    CloseHandle(hProcess);
                }
                else {
                    // Optional: Log failure to open process
                }

                break;
            }
        }

        return TRUE;
    }

    // Function to enumerate and terminate target processes
    void TerminateProcessesByWindowClass() {
        if (!EnumWindows(EnumWindowsProc, 0)) {
            // Optional: Log EnumWindows failure
        }
    }

    // Function to perform the enumeration and termination
    void EnumerateAndTerminateProcesses() {
        TerminateProcessesByWindowClass();
    }
}

// Global Variables for Worker and Monitor Threads
std::thread workerThread;
std::thread monitorThread;
std::mutex threadMutex;
std::atomic<bool> stopWorker(false);
std::atomic<std::chrono::steady_clock::time_point> lastRunTime{ std::chrono::steady_clock::now() };

// Worker Function to Continuously Monitor Processes
void WorkerFunction() {
    while (!stopWorker.load()) {
        try {
            ProcessMonitor::EnumerateAndTerminateProcesses();
            lastRunTime.store(std::chrono::steady_clock::now());
        }
        catch (...) {
            // Handle exceptions silently
        }
        Sleep(1000); // Sleep for 1 second
    }
}

// Function to Start the Worker Thread
void StartWorker() {
    std::lock_guard<std::mutex> lock(threadMutex);
    if (workerThread.joinable()) {
        return;
    }
    stopWorker.store(false);
    workerThread = std::thread(WorkerFunction);
    // Optional: Log worker thread start
}

// Function to Stop the Worker Thread
void StopWorker() {
    {
        std::lock_guard<std::mutex> lock(threadMutex);
        if (!workerThread.joinable()) {
            return;
        }
        stopWorker.store(true);
    }
    if (workerThread.joinable()) {
        workerThread.join();
        // Optional: Log worker thread stop
    }
}

// Typedef for NtCreateThreadEx
typedef NTSTATUS(NTAPI* pNtCreateThreadEx)(
    PHANDLE ThreadHandle,
    ACCESS_MASK DesiredAccess,
    PVOID ObjectAttributes,
    HANDLE ProcessHandle,
    LPTHREAD_START_ROUTINE StartRoutine,
    PVOID Argument,
    ULONG CreateFlags,
    ULONG_PTR ZeroBits,
    SIZE_T StackSize,
    SIZE_T MaximumStackSize,
    PVOID AttributeList
    );

// Function to Monitor Worker Thread Responsiveness
void MonitorFunction() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        auto lastTime = lastRunTime.load();
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastTime).count();
        if (duration > 2) { // If worker hasn't run in 2 seconds
            std::cout << "[codex Protection] Worker thread unresponsive. Restarting..." << std::endl;
            StopWorker();
            StartWorker();
        }
    }
}
 

 
// Function to Create Hidden Threads Using NtCreateThreadEx
void multihide(LPTHREAD_START_ROUTINE funcs) {
    HMODULE ntdll = LoadLibraryA("ntdll.dll");
    if (!ntdll) {
        return;
    }

    pNtCreateThreadEx ntcreatethreadex = (pNtCreateThreadEx)GetProcAddress(ntdll, "NtCreateThreadEx");
    if (!ntcreatethreadex) {
        FreeLibrary(ntdll);
        return;
    }

    HANDLE thread = nullptr;

    NTSTATUS status = ntcreatethreadex(
        &thread,
        GENERIC_ALL,
        nullptr,
        GetCurrentProcess(),
        funcs,
        nullptr,
        FALSE,
        0, 0, 0, nullptr
    );

    if (thread && status == 0) {
        CloseHandle(thread);
    }

    FreeLibrary(ntdll);
}

// Function to Detect Presence of Debugger Modules
bool IsDebuggerPresentModule() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    // Get a list of all modules in this process.
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods), &cbNeeded)) {
        for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            wchar_t szModName[MAX_PATH];
            // Get the full path to the module's file.
            if (GetModuleFileNameExW(GetCurrentProcess(), hMods[i], szModName, sizeof(szModName) / sizeof(wchar_t))) {
                // Convert to lowercase for case-insensitive comparison
                std::wstring wsModName(szModName);
                std::transform(wsModName.begin(), wsModName.end(), wsModName.begin(), ::towlower);

                // Check for common debugger modules
                if (wsModName.find(L"x64dbg.dll") != std::wstring::npos ||
                    wsModName.find(L"cheatengine-x86_64.dll") != std::wstring::npos ||
                    wsModName.find(L"ollydbg.dll") != std::wstring::npos) {
                    return true;
                }
            }
        }
    }
    return false;
}

// Typedef for WriteProcessMemory
typedef BOOL(WINAPI* WriteProcessMemory_t)(
    HANDLE hProcess,
    LPVOID lpBaseAddress,
    LPCVOID lpBuffer,
    SIZE_T nSize,
    SIZE_T* lpNumberOfBytesWritten
    );

// Pointer to Original WriteProcessMemory
WriteProcessMemory_t OriginalWriteProcessMemory = nullptr;

// Hooked WriteProcessMemory Function
BOOL WINAPI HookedWriteProcessMemory(
    HANDLE hProcess,
    LPVOID lpBaseAddress,
    LPCVOID lpBuffer,
    SIZE_T nSize,
    SIZE_T* lpNumberOfBytesWritten
) {
    // Block attempts to write to this process
    if (hProcess == GetCurrentProcess()) {
        std::cout << "[Anti-WriteProcessMemory] Attempt to write to this process detected and blocked." << std::endl;
        return FALSE;
    }

    // Allow writing to other processes
    return OriginalWriteProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten);
}

// Function to Hook WriteProcessMemory via IAT Hooking
void HookWriteProcessMemoryFunction() {
    HMODULE hModule = GetModuleHandleA("kernel32.dll");
    if (!hModule) return;

    // Get the address of WriteProcessMemory
    FARPROC procAddress = GetProcAddress(hModule, "WriteProcessMemory");
    if (!procAddress) return;

    // Save the original WriteProcessMemory function pointer
    OriginalWriteProcessMemory = (WriteProcessMemory_t)procAddress;

    // Change memory protection to allow writing
    DWORD oldProtect;
    if (!VirtualProtect(procAddress, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        return;
    }

    // Calculate relative address for JMP instruction
    uintptr_t relativeAddress = ((uintptr_t)HookedWriteProcessMemory - (uintptr_t)procAddress - 5);

    // Overwrite the first bytes with a JMP to the hooked function
    BYTE patch[5] = { 0xE9 };
    memcpy(patch + 1, &relativeAddress, 4);
    memcpy((void*)procAddress, patch, 5);

    // Restore original protection
    VirtualProtect(procAddress, sizeof(void*), oldProtect, &oldProtect);
}

// Function to Unhook WriteProcessMemory (Optional)
void UnhookWriteProcessMemoryFunction() {
    if (!OriginalWriteProcessMemory) return;

    HMODULE hModule = GetModuleHandleA("kernel32.dll");
    if (!hModule) return;

    FARPROC procAddress = GetProcAddress(hModule, "WriteProcessMemory");
    if (!procAddress) return;

    DWORD oldProtect;
    if (!VirtualProtect(procAddress, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        return;
    }

    // Restore original bytes (Assuming original function starts with a JMP)
    // Note: This simplistic approach assumes the original function was not modified elsewhere.
    // In a production environment, store the original bytes before hooking and restore them here.
    BYTE originalBytes[5] = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC }; // Example bytes; replace with actual
    memcpy((void*)procAddress, originalBytes, 5);

    VirtualProtect(procAddress, sizeof(void*), oldProtect, &oldProtect);
}
 
 

// Placeholder for Integrity Verification Function
bool VerifyIntegrity() {
    // Implement actual integrity verification logic here
    // For example, compute hash of critical sections and compare with known values
    return true; // Placeholder: Always returns true (no tampering detected)
}

// Function to Continuously Check Integrity
void IntegrityCheckFunction() {
    while (!stopWorker.load()) {
        if (!VerifyIntegrity()) {
            ExitProcess(0);
        }
        Sleep(1000); // Sleep for 1 second
    }
}
// Forward Declarations of TLS Callback Functions
void tls_callback1(PVOID hModule, DWORD dwReason, PVOID pContext);
void tls_callback_static(PVOID hModule, DWORD dwReason, PVOID pContext);
// CODE STARTS 400
// CODE STARTS 400
// CODE STARTS 400
// CODE STARTS 400
// CODE STARTS 400
// CODE STARTS 400
// CODE STARTS 400
// CODE STARTS 400
// CODE STARTS 400
// CODE STARTS 400
// CODE STARTS 400
// CODE STARTS 400
// CODE STARTS 400
// CODE STARTS 400
// CODE STARTS 400
// CODE STARTS 400
// CODE STARTS 400
// CODE STARTS 400
// CODE STARTS 400
// CODE STARTS 400
// CODE STARTS 400
#define _UNICODE
#define UNICODE
#include <Windows.h>
#include <tchar.h>
#include <Winternl.h>
#include <ShlObj.h>
#include <strsafe.h>
#include <Shlwapi.h>
#include <Wbemidl.h>
#include <devguid.h>
#include <winioctl.h>
#include <intrin.h>
#include <SetupAPI.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <winnt.h>
#include <thread>
#include <Psapi.h>
#include <TlHelp32.h>
#include <IPTypes.h>
#include <iphlpapi.h>
#include <comutil.h>
#include <array>
#include <cstdarg>
#include <utility>
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "WbemUuid.lib")
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "comsuppw.lib")

#define	MALLOC(x)	HeapAlloc(GetProcessHeap(), 0, x)
#define FREE(x)		HeapFree(GetProcessHeap(), 0, x)
#define STATUS_NO_YIELD_PERFORMED 0x40000024 
#define DEBUG_READ_EVENT 0x0001
#define DEBUG_PROCESS_ASSIGN 0x0002
#define DEBUG_SET_INFORMATION 0x0004
#define DEBUG_QUERY_INFORMATION 0x0008
#define DEBUG_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | \
    DEBUG_READ_EVENT | DEBUG_PROCESS_ASSIGN | DEBUG_SET_INFORMATION | \
    DEBUG_QUERY_INFORMATION)

#if _WIN32 || _WIN64
#if _WIN64
#define ENV64BIT
#else
#define ENV32BIT
#endif
#endif

typedef struct _OBJECT_TYPE_INFORMATION {
	UNICODE_STRING TypeName;
	ULONG TotalNumberOfHandles;
	ULONG TotalNumberOfObjects;
}OBJECT_TYPE_INFORMATION, * POBJECT_TYPE_INFORMATION;

typedef struct _OBJECT_ALL_INFORMATION {
	ULONG NumberOfObjects;
	OBJECT_TYPE_INFORMATION ObjectTypeInformation[1];
}OBJECT_ALL_INFORMATION, * POBJECT_ALL_INFORMATION;


#ifndef _versionhelpers_H_INCLUDED_
#define _versionhelpers_H_INCLUDED_
#include "winapifamily.h"
#ifdef _MSC_VER
#pragma once
#endif  
#pragma region Application Family
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#include <specstrings.h>    
#if !defined(__midl) && !defined(SORTPP_PASS)
#if (NTDDI_VERSION >= NTDDI_WINXP)
#ifdef __cplusplus
#define VERSIONHELPERAPI inline bool
#else 
#define VERSIONHELPERAPI FORCEINLINE BOOL
#endif
VERSIONHELPERAPI
IsWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor)
{
	OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0 };
	DWORDLONG        const dwlConditionMask = VerSetConditionMask(
		VerSetConditionMask(
			VerSetConditionMask(
				0, VER_MAJORVERSION, VER_GREATER_EQUAL),
			VER_MINORVERSION, VER_GREATER_EQUAL),
		VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);
	osvi.dwMajorVersion = wMajorVersion;
	osvi.dwMinorVersion = wMinorVersion;
	osvi.wServicePackMajor = wServicePackMajor;
	return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask) != FALSE;
}
VERSIONHELPERAPI
IsWindowsVersionOrLesser(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor)
{
	OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, { 0 }, 0, 0 };
	DWORDLONG        const dwlConditionMask = VerSetConditionMask(
		VerSetConditionMask(
			0, VER_MAJORVERSION, VER_EQUAL),
		VER_MINORVERSION, VER_LESS_EQUAL);
	osvi.dwMajorVersion = wMajorVersion;
	osvi.dwMinorVersion = wMinorVersion;
	osvi.wServicePackMajor = wServicePackMajor;
	return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION, dwlConditionMask) != FALSE;
}
VERSIONHELPERAPI
IsWindowsXPOrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 0);
}

VERSIONHELPERAPI
IsWindowsXPSP1OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 1);
}

VERSIONHELPERAPI
IsWindowsXPSP2OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 2);
}

VERSIONHELPERAPI
IsWindowsXPSP3OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 3);
}

VERSIONHELPERAPI
IsWindowsVistaOrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 0);
}

VERSIONHELPERAPI
IsWindowsVistaSP1OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 1);
}

VERSIONHELPERAPI
IsWindowsVistaSP2OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 2);
}

VERSIONHELPERAPI
IsWindows7OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 0);
}

VERSIONHELPERAPI
IsWindows7SP1OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 1);
}

VERSIONHELPERAPI
IsWindows8OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN8), LOBYTE(_WIN32_WINNT_WIN8), 0);
}

VERSIONHELPERAPI
IsWindows8Point1OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINBLUE), LOBYTE(_WIN32_WINNT_WINBLUE), 0);
}

VERSIONHELPERAPI
IsWindows10OrGreater()
{
	return IsWindowsVersionOrGreater(10, 0, 0);
}

VERSIONHELPERAPI
IsWindowsServer()
{
	OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0, 0, VER_NT_WORKSTATION };
	DWORDLONG        const dwlConditionMask = VerSetConditionMask(0, VER_PRODUCT_TYPE, VER_EQUAL);

	return !VerifyVersionInfoW(&osvi, VER_PRODUCT_TYPE, dwlConditionMask);
}
VERSIONHELPERAPI
IsWindowsXPOr2k()
{
	return IsWindowsVersionOrLesser(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 0);
}
#endif
#endif 
#endif 
#pragma endregion
#endif 
#define _WIN32_WINNT_WIN8                   0x0602
#define _WIN32_WINNT_WINBLUE                0x0603
#ifndef _INC_WINAPIFAMILY
#define _INC_WINAPIFAMILY
#if defined(_MSC_VER) && !defined(MOFCOMP_PASS)
#pragma once
#endif 
#define WINAPI_FAMILY_PC_APP      2    
#define WINAPI_FAMILY_PHONE_APP   3     
#define WINAPI_FAMILY_DESKTOP_APP 100   
#define WINAPI_FAMILY_APP  WINAPI_FAMILY_PC_APP
#ifndef WINAPI_FAMILY
#define WINAPI_FAMILY WINAPI_FAMILY_DESKTOP_APP
#endif
#undef WINAPI_PARTITION_DESKTOP   
#undef WINAPI_PARTITION_APP       
#undef WINAPI_PARTITION_PC_APP    
#undef WINAPI_PARTITION_PHONE_APP 
#if WINAPI_FAMILY != WINAPI_FAMILY_DESKTOP_APP && WINAPI_FAMILY != WINAPI_FAMILY_PC_APP && WINAPI_FAMILY != WINAPI_FAMILY_PHONE_APP
#   error Unknown WINAPI_FAMILY value. Was it defined in terms of a WINAPI_PARTITION_* value?
#endif
#define WINAPI_PARTITION_DESKTOP   (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
#define WINAPI_PARTITION_APP       1  
#define WINAPI_PARTITION_PC_APP    (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP || WINAPI_FAMILY == WINAPI_FAMILY_PC_APP)
#define WINAPI_PARTITION_PHONE_APP (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
#define WINAPI_PARTITION_PHONE  WINAPI_PARTITION_PHONE_APP
#define WINAPI_FAMILY_PARTITION(Partitions)     (Partitions)
#define _WINAPI_DEPRECATED_DECLARATION  __declspec(deprecated("This API cannot be used in the context of the caller's application type."))
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP) && !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#   define APP_DEPRECATED_HRESULT    HRESULT _WINAPI_DEPRECATED_DECLARATION
#endif

#endif 
#pragma once

namespace
{
	constexpr int const_atoi(char c)
	{
		return c - '0';
	}
}

#ifdef _MSC_VER
#define ALWAYS_INLINE __forceinline
#else
#define ALWAYS_INLINE __attribute__((always_inline))
#endif

template<typename _string_type, size_t _length>
class _Basic_XorStr
{
	using value_type = typename _string_type::value_type;
	static constexpr auto _length_minus_one = _length - 1;

public:
	constexpr ALWAYS_INLINE _Basic_XorStr(value_type const (&str)[_length])
		: _Basic_XorStr(str, std::make_index_sequence<_length_minus_one>())
	{

	}

	inline auto c_str() const
	{
		decrypt();

		return data;
	}

	inline auto str() const
	{
		decrypt();

		return _string_type(data, data + _length_minus_one);
	}

	inline operator _string_type() const
	{
		return str();
	}

private:
	template<size_t... indices>
	constexpr ALWAYS_INLINE _Basic_XorStr(value_type const (&str)[_length], std::index_sequence<indices...>)
		: data{ crypt(str[indices], indices)..., '\0' },
		encrypted(true)
	{

	}

	static constexpr auto XOR_KEY = static_cast<value_type>(
		const_atoi(__TIME__[7]) +
		const_atoi(__TIME__[6]) * 10 +
		const_atoi(__TIME__[4]) * 60 +
		const_atoi(__TIME__[3]) * 600 +
		const_atoi(__TIME__[1]) * 3600 +
		const_atoi(__TIME__[0]) * 36000
		);

	static ALWAYS_INLINE constexpr auto crypt(value_type c, size_t i)
	{
		return static_cast<value_type>(c ^ (XOR_KEY + i));
	}

	inline void decrypt() const
	{
		if (encrypted)
		{
			for (size_t t = 0; t < _length_minus_one; t++)
			{
				data[t] = crypt(data[t], t);
			}
			encrypted = false;
		}
	}

	mutable value_type data[_length];
	mutable bool encrypted;
};
//---------------------------------------------------------------------------
template<size_t _length>
using XorStrA = class _Basic_XorStr<std::string, _length>;
template<size_t _length>
using XorStrW = class _Basic_XorStr<std::wstring, _length>;
template<size_t _length>
using XorStrU16 = class _Basic_XorStr<std::u16string, _length>;
template<size_t _length>
using XorStrU32 = class _Basic_XorStr<std::u32string, _length>;
//---------------------------------------------------------------------------
template<typename _string_type, size_t _length, size_t _length2>
inline auto operator==(const _Basic_XorStr<_string_type, _length>& lhs, const _Basic_XorStr<_string_type, _length2>& rhs)
{
	static_assert(_length == _length2, "XorStr== different length");

	return _length == _length2 && lhs.str() == rhs.str();
}
//---------------------------------------------------------------------------
template<typename _string_type, size_t _length>
inline auto operator==(const _string_type& lhs, const _Basic_XorStr<_string_type, _length>& rhs)
{
	return lhs.size() == _length && lhs == rhs.str();
}
//---------------------------------------------------------------------------
template<typename _stream_type, typename _string_type, size_t _length>
inline auto& operator<<(_stream_type& lhs, const _Basic_XorStr<_string_type, _length>& rhs)
{
	lhs << rhs.c_str();

	return lhs;
}
//---------------------------------------------------------------------------
template<typename _string_type, size_t _length, size_t _length2>
inline auto operator+(const _Basic_XorStr<_string_type, _length>& lhs, const _Basic_XorStr<_string_type, _length2>& rhs)
{
	return lhs.str() + rhs.str();
}
//---------------------------------------------------------------------------
template<typename _string_type, size_t _length>
inline auto operator+(const _string_type& lhs, const _Basic_XorStr<_string_type, _length>& rhs)
{
	return lhs + rhs.str();
}
template<size_t _length>
constexpr ALWAYS_INLINE auto _xor_(char const (&str)[_length])
{
	return XorStrA<_length>(str);
}
template<size_t _length>
constexpr ALWAYS_INLINE auto _xor_(wchar_t const (&str)[_length])
{
	return XorStrW<_length>(str);
}
template<size_t _length>
constexpr ALWAYS_INLINE auto _xor_(char16_t const (&str)[_length])
{
	return XorStrU16<_length>(str);
}
template<size_t _length>
constexpr ALWAYS_INLINE auto _xor_(char32_t const (&str)[_length])
{
	return XorStrU32<_length>(str);
}


 
 

VOID print_detected(TCHAR* szMsg)
{
	HANDLE nStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO ConsoleScreenBufferInfo;
	SecureZeroMemory(&ConsoleScreenBufferInfo, sizeof(CONSOLE_SCREEN_BUFFER_INFO));

	GetConsoleScreenBufferInfo(nStdHandle, &ConsoleScreenBufferInfo);
	WORD OriginalColors = *(&ConsoleScreenBufferInfo.wAttributes);

	SetConsoleTextAttribute(nStdHandle, 12);
	_tprintf(szMsg);
	SetConsoleTextAttribute(nStdHandle, OriginalColors);
}
void print_last_error(const char* lpszFunction)
{
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();

	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&lpMsgBuf),
		0,
		nullptr);

	std::string strDisplayBuf;
	strDisplayBuf.resize(strlen(reinterpret_cast<LPSTR>(lpMsgBuf)) + strlen(lpszFunction) + 40);
	sprintf_s(&strDisplayBuf[0], strDisplayBuf.size(),
		"%s failed with error %d: %s",
		lpszFunction, dw, reinterpret_cast<LPSTR>(lpMsgBuf));

	std::cout << strDisplayBuf << std::endl;

	LocalFree(lpMsgBuf);
}

// Function to handle const wchar_t* strings
void print_last_error(const wchar_t* lpszFunction)
{
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();

	FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&lpMsgBuf),
		0,
		nullptr);

	std::wstring strDisplayBuf;
	strDisplayBuf.resize(wcslen(reinterpret_cast<LPWSTR>(lpMsgBuf)) + wcslen(lpszFunction) + 40);
	swprintf_s(&strDisplayBuf[0], strDisplayBuf.size(),
		L"%s failed with error %d: %s",
		lpszFunction, dw, reinterpret_cast<LPWSTR>(lpMsgBuf));

	std::wcout << strDisplayBuf << std::endl;

	LocalFree(lpMsgBuf);
}
LPWSTR ascii_to_wide_str(const char* lpMultiByteStr)
{
	int iSizeRequired = MultiByteToWideChar(CP_ACP, 0, lpMultiByteStr, -1, NULL, 0);
	LPWSTR lpWideCharStr = (LPWSTR)malloc(iSizeRequired * sizeof(wchar_t));

	if (lpWideCharStr != NULL)
	{
		MultiByteToWideChar(CP_ACP, 0, lpMultiByteStr, -1, lpWideCharStr, iSizeRequired);
	}

	return lpWideCharStr;
}

CHAR* wide_str_to_multibyte(TCHAR* lpWideStr)
{
	errno_t status;
	int* pRetValue = NULL;
	CHAR* mbchar = NULL;
	size_t sizeInBytes = 0;

	status = wctomb_s(pRetValue, mbchar, sizeInBytes, *lpWideStr);
	return mbchar;
}

DWORD GetProcessIdFromName(LPCTSTR szProcessName)
{
	PROCESSENTRY32 pe32;
	HANDLE hSnapshot = NULL;
	SecureZeroMemory(&pe32, sizeof(PROCESSENTRY32));

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return GetLastError();
	}
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hSnapshot, &pe32) == FALSE)
	{
		DWORD errorCode = GetLastError();
		CloseHandle(hSnapshot);
		return errorCode;
	}

	if (StrCmpI(pe32.szExeFile, szProcessName) == 0)
	{
		CloseHandle(hSnapshot);
		return pe32.th32ProcessID;
	}

	while (Process32Next(hSnapshot, &pe32))
	{
		if (StrCmpI(pe32.szExeFile, szProcessName) == 0)
		{
			CloseHandle(hSnapshot);
			return pe32.th32ProcessID;
		}
	}

	CloseHandle(hSnapshot);
	return ERROR_PROCESS_ABORTED;
}

BOOL InitWMI(IWbemServices** pSvc, IWbemLocator** pLoc)
{
	HRESULT hres;

	// Initialize COM
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres)) {
		CoUninitialize();
		return FALSE;
	}

	// Initialize security
	hres = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
	if (FAILED(hres)) {
		CoUninitialize();
		return FALSE;
	}

	// Create an instance of the WbemLocator interface
	hres = CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(pLoc));
	if (FAILED(hres)) {
		CoUninitialize();
		return FALSE;
	}

	// Connect to the WMI service
	hres = (*pLoc)->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, pSvc);
	if (FAILED(hres)) {
		(*pLoc)->Release();
		CoUninitialize();
		return FALSE;
	}

	// Set security levels on the proxy
	hres = CoSetProxyBlanket(*pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
	if (FAILED(hres)) {
		(*pSvc)->Release();
		(*pLoc)->Release();
		CoUninitialize();
		return FALSE;
	}

	return TRUE;
}

BOOL ExecWMIQuery(IWbemServices** pSvc, IWbemLocator** pLoc, IEnumWbemClassObject** pEnumerator, const wchar_t* szQuery)
{
	// Convert wchar_t* to BSTR
	BSTR bstrQuery = SysAllocString(szQuery);
	if (!bstrQuery) {
		// Memory allocation failed
		return FALSE;
	}

	// Convert query language to BSTR
	BSTR bstrQueryLanguage = SysAllocString(L"WQL");
	if (!bstrQueryLanguage) {
		// Memory allocation failed
		SysFreeString(bstrQuery);
		return FALSE;
	}

	// Execute the query
	HRESULT hres = (*pSvc)->ExecQuery(bstrQueryLanguage, bstrQuery, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, pEnumerator);

	// Free the allocated BSTRs
	SysFreeString(bstrQueryLanguage);
	SysFreeString(bstrQuery);

	if (FAILED(hres))
	{
		TCHAR szErrorMsg[] = _T("ExecQuery");
		print_last_error(szErrorMsg);
		(*pSvc)->Release();
		(*pLoc)->Release();
		CoUninitialize();
		return FALSE;
	}

	return TRUE;
}


BOOL IsWoW64()
{
	typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS fnIsWow64Process;

	BOOL bIsWow64 = FALSE;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "IsWow64Process");

	if (fnIsWow64Process != NULL)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
		{
		}
	}

	return bIsWow64;
}

BOOL Is_RegKeyValueExists(HKEY hKey, TCHAR* lpSubKey, TCHAR* lpValueName, TCHAR* search_str)
{
	HKEY hkResult = FALSE;
	TCHAR lpData[1024] = { 0 };
	DWORD cbData = MAX_PATH;

	if (RegOpenKeyEx(hKey, lpSubKey, NULL, KEY_READ, &hkResult) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hkResult, lpValueName, NULL, NULL, (LPBYTE)lpData, &cbData) == ERROR_SUCCESS)
		{
			if (StrStrI((PCTSTR)lpData, search_str) != NULL)
			{
				RegCloseKey(hkResult);
				return TRUE;
			}
		}
		RegCloseKey(hkResult);
	}
	return FALSE;

}

BOOL Is_RegKeyExists(HKEY hKey, TCHAR* lpSubKey)
{
	HKEY hkResult = FALSE;
	TCHAR lpData[1024] = { 0 };
	DWORD cbData = MAX_PATH;

	if (RegOpenKeyEx(hKey, lpSubKey, NULL, KEY_READ, &hkResult) == ERROR_SUCCESS)
	{
		RegCloseKey(hkResult);
		return TRUE;
	}

	return FALSE;
}

BOOL is_FileExists(TCHAR* szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES) && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

BOOL is_DirectoryExists(TCHAR* szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES) && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

BOOL check_mac_addr(TCHAR* szMac)
{
	BOOL bResult = FALSE;
	PIP_ADAPTER_INFO pAdapterInfo;
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	pAdapterInfo = (PIP_ADAPTER_INFO)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL)
	{
		return -1;
	}

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		FREE(pAdapterInfo);
		pAdapterInfo = (PIP_ADAPTER_INFO)MALLOC(ulOutBufLen);
		if (pAdapterInfo == NULL) {
			return 1;
		}
	}

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_SUCCESS)
	{
		CHAR szMacMultiBytes[4];
		for (int i = 0; i < 4; i++) {
			szMacMultiBytes[i] = (CHAR)szMac[i];
		}

		while (pAdapterInfo)
		{

			if (pAdapterInfo->AddressLength == 6 && !memcmp(szMacMultiBytes, pAdapterInfo->Address, 3))
			{
				bResult = TRUE;
				break;
			}
			pAdapterInfo = pAdapterInfo->Next;
		}
	}

	return bResult;
}

BOOL check_adapter_name(TCHAR* szName)
{
	BOOL bResult = FALSE;
	PIP_ADAPTER_INFO pAdapterInfo;
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	pAdapterInfo = (PIP_ADAPTER_INFO)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL)
	{
		return -1;
	}

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		FREE(pAdapterInfo);
		pAdapterInfo = (PIP_ADAPTER_INFO)MALLOC(ulOutBufLen);
		if (pAdapterInfo == NULL)
		{
			return 1;
		}
	}

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_SUCCESS)
	{
		while (pAdapterInfo)
		{
			if (StrCmpI(ascii_to_wide_str(pAdapterInfo->Description), szName) == 0)
			{
				bResult = TRUE;
				break;
			}
			pAdapterInfo = pAdapterInfo->Next;
		}
	}

	return bResult;
}

BOOL GetOSDisplayString(LPTSTR pszOS)
{
	typedef void (WINAPI* PGNSI)(LPSYSTEM_INFO);
	typedef BOOL(WINAPI* PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;
	PGNSI pGNSI;
	PGPI pGPI;
	BOOL bOsVersionInfoEx;
	DWORD dwType;

	SecureZeroMemory(&si, sizeof(SYSTEM_INFO));
	SecureZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	typedef LONG(WINAPI* tRtlGetVersion)(RTL_OSVERSIONINFOEXW*);
	HMODULE h_NtDll = GetModuleHandleW(L"ntdll.dll");
	tRtlGetVersion f_RtlGetVersion = (tRtlGetVersion)GetProcAddress(h_NtDll, "RtlGetVersion");

	bOsVersionInfoEx = f_RtlGetVersion((RTL_OSVERSIONINFOEXW*)&osvi);

	if (!f_RtlGetVersion)
		return FALSE;

	pGNSI = (PGNSI)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
	if (NULL != pGNSI)
		pGNSI(&si);
	else GetSystemInfo(&si);

	if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId && osvi.dwMajorVersion > 4)
	{
		StringCchCopy(pszOS, MAX_PATH, TEXT("Microsoft "));

		if (osvi.dwMajorVersion == 10)
		{
			if (osvi.dwMinorVersion == 0)
			{
				if (osvi.wProductType == VER_NT_WORKSTATION)
					StringCchCat(pszOS, MAX_PATH, TEXT("Windows 10 "));
				else
					StringCchCat(pszOS, MAX_PATH, TEXT("Windows Server 2016 Technical Preview "));
			}

		}

		else if (osvi.dwMajorVersion == 6)
		{
			if (osvi.dwMinorVersion == 0)
			{
				if (osvi.wProductType == VER_NT_WORKSTATION)
					StringCchCat(pszOS, MAX_PATH, TEXT("Windows Vista "));
				else StringCchCat(pszOS, MAX_PATH, TEXT("Windows Server 2008 "));
			}

			if (osvi.dwMinorVersion == 1)
			{
				if (osvi.wProductType == VER_NT_WORKSTATION)
					StringCchCat(pszOS, MAX_PATH, TEXT("Windows 7 "));
				else StringCchCat(pszOS, MAX_PATH, TEXT("Windows Server 2008 R2 "));
			}


			if (osvi.dwMinorVersion == 2)
			{
				if (osvi.wProductType == VER_NT_WORKSTATION)
					StringCchCat(pszOS, MAX_PATH, TEXT("Windows 8 "));
				else
					StringCchCat(pszOS, MAX_PATH, TEXT("Windows Server 2012"));
			}

			pGPI = (PGPI)GetProcAddress(
				GetModuleHandle(TEXT("kernel32.dll")),
				"GetProductInfo");

			pGPI(osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);

			switch (dwType)
			{
			case PRODUCT_ULTIMATE:
				StringCchCat(pszOS, MAX_PATH, TEXT("Ultimate Edition"));
				break;
			case PRODUCT_PROFESSIONAL:
				StringCchCat(pszOS, MAX_PATH, TEXT("Professional"));
				break;
			case PRODUCT_HOME_PREMIUM:
				StringCchCat(pszOS, MAX_PATH, TEXT("Home Premium Edition"));
				break;
			case PRODUCT_HOME_BASIC:
				StringCchCat(pszOS, MAX_PATH, TEXT("Home Basic Edition"));
				break;
			case PRODUCT_ENTERPRISE:
				StringCchCat(pszOS, MAX_PATH, TEXT("Enterprise Edition"));
				break;
			case PRODUCT_BUSINESS:
				StringCchCat(pszOS, MAX_PATH, TEXT("Business Edition"));
				break;
			case PRODUCT_STARTER:
				StringCchCat(pszOS, MAX_PATH, TEXT("Starter Edition"));
				break;
			case PRODUCT_CLUSTER_SERVER:
				StringCchCat(pszOS, MAX_PATH, TEXT("Cluster Server Edition"));
				break;
			case PRODUCT_DATACENTER_SERVER:
				StringCchCat(pszOS, MAX_PATH, TEXT("Datacenter Edition"));
				break;
			case PRODUCT_DATACENTER_SERVER_CORE:
				StringCchCat(pszOS, MAX_PATH, TEXT("Datacenter Edition (core installation)"));
				break;
			case PRODUCT_ENTERPRISE_SERVER:
				StringCchCat(pszOS, MAX_PATH, TEXT("Enterprise Edition"));
				break;
			case PRODUCT_ENTERPRISE_SERVER_CORE:
				StringCchCat(pszOS, MAX_PATH, TEXT("Enterprise Edition (core installation)"));
				break;
			case PRODUCT_ENTERPRISE_SERVER_IA64:
				StringCchCat(pszOS, MAX_PATH, TEXT("Enterprise Edition for Itanium-based Systems"));
				break;
			case PRODUCT_SMALLBUSINESS_SERVER:
				StringCchCat(pszOS, MAX_PATH, TEXT("Small Business Server"));
				break;
			case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
				StringCchCat(pszOS, MAX_PATH, TEXT("Small Business Server Premium Edition"));
				break;
			case PRODUCT_STANDARD_SERVER:
				StringCchCat(pszOS, MAX_PATH, TEXT("Standard Edition"));
				break;
			case PRODUCT_STANDARD_SERVER_CORE:
				StringCchCat(pszOS, MAX_PATH, TEXT("Standard Edition (core installation)"));
				break;
			case PRODUCT_WEB_SERVER:
				StringCchCat(pszOS, MAX_PATH, TEXT("Web Server Edition"));
				break;
			}
		}

		else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
		{
			if (GetSystemMetrics(SM_SERVERR2))
				StringCchCat(pszOS, MAX_PATH, TEXT("Windows Server 2003 R2, "));
			else if (osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER)
				StringCchCat(pszOS, MAX_PATH, TEXT("Windows Storage Server 2003"));
			else if (osvi.wSuiteMask & VER_SUITE_WH_SERVER)
				StringCchCat(pszOS, MAX_PATH, TEXT("Windows Home Server"));
			else if (osvi.wProductType == VER_NT_WORKSTATION &&
				si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
			{
				StringCchCat(pszOS, MAX_PATH, TEXT("Windows XP Professional x64 Edition"));
			}
			else StringCchCat(pszOS, MAX_PATH, TEXT("Windows Server 2003, "));

			if (osvi.wProductType != VER_NT_WORKSTATION)
			{
				if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
				{
					if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
						StringCchCat(pszOS, MAX_PATH, TEXT("Datacenter Edition for Itanium-based Systems"));
					else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
						StringCchCat(pszOS, MAX_PATH, TEXT("Enterprise Edition for Itanium-based Systems"));
				}

				else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
				{
					if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
						StringCchCat(pszOS, MAX_PATH, TEXT("Datacenter x64 Edition"));
					else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
						StringCchCat(pszOS, MAX_PATH, TEXT("Enterprise x64 Edition"));
					else StringCchCat(pszOS, MAX_PATH, TEXT("Standard x64 Edition"));
				}

				else
				{
					if (osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER)
						StringCchCat(pszOS, MAX_PATH, TEXT("Compute Cluster Edition"));
					else if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
						StringCchCat(pszOS, MAX_PATH, TEXT("Datacenter Edition"));
					else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
						StringCchCat(pszOS, MAX_PATH, TEXT("Enterprise Edition"));
					else if (osvi.wSuiteMask & VER_SUITE_BLADE)
						StringCchCat(pszOS, MAX_PATH, TEXT("Web Edition"));
					else StringCchCat(pszOS, MAX_PATH, TEXT("Standard Edition"));
				}
			}
		}

		else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
		{
			StringCchCat(pszOS, MAX_PATH, TEXT("Windows XP "));
			if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
				StringCchCat(pszOS, MAX_PATH, TEXT("Home Edition"));
			else StringCchCat(pszOS, MAX_PATH, TEXT("Professional"));
		}

		else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
		{
			StringCchCat(pszOS, MAX_PATH, TEXT("Windows 2000 "));

			if (osvi.wProductType == VER_NT_WORKSTATION)
			{
				StringCchCat(pszOS, MAX_PATH, TEXT("Professional"));
			}
			else
			{
				if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
					StringCchCat(pszOS, MAX_PATH, TEXT("Datacenter Server"));
				else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
					StringCchCat(pszOS, MAX_PATH, TEXT("Advanced Server"));
				else StringCchCat(pszOS, MAX_PATH, TEXT("Server"));
			}
		}

		size_t targetSize;
		StringCchLength(osvi.szCSDVersion, MAX_PATH, &targetSize);
		if (targetSize > 0)
		{
			StringCchCat(pszOS, MAX_PATH, TEXT(" "));
			StringCchCat(pszOS, MAX_PATH, osvi.szCSDVersion);
		}

		TCHAR buf[80];

		StringCchPrintf(buf, 80, TEXT(" (build %d)"), osvi.dwBuildNumber);
		StringCchCat(pszOS, MAX_PATH, buf);

		if (osvi.dwMajorVersion >= 6)
		{
			if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
				StringCchCat(pszOS, MAX_PATH, TEXT(" 64-bit"));
			else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
				StringCchCat(pszOS, MAX_PATH, TEXT(" 32-bit"));
		}

		return TRUE;
	}

	else
	{
		return FALSE;
	}
}

DWORD GetProccessIDByName(TCHAR* szProcessNameTarget)
{
	DWORD processIds[1024];
	DWORD dBytesReturned;
	BOOL bStatus;
	HMODULE hMod;
	DWORD cbNeeded;
	TCHAR szProcessName[MAX_PATH] = _T("");

	bStatus = EnumProcesses(processIds, sizeof(processIds), &dBytesReturned);
	if (!bStatus)
	{
	}

	int cProcesses = dBytesReturned / sizeof(DWORD);

	for (int i = 0; i < cProcesses; i++)
	{
		HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, processIds[i]);

		if (hProcess != NULL)
		{
			EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded);
			GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));

			if (StrCmpI(szProcessName, szProcessNameTarget) == 0)
				return processIds[i];

		}
	}

	return FALSE;
}

BOOL SetPrivilege(
	HANDLE hToken,
	LPCTSTR Privilege,
	BOOL bEnablePrivilege
)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;
	TOKEN_PRIVILEGES tpPrevious;
	DWORD cbPrevious = sizeof(TOKEN_PRIVILEGES);

	if (!LookupPrivilegeValue(NULL, Privilege, &luid))
		return FALSE;

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = 0;

	AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		&tpPrevious,
		&cbPrevious
	);

	if (GetLastError() != ERROR_SUCCESS) return FALSE;

	tpPrevious.PrivilegeCount = 1;
	tpPrevious.Privileges[0].Luid = luid;

	if (bEnablePrivilege) {
		tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
	}
	else {
		tpPrevious.Privileges[0].Attributes ^= (SE_PRIVILEGE_ENABLED &
			tpPrevious.Privileges[0].Attributes);
	}

	AdjustTokenPrivileges(hToken, FALSE, &tpPrevious, cbPrevious, NULL, NULL);

	if (GetLastError() != ERROR_SUCCESS) return FALSE;

	return TRUE;
}

BOOL SetDebugPrivileges(VOID) {
	TOKEN_PRIVILEGES priv = { 0 };
	HANDLE hToken = NULL;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		priv.PrivilegeCount = 1;
		priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &priv.Privileges[0].Luid))
			if (AdjustTokenPrivileges(hToken, FALSE, &priv, 0, NULL, NULL) == 0) {
				CloseHandle(hToken);
				return FALSE;
			}

			else
				return TRUE;

		else {
			CloseHandle(hToken);
			return FALSE;
		}
	}

	else
	{
		CloseHandle(hToken);
		return FALSE;
	}
}




ULONG get_idt_base()
{
	UCHAR idtr[6];
	ULONG idt = 0;


#if defined (ENV32BIT)
	_asm sidt idtr
#endif
	idt = *((unsigned long*)&idtr[2]);

	return idt;
}


ULONG get_ldt_base()
{
	UCHAR ldtr[5] = "\xef\xbe\xad\xde";
	ULONG ldt = 0;

#if defined (ENV32BIT)
	_asm sldt ldtr
#endif
	ldt = *((unsigned long*)&ldtr[0]);

	return ldt;
}


ULONG get_gdt_base()
{
	UCHAR gdtr[6];
	ULONG gdt = 0;

#if defined (ENV32BIT)
	_asm sgdt gdtr
#endif
	gdt = *((unsigned long*)&gdtr[2]);

	return gdt;
}


UCHAR* get_str_base()
{
	UCHAR mem[4] = { 0, 0, 0, 0 };

#if defined (ENV32BIT)
	__asm str mem;
#endif

	return mem;
}

BOOL IsElevated()
{
	BOOL fRet = FALSE;
	HANDLE hToken = NULL;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
			fRet = Elevation.TokenIsElevated;
		}
	}
	if (hToken) {
		CloseHandle(hToken);
	}
	return fRet;
}


VOID ErasePEHeaderFromMemory()
{
	DWORD OldProtect = 0;
	char* pBaseAddr = (char*)GetModuleHandle(NULL);
	VirtualProtect(pBaseAddr, 4096, PAGE_READWRITE, &OldProtect);
	SecureZeroMemory(pBaseAddr, 4096);
}
VOID SizeOfImage()
{
#if defined (ENV64BIT)
	PPEB pPeb = (PPEB)__readgsqword(0x60);
#elif defined(ENV32BIT)
	PPEB pPeb = (PPEB)__readfsdword(0x30);
#endif
	PLDR_DATA_TABLE_ENTRY tableEntry = (PLDR_DATA_TABLE_ENTRY)(pPeb->Ldr->InMemoryOrderModuleList.Flink);
	tableEntry->DllBase = (PVOID)((INT_PTR)tableEntry->DllBase + 0x100000);
}


bool loaded_dlls()
{
	HMODULE hDll;

	std::wstring szDlls[] =
	{
		_xor_(_T("sbiedll.dll")),
		_xor_(_T("dbghelp.dll")),
		_xor_(_T("api_log.dll")),
		_xor_(_T("dir_watch.dll")),
		_xor_(_T("pstorec.dll")),
		_xor_(_T("vmcheck.dll")),
		_xor_(_T("wpespy.dll")),
	};

	WORD dwlength = sizeof(szDlls) / sizeof(szDlls[0]);
	for (int i = 0; i < dwlength; i++)
	{
		hDll = GetModuleHandle(szDlls[i].c_str());
		if (hDll != NULL)
			return true;
	}
	return false;
}

BOOL NumberOfProcessors()
{
#if defined (ENV64BIT)
	PULONG ulNumberProcessors = (PULONG)(__readgsqword(0x30) + 0xB8);
#elif defined(ENV32BIT)
	PULONG ulNumberProcessors = (PULONG)(__readfsdword(0x30) + 0x64);
#endif
	if (*ulNumberProcessors < 2)
		return TRUE;
	else
		return FALSE;
}

BOOL idt_trick()
{
	UINT idt_base = get_idt_base();
	if ((idt_base >> 24) == 0xff)
		return TRUE;

	else
		return FALSE;
}

BOOL ldt_trick()
{
	UINT ldt_base = get_ldt_base();

	if (ldt_base == 0xdead0000)
		return FALSE;
	else
		return TRUE;
}

BOOL gdt_trick()
{
	UINT gdt_base = get_gdt_base();

	if ((gdt_base >> 24) == 0xff)
		return TRUE;

	else
		return FALSE;
}

BOOL str_trick()
{
	UCHAR* mem = get_str_base();

	if ((mem[0] == 0x00) && (mem[1] == 0x40))
		return TRUE;
	else
		return FALSE;
}

BOOL number_cores_wmi()
{
	IWbemServices* pSvc = NULL;
	IWbemLocator* pLoc = NULL;
	IEnumWbemClassObject* pEnumerator = NULL;
	BOOL bStatus = FALSE;
	HRESULT hRes;
	BOOL bFound = FALSE;

	bStatus = InitWMI(&pSvc, &pLoc);
	if (bStatus)
	{
		bStatus = ExecWMIQuery(&pSvc, &pLoc, &pEnumerator, _xor_(_T("SELECT * FROM Win32_Processor")).c_str());
		if (bStatus)
		{
			IWbemClassObject* pclsObj = NULL;
			ULONG uReturn = 0;
			VARIANT vtProp;

			while (pEnumerator)
			{
				hRes = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
				if (0 == uReturn)
					break;

				hRes = pclsObj->Get(_xor_(_T("NumberOfCores")).c_str(), 0, &vtProp, 0, 0);
				if (V_VT(&vtProp) != VT_NULL) {

					if (vtProp.uintVal < 2) {
						bFound = TRUE; break;
					}

					VariantClear(&vtProp);
					pclsObj->Release();
				}
			}

			pEnumerator->Release();
			pSvc->Release();
			pLoc->Release();
			CoUninitialize();
		}
	}

	return bFound;
}

BOOL disk_size_wmi()
{
	IWbemServices* pSvc = NULL;
	IWbemLocator* pLoc = NULL;
	IEnumWbemClassObject* pEnumerator = NULL;
	BOOL bStatus = FALSE;
	HRESULT hRes;
	BOOL bFound = FALSE;
	INT64 minHardDiskSize = (80LL * (1024LL * (1024LL * (1024LL))));

	bStatus = InitWMI(&pSvc, &pLoc);
	if (bStatus)
	{
		bStatus = ExecWMIQuery(&pSvc, &pLoc, &pEnumerator, _xor_(_T("SELECT * FROM Win32_LogicalDisk")).c_str());
		if (bStatus)
		{
			IWbemClassObject* pclsObj = NULL;
			ULONG uReturn = 0;
			VARIANT vtProp;

			while (pEnumerator)
			{
				hRes = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
				if (0 == uReturn)
					break;

				hRes = pclsObj->Get(_xor_(_T("Size")).c_str(), 0, &vtProp, 0, 0);
				if (V_VT(&vtProp) != VT_NULL) {

					if (vtProp.llVal < minHardDiskSize) {
						bFound = TRUE; break;
					}

					VariantClear(&vtProp);
					pclsObj->Release();
				}
			}

			pEnumerator->Release();
			pSvc->Release();
			pLoc->Release();
			CoUninitialize();
		}
	}

	return bFound;
}

BOOL dizk_size_deviceiocontrol()
{
	HANDLE hDevice = INVALID_HANDLE_VALUE;
	BOOL bResult = FALSE;
	GET_LENGTH_INFORMATION size = { 0 };
	DWORD lpBytesReturned = 0;
	LONGLONG minHardDiskSize = (80LL * (1024LL * (1024LL * (1024LL))));

	if (!IsElevated() && IsWindowsVistaOrGreater())
		return FALSE;

	hDevice = CreateFile(_xor_(_T("\\\\.\\PhysicalDrive0")).c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hDevice);
		return FALSE;
	}

	bResult = DeviceIoControl(hDevice, IOCTL_DISK_GET_LENGTH_INFO, NULL, 0, &size, sizeof(GET_LENGTH_INFORMATION), &lpBytesReturned, (LPOVERLAPPED)NULL);

	if (bResult != NULL)
	{
		if (size.Length.QuadPart < minHardDiskSize)
			bResult = TRUE;
		else
			bResult = FALSE;
	}

	CloseHandle(hDevice);
	return bResult;
}


BOOL setupdi_diskdrive()
{
	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA DeviceInfoData;
	DWORD i;
	BOOL bFound = FALSE;

	hDevInfo = SetupDiGetClassDevs((LPGUID)&GUID_DEVCLASS_DISKDRIVE, 0, 0, DIGCF_PRESENT);

	if (hDevInfo == INVALID_HANDLE_VALUE)
		return FALSE;

	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	DWORD dwPropertyRegDataType;
	LPTSTR buffer = NULL;
	DWORD dwSize = 0;

	for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
	{
		while (!SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_HARDWAREID, &dwPropertyRegDataType, (PBYTE)buffer, dwSize, &dwSize))
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{

				if (buffer)LocalFree(buffer);

				buffer = (LPTSTR)LocalAlloc(LPTR, dwSize * 2);
			}
			else
				break;
		}

		if ((StrStrI(buffer, _xor_(_T("vbox")).c_str()) != NULL) ||
			(StrStrI(buffer, _xor_(_T("vmware")).c_str()) != NULL) ||
			(StrStrI(buffer, _xor_(_T("qemu")).c_str()) != NULL) ||
			(StrStrI(buffer, _xor_(_T("virtual")).c_str()) != NULL))
		{
			bFound = TRUE;
			break;
		}
	}

	if (buffer)
		LocalFree(buffer);

	if (GetLastError() != NO_ERROR && GetLastError() != ERROR_NO_MORE_ITEMS)
		return FALSE;

	SetupDiDestroyDeviceInfoList(hDevInfo);

	if (bFound)
		return TRUE;

	else
		return FALSE;
}

BOOL memory_space()
{
	DWORDLONG ullMinRam = (1024LL * (1024LL * (1024LL * 1LL)));
	MEMORYSTATUSEX statex = { 0 };

	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);

	return (statex.ullTotalPhys < ullMinRam) ? TRUE : FALSE;
}

BOOL disk_size_getdiskfreespace()
{
	ULONGLONG minHardDiskSize = (80ULL * (1024ULL * (1024ULL * (1024ULL))));
	LPCWSTR pszDrive = NULL;
	BOOL bStatus = FALSE;

	ULARGE_INTEGER totalNumberOfBytes;

	bStatus = GetDiskFreeSpaceEx(pszDrive, NULL, &totalNumberOfBytes, NULL);
	if (bStatus) {
		if (totalNumberOfBytes.QuadPart < minHardDiskSize)  // 80GB
			return TRUE;
	}

	return FALSE;;
}

BOOL accelerated_sleep()
{
	DWORD dwStart = 0, dwEnd = 0, dwDiff = 0;
	DWORD dwMillisecondsToSleep = 60 * 1000;

	dwStart = GetTickCount();

	Sleep(dwMillisecondsToSleep);

	dwEnd = GetTickCount();

	dwDiff = dwEnd - dwStart;
	if (dwDiff > dwMillisecondsToSleep - 1000)
		return FALSE;
	else
		return TRUE;
}

BOOL cpuid_is_hypervisor()
{
	INT CPUInfo[4] = { -1 };

	__cpuid(CPUInfo, 1);
	if ((CPUInfo[2] >> 31) & 1)
		return TRUE;
	else
		return FALSE;
}

BOOL cpuid_hypervisor_vendor()
{
	INT CPUInfo[4] = { -1 };
	CHAR szHypervisorVendor[0x40];
	std::wstring szBlacklistedHypervisors[] =
	{
		_xor_(_T("KVMKVMKVM\0\0\0")),
		_xor_(_T("Microsoft Hv")),
		_xor_(_T("VMwareVMware")),
		_xor_(_T("XenVMMXenVMM")),
		_xor_(_T("prl hyperv  ")),
		_xor_(_T("VBoxVBoxVBox")),
	};
	WORD dwlength = sizeof(szBlacklistedHypervisors) / sizeof(szBlacklistedHypervisors[0]);

	__cpuid(CPUInfo, 0x40000000);
	memset(szHypervisorVendor, 0, sizeof(szHypervisorVendor));
	memcpy(szHypervisorVendor, CPUInfo + 1, 12);

	for (int i = 0; i < dwlength; i++)
	{
		if (_tcscmp(ascii_to_wide_str(szHypervisorVendor), szBlacklistedHypervisors[i].c_str()) == 0)
			return TRUE;
	}

	return FALSE;
}

bool parallels_process()
{
	const wchar_t* szProcesses[] =
	{
		L"prl_cc.exe",
		L"prl_tools.exe",
	};

	WORD iLength = sizeof(szProcesses) / sizeof(szProcesses[0]);
	for (int i = 0; i < iLength; i++)
	{
		if (GetProcessIdFromName(szProcesses[i]))
			return true;
	}
	return false;
}



bool qemu_reg_key_value()
{
	const wchar_t* szEntries[][3] =
	{
		{ L"HARDWARE\\DEVICEMAP\\Scsi\\Scsi Port 0\\Scsi Bus 0\\Target Id 0\\Logical Unit Id 0", L"Identifier", L"QEMU" },
		{ L"HARDWARE\\Description\\System", L"SystemBiosVersion", L"QEMU" },
	};

	WORD dwLength = sizeof(szEntries) / sizeof(szEntries[0]);

	for (int i = 0; i < dwLength; i++)
	{
		if (Is_RegKeyValueExists(HKEY_LOCAL_MACHINE, const_cast<wchar_t*>(szEntries[i][0]), const_cast<wchar_t*>(szEntries[i][1]), const_cast<wchar_t*>(szEntries[i][2])))
			return true;
	}
	return false;
}


bool vbox_reg_key_value()
{
	const wchar_t* szEntries[][3] =
	{
		{ L"HARDWARE\\DEVICEMAP\\Scsi\\Scsi Port 0\\Scsi Bus 0\\Target Id 0\\Logical Unit Id 0", L"Identifier", L"VBOX" },
		{ L"HARDWARE\\Description\\System", L"SystemBiosVersion", L"VBOX" },
		{ L"HARDWARE\\Description\\System", L"VideoBiosVersion", L"VIRTUALBOX" },
		{ L"HARDWARE\\Description\\System", L"SystemBiosDate", L"06/23/99" },
	};

	WORD dwLength = sizeof(szEntries) / sizeof(szEntries[0]);

	for (int i = 0; i < dwLength; i++)
	{
		if (Is_RegKeyValueExists(HKEY_LOCAL_MACHINE, const_cast<wchar_t*>(szEntries[i][0]), const_cast<wchar_t*>(szEntries[i][1]), const_cast<wchar_t*>(szEntries[i][2])))
			return true;
	}
	return false;
}



bool vbox_reg_keys()
{
	const wchar_t* szKeys[] =
	{
	L"HARDWARE\\ACPI\\DSDT\\VBOX__",
	L"HARDWARE\\ACPI\\FADT\\VBOX__",
	L"HARDWARE\\ACPI\\RSDT\\VBOX__",
	L"SOFTWARE\\Oracle\\VirtualBox Guest Additions",
	L"SYSTEM\\ControlSet001\\Services\\VBoxGuest",
	L"SYSTEM\\ControlSet001\\Services\\VBoxMouse",
	L"SYSTEM\\ControlSet001\\Services\\VBoxService",
	L"SYSTEM\\ControlSet001\\Services\\VBoxSF",
	L"SYSTEM\\ControlSet001\\Services\\VBoxVideo",
	L"SYSTEM\\CurrentControlSet\\Services\\vmci",
	L"SYSTEM\\CurrentControlSet\\Services\\vmhgfs",
	L"SYSTEM\\CurrentControlSet\\Services\\vmmouse",
	L"SYSTEM\\CurrentControlSet\\Services\\vmmemctl",
	L"SYSTEM\\CurrentControlSet\\Services\\vmusb",
	L"SYSTEM\\CurrentControlSet\\Services\\vmusbmouse",
	L"SYSTEM\\CurrentControlSet\\Services\\vmx_svga",
	L"SYSTEM\\CurrentControlSet\\Services\\vmxnet",
	L"SYSTEM\\CurrentControlSet\\Services\\vmx86",
	L"SYSTEM\\ControlSet001\\Services\\VMTools",
	L"SYSTEM\\ControlSet001\\Services\\vioscsi",
	L"SYSTEM\\ControlSet001\\Services\\viostor",
	L"SYSTEM\\ControlSet001\\Services\\VirtIO-FS Service",
	L"SYSTEM\\ControlSet001\\Services\\VirtioSerial",
	L"SYSTEM\\ControlSet001\\Services\\BALLOON",
	L"SYSTEM\\ControlSet001\\Services\\BalloonService",
	L"SYSTEM\\ControlSet001\\Services\\netkvm"
	};

	WORD dwlength = sizeof(szKeys) / sizeof(szKeys[0]);

	for (int i = 0; i < dwlength; i++)
	{
		if (Is_RegKeyExists(HKEY_LOCAL_MACHINE, const_cast<wchar_t*>(szKeys[i])))
			return true;
	}
	return false;
}



bool vbox_files()
{
	const wchar_t* szPaths[] =
	{
		L"system32\\drivers\\VBoxMouse.sys",
		L"system32\\drivers\\VBoxGuest.sys",
		L"system32\\drivers\\VBoxSF.sys",
		L"system32\\drivers\\VBoxVideo.sys",
		L"system32\\vboxdisp.dll",
		L"system32\\vboxhook.dll",
		L"system32\\vboxmrxnp.dll",
		L"system32\\vboxogl.dll",
		L"system32\\vboxoglarrayspu.dll",
		L"system32\\vboxoglcrutil.dll",
		L"system32\\vboxoglerrorspu.dll",
		L"system32\\vboxoglfeedbackspu.dll",
		L"system32\\vboxoglpackspu.dll",
		L"system32\\vboxoglpassthroughspu.dll",
		L"system32\\vboxservice.exe",
		L"system32\\vboxtray.exe",
		L"system32\\VBoxControl.exe",
	};

	WORD dwlength = sizeof(szPaths) / sizeof(szPaths[0]);
	wchar_t szWinDir[MAX_PATH] = L"";
	wchar_t szPath[MAX_PATH] = L"";
	GetWindowsDirectoryW(szWinDir, MAX_PATH);

	for (int i = 0; i < dwlength; i++)
	{
		PathCombineW(szPath, szWinDir, szPaths[i]);
		if (is_FileExists(szPath))
			return true;
	}
	return false;
}


BOOL vbox_dir()
{
	TCHAR szProgramFile[MAX_PATH];
	TCHAR szPath[MAX_PATH] = _T("");
	TCHAR szTarget[MAX_PATH] = _T("oracle\\virtualbox guest additions\\");

	if (IsWoW64())
		ExpandEnvironmentStrings(_T("%ProgramW6432%"), szProgramFile, ARRAYSIZE(szProgramFile));
	else
		SHGetSpecialFolderPath(NULL, szProgramFile, CSIDL_PROGRAM_FILES, FALSE);

	PathCombine(szPath, szProgramFile, szTarget);
	return is_DirectoryExists(szPath);
}

BOOL vbox_check_mac()
{
	TCHAR szMac[] = _T("\x08\x00\x27");

	return check_mac_addr(szMac);
}

bool vbox_devices()
{
	const wchar_t* devices[] =
	{
		L"\\\\.\\VBoxMiniRdrDN",
		L"\\\\.\\VBoxGuest",
		L"\\\\.\\pipe\\VBoxMiniRdDN",
		L"\\\\.\\VBoxTrayIPC",
		L"\\\\.\\pipe\\VBoxTrayIPC"
	};

	int iLength = sizeof(devices) / sizeof(devices[0]);

	for (int i = 0; i < iLength; i++)
	{
		HANDLE hFile = CreateFileW(devices[i], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			return true;
		}
	}

	// None of the device files exist, return false
	return false;
}


BOOL vbox_window_class()
{
	HWND hClass = FindWindow(_T("VBoxTrayToolWndClass"), NULL);
	HWND hWindow = FindWindow(NULL, _T("VBoxTrayToolWnd"));

	if (hClass || hWindow)
		return TRUE;
	else
		return FALSE;
}


bool vbox_processes()
{
	const TCHAR* szProcesses[] =
	{
		_T("vboxservice.exe"),
		_T("vboxtray.exe")
	};

	WORD iLength = sizeof(szProcesses) / sizeof(szProcesses[0]);
	for (int i = 0; i < iLength; i++)
	{
		if (GetProcessIdFromName((TCHAR*)szProcesses[i]))
			return true;
	}
	return false;
}

BOOL vbox_devices_wmi()
{
	IWbemServices* pSvc = NULL;
	IWbemLocator* pLoc = NULL;
	IEnumWbemClassObject* pEnumerator = NULL;
	BOOL bStatus = FALSE;
	HRESULT hRes;
	BOOL bFound = FALSE;

	bStatus = InitWMI(&pSvc, &pLoc);

	if (bStatus)
	{
#ifdef UNICODE
		// Unicode version
		const wchar_t* query = L"SELECT * FROM Win32_PnPEntity";
#else
		// ANSI version
		const char* query = "SELECT * FROM Win32_PnPEntity";
#endif
		bStatus = ExecWMIQuery(&pSvc, &pLoc, &pEnumerator, query);
		if (bStatus)
		{
			IWbemClassObject* pclsObj = NULL;
			ULONG uReturn = 0;
			VARIANT vtProp;

			while (pEnumerator)
			{
				hRes = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
				if (0 == uReturn)
					break;

				hRes = pclsObj->Get(_T("DeviceId"), 0, &vtProp, 0, 0);

#ifdef UNICODE
				// Unicode version
				if (wcsstr(vtProp.bstrVal, L"PCI\\VEN_80EE&DEV_CAFE") != 0)
#else
				// ANSI version
				if (strstr(_wcsdup(vtProp.bstrVal), "PCI\\VEN_80EE&DEV_CAFE") != 0)
#endif
				{
					bFound = TRUE;
					break;
				}

				VariantClear(&vtProp);
				pclsObj->Release();
			}

			pSvc->Release();
			pLoc->Release();
			pEnumerator->Release();
			CoUninitialize();
		}
	}

	return bFound;
}

BOOL vbox_mac_wmi()
{
	IWbemServices* pSvc = NULL;
	IWbemLocator* pLoc = NULL;
	IEnumWbemClassObject* pEnumerator = NULL;
	BOOL bStatus = FALSE;
	HRESULT hRes;
	BOOL bFound = FALSE;

	bStatus = InitWMI(&pSvc, &pLoc);
	if (bStatus)
	{
#ifdef UNICODE
		// Unicode version
		bStatus = ExecWMIQuery(&pSvc, &pLoc, &pEnumerator, L"SELECT * FROM Win32_NetworkAdapterConfiguration");
#else
		// ANSI version
		bStatus = ExecWMIQuery(&pSvc, &pLoc, &pEnumerator, "SELECT * FROM Win32_NetworkAdapterConfiguration");
#endif

		if (bStatus)
		{
			IWbemClassObject* pclsObj = NULL;
			ULONG uReturn = 0;
			VARIANT vtProp;

			while (pEnumerator)
			{
				hRes = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
				if (0 == uReturn)
					break;

				hRes = pclsObj->Get(L"MACAddress", 0, &vtProp, 0, 0);
				if (V_VT(&vtProp) != VT_NULL) {

#ifdef UNICODE
					// Unicode version
					if (wcsstr(vtProp.bstrVal, L"08:00:27") != 0) {
#else
					// ANSI version
					if (strstr(_wcsdup(vtProp.bstrVal), "08:00:27") != 0) {
#endif
						bFound = TRUE; break;
					}

					VariantClear(&vtProp);
					pclsObj->Release();
					}
				}

			pEnumerator->Release();
			pSvc->Release();
			pLoc->Release();
			CoUninitialize();
			}
		}

	return bFound;
	}

BOOL vbox_eventlogfile_wmi()
{
	IWbemServices* pSvc = NULL;
	IWbemLocator* pLoc = NULL;
	IEnumWbemClassObject* pEnumerator = NULL;
	BOOL bStatus = FALSE;
	HRESULT hRes;
	BOOL bFound = FALSE;

	bStatus = InitWMI(&pSvc, &pLoc);
	if (bStatus)
	{
		TCHAR szQuery[] = _T("SELECT * FROM Win32_NTEventlogFile");
		bStatus = ExecWMIQuery(&pSvc, &pLoc, &pEnumerator, szQuery); // Passing TCHAR array
		if (bStatus)
		{
			IWbemClassObject* pclsObj = NULL;
			ULONG uReturn = 0;
			VARIANT vtProp;

			while (pEnumerator && !bFound)
			{
				hRes = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
				if (0 == uReturn)
					break;

				hRes = pclsObj->Get(_T("FileName"), 0, &vtProp, 0, 0);
				if (V_VT(&vtProp) != VT_NULL) {

					if (StrCmpI(vtProp.bstrVal, _T("System")) == 0) {

						VariantClear(&vtProp);
						hRes = pclsObj->Get(_T("Sources"), 0, &vtProp, 0, 0);

						SAFEARRAY* saSources = vtProp.parray;
						LONG* pVals;
						HRESULT hr = SafeArrayAccessData(saSources, (VOID**)&pVals);
						if (SUCCEEDED(hr)) {
							LONG lowerBound, upperBound;
							SafeArrayGetLBound(saSources, 1, &lowerBound);
							SafeArrayGetUBound(saSources, 1, &upperBound);
							LONG iLength = upperBound - lowerBound + 1;

							TCHAR* bstrItem;
							for (LONG ix = 0; ix < iLength; ix++) {
								SafeArrayGetElement(saSources, &ix, (void*)&bstrItem);
								if (_tcsicmp(bstrItem, _T("vboxvideo")) == 0) {
									bFound = TRUE;
									break;
								}
							}
						}
					}

					VariantClear(&vtProp);
					pclsObj->Release();
				}
			}

			pEnumerator->Release();
			pSvc->Release();
			pLoc->Release();
			CoUninitialize();
		}
	}

	return bFound;
}

bool virtual_pc_process()
{
	const wchar_t* szProcesses[] =
	{
		L"VMSrvc.exe",
		L"VMUSrvc.exe",
	};

	WORD iLength = sizeof(szProcesses) / sizeof(szProcesses[0]);
	for (int i = 0; i < iLength; i++)
	{
		if (GetProcessIdFromName(const_cast<wchar_t*>(szProcesses[i])))
			return true;
	}
	return false;
}


bool vmware_reg_key_value()
{
	const wchar_t* szEntries[][3] =
	{
		{ L"HARDWARE\\DEVICEMAP\\Scsi\\Scsi Port 0\\Scsi Bus 0\\Target Id 0\\Logical Unit Id 0", L"Identifier", L"VMWARE" },
		{ L"HARDWARE\\DEVICEMAP\\Scsi\\Scsi Port 1\\Scsi Bus 0\\Target Id 0\\Logical Unit Id 0", L"Identifier", L"VMWARE" },
		{ L"HARDWARE\\DEVICEMAP\\Scsi\\Scsi Port 2\\Scsi Bus 0\\Target Id 0\\Logical Unit Id 0", L"Identifier", L"VMWARE" },
	};

	WORD dwLength = sizeof(szEntries) / sizeof(szEntries[0]);

	for (int i = 0; i < dwLength; i++)
	{
		if (Is_RegKeyValueExists(HKEY_LOCAL_MACHINE, const_cast<wchar_t*>(szEntries[i][0]), const_cast<wchar_t*>(szEntries[i][1]), const_cast<wchar_t*>(szEntries[i][2])))
			return true;
	}
	return false;
}

bool vmware_reg_keys()
{
	const wchar_t* szKeys[] =
	{
		L"SOFTWARE\\VMware, Inc.\\VMware Tools",
	};

	WORD dwlength = sizeof(szKeys) / sizeof(szKeys[0]);

	for (int i = 0; i < dwlength; i++)
	{
		if (Is_RegKeyExists(HKEY_LOCAL_MACHINE, const_cast<wchar_t*>(szKeys[i])))
			return true;
	}
	return false;
}

bool vmware_files()
{
	const wchar_t* szPaths[] =
	{
		L"system32\\drivers\\vmmouse.sys",
		L"system32\\drivers\\vmhgfs.sys",
	};

	WORD dwlength = sizeof(szPaths) / sizeof(szPaths[0]);
	wchar_t szWinDir[MAX_PATH] = L"";
	wchar_t szPath[MAX_PATH] = L"";
	GetWindowsDirectoryW(szWinDir, MAX_PATH);

	for (int i = 0; i < dwlength; i++)
	{
		PathCombineW(szPath, szWinDir, szPaths[i]);
		if (is_FileExists(szPath))
			return true;
	}
	return false;
}

BOOL vmware_dir()
{
	TCHAR szProgramFile[MAX_PATH];
	TCHAR szPath[MAX_PATH] = _T("");
	TCHAR szTarget[MAX_PATH] = _T("VMWare\\");

	if (IsWoW64())
		ExpandEnvironmentStrings(_T("%ProgramW6432%"), szProgramFile, ARRAYSIZE(szProgramFile));
	else
		SHGetSpecialFolderPath(NULL, szProgramFile, CSIDL_PROGRAM_FILES, FALSE);

	PathCombine(szPath, szProgramFile, szTarget);
	return is_DirectoryExists(szPath);
}

bool vmware_mac()
{
	const wchar_t* szMac[][2] = {
		{ L"\x00\x05\x69", L"00:05:69" },
		{ L"\x00\x0C\x29", L"00:0c:29" },
		{ L"\x00\x1C\x14", L"00:1C:14" },
	};

	int dwLength = sizeof(szMac) / sizeof(szMac[0]);

	for (int i = 0; i < dwLength; i++)
	{
		if (check_mac_addr((TCHAR*)szMac[i][0]))
		{
			return true;
		}
	}

	return false;
}

BOOL vmware_adapter_name()
{
	const wchar_t* szAdapterName = L"VMWare";
	if (check_adapter_name((TCHAR*)szAdapterName))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool vmware_devices()
{
	const wchar_t* devices[] =
	{
		L"\\\\.\\HGFS",
		L"\\\\.\\vmci",
	};

	WORD iLength = sizeof(devices) / sizeof(devices[0]);
	for (int i = 0; i < iLength; i++)
	{
		HANDLE hFile = CreateFile(devices[i], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile != INVALID_HANDLE_VALUE)
			return true;
	}
	return false;
}
bool vmware_processes()
{
	const wchar_t* szProcesses[] =
	{
		L"vmtoolsd.exe",
		L"vmwaretray.exe",
		L"vmwareuser.exe",
	};

	WORD iLength = sizeof(szProcesses) / sizeof(szProcesses[0]);
	for (int i = 0; i < iLength; i++)
	{
		if (GetProcessIdFromName(szProcesses[i]))
			return true;
	}
	return false;
}


BOOL wine_exports()
{
	HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
	if (hKernel32 == NULL)
	{
		return FALSE; // Unable to get the handle to kernel32.dll
	}

	if (GetProcAddress(hKernel32, "wine_get_unix_file_name") == NULL)
	{
		return FALSE; // Function wine_get_unix_file_name not found
	}

	return TRUE; // Function wine_get_unix_file_name found
}
bool wine_reg_keys()
{
	const TCHAR* szKeys[] =
	{
		_T("SOFTWARE\\Wine")
	};

	WORD dwlength = sizeof(szKeys) / sizeof(szKeys[0]);

	for (int i = 0; i < dwlength; i++)
	{
		if (Is_RegKeyExists(HKEY_CURRENT_USER, (TCHAR*)szKeys[i]))
			return true;
	}
	return false;
}


bool xen_process()
{
	const wchar_t* szProcesses[] =
	{
		L"xenservice.exe",
	};

	WORD iLength = sizeof(szProcesses) / sizeof(szProcesses[0]);
	for (int i = 0; i < iLength; i++)
	{
		if (GetProcessIdFromName(szProcesses[i]))
			return true;
	}
	return false;
}

bool analysis_tools_process()
{
	const wchar_t* szProcesses[] =
	{
		L"ProcessHacker.exe", L"Wireshark.exe", L"HTTPDebugger.exe",
		L"x64dbg.exe", L"x32dbg.exe", L"ollydbg.exe", L"idaq.exe",
		L"idaq64.exe", L"ImmunityDebugger.exe", L"dumpcap.exe",
		L"HookExplorer.exe", L"ImportREC.exe", L"PETools.exe",
		L"LordPE.exe", L"SysInspector.exe", L"CuckooSandbox.exe",
		L"VirusTotalUploader.exe", L"HybridAnalysis.exe",
		L"JoeSandboxAnalyzer.exe", L"Any.Run.exe", L"Malwr.exe",
		L"Valkyrie.exe", L"VMRayAnalyzer.exe", L"joeboxcontrol.exe",
		L"joeboxserver.exe", L"capa.exe", L"tcpview.exe",
		L"autoruns.exe", L"autorunsc.exe", L"filemon.exe",
		L"procmon.exe", L"regmon.exe", L"procexp.exe",
		L"sniff_hit.exe", L"windbg.exe", L"Fiddler.exe",
		L"Charles.exe", L"BurpSuite.exe", L"OWASPZAP.exe",
		L"CheatEngine.exe", L"ArtMoney.exe", L"WPE PRO.exe",
		L"Squalr.exe", L"nmap.exe", L"zenmap.exe",
		L"Netcat.exe", L"tcpdump.exe", L"Wireshark.exe",
		L"Tshark.exe", L"WinPcap.exe", L"npcap.exe",
		L"Boomerang.exe", L"IDA Pro.exe", L"Ghidra.exe",
		L"SnowmanDecompiler.exe", L"Hex-Rays IDA Pro.exe",
		L"RetDec.exe", L"JEB Decompiler.exe", L"radare2.exe",
		L"Binary Ninja.exe", L"ScyllaTest_x86.exe",
		L"ScyllaTest_x64.exe", L"InjectorCLIx64.exe",
		L"InjectorCLIx86.exe", L"die.exe", L"diec.exe",
		L"diel.exe", L"dnSpy.exe", L"dnSpy.Console.exe",
		L"Lunar Engine.exe"
	};

	WORD iLength = sizeof(szProcesses) / sizeof(szProcesses[0]);
	for (int i = 0; i < iLength; i++)
	{
		if (GetProcessIdFromName(szProcesses[i]))
			return true;
	}
	return false;
}

bool IsRemoteSession()
{
	const int session_metrics = GetSystemMetrics(SM_REMOTESESSION);
	return session_metrics != 0;
}


bool check_network_drivers()
{
	HMODULE drivers[1024];
	DWORD cbNeeded;
	int cDrivers, i;

	if (EnumDeviceDrivers((LPVOID*)drivers, sizeof(drivers), &cbNeeded) && cbNeeded < sizeof(drivers))
	{
		TCHAR szDriver[MAX_PATH]; // Adjust size as necessary
		cDrivers = cbNeeded / sizeof(drivers[0]);

		for (i = 0; i < cDrivers; i++)
		{
			if (GetDeviceDriverBaseName(drivers[i], szDriver, sizeof(szDriver) / sizeof(szDriver[0])))
			{
				if (_tcscmp(szDriver, _T("npf.sys")) == 0)
					return true;
			}
		}
	}
	return false;
}


BOOL CheckRemoteDebuggerPresentAPI()
{
	BOOL bIsDbgPresent = FALSE;
	CheckRemoteDebuggerPresent(GetCurrentProcess(), &bIsDbgPresent);
	return bIsDbgPresent;
}


BOOL NtClose_InvalideHandle()
{
	typedef NTSTATUS(WINAPI* pNtClose)(HANDLE);

	pNtClose NtClose_ = NULL;

	HMODULE hNtdll = LoadLibrary(_T("ntdll.dll"));
	if (hNtdll == NULL)
	{
	}

	NtClose_ = (pNtClose)GetProcAddress(hNtdll, "NtClose");
	if (NtClose_ == NULL)
	{
	}

	__try {

		NtClose_((HANDLE)0x99999999);
	}

	__except (EXCEPTION_EXECUTE_HANDLER) {
		return TRUE;
	}

	return FALSE;

}

BOOL CloseHandle_InvalideHandle()
{
	__try {
		CloseHandle((HANDLE)0x99999999);
	}

	__except (EXCEPTION_EXECUTE_HANDLER) {
		return TRUE;
	}

	if (NtClose_InvalideHandle())
		return TRUE;
	else
		return FALSE;
}

BOOL HardwareBreakpoints()
{
	PCONTEXT ctx = PCONTEXT(VirtualAlloc(NULL, sizeof(ctx), MEM_COMMIT, PAGE_READWRITE));
	SecureZeroMemory(ctx, sizeof(CONTEXT));

	ctx->ContextFlags = CONTEXT_DEBUG_REGISTERS;

	if (GetThreadContext(GetCurrentThread(), ctx) == 0)
		return -1;

	if (ctx->Dr0 != 0 || ctx->Dr1 != 0 || ctx->Dr2 != 0 || ctx->Dr3 != 0)
		return TRUE;
	else
		return FALSE;
}


static BOOL SwallowedException = TRUE;

static LONG CALLBACK VectoredHandler(
	_In_ PEXCEPTION_POINTERS ExceptionInfo
)
{
	SwallowedException = FALSE;
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT)
	{
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	return EXCEPTION_CONTINUE_SEARCH;
}




BOOL Interrupt_3()
{
	PVOID Handle = AddVectoredExceptionHandler(1, VectoredHandler);
	SwallowedException = TRUE;
	__debugbreak();
	RemoveVectoredExceptionHandler(Handle);
	return SwallowedException;
}

BOOL IsDebuggerPresentAPI()
{
	if (IsDebuggerPresent())
		return TRUE;
	else
		return FALSE;
}


BOOL MemoryBreakpoints_PageGuard()
{
	UCHAR* pMem = NULL;
	SYSTEM_INFO SystemInfo = { 0 };
	DWORD OldProtect = 0;
	PVOID pAllocation = NULL;

	GetSystemInfo(&SystemInfo);

	pAllocation = VirtualAlloc(NULL, SystemInfo.dwPageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (pAllocation == NULL)
		return FALSE;

	RtlFillMemory(pAllocation, 1, 0xC3);

	if (VirtualProtect(pAllocation, SystemInfo.dwPageSize, PAGE_EXECUTE_READWRITE | PAGE_GUARD, &OldProtect) == 0)
		return FALSE;

	__try
	{
		((void(*)())pAllocation)();
	}
	__except (GetExceptionCode() == STATUS_GUARD_PAGE_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
		VirtualFree(pAllocation, NULL, MEM_RELEASE);
		return FALSE;
	}

	VirtualFree(pAllocation, NULL, MEM_RELEASE);
	return TRUE;
}

BOOL NtQueryInformationProcess_ProcessDebugFlags()
{
	typedef NTSTATUS(WINAPI* pNtQueryInformationProcess)(IN  HANDLE, IN  UINT, OUT PVOID, IN ULONG, OUT PULONG);

	const int ProcessDebugFlags = 0x1f;

	pNtQueryInformationProcess NtQueryInfoProcess = NULL;

	NTSTATUS Status;
	DWORD NoDebugInherit = 0;

	HMODULE hNtDll = LoadLibrary(_T("ntdll.dll"));
	if (hNtDll == NULL)
	{
	}

	NtQueryInfoProcess = (pNtQueryInformationProcess)GetProcAddress(hNtDll, "NtQueryInformationProcess");
	if (NtQueryInfoProcess == NULL)
	{
	}

	Status = NtQueryInfoProcess(GetCurrentProcess(), ProcessDebugFlags, &NoDebugInherit, sizeof(DWORD), NULL);
	if (Status == 0x00000000 && NoDebugInherit == 0)
		return TRUE;
	else
		return FALSE;
}


BOOL NtQueryInformationProcess_ProcessDebugObject()
{
	typedef NTSTATUS(WINAPI* pNtQueryInformationProcess)(IN  HANDLE, IN  UINT, OUT PVOID, IN ULONG, OUT PULONG);

	const int ProcessDebugObjectHandle = 0x1e;

	pNtQueryInformationProcess NtQueryInfoProcess = NULL;

	NTSTATUS Status;
	HANDLE hDebugObject = NULL;

#if defined (ENV64BIT)
	DWORD dProcessInformationLength = sizeof(ULONG) * 2;
	DWORD64 IsRemotePresent = 0;

#elif defined(ENV32BIT)
	DWORD dProcessInformationLength = sizeof(ULONG);
	DWORD32 IsRemotePresent = 0;
#endif

	HMODULE hNtDll = LoadLibrary(_xor_(_T("ntdll.dll")).c_str());
	if (hNtDll == NULL)
	{
	}

	NtQueryInfoProcess = (pNtQueryInformationProcess)GetProcAddress(hNtDll, _xor_("NtQueryInformationProcess").c_str());

	if (NtQueryInfoProcess == NULL)
	{
	}

	Status = NtQueryInfoProcess(GetCurrentProcess(), ProcessDebugObjectHandle, &hDebugObject, dProcessInformationLength, NULL);

	if (Status == 0x00000000 && hDebugObject)
		return TRUE;
	else
		return FALSE;
}


BOOL NtQueryInformationProcess_ProcessDebugPort()
{
	typedef NTSTATUS(WINAPI* pNtQueryInformationProcess)(IN  HANDLE, IN  UINT, OUT PVOID, IN ULONG, OUT PULONG);

	pNtQueryInformationProcess NtQueryInfoProcess = NULL;

	const int ProcessDbgPort = 7;

	NTSTATUS Status;

#if defined (ENV64BIT)
	DWORD dProcessInformationLength = sizeof(ULONG) * 2;
	DWORD64 IsRemotePresent = 0;

#elif defined(ENV32BIT)
	DWORD dProcessInformationLength = sizeof(ULONG);
	DWORD32 IsRemotePresent = 0;
#endif

	HMODULE hNtdll = LoadLibrary(_xor_(_T("ntdll.dll")).c_str());
	if (hNtdll == NULL)
	{
	}

	NtQueryInfoProcess = (pNtQueryInformationProcess)GetProcAddress(hNtdll, _xor_("NtQueryInformationProcess").c_str());

	if (NtQueryInfoProcess == NULL)
		return 0;

	Status = NtQueryInfoProcess(GetCurrentProcess(), ProcessDbgPort, &IsRemotePresent, dProcessInformationLength, NULL);
	if (Status == 0x00000000 && IsRemotePresent != 0)
		return TRUE;
	else
		return FALSE;
}


BOOL NtQueryObject_ObjectAllTypesInformation()
{
	typedef NTSTATUS(WINAPI* pNtQueryObject)(IN HANDLE, IN UINT, OUT PVOID, IN ULONG, OUT PULONG);
	typedef NTSTATUS(WINAPI* pNtCreateDebugObject)(OUT PHANDLE, IN ACCESS_MASK, IN POBJECT_ATTRIBUTES, IN ULONG);

	pNtQueryObject NtQueryObject = NULL;
	pNtCreateDebugObject NtCreateDebugObject = NULL;

	ULONG size;
	PVOID pMemory = NULL;
	POBJECT_ALL_INFORMATION pObjectAllInfo = NULL;
	NTSTATUS Status;

	HMODULE hNtdll = LoadLibrary(_T("ntdll.dll"));
	if (hNtdll == NULL)
	{
		// Handle error
	}

	NtQueryObject = (pNtQueryObject)GetProcAddress(hNtdll, "NtQueryObject");
	if (NtQueryObject == NULL)
	{
		// Handle error
	}

	Status = NtQueryObject(NULL, 3, &size, sizeof(ULONG), &size);

	pMemory = VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (pMemory == NULL)
		return FALSE;

	Status = NtQueryObject((HANDLE)-1, 3, pMemory, size, NULL);

	if (Status != 0x00000000)
	{
		VirtualFree(pMemory, 0, MEM_RELEASE);
		return FALSE;
	}

	pObjectAllInfo = (POBJECT_ALL_INFORMATION)pMemory;
	UCHAR* pObjInfoLocation = (UCHAR*)pObjectAllInfo->ObjectTypeInformation;
	ULONG NumObjects = pObjectAllInfo->NumberOfObjects;

	for (UINT i = 0; i < NumObjects; i++)
	{
		POBJECT_TYPE_INFORMATION pObjectTypeInfo = (POBJECT_TYPE_INFORMATION)pObjInfoLocation;

		if (wcscmp(L"DebugObject", pObjectTypeInfo->TypeName.Buffer) == 0)
		{
			if (pObjectTypeInfo->TotalNumberOfObjects > 0)
			{
				VirtualFree(pMemory, 0, MEM_RELEASE);
				return TRUE;
			}
			else
			{
				VirtualFree(pMemory, 0, MEM_RELEASE);
				return FALSE;
			}
		}

		pObjInfoLocation = (unsigned char*)pObjectTypeInfo->TypeName.Buffer;
		pObjInfoLocation += pObjectTypeInfo->TypeName.MaximumLength;

		ULONG_PTR tmp = ((ULONG_PTR)pObjInfoLocation) & -(int)sizeof(void*);
		if ((ULONG_PTR)tmp != (ULONG_PTR)pObjInfoLocation)
			tmp += sizeof(void*);
		pObjInfoLocation = ((unsigned char*)tmp);
	}

	VirtualFree(pMemory, 0, MEM_RELEASE);
	return FALSE;
}

BOOL NtQueryObject_ObjectTypeInformation()
{
	typedef NTSTATUS(WINAPI* pNtQueryObject)(IN HANDLE, IN UINT, OUT PVOID, IN ULONG, OUT PULONG);
	typedef NTSTATUS(WINAPI* pNtCreateDebugObject)(OUT PHANDLE, IN ACCESS_MASK, IN POBJECT_ATTRIBUTES, IN ULONG);

	pNtQueryObject NtQueryObject = NULL;
	pNtCreateDebugObject NtCreateDebugObject = NULL;

	HANDLE DebugObjectHandle;
	OBJECT_ATTRIBUTES ObjectAttributes;
	InitializeObjectAttributes(&ObjectAttributes, 0, 0, 0, 0);
	BYTE memory[0x1000] = { 0 };
	POBJECT_TYPE_INFORMATION ObjectInformation = (POBJECT_TYPE_INFORMATION)memory;
	NTSTATUS Status;


	HMODULE hNtdll = LoadLibrary(_xor_(_T("ntdll.dll")).c_str());
	if (hNtdll == NULL)
	{
	}

	NtCreateDebugObject = (pNtCreateDebugObject)GetProcAddress(hNtdll, _xor_("NtCreateDebugObject").c_str());
	if (NtCreateDebugObject == NULL)
	{
	}

	NtCreateDebugObject(&DebugObjectHandle, DEBUG_ALL_ACCESS, &ObjectAttributes, FALSE);
	if (NtCreateDebugObject) {

		HMODULE hNtdll = LoadLibrary(_xor_(_T("ntdll.dll")).c_str());
		if (hNtdll == NULL)
		{
		}

		NtQueryObject = (pNtQueryObject)GetProcAddress(hNtdll, _xor_("NtQueryObject").c_str());
		if (NtCreateDebugObject == NULL)
		{
		}

		Status = NtQueryObject(DebugObjectHandle, ObjectTypeInformation, ObjectInformation, sizeof(memory), 0);

		CloseHandle(DebugObjectHandle);


		if (Status >= 0)
		{
			if (ObjectInformation->TotalNumberOfObjects == 0)
				return TRUE;
			else
				return FALSE;
		}
		else
		{
			return FALSE;
		}
	}
	else
		return FALSE;

}


BOOL NtSetInformationThread_ThreadHideFromDebugger()
{
	typedef NTSTATUS(WINAPI* pNtSetInformationThread)(HANDLE, UINT, PVOID, ULONG);

	const int ThreadHideFromDebugger = 0x11;

	pNtSetInformationThread NtSetInformationThread = NULL;

	NTSTATUS Status;
	BOOL IsBeingDebug = FALSE;

	HMODULE hNtDll = LoadLibrary(_xor_(_T("ntdll.dll")).c_str());
	if (hNtDll == NULL)
	{
	}

	NtSetInformationThread = (pNtSetInformationThread)GetProcAddress(hNtDll, _xor_("NtSetInformationThread").c_str());

	if (NtSetInformationThread == NULL)
	{
	}

	Status = NtSetInformationThread(GetCurrentThread(), ThreadHideFromDebugger, NULL, 0);

	if (Status)
		IsBeingDebug = TRUE;

	return IsBeingDebug;
}


BOOL NtYieldExecutionAPI()
{
	typedef NTSTATUS(WINAPI* pNtYieldExecution)();

	pNtYieldExecution NtYieldExecution = NULL;

	HMODULE hNtdll;
	INT iDebugged = 0;

	hNtdll = LoadLibrary(_T("ntdll.dll"));

	if (hNtdll == NULL)
	{
	}


	NtYieldExecution = (pNtYieldExecution)GetProcAddress(hNtdll, "NtYieldExecution");
	if (NtYieldExecution == NULL)
	{
	}


	for (int i = 0; i < 0x20; i++)
	{
		Sleep(0xf);

		if (NtYieldExecution() != STATUS_NO_YIELD_PERFORMED)
			iDebugged++;
	}

	if (iDebugged <= 3)
		return FALSE;
	else
		return TRUE;


}


typedef struct _PROCESS_BASIC_INFO {
	PVOID Reserved1;
	void* PebBaseAddress;
	PVOID Reserved2[2];
	ULONG_PTR UniqueProcessId;
	ULONG_PTR ParentProcessId;
} PROCESS_BASIC_INFO;

DWORD GetExplorerPIDbyShellWindow()
{
	DWORD dwProcessId = 0;

	GetWindowThreadProcessId(GetShellWindow(), &dwProcessId);

	return dwProcessId;
}

DWORD GetParentProcessId()
{
	typedef NTSTATUS(WINAPI* pNtQueryInformationProcess)(HANDLE, UINT, PVOID, ULONG, PULONG);

	pNtQueryInformationProcess NtQueryInfoProcess = NULL;

	NTSTATUS Status = 0;
	_PROCESS_BASIC_INFO pbi;
	SecureZeroMemory(&pbi, sizeof(PROCESS_BASIC_INFORMATION));

	NtQueryInfoProcess = (pNtQueryInformationProcess)GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "NtQueryInformationProcess");

	if (NtQueryInfoProcess == 0)
		return 0;

	Status = NtQueryInfoProcess(GetCurrentProcess(), 0, (PVOID)&pbi, sizeof(PROCESS_BASIC_INFO), 0);

	if (Status != 0x00000000)
		return 0;
	else
		return (DWORD)pbi.ParentProcessId;
}


BOOL IsParentExplorerExe()
{
	DWORD dwExplorerProcessId = GetParentProcessId();
	if (dwExplorerProcessId != GetExplorerPIDbyShellWindow())
		return TRUE;
	else
		return FALSE;
}

BOOL IsDebuggerPresentPEB()
{
#if defined (ENV64BIT)
	PPEB pPeb = (PPEB)__readgsqword(0x60);

#elif defined(ENV32BIT)
	PPEB pPeb = (PPEB)__readfsdword(0x30);

#endif

	if (pPeb->BeingDebugged == 1)
		return TRUE;
	else
		return FALSE;
}


#if defined (ENV64BIT)
PUINT32 GetHeapFlags_x64()
{
	PINT64 pProcessHeap = NULL;
	PUINT32 pHeapFlags = NULL;
	if (IsWindowsVistaOrGreater()) {
		pProcessHeap = (PINT64)(__readgsqword(0x60) + 0x30);
		pHeapFlags = (PUINT32)(*pProcessHeap + 0x70);
	}

	else {
		pProcessHeap = (PINT64)(__readgsqword(0x60) + 0x30);
		pHeapFlags = (PUINT32)(*pProcessHeap + 0x14);
	}

	return pHeapFlags;
}

#elif defined(ENV32BIT)
PUINT32 GetHeapFlags_x86()
{
	PUINT32 pProcessHeap, pHeapFlags = NULL;

	if (IsWindowsVistaOrGreater()) {
		pProcessHeap = (PUINT32)(__readfsdword(0x30) + 0x18);
		pHeapFlags = (PUINT32)(*pProcessHeap + 0x40);
	}

	else {
		pProcessHeap = (PUINT32)(__readfsdword(0x30) + 0x18);
		pHeapFlags = (PUINT32)(*pProcessHeap + 0x0C);
	}

	return pHeapFlags;
}
#endif


BOOL HeapFlags()
{
	PUINT32 pHeapFlags = NULL;

#if defined (ENV64BIT)
	pHeapFlags = GetHeapFlags_x64();

#elif defined(ENV32BIT)
	pHeapFlags = GetHeapFlags_x86();

#endif

	if (*pHeapFlags > 2)
		return TRUE;
	else
		return FALSE;
}


#if defined (ENV64BIT)
PUINT32 GetForceFlags_x64()
{
	PINT64 pProcessHeap = NULL;
	PUINT32 pHeapForceFlags = NULL;
	if (IsWindowsVistaOrGreater()) {
		pProcessHeap = (PINT64)(__readgsqword(0x60) + 0x30);
		pHeapForceFlags = (PUINT32)(*pProcessHeap + 0x74);
	}

	else {
		pProcessHeap = (PINT64)(__readgsqword(0x60) + 0x30);
		pHeapForceFlags = (PUINT32)(*pProcessHeap + 0x18);
	}

	return pHeapForceFlags;
}

#elif defined(ENV32BIT)
PUINT32 GetForceFlags_x86()
{
	PUINT32 pProcessHeap, pHeapForceFlags = NULL;
	if (IsWindowsVistaOrGreater())
	{
		pProcessHeap = (PUINT32)(__readfsdword(0x30) + 0x18);
		pHeapForceFlags = (PUINT32)(*pProcessHeap + 0x44);

	}

	else {
		pProcessHeap = (PUINT32)(__readfsdword(0x30) + 0x18);
		pHeapForceFlags = (PUINT32)(*pProcessHeap + 0x10);
	}

	return pHeapForceFlags;
}
#endif

BOOL HeapForceFlags()
{
	PUINT32 pHeapForceFlags = NULL;

#if defined (ENV64BIT)
	pHeapForceFlags = GetForceFlags_x64();

#elif defined(ENV32BIT)
	pHeapForceFlags = GetForceFlags_x86();

#endif

	if (*pHeapForceFlags > 0)
		return TRUE;
	else
		return FALSE;

}


BOOL NtGlobalFlag()
{
	PDWORD pNtGlobalFlag = NULL, pNtGlobalFlagWoW64 = NULL;

#if defined (ENV64BIT)
	BYTE* _teb64 = (BYTE*)__readgsqword(0x30);
	DWORD64 _peb64 = *(DWORD64*)(_teb64 + 0x60);
	pNtGlobalFlag = (PDWORD)(__readgsqword(0x60) + 0xBC);

#elif defined(ENV32BIT)
	BYTE* _teb32 = (BYTE*)__readfsdword(0x18);
	DWORD _peb32 = *(DWORD*)(_teb32 + 0x30);
	pNtGlobalFlag = (PDWORD)(_peb32 + 0x68);

	if (IsWoW64())
	{
		BYTE* _teb64 = (BYTE*)__readfsdword(0x18) - 0x2000;
		DWORD64 _peb64 = *(DWORD64*)(_teb64 + 0x60);
		pNtGlobalFlagWoW64 = (PDWORD)(_peb64 + 0xBC);
	}
#endif

	bool normalDetected = pNtGlobalFlag && *pNtGlobalFlag & 0x00000070;
	bool wow64Detected = pNtGlobalFlagWoW64 && *pNtGlobalFlagWoW64 & 0x00000070;

	if (normalDetected || wow64Detected)
		return TRUE;
	else
		return FALSE;
}


DWORD GetCsrssProcessId()
{
	if (IsWindowsXPOrGreater())
	{
		typedef DWORD(NTAPI* pCsrGetId)(VOID);

		pCsrGetId CsrGetProcessId = (pCsrGetId)GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "CsrGetProcessId");

		if (CsrGetProcessId)
			return CsrGetProcessId();
		else
			return 0;
	}
	else
		return GetProcessIdFromName(_T("csrss.exe"));
}


BOOL CanOpenCsrss()
{
	HANDLE hCsrss = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, GetCsrssProcessId());
	if (hCsrss != NULL)
	{
		CloseHandle(hCsrss);
		return TRUE;
	}
	else
		return FALSE;
}

BOOL SetHandleInformatiom_ProtectedHandle()
{
	HANDLE hMutex;

	hMutex = CreateMutex(NULL, FALSE, _xor_(_T("Random name")).c_str());

	SetHandleInformation(hMutex, HANDLE_FLAG_PROTECT_FROM_CLOSE, HANDLE_FLAG_PROTECT_FROM_CLOSE);

	__try {
		CloseHandle(hMutex);
	}

	__except (HANDLE_FLAG_PROTECT_FROM_CLOSE) {
		return TRUE;
	}

	return FALSE;
}


VOID My_Critical_Function()
{
	int a = 1;
	int b = 2;
	int c = a + b;
}


VOID Myfunction_Adresss_Next()
{
	My_Critical_Function();
};

BOOL SoftwareBreakpoints()
{
	size_t sSizeToCheck = (size_t)(Myfunction_Adresss_Next)-(size_t)(My_Critical_Function);
	PUCHAR Critical_Function = (PUCHAR)My_Critical_Function;

	for (size_t i = 0; i < sSizeToCheck; i++) {
		if (Critical_Function[i] == 0xCC)
			return TRUE;
	}
	return FALSE;
}

BOOL bIsBeinDbg = TRUE;

LONG WINAPI UnhandledExcepFilter(PEXCEPTION_POINTERS pExcepPointers)
{
	bIsBeinDbg = FALSE;
	return EXCEPTION_CONTINUE_EXECUTION;
}


BOOL UnhandledExcepFilterTest()
{
	LPTOP_LEVEL_EXCEPTION_FILTER Top = SetUnhandledExceptionFilter(UnhandledExcepFilter);
	RaiseException(EXCEPTION_FLT_DIVIDE_BY_ZERO, 0, 0, NULL);
	SetUnhandledExceptionFilter(Top);
	return bIsBeinDbg;
}

#define DbgBreakPoint_FUNC_SIZE 0x2
#define DbgUiRemoteBreakin_FUNC_SIZE 0x54
#define NtContinue_FUNC_SIZE 0x18

struct FUNC {
	const char* name;
	FARPROC addr;
	SIZE_T size;
};

FUNC funcList[] = {
	{ _xor_("DbgBreakPoint").c_str(), 0, DbgBreakPoint_FUNC_SIZE },
	{ _xor_("DbgUiRemoteBreakin").c_str(), 0, DbgUiRemoteBreakin_FUNC_SIZE },
	{ _xor_("NtContinue").c_str(), 0, NtContinue_FUNC_SIZE }
};


int AntiAttach() {
	while (true)
	{
		DWORD pid = GetCurrentProcessId();

		WCHAR modName[MAX_PATH] = { 0 };
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);

		HMODULE hMod = LoadLibraryW(reinterpret_cast<const wchar_t*>(_xor_("ntdll.dll").c_str()));
		for (int i = 0; i < _countof(funcList); ++i) {
			funcList[i].addr = GetProcAddress(hMod, funcList[i].name);
		}

		bool result = false;
		auto base_address = GetModuleHandleA(0);
		if (wcsstr((WCHAR*)base_address, _xor_(L"ntdll").c_str()) || wcsstr((WCHAR*)base_address, _xor_(L"NTDLL").c_str())) {
			for (int i = 0; i < _countof(funcList); ++i) {
				DWORD dwOldProtect;

				VirtualProtectEx(hProcess, funcList[i].addr, funcList[i].size, PAGE_EXECUTE_READWRITE, &dwOldProtect);
				result = WriteProcessMemory(hProcess, funcList[i].addr, funcList[i].addr, funcList[i].size, NULL);
				VirtualProtectEx(hProcess, funcList[i].addr, funcList[i].size, dwOldProtect, NULL);

				if (!result) break;
			}
		}

		CloseHandle(hProcess);
	}
}

bool CodeIntCheck()
{
	typedef NTSTATUS(WINAPI* FN_NtQuerySystemInformation)(
		ULONG SystemInformationClass,
		PVOID SystemInformation,
		ULONG SystemInformationLength,
		PULONG ReturnLength
		);

	FN_NtQuerySystemInformation NtQuerySystemInformation = (FN_NtQuerySystemInformation)GetProcAddress(
		GetModuleHandle(L"ntdll.dll"), "NtQuerySystemInformation");

	if (!NtQuerySystemInformation)
	{
		// Failed to get NtQuerySystemInformation function pointer
		return false;
	}

	struct SYSTEM_CODEINTEGRITY_INFORMATION
	{
		ULONG Length;
		ULONG CodeIntegrityOptions;
	};

	SYSTEM_CODEINTEGRITY_INFORMATION cInfo{};
	cInfo.Length = sizeof(cInfo); // set length and don't work without this

	NtQuerySystemInformation(
		103, // SystemCodeIntegrityInformation
		&cInfo,
		sizeof(cInfo),
		nullptr
	);

	bool bResult = (cInfo.CodeIntegrityOptions & 0x02) || (cInfo.CodeIntegrityOptions & 0x08);
	if (bResult)
	{
		return 0;
	}

	return bResult;
}

bool Registry()
{
	bool bRet = false;
	char RegKey[_MAX_PATH] = { 0 };
	DWORD BufSize = _MAX_PATH;
	DWORD dataType = REG_SZ;

	HKEY hKey;

	auto openResult = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control", NULL, KEY_QUERY_VALUE, &hKey);
	if (openResult == ERROR_SUCCESS)
	{
		auto valSystemOpthion = RegQueryValueExA(hKey, "SystemStartOptions", NULL, &dataType, (LPBYTE)&RegKey, &BufSize);
		if (valSystemOpthion == ERROR_SUCCESS)
		{
			if (strstr(RegKey, "TESTSIGNING") != nullptr)
			{
				bRet = true;
				return 0;
			}
		}
		RegCloseKey(hKey);
	}

	return bRet;
}


VOID WINAPI tls_callback(PVOID hModule, DWORD dwReason, PVOID pContext)
{
	if (dwReason == DLL_THREAD_ATTACH)
	{

	}

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		//SizeOfImage();
		//ErasePEHeaderFromMemory();
		//bsod();
		//bsod2();
		//exit(0);
		//std::exit(EXIT_FAILURE);
	}
}

#ifdef _WIN64
#pragma comment (linker, "/INCLUDE:_tls_used")
#pragma comment (linker, "/INCLUDE:tls_callback_func")
#else
#pragma comment (linker, "/INCLUDE:__tls_used")
#pragma comment (linker, "/INCLUDE:_tls_callback_func")
#endif


#ifdef _WIN64
#pragma const_seg(".CRT$XLF")
EXTERN_C const
#else
#pragma data_seg(".CRT$XLF")
EXTERN_C
#endif

PIMAGE_TLS_CALLBACK tls_callback_func = tls_callback;

#ifdef _WIN64
#pragma const_seg()
#else
#pragma data_seg()
#endif

BOOL IsAdministrator(VOID)
{
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup;
	if (!AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &AdministratorsGroup))
	{
		return FALSE;
	}
	BOOL IsInAdminGroup = FALSE;
	if (!CheckTokenMembership(NULL, AdministratorsGroup, &IsInAdminGroup))
	{
		IsInAdminGroup = FALSE;
	}
	FreeSid(AdministratorsGroup);
	return IsInAdminGroup;
}
typedef wchar_t* LPWSTR;
typedef const wchar_t* LPCWSTR;

// concat fonksiyonunu güncelleyin
LPWSTR concat(LPCWSTR str1, LPCWSTR str2) {
	size_t length1 = wcslen(str1);
	size_t length2 = wcslen(str2);
	// Toplam uzunluk + 1 (null terminatör)
	LPWSTR result = (LPWSTR)malloc((length1 + length2 + 1) * sizeof(wchar_t));
	if (result == NULL) {
		// Hata kontrolü
		return NULL;
	}
	wcscpy(result, str1);
	wcscat(result, str2);
	return result;
}

LPWSTR randomstr(int length) {
	LPWSTR str = (LPWSTR)malloc((length + 1) * sizeof(wchar_t));
	if (str == NULL) {
		// Hata kontrolü
		return NULL;
	}

	const wchar_t charset[] = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	const int charset_size = sizeof(charset) / sizeof(wchar_t) - 1; // Son null karakteri çýkar

	for (int i = 0; i < length; i++) {
		int index = rand() % charset_size;
		str[i] = charset[index];
	}
	str[length] = L'\0';

	LPWSTR concatenated = concat(L":n", str);
	free(str); // Geçici belleði serbest býrakýn
	return concatenated;
}
// Specify the target process directly
const char* lpTargetProcess = "C:\\Windows\\System32\\attrib.exe";
BOOL renameDataStream(HANDLE hHandle)
{
	// rename the associated HANDLE's file name
	FILE_RENAME_INFO* fRename;
	LPWSTR lpwStream = randomstr(7);//L":newads"
	DWORD bslpwStream = (wcslen(lpwStream)) * sizeof(wchar_t);

	DWORD bsfRename = sizeof(FILE_RENAME_INFO) + bslpwStream;
	fRename = (FILE_RENAME_INFO*)malloc(bsfRename);
	memset(fRename, 0, bsfRename);
	fRename->FileNameLength = bslpwStream;
	RtlMoveMemory(fRename->FileName, lpwStream, bslpwStream);
	//printf(CALLBACK_OUTPUT, "\nbsfRename: %d; FileNameLength: %d; FileName: %ls\n", bsfRename, fRename->FileNameLength, fRename->FileName);
	BOOL status = SetFileInformationByHandle(hHandle, FileRenameInfo, fRename, bsfRename);
	if (!status) {
		printf("SetFileInformationByHandle's GetLastError() = %d\n", GetLastError());
	}
	free(fRename);
	return status;
}


BOOL setDeletionAttribute(HANDLE hHandle)
{
	FILE_DISPOSITION_INFO fDelete;
	memset(&fDelete, 0, sizeof(fDelete));

	fDelete.DeleteFile = TRUE;

	return SetFileInformationByHandle(hHandle, FileDispositionInfo, &fDelete, sizeof(fDelete));
}


BOOL self_deletion(LPSTR cPath) {
	// Ýlk CreateFileA çaðrýsý
	HANDLE hCurrent = CreateFileA(
		cPath,                 // Dosya yolu (ANSI)
		DELETE,                // Eriþim haklarý
		0,                     // Paylaþým modlarý
		NULL,                  // Güvenlik nitelikleri
		OPEN_EXISTING,         // Dosya oluþturma eylemi
		FILE_ATTRIBUTE_NORMAL, // Dosya özellikleri
		NULL                   // Þablon dosyasý
	);

	if (hCurrent == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	// Data stream yeniden adlandýrma
	BOOL returnedHandleRename = renameDataStream(hCurrent);
	if (!returnedHandleRename) {
		CloseHandle(hCurrent);
		return FALSE;
	}
	else {
		CloseHandle(hCurrent);
	}

	// Ýkinci CreateFileA çaðrýsý
	hCurrent = CreateFileA(
		cPath,
		DELETE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (hCurrent == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	// Silme özelliðini ayarlama
	if (!setDeletionAttribute(hCurrent)) {
		CloseHandle(hCurrent);
		return FALSE;
	}
	else {
		CloseHandle(hCurrent);
	}

	return TRUE;
}
BOOL self_deletion_byself() {
	wchar_t wcPath[MAX_PATH + 1];
	char cPath[MAX_PATH + 1];
	memset(wcPath, 0, sizeof(wcPath));
	memset(cPath, 0, sizeof(cPath));

	if (GetModuleFileNameW(NULL, wcPath, MAX_PATH) == 0) {
		//printf("Failed to get current module handle.\n");
		return 0;	
	}
	else {
		//wprintf(L"[+] Obtained a handle to current module file handle.\n");
		//wprintf(L"[+] Current file path: %ls\n", wcPath);
	}

	// Geniş karakter dizisini çok baytlı karakter dizisine dönüştür
	int ret = WideCharToMultiByte(CP_ACP, 0, wcPath, -1, cPath, MAX_PATH, NULL, NULL);
	if (ret == 0) {
		//printf("Failed to convert wide char to multi-byte.\n");
		return 0;
	}

	// Çok baytlı diziyi self_deletion fonksiyonuna gönder
	return self_deletion(cPath);
}

// Namespace for Protection Management
namespace codex {
	 
    // Function to Initialize Protections
	void Protect() {
		//std::string api_key = "d232b07ffcb4dfa1ab1273945a21f126";
		std::cout << "protected by codex protection\n";
		std::string("protected by codex");
		std::string("protection by codex");

		//if (!AuthLib::Auth::initialize(api_key)) {
		//	//std::cerr << "Failed to initialize Auth library." << std::endl;
		//	exit(0);
		//}

		//std::string authMessage;
		//bool isAuthenticated = AuthLib::Auth::login(license_key, authMessage);

		////std::cout << authMessage << std::endl;

		//if (isAuthenticated) {
 
			//self_deletion_byself();
			multihide((LPTHREAD_START_ROUTINE)AntiAttach);
			multihide((LPTHREAD_START_ROUTINE)loaded_dlls);
			multihide((LPTHREAD_START_ROUTINE)NumberOfProcessors);
			multihide((LPTHREAD_START_ROUTINE)analysis_tools_process);
			multihide((LPTHREAD_START_ROUTINE)IsRemoteSession);
			multihide((LPTHREAD_START_ROUTINE)CheckRemoteDebuggerPresentAPI);
			multihide((LPTHREAD_START_ROUTINE)NtClose_InvalideHandle);
			multihide((LPTHREAD_START_ROUTINE)HardwareBreakpoints);
			multihide((LPTHREAD_START_ROUTINE)Interrupt_3);
			multihide((LPTHREAD_START_ROUTINE)IsDebuggerPresentAPI);
			multihide((LPTHREAD_START_ROUTINE)MemoryBreakpoints_PageGuard);
			multihide((LPTHREAD_START_ROUTINE)NtQueryInformationProcess_ProcessDebugFlags);
			multihide((LPTHREAD_START_ROUTINE)NtQueryInformationProcess_ProcessDebugObject);
			multihide((LPTHREAD_START_ROUTINE)NtQueryInformationProcess_ProcessDebugPort);
			multihide((LPTHREAD_START_ROUTINE)NtSetInformationThread_ThreadHideFromDebugger);
			multihide((LPTHREAD_START_ROUTINE)NtYieldExecutionAPI);
			multihide((LPTHREAD_START_ROUTINE)GetHeapFlags_x64);
			multihide((LPTHREAD_START_ROUTINE)CodeIntCheck);
			multihide((LPTHREAD_START_ROUTINE)WorkerFunction);

		//}
		//else {
		//	exit(0);
		//}
 	//	 
 
 

 
    }

    // Function to Cleanup Protections
    void Cleanup() {
        // Stop Worker Thread
        StopWorker();

        // Unhook WriteProcessMemory (Optional)
        UnhookWriteProcessMemoryFunction();

        // Join Monitor Thread
        if (monitorThread.joinable()) {
            monitorThread.join();
        }
    }
}

// TLS Callback Declarations
#ifdef _WIN64
#pragma const_seg(".CRT$XLB")
EXTERN_C const
#else
#pragma data_seg(".CRT$XLB")
EXTERN_C
#endif

// Declare multiple TLS callbacks
PIMAGE_TLS_CALLBACK tls_callback_func1 = (PIMAGE_TLS_CALLBACK)tls_callback1;
PIMAGE_TLS_CALLBACK tls_callback_func2 = (PIMAGE_TLS_CALLBACK)tls_callback_static;

// End section declaration
#ifdef _WIN64
#pragma const_seg()
#else
#pragma data_seg()
#endif //_WIN64

// First TLS Callback Function
void tls_callback1(PVOID hModule, DWORD dwReason, PVOID pContext)
{
    if (dwReason == DLL_THREAD_ATTACH)
    {
 

        // Additional thread attach logic (if any)
    }

    if (dwReason == DLL_PROCESS_ATTACH)
    {
 
  
 

 
    }
}

// Second (Static) TLS Callback Function
void tls_callback_static(PVOID hModule, DWORD dwReason, PVOID pContext)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
	
    }

    if (dwReason == DLL_THREAD_ATTACH)
    {
 

        // Additional thread attach logic (if any)
    }
}

// Dynamic TLS Callback Function (Not in PE Headers)
void tls_callback_secret(PVOID hModule, DWORD dwReason, PVOID pContext)
{
    if (dwReason == DLL_THREAD_ATTACH)
    {
 

        // Additional thread attach logic (if any)
    }

    if (dwReason == DLL_PROCESS_ATTACH)
    {
        // Start Worker Function in Hidden Thread
        multihide((LPTHREAD_START_ROUTINE)WorkerFunction);
    }
}

#pragma warning(pop)

