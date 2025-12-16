#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <d3d11.h>
#include <iostream>

inline void basicloop() {
    while (true) {
		std::cout << "Uworld: " << WorldCache.UWorld << "\n";
		std::cout << "GameInstance: " << WorldCache.GameInstance << "\n";
		std::cout << "LocalPlayer: " << WorldCache.LocalPlayer << "\n";
		std::cout << "PlayerController: " << WorldCache.PlayerController << "\n";
		std::cout << "AcknownledgedPawn: " << WorldCache.AcknownledgedPawn << "\n";
		std::cout << "PlayerController: " << WorldCache.PlayerController << "\n";
		std::cout << "Mesh: " << WorldCache.Mesh << "\n";
		std::cout << "PlayerState: " << WorldCache.PlayerState << "\n";
    }
}