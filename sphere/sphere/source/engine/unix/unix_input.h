#ifndef UNIX_INPUT_H
#define UNIX_INPUT_H

#include "../../common/types.h"

#define MAX_KEY 256

// key defines
#define KEY_ESCAPE          1
#define KEY_F1              2
#define KEY_F2              3
#define KEY_F3              4
#define KEY_F4              5
#define KEY_F5              6
#define KEY_F6              7
#define KEY_F7              8
#define KEY_F8              9
#define KEY_F9              10
#define KEY_F10             11
#define KEY_F11             12
#define KEY_F12             13
#define KEY_TILDE           14
#define KEY_0               15
#define KEY_1               16
#define KEY_2               17
#define KEY_3               18
#define KEY_4               19
#define KEY_5               20
#define KEY_6               21
#define KEY_7               22
#define KEY_8               23
#define KEY_9               24
#define KEY_MINUS           25
#define KEY_EQUALS          26
#define KEY_BACKSPACE       27
#define KEY_TAB             28
#define KEY_A               29
#define KEY_B               30
#define KEY_C               31
#define KEY_D               32
#define KEY_E               33
#define KEY_F               34
#define KEY_G               35
#define KEY_H               36
#define KEY_I               37
#define KEY_J               38
#define KEY_K               39
#define KEY_L               40
#define KEY_M               41
#define KEY_N               42
#define KEY_O               43
#define KEY_P               44
#define KEY_Q               45
#define KEY_R               46
#define KEY_S               47
#define KEY_T               48
#define KEY_U               49
#define KEY_V               50
#define KEY_W               51
#define KEY_X               52
#define KEY_Y               53
#define KEY_Z               54
#define KEY_SHIFT           55
#define KEY_CAPSLOCK        56
#define KEY_NUMLOCK         57
#define KEY_SCROLLOCK       58
#define KEY_CTRL            59
#define KEY_ALT             60
#define KEY_SPACE           61
#define KEY_OPENBRACE       62
#define KEY_CLOSEBRACE      63
#define KEY_SEMICOLON       64
#define KEY_APOSTROPHE      65
#define KEY_COMMA           66
#define KEY_PERIOD          67
#define KEY_SLASH           68
#define KEY_BACKSLASH       69
#define KEY_ENTER           70
#define KEY_INSERT          71
#define KEY_DELETE          72
#define KEY_HOME            73
#define KEY_END             74
#define KEY_PAGEUP          75
#define KEY_PAGEDOWN        76
#define KEY_UP              77
#define KEY_RIGHT           78
#define KEY_DOWN            79
#define KEY_LEFT            80
#define KEY_NUM_0           81
#define KEY_NUM_1           82
#define KEY_NUM_2           83
#define KEY_NUM_3           84
#define KEY_NUM_4           85
#define KEY_NUM_5           86
#define KEY_NUM_6           87
#define KEY_NUM_7           88
#define KEY_NUM_8           89
#define KEY_NUM_9           90

#define MODKEY_NONE         0
#define MODKEY_CAPSLOCK     1
#define MODKEY_NUMLOCK      2
#define MODKEY_SCROLLOCK    3

#define MOUSE_LEFT          0
#define MOUSE_MIDDLE        1
#define MOUSE_RIGHT         2
#define MOUSE_WHEEL_UP      3
#define MOUSE_WHEEL_DOWN    4

#define JOYSTICK_AXIS_X     0
#define JOYSTICK_AXIS_Y     1
#define JOYSTICK_AXIS_Z     2
#define JOYSTICK_AXIS_R     3

#define PLAYER_1            0
#define PLAYER_2            1
#define PLAYER_3            2
#define PLAYER_4            3
#define PLAYER_KEY_MENU     4
#define PLAYER_KEY_UP       5
#define PLAYER_KEY_DOWN     6
#define PLAYER_KEY_LEFT     7
#define PLAYER_KEY_RIGHT    8
#define PLAYER_KEY_A        9
#define PLAYER_KEY_B        10
#define PLAYER_KEY_X        11
#define PLAYER_KEY_Y        12

void InitializeInput();
bool ResetInput();
void OnKeyDown(int key);
void OnKeyUp(int key);
bool RefreshInput();
void UpdateSystem();

// keyboard
bool GetToggleState(int key);
bool IsKeyPressed(int key);
void GetKeyStates(bool keys[MAX_KEY]);
bool AreKeysLeft();
int GetKey();

// mouse
void SetMousePosition(int x, int y);
int GetMouseX();
int GetMouseY();
bool IsMouseButtonPressed(int button);
int  GetMouseWheelEvent();
int  GetNumMouseWheelEvents();

// joystick
extern int   GetNumJoysticks();
extern int   GetNumJoystickAxes(int joy);
extern float GetJoystickAxis(int joy, int axis); // returns value in range [-1, 1]
extern int   GetNumJoystickButtons(int joy);
extern bool  IsJoystickButtonPressed(int joy, int button);

#endif
