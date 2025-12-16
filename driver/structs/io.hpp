#pragma once
#include <Windows.h>
#include <cstdint>

#include <winternl.h>
#pragma comment( lib, "ntdll.lib" )

#include <ntstatus.h>

#define Ioctl_Base CTL_CODE( FILE_DEVICE_UNKNOWN, 0xA4D9, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS )
#define Ioctl_Cr3 CTL_CODE( FILE_DEVICE_UNKNOWN, 0xCE27, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS )
#define Ioctl_ReWr CTL_CODE( FILE_DEVICE_UNKNOWN, 0xB193, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS )

#define Ioctl_Security 0xC8E5A1D4B79F23A6ULL

#define Driver_Name "{e8b1f4a3-5d7c-46e2-8f19-3a6b2d4e9c75}"

typedef struct _Base
{
    std::int32_t ProcID;
    std::uintptr_t* Address;
    std::uintptr_t Security;
} Base, * PBase;

typedef struct _Cr3
{
    std::int32_t ProcID;
    std::uintptr_t Security;
} Cr3, * CR3;

typedef struct _ReWr
{
    std::uintptr_t Address;
    void* Buffer;
    size_t Size;
    bool Write;
    std::int32_t ProcID;
    std::uintptr_t Security;
} ReWr, * DoReWr;