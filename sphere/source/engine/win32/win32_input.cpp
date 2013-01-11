#include <windows.h>
#include <mmsystem.h>
#include <queue>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include "win32_input.hpp"
#include "win32_internal.hpp"
#include "../../common/minmax.hpp"

static HWND          SphereWindow = NULL;
static SPHERECONFIG* Config = NULL;

// mapping from Sphere key codes to Windows key codes
int SphereToWindows[MAX_KEY] =
    {
        0,
        VK_ESCAPE,
        VK_F1,
        VK_F2,
        VK_F3,
        VK_F4,
        VK_F5,
        VK_F6,
        VK_F7,
        VK_F8,
        VK_F9,
        VK_F10,
        VK_F11,
        VK_F12,
        0x00C0,  // Windows 2000: VK_OEM_3
        0x0030,
        0x0031,
        0x0032,
        0x0033,
        0x0034,
        0x0035,
        0x0036,
        0x0037,
        0x0038,
        0x0039,
        0x00BD,  // Windows 2000: VK_OEM_MINUS,
        0x00BB,  // Windows 2000: VK_EQUALS,
        VK_BACK,
        VK_TAB,
        0x0041,  // a
        0x0042,  // b
        0x0043,  // c
        0x0044,  // d
        0x0045,  // e
        0x0046,  // f
        0x0047,  // g
        0x0048,  // h
        0x0049,  // i
        0x004A,  // j
        0x004B,  // k
        0x004C,  // l
        0x004D,  // m
        0x004E,  // n
        0x004F,  // o
        0x0050,  // p
        0x0051,  // q
        0x0052,  // r
        0x0053,  // s
        0x0054,  // t
        0x0055,  // u
        0x0056,  // v
        0x0057,  // w
        0x0058,  // x
        0x0059,  // y
        0x005A,  // z
        VK_SHIFT,
        VK_CAPITAL, // caps lock
        VK_NUMLOCK, // num lock
        VK_SCROLL,  // scroll lock
        VK_CONTROL,
        VK_MENU,
        VK_SPACE,
        0x00DB,  // Windows 2000: VK_OEM_4
        0x00DD,  // Windows 2000: VK_OEM_6
        186,     // colon
        0x00DE,  // Windows 2000: VK_OEM_7
        0x00BC,  // Windows 2000: VK_OEM_COMMA
        0x00BE,  // Windows 2000: VK_OEM_PERIOD
        0x00BF,  // Windows 2000: VK_OEM_2
        0x00DC,  // Windows 2000: VK_OEM_5
        VK_RETURN,
        VK_INSERT,
        VK_DELETE,
        VK_HOME,
        VK_END,
        VK_PRIOR,
        VK_NEXT,
        VK_UP,
        VK_RIGHT,
        VK_DOWN,
        VK_LEFT,

        VK_NUMPAD0,
        VK_NUMPAD1,
        VK_NUMPAD2,
        VK_NUMPAD3,
        VK_NUMPAD4,
        VK_NUMPAD5,
        VK_NUMPAD6,
        VK_NUMPAD7,
        VK_NUMPAD8,
        VK_NUMPAD9
    };
int WindowsToSphere[MAX_KEY]; // build dynamically
// keyboard state tables (accessed with virtual keys)
static byte CurrentKeyBuffer[MAX_KEY];
static byte KeyBuffer[MAX_KEY];
static byte ModKeyStates[3];

// keyboard key queue (virtual keys also)
static std::queue<int> KeyQueue;

// queue for mouse wheel events
static std::queue<int> MouseWheelQueue;

// mouse
static int MouseX;
static int MouseY;
static bool MouseState[3];

// joystick
struct Joystick
{
    UINT id;
    UINT minX;
    UINT maxX;
    UINT minY;
    UINT maxY;
    UINT minZ;
    UINT maxZ;
    UINT minR;
    UINT maxR;
    UINT minU;
    UINT maxU;
    UINT minV;
    UINT maxV;
    UINT numAxes;
    UINT numButtons;

    float axes[JOYSTICK_MAX_AXIS+1];
    UINT buttons;
};
std::vector<Joystick> Joysticks;

// These functions rely on the window handler to modify the keybuffer values
// and mouse state

////////////////////////////////////////////////////////////////////////////////
void TryJoystick(UINT id)
{

    JOYINFOEX jinfo_ex;
    jinfo_ex.dwSize  = sizeof(jinfo_ex);
    jinfo_ex.dwFlags = JOY_RETURNBUTTONS | JOY_RETURNX | JOY_RETURNY | JOY_RETURNZ | JOY_RETURNR | JOY_RETURNU | JOY_RETURNV;
    
    if (joyGetPosEx(id, &jinfo_ex) != JOYERR_NOERROR)
    {
        return;
    }
    
    JOYCAPS jcaps;
    if (joyGetDevCaps(id, &jcaps, sizeof(jcaps)) != JOYERR_NOERROR)
    {
        return;
    }
    
    Joystick j;
    
    j.id         = id;
    j.minX       = jcaps.wXmin;
    j.maxX       = jcaps.wXmax;
    j.minY       = jcaps.wYmin;
    j.maxY       = jcaps.wYmax;
    j.minZ       = jcaps.wZmin;
    j.maxZ       = jcaps.wZmax;
    j.minR       = jcaps.wRmin;
    j.maxR       = jcaps.wRmax;
    j.minU       = jcaps.wUmin;
    j.maxU       = jcaps.wUmax;
    j.minV       = jcaps.wVmin;
    j.maxV       = jcaps.wVmax;
    j.axes[0]    = 0;
    j.axes[1]    = 0;
    j.axes[2]    = 0;
    j.axes[3]    = 0;
    j.axes[4]    = 0;
    j.axes[5]    = 0;
    j.buttons    = 0;
    j.numButtons = jcaps.wNumButtons;
    j.numAxes    = jcaps.wNumAxes;
    
    Joysticks.push_back(j);
}

bool InitInput(HWND window, SPHERECONFIG* config)
{

    unsigned int i;

    // build mapping from Windows to Sphere keys
    for (i = 0; i < MAX_KEY; ++i)
    {
        int k = SphereToWindows[i];
        if (k >= 0 && k < MAX_KEY)
        {
            WindowsToSphere[k] = i;
        }
    }
    
    SphereWindow = window;
    Config = config;

    // try to initialize joysticks (only the plugged in and valid ones will be initialized)
    UINT num = joyGetNumDevs();

    for (i = 0; i < num; ++i)
    {
        TryJoystick(i);
    }
    
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool ResetInput()
{
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool CloseInput(void)
{
    return true;
}

////////////////////////////////////////////////////////////////////////////////
void OnKeyDown(int virtual_key)
{
    if (virtual_key >= 0 && virtual_key < MAX_KEY)
    {
        int key = WindowsToSphere[virtual_key];
        CurrentKeyBuffer[key] = 1;
        
        KeyQueue.push(key);
    }
}
////////////////////////////////////////////////////////////////////////////////
void OnKeyUp(int virtual_key)
{
    if (virtual_key >= 0 && virtual_key < MAX_KEY)
    {
        int key = WindowsToSphere[virtual_key];
        CurrentKeyBuffer[key] = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
void OnMouseMove(int x, int y)
{
    MouseX = x;
    MouseY = y;
}

////////////////////////////////////////////////////////////////////////////////
void OnMouseDown(int button)
{
    MouseState[button] = true;
}

////////////////////////////////////////////////////////////////////////////////
void OnMouseUp(int button)
{
    MouseState[button] = false;
}

////////////////////////////////////////////////////////////////////////////////
void OnMouseWheel(int dist)
{
    if (dist > 0)
        MouseWheelQueue.push(MOUSE_WHEEL_UP);
    else if (dist < 0)
        MouseWheelQueue.push(MOUSE_WHEEL_DOWN);
}

////////////////////////////////////////////////////////////////////////////////
bool RefreshInput()
{
    UpdateSystem();

    // update modifier keys
    ModKeyStates[MODKEY_CAPSLOCK]   = GetKeyState(VK_CAPITAL) & 0xFFFF;
    ModKeyStates[MODKEY_NUMLOCK]    = GetKeyState(VK_NUMLOCK) & 0xFFFF;
    ModKeyStates[MODKEY_SCROLLOCK]  = GetKeyState(VK_SCROLL)  & 0xFFFF;

    // update currently pressed keys
    memcpy(KeyBuffer, CurrentKeyBuffer, MAX_KEY);
    
    // update joysticks
    for (unsigned int i = 0; i < Joysticks.size(); ++i)
    {
        Joystick &j = Joysticks[i];
        JOYINFOEX jinfo_ex;
        jinfo_ex.dwSize  = sizeof(jinfo_ex);
        jinfo_ex.dwFlags = JOY_RETURNBUTTONS | JOY_RETURNX | JOY_RETURNY | JOY_RETURNZ | JOY_RETURNR |JOY_RETURNU | JOY_RETURNV;
        
        if (joyGetPosEx(j.id, &jinfo_ex) == JOYERR_NOERROR)
        {
            j.axes[JOYSTICK_AXIS_X] = float(jinfo_ex.dwXpos - j.minX) / (j.maxX - j.minX) * 2 - 1;
            j.axes[JOYSTICK_AXIS_Y] = float(jinfo_ex.dwYpos - j.minY) / (j.maxY - j.minY) * 2 - 1;
            j.axes[JOYSTICK_AXIS_Z] = float(jinfo_ex.dwZpos - j.minZ) / (j.maxZ - j.minZ) * 2 - 1;
            j.axes[JOYSTICK_AXIS_R] = float(jinfo_ex.dwRpos - j.minR) / (j.maxR - j.minR) * 2 - 1;
            j.axes[JOYSTICK_AXIS_U] = float(jinfo_ex.dwUpos - j.minU) / (j.maxU - j.minU) * 2 - 1;
            j.axes[JOYSTICK_AXIS_V] = float(jinfo_ex.dwVpos - j.minV) / (j.maxV - j.minV) * 2 - 1;
            j.buttons = jinfo_ex.dwButtons;
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool GetToggleState(int key)
{
    // find the Sphere key code that corresponds with the virtual key
    return (ModKeyStates[key] != 0);
}

////////////////////////////////////////////////////////////////////////////////
bool IsKeyPressed(int key)
{
    // find the Sphere key code that corresponds with the virtual key
    return (KeyBuffer[key] != 0);
}

////////////////////////////////////////////////////////////////////////////////
void GetKeyStates(bool keys[MAX_KEY])
{
    UpdateSystem();
    for (int i = MAX_KEY -1 ; i>=0 ; --i)
    {
        keys[i] = (KeyBuffer[i] != 0);
    }
}

////////////////////////////////////////////////////////////////////////////////
bool AreKeysLeft()
{
    UpdateSystem();
    return (!KeyQueue.empty());
}

////////////////////////////////////////////////////////////////////////////////
int GetKey()
{
    UpdateSystem();
    while (KeyQueue.empty() == true)
        UpdateSystem();

    int key = KeyQueue.front();
    KeyQueue.pop();
    return key;
}



// MOUSE

////////////////////////////////////////////////////////////////////////////////
void SetMousePosition(int x, int y)
{
    POINT p = { x, y };
    ClientToScreen(SphereWindow, &p);
    SetCursorPos(p.x, p.y);  // will send WM_MOUSEMOVE
}

////////////////////////////////////////////////////////////////////////////////
int GetMouseX()
{
    UpdateSystem();
    return MouseX;
}

////////////////////////////////////////////////////////////////////////////////
int GetMouseY()
{
    UpdateSystem();
    return MouseY;
}

////////////////////////////////////////////////////////////////////////////////
bool IsMouseButtonPressed(int button)
{
    return MouseState[button];
}

////////////////////////////////////////////////////////////////////////////////
int GetMouseWheelEvent()
{
    UpdateSystem();
    while (MouseWheelQueue.empty() == true)
        UpdateSystem();
    
    int mw_event = MouseWheelQueue.front();
    MouseWheelQueue.pop();
    
    return mw_event;
}

////////////////////////////////////////////////////////////////////////////////
int GetNumMouseWheelEvents()
{
    UpdateSystem();
    return MouseWheelQueue.size();
}



// JOYSTICKS

////////////////////////////////////////////////////////////////////////////////
int GetNumJoysticks()
{
    return int(Joysticks.size());
}

////////////////////////////////////////////////////////////////////////////////
float GetJoystickAxis(int joy, int axis)
{
    if (joy < 0 || joy >= GetNumJoysticks() || axis < 0 || axis > JOYSTICK_MAX_AXIS)
        return 0;
    return Joysticks[joy].axes[axis];
}

////////////////////////////////////////////////////////////////////////////////
int GetNumJoystickAxes(int joy)
{
    if (joy >= 0 && joy < GetNumJoysticks())
    {
        return Joysticks[joy].numAxes;
    }
    else
    {
        return 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
int GetNumJoystickButtons(int joy)
{
    if (joy >= 0 && joy < GetNumJoysticks())
    {
        return Joysticks[joy].numButtons;
    }
    else
    {
        return 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
bool IsJoystickButtonPressed(int joy, int button)
{
    if (joy >= 0 && joy < GetNumJoysticks())
    {
        return (Joysticks[joy].buttons & (1 << button)) != 0;
    }
    else
    {
        return 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
