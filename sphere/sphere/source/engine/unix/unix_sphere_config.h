#ifndef UNIX_SPHERE_CONFIG_H
#define UNIX_SPHERE_CONFIG_H

#include <string>

enum
{
    SOUND_AUTODETECT,
    SOUND_ON,
    SOUND_OFF,
};

struct PLAYERCONFIG
{
    std::string key_menu_str;
    std::string key_up_str;
    std::string key_down_str;
    std::string key_left_str;
    std::string key_right_str;
    std::string key_a_str;
    std::string key_b_str;
    std::string key_x_str;
    std::string key_y_str;

    bool keyboard_input_allowed;
    bool joypad_input_allowed;
};

struct SPHERECONFIG
{
    // main
    int language;

    // video
    std::string videodriver;

    // audio
    int sound;
    std::string audiodriver;

    // network
    bool allow_networking;

    // input
    PLAYERCONFIG player_configurations[4];
};

extern bool LoadSphereConfig(SPHERECONFIG* config, const char* filename);
extern bool SaveSphereConfig(SPHERECONFIG* config, const char* filename);


#endif
