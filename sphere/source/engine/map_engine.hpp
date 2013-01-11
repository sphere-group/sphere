#ifndef MAP_ENGINE_HPP
#define MAP_ENGINE_HPP

#include <deque>
#include <map>
#include "engineinterface.hpp"
#include "smap.hpp"
#include "audio.hpp"
#include "input.hpp"
#include <string>

#include "../common/Playlist.hpp"
struct PersonData
{
    std::string name;
    std::string string_value;
    double double_value;
    int type;
};

class CMapEngine
{
public:
    // person entity commands
    enum {
        COMMAND_WAIT,
        COMMAND_ANIMATE,

        COMMAND_FACE_NORTH,
        COMMAND_FACE_NORTHEAST,
        COMMAND_FACE_EAST,
        COMMAND_FACE_SOUTHEAST,
        COMMAND_FACE_SOUTH,
        COMMAND_FACE_SOUTHWEST,
        COMMAND_FACE_WEST,
        COMMAND_FACE_NORTHWEST,

        COMMAND_MOVE_NORTH,
        COMMAND_MOVE_EAST,
        COMMAND_MOVE_SOUTH,
        COMMAND_MOVE_WEST,

        COMMAND_DO_SCRIPT,
        NUM_COMMANDS
    };

    // person script indices
    enum {
        SCRIPT_ON_CREATE,
        SCRIPT_ON_DESTROY,
        SCRIPT_ON_ACTIVATE_TOUCH,
        SCRIPT_ON_ACTIVATE_TALK,
        SCRIPT_COMMAND_GENERATOR,
        NUM_PERSON_SCRIPTS
    };

    enum {
        SCRIPT_ON_ENTER_MAP,
        SCRIPT_ON_LEAVE_MAP,
        SCRIPT_ON_LEAVE_MAP_NORTH,
        SCRIPT_ON_LEAVE_MAP_EAST,
        SCRIPT_ON_LEAVE_MAP_SOUTH,
        SCRIPT_ON_LEAVE_MAP_WEST,
        NUM_MAP_SCRIPTS
    };

public:
    CMapEngine(IEngine* engine, IFileSystem& fs);
    ~CMapEngine();

    bool Execute(const char* filename, int fps);
    bool Run();

    const char* GetErrorMessage();
    // script interface to map engine
    bool ChangeMap(const char* filename);
    bool Exit();
    bool IsRunning();
    bool Update();
    bool CallMapScript(int which);
    bool CallDefaultMapScript(int which);
    bool SetDefaultMapScript(int which, const char* script);

    std::string GetCurrentMap();
    bool GetNumLayers(int& layers);
    bool GetLayerWidth(int layer, int& width);
    bool GetLayerHeight(int layer, int& height);
    bool SetLayerWidth(int layer, int width);
    bool SetLayerHeight(int layer, int height);
    bool GetLayerName(int layer, std::string& name);
    bool IsLayerVisible(int layer, bool& visible);
    bool SetLayerVisible(int layer, bool visible);
    bool IsLayerReflective(int layer, bool& reflective);
    bool SetLayerReflective(int layer, bool reflective);
    bool SetLayerScaleFactorX(int layer_index, double factor_x);
    bool SetLayerScaleFactorY(int layer_index, double factor_y);
    bool GetLayerAngle(int layer_index, double& angle);
    bool SetLayerAngle(int layer_index, double angle);
    bool AppendLayer(int width, int height, int tile_index);
    bool GetNumTiles(int& tiles);

    bool SetTile(int x, int y, int layer, int tile);
    bool GetTile(int x, int y, int layer, int& tile);
    bool GetTileName(int tile_index, std::string& name);
    bool GetTileWidth(int& width);
    bool GetTileHeight(int& height);

    bool GetTileImage(int tile, IMAGE& image);
    bool SetTileImage(int tile, IMAGE image);
    bool GetTileSurface(int tile, CImage32* surface);
    bool SetTileSurface(int tile, const CImage32* surface);
    bool GetTileDelay(int tile, int& delay);
    bool SetTileDelay(int tile, int delay);
    bool GetNextAnimatedTile(int& tile);
    bool SetNextAnimatedTile(int current_tile, int next_tile);

    bool ReplaceTilesOnLayer(int layer, int old_tile, int new_tile);
    // trigger and zones
    bool IsTriggerAt(int location_x, int location_y, int layer, bool& found);
    bool AreZonesAt(int location_x, int location_y, int layer, bool& found);
    bool ExecuteTrigger(int location_x, int location_y, int layer);
    bool ExecuteZones(int location_x, int location_y, int layer);
    bool ExecuteTriggerScript(int trigger_index);
    bool ExecuteZoneScript(int zone_index);
    bool GetNumZones(int& zones);
    //bool GetNumTriggers(int& triggers);
    //bool GetCurrentTrigger(int& trigger);
    //bool GetTriggerX(int trigger, int& x);
    //bool GetTriggerY(int trigger, int& y);
    bool GetCurrentZone(int& zone);
    bool GetZoneX(int zone, int& x);
    bool GetZoneY(int zone, int& y);
    bool GetZoneWidth(int zone, int& width);
    bool GetZoneHeight(int zone, int& height);
    bool GetZoneLayer(int zone, int& layer);
    bool SetZoneLayer(int zone, int layer);
    bool SetZoneDimensions(int zone, int x1, int y1, int x2, int y2);
    //bool AddZone(int x1, int y1, int x2, int y2, int layer, const char* script);
    bool GetNumObstructionSegments(int layer, int& num_segments);
    bool RenderMap();

    bool SetColorMask(RGBA color, int num_frames);
    bool SetDelayScript(int num_frames, const char* script);
private:
    inline bool IsKeyBound(int key);

public:
    bool BindKey(int key, const char* on_key_down, const char* on_key_up);
    bool UnbindKey(int key);

private:
    bool IsJoystickButtonBound(int joystick, int button);

public:
    bool BindJoystickButton(int joystick, int button, const char* on_key_down, const char* on_key_up);
    bool UnbindJoystickButton(int joystick, int button);

    bool AttachInput(const char* person);
    bool DetachInput();
    bool IsInputAttached(bool& attached);
    bool GetInputPerson(std::string& person);

    bool AttachPlayerInput(const char* person, int player);
    bool DetachPlayerInput(const char* person);

    bool SetUpdateScript(const char* script);
    bool SetRenderScript(const char* script);
    bool SetLayerRenderer(int layer, const char* script);

    bool SetLayerMask(int layer, RGBA mask);
    bool GetLayerMask(int layer, RGBA& mask);

    bool AttachCamera(const char* person);
    bool DetachCamera();
    bool IsCameraAttached(bool& attached);
    bool GetCameraPerson(std::string& person);

    bool SetCameraX(int x);
    bool SetCameraY(int y);
    bool GetCameraX(int& x);
    bool GetCameraY(int& y);

    bool MapToScreenX(int layer, int mx, int& sx);
    bool MapToScreenY(int layer, int my, int& sy);
    bool ScreenToMapX(int layer, int sx, int& mx);
    bool ScreenToMapY(int layer, int sx, int& my);

    bool GetPersonList(std::vector<std::string>& list, int& size);
	bool DoesPersonExist(const char* name);
    bool CreatePerson(const char* name, const char* spriteset, bool destroy_with_map);
    bool DestroyPerson(const char* name);

    bool SetPersonX(const char* name, int x);
    bool SetPersonY(const char* name, int y);
    bool SetPersonOffsetX(const char* name, int offx);
    bool SetPersonOffsetY(const char* name, int offy);
    bool SetPersonXYFloat(const char* name, double x, double y);
    bool SetPersonLayer(const char* name, int layer);
    bool SetPersonDirection(const char* name, const char* direction);
    bool SetPersonFrame(const char* name, int frame);
    bool GetPersonFrameNext(const char* name, int& frameDelay);
    bool SetPersonFrameNext(const char* name, int frameDelay);
    bool GetPersonX(const char* name, int& x);
    bool GetPersonY(const char* name, int& y);
    bool GetPersonOffsetX(const char* name, int& x);
    bool GetPersonOffsetY(const char* name, int& y);
    bool GetPersonXFloat(const char* name, double& x);
    bool GetPersonYFloat(const char* name, double& y);
    bool GetPersonLayer(const char* name, int& layer);
    bool GetPersonDirection(const char* name, std::string& direction);
    bool GetPersonFrame(const char* name, int& frame);
    bool SetPersonFrameRevert(const char* name, int i);
    bool GetPersonFrameRevert(const char* name, int& i);
    bool SetPersonSpeedXY(const char* name, double x, double y);
    bool GetPersonSpeedX(const char* name, double& x);
    bool GetPersonSpeedY(const char* name, double& y);
    bool GetPersonVectorXY(const char* name, bool historical, int& x, int& y);
    bool SetPersonScaleAbsolute(const char* name, int width, int height);
    bool SetPersonScaleFactor(const char* name, double scale_x, double scale_y);
    bool GetPersonAngle(const char* name, double& angle);
    bool SetPersonAngle(const char* name, double angle);
    bool SetPersonMask(const char* name, RGBA mask);
    bool GetPersonMask(const char* name, RGBA& mask);
    bool SetPersonBase(const char* name, int x1, int y1, int x2, int y2);

    bool IsPersonVisible(const char* person_name, bool& visible);
    bool SetPersonVisible(const char* person_name, bool visible);

    bool GetPersonData(const char* name, std::vector<struct PersonData>& data);
    bool SetPersonData(const char* name, const std::vector<struct PersonData> data);

    bool GetPersonValue(const char* name, const char* key, std::string& string_value, double& double_value, int& type);
    bool SetPersonValue(const char* name, const char* key, const std::string string_value, double double_value, int type);

    SSPRITESET* GetPersonSpriteset(const char* name);
    bool SetPersonSpriteset(const char* name, sSpriteset& spriteset);

    bool FollowPerson(const char* name, const char* leader, int pixels);
    bool SetPersonScript(const char* name, int which, const char* script);
    bool SetDefaultPersonScript(int which, const char* script);
    bool CallPersonScript(const char* name, int which);
    bool CallDefaultPersonScript(const char* name, int which);
    bool GetCurrentPerson(std::string& person);
    bool QueuePersonCommand(const char* name, int command, bool immediate);
    bool QueuePersonScript(const char* name, const char* script, bool immediate);
    bool ClearPersonCommands(const char* name);
    bool IsCommandQueueEmpty(const char* name, bool& empty);

    bool IgnorePersonObstructions(const char* name, bool ignore);
    bool IgnoreTileObstructions(const char* name, bool ignore);

    bool IsIgnoringPersonObstructions(const char* name, bool& ignoring);
    bool IsIgnoringTileObstructions(const char* name, bool& ignoring);

    bool GetPersonIgnoreList(const char* name, std::vector<std::string>& ignore_list);
    bool SetPersonIgnoreList(const char* name, std::vector<std::string> ignore_list);

    bool IsPersonObstructed(const char* name, int x, int y, bool& result);
    bool GetObstructingTile(const char* name, int x, int y, int& result);
    bool GetObstructingPerson(const char* name, int x, int y, std::string& result);

    bool GetTalkingPerson(const char* name, int x, int y, std::string& result);
    bool SetTalkActivationKey(int key);
    bool SetTalkActivationButton(int button);
    int GetTalkActivationKey();
    int GetTalkActivationButton();
    bool SetTalkDistance(int pixels);
    int GetTalkDistance();

    bool SetMapEngineFrameRate(int fps);
    int GetMapEngineFrameRate();

    SMAP& GetMap()
    {
        return m_Map;
    }
    const SMAP& GetMap() const
    {
        return m_Map;
    }
    bool SaveMap(const char* filename);

	bool GetMapMusicName(std::string& result);
	int GetMapMusicType();
	audiere::OutputStreamPtr getMusic(){return m_Music;};
	audiere::MIDIStreamPtr getMidi(){return m_Midi;};

private:
    struct DelayScript
    {
        int frames_left;
        IEngine::script script;
    };

    static struct __PLAYERCONFIG__ player_configurations[4];
	static struct __GLOBALCONFIG__ global_configuration;
    struct Person
    {
        bool destroy_with_map;

        std::string name;
        std::string description;
        SSPRITESET* spriteset;
        RGBA mask;

        bool is_angled;
        double angle;

        double x;
        double y;

        int offset_x;
        int offset_y;

        int layer;
        int width;
        int height;
        int base_x1;
        int base_y1;
        int base_x2;
        int base_y2;

        int dx, hx;
        int dy, hy;
        double speed_x;
        double speed_y;

        double scale_x;
        double scale_y;

        std::string direction;
        int frame;

        int next_frame_switch;
        int stepping_frame_revert;
        int stepping_frame_revert_count;

		int obs_person; // -1 by default or a number of the obstructed person

        IEngine::script person_scripts[NUM_PERSON_SCRIPTS];
        struct Command
        {
            Command(int c, bool i, std::string s = "") : command(c), immediate(i), script(s)
            { }
            int command;
            bool immediate;
            std::string script;
        };
        std::deque<Command> commands;

        struct AnimationState
        {
            double x;
            double y;
            int layer;
            std::string direction;
        };

        int leader;
        std::vector<AnimationState> follow_state_queue;

        bool ignorePersonObstructions;
        bool ignoreTileObstructions;

        std::vector<std::string> ignored_persons;
        std::vector<PersonData> person_data;

        int player_index;
        int key_down;
        int key_up;
        int key_left;
        int key_right;
        int key_a;
        int key_b;
        int key_x;
        int key_y;
        bool keyboard_input_allowed;
        bool joypad_input_allowed;

        bool on_trigger;
        int last_trigger;

        bool is_visible;
    };

    struct Trigger
    {
        IEngine::script script;
        int x;
        int y;
        int layer;
    };

    struct Zone
    {
        IEngine::script script;
        int x1;
        int y1;
        int x2;
        int y2;
        int layer;
        int reactivate_in_num_steps;
        int current_step;
    };

private:
    void CloseEngine();
    bool OpenMap(const char* filename);
    bool CloseMap();

    bool ExecuteScript(IEngine::script script, std::string& error);
    bool ExecuteScript(const char* script, std::string& error);

    bool CreateDefaultPerson(Person& p, const char* name, const char* spriteset, bool destroy_with_map);
    bool CompileEdgeScripts();
    void DestroyEdgeScripts();
    bool LoadMapPersons();
    bool DestroyMapPersons();
    void DestroyPersonScripts(Person& p);
    bool DestroyPersonStructure(Person& p);
    bool LoadTriggers();
    void DestroyTriggers();
    bool LoadZones();
    void DestroyZones();
    bool Render();
    bool RenderEntities(int layer, bool flipped, int offset_x, int offset_y);
    bool UpdateWorld(bool input_valid);
    bool UpdatePersons();
    bool UpdatePerson(int person_index, bool& activated);
    bool UpdateFollower(int person_index);

    int FindTrigger(int location_x, int location_y, int layer);
    bool UpdateTriggers(int person_index);

inline    bool IsPointWithinZone(int location_x, int location_y, int location_layer, int zone_index);
    bool IsPersonInsideZone(int person_index, int zone_index);
    bool UpdateZones(int person_index);

    bool UpdateColorMasks();
    bool UpdateDelayScripts();
    bool UpdateEdgeScripts();
    bool ProcessInput();
    bool ProcessBoundKeyDown(int key);
    bool ProcessBoundKeyUp(int key);
    void ProcessUnboundKeyDown(int key);
    void ProcessUnboundKeyUp(int key);
    bool ProcessBoundJoystickButtonDown(int joystick, int button);
    bool ProcessBoundJoystickButtonUp(int joystick, int button);
    void DestroyBoundKeys();

    // error checking
    bool IsInvalidPersonError(const char* person_name, int& person_index);
    bool IsInvalidLayerError(int layer_index, const char* calling_func);

    // miscellaneous
    void ResetNextFrame();
    int FindPerson(const char* name);
    bool IsObstructed(int person, int x, int y, int& obs_person);
    int FindObstructingTile(int person, int x, int y);
    int FindObstructingPerson(int person, int x, int y);
    int FindTalkingPerson(int person_index, int talk_x, int talk_y);

private:
    // core map engine stuff
    IEngine* m_Engine;
    IFileSystem& m_FileSystem;
    bool m_IsRunning;
    bool m_ShouldExit;

    std::string m_CurrentMap;
    std::string m_ErrorMessage;

    // rendering state
    bool  m_ThrottleFPS;
    int   m_FrameRate;
    qword m_NextFrame;

    SMAP       m_Map;
    std::string m_LastMusicPath;
    audiere::OutputStreamPtr m_Music;
#if defined(WIN32) && defined(USE_MIDI)
    audiere::MIDIStreamPtr m_Midi;
#endif
    CPlaylist m_Playlist;

    // camera
    struct
    {
        int x;
        int y;
        int layer;
    }
    m_Camera;

    // input person
    bool m_IsInputAttached;
    int  m_InputPerson;
    bool m_TouchActivationAllowed;
    bool m_TalkActivationAllowed;
    //bool m_IsTouching;
    //bool m_IsTalking;

    std::vector<int> m_InputPersons;
    // camera person
    bool m_IsCameraAttached;
    int  m_CameraPerson;

    // edge scripts
    IEngine::script m_NorthScript;
    IEngine::script m_EastScript;
    IEngine::script m_SouthScript;
    IEngine::script m_WestScript;

    // general scripts
    IEngine::script m_UpdateScript;
    //bool m_UpdateScriptRunning;
    IEngine::script m_RenderScript;
    //bool m_RenderScriptRunning;
    std::vector<IEngine::script> m_LayerRenderers;

    // default map scripts
    IEngine::script m_DefaultMapScripts[NUM_MAP_SCRIPTS];

    // default person scripts
    IEngine::script m_default_person_scripts[NUM_PERSON_SCRIPTS];

    // color masks
    int m_NumFrames;
    int m_FramesLeft;
    RGBA m_CurrentColorMask;
    RGBA m_PreviousColorMask;
    RGBA m_DestinationColorMask;

    // delay scripts
    std::vector<DelayScript> m_DelayScripts;

    // bound keys
    struct KeyScripts
    {
        IEngine::script down, up;
    };
    std::map<int, KeyScripts> m_BoundKeys;

    // bound joysticks
    struct BoundJoystick
    {
        int m_Joystick;
        std::map<int, KeyScripts> m_BoundButtons;
        std::vector<bool> m_Buttons;
    };
    std::vector<BoundJoystick> m_BoundJoysticks;

    // ENTITIES
    // triggers
    bool m_OnTrigger;
    int m_LastTrigger;
    std::vector<Trigger> m_Triggers;

    // persons
    std::vector<Person> m_Persons;
    std::string m_CurrentPerson;

    int m_TalkActivationKey;
    int m_TalkActivationDistance;

    // zones
    std::vector<Zone> m_Zones;
    int m_CurrentZone;

    // input handling
    bool m_Keys[MAX_KEY];

    int m_JoystickTalkButton;
    int m_JoystickCancelButton;
};

#endif
