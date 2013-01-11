#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include "unix_sphere_config.h"
#include "../../common/configfile.hpp"


////////////////////////////////////////////////////////////////////////////////
static void InitializePlayerConfig(PLAYERCONFIG* config, int player_index)
{
    if (!config)
        return;

    switch (player_index)
    {
        case 0:
            config->key_menu_str              = "KEY_ESCAPE";
            config->key_up_str                = "KEY_UP";
            config->key_down_str              = "KEY_DOWN";
            config->key_left_str              = "KEY_LEFT";
            config->key_right_str             = "KEY_RIGHT";
            config->key_a_str                 = "KEY_DELETE";
            config->key_b_str                 = "KEY_END";
            config->key_x_str                 = "KEY_INSERT";
            config->key_y_str                 = "KEY_HOME";
            config->keyboard_input_allowed    = true;
            config->joypad_input_allowed      = true;
            break;

        case 1:
            config->key_menu_str              = "KEY_ESCAPE";
            config->key_up_str                = "KEY_W";
            config->key_down_str              = "KEY_S";
            config->key_left_str              = "KEY_A";
            config->key_right_str             = "KEY_D";
            config->key_a_str                 = "KEY_F";
            config->key_b_str                 = "KEY_E";
            config->key_x_str                 = "KEY_Y";
            config->key_y_str                 = "KEY_X";
            config->keyboard_input_allowed    = true;
            config->joypad_input_allowed      = true;
            break;

        case 2:
            config->key_menu_str              = "KEY_ESCAPE";
            config->key_up_str                = "KEY_I";
            config->key_down_str              = "KEY_K";
            config->key_left_str              = "KEY_J";
            config->key_right_str             = "KEY_L";
            config->key_a_str                 = "KEY_H";
            config->key_b_str                 = "KEY_U";
            config->key_x_str                 = "KEY_N";
            config->key_y_str                 = "KEY_M";
            config->keyboard_input_allowed    = true;
            config->joypad_input_allowed      = true;
            break;

        case 3:
            config->key_menu_str              = "KEY_ESCAPE";
            config->key_up_str                = "KEY_NUM_8";
            config->key_down_str              = "KEY_NUM_2";
            config->key_left_str              = "KEY_NUM_4";
            config->key_right_str             = "KEY_NUM_6";
            config->key_a_str                 = "KEY_NUM_1";
            config->key_b_str                 = "KEY_NUM_2";
            config->key_x_str                 = "KEY_NUM_7";
            config->key_y_str                 = "KEY_NUM_9";
            config->keyboard_input_allowed    = true;
            config->joypad_input_allowed      = true;
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
bool LoadSphereConfig(SPHERECONFIG* config, const char* filename)
{
    if (!config)
        return false;

    CConfigFile file;
    file.Load(filename);

    // load main settings
    config->language         = file.ReadInt   ("Main", "Language", 0);

    // load video settings
    config->videodriver      = file.ReadString("Video" ,  "Driver",          "");

    // load sound settings
    config->sound            = file.ReadInt   ("Audio" ,  "Preference",      SOUND_AUTODETECT);
    config->audiodriver      = file.ReadString("Audio",   "Driver",          "");

    // load network settings
    config->allow_networking = file.ReadBool  ("Network", "AllowNetworking", true);

    // load input settings
    const char identify[] = "1234";
    for(int x = 0; x < 4; x++)
    {
        std::string current("Player");
        std::string key_menu_str;
        std::string key_up_str;
        std::string key_down_str;
        std::string key_left_str;
        std::string key_right_str;
        std::string key_a_str;
        std::string key_b_str;
        std::string key_x_str;
        std::string key_y_str;

        current += identify[x];

        // Set default configuration to be overridden.
        InitializePlayerConfig(&config->player_configurations[x], x);

        if ((key_menu_str = file.ReadString(current.c_str(), "KeyMenu", "")) != "")
            config->player_configurations[x].key_menu_str = key_menu_str;

        if ((key_up_str = file.ReadString(current.c_str(), "KeyUp", "")) != "")
            config->player_configurations[x].key_up_str = key_up_str;

        if ((key_down_str = file.ReadString(current.c_str(), "KeyDown", "")) != "")
            config->player_configurations[x].key_down_str = key_down_str;

        if ((key_left_str = file.ReadString(current.c_str(), "KeyLeft", "")) != "")
            config->player_configurations[x].key_left_str = key_left_str;

        if ((key_right_str = file.ReadString(current.c_str(), "KeyRight", "")) != "")
            config->player_configurations[x].key_right_str = key_right_str;

        if ((key_a_str = file.ReadString(current.c_str(), "KeyA", "")) != "")
            config->player_configurations[x].key_a_str = key_a_str;

        if ((key_b_str = file.ReadString(current.c_str(), "KeyB", "")) != "")
            config->player_configurations[x].key_b_str = key_b_str;

        if ((key_x_str = file.ReadString(current.c_str(), "KeyX", "")) != "")
            config->player_configurations[x].key_x_str = key_x_str;

        if ((key_y_str = file.ReadString(current.c_str(), "KeyY", "")) != "")
            config->player_configurations[x].key_y_str = key_y_str;

        config->player_configurations[x].keyboard_input_allowed = file.ReadBool(current.c_str(), "AllowKeyboardInput", true);
        config->player_configurations[x].joypad_input_allowed   = file.ReadBool(current.c_str(), "AllowJoypadInput",   true);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool SaveSphereConfig(SPHERECONFIG* config, const char* filename)
{
    if (!config)
        return false;

    CConfigFile file;

    // save main settings
    file.WriteInt   ("Main", "Language",       config->language);

    // save video settings
    file.WriteString("Video" ,  "Driver",      config->videodriver.c_str());

    // save sound settings
    file.WriteInt   ("Audio" ,  "Preference",  config->sound);
    file.WriteString("Audio",   "Driver",      config->audiodriver.c_str());

    // save network settings
    file.WriteBool  ("Network", "AllowNetworking", config->allow_networking);

    // save input settings
    const char identify[] = "1234";
    for(int x = 0; x < 4; x++)
    {
        std::string current("Player");
        current += identify[x];

        file.WriteString(current.c_str(), "KeyMenu",            config->player_configurations[x].key_menu_str.c_str());
        file.WriteString(current.c_str(), "KeyUp",              config->player_configurations[x].key_up_str.c_str());
        file.WriteString(current.c_str(), "KeyLeft",            config->player_configurations[x].key_left_str.c_str());
        file.WriteString(current.c_str(), "KeyRight",           config->player_configurations[x].key_right_str.c_str());
        file.WriteString(current.c_str(), "KeyDown",            config->player_configurations[x].key_down_str.c_str());
        file.WriteString(current.c_str(), "KeyA",               config->player_configurations[x].key_a_str.c_str());
        file.WriteString(current.c_str(), "KeyB",               config->player_configurations[x].key_b_str.c_str());
        file.WriteString(current.c_str(), "KeyX",               config->player_configurations[x].key_x_str.c_str());
        file.WriteString(current.c_str(), "KeyY",               config->player_configurations[x].key_y_str.c_str());
        file.WriteBool  (current.c_str(), "AllowKeyboardInput", config->player_configurations[x].keyboard_input_allowed);
        file.WriteBool  (current.c_str(), "AllowJoypadInput",   config->player_configurations[x].joypad_input_allowed);
    }

    file.Save(filename);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
