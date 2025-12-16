#pragma once
#include <windows.h>
#include <iostream>
#define KMOUSE_LEFT_DOWN   0x01
#define KMOUSE_LEFT_UP     0x02
#define KMOUSE_RIGHT_DOWN  0x04
#define KMOUSE_RIGHT_UP    0x08
#define KMOUSE_MIDDLE_DOWN 0x10
#define KMOUSE_MIDDLE_UP   0x20
#define MOUSE_MOVE_RELATIVE         0
#define MOUSE_MOVE_ABSOLUTE         1
#define KMOUSE_LMB 9123
#define KMOUSE_RMB 9124

#ifndef CTL_CODE
#define CTL_CODE(DeviceType, Function, Method, Access) ( \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) )
#endif

#define FILE_DEVICE_UNKNOWN 0x00000022
#define METHOD_BUFFERED 0
#define FILE_SPECIAL_ACCESS 0

#define MOUSE_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8225, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

#pragma pack(push, 1)
typedef struct _KMOUSE_REQUEST {
    long x;
    long y;
    unsigned char button_flags;
} KMOUSE_REQUEST, * PKMOUSE_REQUEST;
#pragma pack(pop)

class KMouse {
private:
    HANDLE hDevice = INVALID_HANDLE_VALUE;
    LPCWSTR devicePath = L"\\\\.\\5a0bbcf3238a63a37327fab8a171b019";

public:
    ~KMouse() {
        if (hDevice != INVALID_HANDLE_VALUE) {
            CloseHandle(hDevice);
        }
    }

    bool openHandle() {
        if (hDevice != INVALID_HANDLE_VALUE) {
            CloseHandle(hDevice);
            hDevice = INVALID_HANDLE_VALUE;
        }
        hDevice = CreateFileW(
            devicePath,
            GENERIC_READ | GENERIC_WRITE,
            0,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
        );
        return isLoaded();
    }

    bool isLoaded() const {
        return hDevice != INVALID_HANDLE_VALUE;
    }

    bool move(long x, long y, unsigned char button_flags = 0) {
        if (!isLoaded()) {
            std::wcerr << L"Device not loaded." << std::endl;
            return false;
        }
        KMOUSE_REQUEST request = {};
        request.x = x;
        request.y = y;
        request.button_flags = button_flags;

        DWORD bytesReturned = 0;
        BOOL result = DeviceIoControl(
            hDevice,
            MOUSE_REQUEST,
            &request, sizeof(request),
            nullptr, 0,
            &bytesReturned,
            nullptr
        );
        if (!result) {
            std::wcerr << L"DeviceIoControl failed: " << GetLastError() << std::endl;
            return false;
        }
        return true;
    }

    bool click(int type, float delay, float betweenDelay) {
        if (type == KMOUSE_LMB) {
            // Left mouse button click
            move(0, 0, KMOUSE_LEFT_DOWN);
            Sleep(betweenDelay);
            move(0, 0, KMOUSE_LEFT_UP);
            Sleep(delay);

            return true;
        }
        else if (type == KMOUSE_RMB) {
            // Right mouse button click
            move(0, 0, KMOUSE_LEFT_DOWN);
            Sleep(betweenDelay);
            move(0, 0, KMOUSE_LEFT_UP);

            Sleep(delay);

            return true;
        }
        else {
            return false;
        }
    }
};