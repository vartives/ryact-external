#include "driver.hpp"

#include <TlHelp32.h>
#include <string>
#include <windows.h>

extern "C" __int64 direct_device_control(
    HANDLE FileHandle,
    HANDLE Event,
    PIO_APC_ROUTINE ApcRoutine,
    PVOID ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    std::uint32_t IoControlCode,
    PVOID InputBuffer,
    std::uint32_t InputBufferLength,
    PVOID OutputBuffer,
    std::uint32_t OutputBufferLength);

auto Driver::SetHandle() -> bool
{
    device = CreateFileA(Encrypt("\\\\.\\" Driver_Name), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);

    if (!device or device == INVALID_HANDLE_VALUE)
        return false;

    return true;
}

auto Driver::SetProc(const std::wstring& ProcName) -> bool
{
    auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return false;

    PROCESSENTRY32W ProcessEntry{ };
    ProcessEntry.dwSize = sizeof(ProcessEntry);
    if (!Process32FirstW(snapshot, &ProcessEntry))
        return false;

    do {
        if (ProcName == ProcessEntry.szExeFile) {
            procid = ProcessEntry.th32ProcessID;
            return true;
        }
    } while (Process32NextW(snapshot, &ProcessEntry));

    return false;
}

auto Driver::SetBase(const std::int32_t ProcID) -> NTSTATUS
{
    Base imbase = { NULL };

    uintptr_t ReturnBase = { NULL };

    imbase.ProcID = ProcID;
    imbase.Address = &ReturnBase;

    imbase.Security = Ioctl_Security;

    IO_STATUS_BLOCK IOSB = { 0 };
    NTSTATUS status = direct_device_control(
        device,
        nullptr,
        nullptr,
        nullptr,
        &IOSB,
        Ioctl_Base,
        &imbase,
        sizeof imbase,
        &imbase,
        sizeof imbase);

    if (NT_SUCCESS(status))
        base = ReturnBase;

    return status;
}

auto Driver::SetCr3(const std::int32_t ProcID) -> NTSTATUS
{
    Cr3 cr3 = { 0 };
    cr3.ProcID = ProcID;

    IO_STATUS_BLOCK IOSB = { 0 };
    NTSTATUS status = direct_device_control(
        device,
        nullptr,
        nullptr,
        nullptr,
        &IOSB,
        Ioctl_Cr3,
        &cr3,
        sizeof cr3,
        &cr3,
        sizeof cr3);

    return status;
}

auto Driver::ReadMemory(const std::uintptr_t Address, void* Buffer, const size_t Size) -> void
{
    _ReWr ReWr = { 0 };
    ReWr.Address = Address;
    ReWr.Buffer = Buffer;
    ReWr.Size = Size;
    ReWr.ProcID = procid;
    ReWr.Write = false;

    ReWr.Security = Ioctl_Security;

    IO_STATUS_BLOCK IOSB = { 0 };
    NTSTATUS status = direct_device_control(
        device,
        nullptr,
        nullptr,
        nullptr,
        &IOSB,
        Ioctl_ReWr,
        &ReWr,
        sizeof ReWr,
        &ReWr,
        sizeof ReWr);

    ntstatus = status;
}

auto Driver::WriteMemory(const std::uintptr_t Address, void* Buffer, const size_t Size) -> void
{
    _ReWr ReWr = { 0 };
    ReWr.Address = Address;
    ReWr.Buffer = Buffer;
    ReWr.Size = Size;
    ReWr.ProcID = procid;
    ReWr.Write = true;

    ReWr.Security = Ioctl_Security;

    IO_STATUS_BLOCK IOSB = { 0 };
    NTSTATUS status = direct_device_control(
        device,
        nullptr,
        nullptr,
        nullptr,
        &IOSB,
        Ioctl_ReWr,
        &ReWr,
        sizeof ReWr,
        &ReWr,
        sizeof ReWr);

    ntstatus = status;
}