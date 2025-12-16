#pragma once

#include "../../driver/driver.hpp"
#include <unordered_map>
#include "../../driver/drvva.h"
#include "json.hpp"
#include <fstream>

namespace offsets
{
    uintptr_t UWorldDecryption = 0xFFFFFFFFDC445031uLL;
    uintptr_t UWorld = 0x176B9A78;
    uintptr_t GWorld = 0x176B9A78;
    uintptr_t GNames = 0x175746C0;
    uintptr_t GObjects = 0x172467F0;
    uintptr_t GObjectsCount = 0x17246804;
    
    uintptr_t PersistentLevel = 0x38;
    uintptr_t ActorArray = 0x98;
    uintptr_t AActors = 0x48;
    
    uintptr_t GameInstance = 0x240;
    uintptr_t OwningGameInstance = 0x240;
    uintptr_t GameState = 0x1C8;
    uintptr_t PlayerArray = 0x2C8;
    
    uintptr_t LocalPlayers = 0x38;
    uintptr_t PlayerController = 0x30;
    uintptr_t PlayerCameraManager = 0x360;
    uintptr_t AcknowledgedPawn = 0x358;
    uintptr_t PlayerState = 0x2D0;
    uintptr_t PawnPrivate = 0x328;
    
    uintptr_t RootComponent = 0x1B0;
    uintptr_t Mesh = 0x330;
    uintptr_t BoneArray = 0x5F0;
    uintptr_t ComponentToWorld = 0x1E0;
    uintptr_t RelativeLocation = 0x140;
    uintptr_t RelativeRotation = 0x158;
    uintptr_t RelativeScale3D = 0x170;
    uintptr_t ComponentVelocity = 0x188;
    uintptr_t LastRenderTime = 0x328;
    uintptr_t LastSubmitTimeOnScreen = 0x328;
    
    uintptr_t CurrentVehicle = 0x2C28;
    uintptr_t current_vehicle = 0x2C28;
    uintptr_t CurrentWeapon = 0x990;
    uintptr_t current_weapon = 0x990;
    uintptr_t WeaponData = 0x5C0;
    uintptr_t weapon_data = 0x5C0;
    uintptr_t AmmoCount = 0x1464;
    uintptr_t ReloadAnimation = 0x19E8;
    
    uintptr_t PlayerAimOffset = 0x2BE8;
    uintptr_t AdditionalAimOffset = 0x2BB8;
    uintptr_t RotationInput = 0x530;
    uintptr_t RotationInputCopy = 0x530;
    uintptr_t RotationPointer = 0x530;
    uintptr_t PlayerAim = 0x2BE8;
    uintptr_t PlayerAimCopy = 0x91;
    
    uintptr_t CameraLocation = 0x170;   
    uintptr_t CameraRotation = 0x180;
    uintptr_t CameraFOV = 0x3B4;
    uintptr_t PlayerFov = 0x3B4;
    uintptr_t FOV = 0x3B4;
    uintptr_t CameraManager = 0x360;
    
    uintptr_t TeamIndex = 0x11A9;
    uintptr_t bIsDying = 0x728;
    uintptr_t b_is_dying = 0x728;
    uintptr_t bIsDBNO = 0x841;
    uintptr_t b_is_dbno = 0x841;
    uintptr_t bIsABot = 0x2BA;
    uintptr_t ItsBot = 0x2BA;
    uintptr_t PlayerName = 0xA00;
    uintptr_t username_component = 0xA00;
    uintptr_t Platform = 0x440;
    uintptr_t TargetedFortPawn = 0x1840;
    uintptr_t KillScore = 0x11C0;
    uintptr_t HabaneroComponent = 0x940;
    uintptr_t habanero_component = 0x940;
    
    uintptr_t PrimaryPickupItemEntry = 0x370;
    uintptr_t Tier = 0xAA;
    uintptr_t Rarity = 0xA2;
    uintptr_t Color = 0xA2;
    uintptr_t ItemRarity = 0xA2;
    uintptr_t ItemName = 0x40;
    uintptr_t ItemDefinition = 0x10;
    uintptr_t ItemType = 0xA8;
    
    uintptr_t ProjectileSpeed = 0x26E4;
    uintptr_t ProjectileGravity = 0x26E8;
    
    uintptr_t WorldSettings = 0x2B8;
    uintptr_t WorldGravityZ = 0x330;
    uintptr_t WorldToMeters = 0x320;
    uintptr_t WorldTimeSeconds = 0x190;
    uintptr_t ServerWorldTimeSecondsDelta = 0x2E8;
    
    uintptr_t CurrentMovementStyle = 0x843;
    uintptr_t MoveIgnoreActors = 0x348;
    uintptr_t bIsLocalPlayerController = 0x91;
    uintptr_t EmoteMusicEnvelopeBeatCount = 0x91;
    uintptr_t NamePrivate = 0x91;
    uintptr_t NameStructure = 0x91;
    uintptr_t RankedProgress = 0xD8;
    uintptr_t RankProgress = 0xD8;
    uintptr_t SeasonLevelUIDisplay = 0x91;
    uintptr_t Spectators = 0xA98;
    uintptr_t SpectatorArray = 0x2198;
    uintptr_t bAlreadySearched = 0x91;
    uintptr_t BuildingState = 0x91;
    uintptr_t WeaponCoreAnimation = 0x91;
    uintptr_t ItemData = 0x91;
    uintptr_t bIsReloadingWeapon = 0x3B9;
    uintptr_t LastFireTime = 0x139C;
    uintptr_t LastDamagedTime = 0x91;
    uintptr_t GlobalAnimRateScale = 0x91;
    uintptr_t bADSWhileNotOnGround = 0x91;
    uintptr_t WorldTick = 0x91;
    uintptr_t SuperField = 0x91;
    uintptr_t MaxTargetingAimAdjustPerSecond = 0x91;
    uintptr_t LastFiredLocation = 0x5BB0;
    uintptr_t LastFiredDirection = 0x5BC8;
    uintptr_t FiredTime = 0x5BE0;
    
    uintptr_t NetConnection = 0x528;
    uintptr_t Reboots = 0x188C;
    
    uintptr_t DefaultFov = 0x2C4;
    uintptr_t BaseFov = 0x710;
};

inline uintptr_t decryptWorld(uintptr_t address)
{
    return address ^ 0xFFFFFFFFDC445031uLL;
}

struct APlayerCameraManager {};

inline APlayerCameraManager* GetPlayerCameraManager()
{
    return *(APlayerCameraManager**)(Kernel->base + offsets::PlayerCameraManager);
}


enum boneID : int {
    Head = 110,
    Neck = 67,
    LHand = 11,
    RHand = 40,
    RElbow = 39,
    LElbow = 10,
    LShoulder = 9,
    RShoulder = 38,
    Chest = 66,
    Pelvis = 2,
    LHip = 71,
    RHip = 78,
    LKnee = 72,
    RKnee = 79,
    LFeet = 75,
    RFeet = 82,
    Root = 0
};

struct b
{
    int humanbase = 0;
    int	humanpelvis = 2;
    int	humanlthigh1 = 71;
    int	humanlthigh2 = 77;
    int	humanlthigh3 = 72;
    int	humanlcalf = 74;
    int	humanlfoot = 86;
    int humanlfoot2 = 86;
    int	humanltoe = 76;
    int	humanrthigh1 = 78;
    int	humanrthigh2 = 84;
    int	humanrthigh3 = 79;
    int humanrfoot2 = 82;
    int	humanrcalf = 81;
    int	humanrfoot = 87;
    int	humanrtoe = 83;
    int	humanspine1 = 7;
    int	humanspine2 = 5;
    int	humanspine3 = 2;
    int	humanlcollarbone = 9;
    int	humanlupperarm = 35;
    int	humanlforearm1 = 36;
    int humanchestright = 37;
    int humanchestleft = 8;
    int	humanlforearm23 = 10;
    int	humanlforearm2 = 34;
    int	humanlforearm3 = 33;
    int	humanlpalm = 32;
    int humanlhand = 11;
    int	humanrcollarbone = 98;
    int	humanrupperarm = 64;
    int	humanrforearm1 = 65;
    int	humanrforearm23 = 39;
    int humanrhand = 40;
    int	humanrforearm2 = 63;
    int	humanrforearm3 = 62;
    int	humanrpalm = 58;
    int	humanneck = 67;
    int	humanhead = 68;
    int	humanchest = 66;
};

std::unique_ptr<b> bones = std::make_unique<b>();
namespace world_offsets
{
    uintptr_t Levels = 0x1E8;
    uintptr_t Actors = 0x48;
    uintptr_t PersistentLevel = 0x38;
    uintptr_t ActorArray = 0x98;
    uintptr_t AActors = 0x48;
}
template< typename t >

class TArray
{
public:
    TArray() : tData(), iCount(), iMaxCount() {}
    TArray(t* data, int count, int max_count) :
        tData(tData), iCount(iCount), iMaxCount(iMaxCount) {
    }

public:
    auto Get(int idx) -> t
    {
        return Kernel->read< t >(reinterpret_cast<__int64>(this->tData) + (idx * sizeof(t)));
    }

    auto Size() -> std::uint32_t
    {
        return this->iCount;
    }

    bool IsValid()
    {
        return this->iCount != 0;
    }

    t* tData;
    int iCount;
    int iMaxCount;
    std::uintptr_t Array;
    std::uint32_t Count;
    std::uint32_t MaxCount;

    t Get(std::uint32_t Index) const {
        if (Index >= Count) {
            return t();
        }
        return Kernel->read<t>(Array + (Index * sizeof(t)));
    }

    t operator[](std::uint32_t Index) const {
        return Get(Index);
    }

    std::uint32_t size() const {
        return Count;
    }

    bool isValid() const {
        return Array && Count <= MaxCount && MaxCount <= 1000000;
    }

    std::uintptr_t getAddress() const {
        return Array;
    }
};
template<class T>
class FArray
{
public:
    int getLength() const
    {
        return count;
    }

    int getIdentifier()
    {
        return data * count * max;
    }

    bool isValid() const
    {
        if (count > max)
            return false;
        if (!data)
            return false;
        return true;
    }

    uint64_t getAddress() const
    {
        return data;
    }

    T operator [](size_t idx) const
    {
        return Kernel->read<T>(data + sizeof(T) * idx);
    }

protected:
    uint64_t data;
    uint64_t count;
    uint64_t max;
};

struct Fortnite
{
    uint64_t getVAStart(uintptr_t gworldOffset);

    enum ActorType
    {
        NOT_IN_USE = 0,
        CHEST = 1,
        AMMO_BOX = 2
    };

    struct ActorDefinitions
    {
        int actorID;
        std::string fname;
        ActorType actorType;
    };

    const std::unordered_map<std::string, ActorType>& getActorByName()
    {
        const std::unordered_map<std::string, ActorType> actorTypeByName =
        {
            {"Tiered_Chest", ActorType::CHEST},
            {"Tiered_Ammo", ActorType::AMMO_BOX}
        };
        return actorTypeByName;
    }

    ActorDefinitions getActorDefinitions(int actorID);
};