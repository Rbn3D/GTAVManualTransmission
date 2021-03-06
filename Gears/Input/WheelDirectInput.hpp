#pragma once

#include <array>
#include <vector>
#include "DiJoyStick.h"
#include <unordered_map>

// https://stackoverflow.com/questions/24113864/what-is-the-right-way-to-use-a-guid-as-the-key-in-stdhash-map
namespace std {
    template<> struct hash<GUID>
    {
        size_t operator()(const GUID& guid) const noexcept {
            const std::uint64_t* p = reinterpret_cast<const std::uint64_t*>(&guid);
            std::hash<std::uint64_t> hash;
            return hash(p[0]) ^ hash(p[1]);
        }
    };
}

const int MAX_RGBBUTTONS = 128;
const int AVGSAMPLES = 2;
const int POVDIRECTIONS = 8;

class WheelDirectInput {
public:
    enum DIAxis {
        lX,
        lY,
        lZ,
        lRx,
        lRy,
        lRz,
        rglSlider0,
        rglSlider1,
        UNKNOWN_AXIS,
        SIZEOF_DIAxis
    };

    enum POV {
        N = 3600,
        NE = 4500,
        E = 9000,
        SE = 13500,
        S = 18000,
        SW = 22500,
        W = 27000,
        NW = 31500,
        SIZEOF_POV,
    };

    const std::array<POV, POVDIRECTIONS> POVDirections{
        N, NE, E, SE, S, SW, W, NW
    };

    const std::array<std::string, SIZEOF_DIAxis> DIAxisHelper {
        "lX",
        "lY",
        "lZ",
        "lRx",
        "lRy",
        "lRz",
        "rglSlider0",
        "rglSlider1",
        "UNKNOWN_AXIS"
    };

    WheelDirectInput();
    ~WheelDirectInput();
    bool PreInit();

    bool InitWheel();

    bool InitFFB(GUID guid, DIAxis ffAxis);
    void UpdateCenterSteering(GUID guid, DIAxis steerAxis);
    const DiJoyStick::Entry *FindEntryFromGUID(GUID guid);

    // Should be called every update()
    void Update();

    bool IsConnected(GUID device);
    bool IsButtonPressed(int btn, GUID device);
    bool IsButtonJustPressed(int btn, GUID device);
    bool IsButtonJustReleased(int btn, GUID device);
    bool WasButtonHeldForMs(int btn, GUID device, int millis);
    void UpdateButtonChangeStates();

    void SetConstantForce(GUID device, DIAxis ffAxis, int force);
    void SetDamper(GUID device, DIAxis ffAxis, int force);
    void SetCollision(GUID device, DIAxis ffAxis, int force);
    void StopEffects();

    DIAxis StringToAxis(std::string& axisString);

    int GetAxisValue(DIAxis axis, GUID device);
    float GetAxisSpeed(DIAxis axis, GUID device);

    std::vector<GUID> GetGuids();
    void PlayLedsDInput(GUID guid, const FLOAT currentRPM, const FLOAT rpmFirstLedTurnsOn, const FLOAT rpmRedLine);

private:
    void updateAxisSpeed();
    void createConstantForceEffect(DWORD axis, int numAxes, DIEFFECT &diEffect);
    void createDamperEffect(DWORD axis, int numAxes, DIEFFECT &diEffect);
    void createCollisionEffect(DWORD axis, int numAxes, DIEFFECT &diEffect);
    bool createEffects(GUID device, DIAxis ffAxis);
    int povDirectionToIndex(int povDirection);

    std::vector<GUID> foundGuids { GUID_NULL };

    DiJoyStick djs;
    LPDIRECTINPUT lpDi = nullptr;

    // TODO: Group these effects in classes?
    LPDIRECTINPUTEFFECT m_cfEffect = nullptr;
    DICONSTANTFORCE m_constantForceParams;

    LPDIRECTINPUTEFFECT m_dEffect = nullptr;
    DICONDITION m_damperParams;

    LPDIRECTINPUTEFFECT m_colEffect = nullptr;
    DIPERIODIC m_collisionParams;

    std::unordered_map<GUID, std::array<__int64, MAX_RGBBUTTONS>> rgbPressTime   { 0 };
    std::unordered_map<GUID, std::array<__int64, MAX_RGBBUTTONS>> rgbReleaseTime { 0 };
    std::unordered_map<GUID, std::array<bool, MAX_RGBBUTTONS>>	rgbButtonCurr { 0 };
    std::unordered_map<GUID, std::array<bool, MAX_RGBBUTTONS>>	rgbButtonPrev { 0 };
    std::unordered_map<GUID, std::array<__int64, POVDIRECTIONS>>	povPressTime   { 0 };
    std::unordered_map<GUID, std::array<__int64, POVDIRECTIONS>>	povReleaseTime { 0 };
    std::unordered_map<GUID, std::array<bool, POVDIRECTIONS>>		povButtonCurr { 0 };
    std::unordered_map<GUID, std::array<bool, POVDIRECTIONS>>		povButtonPrev { 0 };

    std::unordered_map<GUID, std::array<int, SIZEOF_DIAxis>> prevPosition { 0 };
    std::unordered_map<GUID, std::array<__int64, SIZEOF_DIAxis>> prevTime { 0 };
    std::unordered_map<GUID, std::array<std::array<float, AVGSAMPLES>, SIZEOF_DIAxis>> samples { 0 };

    std::unordered_map<GUID, std::array<int, SIZEOF_DIAxis>> averageIndex { 0 };
    std::unordered_map<GUID, std::array<bool, SIZEOF_DIAxis>> hasForceFeedback { false };
};
