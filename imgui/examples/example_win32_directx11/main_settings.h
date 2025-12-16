#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include "../../imgui_internal.h"
//#include "examples/example_win32_directx9/imgui_settings.h"

namespace ms {

  
    ImFont* Inter_Medium = nullptr;
    ImFont* Inter_Bold = nullptr;
    ImFont* Inter_Bold_1 = nullptr;
    ImFont* Inter_SemmiBold = nullptr;
    ImFont* Inter_SemmiBold_1 = nullptr;
    ImFont* Icon_Arrow;
    inline  float tab_alpha = 0.f; /* */ inline  float tab_add; /* */ inline  int active_tabs = 0;
    inline  int page = 0;
    bool active_audio_notif = true;
    int togle = 0;
    const char* subtabs[] = { "First","Second" };
    const char* subtabs1[] = { "Fishing Spots","Catch Log", "Bot Settings" };
    int x_pos0 = (1920 / 2), x_pos1 = (1080 / 2);
    int pages = 0;
    inline ID3D11ShaderResourceView* ImgConfigs = nullptr;
    inline ID3D11ShaderResourceView* ImgVisuals = nullptr;
    inline ID3D11ShaderResourceView* ImgWeaponConfig = nullptr;
    inline ID3D11ShaderResourceView* ImgExploits = nullptr;
    inline ID3D11ShaderResourceView* ImgPc = nullptr;
    inline ID3D11ShaderResourceView* ImgConfig = nullptr;
    inline ID3D11ShaderResourceView* ImgKey = nullptr;
    inline ID3D11ShaderResourceView* ImgExit = nullptr;
    inline ID3D11ShaderResourceView* ImgUser = nullptr;
    inline ID3D11ShaderResourceView* ImgTime = nullptr;
    inline ID3D11ShaderResourceView* ImgColor = nullptr;
    inline ID3D11ShaderResourceView* ImgFilled = nullptr;
   
    

}