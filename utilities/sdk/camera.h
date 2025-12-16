#include "../aimbot/keybind.h"

struct camera_position_s {
    fvector location{};
    fvector rotation{};
    float fov{};
};
inline camera_position_s camera_postion{};

struct FNRot {
    double A;
    char Pad0008[24];
    double B;
    char Pad0028[424];
    double C;
};

#define deg2Rad(angleDegrees) ((angleDegrees) * M_PI / 180.0)
#define rad2Deg(angleRadians) ((angleRadians) * 180.0 / M_PI)

struct FMinimalViewInfo
{
    FMinimalViewInfo() : Location(), Rotation(), FOV(FOV) {}
    FMinimalViewInfo(fvector Location, fvector Rotation, float FOV) : Location(Location), Rotation(Rotation), FOV(FOV) {}

    fvector Location, Rotation;
    float FOV;
};

inline FMinimalViewInfo m_camera_information{};


typedef struct _D3DMATRIX1 {
    union {
        struct {
            double _11, _12, _13, _14;
            double _21, _22, _23, _24;
            double _31, _32, _33, _34;
            double _41, _42, _43, _44;

        };
        double m[4][4];
    };
} D3DMATRIX1;

typedef struct D3DXMATRIX1 {
    DOUBLE _ij;
} D3DXMATRIX1, * LPD3DXMATRIX1;

struct FPlane : public fvector
{
    double W;
};

class FMatrix
{
public:
    double m[4][4];
    FPlane XPlane, YPlane, ZPlane, WPlane;

    FMatrix() : XPlane(), YPlane(), ZPlane(), WPlane() {}
    FMatrix(FPlane XPlane, FPlane YPlane, FPlane ZPlane, FPlane WPlane)
        : XPlane(XPlane), YPlane(YPlane), ZPlane(ZPlane), WPlane(WPlane) {
    }
};

uintptr_t viewStates;
uintptr_t viewMatrices;
D3DMATRIX1 invProjMatrix;

class Camera
{
public:
    void normalize(fvector& in)
    {
        while (in.x > 180.0f)
            in.x -= 360.0f;

        while (in.x < -180.0f)
            in.x += 360.0f;

        while (in.y > 180.0f)
            in.y -= 360.0f;

        while (in.y < -180.0f)
            in.y += 360.0f;

        in.z = 0;
    }

    auto calc_angle(fvector LocalPos, fvector WorldPos) -> fvector {
        fvector RelativePos = WorldPos - LocalPos;
        float yaw = atan2(RelativePos.y, RelativePos.x) * 180.0f / M_PI;
        float pitch = -((acos(RelativePos.z / LocalPos.distance(WorldPos)) * 180.0f / M_PI) - 90.0f);

        return fvector(pitch, yaw, 0);
    }

}; inline Camera* camera;

std::string string_To_UTF8(const std::string& str)
{
    int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

    wchar_t* pwBuf = new wchar_t[nwLen + 1];
    ZeroMemory(pwBuf, nwLen * 2 + 2);

    ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

    int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

    char* pBuf = new char[nLen + 1];
    ZeroMemory(pBuf, nLen + 1);

    ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

    std::string retStr(pBuf);

    delete[]pwBuf;
    delete[]pBuf;

    pwBuf = NULL;
    pBuf = NULL;

    return retStr;
}

inline std::wstring MBytesToWString(const char* lpcszString)
{
    int len = strlen(lpcszString);
    int unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, NULL, 0);
    wchar_t* pUnicode = new wchar_t[unicodeLen + 1];
    memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
    ::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, (LPWSTR)pUnicode, unicodeLen);
    std::wstring wString = (wchar_t*)pUnicode;
    delete[] pUnicode;
    return wString;
}
inline std::string WStringToUTF8(const wchar_t* lpwcszWString)
{
    char* pElementText;
    int iTextLen = ::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, NULL, 0, NULL, NULL);
    pElementText = new char[iTextLen + 1];
    memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
    ::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, pElementText, iTextLen, NULL, NULL);
    std::string strReturn(pElementText);
    delete[] pElementText;
    return strReturn;
}