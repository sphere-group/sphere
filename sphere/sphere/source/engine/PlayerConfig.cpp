#include "PlayerConfig.hpp"

static __PLAYERCONFIG__ player_configurations[4];
static __GLOBALCONFIG__ global_configuration;

void SetPlayerConfig(int player,
                     int key_menu,
                     int key_up,
                     int key_down,
                     int key_left,
                     int key_right,
                     int key_a,
                     int key_b,
                     int key_x,
                     int key_y,
                     bool use_keyboard,
                     bool use_joypad)
{
    if (player >= 0 && player < 4)
    {
        player_configurations[player].key_menu               = key_menu;
        player_configurations[player].key_up                 = key_up;
        player_configurations[player].key_down               = key_down;
        player_configurations[player].key_left               = key_left;
        player_configurations[player].key_right              = key_right;
        player_configurations[player].key_a                  = key_a;
        player_configurations[player].key_b                  = key_b;
        player_configurations[player].key_x                  = key_x;
        player_configurations[player].key_y                  = key_y;
        player_configurations[player].keyboard_input_allowed = use_keyboard;
        player_configurations[player].joypad_input_allowed   = use_joypad;
    }
}

struct __PLAYERCONFIG__* GetPlayerConfig(int player)
{
    if (player >= 0 && player < 4)
        return &player_configurations[player];

    return 0;
}

void SetGlobalConfig(int language, /*std::string videodriver,*/ int sound, /*std::string audiodriver,*/ bool allow_networking)
{
	global_configuration.language = language;
	//configuration.videodriver = videodriver;
	global_configuration.sound = sound;
	//configuration.audiodriver = audiodriver;
	global_configuration.allow_networking = allow_networking;
}

int GetGlobalConfig(int conf){
	switch(conf){
		case 0: return global_configuration.language;
		case 1: return global_configuration.sound;
		case 2: return global_configuration.allow_networking? 1:0;
		default: return -1;
	}
}