#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include <sstream>
#include <math.h>
#include <assert.h>
#include "map_engine.hpp"
#include "render.hpp"
#include "rendersort.hpp"
#include "time.hpp"
#include "PlayerConfig.hpp"
#include "filesystem.hpp"
#include "../common/minmax.hpp"

////////////////////////////////////////////////////////////////////////////////

static const int c_MaxSkipFrames = 20;

////////////////////////////////////////////////////////////////////////////////

static inline std::string itos(int i)
{
    char s[20];
    sprintf(s, "%d", i);
    return s;
}

////////////////////////////////////////////////////////////////////////////////

CMapEngine::CMapEngine(IEngine* engine, IFileSystem& fs)
        : m_Engine(engine)
        , m_FileSystem(fs)
        , m_IsRunning(false)
        , m_ShouldExit(false)

        , m_ThrottleFPS(true)
        , m_FrameRate(0)
        , m_Music(NULL)
#if defined(WIN32) && defined(USE_MIDI)

        , m_Midi(NULL)
#endif
        , m_IsInputAttached(false)
        , m_InputPerson(-1)
        , m_TouchActivationAllowed(true)
        , m_TalkActivationAllowed(true)

        , m_IsCameraAttached(false)
        , m_CameraPerson(-1)

        , m_NorthScript(NULL)
        , m_EastScript(NULL)
        , m_SouthScript(NULL)
        , m_WestScript(NULL)
        , m_UpdateScript(NULL)

        , m_RenderScript(NULL)

        , m_TalkActivationKey(KEY_SPACE)
        , m_TalkActivationDistance(8)

        , m_CurrentZone(-1)
        , m_JoystickTalkButton(2)
        , m_JoystickCancelButton(0)
{
    int i;
    m_Camera.x     = 0;
    m_Camera.y     = 0;
    m_Camera.layer = 0;

    memset(&m_Keys, 0, sizeof(bool) * MAX_KEY);
    for (i = 0; i < NUM_MAP_SCRIPTS; i++)
    {
        m_DefaultMapScripts[i] = NULL;
    }
    for (i = 0; i < NUM_PERSON_SCRIPTS; i++)
    {
        m_default_person_scripts[i] = NULL;
    }
}
////////////////////////////////////////////////////////////////////////////////
CMapEngine::~CMapEngine()
{
    DestroyBoundKeys();

    // Stop any background music
    m_Playlist.Clear();
    if(m_Music) m_Music->stop();
    m_Music = 0;
    m_LastMusicPath = "";

#if defined(WIN32) && defined(USE_MIDI)
    if (m_Midi) m_Midi->stop();
    m_Midi = 0;
#endif

    // destroy update script
    if (m_UpdateScript)
    {
        m_Engine->DestroyScript(m_UpdateScript);
        m_UpdateScript = NULL;
    }

    // destroy render script
    if (m_RenderScript)
    {
        m_Engine->DestroyScript(m_RenderScript);
        m_RenderScript = NULL;
    }

    int i;
    for (i = 0; i < NUM_MAP_SCRIPTS; i++)
    {
        if (m_DefaultMapScripts[i])
        {
            m_Engine->DestroyScript(m_DefaultMapScripts[i]);
            m_DefaultMapScripts[i] = NULL;
        }
    }

    for (i = 0; i < NUM_PERSON_SCRIPTS; i++)
    {
        if (m_default_person_scripts[i])
        {
            m_Engine->DestroyScript(m_default_person_scripts[i]);
            m_default_person_scripts[i] = NULL;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
bool
CMapEngine::Execute(const char* filename, int fps)
{
    if (m_IsRunning)
    {
        m_ErrorMessage = "Map engine is already running!";
        return false;
    }

    if (fps <= 0)
    {
        m_ErrorMessage = "fps must be greater than zero!";
        return false;
    }

    m_IsRunning = true;
    m_FrameRate = fps;
    m_OnTrigger = false;
    m_LastTrigger = -1;
    m_ErrorMessage = "";
    m_LastMusicPath = "";
    m_Music = 0;
#if defined(WIN32) && defined(USE_MIDI)
    m_Midi = 0;
#endif
    m_NumFrames = 0;
    m_FramesLeft = 0;
    m_CurrentColorMask     = CreateRGBA(0, 0, 0, 0);
    m_PreviousColorMask    = CreateRGBA(0, 0, 0, 0);
    m_DestinationColorMask = CreateRGBA(0, 0, 0, 0);

    // change map to 'filename'
    if (!OpenMap(filename))
    {
        m_IsRunning = false;
        return false;
    }

    if (!Run())
    {
        m_IsRunning = false;
        return false;
    }

    // change map to nothing
    if (!CloseMap())
    {
        m_IsRunning = false;
        return false;
    }

    if (m_Music)
    {
        m_Music->stop();
    }

#if defined(WIN32) && defined(USE_MIDI)
    if (m_Midi)
    {
        m_Midi->stop();
    }
#endif

    m_IsRunning = false;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
const char*
CMapEngine::GetErrorMessage()
{
    return m_ErrorMessage.c_str();
}

////////////////////////////////////////////////////////////////////////////////
bool
CMapEngine::ChangeMap(const char* filename)
{

    if (m_IsRunning)
    {
        // close previous map
        if (!CloseMap())
        {
            return false;
        }

        // clear all entity queues
        for (unsigned int i = 0; i < m_Persons.size(); ++i)
        {
            m_Persons[i].commands.clear();
        }

        // open new one
        if (!OpenMap(filename))
        {
            return false;
        }

        // reset map engine timing
        m_NextFrame = (qword)GetTime() * m_FrameRate;  // update next rendering time

        return true;
    }
    else
    {
        m_ErrorMessage = "ChangeMap() called while map engine was not running";
        return false;
    }
}
////////////////////////////////////////////////////////////////////////////////
std::string
CMapEngine::GetCurrentMap()
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "GetCurrentMap() called while map engine was not running";
        return "";
    }
    else
    {
        return m_CurrentMap;
    }
}

////////////////////////////////////////////////////////////////////////////////
bool
CMapEngine::CallMapScript(int which)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "CallMapScript() called while map engine was not running";
        return false;
    }

    // make sure 'which' is valid
    if (which < 0 || which >= NUM_MAP_SCRIPTS)
    {
        m_ErrorMessage = "CallMapScript() - script does not exist";
        return false;
    }

    std::string error;
    switch (which)
    {
    case SCRIPT_ON_ENTER_MAP:
        if (!ExecuteScript(m_Map.GetMap().GetEntryScript(), error))
        {
            m_ErrorMessage = "Could not execute entry script\n" + error;
            return false;
        }
        break;

    case SCRIPT_ON_LEAVE_MAP:
        if (!ExecuteScript(m_Map.GetMap().GetExitScript(), error))
        {
            m_ErrorMessage = "Could not execute exit script\n" + error;
            return false;
        }
        break;

    case SCRIPT_ON_LEAVE_MAP_NORTH:
        if (m_NorthScript && !ExecuteScript(m_NorthScript, error))
        {
            m_ErrorMessage = "Could not execute north script\n" + error;
            return false;
        }
        break;

    case SCRIPT_ON_LEAVE_MAP_EAST:
        if (m_EastScript && !ExecuteScript(m_EastScript, error))
        {
            m_ErrorMessage = "Could not execute east script\n" + error;
            return false;
        }
        break;

    case SCRIPT_ON_LEAVE_MAP_SOUTH:
        if (m_SouthScript && !ExecuteScript(m_SouthScript, error))
        {
            m_ErrorMessage = "Could not execute south script\n" + error;
            return false;
        }
        break;

    case SCRIPT_ON_LEAVE_MAP_WEST:
        if (m_WestScript && !ExecuteScript(m_WestScript, error))
        {
            m_ErrorMessage = "Could not execute west script\n" + error;
            return false;
        }
        break;
    }

    // if we took more than a second to run the edge script, reset the timer
    if (qword(GetTime()) * m_FrameRate > m_NextFrame)
    {
        ResetNextFrame();
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
CMapEngine::CallDefaultMapScript(int which)

{
    // make sure 'which' is valid
    if (which < 0 || which >= NUM_MAP_SCRIPTS)
    {
        m_ErrorMessage = "CallDefaultMapScript() - script does not exist";
        return false;
    }

    if (m_DefaultMapScripts[which] == NULL ||
        m_DefaultMapScripts[which] == "")
    {
        return true;
    }

    std::string list[NUM_MAP_SCRIPTS] = {"enter", "exit", "north", "east", "south", "west"};

    if (m_Engine->IsScriptBeingUsed(m_DefaultMapScripts[which]))
    {
        m_ErrorMessage = "Default " + list[which] + " map script already running!";
        return false;
    }

    if (!m_Engine->IsScriptBeingUsed(m_DefaultMapScripts[which]))
    {
        std::string error;

        if (!ExecuteScript(m_DefaultMapScripts[which], error))
        {
            m_ErrorMessage = "Could not execute default " + list[which] + " map script\n" + error;
            return false;
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
CMapEngine::SetDefaultMapScript(int which, const char* script)
{
    // make sure 'which' is valid
    if (which < 0 || which >= NUM_MAP_SCRIPTS)
    {
        m_ErrorMessage = "SetDefaultMapScript() - script does not exist";
        return false;
    }

    // try to compile the script
    std::string error;
    IEngine::script s = m_Engine->CompileScript(script, error);
    if (s == NULL)
    {
        m_ErrorMessage = "Could not compile script\n" + error;
        return false;
    }

    // locate the old script
    IEngine::script* ps = &m_DefaultMapScripts[which];

    // now replace the script
    if (*ps)
    {
        m_Engine->DestroyScript(*ps);
    }
    *ps = s;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
CMapEngine::Exit()
{
    if (m_IsRunning)
    {
        m_ShouldExit = true;
        return true;
    }
    else
    {
        m_ErrorMessage = "ExitMapEngine() called while map engine was not running";
        return false;

    }
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::IsRunning()
{
    return m_IsRunning;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetMapEngineFrameRate(int fps)
{
    if (!m_IsRunning)
    {
        m_ShouldExit = true;
        m_ErrorMessage = "SetMapEngineFrameRate() called while map engine was not running";
        return false;
    }
    if (fps <= 0)
    {
        m_ShouldExit = true;
        m_ErrorMessage = "fps must be greater than zero!";
        return false;
    }

    m_FrameRate = fps;
    ResetNextFrame();
    return true;
}

////////////////////////////////////////////////////////////////////////////////

int
CMapEngine::GetMapEngineFrameRate()
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "GetMapEngineFrameRate() called while map engine was not running";
        return 0;
    }
    return m_FrameRate;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::Update()
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "UpdateMapEngine() called while map engine was not running";
        return false;
    }

    if (!UpdateWorld(false))
    {
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetNumLayers(int& layers)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "GetNumLayers() called while map engine was not running";
        return false;
    }

    layers = m_Map.GetMap().GetNumLayers();
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetLayerWidth(int layer, int& width)
{
    if ( IsInvalidLayerError(layer, "GetLayerWidth()") )
        return false;

    width = m_Map.GetMap().GetLayer(layer).GetWidth();
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetLayerHeight(int layer, int& height)
{
    if ( IsInvalidLayerError(layer, "GetLayerHeight()") )
        return false;

    height = m_Map.GetMap().GetLayer(layer).GetHeight();
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetLayerName(int layer, std::string& name)
{
    if ( IsInvalidLayerError(layer, "GetLayerName()") )
        return false;

    name = m_Map.GetMap().GetLayer(layer).GetName();
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::IsLayerVisible(int layer, bool& visible)
{
    if ( IsInvalidLayerError(layer, "IsLayerVisible()") )
        return false;

    visible = m_Map.GetMap().GetLayer(layer).IsVisible();
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetLayerVisible(int layer, bool visible)
{
    if ( IsInvalidLayerError(layer, "SetLayerVisible()") )
        return false;

    m_Map.GetMap().GetLayer(layer).SetVisible(visible);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::IsLayerReflective(int layer, bool& reflective)
{
    if ( IsInvalidLayerError(layer, "IsLayerReflective()") )
        return false;

    reflective = m_Map.GetMap().GetLayer(layer).IsReflective();
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetLayerReflective(int layer, bool reflective)
{
    if ( IsInvalidLayerError(layer, "SetLayerReflective()") )
        return false;

    m_Map.GetMap().GetLayer(layer).SetReflective(reflective);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetLayerWidth(int layer_index, int width)
{
    if ( IsInvalidLayerError(layer_index, "SetLayerWidth()") )
        return false;
    if (width <= 0 || width > 4096)
    {
        m_ErrorMessage = "Invalid layer width: " + itos(width);
        return false;
    }
    sLayer& layer = m_Map.GetMap().GetLayer(layer_index);
    layer.Resize(width, layer.GetHeight());
    if (layer.GetHeight() != width)
    {
        m_ErrorMessage = "Resize failed!";
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetLayerHeight(int layer_index, int height)
{
    if ( IsInvalidLayerError(layer_index, "SetLayerHeight()") )
        return false;
    if (height <= 0 || height > 4096)
    {
        m_ErrorMessage = "Invalid layer height: " + itos(height);
        return false;
    }
    sLayer& layer = m_Map.GetMap().GetLayer(layer_index);
    layer.Resize(layer.GetWidth(), height);
    if (layer.GetHeight() != height)
    {
        m_ErrorMessage = "Resize failed!";
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetLayerScaleFactorX(int layer_index, double factor_x)
{
    if ( IsInvalidLayerError(layer_index, "SetLayerScaleFactorX()") )
        return false;
    if (factor_x <= 0)
    {
        m_ErrorMessage = "Scale factor must be greater than zero";
        return false;
    }
    m_Map.SetLayerScaleFactorX(layer_index, factor_x);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetLayerScaleFactorY(int layer_index, double factor_y)
{
    if ( IsInvalidLayerError(layer_index, "SetLayerScaleFactorY()") )
        return false;
    if (factor_y <= 0)
    {
        m_ErrorMessage = "Scale factor must be greater than zero";
        return false;
    }
    m_Map.SetLayerScaleFactorY(layer_index, factor_y);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetLayerAngle(int layer_index, double& angle)
{
    if ( IsInvalidLayerError(layer_index, "GetLayerAngle()") )
        return false;
    angle = m_Map.GetLayerAngle(layer_index);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetLayerAngle(int layer_index, double angle)
{
    if ( IsInvalidLayerError(layer_index, "SetLayerAngle()") )
        return false;
    m_Map.SetLayerAngle(layer_index, angle);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetNumTiles(int& tiles)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "GetNumTiles() called while map engine was not running";
        return false;
    }

    tiles = m_Map.GetMap().GetTileset().GetNumTiles();
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetTile(int x, int y, int layer, int tile)
{
    if ( IsInvalidLayerError(layer, "SetTile()") )
        return false;

    sLayer& l = m_Map.GetMap().GetLayer(layer);
    // make sure x and y are valid
    if (x < 0 || y < 0 || x >= l.GetWidth() || y >= l.GetHeight())
    {
        m_ErrorMessage = "Invalid x or y";
        return false;
    }

    if (tile < 0 || tile >= m_Map.GetMap().GetTileset().GetNumTiles())
    {
        m_ErrorMessage = "Invalid tile_index: " + itos(tile);
        return false;
    }
    l.SetTile(x, y, tile);
    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetTile(int x, int y, int layer, int& tile)
{
    if ( IsInvalidLayerError(layer, "GetTile()") )
        return false;

    sLayer& l = m_Map.GetMap().GetLayer(layer);
    // make sure x and y are valid
    if (x < 0 || y < 0 || x >= l.GetWidth() || y >= l.GetHeight())
    {
        m_ErrorMessage = "Invalid x or y: x: " + itos(x) + " y: " + itos(y);
        return false;
    }

    tile = l.GetTile(x, y);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetTileName(int tile_index, std::string& name)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "GetTileName() called while map engine was not running";
        return false;
    }
    // make sure tile_index is valid
    if (tile_index < 0 || tile_index >= m_Map.GetMap().GetTileset().GetNumTiles())
    {
        m_ErrorMessage = "Invalid tile_index: " + itos(tile_index);
        return false;
    }
    name = m_Map.GetMap().GetTileset().GetTile(tile_index).GetName();
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetTileWidth(int& width)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "GetTileWidth() called while map engine was not running";
        return false;
    }

    width = m_Map.GetMap().GetTileset().GetTileWidth();
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetTileHeight(int& height)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "GetTileHeight() called while map engine was not running";
        return false;
    }

    height = m_Map.GetMap().GetTileset().GetTileHeight();
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetTileImage(int tile, IMAGE& image)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "GetTileImage() called while map engine was not running";
        return false;
    }

    if (tile < 0 || tile >= m_Map.GetMap().GetTileset().GetNumTiles())
    {
        m_ErrorMessage = "Tile index does not exist";
        return false;
    }

    image = CreateImage(m_Map.GetMap().GetTileset().GetTileWidth(),
                        m_Map.GetMap().GetTileset().GetTileHeight(),
                        m_Map.GetMap().GetTileset().GetTile(tile).GetPixels());

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetTileImage(int tile, IMAGE image)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "SetTileImage() called while map engine was not running";
        return false;
    }

    if (tile < 0 || tile >= m_Map.GetMap().GetTileset().GetNumTiles())
    {
        m_ErrorMessage = "Tile index does not exist";
        return false;
    }

    if ( GetImageWidth(image) != m_Map.GetMap().GetTileset().GetTileWidth() )
    {
        m_ErrorMessage = "Image used in SetTileImage call doesn't match the tile width";
        return false;
    }

    if ( GetImageHeight(image) != m_Map.GetMap().GetTileset().GetTileHeight() )
    {
        m_ErrorMessage = "Image used in SetTileImage call doesn't match the tile height";
        return false;
    }

    RGBA* pixels = LockImage(image);
    if (!pixels)
    {
        m_ErrorMessage = "LockImage failed!!";
        return false;
    }
    CImage32& tile_image = m_Map.GetMap().GetTileset().GetTile(tile);
    CImage32::BlendMode blend_mode = tile_image.GetBlendMode();

    tile_image.SetBlendMode(CImage32::REPLACE);
    for (int x = 0; x < m_Map.GetMap().GetTileset().GetTileWidth(); x++)
    {
        for (int y = 0; y < m_Map.GetMap().GetTileset().GetTileHeight(); y++)
        {
            tile_image.SetPixel(x, y, pixels[y * GetImageWidth(image) + x]);
        }
    }

    UnlockImage(image, false);
    tile_image.SetBlendMode(blend_mode);
    m_Map.UpdateTile(tile);
    m_Map.UpdateSolidTile(tile);

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetTileSurface(int tile, CImage32* surface)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "GetTileSurface() called while map engine was not running";
        return false;
    }
    if (tile < 0 || tile >= m_Map.GetMap().GetTileset().GetNumTiles())
    {
        m_ErrorMessage = "Tile index does not exist";
        return false;
    }
    *surface = m_Map.GetMap().GetTileset().GetTile(tile);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetTileSurface(int tile, const CImage32* surface)
{
    if (!surface)
    {
        m_ErrorMessage = "SetTileSurface() failed!!";
        return false;
    }
    if (!m_IsRunning)
    {
        m_ErrorMessage = "SetTileSurface() called while map engine was not running";
        return false;
    }
    if (tile < 0 || tile >= m_Map.GetMap().GetTileset().GetNumTiles())
    {
        m_ErrorMessage = "Tile index does not exist";
        return false;
    }
    int width  = surface->GetWidth();
    int height = surface->GetHeight();
    const RGBA* src_pixels = surface->GetPixels();
    if ( width != m_Map.GetMap().GetTileset().GetTileWidth() )
    {
        m_ErrorMessage = "Image used in SetTileSurface call doesn't match the tile width";
        return false;
    }
    if ( height != m_Map.GetMap().GetTileset().GetTileHeight() )
    {
        m_ErrorMessage = "Image used in SetTileSurface call doesn't match the tile height";
        return false;
    }
    RGBA* dest_pixels = m_Map.GetMap().GetTileset().GetTile(tile).GetPixels();
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            dest_pixels[y * width + x] = src_pixels[y * width + x];
        }
    }
    m_Map.UpdateTile(tile);
    m_Map.UpdateSolidTile(tile);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetTileDelay(int tile, int& delay)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "GetTileDelay() called while map engine was not running";
        return false;
    }

    if (tile < 0 || tile >= m_Map.GetMap().GetTileset().GetNumTiles())
    {
        m_ErrorMessage = "Tile index does not exist";
        return false;
    }

    delay = m_Map.GetMap().GetTileset().GetTile(tile).GetDelay();
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetTileDelay(int tile, int delay)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "SetTileDelay() called while map engine was not running";
        return false;
    }

    if (tile < 0 || tile >= m_Map.GetMap().GetTileset().GetNumTiles())
    {
        m_ErrorMessage = "Tile index does not exist";
        return false;
    }

    if (delay < 0)
    {
        m_ErrorMessage = "Delay must be greater than zero";
        return false;
    }

    m_Map.GetMap().GetTileset().GetTile(tile).SetDelay(delay);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetNextAnimatedTile(int& tile)
{
    if (tile < 0 || tile >= m_Map.GetMap().GetTileset().GetNumTiles())
    {
        m_ErrorMessage = "Tile index does not exist";
        return false;
    }

    if (m_Map.GetMap().GetTileset().GetTile(tile).GetDelay() > 0)
        tile = m_Map.GetMap().GetTileset().GetTile(tile).GetNextTile();

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetNextAnimatedTile(int current_tile, int next_tile)
{
    if (current_tile < 0 || current_tile >= m_Map.GetMap().GetTileset().GetNumTiles())
    {
        m_ErrorMessage = "Tile index does not exist";
        return false;
    }

    if (next_tile < 0 || next_tile >= m_Map.GetMap().GetTileset().GetNumTiles())
    {
        m_ErrorMessage = "Tile index does not exist";
        return false;
    }

    m_Map.GetMap().GetTileset().GetTile(current_tile).SetNextTile(next_tile);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::ReplaceTilesOnLayer(int layer, int old_tile, int new_tile)
{
    if ( IsInvalidLayerError(layer, "ReplaceTilesOnLayer()") )
        return false;

    sMap& map = m_Map.GetMap();
    sLayer& l = map.GetLayer(layer);
    sTileset& tileset = map.GetTileset();
    if (old_tile < 0 || old_tile >= tileset.GetNumTiles())
    {
        m_ErrorMessage = "Invalid old tile index in ReplaceTilesOnLayer";
        return false;
    }
    if (new_tile < 0 || new_tile >= tileset.GetNumTiles())
    {
        m_ErrorMessage = "Invalid new tile index in ReplaceTilesOnLayer";
        return false;
    }

    l.Replace(old_tile, new_tile);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::ExecuteTrigger(int location_x, int location_y, int layer)
{
    // check to see which trigger we're looking at on
    int trigger_index = FindTrigger(location_x, location_y, layer);
    return ExecuteTriggerScript(trigger_index);
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::AreZonesAt(int location_x, int location_y, int layer, bool& found)
{
    found = false;
    if ( IsInvalidLayerError(layer, "AreZonesAt()") )
        return false;

    for (int i = 0; i < m_Map.GetMap().GetNumZones(); i++)
    {
        if ( IsPointWithinZone(location_x, location_y, layer, i) )
        {
            found = true;
            return true;
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::ExecuteZones(int location_x, int location_y, int layer)
{
    // this doesn't use layers, but I'll include the layer parameter just incase
    if ( IsInvalidLayerError(layer, "ExecuteZones()") )
        return false;

    std::string current_map = m_CurrentMap;
    bool found = false;
    for (int i = 0; i < m_Map.GetMap().GetNumZones(); i++)
    {
        if (IsPointWithinZone(location_x, location_y, layer, i))
        {
            found = true;

            m_CurrentZone = i;
            if ( !ExecuteZoneScript(i) )
            {
                return false;
            }

            if (current_map != m_CurrentMap)
                return true;
        }
    }

    if (!found)
    {
        m_ErrorMessage = "There are no zones at (" + itos(location_x) + ", "
                         + itos(location_y) + ") on layer " + itos(layer);
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetNumZones(int& zones)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "GetNumZones() called while map engine was not running";
        return false;
    }

    zones = m_Map.GetMap().GetNumZones();
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetCurrentZone(int& zone)
{
    if (m_CurrentZone == -1)
    {
        m_ErrorMessage = "GetCurrentZone() called outside of a zone script";
        return false;
    }

    zone = m_CurrentZone;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetZoneX(int zone, int& x)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "GetZoneX() called while map engine was not running";
        return false;
    }

    if (zone < 0 || zone > m_Map.GetMap().GetNumZones())
    {
        m_ErrorMessage = "Invalid zone index: " + itos(zone);
        return false;
    }

    x = m_Map.GetMap().GetZone(zone).x1;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetZoneY(int zone, int& y)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "GetZoneY() called while map engine was not running";
        return false;
    }

    if (zone < 0 || zone > m_Map.GetMap().GetNumZones())
    {
        m_ErrorMessage = "Invalid zone index: " + itos(zone);
        return false;
    }

    y = m_Map.GetMap().GetZone(zone).y1;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetZoneWidth(int zone, int& width)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "GetZoneWidth() called while map engine was not running";
        return false;
    }

    if (zone < 0 || zone > m_Map.GetMap().GetNumZones())
    {
        m_ErrorMessage = "Invalid zone index: " + itos(zone);
        return false;
    }

    width = m_Map.GetMap().GetZone(zone).x2 - m_Map.GetMap().GetZone(zone).x1;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetZoneHeight(int zone, int& height)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "GetZoneHeight() called while map engine was not running";
        return false;
    }

    if (zone < 0 || zone > m_Map.GetMap().GetNumZones())
    {
        m_ErrorMessage = "Invalid zone index: " + itos(zone);
        return false;
    }

    height = m_Map.GetMap().GetZone(zone).y2 - m_Map.GetMap().GetZone(zone).y1;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetZoneLayer(int zone, int& layer)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "GetZoneLayer() called while map engine was not running";
        return false;
    }
    if (zone < 0 || zone > m_Map.GetMap().GetNumZones())
    {
        m_ErrorMessage = "Invalid zone index: " + itos(zone);
        return false;
    }
    layer = m_Map.GetMap().GetZone(zone).layer;
    return true;
}
////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetZoneLayer(int zone, int layer)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "SetZoneLayer() called while map engine was not running";
        return false;
    }
    if (zone < 0 || zone > m_Map.GetMap().GetNumZones())
    {
        m_ErrorMessage = "Invalid zone index: " + itos(zone);
        return false;
    }
    if ( IsInvalidLayerError(layer, "SetZoneLayer()") )
        return false;
    m_Map.GetMap().GetZone(zone).layer = layer;
    return true;
}
////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetZoneDimensions(int zone, int x1, int y1, int x2, int y2)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "SetZoneDimensions() called while map engine was not running";
        return false;
    }
    if (zone < 0 || zone > m_Map.GetMap().GetNumZones())
    {
        m_ErrorMessage = "Invalid zone index: " + itos(zone);
        return false;
    }
	m_Map.GetMap().UpdateZone(zone, x1, y1, x2, y2); 
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/*
bool
CMapEngine::AddZone(int x1, int y1, int x2, int y2, int layer, const char* script)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "AddZone() called while map engine was not running";
        return false;
    }

	sMap::sZone zone; <- needs to be Zone struct, but how convert it?
	zone.x1 = x1;
	zone.y1 = y1;
	zone.x2 = x2;
	zone.y2 = y2;
	if ( IsInvalidLayerError(layer, "AddZone()") )
		return false;
	zone.layer = layer;

    if (zone.x1 > zone.x2)
    {
        std::swap(zone.x1, zone.x2);
    }
    if (zone.y1 > zone.y2)
    {
        std::swap(zone.y1, zone.y2);
    }
	std::string error;
	zone.script = CompileScript(script, &error);
	if (zone.script == NULL)
    {
        m_ErrorMessage = "AddZone() Could not compile script\n" + error;
        return false;
    }
    m_Map.GetMap().m_Zones.push_back(zone);
	m_Map.GetMap().AddZone(zone);
	    // zones
	::m_Zones;

    return true;
}

*/
// TODO: SetZone(zone, x, y, width, heigth, layer); AddZone DeleteZone

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetNumObstructionSegments(int layer, int& num_segments)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "GetNumObstructionSegments() called while map engine was not running";
        return false;
    }
    if ( IsInvalidLayerError(layer, "SetZoneLayer()") )
        return false;
    sObstructionMap& obs_map = m_Map.GetMap().GetLayer(layer).GetObstructionMap();
    num_segments = obs_map.GetNumSegments();
    return true;
}
////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::RenderMap()
{
    if (m_IsRunning)
        return Render();
    else
    {
        m_ErrorMessage = "RenderMap() called while map engine was not running";
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetColorMask(RGBA color, int num_frames)
{
    if (num_frames == 0)
    {
        m_CurrentColorMask     = color;
        m_PreviousColorMask    = color;
        m_DestinationColorMask = color;
    }

    if (m_CurrentColorMask.alpha == 0)
    {
        m_CurrentColorMask = color;
        m_CurrentColorMask.alpha = 0;
    }

    m_NumFrames = num_frames;
    m_FramesLeft = num_frames;
    m_PreviousColorMask = m_CurrentColorMask;
    m_DestinationColorMask = color;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetDelayScript(int num_frames, const char* script)
{
    if (m_IsRunning)
    {
        // compile the script
        DelayScript ds;
        ds.frames_left = num_frames;
        std::string error;
        ds.script = m_Engine->CompileScript(script, error);
        if (ds.script == NULL)
        {
            m_ErrorMessage = "Could not compile delay script\n" + error;
            return false;
        }

        m_DelayScripts.push_back(ds);
        return true;
    }
    else
    {
        m_ErrorMessage = "SetDelayScript() called while map engine was not running";
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////

inline bool
CMapEngine::IsKeyBound(int key)
{
    return (m_BoundKeys.count(key) > 0);
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::BindKey(int key, const char* on_key_down, const char* on_key_up)
{
    // unbind previous binding (if it exists)
    if (m_BoundKeys.count(key) > 0)
    {
        UnbindKey(key);
    }

    // compile the two scripts
    std::string error;
    KeyScripts ks;

    ks.down = m_Engine->CompileScript(on_key_down, error);
    if (ks.down == NULL)
    {
        m_ErrorMessage = "OnKeyDown script compile failed in BindKey()\n" + error;
        return false;
    }

    ks.up = m_Engine->CompileScript(on_key_up, error);
    if (ks.up == NULL)
    {
        m_Engine->DestroyScript(ks.down);
        m_ErrorMessage = "OnKeyUp script compile failed in BindKey()\n" + error;
        return false;
    }

    m_BoundKeys[key] = ks;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::UnbindKey(int key)
{
    if (m_BoundKeys.count(key) > 0)
    {
        m_Engine->DestroyScript(m_BoundKeys[key].down);
        m_Engine->DestroyScript(m_BoundKeys[key].up);
        m_BoundKeys.erase(key);
        return true;
    }
    else
    {
        m_ErrorMessage = "UnbindKey() called on an unbound key";
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::IsJoystickButtonBound(int joystick, int button)
{
    int bound_joystick_index = -1;
    for (int i = 0; i < int(m_BoundJoysticks.size()); ++i)
    {
        if (m_BoundJoysticks[i].m_Joystick == joystick)
        {
            bound_joystick_index = i;
            break;
        }
    }
    if (bound_joystick_index == -1)
    {
        return false;
    }
    return (m_BoundJoysticks[bound_joystick_index].m_BoundButtons.count(button) > 0);
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::BindJoystickButton(int joystick, int button, const char* on_key_down, const char* on_key_up)
{
    int bound_joystick_index = -1;
    for (int i = 0; i < int(m_BoundJoysticks.size()); ++i)
    {
        if (m_BoundJoysticks[i].m_Joystick == joystick)
        {
            bound_joystick_index = i;
            break;
        }
    }
    if (bound_joystick_index == -1)
    {
        BoundJoystick joy;
        joy.m_Joystick = joystick;
        joy.m_Buttons.resize(GetNumJoystickButtons(joystick));
        for (int i = 0; i < GetNumJoystickButtons(joystick); i++)
        {
            joy.m_Buttons[i] = IsJoystickButtonPressed(joystick, i);
        }
        bound_joystick_index = m_BoundJoysticks.size();
        m_BoundJoysticks.push_back(joy);
        if (bound_joystick_index < 0 || bound_joystick_index >= int(m_BoundJoysticks.size()))
        {
            return false;
        }
    }
    // unbind previous binding (if it exists)
    if (m_BoundJoysticks[bound_joystick_index].m_BoundButtons.count(button) > 0)
    {
        UnbindJoystickButton(joystick, button);
    }
    // compile the two scripts
    std::string error;
    KeyScripts ks;
    ks.down = m_Engine->CompileScript(on_key_down, error);
    if (ks.down == NULL)
    {
        m_ErrorMessage = "OnKeyDown script compile failed in BindJoystickButton()\n" + error;
        return false;
    }
    ks.up = m_Engine->CompileScript(on_key_up, error);
    if (ks.up == NULL)
    {
        m_Engine->DestroyScript(ks.down);
        m_ErrorMessage = "OnKeyUp script compile failed in BindJoystickButton()\n" + error;
        return false;
    }
    m_BoundJoysticks[bound_joystick_index].m_BoundButtons[button] = ks;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::UnbindJoystickButton(int joystick, int button)
{
    int bound_joystick_index = -1;
    for (int i = 0; i < int(m_BoundJoysticks.size()); ++i)
    {
        if (m_BoundJoysticks[i].m_Joystick == joystick)
        {
            bound_joystick_index = i;
            break;
        }
    }
    if (bound_joystick_index == -1)
    {
        m_ErrorMessage = "UnbindJoystickButton() called on an unbound joystick";
        return false;
    }
    if (m_BoundJoysticks[bound_joystick_index].m_BoundButtons.count(button) > 0)
    {
        m_Engine->DestroyScript(m_BoundJoysticks[bound_joystick_index].m_BoundButtons[button].down);
        m_Engine->DestroyScript(m_BoundJoysticks[bound_joystick_index].m_BoundButtons[button].up);
        m_BoundJoysticks[bound_joystick_index].m_BoundButtons.erase(button);
        return true;
    }
    else
    {
        m_ErrorMessage = "UnbindJoystickButton() called on an unbound button";
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::IsInvalidPersonError(const char* person_name, int& person_index)
{
    person_index = FindPerson(person_name);

    if (person_index == -1)
    {
        m_ErrorMessage = "Person '" + std::string(person_name) + "' doesn't exist";
        return true;
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::IsInvalidLayerError(int layer, const char* calling_func)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = std::string(calling_func) + " called while map engine isn't running";
        return true;
    }

    // make sure layer is valid
    if (layer < 0 || layer >= m_Map.GetMap().GetNumLayers())
    {
        m_ErrorMessage = "Invalid layer index: " + itos(layer);
        return true;
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::AttachInput(const char* name)
{
    return AttachPlayerInput(name, 0);
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::DetachInput()
{
    if (m_IsInputAttached)
    {
        DetachPlayerInput(m_Persons[m_InputPerson].name.c_str());
    }
    m_InputPerson = -1;
    m_IsInputAttached = false;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::IsInputAttached(bool& attached)
{
    attached = m_IsInputAttached;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetInputPerson(std::string& person)
{
    if (m_IsInputAttached)
    {
        person = m_Persons[m_InputPerson].name;
        return true;
    }
    else
    {
        m_ErrorMessage = "Input not attached!";
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::AttachPlayerInput(const char* name, int player)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }
    if (player < 0)
    {
        m_ErrorMessage = "Player index must be greater than zero";
        return false;
    }
    if (player >= 4)
    {
        m_ErrorMessage = "Player index must be less than four";
        return false;
    }
    // detach player
    for (unsigned int i = 0; i < m_Persons.size(); i++)
    {
        if (m_Persons[i].player_index == player)
        {
            for (unsigned int j = 0; j < m_InputPersons.size(); j++)
            {
                if (m_InputPersons[j] == i)
                {
                    m_Persons[i].player_index = -1;
                    m_InputPersons.erase(m_InputPersons.begin() + j);
                    break;
                }
            }
            break;
        }
    }
    Person& p = m_Persons[person];
    p.player_index = player;
    if (player == 0)
    {
        m_InputPerson = person;
        m_IsInputAttached = true;
    }
    m_InputPersons.push_back(person);
    struct __PLAYERCONFIG__* config = GetPlayerConfig(player);
    if (config)
    {
        p.key_up    = config->key_up;
        p.key_down  = config->key_down;
        p.key_left  = config->key_left;
        p.key_right = config->key_right;
        p.key_a     = config->key_a;
        p.key_b     = config->key_b; // key_b?
        p.key_x     = config->key_x;
        p.key_y     = config->key_y;
        p.keyboard_input_allowed = config->keyboard_input_allowed;
        p.joypad_input_allowed   = config->joypad_input_allowed;
    }
    else
    {
        p.keyboard_input_allowed = false;
        p.joypad_input_allowed = false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::DetachPlayerInput(const char* name)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }
    int player = -1;
    for (int i = 0; i < int(m_InputPersons.size()); i++)
    {
        if (m_InputPersons[i] == person)
        {
            player = m_Persons[m_InputPersons[i]].player_index;
            m_Persons[m_InputPersons[i]].player_index = -1;
            m_InputPersons.erase(m_InputPersons.begin() + i);
            break;
        }
    }
    if (player == 0)
    {
        m_InputPerson = -1;
        m_IsInputAttached = false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetUpdateScript(const char* script)
{
    // destroy the previous script
    if (m_UpdateScript)
    {
        m_Engine->DestroyScript(m_UpdateScript);
        m_UpdateScript = NULL;
    }

    // try to compile the script
    std::string error;
    IEngine::script s = m_Engine->CompileScript(script, error);
    if (s == NULL)
    {
        m_ErrorMessage = "Could not compile update script";
        return false;
    }

    m_UpdateScript = s;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetRenderScript(const char* script)
{
    // destroy the previous script
    if (m_RenderScript)
    {
        m_Engine->DestroyScript(m_RenderScript);
        m_RenderScript = NULL;
    }

    // try to compile the script
    std::string error;
    IEngine::script s = m_Engine->CompileScript(script, error);
    if (s == NULL)
    {
        m_ErrorMessage = "Could not compile render script";
        return false;
    }

    m_RenderScript = s;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetLayerRenderer(int layer, const char* script)
{
    if (IsInvalidLayerError(layer, "SetLayerRenderer()"))
        return false;

    if (m_LayerRenderers.empty())
    {
        m_ErrorMessage = "Layer renderers are not available";
        return false;
    }

    // destroy old layer renderer
    if (m_LayerRenderers[layer])
    {
        m_Engine->DestroyScript(m_LayerRenderers[layer]);
        m_LayerRenderers[layer] = NULL;
    }

    // compile the new one
    std::string error;
    IEngine::script s = m_Engine->CompileScript(script, error);
    if (s == NULL)
    {
        m_ErrorMessage = "Could not compile layer renderer";
        return false;
    }

    m_LayerRenderers[layer] = s;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetLayerMask(int layer, RGBA mask)
{
    if ( IsInvalidLayerError(layer, "SetLayerMask()") )
        return false;

    m_Map.SetLayerMask(layer, mask);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetLayerMask(int layer, RGBA& mask)
{
    if ( IsInvalidLayerError(layer, "GetLayerMask()") )
        return false;

    mask = m_Map.GetLayerMask(layer);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::AttachCamera(const char* name)
{
    // make sure the person entity exists
    m_CameraPerson = -1;
    if ( IsInvalidPersonError(name, m_CameraPerson) )
    {
        return false;
    }

    m_IsCameraAttached = true;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::DetachCamera()
{
    m_IsCameraAttached = false;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::IsCameraAttached(bool& attached)
{
    attached = m_IsCameraAttached;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetCameraPerson(std::string& person)
{
    if (m_IsCameraAttached)
    {
        person = m_Persons[m_CameraPerson].name;
        return true;
    }
    else
    {
        m_ErrorMessage = "Camera not attached!";
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetCameraX(int x)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "SetCameraX() called while map engine was not running";
        return false;
    }

    m_Camera.x = x;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetCameraY(int y)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "SetCameraY() called while map engine was not running";
        return false;
    }

    m_Camera.y = y;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetCameraX(int& x)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "GetCameraX() called while map engine was not running";
        return false;
    }

    x = m_Camera.x;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetCameraY(int& y)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "GetCameraY() called while map engine was not running";
        return false;
    }

    y = m_Camera.y;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::MapToScreenX(int layer, int mx, int& sx)
{
    if ( IsInvalidLayerError(layer, "MapToScreenX()") )
        return false;
    sx = m_Map.MapToScreenX(layer, m_Camera.x, mx);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::MapToScreenY(int layer, int my, int& sy)
{
    if ( IsInvalidLayerError(layer, "MapToScreenY()") )
        return false;

    sy = m_Map.MapToScreenY(layer, m_Camera.y, my);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::ScreenToMapX(int layer, int sx, int& mx)
{
    if ( IsInvalidLayerError(layer, "ScreenToMapX()") )
        return false;

    mx = m_Map.ScreenToMapX(layer, m_Camera.x, sx);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::ScreenToMapY(int layer, int sy, int& my)
{
    if ( IsInvalidLayerError(layer, "ScreenToMapY()") )
        return false;

    my = m_Map.ScreenToMapY(layer, m_Camera.y, sy);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetPersonList(std::vector<std::string>& list, int& size)
{
    list.resize(m_Persons.size());
	if (list.size() !=m_Persons.size())
		return false; //whoops! couldnt resize
	size = list.size();

    unsigned int j = 0;
    for (unsigned int i = 0; (i < m_Persons.size() && i < list.size()); i++)
    {
        if ( !m_Persons[i].name.empty() )
        {
			list[j] = m_Persons[i].name;
			j++;
        }
    }

    if( j < m_Persons.size() ) list.resize(j);
    return true;
}

bool
CMapEngine::DoesPersonExist(const char* name)
{
	return FindPerson(name) > -1;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::CreateDefaultPerson(Person& p, const char* name, const char* spriteset_filename, bool destroy_with_map)
{
    p.name = name;
    p.destroy_with_map = destroy_with_map;

    p.is_visible = true;
    for (int script_index = 0; script_index < NUM_PERSON_SCRIPTS; script_index++)
    {
        p.person_scripts[script_index] = NULL;
    }
    p.player_index = -1;
    p.key_up    = -1;
    p.key_down  = -1;
    p.key_left  = -1;
    p.key_right = -1;
    p.on_trigger = false;
    p.last_trigger = -1;
    p.leader = -1;
    p.offset_x = 0;
    p.offset_y = 0;
    p.speed_x = 1;
    p.speed_y = 1;
    p.scale_x = 1;
    p.scale_y = 1;
    p.stepping_frame_revert = 0;
    p.stepping_frame_revert_count = 0;
    p.is_angled = false;
    p.angle = 0;
    p.ignorePersonObstructions = false;
    p.ignoreTileObstructions = false;
	p.obs_person = -1;
    p.mask = CreateRGBA(255, 255, 255, 255);
    p.frame = 0;
    p.spriteset = m_Engine->LoadSpriteset(spriteset_filename);
    if (p.spriteset == NULL)
    {
        m_ErrorMessage = "Could not load spriteset: '" + std::string(spriteset_filename) + "'\nPerson: " + p.description;
        return false;
    }

    // put them in the starting position by default
    if (m_IsRunning)
    {
        p.x     = m_Map.GetMap().GetStartX();
        p.y     = m_Map.GetMap().GetStartY();
        p.layer = m_Map.GetMap().GetStartLayer();
    }
    else
    {
        p.x     = 0;
        p.y     = 0;
        p.layer = 0;
    }

    p.spriteset->GetSpriteset().GetBase(p.base_x1, p.base_y1, p.base_x2, p.base_y2);
    if (p.base_x1 > p.base_x2) std::swap(p.base_x1, p.base_x2);
    if (p.base_y1 > p.base_y2) std::swap(p.base_y1, p.base_y2);
    p.width  = p.spriteset->GetSpriteset().GetFrameWidth();
    p.height = p.spriteset->GetSpriteset().GetFrameHeight();

    p.direction = p.spriteset->GetSpriteset().GetDirectionName(0);
	p.dx = p.dy = p.hx = p.hy = 0;
    /*
    char debug_str2[1000] = {0};
    sprintf (debug_str2, "%s", p.direction.c_str());
    if (p.frame < 0 || p.frame >= p.spriteset->GetSpriteset().GetNumFrames(p.direction)) {
      m_ErrorMessage = "Bad frame! Bad! " + itos(p.frame);
      m_ErrorMessage += "...\n" + p.direction;
      return false;
    }
    */
    p.next_frame_switch = p.spriteset->GetSpriteset().GetFrameDelay(p.direction, p.frame);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::CreatePerson(const char* name, const char* spriteset_filename, bool destroy_with_map)
{
    Person p;
    p.description = std::string("name=[") + name + "]";

    // Note: CreateDefaultPerson loads the spriteset
    if (!CreateDefaultPerson(p, name,  spriteset_filename, destroy_with_map))
    {
        return false;
    }

    m_Persons.push_back(p);
    // execute default script_create
    std::string error;
    if (m_default_person_scripts[SCRIPT_ON_CREATE] != NULL)
    {
        if (!m_Engine->IsScriptBeingUsed(m_DefaultMapScripts[SCRIPT_ON_CREATE]))
        {
            const std::string old_person = m_CurrentPerson;
            m_CurrentPerson = name;
            if (!ExecuteScript(m_default_person_scripts[SCRIPT_ON_CREATE], error))
            {
                m_ErrorMessage = "Could not execute default OnCreate script\nPerson:" + p.description + "\n" + error;
                m_CurrentPerson = old_person;
                return false;
            }
            m_CurrentPerson = old_person;
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::DestroyPerson(const char* name)
{
    // make sure the person entity exists
    for (int i = 0; i < int(m_Persons.size()); i++)
    {
        if (m_Persons[i].name == name)
        {
            int j;
            // detach camera if necessary
            if (i == m_CameraPerson)
            {
                m_IsCameraAttached = false;
            }
            else if (m_CameraPerson > i)
            {
                m_CameraPerson--;
            }
            // detach input if necessary
            if (i == m_InputPerson)
            {
                m_IsInputAttached = false;
            }
            else if (m_InputPerson > i)
            {
                m_InputPerson--;
            }

            for (j = 0; j < int(m_InputPersons.size()); j++)
            {
                if (m_InputPersons[j] == i)
                {
                    m_Persons[m_InputPersons[j]].player_index = -1;
                    m_InputPersons.erase(m_InputPersons.begin() + j);
                    j--;
                }
                else if (m_InputPersons[j] > i)
                {
                    m_InputPersons[j]--;
                }
            }
            // update all leader indices
            for (j = 0; j < int(m_Persons.size()); j++)
            {
                if (i != j)
                {
                    if (m_Persons[j].leader > i)
                    {
                        m_Persons[j].leader--;
                    }
                    else if (m_Persons[j].leader == i)
                    {
                        m_Persons[j].leader = -1;
                        m_Persons[j].follow_state_queue.resize(0);
                    }
                }
            }

            // destroy the person entity
            if (!DestroyPersonStructure(m_Persons[i]))
            {
                return false;
            }

            m_Persons.erase(m_Persons.begin() + i);
            return true;

        }
    }

    m_ErrorMessage = "Person '" + std::string(name) + "' doesn't exist";
    return false;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetPersonX(const char* name, int x)
{
    // find person
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    // !!!! verify x
    // if person has a leader, ignore the command
    if (m_Persons[person].leader != -1)
    {
        return true;
    }

    m_Persons[person].x = x;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetPersonY(const char* name, int y)
{
    // find person
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    // !!!! verify y
    // if person has a leader, ignore the command
    if (m_Persons[person].leader != -1)
    {
        return true;
    }

    m_Persons[person].y = y;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetPersonOffsetX(const char* name, int offx)
{
    // find person
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
        return false;

    m_Persons[person].offset_x = offx;

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetPersonOffsetY(const char* name, int offy)
{
    // find person
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
        return false;

    m_Persons[person].offset_y = offy;

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetPersonXYFloat(const char* name, double x, double y)
{
    // find person
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    // !!!! verify x and y
    // if person has a leader, ignore the command
    if (m_Persons[person].leader != -1)
    {
        return true;
    }

    m_Persons[person].x = x;
    m_Persons[person].y = y;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetPersonLayer(const char* name, int layer)
{
    // find person
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    // verify layer
    if ( IsInvalidLayerError(layer, "SetPersonLayer()") )
        return false;

    // if person has a leader, ignore the command
    if (m_Persons[person].leader != -1)
    {
        return true;
    }

    m_Persons[person].layer = layer;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetPersonDirection(const char* name, const char* direction)
{
    // find person
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    Person& p = m_Persons[person];
    // if person has a leader, ignore the command
    if (p.leader != -1)
    {
        return true;
    }

    if (p.spriteset->GetSpriteset().GetDirectionNum(direction) == -1)
    {
        m_ErrorMessage = "Person '" + std::string(name) + "' direction '" + std::string(direction) + "' doesn't exist";
        return false;
    }

    p.direction = direction;
    // make sure 'frame' is valid
    m_Persons[person].frame %= p.spriteset->GetSpriteset().GetNumFrames(p.direction);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetPersonFrame(const char* name, int frame)
{
    // find person
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    Person& p = m_Persons[person];
    // if person has a leader, ignore the command
    if (p.leader != -1)
    {
        return true;
    }

    if (frame < 0)
    {
        frame = 0;
    }
    else
    {
        frame %= p.spriteset->GetSpriteset().GetNumFrames(p.direction);
    }

    m_Persons[person].frame = frame;
    m_Persons[person].stepping_frame_revert_count = 0;
    m_Persons[person].next_frame_switch = p.spriteset->GetSpriteset().GetFrameDelay(p.direction, p.frame);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetPersonFrameNext(const char* name, int& frameDelay)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    frameDelay = int(m_Persons[person].next_frame_switch);
    return true;
}

bool
CMapEngine::SetPersonFrameNext(const char* name, int frameDelay)
{
    // find person
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    //Person& p = m_Persons[person];
    // if person has a leader, ignore the command (actually, lets see what happens...)
    //if (p.leader != -1)
    //{
    //    return true;
    //}

    if (frameDelay < 0)
    {
        frameDelay = 0;
    }
    m_Persons[person].stepping_frame_revert_count = 0;
    m_Persons[person].next_frame_switch = frameDelay; //p.spriteset->GetSpriteset().GetFrameDelay(p.direction, p.frame);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
CMapEngine::GetPersonX(const char* name, int& x)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    x = int(m_Persons[person].x);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetPersonY(const char* name, int& y)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    y = int(m_Persons[person].y);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetPersonOffsetX(const char* name, int& x)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    x = m_Persons[person].offset_x;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetPersonOffsetY(const char* name, int& y)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    y = m_Persons[person].offset_y;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetPersonXFloat(const char* name, double& x)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    x = m_Persons[person].x;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetPersonYFloat(const char* name, double& y)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    y = m_Persons[person].y;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetPersonLayer(const char* name, int& layer)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    layer = m_Persons[person].layer;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetPersonDirection(const char* name, std::string& direction)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    direction = m_Persons[person].direction;
//  direction = m_Persons[person].spriteset->GetSpriteset().GetDirectionNum(name);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::IsPersonVisible(const char* person_name, bool& visible)
{
    int person = -1;
    if ( IsInvalidPersonError(person_name, person) )
    {
        return false;
    }
    visible = m_Persons[person].is_visible;
    return true;
}
////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetPersonVisible(const char* person_name, bool visible)
{
    int person = -1;
    if ( IsInvalidPersonError(person_name, person) )
    {
        return false;
    }
    m_Persons[person].is_visible = visible;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::IgnorePersonObstructions(const char* name, bool ignoring)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    m_Persons[person].ignorePersonObstructions = ignoring;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::IsIgnoringPersonObstructions(const char* name, bool& ignoring)
{
    int person = -1;
    ignoring = false;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    ignoring = m_Persons[person].ignorePersonObstructions;
    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::IgnoreTileObstructions(const char* name, bool ignoring)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    m_Persons[person].ignoreTileObstructions = ignoring;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::IsIgnoringTileObstructions(const char* name, bool& ignoring)
{
    int person = -1;
    ignoring = false;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    ignoring = m_Persons[person].ignoreTileObstructions;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetPersonIgnoreList(const char* name, std::vector<std::string> ignore_list)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }
    m_Persons[person].ignored_persons = ignore_list;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetPersonIgnoreList(const char* name, std::vector<std::string>& ignore_list)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }
    ignore_list = m_Persons[person].ignored_persons;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetPersonFrame(const char* name, int& frame)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    frame = m_Persons[person].frame;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetPersonFrameRevert(const char* name, int i)
{
    // find person
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    m_Persons[person].stepping_frame_revert = i;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetPersonFrameRevert(const char* name, int& i)
{
    // find person
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    i = m_Persons[person].stepping_frame_revert;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetPersonSpeedXY(const char* name, double x, double y)
{
    // find person
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    // !!!! verify x
    // if person has a leader, ignore the command
    if (m_Persons[person].leader != -1)
    {
        return true;
    }

    m_Persons[person].speed_x = x;
    m_Persons[person].speed_y = y;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetPersonSpeedX(const char* name, double& x)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    x = m_Persons[person].speed_x;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetPersonSpeedY(const char* name, double& y)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    y = m_Persons[person].speed_y;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetPersonVectorXY(const char* name, bool historical, int& x, int& y)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

	if(historical){
        x = m_Persons[person].hx;
        y = m_Persons[person].hy;
    }else{
        x = m_Persons[person].dx;
        y = m_Persons[person].dy;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetPersonScaleAbsolute(const char* name, int width, int height)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    // convert to float. I can't figure out how to calculate the base in
    // absolute mode...
    const Person& s = m_Persons[person];
    double old_w = s.spriteset->GetSpriteset().GetFrameWidth();
    double old_h = s.spriteset->GetSpriteset().GetFrameHeight();
    double new_w = width;
    double new_h = height;

    double scale_w = new_w / old_w;
    double scale_h = new_h / old_h;

    return SetPersonScaleFactor(name, scale_w, scale_h);
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetPersonScaleFactor(const char* name, double scale_w, double scale_h)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    Person& p = m_Persons[person];
    // convert to integer ;)
    double width = p.spriteset->GetSpriteset().GetFrameWidth();
    double height = p.spriteset->GetSpriteset().GetFrameHeight();

    int base_x1;
    int base_y1;
    int base_x2;
    int base_y2;
    //p.spriteset->GetRealBase(base_x1, base_y1, base_x2, base_y2);
    p.spriteset->GetSpriteset().GetRealBase(base_x1, base_y1, base_x2, base_y2);

    p.width =  (int)(scale_w * width);
    p.height = (int)(scale_h * height);

    p.base_x1 = (int)(scale_w * (double)base_x1);
    p.base_y1 = (int)(scale_h * (double)base_y1);
    p.base_x2 = (int)(scale_w * (double)base_x2);
    p.base_y2 = (int)(scale_h * (double)base_y2);

    // oopsies on scaling problems? ;)
    if (p.width  < 1) p.width  = 1;
    if (p.height < 1) p.height = 1;

    if (p.base_x1 < 0 && p.width > 1 && base_x1 != 0) p.base_x1 = 1;
    if (p.base_x2 < 0 && p.width > 1 && base_x2 != 0) p.base_x2 = 1;
    if (p.base_y1 < 0 && p.height > 1 && base_y1 != 0) p.base_y1 = 1;
    if (p.base_y2 < 0 && p.height > 1 && base_y2 != 0) p.base_y2 = 1;

    p.spriteset->SetBase(p.base_x1,p.base_y1,p.base_x2,p.base_y2);

    p.scale_x = scale_w;
    p.scale_y = scale_h;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetPersonAngle(const char* name, double& angle)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    angle = m_Persons[person].angle;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetPersonAngle(const char* name, double angle)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    m_Persons[person].angle = angle;
    m_Persons[person].is_angled = (angle != 0.0); // zero is not angled
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetPersonMask(const char* name, RGBA mask)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    m_Persons[person].mask = mask;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetPersonMask(const char* name, RGBA& mask)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    mask = m_Persons[person].mask;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

class PersonDataUtil
{
public:
    static int FindDataIndex(const std::vector<struct PersonData>& person_data, const char* name)
    {
        int index = -1;
        for (int i = 0; i < int(person_data.size()); i++)
        {
            if (person_data[i].name == name)
            {
                index = i;
                break;
            }
        }
        return index;
    }
    static void SetDataString(std::vector<struct PersonData>& person_data, const char* name, const char* value)
    {
        int index = PersonDataUtil::FindDataIndex(person_data, name);
        if (index != -1)
        {
            person_data[index].string_value = value;
            person_data[index].double_value = 0;
            person_data[index].type = 0;
        }
        else
        {
            PersonData data;
            data.name = name;
            data.string_value = value;
            data.double_value = 0;
            data.type = 0;
            person_data.push_back(data);
        }
    }
    static void SetDataNumber(std::vector<struct PersonData>& person_data, const char* name, const double value, const int type)
    {
        int index = PersonDataUtil::FindDataIndex(person_data, name);
        if (index != -1)
        {
            person_data[index].string_value = "";
            person_data[index].double_value = value;
            person_data[index].type = type;
        }
        else
        {
            PersonData data;
            data.name = name;
            data.string_value = "";
            data.double_value = value;
            data.type = type;
            person_data.push_back(data);
        }
    }
    static void SetDataDouble(std::vector<struct PersonData>& person_data, const char* name, const double value)
    {
        PersonDataUtil::SetDataNumber(person_data, name, (double)value, 1);
    }
    static void SetDataInt(std::vector<struct PersonData>& person_data, const char* name, const int value)
    {
        PersonDataUtil::SetDataNumber(person_data, name, (double)value, 2);
    }
    static void SetDataBool(std::vector<struct PersonData>& person_data, const char* name, const bool value)
    {
        PersonDataUtil::SetDataNumber(person_data, name, (double)value, 3);
    }
};

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetPersonData(const char* name, std::vector<struct PersonData>& person_data)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    Person& p = m_Persons[person];
    person_data = m_Persons[person].person_data;
    PersonDataUtil::SetDataInt(person_data, "num_frames", p.spriteset->GetSpriteset().GetNumFrames(p.direction));
    PersonDataUtil::SetDataInt(person_data, "num_directions", p.spriteset->GetSpriteset().GetNumDirections());
    PersonDataUtil::SetDataInt(person_data, "width", p.width);
    PersonDataUtil::SetDataInt(person_data, "height", p.height);
    PersonDataUtil::SetDataString(person_data, "leader", p.leader == -1 ? "" : m_Persons[p.leader].name.c_str());
    PersonDataUtil::SetDataBool(person_data, "destroy_with_map", p.destroy_with_map);
	PersonDataUtil::SetDataInt(person_data, "obs_person", p.obs_person);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetPersonData(const char* name, const std::vector<struct PersonData> person_data)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    int index = PersonDataUtil::FindDataIndex(person_data, "destroy_with_map");
    if (index != -1) {
      m_Persons[person].destroy_with_map = ((int)person_data[index].double_value == 1) ? true : false;
    }

    m_Persons[person].person_data = person_data;

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetPersonValue(const char* name, const char* key, std::string& string_value, double& double_value, int& type)
{
    std::vector<PersonData> person_data;
    if ( GetPersonData(name, person_data) == false)
        return false;

    int index = PersonDataUtil::FindDataIndex(person_data, key);
    if (index != -1)
    {
        string_value = person_data[index].string_value;
        double_value = person_data[index].double_value;
        type  = person_data[index].type;
    }
    else
    {
        string_value = "";
        double_value = 0;
        type = -1;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetPersonValue(const char* name, const char* key, const std::string value, const double double_value, int type)
{
    std::vector<PersonData> person_data;
    if ( GetPersonData(name, person_data) == false)
        return false;

    switch (type)
    {
      case 0:
        PersonDataUtil::SetDataString(person_data, key, value.c_str());
      break;
      case 1:
      case 2:
      case 3:
        PersonDataUtil::SetDataNumber(person_data, key, double_value, type);
      break;
    }

    if ( SetPersonData(name, person_data) == false)
      return false;

    return true;
}

////////////////////////////////////////////////////////////////////////////////


bool
CMapEngine::SetPersonBase(const char* name, int x1, int y1, int x2, int y2)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    m_Persons[person].base_x1 = x1;
    m_Persons[person].base_y1 = y1;
    m_Persons[person].base_x2 = x2;
    m_Persons[person].base_y2 = y2;
    m_Persons[person].spriteset->SetBase(x1, y1, x2, y2);
    m_Persons[person].spriteset->Base2Real();

    return true;
}


SSPRITESET*
CMapEngine::GetPersonSpriteset(const char* name)
{
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return 0;
    }

    return m_Persons[person].spriteset;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetPersonSpriteset(const char* name, sSpriteset& spriteset)
{
    int person_index = -1;
    if ( IsInvalidPersonError(name, person_index) )
    {
        return false;
    }

    // release the old spriteset
    m_Engine->DestroySpriteset(m_Persons[person_index].spriteset);
    // create and insert the new spriteset (the constructor calls AddRef)
    m_Persons[person_index].spriteset = new SSPRITESET(spriteset);

    if (m_Persons[person_index].spriteset == NULL)
    {
        return false;
    }
    // make sure direction is valid
    bool is_valid_direction = false;
    for (int i = 0; i < spriteset.GetNumDirections(); i++)
    {
        if (m_Persons[person_index].direction == spriteset.GetDirectionName(i))
        {
            is_valid_direction = true;
            break;
        }
    }
    if (is_valid_direction == false)
    {
        m_Persons[person_index].direction = spriteset.GetDirectionName(0);
    }
    // make sure frame is valid
    if (m_Persons[person_index].frame < 0)
    {
        m_Persons[person_index].frame = 0;
    }
    else
    {
        m_Persons[person_index].frame %= spriteset.GetNumFrames(m_Persons[person_index].direction);
    }
    m_Persons[person_index].width  = m_Persons[person_index].spriteset->GetSpriteset().GetFrameWidth();
    m_Persons[person_index].height = m_Persons[person_index].spriteset->GetSpriteset().GetFrameHeight();

    spriteset.GetRealBase(m_Persons[person_index].base_x1, m_Persons[person_index].base_y1, m_Persons[person_index].base_x2, m_Persons[person_index].base_y2);

    SetPersonBase(name, m_Persons[person_index].base_x1, m_Persons[person_index].base_y1, m_Persons[person_index].base_x2, m_Persons[person_index].base_y2);
    SetPersonScaleFactor(name,m_Persons[person_index].scale_x, m_Persons[person_index].scale_y);

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::FollowPerson(const char* follower, const char* leader, int pixels)
{
    // get follower index
    int follower_index = -1;
    if ( IsInvalidPersonError(follower, follower_index) )
    {
        m_ErrorMessage = "Follower person '" + std::string(follower) + "' not found";
        return false;
    }

    // remove any queued up commands
    m_Persons[follower_index].commands.clear();

    // if we want to remove the leader...
    if (strcmp(leader, "") == 0)
    {
        m_Persons[follower_index].leader = -1;
        return true;
    }

    // get leader index
    int leader_index = -1;
    if ( IsInvalidPersonError(leader, leader_index) )
    {
        m_ErrorMessage = "Leader person '" + std::string(leader) + "' not found";
        return false;
    }

    // verify following distance
    if (pixels < 1)
    {
        m_ErrorMessage = "Invalid following distance: " + itos(pixels);
        return false;
    }

    // make sure the leader doesn't ever point back to the follower (circular list)
    int current = leader_index;
    while (current != -1)
    {         // go until we've reached the end
        if (current == follower_index)
        {
            m_ErrorMessage = "Circular reference in following chain";
            return false;
        }

        // go up in the chain
        current = m_Persons[current].leader;
    }

    // give the follower a follow queue
    m_Persons[follower_index].leader = leader_index;
    m_Persons[follower_index].follow_state_queue.resize(pixels);

    // initialize the follow queue
    std::vector<Person::AnimationState>& vas = m_Persons[follower_index].follow_state_queue;
    for (int i = 0; i < pixels; i++)
    {
        vas[i].x         = m_Persons[leader_index].x;
        vas[i].y         = m_Persons[leader_index].y;
        vas[i].layer     = m_Persons[leader_index].layer;
        vas[i].direction = m_Persons[leader_index].direction;
    }

    m_Persons[follower_index].x         = m_Persons[leader_index].x;
    m_Persons[follower_index].y         = m_Persons[leader_index].y;
    m_Persons[follower_index].layer     = m_Persons[leader_index].layer;
    m_Persons[follower_index].direction = m_Persons[leader_index].direction;
    m_Persons[follower_index].frame     = 0;

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetPersonScript(const char* name, int which, const char* script)
{
    // verify the script constant
    if (which < 0 || which >= NUM_PERSON_SCRIPTS)
    {
        m_ErrorMessage = "SetPersonScript() - script does not exist";
        return false;
    }

    // find the person
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    // try to compile the script
    std::string error;
    IEngine::script s = m_Engine->CompileScript(script, error);
    if (s == NULL)
    {
        m_ErrorMessage = "Could not compile script\n" + error;
        return false;
    }

    // find out which script we're changing
    IEngine::script* ps = &m_Persons[person].person_scripts[which];

    // now replace the script
    if (*ps)
    {
        m_Engine->DestroyScript(*ps);
    }
    *ps = s;

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetDefaultPersonScript(int which, const char* script)
{
    // verify the script constant
    if (which < 0 || which >= NUM_PERSON_SCRIPTS)
    {
        m_ErrorMessage = "SetDefaultPersonScript() - script does not exist";
        return false;
    }
    // try to compile the script
    std::string error;
    IEngine::script s = m_Engine->CompileScript(script, error);
    if (s == NULL)
    {
        m_ErrorMessage = "Could not compile script\n" + error;
        return false;
    }
    // find out which script we're changing
    IEngine::script* ps = &m_default_person_scripts[which];
    // now replace the script
    if (*ps)
    {
        m_Engine->DestroyScript(*ps);
    }
    *ps = s;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::CallPersonScript(const char* name, int which)
{
    // make sure 'which' is valid
    if (which < 0 || which >= NUM_PERSON_SCRIPTS)
    {
        m_ErrorMessage = "SetPersonScript() - script does not exist";
        return false;
    }

    // find the person
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    // find out which script we're dealing with
    bool reset_time = false;

    IEngine::script* ps = &m_Persons[person].person_scripts[which];
    if (*ps)
    {
        bool running = m_Engine->IsScriptBeingUsed(*ps);
        if (which == SCRIPT_ON_ACTIVATE_TOUCH || which == SCRIPT_ON_ACTIVATE_TALK)
        {
            reset_time = true;
        }
        std::string list[5] = {"OnCreate", "OnDestroy", "OnActivate (touch)", "OnActivate (talk)", "OnCommandGenerator"};
        if (running)
        {
            m_ErrorMessage = "Person " + list[which] + " script already running!";
            return false;
        }
        else
        {
            std::string error;

            //m_Persons[person].person_scripts_running[which] = true;
            // set the current person
            const std::string old_person = m_CurrentPerson;
            m_CurrentPerson = m_Persons[person].name;
            if ( !ExecuteScript(*ps, error) )
            {
                m_ErrorMessage = "Could not execute person " + list[which] + " script\n" + error;
                //m_Persons[person].person_scripts_running[which] = false;

                m_CurrentPerson = old_person;
                return false;
            }

            m_CurrentPerson = old_person;
            // if we took more than a second to run the person script, reset the timer
            if (reset_time || qword(GetTime()) * m_FrameRate > m_NextFrame)
            {
                ResetNextFrame();
            }
            //m_Persons[person].person_scripts_running[which] = false;
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::CallDefaultPersonScript(const char* name, int which)
{
    // make sure 'which' is valid
    if (which < 0 || which >= NUM_PERSON_SCRIPTS)
    {
        m_ErrorMessage = "SetPersonScript() - script does not exist";
        return false;
    }
    // find the person
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }
    // find out which script we're dealing with
    bool reset_time = false;
    IEngine::script* ps = &m_default_person_scripts[which];
    if (*ps)
    {
        bool running = m_Engine->IsScriptBeingUsed(*ps);
        if (which == SCRIPT_ON_ACTIVATE_TOUCH || which == SCRIPT_ON_ACTIVATE_TALK)
            reset_time = true;
        std::string list[5] = {"OnCreate", "OnDestroy", "OnActivate (touch)", "OnActivate (talk)", "OnCommandGenerator"};
        if (running)
        {
            m_ErrorMessage = "Default " + list[which] + " person script already running!";
            return false;
        }
        else
        {
            std::string error;
            //m_Persons[person].person_scripts_running[which] = true;
            // set the current person
            const std::string old_person = m_CurrentPerson;
            m_CurrentPerson = m_Persons[person].name;
            if ( !ExecuteScript(*ps, error) )
            {
                m_ErrorMessage = "Could not execute default person " + list[which] + " script\n" + error;
                //m_Persons[person].person_scripts_running[which] = false;
                m_CurrentPerson = old_person;
                return false;
            }
            m_CurrentPerson = old_person;
            if (reset_time || qword(GetTime()) * m_FrameRate > m_NextFrame)
            {
                ResetNextFrame();
            }
            //m_Persons[person].person_scripts_running[which] = false;
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetCurrentPerson(std::string& person)
{
    if (m_CurrentPerson.empty())
    {
        m_ErrorMessage = "GetCurrentPerson() called outside of person script";
        return false;
    }

    person = m_CurrentPerson;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::QueuePersonCommand(const char* name, int command, bool immediate)
{
    // make sure command is valid
    if (command < 0 || command >= NUM_COMMANDS)
    {
        m_ErrorMessage = "Invalid command";
        return false;
    }

    // find person
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    // if person has a leader, ignore the command
    if (m_Persons[person].leader != -1)
    {
        return true;
    }

    // add person to queue
    m_Persons[person].commands.push_back(Person::Command(command, immediate));
    return true;
}

///////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::QueuePersonScript(const char* name, const char* script, bool immediate)
{
    // Make sure script is not null
    if ((script == "") || (script == NULL))
    {
        m_ErrorMessage = "Null script.";
        return false;
    }

    // find person
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    // if person has a leader, ignore the command
    if (m_Persons[person].leader != -1)
    {
        return true;
    }

    // add person to queue
    m_Persons[person].commands.push_back(Person::Command(COMMAND_DO_SCRIPT, immediate, std::string(script)));
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::ClearPersonCommands(const char* name)
{
    // find person
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    // if person has a leader, ignore the command
    if (m_Persons[person].leader != -1)
    {
        return true;
    }

    // clear queue
    m_Persons[person].commands.clear();
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::IsCommandQueueEmpty(const char* name, bool& empty)
{
    // find person
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    empty = m_Persons[person].commands.empty();
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::IsPersonObstructed(const char* name, int x, int y, bool& result)
{
    // find person
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        return false;
    }

    int obs_person;
    result = IsObstructed(person, x, y, obs_person);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetTalkActivationKey(int key)
{
    m_TalkActivationKey = key;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

int
CMapEngine::GetTalkActivationKey()
{
    return m_TalkActivationKey;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetTalkActivationButton(int button)
{
    m_JoystickTalkButton = button;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

int
CMapEngine::GetTalkActivationButton()
{
    return m_JoystickTalkButton;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SetTalkDistance(int pixels)
{
    m_TalkActivationDistance = pixels;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

int
CMapEngine::GetTalkDistance()
{
    return m_TalkActivationDistance;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::OpenMap(const char* filename)
{
    unsigned int i;

    // load the map
    std::string path = "maps/";
    bool need_leave = false;
    int skip = pre_process_filename(filename, path, need_leave);

    if (skip != -1)
    {
        filename += skip;
    }
    else if (skip == -1)
    {
        m_ErrorMessage = "Could not load map '";
        m_ErrorMessage += filename;
        m_ErrorMessage += "'";

        return false;
    }

    path += filename;

    if (!m_Map.Load(path.c_str(), m_FileSystem))
    {
        m_ErrorMessage = "Could not load map '";
        m_ErrorMessage += filename;
        m_ErrorMessage += "'";

        if (need_leave)
            LeaveDirectory();

        return false;
    }

    if (need_leave)
        LeaveDirectory();

    m_CurrentMap = filename;

    // if a person entity is here, it's not map-specific
    // so put it in the starting position!
    for (i = 0; i < m_Persons.size(); i++)
    {
        m_Persons[i].x     = m_Map.GetMap().GetStartX();
        m_Persons[i].y     = m_Map.GetMap().GetStartY();
        m_Persons[i].layer = m_Map.GetMap().GetStartLayer();

        // update follow queues
        if (m_Persons[i].leader != -1)
        {
            for (unsigned int j = 0; j < m_Persons[i].follow_state_queue.size(); j++)
            {
                m_Persons[i].follow_state_queue[j].x         = m_Persons[i].x;
                m_Persons[i].follow_state_queue[j].y         = m_Persons[i].y;
                m_Persons[i].follow_state_queue[j].layer     = m_Persons[i].layer;
                m_Persons[i].follow_state_queue[j].direction = m_Persons[i].direction;
            }
        }
    }

    // reset all the layer angles
    for (i = 0; i < (unsigned int)m_Map.GetMap().GetNumLayers(); i++)
    {
        m_Map.SetLayerAngle(i, 0.0);
    }
    if (!CompileEdgeScripts())
    {
        m_CurrentMap = "";
        return false;
    }
    if (!LoadTriggers())
    {
        m_CurrentMap = "";
        return false;
    }
    if (!LoadZones())
    {
        m_CurrentMap = "";
        return false;
    }
    if (!LoadMapPersons())
    {
        m_CurrentMap = "";
        return false;
    }

    // load the background music (if there is any)
    std::string music = m_Map.GetMap().GetMusicFile();
    // Don't reload already-loaded music.
    if (music.length() && music != m_LastMusicPath)
    {
        if (music.rfind(".m3u") == music.size() - 4)
        {
            if (m_Playlist.LoadFromFile(music.c_str()) == false)
            {
                m_ErrorMessage = "Could not load playlist '" + music + "'";
                return false;
            }
            if (m_Playlist.GetNumFiles() > 0)
            {
                if ( !IsMidi(m_Playlist.GetFile(0)) )
                {
                    m_Music = m_Engine->LoadSound(m_Playlist.GetFile(0), false);
                }
#if defined(WIN32) && defined(USE_MIDI)
                if ( IsMidi(m_Playlist.GetFile(0)) )
                {
                    m_Midi = m_Engine->LoadMIDI(m_Playlist.GetFile(0));
                }
#endif
            }
        }
        else
        {
            if ( !IsMidi(music.c_str()) )
            {
                m_Music = m_Engine->LoadSound(music.c_str(), true);
            }
#if defined(WIN32) && defined(USE_MIDI)
            if ( IsMidi(music.c_str()) )
            {
                m_Midi = m_Engine->LoadMIDI(music.c_str());
            }
            if (!m_Music && !m_Midi)
#else
            if (!m_Music)
#endif
            {
                m_ErrorMessage = "Could not load background music '" + music + "'";
                return false;
            }

        }
    }

    // Prevent restarting of already-playing music.
    // New music? Or do we have to stop the current one?
    if (music != m_LastMusicPath)
    {
        // Seems like we have a new music to play
        if (music.length())
        {
            // start background music
            if (m_Music)
            {
                m_Music->setRepeat(true);
                m_Music->play();
            }

#if defined(WIN32) && defined(USE_MIDI)
            if (m_Midi)
            {
                m_Midi->setRepeat(true);
                m_Midi->play();
            }
#endif
        } else { // Empty music name, stop current playing music
            if (m_Music)
                m_Music->stop();
#if defined(WIN32) && defined(USE_MIDI)
            if (m_Midi)
                m_Midi->stop();
#endif
        }

        m_LastMusicPath = music;
    }

    // initialize camera
    m_Camera.x     = m_Map.GetMap().GetStartX();
    m_Camera.y     = m_Map.GetMap().GetStartY();
    m_Camera.layer = m_Map.GetMap().GetStartLayer();

    // initialize the layer script array
    m_LayerRenderers.resize(m_Map.GetMap().GetNumLayers());
    for (i = 0; i < m_LayerRenderers.size(); i++)
    {
        m_LayerRenderers[i] = NULL;
    }

    // execute entry script
    if (!CallDefaultMapScript(SCRIPT_ON_ENTER_MAP)
            || !CallMapScript(SCRIPT_ON_ENTER_MAP))
    {
        // stop background music
        m_Music = 0;

#if defined(WIN32) && defined(USE_MIDI)
        m_Midi = 0;
#endif
        // destroy edge scripts
        DestroyEdgeScripts();

        m_CurrentMap = "";
        return false;
    }
    ResetNextFrame();

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::CloseMap()
{
    unsigned int i;

    if (!DestroyMapPersons())
    {
        m_CurrentMap = "";
        return false;
    }

    DestroyTriggers();
    DestroyZones();

    // destroy any remaining delay scripts
    for (i = 0; i < m_DelayScripts.size(); i++)
    {
        m_Engine->DestroyScript(m_DelayScripts[i].script);
    }
    m_DelayScripts.clear();

    // destroy layer scripts
    for (i = 0; i < m_LayerRenderers.size(); i++)
    {
        if (m_LayerRenderers[i])
        {
            m_Engine->DestroyScript(m_LayerRenderers[i]);
        }
    }
    m_LayerRenderers.clear();

    // destroy edge scripts
    DestroyEdgeScripts();
    // execute exit script
    if (!CallDefaultMapScript(SCRIPT_ON_LEAVE_MAP) ||
        !CallMapScript(SCRIPT_ON_LEAVE_MAP))
    {
        m_CurrentMap = "";
        return false;
    }
    ResetNextFrame();

    m_CurrentMap = "";
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::Run()
{
    // initialize renderer
    m_NextFrame = (qword)GetTime() * m_FrameRate;
    int frames_skipped = 0;

    while (!m_ShouldExit)
    {
        // RENDER STEP
        if (m_ThrottleFPS)
        {  // throttle

            // if we're ahead of schedule, do the render
            qword actual_time = (qword)GetTime() * m_FrameRate;
            if (actual_time < m_NextFrame || frames_skipped >= c_MaxSkipFrames)
            {
                frames_skipped = 0;
                if (!Render())
                    return false;
                FlipScreen();

                // wait for a while (make sure we don't go faster than fps)
                while (actual_time < m_NextFrame)
                {
                    actual_time = (qword)GetTime() * m_FrameRate;
                }
            }
            else
            {
                ++frames_skipped;
            }

            // update ideal rendering time
            m_NextFrame += 1000;

        }
        else
        {              // don't throttle

            if (!Render())
                return false;
            FlipScreen();

        }
        // UPDATE STEP
        if (!UpdateWorld(true))
            return false;

        if (!ProcessInput())
            return false;

    } // end map engine loop
    m_ShouldExit = false;
    m_FrameRate = 0;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::ExecuteScript(IEngine::script script, std::string& error)
{
    bool should_exit;
    bool result = m_Engine->ExecuteScript(script, should_exit, error);

    if (should_exit)
        m_ShouldExit = true;

    return result;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::ExecuteScript(const char* script, std::string& error)
{
    IEngine::script s = m_Engine->CompileScript(script, error);
    if (s == NULL)
    {
        return false;
    }

    bool result = ExecuteScript(s, error);
    m_Engine->DestroyScript(s);
    return result;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::CompileEdgeScripts()
{
    std::string error;

    // NORTH
    if (strlen(m_Map.GetMap().GetEdgeScript(sMap::NORTH)) > 0)
    {
        m_NorthScript = m_Engine->CompileScript(m_Map.GetMap().GetEdgeScript(sMap::NORTH), error);
        if (m_NorthScript == NULL)
        {
            DestroyEdgeScripts();

            m_ErrorMessage = "Could not compile north script\n" + error;
            return false;
        }

    }
    // EAST
    if (strlen(m_Map.GetMap().GetEdgeScript(sMap::EAST)) > 0)
    {
        m_EastScript = m_Engine->CompileScript(m_Map.GetMap().GetEdgeScript(sMap::EAST), error);
        if (m_EastScript == NULL)
        {
            DestroyEdgeScripts();

            m_ErrorMessage = "Could not compile east script\n" + error;
            return false;
        }

    }
    // SOUTH
    if (strlen(m_Map.GetMap().GetEdgeScript(sMap::SOUTH)) > 0)
    {
        m_SouthScript = m_Engine->CompileScript(m_Map.GetMap().GetEdgeScript(sMap::SOUTH), error);
        if (m_SouthScript == NULL)
        {
            DestroyEdgeScripts();

            m_ErrorMessage = "Could not compile south script\n" + error;
            return false;
        }

    }
    // WEST
    if (strlen(m_Map.GetMap().GetEdgeScript(sMap::WEST)) > 0)
    {
        m_WestScript = m_Engine->CompileScript(m_Map.GetMap().GetEdgeScript(sMap::WEST), error);
        if (m_WestScript == NULL)
        {
            DestroyEdgeScripts();

            m_ErrorMessage = "Could not compile west script\n" + error;
            return false;
        }

    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

void
CMapEngine::DestroyEdgeScripts()
{
    if (m_NorthScript)
    {
        m_Engine->DestroyScript(m_NorthScript);
        m_NorthScript = NULL;
    }

    if (m_EastScript)
    {
        m_Engine->DestroyScript(m_EastScript);
        m_EastScript  = NULL;
    }

    if (m_SouthScript)
    {
        m_Engine->DestroyScript(m_SouthScript);
        m_SouthScript = NULL;
    }

    if (m_WestScript)
    {
        m_Engine->DestroyScript(m_WestScript);
        m_WestScript  = NULL;
    }

}
////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::LoadMapPersons()
{
    const int tile_width  = m_Map.GetMap().GetTileset().GetTileWidth();
    const int tile_height = m_Map.GetMap().GetTileset().GetTileHeight();

    std::string old_person;  // used down where script_create is called
    // load the persons, evaluate their startup scripts, and compile the other scripts
    for (int i = 0; i < m_Map.GetMap().GetNumEntities(); i++)
    {
        if (m_Map.GetMap().GetEntity(i).GetEntityType() == sEntity::PERSON)
        {
            sPersonEntity& person = (sPersonEntity&)m_Map.GetMap().GetEntity(i);

            // generate the person string for error messages
            std::string person_string = "name=[" + person.name + "], x=[" +
                                        itos(person.x / tile_width) + "], y=[" +
                                        itos(person.y / tile_height) + "]";

            std::string error;  // have to define locals before gotos
            Person p;
            p.description = person_string;
            if ( !CreateDefaultPerson(p, person.name.c_str(), person.spriteset.c_str(), true) )
            {
                // m_ErrorMessage = "Could not load spriteset\nPerson: " + person_string;
                goto spriteset_error;
            }

            p.x     = person.x;
            p.y     = person.y;
            p.layer = person.layer;

            // compile script_create
            if (!person.script_create.empty())
            {
                p.person_scripts[SCRIPT_ON_CREATE] = m_Engine->CompileScript(person.script_create.c_str(), error);
                if (p.person_scripts[SCRIPT_ON_CREATE] == NULL)
                {
                    m_ErrorMessage = "Could not compile OnCreate script\nPerson:" + person_string + "\n" + error;
                    goto spriteset_error;
                }

            }
            // compile script_destroy
            if (!person.script_destroy.empty())
            {
                p.person_scripts[SCRIPT_ON_DESTROY] = m_Engine->CompileScript(person.script_destroy.c_str(), error);
                if (p.person_scripts[SCRIPT_ON_DESTROY] == NULL)
                {
                    m_ErrorMessage = "Could not compile OnDestroy script\nPerson:" + person_string + "\n" + error;
                    goto spriteset_error;
                }

            }
            // compile script_activate_touch
            if (!person.script_activate_touch.empty())
            {
                p.person_scripts[SCRIPT_ON_ACTIVATE_TOUCH] = m_Engine->CompileScript(person.script_activate_touch.c_str(), error);
                if (p.person_scripts[SCRIPT_ON_ACTIVATE_TOUCH] == NULL)
                {
                    m_ErrorMessage = "Could not compile OnActivate (touch) script\nPerson:" + person_string + "\n" + error;
                    goto spriteset_error;
                }

            }
            // compile script_activate_talk
            if (!person.script_activate_talk.empty())
            {
                p.person_scripts[SCRIPT_ON_ACTIVATE_TALK] = m_Engine->CompileScript(person.script_activate_talk.c_str(), error);
                if (p.person_scripts[SCRIPT_ON_ACTIVATE_TALK] == NULL)
                {
                    m_ErrorMessage = "Could not compile OnActivate (talk) script\nPerson:" + person_string + "\n" + error;
                    goto spriteset_error;
                }

            }
            // compile script_command_generator
            if (!person.script_generate_commands.empty())
            {
                p.person_scripts[SCRIPT_COMMAND_GENERATOR] = m_Engine->CompileScript(person.script_generate_commands.c_str(), error);
                if (p.person_scripts[SCRIPT_COMMAND_GENERATOR] == NULL)
                {
                    m_ErrorMessage = "Could not compile OnGenerateCommands script\nPerson:" + person_string + "\n" + error;
                    goto spriteset_error;
                }

            }
            // add it to the list
            m_Persons.push_back(p);

            // old_person defined earlier (before goto)
            /*std::string*/
            old_person = m_CurrentPerson;
            m_CurrentPerson = p.name;

            // execute default script_create
            if (m_default_person_scripts[SCRIPT_ON_CREATE] != NULL)
            {
                if (!m_Engine->IsScriptBeingUsed(m_default_person_scripts[SCRIPT_ON_CREATE]))
                {
                    const std::string person_name = m_CurrentPerson;
                    if (!ExecuteScript(m_default_person_scripts[SCRIPT_ON_CREATE], error))
                    {
                        m_ErrorMessage = "Could not execute default OnCreate script\nPerson:" + person_string + "\n" + error;
                        m_Persons.erase(m_Persons.end() - 1);
                        goto spriteset_error;
                    }
                    // the script may have destroyed the person, so check to see that the person still exists
                    if (FindPerson(person_name.c_str()) != i)
                    {
                        m_CurrentPerson = old_person;
                        continue;
                    }
                }
            }
            // execute script_create
            if (p.person_scripts[SCRIPT_ON_CREATE] != NULL && !ExecuteScript(p.person_scripts[SCRIPT_ON_CREATE], error))
            {
                m_ErrorMessage = "Could not execute OnCreate script\nPerson:" + person_string + "\n" + error;
                m_Persons.erase(m_Persons.end() - 1);

                goto spriteset_error;
            }

            m_CurrentPerson = old_person;
            continue;

spriteset_error:
            if (p.spriteset)
            {
                m_Engine->DestroySpriteset(p.spriteset);
            }

            DestroyPersonScripts(p);
            DestroyMapPersons();
            return false;
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::DestroyMapPersons()
{
    // destroy map person entities
    for (int i = 0; i < int(m_Persons.size()); i++)
    {
        if (m_Persons[i].destroy_with_map)
        {
            int j;

            // detach camera if necessary
            if (i == m_CameraPerson)
            {
                m_IsCameraAttached = false;
            }
            else if (m_CameraPerson > i)
            {
                m_CameraPerson--;
            }

            // detach input if necessary
            if (i == m_InputPerson)
            {
                m_IsInputAttached = false;
            }
            else if (m_InputPerson > i)
            {
                m_InputPerson--;
            }

            for (j = 0; j < int(m_InputPersons.size()); j++)
            {
                if (m_InputPersons[j] == i)
                {
                    m_Persons[m_InputPersons[j]].player_index = -1;
                    m_InputPersons.erase(m_InputPersons.begin() + j);
                    j--;
                }
                else if (m_InputPersons[j] > i)
                {
                    m_InputPersons[j]--;
                }
            }
            // update all leader indices
            for (j = 0; j < int(m_Persons.size()); j++)
            {
                if (i != j)
                {
                    if (m_Persons[j].leader > i)
                    {
                        m_Persons[j].leader--;
                    }
                    else if (m_Persons[j].leader == i)
                    {
                        m_Persons[j].leader = -1;
                        m_Persons[j].follow_state_queue.resize(0);

                    }
                }
            }

            // destroy the entity scripts/spriteset/etc.
            if (!DestroyPersonStructure(m_Persons[i]))
            {
                return false;
            }

            // remove the entity from the list
            m_Persons.erase(m_Persons.begin() + i);
            i--;
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

void
CMapEngine::DestroyPersonScripts(Person& p)
{
    for (int script_index = 0; script_index < NUM_PERSON_SCRIPTS; script_index++)
    {
        if (p.person_scripts[script_index] != NULL)
        {
            m_Engine->DestroyScript(p.person_scripts[script_index]);
            p.person_scripts[script_index] = NULL;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::DestroyPersonStructure(Person& p)
{
    // execute OnDestroy scripts (if it exists)
    if (p.person_scripts[SCRIPT_ON_DESTROY])
    {
        // set current person
        std::string old_person = m_CurrentPerson;
        m_CurrentPerson = p.name;

        std::string error;
        if (!ExecuteScript(p.person_scripts[SCRIPT_ON_DESTROY], error))
        {
            m_ErrorMessage = "Could not execute OnDestroy script\nPerson:";
            m_ErrorMessage += p.description + "\n" + error;
            return false;
        }

        // restore current person
        m_CurrentPerson = old_person;
    }

    m_Engine->DestroySpriteset(p.spriteset);
    DestroyPersonScripts(p);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::LoadTriggers()
{
    const int tile_width  = m_Map.GetMap().GetTileset().GetTileWidth();
    const int tile_height = m_Map.GetMap().GetTileset().GetTileHeight();

    // load the triggers and compile the trigger scripts
    for (int i = 0; i < m_Map.GetMap().GetNumEntities(); i++)
    {
        if (m_Map.GetMap().GetEntity(i).GetEntityType() == sEntity::TRIGGER)
        {
            sTriggerEntity& trigger = (sTriggerEntity&)m_Map.GetMap().GetEntity(i);

            Trigger t;
            t.x      = trigger.x;
            t.y      = trigger.y;
            t.layer  = trigger.layer;
            std::string error;
            t.script = m_Engine->CompileScript(trigger.script.c_str(), error);

            if (t.script == NULL)
            {
                // destroy scripts that have been created so far
                for (unsigned int j = 0; j < m_Triggers.size(); j++)
                {
                    m_Engine->DestroyScript(m_Triggers[i].script);
                }

                // build error message and return
                std::ostringstream os;
                os << "Could not compile trigger ("
                << t.x / tile_width
                << ", "
                << t.y / tile_height
                << ")\n";
                m_ErrorMessage = os.str() + error;
                return false;
            }

            m_Triggers.push_back(t);
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

void
CMapEngine::DestroyTriggers()
{
    // destroy trigger scripts
    for (unsigned int i = 0; i < m_Triggers.size(); i++)
    {
        m_Engine->DestroyScript(m_Triggers[i].script);
    }
    m_Triggers.clear();
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::LoadZones()
{
    for (int i = 0; i < m_Map.GetMap().GetNumZones(); i++)
    {
        sMap::sZone zone = m_Map.GetMap().GetZone(i);
        std::string error;
        Zone z;

        z.x1 = zone.x1;
        z.y1 = zone.y1;
        z.x2 = zone.x2;
        z.y2 = zone.y2;
        z.layer = zone.layer;
        z.reactivate_in_num_steps = zone.reactivate_in_num_steps;

        z.current_step = 0;
        z.script = m_Engine->CompileScript(zone.script.c_str(), error);
        if (z.script == NULL)
        {
            // destroy scripts that have been created so far
            DestroyZones();

            // build error message and return
            std::ostringstream os;
            os << "Could not compile zone ("
            << z.x1
            << ", "
            << z.y1
            << ") -> ("
            << z.x2
            << ", "
            << z.y2
            << ")\n";
            m_ErrorMessage = os.str() + error;

            return false;
        }

        m_Zones.push_back(z);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

void
CMapEngine::DestroyZones()
{
    for (unsigned int i = 0; i < m_Zones.size(); i++)
    {
        m_Engine->DestroyScript(m_Zones[i].script);

        m_Zones[i].script = NULL;
    }
    m_Zones.clear();
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::Render()
{
//  const int cx = GetScreenWidth()  / 2;
//  const int cy = GetScreenHeight() / 2;

    // for doing camera correction (with sprites and doodads and whatnot)
    int offset_x;
    int offset_y;

    // render all layers
    for (int i = 0; i < m_Map.GetMap().GetNumLayers(); i++)
    {
        // IF REFLECTIVE
        if (m_Map.GetMap().GetLayer(i).IsReflective())
        {
            // solid render
            m_Map.RenderLayer(i, true, m_Camera.x, m_Camera.y, offset_x, offset_y);

            // render upside-down sprites
            if (!RenderEntities(i, true, offset_x, offset_y))
            {
                return false;
            }

            // do normal render
            m_Map.RenderLayer(i, false, m_Camera.x, m_Camera.y, offset_x, offset_y);

            // draw person entities
            if (!RenderEntities(i, false, offset_x, offset_y))
            {
                return false;
            }

        }
        else
        {  // IF NOT REFLECTIVE

            m_Map.RenderLayer(i, false, m_Camera.x, m_Camera.y, offset_x, offset_y);
            // draw person entities
            if (!RenderEntities(i, false, offset_x, offset_y))
            {
                return false;
            }

        } // end if reflective
        // execute layer renderer
        if (m_LayerRenderers[i])
        {
            std::string error;
            if (!ExecuteScript(m_LayerRenderers[i], error))
            {
                m_ErrorMessage = "Could not execute layer renderer " + itos(i) + "\n" + error;
                return false;
            }
        }

    } // end for all layers

    //if (!(m_CurrentColorMask.red == 255 && m_CurrentColorMask.green == 255 && m_CurrentColorMask.blue == 255 && m_CurrentColorMask.alpha == 255))
    if ( (m_CurrentColorMask.red-255)|(m_CurrentColorMask.green-255)|(m_CurrentColorMask.blue-255)|(m_CurrentColorMask.alpha-255) )
        ApplyColorMask(m_CurrentColorMask);

    // render script
    if (m_RenderScript && !m_Engine->IsScriptBeingUsed(m_RenderScript))
    {
        std::string error;

        if (!ExecuteScript(m_RenderScript, error))
        {
            m_ErrorMessage = "Could not execute render script\n" + error;

            return false;
        }

    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::RenderEntities(int layer, bool flipped, int offset_x, int offset_y)
{
    CRenderSort rs;

    const int tile_width = m_Map.GetMap().GetTileset().GetTileWidth();
    const int tile_height = m_Map.GetMap().GetTileset().GetTileHeight();

    // Note: zooming is ignored below. Should it be ignored here too?
    double zoom_factor_x = m_Map.GetLayerScaleFactorX(layer);
    double zoom_factor_y = m_Map.GetLayerScaleFactorY(layer);
    int layer_pixel_width = int(m_Map.GetMap().GetLayer(layer).GetWidth() * tile_width * zoom_factor_x);
    int layer_pixel_height = int(m_Map.GetMap().GetLayer(layer).GetHeight() * tile_height * zoom_factor_y);

    // add non-map-specific person entities
    for (unsigned int i = 0; i < m_Persons.size(); i++)
    {
        if (m_Persons[i].layer == layer && m_Persons[i].is_visible)
        {
            Person& p = m_Persons[i];
            const sSpriteset& ss = p.spriteset->GetSpriteset();

            IMAGE image = (flipped
                           ? p.spriteset->GetFlipImage(ss.GetFrameIndex(p.direction, p.frame))
                           : p.spriteset->GetImage(ss.GetFrameIndex(p.direction, p.frame))
                          );

            // calculate distance from upper-left corner of image to center of base
            int base_x = (p.base_x1 + p.base_x2) / 2;
            int base_y = (p.base_y1 + p.base_y2) / 2;
            //int base_x = (((double)p.base_x1 * p.scale_x) + ((double)p.base_x2 * p.scale_x)) / 2;
            //int base_y = (((double)p.base_y1 * p.scale_y) + ((double)p.base_y2 * p.scale_y)) / 2;

            //int draw_x = int((zoom_factor_x * p.x) - base_x - m_Camera.x - offset_x + (GetScreenWidth()  / 2));
            //int draw_y = int((zoom_factor_y * p.y) - base_y - m_Camera.y - offset_y + (GetScreenHeight() / 2));
            int draw_x = int(p.offset_x + p.x - base_x - m_Camera.x - offset_x + (GetScreenWidth()  / 2));
            int draw_y = int(p.offset_y + p.y - base_y - m_Camera.y - offset_y + (GetScreenHeight() / 2));

            if (flipped)
                draw_y += base_y;

            // Render person multiple times for repeating maps
            if (m_Map.GetMap().IsRepeating())
            {
                while (draw_x + ss.GetFrameWidth() > 0)
                    draw_x -= layer_pixel_width;
                draw_x += layer_pixel_width;
                int start_draw_x = draw_x;

                while (draw_y + ss.GetFrameHeight() > 0)
                    draw_y -= layer_pixel_height;
                draw_y += layer_pixel_height;

                for (; draw_y < GetScreenHeight() + ss.GetFrameHeight(); draw_y += layer_pixel_height)
                {
                    int sort_y = int(draw_y);
                    for (draw_x = start_draw_x; draw_x < GetScreenWidth() + ss.GetFrameWidth(); draw_x += layer_pixel_width)
                        rs.AddObject(draw_x, draw_y, sort_y, p.width, p.height, p.is_angled, p.angle, image, p.mask);
                }
            }
            else
            {
                int sort_y = int(p.y);
                rs.AddObject(draw_x, draw_y, sort_y, p.width, p.height, p.is_angled, p.angle, image, p.mask);
            }
        }
    }

    rs.DrawObjects();
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::UpdateWorld(bool input_valid)
{
    m_Map.UpdateMap();

    if (!UpdatePersons())
        return false;

    if (input_valid)
    {
        //for (int i = 0; i < int(m_InputPersons.size()); i++)
        for (int i = int(m_InputPersons.size())-1 ; i>=0 ; --i)
        {
            if (!UpdateTriggers(m_InputPersons[i]))
                return false;

        }
    }

    if (!UpdateColorMasks())

        return false;

    if (!UpdateDelayScripts())
        return false;

    if (input_valid)
    {
        if (!UpdateEdgeScripts())
            return false;
    }

    // update the camera
    if (m_IsCameraAttached)
    {
        m_Camera.x     = int(m_Persons[m_CameraPerson].x);
        m_Camera.y     = int(m_Persons[m_CameraPerson].y);
        m_Camera.layer = m_Persons[m_CameraPerson].layer;
    }

    // call update script
    if (m_UpdateScript && !m_Engine->IsScriptBeingUsed(m_UpdateScript))
    {
        std::string error;

        if (!ExecuteScript(m_UpdateScript, error))
        {
            m_ErrorMessage = "Could not execute update script\n" + error;

            return false;
        }

        // if we took more than a second to run the update script, reset the timer
        if (qword(GetTime()) * m_FrameRate > m_NextFrame)
            ResetNextFrame();
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::UpdatePersons()
{
    // if any of the persons are activated, disable talk activation
    bool anything_activated = false;

    // for each person...
    // for (int i = int(m_Persons.size())-1 ; i>=0 ; --i) // Sometimes, it starts at i=12, then suddenly .size()==2 and its out of range!
    for (int i = 0; i < int(m_Persons.size()) ; ++i)
    {
        bool activated;
        if (!UpdatePerson(i, activated))
            return false;

        anything_activated |= activated;
    }
    m_TalkActivationAllowed = !anything_activated;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::UpdatePerson(int person_index, bool& activated)
{
    Person* p = &m_Persons[person_index];
    activated = false;
	p->obs_person = -1;

    // Reset Lithonite directional vectors
	p->dx = 0;
    p->dy = 0;

	// if this person has a leader, skip it
    if (p->leader != -1)
    {
        // revert back to the first frame if reversion has been set and enough updates have passed
        if (p->stepping_frame_revert > 0)
        {
            if (p->stepping_frame_revert_count++ >= p->stepping_frame_revert)
            {
                if (p->frame != 0)
                {
                    p->frame = 0;
                    p->next_frame_switch = p->spriteset->GetSpriteset().GetFrameDelay(p->direction, p->frame);
                }
            }
        }
        return true;
    }

    // store current position (before command_generator moves us)
    double x = p->x;
    double y = p->y;

    // we haven't called an activation function yet
    bool activation_called = false;
    bool should_animate    = false;
    bool processing = true;
    bool force_stop = false;

    while (processing && !force_stop)
    {
        // if this entity has no commands, execute generator
        if (p->commands.empty())
        {
            if (p->person_scripts[SCRIPT_COMMAND_GENERATOR])
            {
                // set current person
                std::string old_person = m_CurrentPerson;
                m_CurrentPerson = p->name;

                std::string person_name = m_CurrentPerson;
                std::string error;
                if (!ExecuteScript(p->person_scripts[SCRIPT_COMMAND_GENERATOR], error))
                {
                    m_ErrorMessage = "Error executing person command generator\nPerson:"
                            + p->description +
                              "\nError:"
                            + error;
                    return false;
                }

                // the script may have destroyed the person, so check to see that the person still exists
                if (FindPerson(person_name.c_str()) != person_index)
                    return true;
                m_CurrentPerson = old_person;
                // update the person pointer
                p = &m_Persons[person_index];
            }

            // if all of the commands are immediate, force a stop after this command
            force_stop = true;

            std::deque<Person::Command>::iterator k;
            for (k = p->commands.begin(); k != p->commands.end(); k++)
            {
                if (k->immediate == false)
                {
                    force_stop = false;
                    break;
                }
            }
            // if there are no commands, stop
            if (p->commands.empty())
                break;
        } // end (if command queue is empty)

        // read the top command
        Person::Command c = p->commands.front();
        p->commands.pop_front();

        // store position in case the obstruction check needs to put it back
        double old_x = p->x;
        double old_y = p->y;

        std::string error;
        switch (c.command)
        {
        case COMMAND_WAIT:
            break;

        case COMMAND_ANIMATE:
            should_animate = true;
            break;
        case COMMAND_FACE_NORTH:
            p->direction = "north";
            break;
        case COMMAND_FACE_NORTHEAST:
            p->direction = "northeast";
            break;
        case COMMAND_FACE_EAST:
            p->direction = "east";
            break;
        case COMMAND_FACE_SOUTHEAST:
            p->direction = "southeast";
            break;
        case COMMAND_FACE_SOUTH:
            p->direction = "south";
            break;
        case COMMAND_FACE_SOUTHWEST:
            p->direction = "southwest";
            break;
        case COMMAND_FACE_WEST:
            p->direction = "west";
            break;
        case COMMAND_FACE_NORTHWEST:
            p->direction = "northwest";
            break;
        case COMMAND_MOVE_NORTH:
            p->y -= p->speed_y;
            p->dy = -1;
            break;
        case COMMAND_MOVE_EAST:
            p->x += p->speed_x;
            p->dx = 1;
            break;
        case COMMAND_MOVE_SOUTH:
            p->y += p->speed_y;
            p->dy = 1;
            break;
        case COMMAND_MOVE_WEST:
            p->x -= p->speed_x;
            p->dx = -1;
            break;
        case COMMAND_DO_SCRIPT:
            {
                const std::string old_person = m_CurrentPerson;
                m_CurrentPerson = m_Persons[person_index].name;
                const std::string person_name = m_Persons[person_index].name;
                if (!ExecuteScript(c.script.c_str(), error) || !error.empty())
                {
                    m_ErrorMessage = "Could not execute queued script\nPerson:"
                            + p->description + "\nError:" + error;
                return false;
                }
                m_CurrentPerson = old_person;
                if (qword(GetTime()) * m_FrameRate > m_NextFrame)
                {
                    ResetNextFrame();
                }
                // the script may have destroyed the person, so check to see that the person still exists
                if (FindPerson(person_name.c_str()) != person_index)
                {
                    return true;
                }
                p = &m_Persons[person_index];
                break;
            }
        }

		if(p->dx || p->dy){
			p->hx = p->dx; 
			p->hy = p->dy;
		}

        // todo: this sucks, fix me
        // confine the input person within the map if the map is repeating
        if (person_index == m_InputPerson)
        {
            if (m_Map.GetMap().IsRepeating())
            {
                int layer_width =  m_Map.GetMap().GetLayer(p->layer).GetWidth()  * m_Map.GetMap().GetTileset().GetTileWidth();
                int layer_height = m_Map.GetMap().GetLayer(p->layer).GetHeight() * m_Map.GetMap().GetTileset().GetTileHeight();

                while (p->x < 0)
                    p->x += layer_width;

                while (p->x >= layer_width)
                    p->x -= layer_width;

                while (p->y < 0)
                    p->y += layer_height;

                while (p->y >= layer_height)
                    p->y -= layer_height;
            }
        }

        // make sure frame is valid
        if (p->spriteset->GetSpriteset().GetNumFrames(p->direction))
            p->frame %= p->spriteset->GetSpriteset().GetNumFrames(p->direction);

        else
            p->frame = 0;
        // check for obstructions
        int obs_person = -1;
		if (IsObstructed(person_index, int(p->x), int(p->y), obs_person))
        {
            p->x = old_x;
            p->y = old_y;
            p->obs_person = obs_person ; // obs_person is reused for talkactivation, so it needs to be temporal
		}
	
        // CHECK FOR ENTITY ACTIVATION
        // if we're processing the input target
        if (person_index == m_InputPerson)
        {
            // and if a person caused the obstruction
            // and if the activation function has not already
            if (obs_person != -1)
            {
                if (m_TouchActivationAllowed)
                {
                    IEngine::script script = m_Persons[obs_person].person_scripts[SCRIPT_ON_ACTIVATE_TOUCH];
                    // execute the script!
                    if (script && !m_Engine->IsScriptBeingUsed(script))
                    {
                        const std::string old_person = m_CurrentPerson;
                        m_CurrentPerson = m_Persons[obs_person].name;

                        const std::string person_name = m_Persons[person_index].name;
                        std::string error;
                        if (!ExecuteScript(script, error) || !error.empty())
                        {
                            m_ErrorMessage = "Error executing person activation (touch) script\n"
                                             "Person:"
                                             + p->description +
                                             "\nError:" + error;

                            return false;
                        }

                        m_CurrentPerson = old_person;
                        ResetNextFrame();

                        // the script may have destroyed the person, so check to see that the person still exists
                        if (FindPerson(person_name.c_str()) != person_index)
                        {
                            return true;
                        }
                        p = &m_Persons[person_index];
                    }
                }

                activation_called = true;
            }
        }

        processing = c.immediate;
    }

    // if an activation function was called, activation should not be allowed
    if (person_index == m_InputPerson)
        m_TouchActivationAllowed = !activation_called;

    // if position has changed, update frame index and state of followers
    if (x != p->x || y != p->y || should_animate)
    {
        p->stepping_frame_revert_count = 0;

        // frame index
        if (--p->next_frame_switch <= 0)
        {
            const int num_frames = p->spriteset->GetSpriteset().GetNumFrames(p->direction);
            if (num_frames > 0)
                p->frame = (p->frame + 1) % num_frames;
            p->next_frame_switch = p->spriteset->GetSpriteset().GetFrameDelay(p->direction, p->frame);
        }

        // followers
        for (int j = 0; j < int(m_Persons.size()); j++)
        {
            if (person_index != j)
            {
                if (m_Persons[j].leader == person_index)
                {  // if the current entity is a leader...
                    UpdateFollower(j);
                }
            }
        }
    }
    else
    {
        // revert back to the first frame if reversion has been set and enough updates have passed
        if (p->stepping_frame_revert > 0)
        {
            if (p->stepping_frame_revert_count++ >= p->stepping_frame_revert)
            {
                if (p->frame != 0)
                {
                    p->frame = 0;
                    p->next_frame_switch = p->spriteset->GetSpriteset().GetFrameDelay(p->direction, p->frame);
                }
            }
        }
    }

    // test if talk activation script should be called
    if (m_InputPerson == person_index)
    {
        // if the activation key is pressed
        if ( (m_Keys[m_TalkActivationKey] && !IsKeyBound(m_TalkActivationKey))
                || (GetNumJoysticks() > 0 && IsJoystickButtonPressed(0, m_JoystickTalkButton)) && !IsJoystickButtonBound(0, m_JoystickTalkButton))
        {
            int talk_x = int(m_Persons[m_InputPerson].x);
            int talk_y = int(m_Persons[m_InputPerson].y);

            int obs_person = FindTalkingPerson(person_index, talk_x, talk_y);
                if (obs_person != -1)
                {
                    activated = true;
                    if (m_TalkActivationAllowed)
                    {
                        IEngine::script s = m_Persons[obs_person].person_scripts[SCRIPT_ON_ACTIVATE_TALK];
                        if (s && !m_Engine->IsScriptBeingUsed(s))
                        {
                            const std::string old_person = m_CurrentPerson;
                            m_CurrentPerson = m_Persons[obs_person].name;

                            const std::string person_name = m_Persons[person_index].name;
                            std::string error;
                            if (!ExecuteScript(s, error))
                            {
                                m_ErrorMessage = "Error executing person activation (talk) script\n"
                                                 "Person:" + p->description +
                                                 "\nError:" + error;

                                return false;
                            }

                            m_CurrentPerson = old_person;
                            ResetNextFrame();

                            // the script may have destroyed the person, so check to see that the person still exists
                            if (FindPerson(person_name.c_str()) != person_index)
                            {
                                return true;
                            }
                            p = &m_Persons[person_index];
                        }
                    }
                }

        }
    }

	// Update the zones if any of the 4 input persons are walking over it
    for (int j = int(m_InputPersons.size())-1;j>=0; --j)
    {
        if (m_InputPersons[j] == person_index)
        {
            int px = int(fabs(x - p->x));
            int py = int(fabs(y - p->y));
            int s;

            if (px > py) s = px;
            else s = py;

            while (s-- > 0)
            {
                if (!UpdateZones(person_index))
                {
                    return false;
                }
            }

            break;
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::UpdateFollower(int person_index)
{
    int i;
    Person& p = m_Persons[person_index];

    p.x         = p.follow_state_queue[0].x;
    p.y         = p.follow_state_queue[0].y;
    p.layer     = p.follow_state_queue[0].layer;
    p.direction = p.follow_state_queue[0].direction;

    // make sure frame is valid
    p.frame %= p.spriteset->GetSpriteset().GetNumFrames(p.direction);
    p.stepping_frame_revert_count = 0;

    // update the follow state
    for (i = 0; i < (int)p.follow_state_queue.size() - 1; i++)
    {
        p.follow_state_queue[i] = p.follow_state_queue[i + 1];
    }

    int last = p.follow_state_queue.size() - 1;
    p.follow_state_queue[last].x         = m_Persons[p.leader].x;
    p.follow_state_queue[last].y         = m_Persons[p.leader].y;
    p.follow_state_queue[last].layer     = m_Persons[p.leader].layer;
    p.follow_state_queue[last].direction = m_Persons[p.leader].direction;

    // frame index
    if (--p.next_frame_switch <= 0)
    {
        const int num_frames = p.spriteset->GetSpriteset().GetNumFrames(p.direction);
        p.frame = (p.frame + 1) % num_frames;
        p.next_frame_switch = p.spriteset->GetSpriteset().GetFrameDelay(p.direction, p.frame);
    }

    // now update any followers of this one
    for (i = 0; i < int(m_Persons.size()); i++)
    {
        if (i != person_index)
        {
            if (m_Persons[i].leader == person_index)
            {
                UpdateFollower(i);
            }
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

int
CMapEngine::FindTrigger(int location_x, int location_y, int layer)
{
    sTileset& tileset = m_Map.GetMap().GetTileset();
    const int tile_width  = tileset.GetTileWidth();
    const int tile_height = tileset.GetTileHeight();

    // this doesn't seem to use layers, but I'll include the layer parameter just incase
    int trigger_index = -1;
    for (unsigned int i = 0; i < m_Triggers.size(); i++)
    {
        int dx = m_Triggers[i].x - location_x;
        int dy = m_Triggers[i].y - location_y;

        if (dx < tile_width / 2 && dx >= -tile_width / 2 &&
                dy < tile_height / 2 && dy >= -tile_height / 2)
        {
            trigger_index = i;
            break;
        }
    }
    return trigger_index;
}

///////////////////////////////////////////////////////////////////////////////

/*
bool
CMapEngine::GetNumTriggers(int& triggers)
{
  if(!m_IsRunning) {
    m_ErrorMessage = "GetNumTriggers called while map engine isn't running.";
    return false;
  }
  triggers = int(m_Triggers.size());
  return true;
}
bool
CMapEngine::GetCurrentTrigger(int& trigger_index) {
  if(!m_IsRunning) {
    m_ErrorMessage = "GetCurrentTrigger called while map engine isn't running.";
    return false;
  }
  if (!m_IsInputAttached) {
    m_ErrorMessage = "Input not attached!";
    return true;
  }
  // convenience
  int location_x = int(m_Persons[m_InputPerson].x);
  int location_y = int(m_Persons[m_InputPerson].y);
  int location_layer = m_Persons[m_InputPerson].layer;
  // check to see which trigger we're on
  trigger_index = FindTrigger(location_x, location_y, location_layer);
  return true;
}
bool
CMapEngine::GetTriggerX(int trigger, int& x) {
  if(!m_IsRunning) {
    m_ErrorMessage = "GetTriggerX called while map engine isn't running.";
    return false;
  }
  if (trigger < 0 || trigger > int(m_Triggers.size())) {
    m_ErrorMessage = "Invalid trigger index: " + itos(trigger);
    return false;
  }
  x = m_Triggers[trigger].x;
  return true;
}
bool
CMapEngine::GetTriggerY(int trigger, int& y) {
  if(!m_IsRunning) {
    m_ErrorMessage = "GetTriggerY called while map engine isn't running.";
    return false;
  }
  if (trigger < 0 || trigger > int(m_Triggers.size())) {
    m_ErrorMessage = "Invalid trigger index: " + itos(trigger);
    return false;
  }
  y = m_Triggers[trigger].y;
  return true;
}
*/

///////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::IsTriggerAt(int location_x, int location_y, int layer, bool& found)
{
    found = false;
    if ( IsInvalidLayerError(layer, "IsTriggerAt()") )
        return false;

    int trigger_index = FindTrigger(location_x, location_y, layer);
    found = (trigger_index != -1);
    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::ExecuteTriggerScript(int trigger_index)
{

    if (trigger_index < 0 || trigger_index >= int(m_Triggers.size()))
    {
        m_ErrorMessage = "Invalid trigger index";
        return false;
    }

    // execute the trigger code
    IEngine::script script = m_Triggers[trigger_index].script;
    std::string error;

    // save trigger coordinates in case the map will change and an error will occur
    int trigger_x = m_Triggers[trigger_index].x;
    int trigger_y = m_Triggers[trigger_index].y;

    if (script && !ExecuteScript(script, error))
    {

        sTileset& tileset = m_Map.GetMap().GetTileset();
        const int tile_width  = tileset.GetTileWidth();
        const int tile_height = tileset.GetTileHeight();

        std::ostringstream os;

        os << "Could not execute trigger (";
        os << trigger_x / tile_width;
        os << ", ";
        os << trigger_y / tile_height;
        os << ")\n";

        m_ErrorMessage = os.str() + error;

        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::UpdateTriggers(int person_index)
{
    // can't process triggers if we don't have an input target
    //if (!m_IsInputAttached)

    if ( m_InputPersons.size() == 0)
    {
        return true;
    }

    // convenience
    int location_x = int(m_Persons[person_index].x);
    int location_y = int(m_Persons[person_index].y);
    int location_layer = m_Persons[person_index].layer;

    // check to see which trigger we're on
    int trigger_index = FindTrigger(location_x, location_y, location_layer);

    // if we're moving from one trigger to a new one (or off of one)
    if (m_Persons[person_index].on_trigger && trigger_index != m_Persons[person_index].last_trigger)
    {
        if (trigger_index != -1)
        {
            if (!ExecuteTriggerScript(trigger_index))
            {
                return false;
            }
            ResetNextFrame();
        }
        else
        {
            m_Persons[person_index].on_trigger = false;
        }
        m_Persons[person_index].last_trigger = trigger_index;

        // if we're moving on to another trigger
    }
    else if (!m_Persons[person_index].on_trigger && trigger_index != -1)
    {
        if (!ExecuteTriggerScript(trigger_index))
        {
            return false;
        }
        ResetNextFrame();

        m_Persons[person_index].on_trigger = true;
        m_Persons[person_index].last_trigger = trigger_index;

    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::IsPointWithinZone(int location_x, int location_y, int location_layer, int zone_index)
{
    if (zone_index < 0 || zone_index >= int(m_Zones.size()))
        return false;

    Zone& z = m_Zones[zone_index];
    return (location_layer == z.layer &&
            location_x >= z.x1 &&
            location_y >= z.y1 &&
            location_x < z.x2 &&
            location_y < z.y2 );
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::IsPersonInsideZone(int person_index, int zone_index)
{
    if (person_index < 0 || person_index >= int(m_Persons.size()))
        return false;

    Person& p = m_Persons[person_index];
	return IsPointWithinZone((int)p.x, (int)p.y, p.layer, zone_index);
}

///////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::ExecuteZoneScript(int zone_index)
{
    if (!m_IsRunning)
    {
        m_ErrorMessage = "ExecuteZoneScript called while map engine isn't running.";
        return false;
    }

    if (zone_index < 0 || zone_index >= int(m_Zones.size()))
    {
        m_ErrorMessage = "Invalid zone index: " + itos(zone_index);
        return false;
    }

    Zone& z = m_Zones[zone_index];
    m_CurrentZone = zone_index;

    // execute the zone code
    IEngine::script script = z.script;

    if (script)
    {
        std::string error;
        bool execute_script = ExecuteScript(script, error);

        m_CurrentZone = -1;
        if (!execute_script)
        {
            std::ostringstream os;
            os << "Could not execute zone ("
            << z.x1
            << ", "
            << z.y1
            << ") -> ("
            << z.x2
            << ", "
            << z.y2
            << ")\n";
            m_ErrorMessage = os.str() + error;

            return false;
        }

    }
    // if we took more than a second to run the zone script, reset the timer
    if (qword(GetTime()) * m_FrameRate > m_NextFrame)
    {
        ResetNextFrame();
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::UpdateZones(int person_index)
{
    // can't process zones if we don't have an input target
    //if (!m_IsInputAttached)

    if (m_InputPersons.size() == 0)
    {
        return true;
    }

    for (unsigned int i = 0; i < m_Zones.size(); i++)
    {
        // check if the person is inside the zone
        if (IsPersonInsideZone(person_index, i))
        {
            std::string current_map = m_CurrentMap;
            Zone& z = m_Zones[i];
            z.current_step--;
            if (z.current_step < 0)
            {
                z.current_step = z.reactivate_in_num_steps - 1;

                if (!ExecuteZoneScript(i))
                    return false;

                if (current_map != m_CurrentMap)
                {
                    return true;
                }
            }
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::UpdateColorMasks()
{
    // update color masks
    if (m_FramesLeft > 0 && m_NumFrames > 0)
    {
        m_CurrentColorMask.red   = (m_DestinationColorMask.red   * (m_NumFrames - m_FramesLeft) + m_PreviousColorMask.red   * m_FramesLeft) / m_NumFrames;
        m_CurrentColorMask.green = (m_DestinationColorMask.green * (m_NumFrames - m_FramesLeft) + m_PreviousColorMask.green * m_FramesLeft) / m_NumFrames;
        m_CurrentColorMask.blue  = (m_DestinationColorMask.blue  * (m_NumFrames - m_FramesLeft) + m_PreviousColorMask.blue  * m_FramesLeft) / m_NumFrames;
        m_CurrentColorMask.alpha = (m_DestinationColorMask.alpha * (m_NumFrames - m_FramesLeft) + m_PreviousColorMask.alpha * m_FramesLeft) / m_NumFrames;

        m_FramesLeft--;
    }
    else
    {
        m_CurrentColorMask = m_DestinationColorMask;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::UpdateDelayScripts()
{
	if(m_DelayScripts.size() == 0)
		return true;
    // update delay scripts
    for (unsigned int i = 0; i < m_DelayScripts.size(); ++i)
    {
        if (--m_DelayScripts[i].frames_left < 0)
        {
            IEngine::script script = m_DelayScripts[i].script;
            // the script may cause a CloseMap call, so remove the script from the array now
            m_DelayScripts.erase(m_DelayScripts.begin() + i);
            i--;

            std::string error;
            if (!ExecuteScript(script, error) || !error.empty())
            {
                m_Engine->DestroyScript(script);

                m_ErrorMessage = "Could not execute delay script\n" + error;
                return false;
            }

            m_Engine->DestroyScript(script);
            // if we took more than a second to run the delay script, reset the timer
            if (qword(GetTime()) * m_FrameRate > m_NextFrame)
            {
                ResetNextFrame();
            }
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::UpdateEdgeScripts()
{
    const sMap& map = m_Map.GetMap();
    const int tile_width   = map.GetTileset().GetTileWidth();
    const int tile_height  = map.GetTileset().GetTileHeight();
    const int layer_width  = map.GetLayer(m_Camera.layer).GetWidth();
    const int layer_height = map.GetLayer(m_Camera.layer).GetHeight();

    if (m_Camera.x < 0)
    {                                 // west
        if (!CallDefaultMapScript(SCRIPT_ON_LEAVE_MAP_WEST)
                || !CallMapScript(SCRIPT_ON_LEAVE_MAP_WEST))
        {
            return false;
        }
    }
    else if (m_Camera.x > tile_width * layer_width)
    {   // east
        if (!CallDefaultMapScript(SCRIPT_ON_LEAVE_MAP_EAST)
                || !CallMapScript(SCRIPT_ON_LEAVE_MAP_EAST))
        {
            return false;
        }
    }
    if (m_Camera.y < 0)
    { // north
        if (!CallDefaultMapScript(SCRIPT_ON_LEAVE_MAP_NORTH)
                || !CallMapScript(SCRIPT_ON_LEAVE_MAP_NORTH))
        {
            return false;
        }
    }
    else if (m_Camera.y > tile_height * layer_height)
    { // south
        if (!CallDefaultMapScript(SCRIPT_ON_LEAVE_MAP_SOUTH)
                || !CallMapScript(SCRIPT_ON_LEAVE_MAP_SOUTH))
        {
            return false;
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

static inline int __round__(double v)
{
    return (int) floor(v + 0.5);
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::ProcessInput()
{
    int i;


    RefreshInput();
    bool new_keys[MAX_KEY];
    GetKeyStates(new_keys);

    // clear the key queue
    while (AreKeysLeft())
    {
        GetKey();
    }

    // check to see if key state has changed
    for (i = MAX_KEY -1; i>=0; --i)
    {
        if (new_keys[i] != m_Keys[i])
        {
            if (new_keys[i])
            {                // event: key down
                if (m_BoundKeys.count(i) > 0)
                {
                    // bound
                    if (!ProcessBoundKeyDown(i))
                        return false;
                }
                else
                {
                    // unbound
                    ProcessUnboundKeyDown(i);
                }
            }
            else
            {       // event: key up
                if (m_BoundKeys.count(i) > 0)
                {
                    // bound
                    if (!ProcessBoundKeyUp(i))
                        return false;
                }
                else
                {
                    // unbound
                    ProcessUnboundKeyUp(i);
                }
            }
        }
    }

    // save the current key buffer until the next call to ProcessInput
    GetKeyStates(m_Keys);


    // process default input bindings
    for (i = 0; i < int(m_InputPersons.size()); i++)
    {
        const int person = m_InputPersons[i];
        // if (m_IsInputAttached && m_Persons[m_InputPerson].commands.size() == 0) {
        if (m_Persons[person].commands.size() == 0)
        {
            int dx = 0;
            int dy = 0;
            Person* p = &m_Persons[person];

            bool moved = false;
			bool is_left_bound  = IsKeyBound(p->key_left);
            bool is_right_bound = IsKeyBound(p->key_right);
            bool is_up_bound    = IsKeyBound(p->key_up);
            bool is_down_bound  = IsKeyBound(p->key_down);

            if (m_Persons[person].keyboard_input_allowed)
            {
                if (!is_up_bound    && new_keys[p->key_up])    dy--;
                if (!is_right_bound && new_keys[p->key_right]) dx++;
                if (!is_down_bound  && new_keys[p->key_down])  dy++;
                if (!is_left_bound  && new_keys[p->key_left])  dx--;

            }
            if (p->joypad_input_allowed)
            {
                if (p->player_index >= 0 && p->player_index < GetNumJoysticks())
                {
                    if (!is_left_bound && !is_right_bound)
                        dx += __round__(GetJoystickAxis(p->player_index, JOYSTICK_AXIS_X));

                    if (!is_up_bound && !is_down_bound)
                        dy += __round__(GetJoystickAxis(p->player_index, JOYSTICK_AXIS_Y));
                }

            }

			// Unitarian directional vectors
			dx = dx < 0? -1 : dx>0? 1 : 0;
			dy = dy < 0? -1 : dy>0? 1 : 0;

			int command = -1;
			switch(dx + dy*3){
				case -4: // north west
					p->commands.push_back(Person::Command(COMMAND_MOVE_NORTH, true));
					p->commands.push_back(Person::Command(COMMAND_MOVE_WEST, true));
					p->commands.push_back(Person::Command(COMMAND_FACE_NORTHWEST, false));
					break;
				case -1: //west
					p->commands.push_back(Person::Command(COMMAND_MOVE_WEST, true));
					p->commands.push_back(Person::Command(COMMAND_FACE_WEST, false));
					break;
				case 2: // south west
					p->commands.push_back(Person::Command(COMMAND_MOVE_SOUTH, true));
					p->commands.push_back(Person::Command(COMMAND_MOVE_WEST, true));
					p->commands.push_back(Person::Command(COMMAND_FACE_SOUTHWEST, false));
					break;
				case -3: // north
					p->commands.push_back(Person::Command(COMMAND_MOVE_NORTH, true));
					p->commands.push_back(Person::Command(COMMAND_FACE_NORTH, false));
					break;
				case 3: // south
					p->commands.push_back(Person::Command(COMMAND_MOVE_SOUTH, true));
					p->commands.push_back(Person::Command(COMMAND_FACE_SOUTH, false));
					break;
				case -2: // north east
					p->commands.push_back(Person::Command(COMMAND_MOVE_NORTH, true));
					p->commands.push_back(Person::Command(COMMAND_MOVE_EAST, true));
					p->commands.push_back(Person::Command(COMMAND_FACE_NORTHEAST, false));
					break;
				case 1: // east
					p->commands.push_back(Person::Command(COMMAND_MOVE_EAST, true));
					p->commands.push_back(Person::Command(COMMAND_FACE_EAST, false));
					break;
				case 4: // south east
					p->commands.push_back(Person::Command(COMMAND_MOVE_SOUTH, true));
					p->commands.push_back(Person::Command(COMMAND_MOVE_EAST, true));
					p->commands.push_back(Person::Command(COMMAND_FACE_SOUTHEAST, false));
					break;
			}
			
        }
    }

    // process bound joystick buttons
    for (unsigned int j = 0; j < m_BoundJoysticks.size(); ++j)
    {
        int joystick = m_BoundJoysticks[j].m_Joystick;
        if (joystick >= 0 && joystick < GetNumJoysticks())
        {
            int num_buttons = GetNumJoystickButtons(joystick);
            m_BoundJoysticks[j].m_Buttons.resize(num_buttons);
            for (int button = 0; button < num_buttons; ++button)
            {
                bool button_pressed = IsJoystickButtonPressed(joystick, button);
                if (button_pressed == true && m_BoundJoysticks[j].m_Buttons[button] == false)
                {
                    m_BoundJoysticks[j].m_Buttons[button] = true;
                    if (m_BoundJoysticks[j].m_BoundButtons.count(button) > 0)
                    {
                        if (!ProcessBoundJoystickButtonDown(joystick, button))
                        {
                            return false;
                        }
                    }
                }
                else if (button_pressed == false && m_BoundJoysticks[j].m_Buttons[button] == true)
                {
                    m_BoundJoysticks[j].m_Buttons[button] = false;
                    if (m_BoundJoysticks[j].m_BoundButtons.count(button) > 0)
                    {
                        if (!ProcessBoundJoystickButtonUp(joystick, button))
                        {
                            return false;
                        }
                    }
                }
            }
        }
    }
    if (GetNumJoysticks() > 0)
    {
        if (IsJoystickButtonPressed(0, m_JoystickCancelButton))
        {
            // m_ShouldExit = true;
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::ProcessBoundKeyDown(int key)
{
    KeyScripts& a = m_BoundKeys[key];

    if (m_Engine->IsScriptBeingUsed(a.down))
    {
        m_ErrorMessage = "Key down script already running!";
        return false;
    }
    std::string error;
    if (!ExecuteScript(a.down, error))
    {
        m_ErrorMessage = "Could not execute key down script\n" + error;
        return false;
    }

    ResetNextFrame();
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::ProcessBoundKeyUp(int key)
{
    KeyScripts& a = m_BoundKeys[key];

    if (m_Engine->IsScriptBeingUsed(a.up))
    {
        m_ErrorMessage = "Key up script already running!";
        return false;
    }
    std::string error;
    if (!ExecuteScript(a.up, error))
    {
        m_ErrorMessage = "Could not execute key up script\n" + error;
        return false;
    }

    ResetNextFrame();
    return true;
}

////////////////////////////////////////////////////////////////////////////////

void
CMapEngine::ProcessUnboundKeyDown(int key)
{
    switch (key)
    {
    case KEY_ESCAPE:
    {
        m_ShouldExit = true;
    }
    break;

    case KEY_F1:
    {
        m_ThrottleFPS = !m_ThrottleFPS;
        m_NextFrame = (qword)GetTime() * m_FrameRate;  // update next rendering time
    }
    break;
    }
}

////////////////////////////////////////////////////////////////////////////////

void
CMapEngine::ProcessUnboundKeyUp(int key)
{}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::ProcessBoundJoystickButtonDown(int joystick, int button)
{
    int bound_joystick_index = -1;
    for (unsigned int i = 0; i < m_BoundJoysticks.size(); ++i)
    {
        if (m_BoundJoysticks[i].m_Joystick == joystick)
        {
            bound_joystick_index = i;
            break;
        }
    }
    //if (bound_joystick_index == -1) {
    //  m_ErrorMessage = "Could not process bound joystick button down event\n" + error;
    //  return false; // this should error should never occur
    //}
    KeyScripts& a = m_BoundJoysticks[bound_joystick_index].m_BoundButtons[button];
    std::string error;
    if (!ExecuteScript(a.down, error))
    {
        m_ErrorMessage = "Could not execute button down script\n" + error;
        return false;
    }
    ResetNextFrame();
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::ProcessBoundJoystickButtonUp(int joystick, int button)
{
    int bound_joystick_index = -1;
    for (unsigned int i = 0; i < m_BoundJoysticks.size(); ++i)
    {
        if (m_BoundJoysticks[i].m_Joystick == joystick)
        {
            bound_joystick_index = i;
            break;
        }
    }
    //if (bound_joystick_index == -1) {
    //  m_ErrorMessage = "Could not process bound joystick button up event\n" + error;
    //  return false; // this should error should never occur
    //}
    KeyScripts& a = m_BoundJoysticks[bound_joystick_index].m_BoundButtons[button];
    std::string error;
    if (!ExecuteScript(a.up, error))
    {
        m_ErrorMessage = "Could not execute button up script\n" + error;
        return false;
    }
    ResetNextFrame();
    return true;
}

////////////////////////////////////////////////////////////////////////////////

void
CMapEngine::DestroyBoundKeys()
{
    std::map<int, KeyScripts>::iterator i;
    for (i = m_BoundKeys.begin(); i != m_BoundKeys.end(); i++)
    {
        m_Engine->DestroyScript(i->second.down);
        m_Engine->DestroyScript(i->second.up);
    }

    m_BoundKeys.clear();
    for (int j = 0; j < int(m_BoundJoysticks.size()); j++)
    {
        std::map<int, KeyScripts>::iterator i;
        for (i = m_BoundJoysticks[j].m_BoundButtons.begin(); i != m_BoundJoysticks[j].m_BoundButtons.end(); i++)
        {
            m_Engine->DestroyScript(i->second.down);
            m_Engine->DestroyScript(i->second.up);
        }

        m_BoundJoysticks[j].m_BoundButtons.clear();
    }
}

////////////////////////////////////////////////////////////////////////////////

void
CMapEngine::ResetNextFrame()
{
    m_NextFrame = (qword)GetTime() * m_FrameRate;
}

////////////////////////////////////////////////////////////////////////////////

int
CMapEngine::FindPerson(const char* name)
{
    for (int i = int(m_Persons.size())-1; i>=0; --i)
    {
        if (m_Persons[i].name == name)
        {
            return i;
        }
    }

    return -1;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetObstructingTile(const char* name, int x, int y, int& result)
{
    // find person
    int person = -1;
    if ( IsInvalidPersonError(name, person) )
    {
        result = -1;
        return false;
    }

    result = FindObstructingTile(person, x, y);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

int
CMapEngine::FindObstructingTile(int person, int x, int y)
{
    const Person& p = m_Persons[person];
    const sLayer& layer = m_Map.GetMap().GetLayer(p.layer);
    const int tile_width  = m_Map.GetMap().GetTileset().GetTileWidth();
    const int tile_height = m_Map.GetMap().GetTileset().GetTileHeight();

    int bx = (p.base_x1 + p.base_x2) / 2;
    int by = (p.base_y1 + p.base_y2) / 2;
    int x1 = (x - bx) + std::min(p.base_x1, p.base_x2);
    int y1 = (y - by) + std::min(p.base_y1, p.base_y2);
    int x2 = (x - bx) + std::max(p.base_x2, p.base_x1);
    int y2 = (y - by) + std::max(p.base_y2, p.base_y2);

    // test per-tile obstructions
    int min_x = (x1 < x2 ? x1 : x2);
    int max_x = (x1 > x2 ? x1 : x2);
    int min_y = (y1 < y2 ? y1 : y2);
    int max_y = (y1 > y2 ? y1 : y2);

    int min_tx = min_x / tile_width;
    int max_tx = max_x / tile_width;
    int min_ty = min_y / tile_height;
    int max_ty = max_y / tile_height;

    for (int ty = min_ty; ty <= max_ty; ty++)
    {
        if (ty < 0 || ty >= layer.GetHeight())
        {
            continue;
        }
        for (int tx = min_tx; tx <= max_tx; tx++)
        {
            if (tx < 0 || tx >= layer.GetWidth())
            {
                continue;
            }

            // get the tile object
            const int t = m_Map.GetAnimationMap()[layer.GetTile(tx, ty)].current;
            const sTile& tile = m_Map.GetMap().GetTileset().GetTile(t);

            int tbx = tx * tile_width;
            int tby = ty * tile_height;

            if (tile.GetObstructionMap().TestRectangle(x1 - tbx, y1 - tby, x2 - tbx, y2 - tby))
            {
                return t;
            }
        }
    }

    return -1;
}

///////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetObstructingPerson(const char* person_name, int x, int y, std::string& result)
{
    // find person
    int person_index = -1;
    int found = -1;
    if ( IsInvalidPersonError(person_name, person_index) )
    {
        result = "";
        return false;
    }

    found = FindObstructingPerson(person_index, x, y);
    if (found != -1)
    {
        result = m_Persons[found].name;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::GetTalkingPerson(const char* person_name, int talk_x, int talk_y, std::string& result)
{
    // find person
    int person_index = -1;
    int found = -1;
    if ( IsInvalidPersonError(person_name, person_index) )
    {
        result = "";
        return false;
    }
    found = FindTalkingPerson(person_index, talk_x, talk_y);
    if (found != -1)
    {
        result = m_Persons[found].name;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////

int
CMapEngine::FindTalkingPerson(int person_index, int talk_x, int talk_y)
{
    const char* direction =  m_Persons[person_index].direction.c_str();

    // God this was slow... (successfully gets 'northwest' )
    if (strstr(direction, "north"))
        talk_y -= m_TalkActivationDistance;
    if (strstr(direction, "east"))
        talk_x += m_TalkActivationDistance;
    if (strstr(direction, "south"))
        talk_y += m_TalkActivationDistance;
    if (strstr(direction, "west"))
        talk_x -= m_TalkActivationDistance;

    // does a person obstructs that spot
    int obs_person;
    IsObstructed(person_index, talk_x, talk_y, obs_person);
    return obs_person;
}

///////////////////////////////////////////////////////////////////////////////

int
CMapEngine::FindObstructingPerson(int person, int x, int y)
{
    const Person& p = m_Persons[person];
    //  const sLayer& layer = m_Map.GetMap().GetLayer(p.layer);
    //  const int tile_width  = m_Map.GetMap().GetTileset().GetTileWidth();
    //  const int tile_height = m_Map.GetMap().GetTileset().GetTileHeight();

    int bx = (p.base_x1 + p.base_x2) / 2;
    int by = (p.base_y1 + p.base_y2) / 2;

    int x1 = x - bx + p.base_x1;
    int y1 = y - by + p.base_y1;
    int x2 = x - bx + p.base_x2;
    int y2 = y - by + p.base_y2;

    int min_x = (x1 < x2 ? x1 : x2);
    int max_x = (x1 > x2 ? x1 : x2);
    int min_y = (y1 < y2 ? y1 : y2);
    int max_y = (y1 > y2 ? y1 : y2);

    // check obstructions against other entities
    for (int i = 0; i < int(m_Persons.size()); i++)
    {
        // don't check current person
        if (i == person)
            continue;

        // if people aren't on the same layer, skip
        if (m_Persons[i].layer != m_Persons[person].layer)
            continue;

        // if the person is ignoring person obstructions, don't check it
        if (m_Persons[i].ignorePersonObstructions)
            continue;

        // if this entity is a follower of the current entity, don't check it
        int j = m_Persons[i].leader;
        while (j != -1)
        {
            if (j == person)
            {
                goto skip_this_guy;
            }
            j = m_Persons[j].leader;
        }

        // if the someone else is ignoring me, dont check collission with that person
        for (j = 0; j < int(m_Persons[person].ignored_persons.size()); j++)
        {
            if (m_Persons[person].ignored_persons[j] == m_Persons[i].name)
            {
                goto skip_this_guy;
            }
        }
        goto dont_skip;
skip_this_guy:
        continue;
dont_skip:

        // now do a simple bounding rectangle test
        const Person& q = m_Persons[i];

        int j_bx = (q.base_x1 + q.base_x2) / 2;
        int j_by = (q.base_y1 + q.base_y2) / 2;

        int j_x1 = int(q.x - j_bx + q.base_x1);
        int j_y1 = int(q.y - j_by + q.base_y1);
        int j_x2 = int(q.x - j_bx + q.base_x2);
        int j_y2 = int(q.y - j_by + q.base_y2);

        int min_jx = (j_x1 < j_x2 ? j_x1 : j_x2);
        int max_jx = (j_x1 > j_x2 ? j_x1 : j_x2);
        int min_jy = (j_y1 < j_y2 ? j_y1 : j_y2);
        int max_jy = (j_y1 > j_y2 ? j_y1 : j_y2);

        // from: http://www.cprogramming.com/snippets/show.php?tip=39&count=30&page=0
        if (max_x < min_jx ||
            min_x > max_jx ||
            min_y > max_jy ||
            max_y < min_jy)
            continue;

        return i;
/*
            // if a corner is within the rectangle
            (j_x1 >= min_x && j_x1 <= max_x && j_y1 >= min_y && j_y1 <= max_y) ||
            (j_x1 >= min_x && j_x1 <= max_x && j_y2 >= min_y && j_y2 <= max_y) ||
            (j_x2 >= min_x && j_x2 <= max_x && j_y1 >= min_y && j_y1 <= max_y) ||
            (j_x2 >= min_x && j_x2 <= max_x && j_y2 >= min_y && j_y2 <= max_y) ||

            // if the other rectangle has a corner in this one
            // The only way this can happen is that one rectangle is contained in the other,
            // http://www.gamedev.net/reference/articles/article735.asp
            // in this case, just need to check for one corner:
            (x1 >= min_jx && x1 <= max_jx && y1 >= min_jy && y1 <= max_jy)

            // this is the original code:
            //(x1 >= min_jx && x1 <= max_jx && y1 >= min_jy && y1 <= max_jy) ||
            //(x1 >= min_jx && x1 <= max_jx && y2 >= min_jy && y2 <= max_jy) ||
            //(x2 >= min_jx && x2 <= max_jx && y1 >= min_jy && y1 <= max_jy) ||
            //(x2 >= min_jx && x2 <= max_jx && y2 >= min_jy && y2 <= max_jy)
*/
 }
    return -1;
}

///////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::IsObstructed(int person, int x, int y, int& obs_person)
{
    // get useful elements
    const Person& p = m_Persons[person];
    const sObstructionMap& obs_map =

    m_Map.GetMap().GetLayer(p.layer).GetObstructionMap();
    // test obstruction map
    int bx = (p.base_x1 == p.base_x2) ? 1 : ((p.base_x1 + p.base_x2) / 2);
    int by = (p.base_y1 == p.base_y2) ? 1 : ((p.base_y1 + p.base_y2) / 2);

    int x1 = x - bx + p.base_x1;
    int y1 = y - by + p.base_y1;
    int x2 = x - bx + p.base_x2;
    int y2 = y - by + p.base_y2;

    obs_person = -1;
    if (obs_map.TestRectangle(x1, y1, x2, y2))
    {
        return true;
    }

    // don't check other entity obstructions if this spriteset ignores them
    if (!p.ignorePersonObstructions)
    {
        obs_person = FindObstructingPerson(person, x, y);
        if (obs_person != -1)
            return true;
    }

    if (!p.ignoreTileObstructions)
    {
        if (FindObstructingTile(person, x, y) != -1)
        {
            return true;
        }
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMapEngine::SaveMap(const char* filename)
{
    return m_Map.GetMap().Save(filename);
}

////////////////////////////////////////////////////////////////////////////////

/*
    BGM functions
*/

bool
CMapEngine::GetMapMusicName(std::string& result)
{
    result = m_LastMusicPath;
    return true;
}

int
CMapEngine::GetMapMusicType()
{
    if (m_Music)
    {
        return 2;
    }

#if defined(WIN32) && defined(USE_MIDI)
    if (m_Midi)
    {
        return 1;
    }
#endif
    return 0;
}

