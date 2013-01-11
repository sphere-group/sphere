#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include "win32_sphere_config.hpp"
#include "../../common/configfile.hpp"


////////////////////////////////////////////////////////////////////////////////
static void InitializePlayerConfig(PLAYERCONFIG* config, int player_index)
{
    if (!config)
        return;

    switch (player_index)
    {
        case 0:
            strcpy(config->key_menu_str,  "KEY_ESCAPE");
            strcpy(config->key_up_str,    "KEY_UP");
            strcpy(config->key_down_str,  "KEY_DOWN");
            strcpy(config->key_left_str,  "KEY_LEFT");
            strcpy(config->key_right_str, "KEY_RIGHT");
            strcpy(config->key_a_str,     "KEY_DELETE");
            strcpy(config->key_b_str,     "KEY_END");
            strcpy(config->key_x_str,     "KEY_INSERT");
            strcpy(config->key_y_str,     "KEY_HOME");
            config->keyboard_input_allowed = true;
            config->joypad_input_allowed   = true;
            break;

        case 1:
            strcpy(config->key_menu_str,  "KEY_ESCAPE");
            strcpy(config->key_up_str,    "KEY_W");
            strcpy(config->key_down_str,  "KEY_S");
            strcpy(config->key_left_str,  "KEY_A");
            strcpy(config->key_right_str, "KEY_D");
            strcpy(config->key_a_str,     "KEY_F");
            strcpy(config->key_b_str,     "KEY_E");
            strcpy(config->key_x_str,     "KEY_Y");
            strcpy(config->key_y_str,     "KEY_X");
            config->keyboard_input_allowed = true;
            config->joypad_input_allowed   = true;
            break;

        case 2:
            strcpy(config->key_menu_str,  "KEY_ESCAPE");
            strcpy(config->key_up_str,    "KEY_I");
            strcpy(config->key_down_str,  "KEY_K");
            strcpy(config->key_left_str,  "KEY_J");
            strcpy(config->key_right_str, "KEY_L");
            strcpy(config->key_a_str,     "KEY_H");
            strcpy(config->key_b_str,     "KEY_U");
            strcpy(config->key_x_str,     "KEY_N");
            strcpy(config->key_y_str,     "KEY_M");
            config->keyboard_input_allowed = true;
            config->joypad_input_allowed   = true;
            break;

        case 3:
            strcpy(config->key_menu_str,  "KEY_ESCAPE");
            strcpy(config->key_up_str,    "KEY_NUM_8");
            strcpy(config->key_down_str,  "KEY_NUM_5");
            strcpy(config->key_left_str,  "KEY_NUM_4");
            strcpy(config->key_right_str, "KEY_NUM_6");
            strcpy(config->key_a_str,     "KEY_NUM_1");
            strcpy(config->key_b_str,     "KEY_NUM_2");
            strcpy(config->key_x_str,     "KEY_NUM_7");
            strcpy(config->key_y_str,     "KEY_NUM_9");
            config->keyboard_input_allowed = true;
            config->joypad_input_allowed   = true;
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
    config->language    = file.ReadInt   ("Main", "Language", 0);

    // load video settings
    config->videodriver = file.ReadString("Video", "Driver",     "");

    // load sound settings
    config->sound       = file.ReadInt   ("Audio", "Preference", SOUND_AUTODETECT);
    config->audiodriver = file.ReadString("Audio", "Driver",     "directsound");

    // load network settings
    config->allow_networking = file.ReadBool("Network", "AllowNetworking", true);

    // load video capture settings
    config->video_capture_mode      = file.ReadInt("VideoCapture", "Mode",      VIDEO_CAPTURE_SCREENSHOT_ONLY);
    config->video_capture_timer     = file.ReadInt("VideoCapture", "Timer",     1000);
    config->video_capture_framerate = file.ReadInt("VideoCapture", "FrameRate", 1);

    // validate video capture config settings
    if (config->video_capture_framerate <= 0)
        config->video_capture_framerate = 1;

    if (config->video_capture_mode != VIDEO_CAPTURE_SCREENSHOT_ONLY &&
        config->video_capture_mode != VIDEO_CAPTURE_UNTIL_OUTOFTIME &&
        config->video_capture_mode != VIDEO_CAPTURE_UNTIL_F12_KEYED)
    {
        config->video_capture_mode = VIDEO_CAPTURE_SCREENSHOT_ONLY;
    }

    if (config->video_capture_timer <= 0)
    {
        config->video_capture_timer = 1000;
    }

    for (int i = 0; i < 4; i++)
    {
        InitializePlayerConfig(&config->player_configurations[i], i);

        char section[100];
        sprintf (section, "Player%d", i + 1);

        std::string key_menu  = file.ReadString(section, "KeyMenu",  config->player_configurations[i].key_menu_str);
        std::string key_up    = file.ReadString(section, "KeyUp",    config->player_configurations[i].key_up_str);
        std::string key_down  = file.ReadString(section, "KeyDown",  config->player_configurations[i].key_down_str);
        std::string key_left  = file.ReadString(section, "KeyLeft",  config->player_configurations[i].key_left_str);
        std::string key_right = file.ReadString(section, "KeyRight", config->player_configurations[i].key_right_str);
        std::string key_a     = file.ReadString(section, "KeyA",     config->player_configurations[i].key_a_str);
        std::string key_b     = file.ReadString(section, "KeyB",     config->player_configurations[i].key_b_str);
        std::string key_x     = file.ReadString(section, "KeyX",     config->player_configurations[i].key_x_str);
        std::string key_y     = file.ReadString(section, "KeyY",     config->player_configurations[i].key_y_str);

        config->player_configurations[i].keyboard_input_allowed = file.ReadBool(section, "AllowKeyboardInput", config->player_configurations[i].keyboard_input_allowed);
        config->player_configurations[i].joypad_input_allowed   = file.ReadBool(section, "AllowJoypadInput",   config->player_configurations[i].joypad_input_allowed);

        if (key_menu.length() > 0 && key_menu.length() < sizeof(config->player_configurations[i].key_menu_str))
            strcpy(config->player_configurations[i].key_menu_str, key_menu.c_str());

        if (key_up.length() > 0 && key_up.length() < sizeof(config->player_configurations[i].key_up_str))
            strcpy(config->player_configurations[i].key_up_str, key_up.c_str());

        if (key_down.length() > 0 && key_down.length() < sizeof(config->player_configurations[i].key_down_str))
            strcpy(config->player_configurations[i].key_down_str, key_down.c_str());

        if (key_left.length() > 0 && key_left.length() < sizeof(config->player_configurations[i].key_left_str))
            strcpy(config->player_configurations[i].key_left_str, key_left.c_str());

        if (key_right.length() > 0 && key_right.length() < sizeof(config->player_configurations[i].key_right_str))
            strcpy(config->player_configurations[i].key_right_str, key_right.c_str());

        if (key_a.length() > 0 && key_a.length() < sizeof(config->player_configurations[i].key_a_str))
            strcpy(config->player_configurations[i].key_a_str, key_a.c_str());

        if (key_b.length() > 0 && key_b.length() < sizeof(config->player_configurations[i].key_b_str))
            strcpy(config->player_configurations[i].key_b_str, key_b.c_str());

        if (key_x.length() > 0 && key_x.length() < sizeof(config->player_configurations[i].key_x_str))
            strcpy(config->player_configurations[i].key_x_str, key_x.c_str());

        if (key_y.length() > 0 && key_y.length() < sizeof(config->player_configurations[i].key_y_str))
            strcpy(config->player_configurations[i].key_y_str, key_y.c_str());

    }

    SaveSphereConfig(config, filename);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool SaveSphereConfig(SPHERECONFIG* config, const char* filename)
{
    if (!config)
        return false;

    CConfigFile file;

    // save main settings
    file.WriteInt   ("Main", "Language",                config->language);

    // save video settings
    file.WriteString("Video", "Driver",                 config->videodriver.c_str());

    // save sound settings
    file.WriteInt   ("Audio", "Preference",             config->sound);
    file.WriteString("Audio", "Driver",                 config->audiodriver.c_str());

    // save network settings
    file.WriteBool  ("Network", "AllowNetworking",      config->allow_networking);

    // save video capture settings
    file.WriteInt   ("VideoCapture", "Mode",            config->video_capture_mode);
    file.WriteInt   ("VideoCapture", "Timer",           config->video_capture_timer);
    file.WriteInt   ("VideoCapture", "FrameRate",       config->video_capture_framerate);

    // save input settings
    for (int i = 0; i < 4; i++)
    {
        char section[100];
        sprintf (section, "Player%d", i + 1);

        file.WriteString(section, "KeyMenu",            config->player_configurations[i].key_menu_str);
        file.WriteString(section, "KeyUp",              config->player_configurations[i].key_up_str);
        file.WriteString(section, "KeyDown",            config->player_configurations[i].key_down_str);
        file.WriteString(section, "KeyLeft",            config->player_configurations[i].key_left_str);
        file.WriteString(section, "KeyRight",           config->player_configurations[i].key_right_str);
        file.WriteString(section, "KeyA",               config->player_configurations[i].key_a_str);
        file.WriteString(section, "KeyB",               config->player_configurations[i].key_b_str);
        file.WriteString(section, "KeyX",               config->player_configurations[i].key_x_str);
        file.WriteString(section, "KeyY",               config->player_configurations[i].key_y_str);
        file.WriteBool(section, "AllowKeyboardInput",   config->player_configurations[i].keyboard_input_allowed);
        file.WriteBool(section, "AllowJoypadInput",     config->player_configurations[i].joypad_input_allowed);
    }

    file.Save(filename);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
