#include "unix_time.h"
#include "../sphere.hpp"

#include <stdlib.h>
#include <unistd.h>
#include <cstring>

#include "unix_internal.h"
#include "unix_filesystem.h"
#include "unix_audio.h"
#include "unix_input.h"
#include "unix_sphere_config.h"
#include "../PlayerConfig.hpp"

#ifndef DATADIR
#define DATADIR "."
#endif

// should this just be FILENAME_MAX?
#define MAX_UNIX_PATH 256

static char unix_data_dir[MAX_UNIX_PATH] = DATADIR;
static char* original_directory;

////////////////////////////////////////////////////////////////////////////////

int KeyStringToKeyCode(const char* key_string)
{
    if (strlen(key_string) == strlen("KEY_X"))
    {
        if (memcmp(key_string, "KEY_", strlen("KEY_")) == 0)
        {
            switch (key_string[strlen("KEY_")])
            {
                case 'A': return KEY_A;
                case 'B': return KEY_B;
                case 'C': return KEY_C;
                case 'D': return KEY_D;
                case 'E': return KEY_E;
                case 'F': return KEY_F;
                case 'G': return KEY_G;
                case 'H': return KEY_H;
                case 'I': return KEY_I;
                case 'J': return KEY_J;
                case 'K': return KEY_K;
                case 'L': return KEY_L;
                case 'M': return KEY_M;
                case 'N': return KEY_N;
                case 'O': return KEY_O;
                case 'P': return KEY_P;
                case 'Q': return KEY_Q;
                case 'R': return KEY_R;
                case 'S': return KEY_S;
                case 'T': return KEY_T;
                case 'U': return KEY_U;
                case 'V': return KEY_V;
                case 'W': return KEY_W;
                case 'X': return KEY_X;
                case 'Y': return KEY_Y;
                case 'Z': return KEY_Z;
                case '0': return KEY_0;
                case '1': return KEY_1;
                case '2': return KEY_2;
                case '3': return KEY_3;
                case '4': return KEY_4;
                case '5': return KEY_5;
                case '6': return KEY_6;
                case '7': return KEY_7;
                case '8': return KEY_8;
                case '9': return KEY_9;
                default:  return -1;
            }
        }
    }

    if (strlen(key_string) == strlen("KEY_NUM_X"))
    {
        if (memcmp(key_string, "KEY_NUM_", strlen("KEY_NUM_")) == 0)
        {
            switch (key_string[strlen("KEY_NUM_")])
            {
                case '0': return KEY_NUM_0;
                case '1': return KEY_NUM_1;
                case '2': return KEY_NUM_2;
                case '3': return KEY_NUM_3;
                case '4': return KEY_NUM_4;
                case '5': return KEY_NUM_5;
                case '6': return KEY_NUM_6;
                case '7': return KEY_NUM_7;
                case '8': return KEY_NUM_8;
                case '9': return KEY_NUM_9;
                default:  return -1;
            }
        }
    }

    if (strcmp(key_string, "KEY_UP")            == 0) return KEY_UP;
    if (strcmp(key_string, "KEY_DOWN")          == 0) return KEY_DOWN;
    if (strcmp(key_string, "KEY_LEFT")          == 0) return KEY_LEFT;
    if (strcmp(key_string, "KEY_RIGHT")         == 0) return KEY_RIGHT;
    if (strcmp(key_string, "KEY_TILDE")         == 0) return KEY_TILDE;
    if (strcmp(key_string, "KEY_MINUS")         == 0) return KEY_MINUS;
    if (strcmp(key_string, "KEY_EQUALS")        == 0) return KEY_EQUALS;
    if (strcmp(key_string, "KEY_SPACE")         == 0) return KEY_SPACE;
    if (strcmp(key_string, "KEY_OPENBRACE")     == 0) return KEY_OPENBRACE;
    if (strcmp(key_string, "KEY_CLOSEBRACE")    == 0) return KEY_CLOSEBRACE;
    if (strcmp(key_string, "KEY_SEMICOLON")     == 0) return KEY_SEMICOLON;
    if (strcmp(key_string, "KEY_COMMA")         == 0) return KEY_COMMA;
    if (strcmp(key_string, "KEY_APOSTROPHE")    == 0) return KEY_APOSTROPHE;
    if (strcmp(key_string, "KEY_PERIOD")        == 0) return KEY_PERIOD;
    if (strcmp(key_string, "KEY_SLASH")         == 0) return KEY_SLASH;
    if (strcmp(key_string, "KEY_BACKSLASH")     == 0) return KEY_BACKSLASH;
    if (strcmp(key_string, "KEY_SHIFT")         == 0) return KEY_SHIFT;
    if (strcmp(key_string, "KEY_CAPSLOCK")      == 0) return KEY_CAPSLOCK;
    if (strcmp(key_string, "KEY_NUMLOCK")       == 0) return KEY_NUMLOCK;
    if (strcmp(key_string, "KEY_SCROLLOCK")     == 0) return KEY_SCROLLOCK;
    if (strcmp(key_string, "KEY_CTRL")          == 0) return KEY_CTRL;
    if (strcmp(key_string, "KEY_ALT")           == 0) return KEY_ALT;
    if (strcmp(key_string, "KEY_ENTER")         == 0) return KEY_ENTER;
    if (strcmp(key_string, "KEY_INSERT")        == 0) return KEY_INSERT;
    if (strcmp(key_string, "KEY_DELETE")        == 0) return KEY_DELETE;
    if (strcmp(key_string, "KEY_HOME")          == 0) return KEY_HOME;
    if (strcmp(key_string, "KEY_END")           == 0) return KEY_END;
    if (strcmp(key_string, "KEY_PAGEUP")        == 0) return KEY_PAGEUP;
    if (strcmp(key_string, "KEY_PAGEDOWN")      == 0) return KEY_PAGEDOWN;
    if (strcmp(key_string, "KEY_BACKSPACE")     == 0) return KEY_BACKSPACE;
    if (strcmp(key_string, "KEY_TAB")           == 0) return KEY_TAB;
    if (strcmp(key_string, "KEY_ESCAPE")        == 0) return KEY_ESCAPE;
    if (strcmp(key_string, "KEY_F1")            == 0) return KEY_F1;
    if (strcmp(key_string, "KEY_F2")            == 0) return KEY_F2;
    if (strcmp(key_string, "KEY_F3")            == 0) return KEY_F3;
    if (strcmp(key_string, "KEY_F4")            == 0) return KEY_F4;
    if (strcmp(key_string, "KEY_F5")            == 0) return KEY_F5;
    if (strcmp(key_string, "KEY_F6")            == 0) return KEY_F6;
    if (strcmp(key_string, "KEY_F7")            == 0) return KEY_F7;
    if (strcmp(key_string, "KEY_F8")            == 0) return KEY_F8;
    if (strcmp(key_string, "KEY_F9")            == 0) return KEY_F9;
    if (strcmp(key_string, "KEY_F10")           == 0) return KEY_F10;
    if (strcmp(key_string, "KEY_F11")           == 0) return KEY_F11;
    if (strcmp(key_string, "KEY_F12")           == 0) return KEY_F12;
    return -1;
}

////////////////////////////////////////////////////////////////////////////////

static void LoadSphereConfiguration(SPHERECONFIG* config)
{
  // Loads configuration settings
  LoadSphereConfig(config, (GetSphereDirectory() + "/engine.ini").c_str());
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char* argv[])
{
    SetSphereDirectory();

    // load the configuration settings, then save it for future reference
    SPHERECONFIG config;
    LoadSphereConfiguration(&config);

    for (int i = 0; i < 4; i++)
    {
      SetPlayerConfig(i,
                KeyStringToKeyCode(config.player_configurations[i].key_menu_str.c_str()),
                KeyStringToKeyCode(config.player_configurations[i].key_up_str.c_str()),
                KeyStringToKeyCode(config.player_configurations[i].key_down_str.c_str()),
                KeyStringToKeyCode(config.player_configurations[i].key_left_str.c_str()),
                KeyStringToKeyCode(config.player_configurations[i].key_right_str.c_str()),
                KeyStringToKeyCode(config.player_configurations[i].key_a_str.c_str()),
                KeyStringToKeyCode(config.player_configurations[i].key_b_str.c_str()),
                KeyStringToKeyCode(config.player_configurations[i].key_x_str.c_str()),
                KeyStringToKeyCode(config.player_configurations[i].key_y_str.c_str()),
                config.player_configurations[i].keyboard_input_allowed,
                config.player_configurations[i].joypad_input_allowed);
    }

	SetGlobalConfig(config.language, config.sound, config.allow_networking);
    SaveSphereConfig(&config, (GetSphereDirectory() + "/engine.ini").c_str());

    original_directory = getcwd(NULL, 0);
    char* env_data_dir = getenv("SPHERE_DATA_DIR");

    if (env_data_dir != NULL)
        strcpy(unix_data_dir, env_data_dir);

    if (getopt(argc, const_cast<char**>(argv), "d:") == 'd')
        strcpy(unix_data_dir, optarg);

    chdir(unix_data_dir);
    srand((unsigned)GetTime());

    // initialize video subsystem
    if (InitVideo(&config) == false)
    {
        printf("Video subsystem could not be initialized...\n");
        return 0;
    }

    // initialize input
    InitializeInput();

    // initialize audio
    if (!InitAudio(&config))
    {
        printf("Sound could not be initialized...\n");
    }

    atexit(CloseVideo);
    atexit(CloseAudio);

    RunSphere(argc, argv);

}

////////////////////////////////////////////////////////////////////////////////
