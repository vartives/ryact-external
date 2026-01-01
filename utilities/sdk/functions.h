#include <string.h>
#include <list>
#include <iostream>
#include "../../driver/driver.hpp"
#include "../sdk/camera.h"
#include "../../driver/drvva.h"
#include "../utility/offsets.h"

#define FNAMEPOOL_OFFSET 0x117284C0

D3DMATRIX matrix_multiplication(D3DMATRIX pm1, D3DMATRIX pm2)
{
	D3DMATRIX pout{};
	pout._11 = pm1._11 * pm2._11 + pm1._12 * pm2._21 + pm1._13 * pm2._31 + pm1._14 * pm2._41;
	pout._12 = pm1._11 * pm2._12 + pm1._12 * pm2._22 + pm1._13 * pm2._32 + pm1._14 * pm2._42;
	pout._13 = pm1._11 * pm2._13 + pm1._12 * pm2._23 + pm1._13 * pm2._33 + pm1._14 * pm2._43;
	pout._14 = pm1._11 * pm2._14 + pm1._12 * pm2._24 + pm1._13 * pm2._34 + pm1._14 * pm2._44;
	pout._21 = pm1._21 * pm2._11 + pm1._22 * pm2._21 + pm1._23 * pm2._31 + pm1._24 * pm2._41;
	pout._22 = pm1._21 * pm2._12 + pm1._22 * pm2._22 + pm1._23 * pm2._32 + pm1._24 * pm2._42;
	pout._23 = pm1._21 * pm2._13 + pm1._22 * pm2._23 + pm1._23 * pm2._33 + pm1._24 * pm2._43;
	pout._24 = pm1._21 * pm2._14 + pm1._22 * pm2._24 + pm1._23 * pm2._34 + pm1._24 * pm2._44;
	pout._31 = pm1._31 * pm2._11 + pm1._32 * pm2._21 + pm1._33 * pm2._31 + pm1._34 * pm2._41;
	pout._32 = pm1._31 * pm2._12 + pm1._32 * pm2._22 + pm1._33 * pm2._32 + pm1._34 * pm2._42;
	pout._33 = pm1._31 * pm2._13 + pm1._32 * pm2._23 + pm1._33 * pm2._33 + pm1._34 * pm2._43;
	pout._34 = pm1._31 * pm2._14 + pm1._32 * pm2._24 + pm1._33 * pm2._34 + pm1._34 * pm2._44;
	pout._41 = pm1._41 * pm2._11 + pm1._42 * pm2._21 + pm1._43 * pm2._31 + pm1._44 * pm2._41;
	pout._42 = pm1._41 * pm2._12 + pm1._42 * pm2._22 + pm1._43 * pm2._32 + pm1._44 * pm2._42;
	pout._43 = pm1._41 * pm2._13 + pm1._42 * pm2._23 + pm1._43 * pm2._33 + pm1._44 * pm2._43;
	pout._44 = pm1._41 * pm2._14 + pm1._42 * pm2._24 + pm1._43 * pm2._34 + pm1._44 * pm2._44;
	return pout;
}

class decryption_t
{
public:
	static std::string get_fname(int key)
	{
		uint32_t ChunkOffset = (uint32_t)((int)(key) >> 16);
		uint16_t NameOffset = (uint16_t)key;

		uint64_t NamePoolChunk = Kernel->read<uint64_t>(globals.va_text + FNAMEPOOL_OFFSET + (8 * ChunkOffset) + 16) + (unsigned int)(2 * NameOffset); //((ChunkOffset + 2) * 8) ERROR_NAME_SIZE_EXCEEDED
		if (Kernel->read<uint16_t>(NamePoolChunk) < 64)
		{
			auto a1 = Kernel->read<DWORD>(NamePoolChunk + 2);
			return GetNameFromIndex(a1);
		}
		else
		{
			return GetNameFromIndex(key);
		}
	}

	static std::string GetNameFromIndex(int key)
	{
		uint32_t ChunkOffset = (uint32_t)((int)(key) >> 16);
		uint16_t NameOffset = (uint16_t)key;
		uint64_t NamePoolChunk = Kernel->read<uint64_t>(globals.va_text + FNAMEPOOL_OFFSET + (8 * ChunkOffset) + 16) + (unsigned int)(2 * NameOffset);
		uint16_t nameEntry = Kernel->read<uint16_t>(NamePoolChunk);
		int nameLength = nameEntry >> 6;
		char buff[1024] = {};

		char* v3 = buff; // rdi
		int v5; // r8d
		__int64 result = nameLength; // rax
		__int64 v7 = 0; // rdx
		unsigned int v8; // r8d
		v5 = 26;
		if ((int)result)
		{
			do
			{
				v8 = v5 + 45297;
				*v3 = v8 + ~*v3;
				result = v8 << 8;
				v5 = result | (v8 >> 8);
				++v3;
				--v7;
			} while (v7);
			buff[nameLength] = '\0';
			return std::string(buff);
		}
		return std::string("");
	}

	void AddPlayerToFovCircle(fvector WorldLocation, float fDistance, bool visible, ImColor color)
	{
		fvector vAngle = camera_postion.rotation;
		float fYaw = vAngle.y * PI / 180.0f;
		float dx = WorldLocation.x - camera_postion.location.x;
		float dy = WorldLocation.y - camera_postion.location.y;
		float fsin_yaw = sinf(fYaw);
		float fminus_cos_yaw = -cosf(fYaw);

		auto Center = ImVec2(globals.ScreenWidth / 2, globals.ScreenHeight / 2);

		float x = -(dy * fminus_cos_yaw + dx * fsin_yaw);
		float y = dx * fminus_cos_yaw - dy * fsin_yaw;

		float fovRadius = aimbot.fovsize;
		float angle = atan2f(y, x);
		float triangleSize = 12.0f;
		float widthFactor = 8.0f;
		float outlineThickness = 2.0f;

		ImVec2 triangleCenter = ImVec2(Center.x + cosf(angle) * (fovRadius + triangleSize),
			Center.y + sinf(angle) * (fovRadius + triangleSize));

		ImVec2 point1 = ImVec2(triangleCenter.x + cosf(angle) * triangleSize,
			triangleCenter.y + sinf(angle) * triangleSize);
		ImVec2 point2 = ImVec2(triangleCenter.x + cosf(angle + widthFactor) * triangleSize,
			triangleCenter.y + sinf(angle + widthFactor) * triangleSize);
		ImVec2 point3 = ImVec2(triangleCenter.x + cosf(angle - widthFactor) * triangleSize,
			triangleCenter.y + sinf(angle - widthFactor) * triangleSize);

		ImU32 fillColor = ImGui::ColorConvertFloat4ToU32(visible ? color : ImColor(color.Value.x, color.Value.y, color.Value.z, 0.5f));
		ImU32 outlineColor = ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0, 1.0));

		ImDrawList* drawList = ImGui::GetForegroundDrawList();
		drawList->AddTriangleFilled(point1, point2, point3, fillColor);
		drawList->AddTriangle(point1, point2, point3, outlineColor, outlineThickness);
	}

	std::string getRank(int tier) {
		switch (tier) {
		case 0:  return "Unranked";
		case 1:  return "Bronze 2";
		case 2:  return "Bronze 3";
		case 3:  return "Silver 1";
		case 4:  return "Silver 2";
		case 5:  return "Silver 3";
		case 6:  return "Gold 1";
		case 7:  return "Gold 2";
		case 8:  return "Gold 3";
		case 9:  return "Platinum 1";
		case 10: return "Platinum 2";
		case 11: return "Platinum 3";
		case 12: return "Diamond 1";
		case 13: return "Diamond 2";
		case 14: return "Diamond 3";
		case 15: return "Elite";
		case 16: return "Champion";
		case 17: return "Unreal";
		default: return "Unranked";
		}
	}

	ImVec4 getRankColor(int tier) {
		switch (tier) {
		case 0:  return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);    // Unranked
		case 1:
		case 2:  return ImVec4(0.902f, 0.580f, 0.227f, 1.0f); // Bronze
		case 3:
		case 4:
		case 5:  return ImVec4(0.843f, 0.843f, 0.843f, 1.0f); // Silver
		case 6:
		case 7:
		case 8:  return ImVec4(1.0f, 0.871f, 0.0f, 1.0f); // Gold
		case 9:
		case 10:
		case 11: return ImVec4(0.0f, 0.7f, 0.7f, 1.0f);  // Platinum
		case 12:
		case 13:
		case 14: return ImVec4(0.1686f, 0.3294f, 0.8235f, 1.0f); // Diamond
		case 15: return ImVec4(0.7f, 0.7f, 0.7f, 1.0f);   // Elite
		case 16: return ImVec4(1.0f, 0.6f, 0.0f, 1.0f);   // Champion
		case 17: return ImVec4(0.6f, 0.0f, 0.6f, 1.0f);   // Unreal
		default: return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);    // Unranked
		}
	}

	std::string ReadWideString(uint64_t address, int length) {
		if (length <= 0 || length > 50) return "";

		std::wstring buffer(length, L'\0');
		Kernel->ReadMemory(address, buffer.data(), length * sizeof(wchar_t));
		return std::string(buffer.begin(), buffer.end());
	}

	static std::string GetPlayerName(uintptr_t playerState) {
		auto Name = Kernel->read<uintptr_t>(playerState + offsets::username_component);
		auto length = Kernel->read<int>(Name + 0x10);
		auto v6 = (__int64)length;
		auto displayname = Kernel->read<char>(playerState + 0x2a8);
		auto isinlobby = Kernel->read<uintptr_t>(playerState + 0x318);

		if (!v6) return std::string("NaN");

		auto FText = (uintptr_t)Kernel->read<__int64>(Name + 0x8);

		wchar_t* Buffer = new wchar_t[length];
		Kernel->ReadMemory(FText, Buffer, length * sizeof(wchar_t));

		char v21;
		int v22;
		int i;

		int v25;
		UINT16* v23;

		v21 = v6 - 1;
		if (!(UINT32)v6)
			v21 = 0;
		v22 = 0;
		v23 = (UINT16*)Buffer;
		for (i = (v21) & 3; ; *v23++ += i & 7)
		{
			v25 = v6 - 1;
			if (!(UINT32)v6)
				v25 = 0;
			if (v22 >= v25)
				break;
			i += 3;
			++v22;
		}

		std::wstring PlayerName{ Buffer };
		delete[] Buffer;
		return std::string(PlayerName.begin(), PlayerName.end());
	}

	std::string GetWeaponName(uintptr_t Player) {
		std::string weapon_name;

		uint64_t current_weapon = Kernel->read<uint64_t>(Player + offsets::current_weapon);
		uint64_t weapon_data = Kernel->read<uint64_t>(current_weapon + offsets::weapon_data);
		uint64_t item_name = Kernel->read<uint64_t>(weapon_data + 0x40);

		uint64_t FData = Kernel->read<uint64_t>(item_name + 0x20);
		int FLength = Kernel->read<int>(item_name + 0x28);

		if (FLength > 0 && FLength < 50)
		{
			wchar_t* WeaponBuffer = new wchar_t[FLength];
			Kernel->ReadMemory(FData, WeaponBuffer, FLength * sizeof(wchar_t));
			std::wstring wstr_buf(WeaponBuffer);
			weapon_name.append(std::string(wstr_buf.begin(), wstr_buf.end()));

			delete[] WeaponBuffer;
		}

		return weapon_name;
	}
}; decryption_t decryption;
struct UObject {
};
typedef long long ll;
#define current_address reinterpret_cast<uint64_t>(this)

class gamehelper_t
{
public:

	static auto getsocketlocation(ll skeletal_mesh, int bone_index) -> fvector {
		ll ActiveArray = Kernel->read<ll>(skeletal_mesh + offsets::BoneArray);
		if (ActiveArray == 0)
		{
			ActiveArray = Kernel->read<ll>(skeletal_mesh + offsets::BoneArray + 0x10);
		}
		FTransform ActiveBone = Kernel->read<FTransform>(ActiveArray + (bone_index * 0x60));
		FTransform ComponentToWorld = Kernel->read<FTransform>(skeletal_mesh + offsets::ComponentToWorld);
		D3DMATRIX M_Matrix = matrix_multiplication(ActiveBone.to_matrix_with_scale(), ComponentToWorld.to_matrix_with_scale());

		return fvector(M_Matrix._41, M_Matrix._42, M_Matrix._43);
	}
	fvector target_prediction(fvector TargetPosition, fvector ComponentVelocity, float player_distance, float ProjectileSpeed)
	{
		float gravity = abs(-336);
		float time = player_distance / abs(ProjectileSpeed);
		float bulletDrop = (gravity / 250) * time * time;
		return fvector(TargetPosition.x += time * (ComponentVelocity.x), TargetPosition.y += time * (ComponentVelocity.y), TargetPosition.z += time * (ComponentVelocity.z));
	}
	inline double RadiansToDegrees(double dRadians)
	{
		return dRadians * (180.0 / M_PI);
	}
	fvector predict_location(fvector target, fvector target_velocity, float distance, float speed)
	{
		speed *= 100;
		float gravity = 9.81f * 100.0f;
		float time = distance / speed;

		fvector target_positsion = target /* + (target_velocity * time)*/;

		return target_positsion;
	}
	uintptr_t get_view_state()
	{
		TArray<uintptr_t> view_state = Kernel->read<TArray<uintptr_t>>(WorldCache.LocalPlayer + 0xD0);

		return view_state.Get(1);
	}


	camera_position_s get_camera()
	{
		camera_position_s cam{};
		
		uintptr_t LocationPointer = Kernel->read<uintptr_t>(WorldCache.UWorld + offsets::CameraLocation);
		uintptr_t RotationPointer = Kernel->read<uintptr_t>(WorldCache.UWorld + offsets::CameraRotation);
		
		FNRot FnRot{};
		FnRot.A = Kernel->read<double>(RotationPointer);
		FnRot.B = Kernel->read<double>(RotationPointer + 0x20);
		FnRot.C = Kernel->read<double>(RotationPointer + 0x1D0);
		
		cam.location = Kernel->read<fvector>(LocationPointer);
		cam.rotation.x = asin(FnRot.C) * (180.0 / M_PI);
		cam.rotation.y = ((atan2(FnRot.A * -1, FnRot.B) * (180.0 / M_PI)) * -1) * -1;
		cam.rotation.z = 0.0;
		
		cam.fov = Kernel->read<float>(WorldCache.PlayerController + offsets::CameraFOV) * 90.0f;

		return cam;
	}
	D3DMATRIX Matrix3d(fvector rot, fvector origin = fvector(0, 0, 0))
	{
		float radPitch = (rot.x * float(M_PI) / 180.f);
		float radYaw = (rot.y * float(M_PI) / 180.f);
		float radRoll = (rot.z * float(M_PI) / 180.f);

		float SP = sinf(radPitch);
		float CP = cosf(radPitch);
		float SY = sinf(radYaw);
		float CY = cosf(radYaw);
		float SR = sinf(radRoll);
		float CR = cosf(radRoll);

		D3DMATRIX matrix;
		matrix.m[0][0] = CP * CY;
		matrix.m[0][1] = CP * SY;
		matrix.m[0][2] = SP;
		matrix.m[0][3] = 0.f;

		matrix.m[1][0] = SR * SP * CY - CR * SY;
		matrix.m[1][1] = SR * SP * SY + CR * CY;
		matrix.m[1][2] = -SR * CP;
		matrix.m[1][3] = 0.f;

		matrix.m[2][0] = -(CR * SP * CY + SR * SY);
		matrix.m[2][1] = CY * SR - CR * SP * SY;
		matrix.m[2][2] = CR * CP;
		matrix.m[2][3] = 0.f;

		matrix.m[3][0] = origin.x;
		matrix.m[3][1] = origin.y;
		matrix.m[3][2] = origin.z;
		matrix.m[3][3] = 1.f;

		return matrix;
	}
	fvector W2S3d(fvector WorldLocation)
	{

		camera_position_s ViewPoint = get_camera();
		D3DMATRIX tempMatrix = Matrix3d(ViewPoint.rotation);
		fvector vAxisX = fvector(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
		fvector vAxisY = fvector(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
		fvector vAxisZ = fvector(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);
		fvector vDelta = WorldLocation - ViewPoint.location;
		fvector vTransformed = fvector(vDelta.dot(vAxisY), vDelta.dot(vAxisZ), vDelta.dot(vAxisX));
		if (vTransformed.z < 1.f)
			vTransformed.z = 1.f;
		return fvector((globals.ScreenWidth / 2.0f) + vTransformed.x * (((globals.ScreenWidth / 2.0f) / tanf(ViewPoint.fov * (float)M_PI / 360.f))) / vTransformed.z, (globals.ScreenHeight / 2.0f) - vTransformed.y * (((globals.ScreenHeight / 2.0f) / tanf(ViewPoint.fov * (float)M_PI / 360.f))) / vTransformed.z, 0);

	}
	bool WorldToScreen(fvector Position, fvector2d* Output) {
		if (Position.x == 0) {
			return false;
		}

		D3DMATRIX temp_matrix = to_matrix(camera_postion.rotation);

		auto XAxis = fvector(temp_matrix.m[0][0], temp_matrix.m[0][1], temp_matrix.m[0][2]);
		auto YAxis = fvector(temp_matrix.m[1][0], temp_matrix.m[1][1], temp_matrix.m[1][2]);
		auto ZAxis = fvector(temp_matrix.m[2][0], temp_matrix.m[2][1], temp_matrix.m[2][2]);

		fvector vdelta = Position - camera_postion.location;
		fvector vtransformed = fvector(vdelta.dot(YAxis), vdelta.dot(ZAxis), vdelta.dot(XAxis));

		fvector DeltaCoordinates = Position - camera_postion.location;

		fvector Transformed = fvector(DeltaCoordinates.dot(YAxis), DeltaCoordinates.dot(ZAxis), DeltaCoordinates.dot(XAxis));
		if (Transformed.z < 1.f) {
			Transformed.z = 1.f;
		}

		Output->x = (globals.ScreenWidthHALF) + Transformed.x * (((globals.ScreenWidthHALF) / tanf(camera_postion.fov * (float)M_PI / 360.f))) / Transformed.z;
		Output->y = (globals.ScreenHeightHALF) - Transformed.y * (((globals.ScreenWidthHALF) / tanf(camera_postion.fov * (float)M_PI / 360.f))) / Transformed.z;

		return true;
	}
	inline fvector2d ProjectWorldToScreen(fvector WorldLocation)
	{
		int width = GetSystemMetrics(SM_CXSCREEN);
		int height = GetSystemMetrics(SM_CYSCREEN);
		int screen_center_x = width / 2;
		int screen_center_y = height / 2;
		camera_postion = get_camera();
		D3DMATRIX temp_matrix = to_matrix(camera_postion.rotation);
		fvector vaxisx = fvector(temp_matrix.m[0][0], temp_matrix.m[0][1], temp_matrix.m[0][2]);
		fvector vaxisy = fvector(temp_matrix.m[1][0], temp_matrix.m[1][1], temp_matrix.m[1][2]);
		fvector vaxisz = fvector(temp_matrix.m[2][0], temp_matrix.m[2][1], temp_matrix.m[2][2]);
		fvector vdelta = WorldLocation - camera_postion.location;
		fvector vtransformed = fvector(vdelta.dot(vaxisy), vdelta.dot(vaxisz), vdelta.dot(vaxisx));

		if (vtransformed.z < 1) vtransformed.z = 1;

		return fvector2d(screen_center_x + vtransformed.x * ((screen_center_x / tanf(camera_postion.fov * (float)M_PI / 360.0f))) / vtransformed.z, screen_center_y - vtransformed.y * ((screen_center_x / tanf(camera_postion.fov * (float)M_PI / 360.0f))) / vtransformed.z);
	}


	fvector PredictHeadshot(fvector TargetPosition, fvector ComponentVelocity, fvector ComponentAcceleration, float player_distance, float ProjectileSpeed = 375.0f, float gravity = 9.8f, float Latency = 0.05f, float HeadOffset = 70.0f)
	{

		float TimeToTarget = player_distance / ProjectileSpeed;

		TimeToTarget += Latency;
		float bulletDrop = gravity * (TimeToTarget * TimeToTarget) * 0.5f; // 0.5

		TargetPosition.z += HeadOffset;

		fvector PredictedPosition;
		PredictedPosition.x = TargetPosition.x + TimeToTarget * ComponentVelocity.x + 0.5f * ComponentAcceleration.x * (TimeToTarget * TimeToTarget);
		PredictedPosition.y = TargetPosition.y + TimeToTarget * ComponentVelocity.y + 0.5f * ComponentAcceleration.y * (TimeToTarget * TimeToTarget);
		PredictedPosition.z = TargetPosition.z + TimeToTarget * ComponentVelocity.z + 0.5f * ComponentAcceleration.z * (TimeToTarget * TimeToTarget) - bulletDrop;


		PredictedPosition.z += 15.0f * TimeToTarget;

		return PredictedPosition;
	}
	fvector PredictLocation(fvector target, fvector targetVelocity, float projectileSpeed, float projectileGravityScale, float distance)
	{
		float horizontalTime = distance / projectileSpeed;
		float verticalTime = distance / projectileSpeed;

		target.x += targetVelocity.x * horizontalTime;
		target.y += targetVelocity.y * horizontalTime;
		target.z += targetVelocity.z * verticalTime +
			abs(-980 * projectileGravityScale) * 0.5f * (verticalTime * verticalTime);

		return target;
	}
	fvector prediction(fvector TargetPosition, fvector ComponentVelocity, float player_distance, float projectile_speed, float projectile_gravity)
	{

		float TimeToTarget = player_distance / projectile_speed;
		float bulletDrop = abs(projectile_gravity) * (TimeToTarget * TimeToTarget) * 0.5;

		return fvector
		{
			TargetPosition.x + TimeToTarget * ComponentVelocity.x,
			TargetPosition.y + TimeToTarget * ComponentVelocity.y,
			TargetPosition.z + TimeToTarget * ComponentVelocity.z + bulletDrop
		};
	}

	bool IsEnemyVisible(uintptr_t mesh)
	{
		auto Seconds = Kernel->read<double>(WorldCache.UWorld + 0x1A0);
		auto LastRenderTime = Kernel->read<float>(mesh + 0x32C);
		return Seconds - LastRenderTime <= 0.06f;
	}

	// 

	auto IsInScreen(fvector2d screen_location) -> bool
	{
		if (screen_location.x > 0 && screen_location.x < globals.ScreenWidth && screen_location.y > 0 && screen_location.y < globals.ScreenHeight) return true;
		else return false;
	}
}; gamehelper_t game_helper;

char* wchar_to_char(const wchar_t* pwchar)
{
	int currentCharIndex = 0;
	char currentChar = pwchar[currentCharIndex];

	while (currentChar != '\0')
	{
		currentCharIndex++;
		currentChar = pwchar[currentCharIndex];
	}

	const int charCount = currentCharIndex + 1;

	char* filePathC = (char*)malloc(sizeof(char) * charCount);

	for (int i = 0; i < charCount; i++)
	{
		char character = pwchar[i];

		*filePathC = character;

		filePathC += sizeof(char);

	}
	filePathC += '\0';

	filePathC -= (sizeof(char) * charCount);

	return filePathC;
}

ImVec2 calc_aim(const fvector2d& target2D, const ImVec2& screenCenter, int aimSpeed, float screenWidth, float screenHeight)
{
	float targetX = 0.0f;
	float targetY = 0.0f;

	if (target2D.x != 0)
	{
		if (target2D.x > screenCenter.x)
		{
			targetX = -(screenCenter.x - target2D.x);
			targetX /= aimSpeed;
			if (targetX + screenCenter.x > screenCenter.x * 2) targetX = 0;
		}

		if (target2D.x < screenCenter.x)
		{
			targetX = target2D.x - screenCenter.x;
			targetX /= aimSpeed;
			if (targetX + screenCenter.x < 0) targetX = 0;
		}
	}

	if (target2D.y != 0)
	{
		if (target2D.y > screenCenter.y)
		{
			targetY = -(screenCenter.y - target2D.y);
			targetY /= aimSpeed;
			if (targetY + screenCenter.y > screenCenter.y * 2) targetY = 0;
		}

		if (target2D.y < screenCenter.y)
		{
			targetY = target2D.y - screenCenter.y;
			targetY /= aimSpeed;
			if (targetY + screenCenter.y < 0) targetY = 0;
		}
	}

	return ImVec2(targetX, targetY);
}

