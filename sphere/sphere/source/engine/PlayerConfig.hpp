#ifndef PLAYER_CONFIG_HPP
#define PLAYER_CONFIG_HPP


struct __PLAYERCONFIG__
{
    int key_menu;
    int key_up;
    int key_down;
    int key_left;
    int key_right;
    int key_a;
    int key_b;
    int key_x;
    int key_y;
    bool keyboard_input_allowed;
    bool joypad_input_allowed;
};

struct __GLOBALCONFIG__
{
	int language;
	//std::string videodriver;
	int sound;
	//std::string audiodriver;
	bool allow_networking;
};

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
                     bool use_joypad);

void SetGlobalConfig(int language, int sound, bool allow_networking);

struct __PLAYERCONFIG__* GetPlayerConfig(int player);
int GetGlobalConfig(int conf);

#endif
