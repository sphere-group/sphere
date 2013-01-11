#include <SDL.h>
#include <queue>
#include <map>

#include "unix_input.h"
#include "unix_video.h"
#include "unix_internal.h"

// map container for virtual keys
std::map<SDLKey, int> VirtualKeys;

// keyboard key queue
static std::queue<int> KeyQueue;

// keyboard state tables
static bool KeyBuffer[MAX_KEY];
static bool CurrentKeyBuffer[MAX_KEY];
static bool ModKeyStates[2];

// queue for mouse wheel events
static std::queue<int> MouseWheelQueue;

// mouse globals
int MouseX, MouseY;
bool MouseButton[3];

///////////////////////////////////////////////////////////

void InitializeInput()
{
  memset(KeyBuffer, false, sizeof(bool) * MAX_KEY);
  memset(CurrentKeyBuffer, false, sizeof(bool) * MAX_KEY);

  // initialize the virtual key map
  VirtualKeys[SDLK_ESCAPE]       = KEY_ESCAPE;
  VirtualKeys[SDLK_F1]           = KEY_F1;
  VirtualKeys[SDLK_F2]           = KEY_F2;
  VirtualKeys[SDLK_F3]           = KEY_F3;
  VirtualKeys[SDLK_F4]           = KEY_F4;
  VirtualKeys[SDLK_F5]           = KEY_F5;
  VirtualKeys[SDLK_F6]           = KEY_F6;
  VirtualKeys[SDLK_F7]           = KEY_F7;
  VirtualKeys[SDLK_F8]           = KEY_F8;
  VirtualKeys[SDLK_F9]           = KEY_F9;
  VirtualKeys[SDLK_F10]          = KEY_F10;
  VirtualKeys[SDLK_F11]          = KEY_F11;
  VirtualKeys[SDLK_F12]          = KEY_F12;
  VirtualKeys[SDLK_BACKQUOTE]    = KEY_TILDE;
  VirtualKeys[SDLK_0]            = KEY_0;
  VirtualKeys[SDLK_1]            = KEY_1;
  VirtualKeys[SDLK_2]            = KEY_2;
  VirtualKeys[SDLK_3]            = KEY_3;
  VirtualKeys[SDLK_4]            = KEY_4;
  VirtualKeys[SDLK_5]            = KEY_5;
  VirtualKeys[SDLK_6]            = KEY_6;
  VirtualKeys[SDLK_7]            = KEY_7;
  VirtualKeys[SDLK_8]            = KEY_8;
  VirtualKeys[SDLK_9]            = KEY_9;
  VirtualKeys[SDLK_MINUS]        = KEY_MINUS;
  VirtualKeys[SDLK_KP_MINUS]     = KEY_MINUS;
  VirtualKeys[SDLK_EQUALS]       = KEY_EQUALS;
  VirtualKeys[SDLK_KP_EQUALS]    = KEY_EQUALS;
  VirtualKeys[SDLK_BACKSPACE]    = KEY_BACKSPACE;
  VirtualKeys[SDLK_TAB]          = KEY_TAB;
  VirtualKeys[SDLK_a]            = KEY_A;
  VirtualKeys[SDLK_b]            = KEY_B;
  VirtualKeys[SDLK_c]            = KEY_C;
  VirtualKeys[SDLK_d]            = KEY_D;
  VirtualKeys[SDLK_e]            = KEY_E;
  VirtualKeys[SDLK_f]            = KEY_F;
  VirtualKeys[SDLK_g]            = KEY_G;
  VirtualKeys[SDLK_h]            = KEY_H;
  VirtualKeys[SDLK_i]            = KEY_I;
  VirtualKeys[SDLK_j]            = KEY_J;
  VirtualKeys[SDLK_k]            = KEY_K;
  VirtualKeys[SDLK_l]            = KEY_L;
  VirtualKeys[SDLK_m]            = KEY_M;
  VirtualKeys[SDLK_n]            = KEY_N;
  VirtualKeys[SDLK_o]            = KEY_O;
  VirtualKeys[SDLK_p]            = KEY_P;
  VirtualKeys[SDLK_q]            = KEY_Q;
  VirtualKeys[SDLK_r]            = KEY_R;
  VirtualKeys[SDLK_s]            = KEY_S;
  VirtualKeys[SDLK_t]            = KEY_T;
  VirtualKeys[SDLK_u]            = KEY_U;
  VirtualKeys[SDLK_v]            = KEY_V;
  VirtualKeys[SDLK_w]            = KEY_W;
  VirtualKeys[SDLK_x]            = KEY_X;
  VirtualKeys[SDLK_y]            = KEY_Y;
  VirtualKeys[SDLK_z]            = KEY_Z;
  VirtualKeys[SDLK_LSHIFT]       = KEY_SHIFT;
  VirtualKeys[SDLK_RSHIFT]       = KEY_SHIFT;
  VirtualKeys[SDLK_CAPSLOCK]     = KEY_CAPSLOCK;
  VirtualKeys[SDLK_NUMLOCK]      = KEY_NUMLOCK;
  VirtualKeys[SDLK_SCROLLOCK]    = KEY_SCROLLOCK;
  VirtualKeys[SDLK_LCTRL]        = KEY_CTRL;
  VirtualKeys[SDLK_RCTRL]        = KEY_CTRL;
  VirtualKeys[SDLK_LALT]         = KEY_ALT;
  VirtualKeys[SDLK_RALT]         = KEY_ALT;
  VirtualKeys[SDLK_SPACE]        = KEY_SPACE;
  VirtualKeys[SDLK_LEFTBRACKET]  = KEY_OPENBRACE;
  VirtualKeys[SDLK_RIGHTBRACKET] = KEY_CLOSEBRACE;
  VirtualKeys[SDLK_SEMICOLON]    = KEY_SEMICOLON;
  VirtualKeys[SDLK_QUOTE]        = KEY_APOSTROPHE;
  VirtualKeys[SDLK_COMMA]        = KEY_COMMA;
  VirtualKeys[SDLK_PERIOD]       = KEY_PERIOD;
  VirtualKeys[SDLK_SLASH]        = KEY_SLASH;
  VirtualKeys[SDLK_KP_DIVIDE]    = KEY_SLASH;
  VirtualKeys[SDLK_BACKSLASH]    = KEY_BACKSLASH;
  VirtualKeys[SDLK_RETURN]       = KEY_ENTER;
  VirtualKeys[SDLK_KP_ENTER]     = KEY_ENTER;
  VirtualKeys[SDLK_INSERT]       = KEY_INSERT;
  VirtualKeys[SDLK_DELETE]       = KEY_DELETE;
  VirtualKeys[SDLK_HOME]         = KEY_HOME;
  VirtualKeys[SDLK_END]          = KEY_END;
  VirtualKeys[SDLK_PAGEUP]       = KEY_PAGEUP;
  VirtualKeys[SDLK_PAGEDOWN]     = KEY_PAGEDOWN;
  VirtualKeys[SDLK_UP]           = KEY_UP;
  VirtualKeys[SDLK_RIGHT]        = KEY_RIGHT;
  VirtualKeys[SDLK_DOWN]         = KEY_DOWN;
  VirtualKeys[SDLK_LEFT]         = KEY_LEFT;
  VirtualKeys[SDLK_KP0]          = KEY_NUM_0;
  VirtualKeys[SDLK_KP1]          = KEY_NUM_1;
  VirtualKeys[SDLK_KP2]          = KEY_NUM_2;
  VirtualKeys[SDLK_KP3]          = KEY_NUM_3;
  VirtualKeys[SDLK_KP4]          = KEY_NUM_4;
  VirtualKeys[SDLK_KP5]          = KEY_NUM_5;
  VirtualKeys[SDLK_KP6]          = KEY_NUM_6;
  VirtualKeys[SDLK_KP7]          = KEY_NUM_7;
  VirtualKeys[SDLK_KP8]          = KEY_NUM_8;
  VirtualKeys[SDLK_KP9]          = KEY_NUM_9;

  // initialize mouse buttons
  MouseButton[MOUSE_LEFT]   = false;
  MouseButton[MOUSE_MIDDLE] = false;
  MouseButton[MOUSE_RIGHT]  = false;

  // initialize modifier states
  ModKeyStates[MODKEY_CAPSLOCK]  = false;
  ModKeyStates[MODKEY_NUMLOCK]   = false;
  ModKeyStates[MODKEY_SCROLLOCK] = false;
}

///////////////////////////////////////////////////////////

bool RefreshInput()
{
  // update currently pressed keys
  memcpy(KeyBuffer, CurrentKeyBuffer, sizeof(bool) * MAX_KEY);
}

///////////////////////////////////////////////////////////

bool ResetInput()
{
  memset(KeyBuffer, false, sizeof(bool) * MAX_KEY);
  memset(CurrentKeyBuffer, false, sizeof(bool) * MAX_KEY);
  return true;
}

///////////////////////////////////////////////////////////

void UpdateSystem()
{
  SDL_Event event;
  std::map<SDLKey, int>::iterator it;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        exit(0);
        break;

        ////////////////////////////////////////////////////////////////////////////

      case SDL_KEYDOWN:
        it = VirtualKeys.find(event.key.keysym.sym);
        if (it != VirtualKeys.end())
          OnKeyDown(it->second);
        break;

        ////////////////////////////////////////////////////////////////////////////

      case SDL_KEYUP:
        it = VirtualKeys.find(event.key.keysym.sym);
        if (it != VirtualKeys.end())
          OnKeyUp(it->second);
        break;

        ////////////////////////////////////////////////////////////////////////////

      case SDL_MOUSEMOTION:
        OnMouseMove(event.motion.x, event.motion.y);
        break;

        ////////////////////////////////////////////////////////////////////////////

      case SDL_MOUSEBUTTONDOWN:
        OnMouseDown(event.button.button);
        break;

        ////////////////////////////////////////////////////////////////////////////

      case SDL_MOUSEBUTTONUP:
        OnMouseUp(event.button.button);
        break;
    }
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
  switch(button)
  {
    case SDL_BUTTON_LEFT:      MouseButton[MOUSE_LEFT]   = true;       break;
    case SDL_BUTTON_MIDDLE:    MouseButton[MOUSE_MIDDLE] = true;       break;
    case SDL_BUTTON_RIGHT:     MouseButton[MOUSE_RIGHT]  = true;       break;
    case SDL_BUTTON_WHEELUP:   MouseWheelQueue.push(MOUSE_WHEEL_UP);   break;
    case SDL_BUTTON_WHEELDOWN: MouseWheelQueue.push(MOUSE_WHEEL_DOWN); break;
  }
}

////////////////////////////////////////////////////////////////////////////////

void OnMouseUp(int button)
{
  switch(button)
  {
    case SDL_BUTTON_LEFT:      MouseButton[MOUSE_LEFT]   = false; break;
    case SDL_BUTTON_MIDDLE:    MouseButton[MOUSE_MIDDLE] = false; break;
    case SDL_BUTTON_RIGHT:     MouseButton[MOUSE_RIGHT]  = false; break;
  }
}

///////////////////////////////////////////////////////////

void OnKeyDown(int key)
{
  KeyQueue.push(key);
  CurrentKeyBuffer[key] = true;

  // update modifier states
  if (key == KEY_CAPSLOCK)
    ModKeyStates[MODKEY_CAPSLOCK]  = !ModKeyStates[MODKEY_CAPSLOCK];

  else if (key == KEY_NUMLOCK)
    ModKeyStates[MODKEY_NUMLOCK]   = !ModKeyStates[MODKEY_NUMLOCK];

  else if (key == KEY_SCROLLOCK)
    ModKeyStates[MODKEY_SCROLLOCK] = !ModKeyStates[MODKEY_SCROLLOCK];

  switch(key) {
    case KEY_F10:
      ToggleFullScreen();
    break;

    case KEY_F11:
      ToggleFPS();
    break;

    case KEY_F12:
      ShouldTakeScreenshot = true;
    break;
  }
}

///////////////////////////////////////////////////////////

void OnKeyUp(int key)
{
  CurrentKeyBuffer[key] = false;
}

///////////////////////////////////////////////////////////

bool GetToggleState(int key)
{
  return ModKeyStates[key];
}

///////////////////////////////////////////////////////////

bool IsKeyPressed(int key)
{
  if (key >= 0 && key < MAX_KEY) {
    UpdateSystem();
    return KeyBuffer[key];
  }

  return false;
}

///////////////////////////////////////////////////////////

void GetKeyStates(bool keys[MAX_KEY]) {
  UpdateSystem();
  for (int i = 0; i < MAX_KEY; ++i) {
    keys[i] = KeyBuffer[i];
  }
}

///////////////////////////////////////////////////////////

bool AreKeysLeft() {
  UpdateSystem();
  return !KeyQueue.empty();
}

///////////////////////////////////////////////////////////

int GetKey()
{
  UpdateSystem();
  while (KeyQueue.empty() == true)
    UpdateSystem();

  int key = KeyQueue.front();
  KeyQueue.pop();

  return key;
}

// MOUSE SUPPORT

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
  return MouseButton[button];
}

////////////////////////////////////////////////////////////////////////////////

void SetMousePosition(int x, int y)
{
  SDL_WarpMouse(x, y);
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



// JOYSTICK SUPPORT

///////////////////////////////////////////////////////////

int GetNumJoysticks()
{
  return SDL_NumJoysticks();
}

///////////////////////////////////////////////////////////

int GetNumJoystickAxes(int joy_index)
{
  if (joy_index < 0 || joy_index >= SDL_NumJoysticks())
    return 0;

  SDL_Joystick* joy = SDL_JoystickOpen(joy_index);
  if (joy == NULL)
    return 0;

  return SDL_JoystickNumAxes(joy);
}

///////////////////////////////////////////////////////////

float GetJoystickAxis(int joy_index, int axis)
{
  if (joy_index < 0 || joy_index >= SDL_NumJoysticks() || axis < 0 || axis > 3)
    return 0;

  SDL_Joystick* joy = SDL_JoystickOpen(joy_index);
  if (joy == NULL)
    return 0;

  Sint16 value = SDL_JoystickGetAxis(joy, axis);

  // GetAxis returns between -32768 to 32768 so we map that onto -1.0 to 1.0
  return ((float) value / (float) 32768.0);
}

///////////////////////////////////////////////////////////

int GetNumJoystickButtons(int joy_index)
{
  if (joy_index < 0 || joy_index >= SDL_NumJoysticks())
    return 0;

  SDL_Joystick* joy = SDL_JoystickOpen(joy_index);
  if (joy == NULL)
    return 0;

  return SDL_JoystickNumButtons(joy);
}

///////////////////////////////////////////////////////////

bool IsJoystickButtonPressed(int joy_index, int button)
{
  if (joy_index < 0 || joy_index >= SDL_NumJoysticks())
    return false;

  SDL_Joystick* joy = SDL_JoystickOpen(joy_index);
  if (joy == NULL)
    return false;

  if (button < 0 || button >= SDL_JoystickNumButtons(joy))
    return false;

  return (SDL_JoystickGetButton(joy, button) == 1);
}

///////////////////////////////////////////////////////////
