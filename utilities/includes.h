#pragma once

#include "../utilities/sdk/sdk.h"
#include "../imgui/imgui.h"
#include <d3d9.h>
#include "../imgui/imgui_impl_win32.h"
#include "../driver/driver.hpp"
#include <windows.h>
#include <dwmapi.h>
#include "../driver/drvva.h"
ImFont* MenuFont;
ImFont* GameFont;
#define e_log( fmt, ... ) e_log( " [>] " fmt "", ##__VA_ARGS__ )

class structs
{
public:

	uintptr_t
		UWorld,
		GameInstance,
		GameState,
		LocalPlayer,
		AcknownledgedPawn,
		PlayerState,
		PlayerController,
		RootComponent,
		Mesh,
		PlayerArray,
		LocalWeapon,
		LocalVehicle;
	int32_t
		AmmoCount;

	int
		TeamIndex,
		PlayerArraySize;


}; structs WorldCache;

class entity {
public:
	uintptr_t
		entity,
		skeletal_mesh,
		root_component,
		player_state,
		pawn_private;

	char
		IgnoreDeads;

	int
		team_index,
		kills;
	char
		team_number;
	float
		lastrendertime;
	bool
		is_visible;
};
std::vector<entity> entity_list;
std::vector<entity> temporary_entity_list;

class item {
public:
	uintptr_t
		Actor;

	std::string
		Name;
	bool
		isVehicle,
		isChest,
		isPickup,
		isAmmoBox;
	float
		distance;
};

std::vector<item> item_pawns;