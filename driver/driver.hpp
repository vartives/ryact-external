#pragma once

#include "structs/io.hpp"
#include "structs/crypter.hpp"

#include <iostream>

class Driver
{
public:
    HANDLE device;
    std::uint32_t procid;
    std::uintptr_t base;

    NTSTATUS ntstatus = STATUS_UNSUCCESSFUL;

    auto SetHandle() -> bool;

    auto SetProc(const std::wstring& ProcName) -> bool;

    auto SetBase(const std::int32_t ProcID) -> NTSTATUS;

    auto SetCr3(const std::int32_t ProcID) -> NTSTATUS;

    auto ReadMemory(const std::uintptr_t Address, void* Buffer, const size_t Size) -> void;

    auto WriteMemory(const std::uintptr_t Address, void* Buffer, const size_t Size) -> void;

    template <typename T>
    T read(const std::uintptr_t Address)
    {
        T Ret{ };
        ReadMemory(Address, &Ret, sizeof(T));
        return Ret;
    }

    template <typename T>
    void write(const std::uintptr_t Address, const T& Value)
    {
        WriteMemory(Address, (void*)&Value, sizeof(T));
    }

    bool IsValid(const uint64_t Address)
    {
        if (Address <= 0x400000 || Address == 0xCCCCCCCCCCCCCCCC || reinterpret_cast<void*>(Address) == nullptr || Address > 0x7FFFFFFFFFFFFFFF)
            return false;

        return true;
    }
};

inline const std::unique_ptr<Driver> Kernel = std::make_unique<Driver>();