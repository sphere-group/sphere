#ifndef WIN32_INPUT_HPP
#define WIN32_INPUT_HPP

#include "../../common/types.h"
#include "../../common/input_constants.h"


// general
extern bool RefreshInput();
extern bool ResetInput();

// keyboard
extern bool GetToggleState(int key);
extern bool IsKeyPressed(int key);
extern void GetKeyStates(bool keys[MAX_KEY]);

// keyboard key queue
extern bool AreKeysLeft();
extern int  GetKey();

// mouse
extern void SetMousePosition(int x, int y);
extern int  GetMouseX();
extern int  GetMouseY();
extern bool IsMouseButtonPressed(int button);
extern int  GetMouseWheelEvent();
extern int  GetNumMouseWheelEvents();

// joystick
extern int   GetNumJoysticks();
extern int   GetNumJoystickAxes(int joy);
extern float GetJoystickAxis(int joy, int axis); // returns value in range [-1, 1]
extern int   GetNumJoystickButtons(int joy);
extern bool  IsJoystickButtonPressed(int joy, int button);
#endif
