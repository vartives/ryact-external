#pragma once

#include <Windows.h>
#include "import_spoof.h"
#include "string_hider.h"
#include "../calls.h"


BOOL(*_GetCursorPos)(LPPOINT lpPoint) = nullptr;
BOOL(*_SetLayeredWindowAttributes)(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags) = nullptr;
LRESULT(*_DefWindowProc)(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam) = nullptr;
HWND(*_CreateWindowExA)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) = nullptr;
LONG(*_SetWindowLongA)(HWND hwnd, int nIndex, LONG dwNewLong) = nullptr;
LONG(*_GetWindowLongA)(HWND hwnd, int nIndex) = nullptr;
ATOM(*_RegisterClassExA)(const WNDCLASSEXA*) = nullptr;
int(*_GetSystemMetrics)(int nIndex) = nullptr;
HWND(*_FindWindowA)(LPCSTR lpClassName, LPCSTR lpWindowName) = nullptr;
SHORT(*_GetAsyncKeyState)(int vKey) = nullptr;
HWND(*_GetForegroundWindow)() = nullptr;
BOOL(*_MoveWindow)(HWND hWnd, int X, int Y, int nWidth, int nHeight, bool bRepaint) = nullptr;
BOOL(*_PeekMessageA)(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg) = nullptr;
BOOL(*_TranslateMessage)(const MSG* lpMsg) = nullptr;
LRESULT(*_DispatchMessageA)(const MSG* lpMsg) = nullptr;
HANDLE(*_CreateThread)(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE     lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId) = nullptr;
BOOL(*_CloseHandle)(HANDLE hObject) = nullptr;
BOOL(*_SetWindowPos)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) = nullptr;
BOOL(*_ShowWindow)(HWND hWnd, int nCmdShow) = nullptr;
BOOL(*_SetWindowDisplayAffinity)(HWND hWnd, DWORD dwAffinity) = nullptr;
BOOL(*_mouse_event)(DWORD, DWORD, DWORD, DWORD, ULONG_PTR) = nullptr;
BOOL(*_UpdateWindow)(HWND) = nullptr;
BOOL(*_GetClientRect)(HWND, LPRECT) = nullptr;
BOOL(*_ClientToScreen)(HWND, LPPOINT) = nullptr;

void Star_Mouse()
{
	SPOOF_FUNC;

	HMODULE user32 = IMPORT(LoadLibraryA).get()(HIDE("user32.dll"));
	HMODULE kernel32 = IMPORT(LoadLibraryA).get()(HIDE("Kernel32.dll"));
	HMODULE win32u = IMPORT(LoadLibraryA).get()(HIDE("win32u.dll"));
	HMODULE dwmapi = IMPORT(LoadLibraryA).get()(HIDE("dwmapi.dll"));

	if (!win32u) {
		SPOOF_FUNC;
		return;
	}

	if (!user32) {
		SPOOF_FUNC;
		return;
	}

	if (!dwmapi) {
		SPOOF_FUNC;
		return;
	}

	if (!kernel32) {
		SPOOF_FUNC;
		return;
	}

	void* NtUserInjectMouseInputAddress = (void*)IMPORT(GetProcAddress).get()(win32u, HIDE("NtUserInjectMouseInput"));
	void* GetCursorPos = (void*)IMPORT(GetProcAddress).get()(user32, HIDE("GetCursorPos"));
	void* SetLayeredWindowAttributes = (void*)IMPORT(GetProcAddress).get()(user32, HIDE("SetLayeredWindowAttributes"));
	void* DwmExtendFrameIntoClientArea = (void*)IMPORT(GetProcAddress).get()(dwmapi, HIDE("DwmExtendFrameIntoClientArea"));
	void* DefWindowProcA = (void*)IMPORT(GetProcAddress).get()(user32, HIDE("DefWindowProcA"));
	void* CreateWindowExA = (void*)IMPORT(GetProcAddress).get()(user32, HIDE("CreateWindowExA"));
	void* SetWindowLongA = (void*)IMPORT(GetProcAddress).get()(user32, HIDE("SetWindowLongA"));
	void* GetWindowLongA = (void*)IMPORT(GetProcAddress).get()(user32, HIDE("GetWindowLongA"));
	void* RegisterClassExA = (void*)IMPORT(GetProcAddress).get()(user32, HIDE("RegisterClassExA"));
	void* GetSystemMetrics = (void*)IMPORT(GetProcAddress).get()(user32, HIDE("GetSystemMetrics"));
	void* FindWindowA = (void*)IMPORT(GetProcAddress).get()(user32, HIDE("FindWindowA"));
	void* GetAsyncKeyState = (void*)IMPORT(GetProcAddress).get()(user32, HIDE("GetAsyncKeyState"));
	void* GetForegroundWindow = (void*)IMPORT(GetProcAddress).get()(user32, HIDE("GetForegroundWindow"));
	void* MoveWindow = (void*)IMPORT(GetProcAddress).get()(user32, HIDE("MoveWindow"));
	void* PeekMessageA = (void*)IMPORT(GetProcAddress).get()(user32, HIDE("PeekMessageA"));
	void* TranslateMessage = (void*)IMPORT(GetProcAddress).get()(user32, HIDE("TranslateMessage"));
	void* DispatchMessageA = (void*)IMPORT(GetProcAddress).get()(user32, HIDE("DispatchMessageA"));
	void* CreateThread = (void*)IMPORT(GetProcAddress).get()(kernel32, HIDE("CreateThread"));
	void* SetWindowPos = (void*)IMPORT(GetProcAddress).get()(user32, HIDE("SetWindowPos"));
	void* CloseHandle = (void*)IMPORT(GetProcAddress).get()(kernel32, HIDE("CloseHandle"));
	void* ShowWindow = (void*)IMPORT(GetProcAddress).get()(user32, HIDE("ShowWindow"));
	void* SetWindowDisplayAffinity = (void*)IMPORT(GetProcAddress).get()(user32, HIDE("SetWindowDisplayAffinity"));
	void* mouzeevent = (void*)IMPORT(GetProcAddress).get()(user32, HIDE("mouse_event"));
	void* updetewindow = (void*)IMPORT(GetProcAddress).get()(user32, HIDE("UpdateWindow"));
	void* clientrect = (void*)IMPORT(GetProcAddress).get()(user32, HIDE("GetClientRect"));
	void* clienttoscreen = (void*)IMPORT(GetProcAddress).get()(user32, HIDE("ClientToScreen"));

	if (!NtUserInjectMouseInputAddress) {
		SPOOF_FUNC;
		return;
	}

	if (!GetCursorPos) {
		SPOOF_FUNC;
		return;
	}

	if (!SetLayeredWindowAttributes) {
		SPOOF_FUNC;
		return;
	}

	if (!DwmExtendFrameIntoClientArea) {
		SPOOF_FUNC;
		return;
	}

	if (!DefWindowProcA) {
		SPOOF_FUNC;
		return;
	}

	if (!CreateWindowExA) {
		SPOOF_FUNC;
		return;
	}

	if (!SetWindowLongA) {
		SPOOF_FUNC;
		return;
	}

	if (!GetWindowLongA) {
		SPOOF_FUNC;
		return;
	}

	if (!RegisterClassExA) {
		SPOOF_FUNC;
		return;
	}

	if (!GetSystemMetrics) {
		SPOOF_FUNC;
		return;
	}

	if (!FindWindowA) {
		SPOOF_FUNC;
		return;
	}

	if (!GetAsyncKeyState) {
		SPOOF_FUNC;
		return;
	}

	if (!GetForegroundWindow) {
		SPOOF_FUNC;
		return;
	}

	if (!MoveWindow) {
		SPOOF_FUNC;
		return;
	}

	if (!PeekMessageA) {
		SPOOF_FUNC;
		return;
	}

	if (!TranslateMessage) {
		SPOOF_FUNC;
		return;
	}

	if (!DispatchMessageA) {
		SPOOF_FUNC;
		return;
	}

	if (!CreateThread) {
		SPOOF_FUNC;
		return;
	}

	if (!CloseHandle) {
		SPOOF_FUNC;
		return;
	}

	if (!SetWindowPos) {
		SPOOF_FUNC;
		return;
	}

	if (!ShowWindow) {
		SPOOF_FUNC;
		return;
	}

	if (!SetWindowDisplayAffinity) {
		SPOOF_FUNC;
		return;
	}

	if (!mouzeevent) {
		SPOOF_FUNC;
		return;
	}

	if (!updetewindow) {
		SPOOF_FUNC;
		return;
	}

	if (!clientrect) {
		SPOOF_FUNC;
		return;
	}

	if (!clienttoscreen) {
		SPOOF_FUNC;
		return;
	}

	*(void**)&_GetCursorPos = GetCursorPos;
	*(void**)&_SetLayeredWindowAttributes = SetLayeredWindowAttributes;
	*(void**)&_DefWindowProc = DefWindowProcA;
	*(void**)&_CreateWindowExA = CreateWindowExA;
	*(void**)&_SetWindowLongA = SetWindowLongA;
	*(void**)&_GetWindowLongA = GetWindowLongA;
	*(void**)&_RegisterClassExA = RegisterClassExA;
	*(void**)&_GetSystemMetrics = GetSystemMetrics;
	*(void**)&_FindWindowA = FindWindowA;
	*(void**)&_GetAsyncKeyState = GetAsyncKeyState;
	*(void**)&_GetForegroundWindow = GetForegroundWindow;
	*(void**)&_MoveWindow = MoveWindow;
	*(void**)&_PeekMessageA = PeekMessageA;
	*(void**)&_TranslateMessage = TranslateMessage;
	*(void**)&_DispatchMessageA = DispatchMessageA;
	*(void**)&_CreateThread = CreateThread;
	*(void**)&_CloseHandle = CloseHandle;
	*(void**)&_SetWindowPos = SetWindowPos;
	*(void**)&_ShowWindow = ShowWindow;
	*(void**)&_SetWindowDisplayAffinity = SetWindowDisplayAffinity;
	*(void**)&_mouse_event = mouzeevent;
	*(void**)&_UpdateWindow = updetewindow;
	*(void**)&_GetClientRect = clientrect;
	*(void**)&_ClientToScreen = clienttoscreen;

	IMPORT(FreeLibrary).get()(kernel32);
	IMPORT(FreeLibrary).get()(user32);
	IMPORT(FreeLibrary).get()(win32u);
	IMPORT(FreeLibrary).get()(dwmapi);
}

BOOL GetCursorPosA_Spoofed(LPPOINT lpPoint)
{
	SPOOF_FUNC;

	return _GetCursorPos(lpPoint);
}

BOOL SetLayeredWindowAttributes_Spoofed(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags)
{
	SPOOF_FUNC;

	return _SetLayeredWindowAttributes(hwnd, crKey, bAlpha, dwFlags);
}

LRESULT __stdcall DefWindowProcA_Spoofed(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	SPOOF_FUNC;

	return _DefWindowProc(hwnd, Msg, wParam, lParam);
}

HWND __stdcall CreateWindowExA_Spoofed(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	SPOOF_FUNC;

	return _CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

LONG __stdcall SetWindowLongA_Spoofed(HWND hwnd, int nIndex, LONG dwNewLong)
{
	SPOOF_FUNC;

	return _SetWindowLongA(hwnd, nIndex, dwNewLong);
}

LONG __stdcall GetWindowLongA_Spoofed(HWND hwnd, int nIndex)
{
	SPOOF_FUNC;

	return _GetWindowLongA(hwnd, nIndex);
}

ATOM __stdcall RegisterClassExA_Spoofed(const WNDCLASSEXA* softaim) {
	SPOOF_FUNC;

	return _RegisterClassExA(softaim);
}

int __stdcall GetSystemMetrics_Spoofed(int nIndex) {
	SPOOF_FUNC;

	return _GetSystemMetrics(nIndex);
}

HWND __stdcall FindWindowA_Spoofed(LPCSTR lpClassName, LPCSTR lpWindowName) {
	SPOOF_FUNC;

	return _FindWindowA(lpClassName, lpWindowName);
}

HWND __stdcall GetForegroundWindow_Spoofed() {
	SPOOF_FUNC;

	return _GetForegroundWindow();
}

SHORT GetAsyncKeyState_Spoofed(int vKey) {
	SPOOF_FUNC;

	return _GetAsyncKeyState(vKey);
}

SHORT MoveWindow_Spoofed(HWND hWnd, int X, int Y, int nWidth, int nHeight, bool bRepaint) {
	SPOOF_FUNC;

	return _MoveWindow(hWnd, X, Y, nWidth, nHeight, bRepaint);
}

BOOL __stdcall PeekMessageA_Spoofed(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg) {
	SPOOF_FUNC;

	return _PeekMessageA(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
}

BOOL __stdcall TranslateMessage_Spoofed(const MSG* lpMsg) {
	SPOOF_FUNC;

	return _TranslateMessage(lpMsg);
}

LRESULT __stdcall DispatchMessageA_Spoofed(const MSG* lpMsg) {
	SPOOF_FUNC;

	return _DispatchMessageA(lpMsg);
}

HANDLE __stdcall CreateThread_Spoofed(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId) {
	SPOOF_FUNC;

	return _CreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);
}

BOOL __stdcall CloseHandle_Spoofed(HANDLE hObject) {
	SPOOF_FUNC;

	return _CloseHandle(hObject);
}

BOOL __stdcall SetWindowPos_Spoofed(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
	SPOOF_FUNC;

	return _SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

BOOL __stdcall ShowWindow_Spoofed(HWND hWnd, int nCmdShow) {
	SPOOF_FUNC;

	return _ShowWindow(hWnd, nCmdShow);
}

BOOL __stdcall SetWindowDisplayAffinity_Spoofed(HWND hWnd, DWORD dwAffinity) {
	SPOOF_FUNC;

	return _SetWindowDisplayAffinity(hWnd, dwAffinity);
}

BOOL __stdcall mouse_event_Spoofed(DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, ULONG_PTR dwExtraInfo) {
	SPOOF_FUNC;

	return _mouse_event(dwFlags, dx, dy, dwData, dwExtraInfo);
}

BOOL __stdcall UpdateWindow_Spoofed(HWND hWnd) {
	SPOOF_FUNC;

	return _UpdateWindow(hWnd);
}

BOOL __stdcall GetClientRect_Spoofed(HWND hWnd, LPRECT lpRect) {
	SPOOF_FUNC;

	return _GetClientRect(hWnd, lpRect);
}

BOOL __stdcall ClientToScreen_Spoofed(HWND hWnd, LPPOINT lpRect) {
	SPOOF_FUNC;

	return _ClientToScreen(hWnd, lpRect);
}


inline const BYTE XOR_KEY = 0xACACACADDEDFFFA;

inline void EncryptFunctionBytes(BYTE* bytes, size_t size) {
	for (size_t i = 0; i < size; ++i) {
		bytes[i] ^= XOR_KEY;
	}
}

inline void DecryptFunctionBytes(BYTE* bytes, size_t size) {
	for (size_t i = 0; i < size; ++i) {
		bytes[i] ^= XOR_KEY;
	}
}

