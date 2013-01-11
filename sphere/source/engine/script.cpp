
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include <sstream>
#include <memory>
#include <math.h>

#include "script.hpp"

#include "audio.hpp"
#include "inputx.hpp"
#include "log.hpp"
#include "map_engine.hpp"
#include "render.hpp"
#include "sfont.hpp"
#include "sspriteset.hpp"
#include "system.hpp"
#include "swindowstyle.hpp"
#include "time.hpp"
#include "PlayerConfig.hpp"
#include "ssfxr.hpp"

#include "../common/sphere_version.h"
#include "../common/configfile.hpp"
#include "../common/Entities.hpp"

#include "../common/md5global.h"
#include "../common/md5.h"

#include "../common/zlibengn.h"

#include "../common/VectorStructs.hpp"

// parameter grabbing
#include "parameters.hpp"
const int MAX_RECURSE_COUNT = 256;
const int MAX_FRAME_SKIP    = 20;

///////////////////////////////////////////////////////////
const dword SS_SOCKET_MAGIC      = 0x70274be2;
const dword SS_LOG_MAGIC         = 0x435262c9;
const dword SS_COLOR_MAGIC       = 0x449b0beb;
const dword SS_SPRITESET_MAGIC   = 0x5f7ca182;
const dword SS_SOUND_MAGIC       = 0x7a5e371a;
const dword SS_SOUNDEFFECT_MAGIC = 0x3a2a396a;
const dword SS_FONT_MAGIC        = 0x7f7d79ef;
const dword SS_WINDOWSTYLE_MAGIC = 0x53f8d469;
const dword SS_IMAGE_MAGIC       = 0x168875d3;
const dword SS_SURFACE_MAGIC     = 0x09bbff98;
const dword SS_COLORMATRIX_MAGIC = 0x18ffbb09; //todo: check how magic numbers decided/created
const dword SS_ANIMATION_MAGIC   = 0x4c4ba103;
const dword SS_FILE_MAGIC        = 0x672d369a;
const dword SS_RAWFILE_MAGIC     = 0x29bcd805;
const dword SS_BYTEARRAY_MAGIC   = 0x2295027f;
const dword SS_MAPENGINE_MAGIC   = 0x42424401;
const dword SS_TILESET_MAGIC     = 0x43434402;
const dword SS_SFXR_MAGIC        = 0x474245a3;
const dword SS_PARTICLE_SYSTEM_PARENT_MAGIC  = 0x80000000;
const dword SS_PARTICLE_SYSTEM_CHILD_MAGIC   = 0x80000001;
const dword SS_PARTICLE_BODY_MAGIC           = 0x80000002;
const dword SS_PARTICLE_INITIALIZER_MAGIC    = 0x80000003;
const dword SS_PARTICLE_UPDATER_MAGIC        = 0x80000004;
const dword SS_PARTICLE_RENDERER_MAGIC       = 0x80000005; // Kyuu: I'm not a random number generator,
const dword SS_PARTICLE_CALLBACK_MAGIC       = 0x80000006; //       so this should do the trick.
const dword SS_PARTICLE_SWARM_MAGIC          = 0x80000007;
const dword SS_PARTICLE_SWARM_RENDERER_MAGIC = 0x80000008;
const dword SS_PARTICLE_DESCENDANTS_MAGIC    = 0x80000009;

// Addition to jsapi.h
#define JSVAL_IS_OBJECTNOTNULL(v)      ( (JSVAL_TAG(v) == JSVAL_OBJECT) && ((v) != JSVAL_NULL) && ((v) != JSVAL_VOID))

// inline to convert int's to strings (used in ToJSON functions)
template <class T>
inline std::string to_string (const T& t)
{
std::stringstream ss;
ss << t;
return ss.str();
}

struct SS_OBJECT
{
    dword magic;  // for object verification
};

#define BEGIN_SS_OBJECT(name)             \
  struct name : SS_OBJECT {               \
    name() { magic = name##_MAGIC; }

#define END_SS_OBJECT() \
  };

BEGIN_SS_OBJECT(SS_PARTICLE_SYSTEM_PARENT)
ParticleSystemParent* system;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_PARTICLE_SYSTEM_CHILD)
ParticleSystemChild* system;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_PARTICLE_BODY)
ParticleSystemBase* system;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_PARTICLE_INITIALIZER)
ParticleSystemBase* system;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_PARTICLE_UPDATER)
ParticleSystemBase* system;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_PARTICLE_RENDERER)
ParticleSystemBase* system;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_PARTICLE_CALLBACK)
ParticleSystemBase* system;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_PARTICLE_SWARM)
ParticleSystemChild* system;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_PARTICLE_SWARM_RENDERER)
ParticleSystemChild* system;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_PARTICLE_DESCENDANTS)
ParticleSystemParent* system;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_SOCKET)
NSOCKET socket;
bool is_open;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_LOG)
CLog* log;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_COLOR)
RGBA color;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_SPRITESET)
SSPRITESET* spriteset;
JSObject* object;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_SOUND)
audiere::OutputStream* sound;
#if defined(WIN32) && defined(USE_MIDI)
audiere::MIDIStream* midi;
#endif
audiere::File* memoryfile;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_SOUNDEFFECT)
audiere::SoundEffect* sound;
audiere::File* memoryfile;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_SFXR)
bool destroy_me;
SSFXR* sfxr;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_FONT)
bool destroy_me;
SFONT* font;
RGBA mask;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_WINDOWSTYLE)
bool destroy_me;
SWINDOWSTYLE* windowstyle;
RGBA mask;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_IMAGE)
bool destroy_me;
IMAGE image;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_SURFACE)
CImage32* surface;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_COLORMATRIX)
CColorMatrix* colormatrix;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_ANIMATION)
IAnimation* animation;
RGBA*       frame;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_FILE)
CConfigFile* file;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_RAWFILE)
IFile* file;
bool is_open;

bool is_writeable;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_BYTEARRAY)
int   size;
byte* array;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_MAPENGINE)
int __value__;
END_SS_OBJECT()

BEGIN_SS_OBJECT(SS_TILESET)
int __value__;
END_SS_OBJECT()

class NoGCBlock
{
public:
    NoGCBlock(CScript* script)
    {
        m_script = script;
        m_script->m_GCEnabled = false;
    }

    ~NoGCBlock()
    {
        m_script->m_GCEnabled = true;
    }

private:
    CScript* m_script;
};

/////////////////
// CScriptCode //
/////////////////

////////////////////////////////////////////////////////////////////////////////
CScriptCode::CScriptCode(JSContext* context, JSObject* global, JSScript* script)
        : m_Context(context)
        , m_Global(global)
        , m_Script(script)
        , m_NumReferences(1)

        , m_BeingUsed(false)
{
    m_ScriptObject = JS_NewScriptObject(context, script);
    JS_AddRoot(context, &m_ScriptObject);
}

////////////////////////////////////////////////////////////////////////////////
CScriptCode::~CScriptCode()
{
    JS_RemoveRoot(m_Context, &m_ScriptObject);
    // script will be GC'd automatically
}

////////////////////////////////////////////////////////////////////////////////
void
CScriptCode::AddRef()
{
    m_NumReferences++;
}

////////////////////////////////////////////////////////////////////////////////
void
CScriptCode::Release()
{
    if (--m_NumReferences == 0)
    {
        delete this;
    }
}

////////////////////////////////////////////////////////////////////////////////
bool
CScriptCode::IsBeingUsed()
{
    return (m_BeingUsed);
}
////////////////////////////////////////////////////////////////////////////////
bool
CScriptCode::Execute(bool& should_exit)
{
    m_BeingUsed = true;

    // get a handle to the parent CScript object
    CScript* script_object = (CScript*)JS_GetContextPrivate(m_Context);

    // update the system (handle quit messages if we must)
    UpdateSystem();

    // default error message if the error reporter isn't called
    script_object->m_Error = "Unspecified Error";

    bool do_exit = false;
    if (script_object->m_ShouldExit)
    {
        bool do_exit = true;
        script_object->m_ShouldExit = false;
    }

    jsval rval;
    JSBool result = JS_ExecuteScript(m_Context, m_Global, m_Script, &rval);
    bool succeeded = (result == JS_TRUE);

    if (do_exit)
        script_object->m_ShouldExit = true;

    // if there is no error message, we succeeded
    if (script_object->m_Error == "")
    {
        succeeded = true;
    }

    should_exit = script_object->m_ShouldExit;
    m_BeingUsed = false;
    return succeeded;
}

////////////////////////////////////////////////////////////////////////////////
static bool IsValidPath(const char* path, bool no_double_dots = false)
{
    int num_double_dots = 0;
    bool prev_was_dot = false;

    // empty path
    if (strlen(path) == 0)
        return false;

    for (unsigned int i = 0; i < strlen(path); ++i)
    {

        if (path[i] == '.')
        {
            if (prev_was_dot)
                num_double_dots += 1;

            prev_was_dot = true;
        }
        else
        {
            prev_was_dot = false;
        }
    }

    // path is trying to do things like "../../"
    if (no_double_dots && num_double_dots > 0)
        return false;
    else if (num_double_dots > 1)
        return false;

    // path starts with ~/ and has no ..'s
    if (strlen(path) >= 2)
    {
        if (path[0] == '~' || path[1] == '/')
        {
            if (num_double_dots > 0)
                return false;
        }
    }

    // path starts /
    if (path[0] == '/')
    {
        // path starts with /common/ and has no ..'s
        if (strlen(path) >= strlen("/common/"))
        {

            if (memcmp(path, "/common/", strlen("/common/")) == 0)
            {
                if (num_double_dots > 0)
                    return false;

                return true;
            }
        }

        return false;
    }

    // path begins with backslash or forwardslash
    if (path[0] == '/' || path[0] == '\\')
        return false;

    return true;
}

// Very crude way to check if a file exists. W32 has nicer functions, but they are incompatible with the other interps...
/*
#include <sys/stat.h>

int fexist( char *filename ) {
struct stat buffer ;
if ( stat( filename, &buffer ) ) return 1 ;
return 0 ;
}
*/
static bool DoesFileExist(const char* pathandfile)
{
    FILE *f = fopen(pathandfile, "r");
    bool found = f ? true: false;
    if(found) fclose(f);
    return found;
}

////////////////////////////////////////////////////////////////////////////////
/////////////
// CScript //
/////////////

////////////////////////////////////////////////////////////////////////////////
CScript::CScript(IEngine* engine)
        : m_Engine(engine)
        , m_Runtime(NULL)
        , m_Context(NULL)
        , m_Global(NULL)

        , m_GCEnabled(true)
        , m_GCCount(0)

        , m_RecurseCount(0)
        , m_ShouldRender(true)
        , m_FrameRate(0)
        , m_FramesSkipped(0)
        , m_IdealTime(0)
{
    // create runtime
    m_Runtime = JS_NewRuntime(5 *1024 * 1024);
    if (m_Runtime == NULL)
    {
        return;
    }

    // create context
    m_Context = JS_NewContext(m_Runtime, 8192);
    if (m_Context == NULL)
    {
        JS_DestroyRuntime(m_Runtime);
        m_Runtime = NULL;
    }

    // create global class
    static JSClass global_class =
        {
            "global", 0,
            JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
            JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
        };

    // create global object
    m_Global = JS_NewObject(m_Context, &global_class, NULL, NULL);
    if (m_Global == NULL)
    {
        JS_DestroyContext(m_Context);
        m_Context = NULL;
        JS_DestroyRuntime(m_Runtime);
        m_Runtime = NULL;
        return;
    }

    // assign the script object to the context
    JS_SetContextPrivate(m_Context, this);

    JS_InitStandardClasses(m_Context, m_Global);
    JS_SetErrorReporter(m_Context, ErrorReporter);
#ifndef TRACEMONKEY
    JS_SetBranchCallback(m_Context, BranchCallback);
#else
    /*
    extern JS_PUBLIC_API(JSOperationCallback)
    JS_SetOperationCallback(JSContext *cx, JSOperationCallback callback);

    extern JS_PUBLIC_API(JSOperationCallback)
    JS_GetOperationCallback(JSContext *cx);

    extern JS_PUBLIC_API(void)
    JS_TriggerOperationCallback(JSContext *cx);
    */
#endif


    InitializeSphereFunctions();
    InitializeSphereConstants();
}

////////////////////////////////////////////////////////////////////////////////
CScript::~CScript()
{
    if (m_Runtime)
    {
        if (m_Context)
        {
            JS_DestroyContext(m_Context);
            m_Context = NULL;
        }

        JS_DestroyRuntime(m_Runtime);
        m_Runtime = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////
bool
CScript::EvaluateFile(const char* filename, IFileSystem& fs)
{
    // load the script
    std::auto_ptr<IFile> file(fs.Open(filename, IFileSystem::read));
    if (!file.get())
    {
        m_Error = "Could not load file '";
        m_Error += filename;
        m_Error += "'";
        return false;
    }

    // read file
    int size = file->Size();
    char* script = new char[size + 1];
    if (!script)
    {

        return false;
    }
    file->Read(script, size);
    script[size] = '\0';

    // evaluate the read text
    bool result = Evaluate(script, filename);
    delete[] script;
    return result;
}

////////////////////////////////////////////////////////////////////////////////
bool
CScript::Evaluate(const char* script, const char* filename)
{
    if (!m_Runtime)
    {
        m_Error = "JavaScript engine initialization failed";
        return false;
    }

    // update the system (handle quit messages if we must)
    UpdateSystem();

    m_Error = "";
    m_ShouldExit = false;

    // run the script
    jsval result;
    JSBool r = JS_EvaluateScript(
                   m_Context,
                   m_Global,
                   script,
                   strlen(script),
                   (filename ? filename : NULL),
                   0,
                   &result);

    // if script execution failed but there is no error, Exit() was called
    if (r == JS_FALSE && m_Error == "")
    {
        return true;
    }

    return (r == JS_TRUE);
}

////////////////////////////////////////////////////////////////////////////////
CScriptCode*
CScript::Compile(const char* script)
{
    m_ShouldExit = false;

    JSScript* s = JS_CompileScript(
                      m_Context,
                      m_Global,
                      script,
                      strlen(script),
                      NULL,
                      0
                  );

    if (!s)
    {
        return NULL;
    }

    return new CScriptCode(m_Context, m_Global, s);
}

////////////////////////////////////////////////////////////////////////////////
const char*
CScript::GetError()
{
    return m_Error.c_str();
}

////////////////////////////////////////////////////////////////////////////////
void
CScript::InitializeSphereFunctions()
{
    static JSFunctionSpec functions[] =
        {
#define SS_FUNCTION(name, numargs) { #name, ss##name, numargs, 0, 0 },
#include "ss_functions.table"
#undef SS_FUNCTION
            { 0, 0, 0, 0, 0 },
        };

    JS_DefineFunctions(m_Context, m_Global, functions);
}

////////////////////////////////////////////////////////////////////////////////
void
CScript::InitializeSphereConstants()
{
    static struct
    {
        const char* name;
        int         value;
    }
    constants[] = {

                      // blend mode constants
                      { "BLEND",         CImage32::BLEND },
                      { "REPLACE",       CImage32::REPLACE },
                      { "RGB_ONLY",      CImage32::RGB_ONLY },
                      { "ALPHA_ONLY",    CImage32::ALPHA_ONLY },
                      { "ADD",           CImage32::ADD },
                      { "SUBTRACT",      CImage32::SUBTRACT },
                      { "MULTIPLY",      CImage32::MULTIPLY },
                      { "AVERAGE",       CImage32::AVERAGE },
                      { "INVERT",        CImage32::INVERT },

                      // primitive constants
                      { "LINE_MULTIPLE", 0 },
                      { "LINE_STRIP",    1 },
                      { "LINE_LOOP",     2 },

                      // sound effect constants
                      { "SE_SINGLE",   audiere::SINGLE },
                      { "SE_MULTIPLE", audiere::MULTIPLE },

                      // sfxr wave type constants
                      { "SQUAREWAVE",  sSfxr::SQUAREWAVE },
                      { "SAWTOOTH",    sSfxr::SAWTOOTH },
                      { "SINEWAVE",    sSfxr::SINEWAVE },
                      { "NOISE",       sSfxr::NOISE },

                      // windowstyle constants
                      { "EDGE_LEFT", sWindowStyle::LEFT },
                      { "EDGE_TOP", sWindowStyle::TOP },
                      { "EDGE_RIGHT", sWindowStyle::RIGHT },
                      { "EDGE_BOTTOM", sWindowStyle::BOTTOM },

                      // particle system constants
                      { "PS_PARENT", ParticleSystemBase::PARENT },
                      { "PS_CHILD",  ParticleSystemBase::CHILD },

                      { "PS_SHAPE_NULL",      ParticleInitializer::NULL_SHAPE },
                      { "PS_SHAPE_RECTANGLE", ParticleInitializer::RECTANGULAR_SHAPE },
                      { "PS_SHAPE_ELLIPSE",   ParticleInitializer::ELLIPTICAL_SHAPE },

                      { "PS_ORIENTATION_EXPLICIT", ParticleInitializer::EXPLICIT_ORIENTATION },
                      { "PS_ORIENTATION_IMPLICIT", ParticleInitializer::IMPLICIT_ORIENTATION },


                      /* keyboard constants */
#define KEY_CONSTANT(name) { #name, name },
                      KEY_CONSTANT(KEY_ESCAPE)
                      KEY_CONSTANT(KEY_F1)
                      KEY_CONSTANT(KEY_F2)
                      KEY_CONSTANT(KEY_F3)
                      KEY_CONSTANT(KEY_F4)
                      KEY_CONSTANT(KEY_F5)
                      KEY_CONSTANT(KEY_F6)
                      KEY_CONSTANT(KEY_F7)
                      KEY_CONSTANT(KEY_F8)
                      KEY_CONSTANT(KEY_F9)
                      KEY_CONSTANT(KEY_F10)
                      KEY_CONSTANT(KEY_F11)
                      KEY_CONSTANT(KEY_F12)
                      KEY_CONSTANT(KEY_TILDE)
                      KEY_CONSTANT(KEY_0)
                      KEY_CONSTANT(KEY_1)
                      KEY_CONSTANT(KEY_2)
                      KEY_CONSTANT(KEY_3)
                      KEY_CONSTANT(KEY_4)
                      KEY_CONSTANT(KEY_5)
                      KEY_CONSTANT(KEY_6)
                      KEY_CONSTANT(KEY_7)
                      KEY_CONSTANT(KEY_8)
                      KEY_CONSTANT(KEY_9)
                      KEY_CONSTANT(KEY_MINUS)
                      KEY_CONSTANT(KEY_EQUALS)
                      KEY_CONSTANT(KEY_BACKSPACE)
                      KEY_CONSTANT(KEY_TAB)
                      KEY_CONSTANT(KEY_A)
                      KEY_CONSTANT(KEY_B)
                      KEY_CONSTANT(KEY_C)
                      KEY_CONSTANT(KEY_D)
                      KEY_CONSTANT(KEY_E)
                      KEY_CONSTANT(KEY_F)
                      KEY_CONSTANT(KEY_G)
                      KEY_CONSTANT(KEY_H)
                      KEY_CONSTANT(KEY_I)
                      KEY_CONSTANT(KEY_J)
                      KEY_CONSTANT(KEY_K)
                      KEY_CONSTANT(KEY_L)
                      KEY_CONSTANT(KEY_M)
                      KEY_CONSTANT(KEY_N)
                      KEY_CONSTANT(KEY_O)
                      KEY_CONSTANT(KEY_P)
                      KEY_CONSTANT(KEY_Q)
                      KEY_CONSTANT(KEY_R)
                      KEY_CONSTANT(KEY_S)
                      KEY_CONSTANT(KEY_T)
                      KEY_CONSTANT(KEY_U)
                      KEY_CONSTANT(KEY_V)
                      KEY_CONSTANT(KEY_W)
                      KEY_CONSTANT(KEY_X)
                      KEY_CONSTANT(KEY_Y)
                      KEY_CONSTANT(KEY_Z)
                      KEY_CONSTANT(KEY_SHIFT)
                      KEY_CONSTANT(KEY_CAPSLOCK)
                      KEY_CONSTANT(KEY_NUMLOCK)
                      KEY_CONSTANT(KEY_SCROLLOCK)
                      KEY_CONSTANT(KEY_CTRL)
                      KEY_CONSTANT(KEY_ALT)
                      KEY_CONSTANT(KEY_SPACE)
                      KEY_CONSTANT(KEY_OPENBRACE)
                      KEY_CONSTANT(KEY_CLOSEBRACE)
                      KEY_CONSTANT(KEY_SEMICOLON)
                      KEY_CONSTANT(KEY_APOSTROPHE)
                      KEY_CONSTANT(KEY_COMMA)
                      KEY_CONSTANT(KEY_PERIOD)
                      KEY_CONSTANT(KEY_SLASH)
                      KEY_CONSTANT(KEY_BACKSLASH)
                      KEY_CONSTANT(KEY_ENTER)
                      KEY_CONSTANT(KEY_INSERT)
                      KEY_CONSTANT(KEY_DELETE)
                      KEY_CONSTANT(KEY_HOME)
                      KEY_CONSTANT(KEY_END)
                      KEY_CONSTANT(KEY_PAGEUP)
                      KEY_CONSTANT(KEY_PAGEDOWN)
                      KEY_CONSTANT(KEY_UP)
                      KEY_CONSTANT(KEY_RIGHT)
                      KEY_CONSTANT(KEY_DOWN)
                      KEY_CONSTANT(KEY_LEFT)

                      KEY_CONSTANT(KEY_NUM_0)
                      KEY_CONSTANT(KEY_NUM_1)
                      KEY_CONSTANT(KEY_NUM_2)
                      KEY_CONSTANT(KEY_NUM_3)
                      KEY_CONSTANT(KEY_NUM_4)
                      KEY_CONSTANT(KEY_NUM_5)
                      KEY_CONSTANT(KEY_NUM_6)
                      KEY_CONSTANT(KEY_NUM_7)
                      KEY_CONSTANT(KEY_NUM_8)
                      KEY_CONSTANT(KEY_NUM_9)

                      KEY_CONSTANT(MOUSE_LEFT)
                      KEY_CONSTANT(MOUSE_MIDDLE)
                      KEY_CONSTANT(MOUSE_RIGHT)
                      KEY_CONSTANT(MOUSE_WHEEL_UP)
                      KEY_CONSTANT(MOUSE_WHEEL_DOWN)

                      KEY_CONSTANT(JOYSTICK_AXIS_X)
                      KEY_CONSTANT(JOYSTICK_AXIS_Y)
                      KEY_CONSTANT(JOYSTICK_AXIS_Z)
                      KEY_CONSTANT(JOYSTICK_AXIS_R)
					  KEY_CONSTANT(JOYSTICK_AXIS_U)
					  KEY_CONSTANT(JOYSTICK_AXIS_V)
					  KEY_CONSTANT(JOYSTICK_MAX_AXIS)

                      KEY_CONSTANT(PLAYER_1)
                      KEY_CONSTANT(PLAYER_2)
                      KEY_CONSTANT(PLAYER_3)
                      KEY_CONSTANT(PLAYER_4)

                      KEY_CONSTANT(PLAYER_KEY_MENU)
                      KEY_CONSTANT(PLAYER_KEY_UP)
                      KEY_CONSTANT(PLAYER_KEY_DOWN)
                      KEY_CONSTANT(PLAYER_KEY_LEFT)
                      KEY_CONSTANT(PLAYER_KEY_RIGHT)
                      KEY_CONSTANT(PLAYER_KEY_A)
                      KEY_CONSTANT(PLAYER_KEY_B)
                      KEY_CONSTANT(PLAYER_KEY_X)
                      KEY_CONSTANT(PLAYER_KEY_Y)

#undef KEY_CONSTANT

#define MAP_ENGINE_CONSTANT(c) { #c, CMapEngine:: c },
                      MAP_ENGINE_CONSTANT(COMMAND_WAIT)
                      MAP_ENGINE_CONSTANT(COMMAND_ANIMATE)

                      MAP_ENGINE_CONSTANT(COMMAND_FACE_NORTH)
                      MAP_ENGINE_CONSTANT(COMMAND_FACE_NORTHEAST)
                      MAP_ENGINE_CONSTANT(COMMAND_FACE_EAST)
                      MAP_ENGINE_CONSTANT(COMMAND_FACE_SOUTHEAST)
                      MAP_ENGINE_CONSTANT(COMMAND_FACE_SOUTH)
                      MAP_ENGINE_CONSTANT(COMMAND_FACE_SOUTHWEST)
                      MAP_ENGINE_CONSTANT(COMMAND_FACE_WEST)
                      MAP_ENGINE_CONSTANT(COMMAND_FACE_NORTHWEST)
                      MAP_ENGINE_CONSTANT(COMMAND_MOVE_NORTH)
                      MAP_ENGINE_CONSTANT(COMMAND_MOVE_EAST)
                      MAP_ENGINE_CONSTANT(COMMAND_MOVE_SOUTH)
                      MAP_ENGINE_CONSTANT(COMMAND_MOVE_WEST)

                      MAP_ENGINE_CONSTANT(SCRIPT_ON_CREATE)
                      MAP_ENGINE_CONSTANT(SCRIPT_ON_DESTROY)
                      MAP_ENGINE_CONSTANT(SCRIPT_ON_ACTIVATE_TOUCH)
                      MAP_ENGINE_CONSTANT(SCRIPT_ON_ACTIVATE_TALK)
                      MAP_ENGINE_CONSTANT(SCRIPT_COMMAND_GENERATOR)

                      MAP_ENGINE_CONSTANT(SCRIPT_ON_ENTER_MAP)
                      MAP_ENGINE_CONSTANT(SCRIPT_ON_LEAVE_MAP)
                      MAP_ENGINE_CONSTANT(SCRIPT_ON_LEAVE_MAP_NORTH)
                      MAP_ENGINE_CONSTANT(SCRIPT_ON_LEAVE_MAP_EAST)
                      MAP_ENGINE_CONSTANT(SCRIPT_ON_LEAVE_MAP_SOUTH)
                      MAP_ENGINE_CONSTANT(SCRIPT_ON_LEAVE_MAP_WEST)

#undef MAP_ENGINE_CONSTANT
                  };

    // define the constants
    for (unsigned int i = 0; i < sizeof(constants) / sizeof(*constants); i++)
    {
        JS_DefineProperty(
            m_Context,
            m_Global,
            constants[i].name,
            INT_TO_JSVAL(constants[i].value),
            JS_PropertyStub,
            JS_PropertyStub,
            JSPROP_READONLY | JSPROP_PERMANENT
        );
    }
}

////////////////////////////////////////////////////////////////////////////////
void
CScript::ReportMapEngineError(const char* header)
{
    std::string error = header;
    error += "\n";
    error += m_Engine->GetMapEngine()->GetErrorMessage();
    JS_ReportError(m_Context, "%s", error.c_str());
}

////////////////////////////////////////////////////////////////////////////////
void
CScript::ErrorReporter(JSContext* cx, const char* message, JSErrorReport* report)
{
    CScript* This = (CScript*)JS_GetContextPrivate(cx);

    if (!report)
    {

        This->m_Error = message;
    }
    else
    {

        // build an error
        std::ostringstream os;

        if ( !(strlen(message) > 0 && message[strlen(message) - 1] == '\n') )
        {

            os << "Script error in '" << (report->filename ? report->filename : "(no filename)") << "', line: " << (report->lineno + 1) << std::endl;
            os << std::endl;
        }
        os << message << std::endl;
        if (report->linebuf)
        {
            os << report->linebuf << std::endl;
        }
        This->m_Error = os.str();
    }

    This->m_ShouldExit = true;
    //FILE* file = fopen("last_error.txt", "wb+");
    //if (file != NULL) {
    //  fwrite(This->m_Error.c_str(), sizeof(char), This->m_Error.length(), file);
    //  fclose(file);
    //}
    printf ("%s", This->m_Error.c_str());
}

////////////////////////////////////////////////////////////////////////////////
JSBool
CScript::BranchCallback(JSContext* cx, JSScript* script)
{
    CScript* This = (CScript*)JS_GetContextPrivate(cx);
    if (This)
    {
        // handle garbage collection
        if (This->m_GCCount++ >= 60*60*16 && This->m_GCEnabled)
        {
            // handle system events
            UpdateSystem();

            // garbage collect!
            JS_MaybeGC(cx);
            This->m_GCCount = 0;
        }
    }

    return JS_TRUE;
}

////////////////////////////////////////////////////////////////////////////////
template<typename T>
inline void USED(T /*t*/)
{ }

// system function definition macros and inline functions
#define begin_func(name, minargs)                                                                      \
  JSBool CScript::ss##name(JSContext* cx, JSObject* obj, uintN argc, jsval* argv, jsval* rval) {       \
    const char* script_name = #name;                                                                   \
    CScript* This = (CScript*)JS_GetContextPrivate(cx);                                                \
    NoGCBlock no_gc__(This);                                                                           \
    if (argc < minargs) {                                                                              \
      JS_ReportError(cx, "%s called with less than %s parameters", script_name, #minargs);             \
      *rval = JSVAL_NULL;                                                                              \
      return JS_FALSE;                                                                                 \
    }                                                                                                  \
    int arg = 0;                                                                                       \
    USED(arg);

#define end_func()  \
    return (This->m_ShouldExit ? JS_FALSE : JS_TRUE); \
  }

///////////////////////////////////////////////////////////
inline ParticleSystemBase* argParticleSystem(JSContext* cx, jsval arg)
{
    if (!JSVAL_IS_OBJECTNOTNULL(arg))
    {
        JS_ReportError(cx, "Invalid particle system object");
        return NULL;
    }

    if (JS_IsArrayObject(cx, JSVAL_TO_OBJECT(arg)))
    {
        JS_ReportError(cx, "Invalid particle system object");
        return NULL;
    }

    SS_OBJECT* unknown = (SS_OBJECT*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(arg));

    if (unknown == NULL)
    {
        JS_ReportError(cx, "Invalid particle system object");
        return NULL;
    }

    if (unknown->magic == SS_PARTICLE_SYSTEM_PARENT_MAGIC)
    {
        SS_PARTICLE_SYSTEM_PARENT* system;
        system = (SS_PARTICLE_SYSTEM_PARENT*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(arg));
        return system->system;
    }
    else if (unknown->magic == SS_PARTICLE_SYSTEM_CHILD_MAGIC)
    {
        SS_PARTICLE_SYSTEM_CHILD* system;
        system = (SS_PARTICLE_SYSTEM_CHILD*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(arg));
        return system->system;
    }
    else
    {
        JS_ReportError(cx, "Invalid particle system object");
        return NULL;
    }

}

///////////////////////////////////////////////////////////
inline RGBA argColor(JSContext* cx, jsval arg)
{
    if (!JSVAL_IS_OBJECTNOTNULL(arg))
    {
        JS_ReportError(cx, "Invalid color object");
        return CreateRGBA(0, 0, 0, 0);
    }

    if (JS_IsArrayObject(cx, JSVAL_TO_OBJECT(arg)))
    {
        JS_ReportError(cx, "Invalid color object");
        return CreateRGBA(0, 0, 0, 0);
    }

    SS_COLOR* color = (SS_COLOR*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(arg));
    if (color == NULL)
    {
        JS_ReportError(cx, "Invalid color object");
        return CreateRGBA(0, 0, 0, 0);
    }

    if (color->magic != SS_COLOR_MAGIC)
    {
        JS_ReportError(cx, "Invalid color object");
        return CreateRGBA(0, 0, 0, 0);
    }

    return color->color;
}

///////////////////////////////////////////////////////////
inline CImage32* argSurface(JSContext* cx, jsval arg)
{
    if (!JSVAL_IS_OBJECTNOTNULL(arg))
    {
        JS_ReportError(cx, "Invalid surface object");
        return NULL;
    }

    if (JS_IsArrayObject(cx, JSVAL_TO_OBJECT(arg)))
    {
        JS_ReportError(cx, "Invalid surface object");
        return NULL;
    }

    SS_SURFACE* surface = (SS_SURFACE*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(arg));
    if (surface == NULL)
    {
        JS_ReportError(cx, "Invalid surface object");
        return NULL;
    }

    if (surface->magic != SS_SURFACE_MAGIC)
    {
        JS_ReportError(cx, "Invalid surface object");
        return NULL;
    }

    return surface->surface;
}

///////////////////////////////////////////////////////////
inline CColorMatrix* argColorMatrix(JSContext* cx, jsval arg)
{
    if (!JSVAL_IS_OBJECTNOTNULL(arg))
    {
        JS_ReportError(cx, "Invalid colormatrix object");
        return NULL;
    }

    if (JS_IsArrayObject(cx, JSVAL_TO_OBJECT(arg)))
    {

        JS_ReportError(cx, "Invalid colormatrix object");
        return NULL;
    }
    SS_COLORMATRIX* colormatrix = (SS_COLORMATRIX*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(arg));
    if (colormatrix == NULL)
    {
        JS_ReportError(cx, "Invalid colormatrix object");
        return NULL;
    }

    if (colormatrix->magic != SS_COLORMATRIX_MAGIC)
    {
        JS_ReportError(cx, "Invalid colormatrix object");
        return NULL;
    }

    return colormatrix->colormatrix;
}

///////////////////////////////////////////////////////////
inline SS_BYTEARRAY* argByteArray(JSContext* cx, jsval arg)
{
    if (!JSVAL_IS_OBJECTNOTNULL(arg))
    {
        JS_ReportError(cx, "Invalid byte_array object");
        return NULL;
    }

    SS_BYTEARRAY* array = (SS_BYTEARRAY*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(arg));
    if (array == NULL)
    {
        JS_ReportError(cx, "Invalid byte_array object");
        return NULL;
    }

    if (array->magic != SS_BYTEARRAY_MAGIC)
    {
        JS_ReportError(cx, "Invalid byte_array object");
        return NULL;
    }

    return array;
}

///////////////////////////////////////////////////////////
inline SS_IMAGE* argImage(JSContext* cx, jsval arg)
{
    if (!JSVAL_IS_OBJECTNOTNULL(arg))
    {
        JS_ReportError(cx, "Invalid image object");
        return NULL;
    }

    if (JS_IsArrayObject(cx, JSVAL_TO_OBJECT(arg)))
    {
        JS_ReportError(cx, "Invalid image object");
        return NULL;
    }

    SS_IMAGE* image = (SS_IMAGE*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(arg));
    if (image == NULL)
    {
        JS_ReportError(cx, "Invalid image object");
        return NULL;
    }

    return image;
}

///////////////////////////////////////////////////////////
inline SS_SFXR* argSfxr(JSContext* cx, jsval arg)
{
    if (!JSVAL_IS_OBJECTNOTNULL(arg))
    {
        JS_ReportError(cx, "Invalid sfxr object");
        return NULL;
    }

    if (JS_IsArrayObject(cx, JSVAL_TO_OBJECT(arg)))
    {
        JS_ReportError(cx, "Invalid sfxr object");
        return NULL;
    }

    SS_SFXR* sfxr = (SS_SFXR*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(arg));
    if (sfxr == NULL)
    {
        JS_ReportError(cx, "Invalid sfxr object");
        return NULL;
    }

    if (sfxr->magic != SS_SFXR_MAGIC)
    {
        JS_ReportError(cx, "Invalid sfxr object magic");
        return NULL;
    }

    return sfxr;
}
///////////////////////////////////////////////////////////
inline SS_FONT* argFont(JSContext* cx, jsval arg)
{
    if (!JSVAL_IS_OBJECTNOTNULL(arg))
    {
        JS_ReportError(cx, "Invalid font object");
        return NULL;
    }

    if (JS_IsArrayObject(cx, JSVAL_TO_OBJECT(arg)))
    {
        JS_ReportError(cx, "Invalid font object");
        return NULL;
    }

    SS_FONT* font = (SS_FONT*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(arg));
    if (font == NULL)
    {
        JS_ReportError(cx, "Invalid font object");
        return NULL;
    }

    if (font->magic != SS_FONT_MAGIC)
    {
        JS_ReportError(cx, "Invalid font object magic");
        return NULL;
    }

    return font;
}

///////////////////////////////////////////////////////////
sSpriteset* argSpriteset(JSContext* cx, jsval arg)
{

    if ( !argObject(cx, arg) )
    {
        JS_ReportError(cx, "Invalid object.");
        return NULL;
    }
    JSObject* obj = JSVAL_TO_OBJECT(arg);
    if (!obj || !JS_AddRoot(cx, &obj))
    {
        JS_ReportError(cx, "Invalid object.");
        return NULL;
    }

    jsval images_array;
    if ( JS_GetProperty(cx, obj, "images", &images_array) == JS_FALSE || !JS_AddNamedRoot(cx, &images_array, "images_array"))
    {
        JS_RemoveRoot(cx, &obj);
        JS_ReportError(cx, "spriteset.images array property doesn't appear to exist.");
        return NULL;
    }

    jsval base_obstruction_val;
    if ( JS_GetProperty(cx, obj, "base", &base_obstruction_val) == JS_FALSE || !JS_AddNamedRoot(cx, &base_obstruction_val, "base_obstruction_val"))
    {
        JS_RemoveRoot(cx, &images_array);
		JS_RemoveRoot(cx, &obj);
        JS_ReportError(cx, "spriteset.base object property doesn't appear to exist.");
        return NULL;
    }

    JSObject* base_obstruction_object = argObject(cx, base_obstruction_val);
    if (base_obstruction_object == NULL || !JS_AddNamedRoot(cx, &base_obstruction_object, "base_obstruction_object"))
    {
        JS_RemoveRoot(cx, &base_obstruction_val);
        JS_RemoveRoot(cx, &images_array);
		JS_RemoveRoot(cx, &obj);
		JS_ReportError(cx, "spriteset.base object is not a valid object.");
        return NULL;
    }

	// Obtain and Root variable to keep it save from CG
    jsval x1_val, y1_val, x2_val, y2_val;
    if (
		JS_GetProperty(cx, base_obstruction_object, "x1", &x1_val) == JS_FALSE || !JS_AddRoot(cx, &x1_val)
            || JS_GetProperty(cx, base_obstruction_object, "y1", &y1_val) == JS_FALSE || !JS_AddRoot(cx, &y1_val)
            || JS_GetProperty(cx, base_obstruction_object, "x2", &x2_val) == JS_FALSE || !JS_AddRoot(cx, &x2_val)
            || JS_GetProperty(cx, base_obstruction_object, "y2", &y2_val) == JS_FALSE || !JS_AddRoot(cx, &y2_val)
		)
    {
        JS_RemoveRoot(cx, &base_obstruction_object);
        JS_RemoveRoot(cx, &base_obstruction_val);
        JS_RemoveRoot(cx, &images_array);
		JS_RemoveRoot(cx, &obj);
        JS_ReportError(cx, "spriteset.base object is invalid.");
        return NULL;
    }

	if (!JSVAL_IS_INT(x1_val) || !JSVAL_IS_INT(y1_val) || !JSVAL_IS_INT(x2_val) || !JSVAL_IS_INT(y2_val))
    {
        JS_RemoveRoot(cx, &x1_val);
        JS_RemoveRoot(cx, &y1_val);
        JS_RemoveRoot(cx, &x2_val);
        JS_RemoveRoot(cx, &y2_val);
        JS_RemoveRoot(cx, &base_obstruction_object);
        JS_RemoveRoot(cx, &base_obstruction_val);
        JS_RemoveRoot(cx, &images_array);
		JS_RemoveRoot(cx, &obj);
		if (!JSVAL_IS_INT(x1_val))
			JS_ReportError(cx, "spriteset.base.x1 is invalid");
		else if (!JSVAL_IS_INT(y1_val))
			JS_ReportError(cx, "spriteset.base.y1 is invalid");
		else if (!JSVAL_IS_INT(x2_val))
			JS_ReportError(cx, "spriteset.base.x2 is invalid");
		else if (!JSVAL_IS_INT(y2_val))
			JS_ReportError(cx, "spriteset.base.y2 is invalid");
        return NULL;
    }
    int x1 = argInt(cx, x1_val);
    int y1 = argInt(cx, y1_val);
    int x2 = argInt(cx, x2_val);
    int y2 = argInt(cx, y2_val);


    jsuint num_images = 0;
    JSObject* images_object = argArray(cx, images_array);
    if (images_object == NULL || !JS_AddNamedRoot(cx, &images_object, "images_object") )
    {
        JS_RemoveRoot(cx, &x1_val);
        JS_RemoveRoot(cx, &y1_val);
        JS_RemoveRoot(cx, &x2_val);
        JS_RemoveRoot(cx, &y2_val);
        JS_RemoveRoot(cx, &base_obstruction_object);
        JS_RemoveRoot(cx, &base_obstruction_val);
        JS_RemoveRoot(cx, &images_array);
		JS_RemoveRoot(cx, &obj);
        JS_ReportError(cx, "Invalid spriteset.images array.");
        return NULL;
    }

    if ( JS_GetArrayLength(cx, images_object, &num_images) == JS_FALSE || (num_images <= 0) )
    {
		JS_RemoveRoot(cx, &images_object);
        JS_RemoveRoot(cx, &x1_val);
        JS_RemoveRoot(cx, &y1_val);
        JS_RemoveRoot(cx, &x2_val);
        JS_RemoveRoot(cx, &y2_val);
        JS_RemoveRoot(cx, &base_obstruction_object);
        JS_RemoveRoot(cx, &base_obstruction_val);
        JS_RemoveRoot(cx, &images_array);
		JS_RemoveRoot(cx, &obj);
		if (num_images <= 0)
			JS_ReportError(cx, "Invalid spriteset.images length");
		else
			JS_ReportError(cx, "Invalid spriteset.images array length.");
        return NULL;
    }

    CImage32* images = new CImage32[num_images];
    if (images == NULL)
    {
		JS_RemoveRoot(cx, &images_object);
        JS_RemoveRoot(cx, &x1_val);
        JS_RemoveRoot(cx, &y1_val);
        JS_RemoveRoot(cx, &x2_val);
        JS_RemoveRoot(cx, &y2_val);
        JS_RemoveRoot(cx, &base_obstruction_object);
        JS_RemoveRoot(cx, &base_obstruction_val);
        JS_RemoveRoot(cx, &images_array);
		JS_RemoveRoot(cx, &obj);
        JS_ReportError(cx, "Cannot allocate space for spriteset.images (%d)", num_images);
        return NULL;
    }

    unsigned int i;
    int frame_width = 0;
    int frame_height = 0;

    for (i = 0; i < num_images; i++)
    {
        jsval image;
        if ( JS_GetElement(cx, images_object, i, &image) == JS_FALSE )
        {
			JS_RemoveRoot(cx, &images_object);
	        JS_RemoveRoot(cx, &x1_val);
			JS_RemoveRoot(cx, &y1_val);
			JS_RemoveRoot(cx, &x2_val);
		    JS_RemoveRoot(cx, &y2_val);
	        JS_RemoveRoot(cx, &base_obstruction_object);
			JS_RemoveRoot(cx, &base_obstruction_val);
		    JS_RemoveRoot(cx, &images_array);
			JS_RemoveRoot(cx, &obj);
            JS_ReportError(cx, "Invalid image %d", i);
            return NULL;
        }
        else
        {
            SS_IMAGE* ss_image = argImage(cx, image);
            if (ss_image == NULL)
            {
				JS_RemoveRoot(cx, &images_object);
			    JS_RemoveRoot(cx, &x1_val);
				JS_RemoveRoot(cx, &y1_val);
				JS_RemoveRoot(cx, &x2_val);
			    JS_RemoveRoot(cx, &y2_val);
				JS_RemoveRoot(cx, &base_obstruction_object);
			    JS_RemoveRoot(cx, &base_obstruction_val);
				JS_RemoveRoot(cx, &images_array);
				JS_RemoveRoot(cx, &obj);
                JS_ReportError(cx, "Invalid image: spriteset.images[%d]", i);
                delete[] images;
                return NULL;
            }

            int width  = GetImageWidth(ss_image->image);
            int height = GetImageHeight(ss_image->image);

            if ( (width <= 0) || (height <= 0))
            {
				JS_RemoveRoot(cx, &images_object);
			    JS_RemoveRoot(cx, &x1_val);
				JS_RemoveRoot(cx, &y1_val);
				JS_RemoveRoot(cx, &x2_val);
			    JS_RemoveRoot(cx, &y2_val);
				JS_RemoveRoot(cx, &base_obstruction_object);
			    JS_RemoveRoot(cx, &base_obstruction_val);
				JS_RemoveRoot(cx, &images_array);
				JS_RemoveRoot(cx, &obj);
				JS_ReportError(cx, "Invalid image dimensions: height=%d width=%d at index:%d", height,width, i);
                delete[] images;
                return NULL;
            }

            RGBA* pixels = LockImage(ss_image->image);
            if (!pixels)
            {
				JS_RemoveRoot(cx, &images_object);
			    JS_RemoveRoot(cx, &x1_val);
				JS_RemoveRoot(cx, &y1_val);
				JS_RemoveRoot(cx, &x2_val);
			    JS_RemoveRoot(cx, &y2_val);
				JS_RemoveRoot(cx, &base_obstruction_object);
			    JS_RemoveRoot(cx, &base_obstruction_val);
				JS_RemoveRoot(cx, &images_array);
				JS_RemoveRoot(cx, &obj);
                JS_ReportError(cx, "LockImage failed");
                delete[] images;
                return NULL;
            }
            images[i] = CImage32(width, height, pixels);
            UnlockImage(ss_image->image, false);
            if (images[i].GetWidth() != width || images[i].GetHeight() != height)
            {
				JS_RemoveRoot(cx, &images_object);
			    JS_RemoveRoot(cx, &x1_val);
				JS_RemoveRoot(cx, &y1_val);
				JS_RemoveRoot(cx, &x2_val);
			    JS_RemoveRoot(cx, &y2_val);
				JS_RemoveRoot(cx, &base_obstruction_object);
			    JS_RemoveRoot(cx, &base_obstruction_val);
				JS_RemoveRoot(cx, &images_array);
				JS_RemoveRoot(cx, &obj);
                JS_ReportError(cx, "Temporary image allocation failed");
                delete[] images;
                return NULL;
            }

			// All the other frames should have the same dimensions, so we use the first
            if (i == 0)
            {
                frame_width  = images[i].GetWidth();
                frame_height = images[i].GetHeight();
            }

            if (frame_width != images[i].GetWidth() || frame_width <= 0)
            {
				JS_RemoveRoot(cx, &images_object);
			    JS_RemoveRoot(cx, &x1_val);
				JS_RemoveRoot(cx, &y1_val);
				JS_RemoveRoot(cx, &x2_val);
			    JS_RemoveRoot(cx, &y2_val);
				JS_RemoveRoot(cx, &base_obstruction_object);
			    JS_RemoveRoot(cx, &base_obstruction_val);
				JS_RemoveRoot(cx, &images_array);
				JS_RemoveRoot(cx, &obj);
                JS_ReportError(cx, "Invalid frame width %d %d %d", i, frame_width, images[i].GetWidth());
                delete[] images;
                return NULL;
            }

            if (frame_height != images[i].GetHeight() || frame_height <= 0)
            {
				JS_RemoveRoot(cx, &images_object);
			    JS_RemoveRoot(cx, &x1_val);
				JS_RemoveRoot(cx, &y1_val);
				JS_RemoveRoot(cx, &x2_val);
			    JS_RemoveRoot(cx, &y2_val);
				JS_RemoveRoot(cx, &base_obstruction_object);
			    JS_RemoveRoot(cx, &base_obstruction_val);
				JS_RemoveRoot(cx, &images_array);
				JS_RemoveRoot(cx, &obj);
                JS_ReportError(cx, "Invalid frame height %d %d %d", i, frame_height, images[i].GetHeight());
                delete[] images;
                return NULL;
            }

        }
    }


    jsval directions_array;
    if ( !JS_GetProperty(cx, obj, "directions", &directions_array) || !JS_AddNamedRoot(cx, &directions_array, "directions_array"))
    {
		JS_RemoveRoot(cx, &images_object);
	    JS_RemoveRoot(cx, &x1_val);
		JS_RemoveRoot(cx, &y1_val);
		JS_RemoveRoot(cx, &x2_val);
	    JS_RemoveRoot(cx, &y2_val);
		JS_RemoveRoot(cx, &base_obstruction_object);
	    JS_RemoveRoot(cx, &base_obstruction_val);
		JS_RemoveRoot(cx, &images_array);
		JS_RemoveRoot(cx, &obj);
        JS_ReportError(cx, "spriteset.directions array property doesn't appear to exist.");
        delete[] images;
        return NULL;
    }

    
    JSObject* directions_object = argArray(cx, directions_array);
    if (!directions_object || !JS_AddRoot(cx, &directions_object))
    {
		JS_RemoveRoot(cx, &directions_array);
		JS_RemoveRoot(cx, &images_object);
	    JS_RemoveRoot(cx, &x1_val);
		JS_RemoveRoot(cx, &y1_val);
		JS_RemoveRoot(cx, &x2_val);
	    JS_RemoveRoot(cx, &y2_val);
		JS_RemoveRoot(cx, &base_obstruction_object);
	    JS_RemoveRoot(cx, &base_obstruction_val);
		JS_RemoveRoot(cx, &images_array);
		JS_RemoveRoot(cx, &obj);
        JS_ReportError(cx, "Invalid spriteset.directions array.");
        delete[] images;
        return NULL;
    }

    jsuint num_directions = 0;
    if ( JS_GetArrayLength(cx, directions_object, &num_directions) == JS_FALSE || (num_directions <= 0))
    {
		JS_RemoveRoot(cx, &directions_object);
		JS_RemoveRoot(cx, &directions_array);
		JS_RemoveRoot(cx, &images_object);
	    JS_RemoveRoot(cx, &x1_val);
		JS_RemoveRoot(cx, &y1_val);
		JS_RemoveRoot(cx, &x2_val);
	    JS_RemoveRoot(cx, &y2_val);
		JS_RemoveRoot(cx, &base_obstruction_object);
	    JS_RemoveRoot(cx, &base_obstruction_val);
		JS_RemoveRoot(cx, &images_array);
		JS_RemoveRoot(cx, &obj);
		if(num_directions <= 0)
			JS_ReportError(cx, "Invalid spriteset.directions length");
		else
	        JS_ReportError(cx, "Invalid spriteset.directions array length.");
        delete[] images;
        return NULL;
    }

    sSpriteset* s = new sSpriteset();
    if (s == NULL)
    {
		JS_RemoveRoot(cx, &directions_object);
		JS_RemoveRoot(cx, &directions_array);
		JS_RemoveRoot(cx, &images_object);
	    JS_RemoveRoot(cx, &x1_val);
		JS_RemoveRoot(cx, &y1_val);
		JS_RemoveRoot(cx, &x2_val);
	    JS_RemoveRoot(cx, &y2_val);
		JS_RemoveRoot(cx, &base_obstruction_object);
	    JS_RemoveRoot(cx, &base_obstruction_val);
		JS_RemoveRoot(cx, &images_array);
		JS_RemoveRoot(cx, &obj);
        JS_ReportError(cx, "Unable to allocate memory for spriteset");
        delete[] images;
        return NULL;
    }

    s->Create(frame_width, frame_height, num_images, num_directions, 0);
    for (i = 0; i < num_images; i++)
    {
        s->GetImage(i) = images[i];
    }
    delete[] images;

    s->SetBase(x1, y1, x2, y2);
    s->Base2Real();
    for (i = 0; i < num_directions; i++)
    {

        jsval direction_object_val; // This is a pointer to a rooted directions_object, and so contains protected information (no need for root)
        if ( !JS_GetElement(cx, directions_object, i, &direction_object_val) )
        {
            JS_ReportError(cx, "Invalid spriteset.directions[%d] object", i);
            return NULL;
        }

        jsval direction_name;
        JSObject* direction_object = JSVAL_TO_OBJECT(direction_object_val);

        if ( JS_GetProperty(cx, direction_object, "name", &direction_name) == JS_FALSE )
        {
            JS_ReportError(cx, "spriteset.directions[%d].name property doesn't appear to exist.", i);
            return NULL;
        }

        s->SetDirectionName(i, argStr(cx, direction_name));
        jsval frames_array_val;
        if ( JS_GetProperty(cx, direction_object, "frames", &frames_array_val) == JS_FALSE )
        {
            JS_ReportError(cx, "spriteset.directions[%d].frames property doesn't appear to exist.", i);
            return NULL;
        }

        JSObject* frames_array = JSVAL_TO_OBJECT(frames_array_val);
        jsuint num_frames = 0;

        if ( JS_GetArrayLength(cx, frames_array, &num_frames) == JS_FALSE )
        {
            JS_ReportError(cx, "Invalid spriteset.directions[%d].frames array length.", i);
            return NULL;
        }

        if (num_frames <= 0)
        {
            JS_ReportError(cx, "Invalid spriteset.directions[%d].frames length", i);
            return NULL;
        }

        for (unsigned int j = 0; j < num_frames; j++)
        {
            jsval frame_val;

            if ( JS_GetElement(cx, frames_array, j, &frame_val) == JS_FALSE )
            {
                JS_ReportError(cx, "Invalid spriteset.directions[%d].frames[%d] object", i, j);
                return NULL;
            }

            JSObject* frame_object = JSVAL_TO_OBJECT(frame_val);
            if (frame_object == NULL)
                return NULL;
            jsval frame_index_val;
            if ( !JS_GetProperty(cx, frame_object, "index", &frame_index_val) )
            {
                JS_ReportError(cx, "spriteset.directions[%d].frames[%d].index property doesn't appear to exist.", i, j);
                return NULL;
            }

            int frame_index = argInt(cx, frame_index_val);
            if (frame_index < 0 || frame_index >= int(num_images))
            {
                JS_ReportError(cx, "spriteset.directions[%d].frames[%d].index is an invalid image index of %d", i, j, frame_index);
                return NULL;
            }

            jsval frame_delay_val = 0;
            if ( !JS_GetProperty(cx, frame_object, "delay", &frame_delay_val) )
            {
                JS_ReportError(cx, "spriteset.directions[%d].frames[%d].delay property doesn't appear to exist.", i, j);
                return NULL;
            }

            int frame_delay = argInt(cx, frame_delay_val);
            if (frame_delay < 0)
            {
                JS_ReportError(cx, "spriteset.directions[%d].frames[%d].delay has an invalid value", i, j);
                return NULL;
            }

            s->InsertFrame(i, j);
            s->SetFrameDelay(i, j, frame_delay);
            s->SetFrameIndex(i, j, frame_index);
        }
    }

		JS_RemoveRoot(cx, &directions_object);
		JS_RemoveRoot(cx, &directions_array);
		JS_RemoveRoot(cx, &images_object);
	    JS_RemoveRoot(cx, &x1_val);
		JS_RemoveRoot(cx, &y1_val);
		JS_RemoveRoot(cx, &x2_val);
	    JS_RemoveRoot(cx, &y2_val);
		JS_RemoveRoot(cx, &base_obstruction_object);
	    JS_RemoveRoot(cx, &base_obstruction_val);
		JS_RemoveRoot(cx, &images_array);
		JS_RemoveRoot(cx, &obj);


    return s;
}

///////////////////////////////////////////////////////////
#define __arg_error_check__(arg_type)                                                                                          \
  if (This->m_ShouldExit) {                                                                                                    \
    JS_ReportError(cx, "%s - Argument %d, invalid %s...\n\"%s\"", script_name, arg - 1, arg_type, argStr(cx, argv[arg - 1])); \
    return JS_FALSE;                                                                                                           \
  }                                                                                                                            \


#define arg_int(name)             int name                  = argInt(cx, argv[arg++]);                                               __arg_error_check__("integer")
#define arg_str(name)             const char* name          = argStr(cx, argv[arg++]);                                               __arg_error_check__("string")
#define arg_bool(name)            bool name                 = argBool(cx, argv[arg++]);                                              __arg_error_check__("boolean")
#define arg_double(name)          double name               = argDouble(cx, argv[arg++]);                                            __arg_error_check__("double")
#define arg_object(name)          JSObject* name            = argObject(cx, argv[arg++]);                                            __arg_error_check__("Object")
#define arg_array(name)           JSObject* name            = argArray(cx, argv[arg++]);                                             __arg_error_check__("Array")
#define arg_color(name)           RGBA name                 = argColor(cx, argv[arg++]);                                             __arg_error_check__("Color")
#define arg_function_object(name) JSObject* name            = argFunctionObject(cx, argv[arg++]); if (name == NULL) return JS_FALSE; __arg_error_check__("FunctionObject")
#define arg_surface(name)         CImage32* name            = argSurface(cx, argv[arg++]);        if (name == NULL) return JS_FALSE; __arg_error_check__("Surface")
#define arg_colormatrix(name)     CColorMatrix* name        = argColorMatrix(cx, argv[arg++]);    if (name == NULL) return JS_FALSE; __arg_error_check__("ColorMatrix")
#define arg_byte_array(name)      SS_BYTEARRAY* name        = argByteArray(cx, argv[arg++]);      if (name == NULL) return JS_FALSE; __arg_error_check__("ByteArray")
#define arg_image(name)           SS_IMAGE* name            = argImage(cx, argv[arg++]);          if (name == NULL) return JS_FALSE; __arg_error_check__("Image")
#define arg_font(name)            SS_FONT* name             = argFont(cx, argv[arg++]);           if (name == NULL) return JS_FALSE; __arg_error_check__("Font")
#define arg_sfxr(name)            SS_SFXR* name             = argSfxr(cx, argv[arg++]);           if (name == NULL) return JS_FALSE; __arg_error_check__("Sfxr")
#define arg_spriteset(name)       sSpriteset* name          = argSpriteset(cx, argv[arg++]);      if (name == NULL) return JS_FALSE; __arg_error_check__("Spriteset")
#define arg_particle_system(name) ParticleSystemBase* name  = argParticleSystem(cx, argv[arg++]); if (name == NULL) return JS_FALSE; __arg_error_check__("ParticleSystem")

// return values
#define return_null()         *rval = JSVAL_NULL
#define return_void()         *rval = JSVAL_VOID
#define return_int(expr)      *rval = INT_TO_JSVAL(expr)
#define return_bool(expr)     *rval = BOOLEAN_TO_JSVAL(expr)
#define return_object(expr)   *rval = OBJECT_TO_JSVAL(expr)
#define return_str(expr)      *rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, expr))
#define return_str_n(expr, n) *rval = STRING_TO_JSVAL(JS_NewStringCopyN(cx, expr, n))
#define return_double(expr)   *rval = DOUBLE_TO_JSVAL(JS_NewDouble(cx, expr))
#define return_jsval(expr)    *rval = expr
#define return_intOrDouble(expr)      *rval = INT_FITS_IN_JSVAL(expr)? INT_TO_JSVAL(expr) : DOUBLE_TO_JSVAL(JS_NewDouble(cx, expr))

// Sphere function implementations
////////////////////////////////////////////////////////////////////////////////
// section: version functions //
/**
    - Returns the current version of Sphere as a floating point number
      (e.g. 1.0 or 1.1)
*/
begin_func(GetVersion, 0)
return_double(GetSphereVersion());
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Returns the current Sphere version string
*/
begin_func(GetVersionString, 0)
return_str(SPHERE_VERSION);
end_func()

////////////////////////////////////////////////////////////////////////////////
// section: configuration //
/**
    - Get some global configuration options.
      Parameter 'select' can be one of these: 0: language, 1: sound, 2: networking, 3: platform
      return values for language:
        0: default
        1: English
        2: German
        3: French
        4: Dutch
        5: Japanese
        6: Chinese_simplified
        7: Russian
        8: Spanish
        9: Indonesian

      return values for sound:
        0: Sound Auto
        1: Sound On
        2: Sound Off

      return values for networking:
        0: No networking allowed
        1: Networking is allowed

      return values for platform:
        0: Mac
        1: Win32
        2: Linux
    @see GetPlayerKey for keyboard player configuration
*/
begin_func(GetGlobalConfiguration, 1)
arg_int(item);
if (item>=3)
{
#if defined(MAC)
return_int(0);
#elif defined(WIN32)
return_int(1);
#elif defined(unix)
return_int(2);
#else
return_int(-1);
#endif
}
else
return_int(GetGlobalConfig(item));
end_func()


////////////////////////////////////////////////////////////////////////////////
// section: script functions //
/**
    - Reads the script in and uses it as if it were a part of the current script.
      ex: EvaluateScript("myscript.js");
*/
begin_func(EvaluateScript, 1)
arg_str(name);

if (IsValidPath(name) == false)
{

    JS_ReportError(cx, "Invalid filename: '%s'", name);
    return JS_FALSE;
}
// read script
std::string text;
if (!This->m_Engine->GetScriptText(name, text))
{
    JS_ReportError(cx, "%s() failed: Could not load script '%s'", script_name, name);
    return JS_FALSE;
}

// increment the recursion count, checking for overflow
This->m_RecurseCount++;
if (This->m_RecurseCount > MAX_RECURSE_COUNT)
{
    JS_ReportError(cx, "%s() recursed too deeply", script_name);
    return JS_FALSE;
}

// execute!
if (!JS_EvaluateScript(cx, This->m_Global, text.c_str(), text.length(), name, 0, rval))
{
    return JS_FALSE;
}

This->m_Engine->AddEvaluatedScript(name);
This->m_RecurseCount--;
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - Reads in one of the preset system scripts for use in the current script
      ex: EvaluateSystemScript("menu.js");
*/
begin_func(EvaluateSystemScript, 1)
arg_str(name);

if (IsValidPath(name) == false)
{

    JS_ReportError(cx, "Invalid filename: '%s'", name);
    return JS_FALSE;
}
if (name[0] == '/')
{

    JS_ReportError(cx, "Invalid filename: '%s'", name);
    return JS_FALSE;
}
// read script
std::string text;
if (!This->m_Engine->GetSystemScript(name, text))
{
    JS_ReportError(cx, "EvaluateSystemScript() failed: Could not load script '%s'", name);
    return JS_FALSE;
}

// increment the recursion count, checking for overflow
This->m_RecurseCount++;
if (This->m_RecurseCount > MAX_RECURSE_COUNT)
{
    JS_ReportError(cx, "EvaluateSystemScript() recursed too deeply");
    return JS_FALSE;
}

// execute!
if (!JS_EvaluateScript(cx, This->m_Global, text.c_str(), text.length(), name, 0, rval))
{
    return JS_FALSE;
}

This->m_Engine->AddEvaluatedSystemScript(name);
This->m_RecurseCount--;

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - Reads the script in and uses it as if it were a part of the current script.
      But only if the script has not already been evaluated.
      ex: RequireScript("myscript.js");
*/
begin_func(RequireScript, 1)
arg_str(name);

if (IsValidPath(name) == false)
{

    JS_ReportError(cx, "Invalid filename: '%s'", name);
    return JS_FALSE;
}
if (!This->m_Engine->IsScriptEvaluated(name))
{
    // read script
    std::string text;
    if (!This->m_Engine->GetScriptText(name, text))
    {
        JS_ReportError(cx, "RequireScript() failed: Could not load script '%s'", name);
        return JS_FALSE;
    }

    // increment the recursion count, checking for overflow
    This->m_RecurseCount++;
    if (This->m_RecurseCount > MAX_RECURSE_COUNT)
    {
        JS_ReportError(cx, "RequireScript() recursed too deeply");
        return JS_FALSE;
    }

    // execute!
    if (!JS_EvaluateScript(cx, This->m_Global, text.c_str(), text.length(), name, 0, rval))
    {
        return JS_FALSE;
    }

    This->m_Engine->AddEvaluatedScript(name);
    This->m_RecurseCount--;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - Reads in one of the preset system scripts for use in the current script.
      But only if the script has not already been evaluated.
      ex: RequireSystemScript("menu.js");
*/
begin_func(RequireSystemScript, 1)
arg_str(name);

if (IsValidPath(name) == false)
{

    JS_ReportError(cx, "Invalid filename: '%s'", name);
    return JS_FALSE;
}
if (name[0] == '/')
{

    JS_ReportError(cx, "Invalid filename: '%s'", name);
    return JS_FALSE;
}
if (!This->m_Engine->IsSystemScriptEvaluated(name))
{
    // read script
    std::string text;
    if (!This->m_Engine->GetSystemScript(name, text))
    {
        JS_ReportError(cx, "RequireSystemScript() failed: Could not load script '%s'", name);
        return JS_FALSE;
    }

    // increment the recursion count, checking for overflow
    This->m_RecurseCount++;
    if (This->m_RecurseCount > MAX_RECURSE_COUNT)
    {
        JS_ReportError(cx, "RequireSystemScript() recursed too deeply");
        return JS_FALSE;
    }

    // execute!
    if (!JS_EvaluateScript(cx, This->m_Global, text.c_str(), text.length(), name, 0, rval))
    {
        return JS_FALSE;
    }

    This->m_Engine->AddEvaluatedSystemScript(name);
    This->m_RecurseCount--;
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - invokes the JavaScript garbage collector
      if given a parameter, then Sphere will decide if GC is required.
*/
begin_func(GarbageCollect, 0)
if (argc >= 1)
    JS_MaybeGC(cx);
else
    JS_GC(cx);
end_func()

////////////////////////////////////////////////////////////////////////////////
// section: misc functions //
/**
    - creates a single-character string based on the code passed in, i.e. 65 is "A"

      Do not use this function, use String.fromCharCode(code) instead...
      e.g. String.fromCharCode(65) == "A"
*/
begin_func(CreateStringFromCode, 1)
arg_int(code);
jschar c[2] = { (jschar)code, 0 };
*rval = STRING_TO_JSVAL(JS_NewUCStringCopyZ(cx, c));
return JS_TRUE;
end_func()

////////////////////////////////////////////////////////////////////////////////
// section: engine functions //
/**
    - Returns array of game objects
      - game.name          Name of game
      - game.directory     Directory (project name) where game is stored
      - game.author        Who wrote it?
      - game.description   Bite-sized summary of game.
*/
begin_func(GetGameList, 0)

// get the list of games
std::vector<Game> games;
This->m_Engine->GetGameList(games);

static JSClass clasp =
    {
        "object", 0,
        JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
        JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    };

int array_size = games.size();
jsval* array = new jsval[array_size];
if (!array)
{
    return JS_FALSE;
}
*rval = *array; // temporally root it to the return value...

for (int i = 0; i < array_size; i++)
{
    JSObject* element = JS_NewObject(cx, &clasp, NULL, NULL);
    array[i] = OBJECT_TO_JSVAL(element); // adding the object as property to a rooted jsval

    // define the 'name' property
    jsval name_val = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, games[i].name.c_str()));
    JS_DefineProperty(
        cx, element, "name", name_val,
        JS_PropertyStub, JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    // define the 'directory' property
    jsval dir_val  = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, games[i].directory.c_str()));
    JS_DefineProperty(
        cx, element, "directory", dir_val,
        JS_PropertyStub, JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    // define the 'author' property
    jsval auth_val = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, games[i].author.c_str()));
    JS_DefineProperty(
        cx, element, "author", auth_val,
        JS_PropertyStub, JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    // define the 'description' property
    jsval desc_val = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, games[i].description.c_str()));
    JS_DefineProperty(
        cx, element, "description", desc_val,
        JS_PropertyStub, JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);
}

// create the array
JSObject* object = JS_NewArrayObject(cx, array_size, array);
return_object(object);
delete[] array;

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - executes the game in sphere/games/<directory>.  This function
      actually exits the first game and loads the one in 'directory'.
      When the second game returns, the original is loaded again.
      (Note that this is unlike Sphere 0.97, which returned directly
      from ExecuteGame when the other game finished.)
*/
begin_func(ExecuteGame, 1)
arg_str(directory);

This->m_Engine->ExecuteGame(directory);
This->m_Error = "";
return JS_FALSE;
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Exits the Sphere engine unconditionally
*/
begin_func(Exit, 0)

// close the map engine
if (This->m_Engine->GetMapEngine()->IsRunning())
    This->m_Engine->GetMapEngine()->Exit();

This->m_ShouldExit = true;
This->m_Error = "";  // don't report an error (there is none)

return JS_FALSE;
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Exits the Sphere engine unconditionally,

      displays the 'message' to the user
      If you end the message with a newline it wont display the file/line

      where the Abort occured.
      e.g. Abort("Mistake here")
      vs. Abort("Game over!\n");
*/
begin_func(Abort, 1)
arg_str(message);

// close the map engine
if (This->m_Engine->GetMapEngine()->IsRunning())
    This->m_Engine->GetMapEngine()->Exit();

This->m_ShouldExit = true;
JS_ReportError(cx, "%s", message);

return JS_FALSE;
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - restarts the current game
*/
begin_func(RestartGame, 0)
This->m_Engine->RestartGame();
This->m_Error = "";

// close the map engine
if (This->m_Engine->GetMapEngine()->IsRunning())
    This->m_Engine->GetMapEngine()->Exit();

return JS_FALSE;
end_func()

////////////////////////////////////////////////////////////////////////////////
// section: graphics functions //
/**
    - displays the contents from the video buffer onto the screen.

      Then the video buffer is cleared.
      You *need* to call this to make anything
      you've drawn in code to appear on the screen.
*/
begin_func(FlipScreen, 0)

// Framerate disabled, FlipScreen as fast as possible
if (This->m_FrameRate == 0)
{

    FlipScreen();
    ClearScreen();

}
else
{

    // never skip more than MAX_FRAME_SKIP frames
    if (This->ShouldRender() || This->m_FramesSkipped >= MAX_FRAME_SKIP)
    {
        FlipScreen();
        ClearScreen();

        This->m_FramesSkipped = 0;
    }
    else
        This->m_FramesSkipped++;

    if (GetTime() * This->m_FrameRate < (dword)This->m_IdealTime)
    {
        This->m_ShouldRender = true;

        // delay!
        while (GetTime() * This->m_FrameRate < (dword)This->m_IdealTime)
        { }

    }
    else
        This->m_ShouldRender = false;

    // update timing variables
    This->m_IdealTime += 1000;

}
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - Sets a clipping rectangle of width w and height h at (x, y) into the
      video buffer. Anything drawn outside the rectangle is not drawn into
      the video buffer.
*/
begin_func(SetClippingRectangle, 4)
arg_int(x);
arg_int(y);
arg_int(w);
arg_int(h);
SetClippingRectangle(x, y, w, h);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Returns a rectangle object representing the clipping rectangle.
      i.e.
      var clip = GetClippingRectangle();
          clip.x
          clip.y
          clip.width
          clip.height
*/
begin_func(GetClippingRectangle, 0)

static JSClass base_clasp =
    {
        "base", 0,
        JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
        JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    };

JSObject* clip_obj = JS_NewObject(cx, &base_clasp, NULL, NULL);
*rval = OBJECT_TO_JSVAL(clip_obj); // if (!clip_obj || !JS_AddRoot(cx, &clip_obj)) return NULL;

int x, y, w, h;
GetClippingRectangle(&x, &y, &w, &h);

JS_DefineProperty(cx, clip_obj, "x",      INT_TO_JSVAL(x), JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);
JS_DefineProperty(cx, clip_obj, "y",      INT_TO_JSVAL(y), JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);
JS_DefineProperty(cx, clip_obj, "width",  INT_TO_JSVAL(w),  JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);
JS_DefineProperty(cx, clip_obj, "height", INT_TO_JSVAL(h),  JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

return_object(clip_obj);
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - fills the whole screen with the color specified. Note that the color
      passed must have an alpha that is less than 255. Otherwise, it'll
      just make the screen solidly that color.

      ApplyColorMask tints the screen's current state
      (meaning you'll have to call it every frame
       if you want the effect to be permanent)
*/
begin_func(ApplyColorMask, 1)
arg_color(c);
if (This->ShouldRender())
{
    ApplyColorMask(c);
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - allows you to set the maximum frames rendered per second that
      the engine is allowed to draw at most.  Set to 0 in order to
      unthrottle the graphics renderer.  Keep in mind that this
      is not for the map engine, which uses the fps specified in the
      MapEngine() call.  This function only controls standard drawing
      functions and FlipScreen() outside of the map engine.  In short,
      don't use this function if you plan to be doing rendering in your
      own scripts in the map engine.
*/
begin_func(SetFrameRate, 1)
arg_int(fps);
// 1 fps is the lowest we can throttle
if (fps < 1)
{
    This->m_ShouldRender  = true;
    This->m_FrameRate     = 0;
    This->m_FramesSkipped = 0;
    This->m_IdealTime     = 0;
}
else
{
    This->m_ShouldRender  = true;
    This->m_FrameRate     = fps;
    This->m_FramesSkipped = 0;
    This->m_IdealTime     = GetTime() * fps + 1000;
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Returns the current fps set by SetFrameRate...

    (note: this is not the same as the map engine frame rate)
*/
begin_func(GetFrameRate, 0)

bool calculate = false;
int frame_rate = This->m_FrameRate;
if (argc >= 1)
{

    calculate = argBool(cx, argv[0]);
    if (calculate)
    {
        // TODO
    }
}
return_int(frame_rate);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Sets the max frame rate (in frames per second) that the map engine will go at
*/
begin_func(SetMapEngineFrameRate, 1)
arg_int(fps);
if (!This->m_Engine->GetMapEngine()->SetMapEngineFrameRate(fps))
{
    This->ReportMapEngineError("SetMapEngineFrameRate() failed");

    return JS_FALSE;
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Returns the current map engine frames per second rate set by either MapEngine(map, fps) or SetMapEngineFrameRate(fps)
*/
begin_func(GetMapEngineFrameRate, 0)
int fps = This->m_Engine->GetMapEngine()->GetMapEngineFrameRate();

if (fps == 0)
{

    This->ReportMapEngineError("GetMapEngineFrameRate() failed");
    return JS_FALSE;
}
return_int(This->m_Engine->GetMapEngine()->GetMapEngineFrameRate());
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - returns the width of the engine screen
*/
begin_func(GetScreenWidth, 0)
return_int(GetScreenWidth());
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns the height of the engine screen
*/
begin_func(GetScreenHeight, 0)
return_int(GetScreenHeight());
end_func()

////////////////////////////////////////////////////////////////////////////////
inline VECTOR_INT* getObjCoordinates(JSContext* cx, jsval arg)
{

    if (!JSVAL_IS_OBJECTNOTNULL(arg))
    {
        return NULL;
    }

    JSObject* object;

    if (!JS_ValueToObject(cx, arg, &object) || !object || !JS_AddRoot(cx, &object))
        return NULL;

    VECTOR_INT* point = new VECTOR_INT;

    // get the x-coordinate
    JS_LookupProperty(cx, object, "x", &arg);
    if (arg == JSVAL_VOID)
    {
        delete point;
        return NULL;
    }
    point->x = argInt(cx, arg);

    // get the y-coordinate
    JS_LookupProperty(cx, object, "y", &arg);
    if (arg == JSVAL_VOID)
    {
        delete point;
        return NULL;
    }
    point->y = argInt(cx, arg);
    JS_RemoveRoot(cx, &object);
    return point;
}

////////////////////////////////////////////////////////////////////////////////
/**
    - plots a point onto the video buffer at (x, y) with the color
*/
begin_func(Point, 3)
if (This->ShouldRender())
{
    arg_int(x);
    arg_int(y);
    arg_color(c);

    DrawPoint(x, y, c);
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - plots a series of points onto the video buffer with the color
*/
begin_func(PointSeries, 2)
if (This->ShouldRender())
{
    arg_array(arr);
    arg_color(c);

    jsval  v;
    jsval* vp = &v;
    jsuint length;

    JS_GetArrayLength(cx, arr, &length);

    if (length < 1)
    {
        JS_ReportError(cx, "PointSeries() failed: Not enough points in array");
        return JS_FALSE;
    }

    VECTOR_INT** points = new VECTOR_INT*[length];

    for (unsigned int i = 0; i < length; i++)
    {
        JS_GetElement(cx, arr, i, vp); // not sure if we need rooting, getObjCoordinates at least roots argv

        points[i] = getObjCoordinates(cx, v);

        if (points[i] == NULL)
        {
            JS_ReportError(cx, "PointSeries() failed: Invalid object at array index %d", i);
            for (unsigned int i = 0; i < length; i++) delete points[i];
            delete [] points;
            return JS_FALSE;
        }
    }

    DrawPointSeries(points, length, c);
    for (unsigned int i = 0; i < length; i++)
        delete points[i];
    delete [] points;
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - draws a line from (x1, y1) to (x2, y2) with the color
*/
begin_func(Line, 5)
if (This->ShouldRender())
{
    arg_int(x1);
    arg_int(y1);
    arg_int(x2);
    arg_int(y2);
    arg_color(c);

    int x[2] = { x1, x2 };
    int y[2] = { y1, y2 };
    DrawLine(x, y, c);
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Draws a line from (x1, y1) to (x2, y2) with a color fade from color1
      to color2
*/
begin_func(GradientLine, 6)
if (This->ShouldRender())
{
    arg_int(x1);
    arg_int(y1);
    arg_int(x2);
    arg_int(y2);
    arg_color(c1);
    arg_color(c2);

    int x[2] = { x1, x2 };
    int y[2] = { y1, y2 };
    RGBA c[2];
    c[0] = c1;
    c[1] = c2;
    DrawGradientLine(x, y, c);
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Draws a series of lines with the color
    - type = 0 = LINE_MULTIPLE: line list
    - type = 1 = LINE_STRIP: line strip
    - type = 2 = LINE_LOOP: line loop
*/
begin_func(LineSeries, 2)
if (This->ShouldRender())
{
    arg_array(arr);
    arg_color(c);

    int type = 0;
    if (argc >= 3)
    {
        type = argInt(cx, argv[2]);
        if (type < 0)
        {
            type = 0;
        }
        else if (type > 2)
        {
            type = 2;
        }
    }

    jsval  v;
    jsval* vp = &v;
    jsuint length;

    JS_GetArrayLength(cx, arr, &length);

    if (length < 2)
    {
        JS_ReportError(cx, "LineSeries() failed: Not enough points in array");
        return JS_FALSE;
    }
    if (type == 0 && length % 2)
    {
        length--;
    }
    if (type == 2 && length < 3)
    {
        type = 0;
    }

    VECTOR_INT** points = new VECTOR_INT*[length];

    for (unsigned int i = 0; i < length; i++)
    {
        JS_GetElement(cx, arr, i, vp);

        points[i] = getObjCoordinates(cx, v);

        if (points[i] == NULL)
        {
            JS_ReportError(cx, "LineSeries() failed: Invalid object at array index %d", i);
            for (unsigned int i = 0; i < length; i++) delete points[i];
            delete [] points;
            return JS_FALSE;
        }
    }

    DrawLineSeries(points, length, c, type);
    for (unsigned int i = 0; i < length; i++)
        delete points[i];
    delete [] points;
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - draws a Bezier curve from A(x1, y1) to C(x3, y3) with B(x2, y2) and D(x4, y4) as control points
*/
begin_func(BezierCurve, 8)
if (This->ShouldRender())
{
    arg_color(c);
    arg_double(step);
    arg_int(x1);
    arg_int(y1);
    arg_int(x2);
    arg_int(y2);
    arg_int(x3);
    arg_int(y3);
    int x4 = 0;
    int y4 = 0;
    int cubic = 0;
    if (argc >= 10)
    {
        x4 = argInt(cx, argv[8]);
        y4 = argInt(cx, argv[9]);
        cubic = 1;
    }
    int x[4] = { x1, x2, x3, x4 };
    int y[4] = { y1, y2, y3, y4 };
    DrawBezierCurve(x, y, step, c, cubic);
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Draws a filled triangle with the points (x1, y1), (x2, y2), (x3, y3),
      with the color c
*/
begin_func(Triangle, 7)
if (This->ShouldRender())
{
    arg_int(x1);
    arg_int(y1);
    arg_int(x2);
    arg_int(y2);
    arg_int(x3);
    arg_int(y3);
    arg_color(c);

    int x[3] = { x1, x2, x3 };
    int y[3] = { y1, y2, y3 };
    DrawTriangle(x, y, c);
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Draws a gradient triangle with the points (x1, y1), (x2, y2), (x3, y3),
      with each point (c1 = color of (x1, y1), c2 = color of (x2, y2), c3 = color
      of (x3, y3)) having a color to generate the gradient of the triangle
*/
begin_func(GradientTriangle, 9)
if (This->ShouldRender())
{
    arg_int(x1);
    arg_int(y1);
    arg_int(x2);
    arg_int(y2);
    arg_int(x3);
    arg_int(y3);
    arg_color(c1);
    arg_color(c2);
    arg_color(c3);

    int x[3] = { x1, x2, x3 };
    int y[3] = { y1, y2, y3 };
    RGBA c[3];
    c[0] = c1;
    c[1] = c2;
    c[2] = c3;
    DrawGradientTriangle(x, y, c);
}
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - Determines if two polygons are colliding
      It returns 0 when there is no collision, a positive number if a vertex of A 
      collides with B and a negative number is a vertex of B collides with A.
      1 and -1 is the first point in the array, and so on. Because the first element
      in an array is zero, you need to substract or add 1 to get the element number.
*/
begin_func(PolygonCollision, 2)
    arg_array(arrA);
    arg_array(arrB);
	int offsetAx = 0;
	int offsetAy = 0;
	int offsetBx = 0;
	int offsetBy = 0;
	if (argc > 2) 
	{
		offsetAx = argInt(cx, argv[2]);
	}
	if (argc > 3)
	{
		offsetAy = argInt(cx, argv[3]);
	}
	if (argc > 4)
	{
		offsetBx = argInt(cx, argv[4]);
	}
	if (argc > 5)
	{
		offsetBy = argInt(cx, argv[5]);
	}

    jsval  v;
    jsval* vp = &v;
    jsuint lengthA;
    jsuint lengthB;

    JS_GetArrayLength(cx, arrA, &lengthA);
    JS_GetArrayLength(cx, arrB, &lengthB);

    if (lengthA < 3)
    {
        JS_ReportError(cx, "PolygonCollision() failed: Not enough points in first array");
        return JS_FALSE;
    }
    if (lengthB < 3)
    {
        JS_ReportError(cx, "PolygonCollision() failed: Not enough points in second array");
        return JS_FALSE;
    }

    // Copy our array-of-objects data over to vectors
    VECTOR_INT** pointsA = new VECTOR_INT*[lengthA];
    VECTOR_INT** pointsB = new VECTOR_INT*[lengthB];

    for (unsigned int i = 0; i < lengthA; i++)
    {
        JS_GetElement(cx, arrA, i, vp);
        pointsA[i] = getObjCoordinates(cx, v);
        pointsA[i]->x += offsetAx;
        pointsA[i]->y += offsetAy;

        if (pointsA[i] == NULL)
        {
            JS_ReportError(cx, "PolygonCollision() failed: Invalid object at first array index %d", i);
            for (unsigned int i = 0; i < lengthA; i++) delete pointsA[i];
            for (unsigned int i = 0; i < lengthB; i++) delete pointsB[i];
            delete [] pointsA;
            delete [] pointsB;
            return JS_FALSE;
        }
    }
    for (unsigned int i = 0; i < lengthB; i++)
    {
        JS_GetElement(cx, arrB, i, vp);
        pointsB[i] = getObjCoordinates(cx, v);
        pointsB[i]->x += offsetBx;
        pointsB[i]->y += offsetBy;

        if (pointsB[i] == NULL)
        {
            JS_ReportError(cx, "PolygonCollision() failed: Invalid object at second array index %d", i);
            for (unsigned int i = 0; i < lengthA; i++) delete pointsA[i];
            for (unsigned int i = 0; i < lengthB; i++) delete pointsB[i];
            delete [] pointsA;
            delete [] pointsB;
            return JS_FALSE;
        }
    }

    // Code from: http://www.visibone.com/inpoly/inpoly.c.txt
    signed int isinside = 0;
    int inside = 0;
    int xold = 0;
    int xnew = 0;
    int yold = 0;
    int ynew = 0;
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;

    // For each point int pointsA, we will check point-in-poly in polygon pointsB
    for (unsigned int iA = 0; iA<lengthA; ++iA){
        xold = pointsB[lengthB-1]->x;
        yold = pointsB[lengthB-1]->y;
        for (unsigned int i=0 ; i < lengthB ; i++) {
            xnew = (int)pointsB[i]->x;
            ynew = (int)pointsB[i]->y;
            if (xnew > xold) {
                x1 = xold;
                x2 = xnew;
                y1 = yold;
                y2 = ynew;
            } else {
                x1 = xnew;
                x2 = xold;
                y1 = ynew;
                y2 = yold;
            }
            if ((xnew < pointsA[iA]->x) == (pointsA[iA]->x <= xold)  // edge "open" at left end
                && ((long)pointsA[iA]->y-(long)y1)*(long)(x2-x1)
                < ((long)y2-(long)y1)*(long)(pointsA[iA]->x-x1))
            {
                inside=!inside; // I nod and smile...
            }
            xold = xnew;
            yold = ynew;
        }

        if (inside){
            isinside = iA+1;
            break;
        }
    }

    // No need to check for the other one if we already know that we collided
    if (isinside == 0) {
    // For each point int pointsB, we will check point-in-poly in polygon pointsA
    for (unsigned int iB = 0; iB<lengthB; ++iB){
        xold = pointsA[lengthA-1]->x;
        yold = pointsA[lengthA-1]->y;
        for (unsigned int i=0 ; i < lengthA ; i++) {
            xnew = (int)pointsA[i]->x;
            ynew = (int)pointsA[i]->y;
            if (xnew > xold) {
                x1 = xold;
                x2 = xnew;
                y1 = yold;
                y2 = ynew;
            } else {
                x1 = xnew;
                x2 = xold;
                y1 = ynew;
                y2 = yold;
            }
            if ((xnew < pointsB[iB]->x) == (pointsB[iB]->x <= xold)  // edge "open" at left end
                && ((long)pointsB[iB]->y-(long)y1)*(long)(x2-x1)
                < ((long)y2-(long)y1)*(long)(pointsB[iB]->x-x1))
            {
                inside=!inside; // I nod and smile again...
            }
            xold = xnew;
            yold = ynew;
        }

        if (inside){
            isinside = -1 - iB;
            break;
        }
    }
    }

	for (unsigned int i = 0; i < lengthA; i++)
        delete pointsA[i];
    for (unsigned int i = 0; i < lengthB; i++)
        delete pointsB[i];
    delete [] pointsA;
    delete [] pointsB;
    return_int(isinside);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Draws a color-filled polygon using the array of objects
      (each object must have a 'x' and 'y' property).
      If invert is true, all points in the bounding box of the polygon, but
      not in the polygon will be colored.
*/
begin_func(Polygon, 2)
if (This->ShouldRender())
{
    arg_array(arr);
    arg_color(c);

    int invert = 0;
    if (argc > 2)
    {
        invert = argInt(cx, argv[2]);
        if (invert != 0 && invert != 1)
        {
            invert = 0;
        }
    }
	int offsetX = 0;
	int offsetY = 0;
    if (argc > 3)
    {
        offsetX = argInt(cx, argv[3]);
	}
    if (argc > 4)
    {
        offsetY = argInt(cx, argv[4]);
	}

    jsval  v;
    jsval* vp = &v;
    jsuint length;

    JS_GetArrayLength(cx, arr, &length);

    if (length < 3)
    {
        JS_ReportError(cx, "Polygon() failed: Not enough points in array");
        return JS_FALSE;
    }

    VECTOR_INT** points = new VECTOR_INT*[length];

    for (unsigned int i = 0; i < length; i++)
    {
        JS_GetElement(cx, arr, i, vp);

        points[i] = getObjCoordinates(cx, v);

        if (points[i] == NULL)
        {
            JS_ReportError(cx, "Polygon() failed: Invalid object at array index %d", i);
            for (unsigned int i = 0; i < length; i++) delete points[i];
            delete [] points;
            return JS_FALSE;
        }
    }

    if(offsetX !=0 || offsetY !=0){
        for (unsigned int i = 0; i < length; i++)
        {
            points[i]->x += offsetX;
            points[i]->y += offsetY;
		}
    }

    DrawPolygon(points, length, invert, c);
    for (unsigned int i = 0; i < length; i++)
        delete points[i];
    delete [] points;
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Draws an outlined rectangle at (x, y) of width w and height h and size size, filled with color c.
*/
begin_func(OutlinedRectangle, 5)
if (This->ShouldRender())
{
    arg_int(x);
    arg_int(y);
    arg_int(w);
    arg_int(h);
    arg_color(c);
    int size = 1;
    if (argc >= 6)
    {
        size = argInt(cx, argv[5]);
        if (size < 0)
        {
            size = 1;
        }
    }
    if (size > h / 2)
    {
        DrawRectangle(x, y, w, h, c);
    }
    else
    {
        DrawOutlinedRectangle(x, y, w, h, size, c);
    }
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Draws a rectangle at (x, y) of width w and height h, filled with color c.
*/
begin_func(Rectangle, 5)
if (This->ShouldRender())
{
    arg_int(x);
    arg_int(y);
    arg_int(w);
    arg_int(h);
    arg_color(c);

    DrawRectangle(x, y, w, h, c);
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Draws a gradient rectangle at (x,y) with the height h and width w.
      Each corner of a rectangle (c_ul = color of upper left corner,
      c_ur = color of upper right corner, c_lr = color of lower right corner,
      c_ll = color of lower left corner) accepts a color information to
      generate the gradient of the rectangle.
*/
begin_func(GradientRectangle, 8)
if (This->ShouldRender())
{
    arg_int(x);
    arg_int(y);
    arg_int(w);
    arg_int(h);
    arg_color(c1);
    arg_color(c2);
    arg_color(c3);
    arg_color(c4);

    RGBA c[4];
    c[0] = c1;
    c[1] = c2;
    c[2] = c3;
    c[3] = c4;
    DrawGradientRectangle(x, y, w, h, c);
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Draws an outlined ellipse at (x, y) with radius rx and ry
*/
begin_func(OutlinedEllipse, 5)
if (This->ShouldRender())
{
    arg_int(x);
    arg_int(y);
    arg_int(rx);
    arg_int(ry);
    arg_color(c);

    DrawOutlinedEllipse(x, y, rx, ry, c);
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Draws a filled ellipse at (x, y) with radius rx and ry
*/
begin_func(FilledEllipse, 5)
if (This->ShouldRender())
{
    arg_int(x);
    arg_int(y);
    arg_int(rx);
    arg_int(ry);
    arg_color(c);

    DrawFilledEllipse(x, y, rx, ry, c);
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Draws a filled rectangle at (r_x, r_y) with width r_w and height r_h and a circle cut out
    of it at (circ_x, circ_y) with a radius of circ_r.
*/
begin_func(OutlinedComplex, 8)
if (This->ShouldRender())
{
    arg_int(r_x);
    arg_int(r_y);
    arg_int(r_w);
    arg_int(r_h);
    arg_int(circ_x);
    arg_int(circ_y);
    arg_int(circ_r);
    arg_color(color);

    int antialias = 0;
    if (argc >= 9)
    {
        antialias = argInt(cx, argv[8]);
    }
    DrawOutlinedComplex(r_x, r_y, r_w, r_h, circ_x, circ_y, circ_r, color, antialias);
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Draws a filled rectangle at (r_x, r_y) with width r_w and height r_h and a circle
    at (circ_x, circ_y) with a radius of circ_r (fractioned, if defined)
*/
begin_func(FilledComplex, 12)
if (This->ShouldRender())
{
    arg_int(r_x);
    arg_int(r_y);
    arg_int(r_w);
    arg_int(r_h);
    arg_int(circ_x);
    arg_int(circ_y);
    arg_int(circ_r);
    arg_double(angle);
    arg_double(frac_size);
    arg_int(fill_empty);
    arg_color(color1);
    arg_color(color2);

    float fangle = (float)(angle);
    float ffrac_size = (float)(frac_size);

    RGBA c[2];
    c[0] = color1;
    c[1] = color2;

    DrawFilledComplex(r_x, r_y, r_w, r_h, circ_x, circ_y, circ_r, fangle, ffrac_size, fill_empty, c);
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Draws a filled rectangle at (r_x, r_y) with width r_w and height r_h and a gradient circle
    at (circ_x, circ_y) with a radius of circ_r (fractioned, if defined)
*/
begin_func(GradientComplex, 13)
if (This->ShouldRender())
{
    arg_int(r_x);
    arg_int(r_y);
    arg_int(r_w);
    arg_int(r_h);
    arg_int(circ_x);
    arg_int(circ_y);
    arg_int(circ_r);
    arg_double(angle);
    arg_double(frac_size);
    arg_int(fill_empty);
    arg_color(color1);
    arg_color(color2);
    arg_color(color3);

    float fangle = (float)(angle);
    float ffrac_size = (float)(frac_size);

    RGBA c[2];
    c[0] = color1;
    c[1] = color2;
    c[2] = color3;

    DrawGradientComplex(r_x, r_y, r_w, r_h, circ_x, circ_y, circ_r, fangle, ffrac_size, fill_empty, c);
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Draws an outlined circle at (x, y) with radius r, filled with color c.
*/
begin_func(OutlinedCircle, 4)
if (This->ShouldRender())
{
    arg_int(x);
    arg_int(y);
    arg_int(r);
    arg_color(c);

    int antialias = 0;
    if (argc >= 5)
    {
        antialias = argInt(cx, argv[4]);
    }

    DrawOutlinedCircle(x, y, r, c, antialias);
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Draws a filled circle at (x, y) with radius r, filled with color c.
*/
begin_func(FilledCircle, 4)
if (This->ShouldRender())
{
    arg_int(x);
    arg_int(y);
    arg_int(r);
    arg_color(c);

    int antialias = 0;
    if (argc >= 5)
    {
        antialias = argInt(cx, argv[4]);
    }

    DrawFilledCircle(x, y, r, c, antialias);
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Draws a gradient circle at (x, y) with radius r, filled with internal color c1 and external color c2.
*/
begin_func(GradientCircle, 5)
if (This->ShouldRender())
{
    arg_int(x);
    arg_int(y);
    arg_int(r);
    arg_color(c1);
    arg_color(c2);

    int antialias = 0;
    if (argc >= 6)
    {
        antialias = argInt(cx, argv[5]);
    }

    RGBA c[2];
    c[0] = c1;
    c[1] = c2;

    DrawGradientCircle(x, y, r, c, antialias);
}
end_func()

////////////////////////////////////////////////////////////////////////////////
// section: input //
/**
    - returns true or false depending if the there are keys from the key
      input queue.
*/
begin_func(AreKeysLeft, 0)
return_bool(AreKeysLeft());
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns the first key in the queue. If there are no keys in the queue,
      Sphere will wait until there is a key in the queue.
*/
begin_func(GetKey, 0)
return_int(GetKey());
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - checks if Caps Lock, Num Lock or Scroll Lock are active.
      Allowed key values are:
        KEY_CAPSLOCK
        KEY_NUMLOCK
        KEY_SCROLLOCK (note: only two Ls)
*/
begin_func(GetToggleState, 1)
arg_int(key);

switch (key)
{
    case KEY_CAPSLOCK:  key = MODKEY_CAPSLOCK;  break;
    case KEY_NUMLOCK:   key = MODKEY_NUMLOCK;   break;
    case KEY_SCROLLOCK: key = MODKEY_SCROLLOCK; break;
    default:            key = MODKEY_NONE;      break;
}

RefreshInput();

if (key != MODKEY_NONE)
    return_bool(GetToggleState(key));
else
    return_bool(false);

end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - checks if the key has been pressed.

      Returns true if 'key' is pressed....
*/
begin_func(IsKeyPressed, 1)
arg_int(key);
RefreshInput();
return_bool(IsKeyPressed(key));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - checks if any key is pressed at the time.
*/
begin_func(IsAnyKeyPressed, 0)
RefreshInput();
return_bool(AnyKeyPressed());
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - converts the key into a string, KEY_A will become "a", etc.
    - if shift is true, returns uppercase/special value of key
    - control keys return ""
*/
begin_func(GetKeyString, 2)
arg_int(key);
arg_bool(shift);
return_str(GetKeyString(key, shift));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns the key constant associated with the configurable player key of 'player'.
      Allowed player values are 0 - 3 or:
        PLAYER_1
        PLAYER_2
        PLAYER_3
        PLAYER_4
      Allowed player_key_constant values are:
        PLAYER_KEY_MENU
        PLAYER_KEY_UP
        PLAYER_KEY_DOWN
        PLAYER_KEY_LEFT
        PLAYER_KEY_RIGHT
        PLAYER_KEY_A
        PLAYER_KEY_B
        PLAYER_KEY_X
        PLAYER_KEY_Y
*/
begin_func(GetPlayerKey, 2)
arg_int(player);
arg_int(key);

__PLAYERCONFIG__* pc = GetPlayerConfig(player);

if (!pc)
{
    JS_ReportError(cx, "GetPlayerKey() failed: Invalid player");
    return JS_FALSE;
}

switch (key)
{
    case PLAYER_KEY_MENU:  return_int(pc->key_menu);  break;
    case PLAYER_KEY_UP:    return_int(pc->key_up);    break;
    case PLAYER_KEY_DOWN:  return_int(pc->key_down);  break;
    case PLAYER_KEY_LEFT:  return_int(pc->key_left);  break;
    case PLAYER_KEY_RIGHT: return_int(pc->key_right); break;
    case PLAYER_KEY_A:     return_int(pc->key_a);     break;
    case PLAYER_KEY_B:     return_int(pc->key_b);     break;
    case PLAYER_KEY_X:     return_int(pc->key_x);     break;
    case PLAYER_KEY_Y:     return_int(pc->key_y);     break;

    default:
    {
        JS_ReportError(cx, "GetPlayerKey() failed: Invalid key");
        return JS_FALSE;
    }
}

end_func()


////////////////////////////////////////////////////////////////////////////////
/**
    - Sets the x and y of the mouse cursor
*/
begin_func(SetMousePosition, 2)
arg_int(x);
arg_int(y);
SetMousePosition(x, y);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns the x location of the mouse cursor within the engine screen
*/
begin_func(GetMouseX, 0)
return_int(GetMouseX());
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns the y location of the mouse cursor within the engine screen
*/
begin_func(GetMouseY, 0)
return_int(GetMouseY());
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns true if the button is pressed
      allowed button values are: MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE
*/
begin_func(IsMouseButtonPressed, 1)
arg_int(button);

if ( !(button == MOUSE_LEFT || button == MOUSE_RIGHT || button == MOUSE_MIDDLE) )
{

    JS_ReportError(cx, "Invalid mouse button");
    return JS_FALSE;
}
return_int(IsMouseButtonPressed(button));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns the first key in the queue for mouse wheel events
*/
begin_func(GetMouseWheelEvent, 0)
return_int(GetMouseWheelEvent());
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns the size of the queue for mouse wheel events
*/
begin_func(GetNumMouseWheelEvents, 0)
return_int(GetNumMouseWheelEvents());
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns the number of joysticks available on the system
*/
begin_func(GetNumJoysticks, 0)
return_int(GetNumJoysticks());
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns the number of axes available on this joystick
*/
begin_func(GetNumJoystickAxes, 1)
arg_int(joy);
return_int(GetNumJoystickAxes(joy));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns the current position of the 'axis' axis of the joystick 'joy'
      in normalized coordinates from -1 to 1
      Axis values can be: JOYSTICK_AXIS_X, JOYSTICK_AXIS_Y, JOYSTICK_AXIS_Z, JOYSTICK_AXIS_R
*/
begin_func(GetJoystickAxis, 2)
arg_int(joy);
arg_int(axis);
RefreshInput();
return_double(GetJoystickAxis(joy, axis));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns the number of buttons available on this joystick
*/
begin_func(GetNumJoystickButtons, 1)
arg_int(joy);
RefreshInput();
return_int(GetNumJoystickButtons(joy));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns true if the button on joystick 'joy' is pressed
*/
begin_func(IsJoystickButtonPressed, 2)
arg_int(joy);
arg_int(but);
RefreshInput();
return_bool(IsJoystickButtonPressed(joy, but));
end_func()

////////////////////////////////////////////////////////////////////////////////
// section: time //
/**
    - returns the number of milliseconds since some arbitrary time.
      ex:
       var start = GetTime();
       while (GetTime() < start + 1000) {}
*/
begin_func(GetTime, 0)
//return_int(GetTime()); // TODO: TEST intOrDouble
return_intOrDouble(GetTime());
end_func()

////////////////////////////////////////////////////////////////////////////////
// section: particle engine //
/**
    - returns a new particle system parent object
*/
begin_func(CreateParticleSystemParent, 0)
ParticleSystemParent* system = new ParticleSystemParent;
if (!system)
{
    return JS_FALSE;
}
return_object(CreateParticleSystemParentObject(cx, system));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns a new particle system child object
*/
begin_func(CreateParticleSystemChild, 1)
arg_int(size);
if (size < 0)
{
    size = 0;
}
ParticleSystemChild* system = new ParticleSystemChild(size);
if (!system)
{
    return JS_FALSE;
}
return_object(CreateParticleSystemChildObject(cx, system));
end_func()

////////////////////////////////////////////////////////////////////////////////
// section: networking //
/**
    - returns a string with the local name of your computer
*/
begin_func(GetLocalName, 0)
char name[256];
GetLocalName(name, 256);
return_str(name);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns a string with the IP address of your computer
*/
begin_func(GetLocalAddress, 0)
char name[256];
GetLocalAddress(name, 256);
return_str(name);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - attempts to open a connection to the computer specified with 'address' on 'port'
      returns a socket object
*/
begin_func(OpenAddress, 2)
arg_str(name);
arg_int(port);
NSOCKET s = OpenAddress(name, port);

if (s == NULL)
{
    return_object(JSVAL_NULL);
}
else
{
    return_object(CreateSocketObject(cx, s));
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - listens for connections on port, returns a socket object if successful
*/
begin_func(ListenOnPort, 1)
arg_int(port);

NSOCKET s = ListenOnPort(port);
if (s == NULL)
{

    return_object(JSVAL_NULL);
}
else
{

    return_object(CreateSocketObject(cx, s));
}
end_func()
////////////////////////////////////////////////////////////////////////////////
// section: byte_arrays //
/**
    - returns a ByteArray object of 'size' bytes
*/
begin_func(CreateByteArray, 1)
arg_int(size);
return_object(CreateByteArrayObject(cx, size));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns a ByteArray object from string 'string'
*/
begin_func(CreateByteArrayFromString, 1)
arg_str(str);
return_object(CreateByteArrayObject(cx, strlen(str), str));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns a ByteArray object from string 'string'
*/
begin_func(CreateStringFromByteArray, 1)
arg_byte_array(array);
return_str_n((char*)array->array, array->size);
end_func()

////////////////////////////////////////////////////////////////////////////////
// section: colors //
/**
    - returns a color object with the color r is Red, g is Green, b is Blue,
      and a is alpha (translucency of the color).
      Note + alpha of 0 = transparent, alpha of 255 = opaque
           + alpha is optional, and defaults to 255 if not specified
*/
begin_func(CreateColor, 3)
arg_int(r);
arg_int(g);
arg_int(b);

// if alpha isn't specified, default to 255
int a = 255;
if (argc >= 4)
{
    a = argInt(cx, argv[3]);
}

// range check
if (r < 0)
{
    r = 0;
}
if (r > 255)
{
    r = 255;
}
if (g < 0)
{
    g = 0;
}
if (g > 255)
{
    g = 255;
}
if (b < 0)
{
    b = 0;
}
if (b > 255)
{
    b = 255;
}
if (a < 0)
{
    a = 0;
}
if (a > 255)
{
    a = 255;
}

return_object(CreateColorObject(cx, CreateRGBA(r, g, b, a)));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns a color object that is the blended color of color c1 and c2
*/
begin_func(BlendColors, 2)
arg_color(a);
arg_color(b);
return_object(CreateColorObject(cx, CreateRGBA(
                                    (a.red   + b.red)   / 2,
                                    (a.green + b.green) / 2,
                                    (a.blue  + b.blue)  / 2,
                                    (a.alpha + b.alpha) / 2
                                )));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - blends two colors together, allowing you to specify the amount of each color
      ex:
      BlendColorsWeighted(a, b, 1, 1) // equal amounts (like BlendColors())
      BlendColorsWeighted(a, b, 1, 2) // 33% a, 66% b
*/
begin_func(BlendColorsWeighted, 4)
arg_color(a);
arg_color(b);
arg_double(w1);
arg_double(w2);

// Fixes an issue astie was having with negative weights.
if (w1 < 0) w1 = 0;
if (w2 < 0) w2 = 0;

if (w1 + w2 == 0)
{
    return_object(CreateColorObject(cx, CreateRGBA(0, 0, 0, 255)));
}
else
{
    return_object(CreateColorObject(cx, CreateRGBA(
                                        int((a.red   * w1 + b.red   * w2) / (w1 + w2)),
                                        int((a.green * w1 + b.green * w2) / (w1 + w2)),
                                        int((a.blue  * w1 + b.blue  * w2) / (w1 + w2)),
                                        int((a.alpha * w1 + b.alpha * w2) / (w1 + w2))
                                    )));
}
end_func()

////////////////////////////////////////////////////////////////////////////////
// section: mapengine //
/**
    - starts the map engine with the map specified and runs at 'fps' frames per second
*/
begin_func(MapEngine, 2)
arg_str(map);
arg_int(fps);

if (!This->m_Engine->GetMapEngine()->Execute(map, fps))
{
    This->ReportMapEngineError("MapEngine() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - changes current map

    (This clears any pending delay scripts...)
*/
begin_func(ChangeMap, 1)
arg_str(map);

if (!This->m_Engine->GetMapEngine()->ChangeMap(map))
{
    This->ReportMapEngineError("ChangeMap() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - Returns the current map, e.g. "noisyforest.rmp"
*/
begin_func(GetCurrentMap, 0)
std::string map_filename = This->m_Engine->GetMapEngine()->GetCurrentMap();
if (map_filename == "")
{
    This->ReportMapEngineError("GetCurrentMap() failed");
    return JS_FALSE;
}
return_str(map_filename.c_str());
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Exits the map engine.  Note:  This tells the map engine to shut
      down.  This does not mean the engine shuts down immediately.  You
      must wait for the original call to MapEngine() to return before you
      can start a new map engine.
*/
begin_func(ExitMapEngine, 0)

if (!This->m_Engine->GetMapEngine()->Exit())
{
    This->ReportMapEngineError("ExitMapEngine() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - Returns true if the map engine is running, false if not
*/
begin_func(IsMapEngineRunning, 0)
return_bool(This->m_Engine->GetMapEngine()->IsRunning());
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - updates map engine (state of entities, color masks, etc.)
*/
begin_func(UpdateMapEngine, 0)

if (!This->m_Engine->GetMapEngine()->Update())
{
    This->ReportMapEngineError("UpdateMapEngine() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - calls a map's script from code
    the six events are:
      SCRIPT_ON_ENTER_MAP
      SCRIPT_ON_LEAVE_MAP
      SCRIPT_ON_LEAVE_MAP_NORTH
      SCRIPT_ON_LEAVE_MAP_EAST
      SCRIPT_ON_LEAVE_MAP_SOUTH
      SCRIPT_ON_LEAVE_MAP_WEST
*/
begin_func(CallMapScript, 1)
arg_int(which);

if (!This->m_Engine->GetMapEngine()->CallMapScript(which))
{
    This->ReportMapEngineError("CallMapScript() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - set the default script that the map engine should call before calling the map's specific script
    (The default map script is called, then the map specific script is called.)
    The events are the same from CallMapScript.
    The map engine doesn't have to be on to set a default script.
*/
begin_func(SetDefaultMapScript, 2)
arg_int(which);
arg_str(script);

if (!This->m_Engine->GetMapEngine()->SetDefaultMapScript(which, script))
{
    This->ReportMapEngineError("SetDefaultMapScript() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - Renders the map into the video buffer
*/
begin_func(RenderMap, 0)
if ( !This->m_Engine->GetMapEngine()->RenderMap() )
{

    This->ReportMapEngineError("RenderMap() failed");
    return JS_FALSE;
}
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - applies a color mask to things drawn by the map engine for 'num_frames' frames
*/
begin_func(SetColorMask, 2)
arg_color(color);
arg_int(num_frames);
if (!This->m_Engine->GetMapEngine()->SetColorMask(color, num_frames))
{

    This->ReportMapEngineError("SetColorMask() failed");
    return JS_FALSE;
}
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - in 'num_frames' frames, runs 'script'
      ex: SetDelayScript(60, "ChangeMap('forest.rmp')");
      this tells the map engine to change to forest.rmp after 60 frames
*/
begin_func(SetDelayScript, 2)
arg_int(num_frames);
arg_str(script);
if (!This->m_Engine->GetMapEngine()->SetDelayScript(num_frames, script))
{

    This->ReportMapEngineError("SetDelayScript() failed");
    return JS_FALSE;
}
end_func()
//////////////////////////////////////////////////////////////////////////////
/**
    - call the default map script
      The events are the same from CallMapScript.
      The map engine doesn't have to be on to call a default script.
*/
begin_func(CallDefaultMapScript, 1)
arg_int(which);

if (!This->m_Engine->GetMapEngine()->CallDefaultMapScript(which))
{
    This->ReportMapEngineError("CallDefaultMapScript() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
// section: layers //
/**
    - get number of layers on map
    - in the following functions, layer 0 is the bottommost layer.
    - layer 1 is the next one up, etc.
*/
begin_func(GetNumLayers, 0)

int layers;
if (!This->m_Engine->GetMapEngine()->GetNumLayers(layers))
{
    This->ReportMapEngineError("GetNumLayers() failed");
    return JS_FALSE;
}

return_int(layers);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - get width of 'layer'
*/
begin_func(GetLayerWidth, 1)
arg_int(layer);

int width;
if (!This->m_Engine->GetMapEngine()->GetLayerWidth(layer, width))
{
    This->ReportMapEngineError("GetLayerWidth() failed");
    return JS_FALSE;
}

return_int(width);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - get height of 'layer'
*/
begin_func(GetLayerHeight, 1)
arg_int(layer);

int height;
if (!This->m_Engine->GetMapEngine()->GetLayerHeight(layer, height))
{
    This->ReportMapEngineError("GetLayerHeight() failed");
    return JS_FALSE;
}

return_int(height);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - set width of 'layer'
*/
begin_func(SetLayerWidth, 2)
arg_int(layer);
arg_int(width);
if (!This->m_Engine->GetMapEngine()->SetLayerWidth(layer, width))
{

    This->ReportMapEngineError("SetLayerWidth() failed");
    return JS_FALSE;
}
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - set height of 'layer'
*/
begin_func(SetLayerHeight, 2)
arg_int(layer);
arg_int(height);
if (!This->m_Engine->GetMapEngine()->SetLayerHeight(layer, height))
{

    This->ReportMapEngineError("SetLayerHeight() failed");
    return JS_FALSE;
}
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - returns the name of 'layer'
*/
begin_func(GetLayerName, 1)
arg_int(layer);

std::string name;
if (!This->m_Engine->GetMapEngine()->GetLayerName(layer, name))
{
    This->ReportMapEngineError("GetLayerName() failed");
    return JS_FALSE;
}

return_str(name.c_str());
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns true if the layer is visible
*/
begin_func(IsLayerVisible, 1)
arg_int(layer);

bool visible = false;
if (!This->m_Engine->GetMapEngine()->IsLayerVisible(layer, visible))
{
    This->ReportMapEngineError("IsLayerVisible() failed");
    return JS_FALSE;
}

return_bool(visible);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - shows 'layer' if visible == true, and hides it if visible == false

      e.g. SetLayerVisible(0, !IsLayerVisisble(0)); will toggle layer zero's visibility
*/
begin_func(SetLayerVisible, 2)
arg_int(layer);
arg_bool(visible);

if (!This->m_Engine->GetMapEngine()->SetLayerVisible(layer, visible))
{
    This->ReportMapEngineError("SetLayerVisible() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - returns true if the layer is reflective
*/
begin_func(IsLayerReflective, 1)
arg_int(layer);
bool reflective = false;

if (!This->m_Engine->GetMapEngine()->IsLayerReflective(layer, reflective))
{
    This->ReportMapEngineError("IsLayerReflective() failed");
    return JS_FALSE;
}

return_bool(reflective);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - sets whether layer should be reflective
*/
begin_func(SetLayerReflective, 2)
arg_int(layer);
arg_bool(reflective);

if (!This->m_Engine->GetMapEngine()->SetLayerReflective(layer, reflective))
{
    This->ReportMapEngineError("SetLayerReflective() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - Sets the x zoom/scale factor for the layer 'layer_index' to 'factor_x'
      e.g. SetLayerScaleFactor(0, 0.5) will make the layer zoom out to half the normal size
    @see SetPersonScaleFactor
*/
begin_func(SetLayerScaleFactorX, 2)
arg_int(layer_index);
arg_double(factor_x);
if (!This->m_Engine->GetMapEngine()->SetLayerScaleFactorX(layer_index, factor_x))
{

    This->ReportMapEngineError("SetLayerScaleFactorX() failed");
    return JS_FALSE;
}
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - Sets the y zoom/scale factor for the layer 'layer_index' to 'factor_y'
      e.g. SetLayerScaleFactor(0, 2) will make the layer zoom in to twice the normal size
    @see SetPersonScaleFactor
*/
begin_func(SetLayerScaleFactorY, 2)
arg_int(layer_index);
arg_double(factor_y);
if (!This->m_Engine->GetMapEngine()->SetLayerScaleFactorY(layer_index, factor_y))
{

    This->ReportMapEngineError("SetLayerScaleFactorY() failed");
    return JS_FALSE;
}
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - Gets the angle (in radians) for the layer 'layer_index'
      e.g. var angle = GetLayerAngle(0) will get the angle for the first layer
      An angle of 0.0 is not rotated at all.
    @see SetLayerAngle
*/
begin_func(GetLayerAngle, 1)
arg_int(layer_index);
double angle = 0.0;
if (!This->m_Engine->GetMapEngine()->GetLayerAngle(layer_index, angle))
{

    This->ReportMapEngineError("GetLayerAngle() failed");
    return JS_FALSE;
}
return_double(angle);
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - Sets the angle (in radians) for the layer 'layer_index' to 'angle'
      e.g. SetLayerAngle(0, Math.PI) will make the layer rotate slightly
    @see GetLayerAngle
*/
begin_func(SetLayerAngle, 2)
arg_int(layer_index);
arg_double(angle);
if (!This->m_Engine->GetMapEngine()->SetLayerAngle(layer_index, angle))
{

    This->ReportMapEngineError("SetLayerAngle() failed");
    return JS_FALSE;
}
end_func()
////////////////////////////////////////////////////////////////////////////////
// section: tiles //
/**
    - return number of tiles in map
*/
begin_func(GetNumTiles, 0)

int tiles;
if (!This->m_Engine->GetMapEngine()->GetNumTiles(tiles))
{
    This->ReportMapEngineError("GetNumTiles() failed");
    return JS_FALSE;
}

return_int(tiles);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - changes tile on map to 'tile'
*/
begin_func(SetTile, 4)
arg_int(x);
arg_int(y);
arg_int(layer);
arg_int(tile);

if (!This->m_Engine->GetMapEngine()->SetTile(x, y, layer, tile))
{
    This->ReportMapEngineError("SetTile() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - returns tile on map
*/
begin_func(GetTile, 3)
arg_int(x);
arg_int(y);
arg_int(layer);

int tile;
if (!This->m_Engine->GetMapEngine()->GetTile(x, y, layer, tile))
{
    This->ReportMapEngineError("GetTile() failed");
    return JS_FALSE;
}

return_int(tile);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns the name of the tile 'tile_index'
*/
begin_func(GetTileName, 1)
arg_int(tile_index);

std::string name;
if (!This->m_Engine->GetMapEngine()->GetTileName(tile_index, name))
{
    This->ReportMapEngineError("GetTileName() failed");
    return JS_FALSE;
}

return_str(name.c_str());
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns width in pixels of tiles on current map
*/
begin_func(GetTileWidth, 0)

int width;
if (!This->m_Engine->GetMapEngine()->GetTileWidth(width))
{
    This->ReportMapEngineError("GetTileWidth() failed");
    return JS_FALSE;
}

return_int(width);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns height in pixels of tiles on current map
*/
begin_func(GetTileHeight, 0)

int height;
if (!This->m_Engine->GetMapEngine()->GetTileHeight(height))
{
    This->ReportMapEngineError("GetTileHeight() failed");
    return JS_FALSE;
}

return_int(height);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns the image of the tile 'tile_index'
*/
begin_func(GetTileImage, 1)
arg_int(tile);
IMAGE image;

if (!This->m_Engine->GetMapEngine()->GetTileImage(tile, image))
{
    This->ReportMapEngineError("GetTileImage() failed");
    return JS_FALSE;
}

return_object(CreateImageObject(cx, image, true));
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - sets the tile 'tile_index' to the image 'image_object'
*/
begin_func(SetTileImage, 2)

arg_int(tile);
arg_image(image);

if (!This->m_Engine->GetMapEngine()->SetTileImage(tile, image->image))
{
    This->ReportMapEngineError("SetTileImage() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - returns the surface of the tile 'tile_index'
*/
begin_func(GetTileSurface, 1)
arg_int(tile);
int width = 0;
int height = 0;
if (!This->m_Engine->GetMapEngine()->GetTileWidth(width)
        || !This->m_Engine->GetMapEngine()->GetTileHeight(height))
{

    This->ReportMapEngineError("GetTileSurface() failed");
    return JS_FALSE;
}
CImage32* surface = new CImage32(width, height);
if (!surface || surface->GetWidth() != width || surface->GetHeight() != height)
{

    This->ReportMapEngineError("GetTileSurface() failed!!");
    return JS_FALSE;
}
if (!This->m_Engine->GetMapEngine()->GetTileSurface(tile, surface))
{

    delete surface;
    surface = NULL;
    This->ReportMapEngineError("GetTileSurface() failed");
    return JS_FALSE;
}
return_object(CreateSurfaceObject(cx, surface));
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - sets the tile 'tile_index' to the surface 'surface_object'
*/
begin_func(SetTileSurface, 2)
arg_int(tile);
arg_surface(surface);
if (!This->m_Engine->GetMapEngine()->SetTileSurface(tile, surface))
{

    This->ReportMapEngineError("SetTileSurface() failed");
    return JS_FALSE;
}
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - gets the animation delay of the tile 'tile'
      If it returns 0, the tile is not animated
*/
begin_func(GetTileDelay, 1)

arg_int(tile);
int delay = 0;

if ( !This->m_Engine->GetMapEngine()->GetTileDelay(tile, delay) )
{
    This->ReportMapEngineError("GetTileDelay() failed");
    return JS_FALSE;
}

return_int(delay);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - sets the animation delay of the tile 'tile' to 'delay'
      A delay of 0 is considered not animated
*/
begin_func(SetTileDelay, 2)

arg_int(tile);
arg_int(delay);

if ( !This->m_Engine->GetMapEngine()->SetTileDelay(tile, delay) )
{
    This->ReportMapEngineError("SetTileDelay() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - gets the next tile in the animation sequence of 'tile'
      Note that if the return value is 'tile' the tile is not animated.
*/
begin_func(GetNextAnimatedTile, 1)
arg_int(tile);

if ( !This->m_Engine->GetMapEngine()->GetNextAnimatedTile(tile) )
{
    This->ReportMapEngineError("GetNextAnimatedTile() failed");
    return JS_FALSE;
}

return_int(tile);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - sets the next tile in the animation sequence of 'tile' to 'new_tile'
      SetNextAnimatedTile(tile, tile) turns off the tile animation for 'tile'
*/
begin_func(SetNextAnimatedTile, 2)
arg_int(current_tile);
arg_int(next_tile);

if ( !This->m_Engine->GetMapEngine()->SetNextAnimatedTile(current_tile, next_tile) )
{
    This->ReportMapEngineError("SetNextAnimatedTile() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - Replaces all 'oldtile' tiles with 'newtile' on layer 'layer'
*/
begin_func(ReplaceTilesOnLayer, 3)
arg_int(layer);
arg_int(old_tile);
arg_int(new_tile);

if (!This->m_Engine->GetMapEngine()->ReplaceTilesOnLayer(layer, old_tile, new_tile))
{
    This->ReportMapEngineError("ReplaceTilesOnLayer() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
// section: triggers //
/**
    - Returns true if there is a trigger at map_x, map_y on layer.  map_x
      and map_y are in map (per-pixel) coordinates.
      (Currently the layer parameter is ignored, although it still must be valid.)
*/
begin_func(IsTriggerAt, 3)
arg_int(location_x);
arg_int(location_y);
arg_int(layer);

bool found;
if ( !This->m_Engine->GetMapEngine()->IsTriggerAt(location_x, location_y, layer, found) )
{
    This->ReportMapEngineError("IsTriggerAt() failed");
    return JS_FALSE;
}
return_bool(found);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - activates the trigger positioned on map_x, map_y, layer if one exists.
      map_x and map_y are in map (per-pixel) coordinates.
*/
begin_func(ExecuteTrigger, 3)
arg_int(location_x);
arg_int(location_y);
arg_int(layer);

if (!This->m_Engine->GetMapEngine()->ExecuteTrigger(location_x, location_y, layer))
{
    This->ReportMapEngineError("ExecuteTrigger() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
#if 0
/**
    [DISABLED]- returns the x value of the trigger in map (per-pixel) coordinates
*/
begin_func(GetTriggerX, 1)
arg_int(trigger_index);
int x = 0;
if (!This->m_Engine->GetMapEngine()->GetTriggerX(trigger_index, x))
{

    This->ReportMapEngineError("GetTriggerX() failed");
    return JS_FALSE;
}
return_int(x);
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    [DISABLED]- returns the x value of the trigger in map (per-pixel) coordinates
*/
begin_func(GetTriggerY, 1)
arg_int(trigger_index);
int y = 0;
if (!This->m_Engine->GetMapEngine()->GetTriggerY(trigger_index, y))
{

    This->ReportMapEngineError("GetTriggerY() failed");
    return JS_FALSE;
}
return_int(y);
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    [DISABLED]- returns the amount of triggers that there is
*/
begin_func(GetNumTriggers, 0)
int triggers = 0;
if ( !This->m_Engine->GetMapEngine()->GetNumTriggers(triggers))
{

    This->ReportMapEngineError("GetNumTriggers() failed");
    return JS_FALSE;
}
return_int(triggers);
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
- [DISABLED]best when called from inside a trigger script
      it will return the index of the trigger for which the current script
      is running
*/
begin_func(GetCurrentTrigger, 0)
int trigger = 0;
if ( !This->m_Engine->GetMapEngine()->GetCurrentTrigger(trigger))
{

    This->ReportMapEngineError("GetCurrentTrigger() failed");
    return JS_FALSE;
}
return_int(trigger);
end_func()
#endif
////////////////////////////////////////////////////////////////////////////////
// section: zones //
/**
    - returns true if there are any zones at map_x, map_y on layer
      (Currently the layer parameter is ignored, although it still must be valid.)
*/
begin_func(AreZonesAt, 3)
arg_int(location_x);
arg_int(location_y);
arg_int(layer);
bool found;
if ( !This->m_Engine->GetMapEngine()->AreZonesAt(location_x, location_y, layer, found) )
{
    This->ReportMapEngineError("AreZonesAt() failed");
    return JS_FALSE;
}

return_bool(found);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - executes all the zones that map_x, map_y, layer is within
      map_x and map_y are in map (per-pixel) coordinates.
      (Currently the layer parameter is ignored, although it still must be valid.)
*/
begin_func(ExecuteZones, 3)
arg_int(location_x);
arg_int(location_y);
arg_int(layer);

if (!This->m_Engine->GetMapEngine()->ExecuteZones(location_x, location_y, layer))
{
    This->ReportMapEngineError("ExecuteZones() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - executes the script for the zone 'zone'
*/
begin_func(ExecuteZoneScript, 1)
arg_int(zone);

if ( !This->m_Engine->GetMapEngine()->ExecuteZoneScript(zone) )
{
    This->ReportMapEngineError("ExecuteZoneScript() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - returns the amount of zones that there is
*/
begin_func(GetNumZones, 0)
int zones = 0;
if ( !This->m_Engine->GetMapEngine()->GetNumZones(zones))
{
    This->ReportMapEngineError("GetNumZones() failed");
    return JS_FALSE;
}

return_int(zones);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - best when called from inside a ZoneScript handler
      it will return the index of the zone for which the current script
      is running
*/
begin_func(GetCurrentZone, 0)
int zone = 0;
if ( !This->m_Engine->GetMapEngine()->GetCurrentZone(zone))
{
    This->ReportMapEngineError("GetCurrentZone() failed");
    return JS_FALSE;
}
return_int(zone);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - gets the x value of zone 'zone'
*/
begin_func(GetZoneX, 1)
arg_int(zone);
int x = 0;

if ( !This->m_Engine->GetMapEngine()->GetZoneX(zone, x) )
{
    This->ReportMapEngineError("GetZoneX() failed");
    return JS_FALSE;
}

return_int(x);
end_func()

///////////////////////////////////////////////////////////////////////////////
/**
    - gets the y value of zone 'zone'
*/
begin_func(GetZoneY, 1)
arg_int(zone);
int y = 0;

if ( !This->m_Engine->GetMapEngine()->GetZoneY(zone, y) )
{
    This->ReportMapEngineError("GetZoneY() failed");
    return JS_FALSE;
}

return_int(y);
end_func()

///////////////////////////////////////////////////////////////////////////////
/**
    - gets the width value of zone 'zone'
*/
begin_func(GetZoneWidth, 1)
arg_int(zone);
int w = 0;

if ( !This->m_Engine->GetMapEngine()->GetZoneWidth(zone, w) )
{
    This->ReportMapEngineError("GetZoneWidth() failed");
    return JS_FALSE;
}

return_int(w);
end_func()

///////////////////////////////////////////////////////////////////////////////
/**
    - gets the height value of zone 'zone'
*/
begin_func(GetZoneHeight, 1)
arg_int(zone);
int h = 0;

if ( !This->m_Engine->GetMapEngine()->GetZoneHeight(zone, h) )
{
    This->ReportMapEngineError("GetZoneHeight() failed");
    return JS_FALSE;
}

return_int(h);
end_func()

///////////////////////////////////////////////////////////////////////////////
/**
    - gets the layer value of zone 'zone'
*/
begin_func(GetZoneLayer, 1)
arg_int(zone);
int layer = 0;
if ( !This->m_Engine->GetMapEngine()->GetZoneLayer(zone, layer) )
{

    This->ReportMapEngineError("GetZoneLayer() failed");
    return JS_FALSE;
}
return_int(layer);
end_func()
///////////////////////////////////////////////////////////////////////////////
/**
    - sets the layer value of zone 'zone'
*/
begin_func(SetZoneLayer, 2)
arg_int(zone);
arg_int(layer);
if ( !This->m_Engine->GetMapEngine()->SetZoneLayer(zone, layer) )
{

    This->ReportMapEngineError("SetZoneLayer() failed");
    return JS_FALSE;
}
end_func()
///////////////////////////////////////////////////////////////////////////////
/**
    - sets the dimensions of zone 'zone'
*/
begin_func(SetZoneDimensions, 5)
arg_int(zone);
arg_int(x);
arg_int(y);
arg_int(width);
arg_int(height);
if ( !This->m_Engine->GetMapEngine()->SetZoneDimensions(zone, x, y, x+width, y+height) )
{
    This->ReportMapEngineError("SetZoneDimensions() failed");
    return JS_FALSE;
}
end_func()
////////////////////////////////////////////////////////////////////////////////
// section: obstruction segments functions //
/**
    - Returns the number of obstruction segments on the map...
*/
begin_func(GetNumObstructionSegments, 0)
arg_int(layer);
int num_segments = 0;
if ( !This->m_Engine->GetMapEngine()->GetNumObstructionSegments(layer, num_segments) )
{

    This->ReportMapEngineError("GetNumObstructionSegments() failed");
    return JS_FALSE;
}
return_int(num_segments);
end_func()
////////////////////////////////////////////////////////////////////////////////
// section: input //
/**
    BindKey(key, onkeydown, onkeyup)
    - runs the 'onkeydown' script when the 'key' is pressed down and runs
      'onkeyup' when the 'key' is released
      ex: BindKey(KEY_SPACE, "mode = 'in';", "mode = 'out';");
      ex: BindKey(KEY_CTRL, "OnKeyPressed(KEY_CTRL)", "OnKeyReleased(KEY_CTRL)");

      refer to keys.txt for a list of key names
*/
begin_func(BindKey, 3)
arg_int(key);
arg_str(on_down);
arg_str(on_up);

if (!This->m_Engine->GetMapEngine()->BindKey(key, on_down, on_up))
{
    This->ReportMapEngineError("BindKey() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - unbinds a bound key
*/
begin_func(UnbindKey, 1)
arg_int(key);

if (!This->m_Engine->GetMapEngine()->UnbindKey(key))
{
    This->ReportMapEngineError("UnbindKey() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - runs the 'on_down' script when the joystick 'button' is pressed down and runs
      'on_up' when the joystick button is released
*/
begin_func(BindJoystickButton, 4)
arg_int(joystick);
arg_int(button);
arg_str(on_down);
arg_str(on_up);

if (!This->m_Engine->GetMapEngine()->BindJoystickButton(joystick, button, on_down, on_up))
{
    This->ReportMapEngineError("BindJoystickButton() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    UnbindJoystickButton(joystick, button)
    - unbinds a bound joystick button
*/
begin_func(UnbindJoystickButton, 2)
arg_int(joystick);
arg_int(button);

if (!This->m_Engine->GetMapEngine()->UnbindJoystickButton(joystick, button))
{
    This->ReportMapEngineError("UnbindJoystickButton() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - makes the 'person_entity' respond to the input
      (up = KEY_UP, down = KEY_DOWN, left = KEY_LEFT, right = KEY_RIGHT)

    @see AttachPlayerInput
*/
begin_func(AttachInput, 1)
arg_str(person);

if (!This->m_Engine->GetMapEngine()->AttachInput(person))
{
    This->ReportMapEngineError("AttachInput() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - releases input from the attached person entity
    @see DetachPlayerInput

*/
begin_func(DetachInput, 0)

if (!This->m_Engine->GetMapEngine()->DetachInput())
{
    This->ReportMapEngineError("DetachInput() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - returns true if a person is attached to the input
    @see IsPlayerInputAttached

*/
begin_func(IsInputAttached, 0)

bool attached;
if (!This->m_Engine->GetMapEngine()->IsInputAttached(attached))
{
    This->ReportMapEngineError("IsInputAttached() failed");
    return JS_FALSE;
}

return_bool(attached);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns a string with the name of the person who currently holds input
*/
begin_func(GetInputPerson, 0)

std::string person;
if (!This->m_Engine->GetMapEngine()->GetInputPerson(person))
{
    This->ReportMapEngineError("GetInputPerson() failed");
    return JS_FALSE;
}

return_str(person.c_str());
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - makes the 'person_entity' respond to the input
      Keys vary based on player_index (0,1,2 or 3)
      for now, only the up/down/left/right keys work. keyA, keyB, keyX and keyY are inoperative.
*/
begin_func(AttachPlayerInput, 2)
arg_str(name);
arg_int(player_index);
if (!This->m_Engine->GetMapEngine()->AttachPlayerInput(name, player_index))
{

    This->ReportMapEngineError("AttachPlayerInput() failed");
    return JS_FALSE;
}
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - releases input from the attached player entity
*/
begin_func(DetachPlayerInput, 1)
arg_str(name);
if (!This->m_Engine->GetMapEngine()->DetachPlayerInput(name))
{

    This->ReportMapEngineError("DetachPlayerInput() failed");
    return JS_FALSE;
}
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - calls 'script' after each frame (don't draw stuff in here!)
*/
begin_func(SetUpdateScript, 1)
arg_str(script);

if (!This->m_Engine->GetMapEngine()->SetUpdateScript(script))
{
    This->ReportMapEngineError("SetUpdateScript() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - calls 'script' after all map layers are rendered
*/
begin_func(SetRenderScript, 1)
arg_str(script);

if (!This->m_Engine->GetMapEngine()->SetRenderScript(script))
{
    This->ReportMapEngineError("SetRenderScript() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - calls the rendering 'script' after 'layer' has been rendered.
      Only one rendering script can be used for each layer of the map
*/
begin_func(SetLayerRenderer, 2)
arg_int(layer);
arg_str(script);

if (!This->m_Engine->GetMapEngine()->SetLayerRenderer(layer, script))
{
    This->ReportMapEngineError("SetLayerRenderer() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - set the mask of 'layer' to 'mask'

      e.g. SetLayerMask(0, CreateColor(255, 0, 0, 128)); will make the layer semi red
*/
begin_func(SetLayerMask, 2)
arg_int(layer);
arg_color(mask);

if (!This->m_Engine->GetMapEngine()->SetLayerMask(layer, mask))
{
    This->ReportMapEngineError("SetLayerMask() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - get the mask currently being used by 'layer'
*/
begin_func(GetLayerMask, 1)
arg_int(layer);

RGBA mask;
if (!This->m_Engine->GetMapEngine()->GetLayerMask(layer, mask))
{
    This->ReportMapEngineError("GetLayerMask() failed");
    return JS_FALSE;
}

return_object(CreateColorObject(cx, mask));
end_func()

////////////////////////////////////////////////////////////////////////////////
// section: camera functions //
/**
    - Attaches the camera view to specified person
*/
begin_func(AttachCamera, 1)
arg_str(person);

if (!This->m_Engine->GetMapEngine()->AttachCamera(person))
{
    This->ReportMapEngineError("AttachCamera() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - Detaches camera so it can be controlled directly
*/
begin_func(DetachCamera, 0)

if (!This->m_Engine->GetMapEngine()->DetachCamera())
{
    This->ReportMapEngineError("DetachCamera() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - returns true if the camera is attached to a person, false if the
      camera is floating
*/
begin_func(IsCameraAttached, 0)

bool attached;
if (!This->m_Engine->GetMapEngine()->IsCameraAttached(attached))
{
    This->ReportMapEngineError("IsCameraAttached() failed");
    return JS_FALSE;
}

return_bool(attached);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns a string with the name of the person whom the camera
      is attached to
*/
begin_func(GetCameraPerson, 0)

std::string person;
if (!This->m_Engine->GetMapEngine()->GetCameraPerson(person))
{
    This->ReportMapEngineError("GetCameraPerson() failed");
    return JS_FALSE;
}

return_str(person.c_str());
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - sets the x location of the camera object on the map
      (the center of the screen if possible)
*/
begin_func(SetCameraX, 1)
arg_int(x);

if (!This->m_Engine->GetMapEngine()->SetCameraX(x))
{
    This->ReportMapEngineError("SetCameraX() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - sets the y location of the camera object on the map
      (the center of the screen if possible)
*/
begin_func(SetCameraY, 1)
arg_int(y);

if (!This->m_Engine->GetMapEngine()->SetCameraY(y))
{
    This->ReportMapEngineError("SetCameraY() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - returns the x location of the camera object on the map
      (the center of the screen if possible)
*/
begin_func(GetCameraX, 0)

int x;
if (!This->m_Engine->GetMapEngine()->GetCameraX(x))
{
    This->ReportMapEngineError("GetCameraX() failed");
    return JS_FALSE;
}

return_int(x);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns the y location of the camera object on the map
      (the center of the screen if possible)
*/
begin_func(GetCameraY, 0)

int y;
if (!This->m_Engine->GetMapEngine()->GetCameraY(y))
{
    This->ReportMapEngineError("GetCameraY() failed");
    return JS_FALSE;
}

return_int(y);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns screen coordinates of position on map
*/
begin_func(MapToScreenX, 2)
arg_int(layer);
arg_int(mx);

int sx;
if (!This->m_Engine->GetMapEngine()->MapToScreenX(layer, mx, sx))
{
    This->ReportMapEngineError("MapToScreenX() failed");
    return JS_FALSE;
}

return_int(sx);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns screen coordinates of position on map
*/
begin_func(MapToScreenY, 2)
arg_int(layer);
arg_int(my);

int sy;
if (!This->m_Engine->GetMapEngine()->MapToScreenY(layer, my, sy))
{
    This->ReportMapEngineError("MapToScreenY() failed");
    return JS_FALSE;
}

return_int(sy);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns map coordinates of position on screen
*/
begin_func(ScreenToMapX, 2)
arg_int(layer);
arg_int(sx);

int mx;
if (!This->m_Engine->GetMapEngine()->ScreenToMapX(layer, sx, mx))
{
    This->ReportMapEngineError("ScreenToMapX() failed");
    return JS_FALSE;
}

return_int(mx);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns map coordinates of position on screen
*/
begin_func(ScreenToMapY, 2)
arg_int(layer);
arg_int(sy);

int my;
if (!This->m_Engine->GetMapEngine()->ScreenToMapY(layer, sy, my))
{
    This->ReportMapEngineError("ScreenToMapY() failed");
    return JS_FALSE;
}

return_int(my);
end_func()

////////////////////////////////////////////////////////////////////////////////
// section: person functions //
/**
    - returns an array of strings representing the current person entities
      (unnamed persons will not be in this list)
*/
begin_func(GetPersonList, 0)

// ask the map engine for the list of names
std::vector<std::string> names;
int size;
if (!This->m_Engine->GetMapEngine()->GetPersonList(names,size))
{
    This->ReportMapEngineError("GetPersonList() failed");
    return JS_FALSE;
}
//JS_GC(cx);

JSObject *arg_array = JS_NewArrayObject(cx, names.size(), NULL);
if (arg_array == NULL || !JS_AddRoot(cx, &arg_array))
{
    JS_ReportError(cx, "GetPersonList() failed object");
    return JS_FALSE;
}

for(unsigned int i=0; i<names.size(); i++) {
	JSString *jStr = JS_NewStringCopyZ(cx, names[i].c_str());
	*rval = STRING_TO_JSVAL(jStr);
	//JS_GC(cx);
	jsval aname = STRING_TO_JSVAL(jStr);
	*rval = aname;
	//JS_GC(cx);
	JS_SetElement(cx, arg_array, (jsint)i, &aname);
	//JS_GC(cx);
/*
	jsval j = STRING_TO_JSVAL(JS_NewStringCopyZ( cx, names[i].c_str() ));
	*rval = j;
	JS_GC(cx);
	JS_SetElement(cx, result, (jsint)i, &j  );
	JS_GC(cx);
*/
}

return_object(arg_array); //*rval = OBJECT_TO_JSVAL(result); 
JS_RemoveRoot(cx, &arg_array);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Searches for person 'name' in the current person list and returns true if found.
*/
begin_func(DoesPersonExist, 1)
arg_str(name);
if (name == "")
{
    JS_ReportError(cx, "DoesPersonExist: empty person name given");
    return JS_FALSE;
}
return_bool( This->m_Engine->GetMapEngine()->DoesPersonExist(name) );
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns a person object with 'name' from 'spriteset'. If Sphere is
      unable to open the file, the engine will give an error message and
      exit. destroy_with_map is a boolean (true/false value), which the
      spriteset is destroyed when the current map is changed if the flag
      is set to true.
*/
begin_func(CreatePerson, 3)
arg_str(name);
arg_str(spriteset);
arg_bool(destroy_with_map);

if (!This->m_Engine->GetMapEngine()->CreatePerson(name, spriteset, destroy_with_map))
{
    This->ReportMapEngineError("CreatePerson() failed");
    return JS_FALSE;
}

JS_MaybeGC(cx);
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - destroys the person with the name
*/
begin_func(DestroyPerson, 1)
arg_str(name);

if (!This->m_Engine->GetMapEngine()->DestroyPerson(name))
{
    This->ReportMapEngineError("DestroyPerson() failed");
    return JS_FALSE;
}

JS_MaybeGC(cx);
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - sets the x position of the person on the map
*/
begin_func(SetPersonX, 2)
arg_str(name);
arg_int(x);

if (!This->m_Engine->GetMapEngine()->SetPersonX(name, x))
{
    This->ReportMapEngineError("SetPersonX() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - sets the y position of the person on the map
*/
begin_func(SetPersonY, 2)
arg_str(name);
arg_int(y);

if (!This->m_Engine->GetMapEngine()->SetPersonY(name, y))
{
    This->ReportMapEngineError("SetPersonY() failed");
    return JS_FALSE;
}

end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - sets the horizontal offset of the person on the map
*/
begin_func(SetPersonOffsetX, 2)
arg_str(name);
arg_int(offx);

if (!This->m_Engine->GetMapEngine()->SetPersonOffsetX(name, offx))
{
    This->ReportMapEngineError("SetPersonOffsetX() failed");
    return JS_FALSE;
}

end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - sets the vertical offset of the person on the map
*/
begin_func(SetPersonOffsetY, 2)
arg_str(name);
arg_int(offy);

if (!This->m_Engine->GetMapEngine()->SetPersonOffsetY(name, offy))
{
    This->ReportMapEngineError("SetPersonOffsetY() failed");
    return JS_FALSE;
}

end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - sets the position of the person with floating point accuracy
*/
begin_func(SetPersonXYFloat, 3)
arg_str(name);
arg_double(x);
arg_double(y);

if (!This->m_Engine->GetMapEngine()->SetPersonXYFloat(name, x, y))
{
    This->ReportMapEngineError("SetPersonXYFloat() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - sets the position of the person on the map
*/
begin_func(SetPersonLayer, 2)
arg_str(name);
arg_int(layer);

if (!This->m_Engine->GetMapEngine()->SetPersonLayer(name, layer))
{
    This->ReportMapEngineError("SetPersonLayer() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - gets the position of the person on the map
      The position is based on the middle of the spriteset's base
      rectangle.
*/
begin_func(GetPersonX, 1)
arg_str(name);

int x;
if (!This->m_Engine->GetMapEngine()->GetPersonX(name, x))
{
    This->ReportMapEngineError("GetPersonX() failed");
    return JS_FALSE;
}

return_int(x);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - gets the position of the person on the map
      The position is based on the middle of the spriteset's base
      rectangle.
*/
begin_func(GetPersonY, 1)
arg_str(name);

int y;
if (!This->m_Engine->GetMapEngine()->GetPersonY(name, y))
{
    This->ReportMapEngineError("GetPersonY() failed");
    return JS_FALSE;
}

return_int(y);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - gets the position of the person on the map
      The position is based on the middle of the spriteset's base
      rectangle.
*/
begin_func(GetPersonOffsetX, 1)
arg_str(name);

int x;
if (!This->m_Engine->GetMapEngine()->GetPersonOffsetX(name, x))
{
    This->ReportMapEngineError("GetPersonOffsetX() failed");
    return JS_FALSE;
}

return_int(x);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - gets the position of the person on the map
      The position is based on the middle of the spriteset's base
      rectangle.
*/
begin_func(GetPersonOffsetY, 1)
arg_str(name);

int y;
if (!This->m_Engine->GetMapEngine()->GetPersonOffsetY(name, y))
{
    This->ReportMapEngineError("GetPersonOffsetY() failed");
    return JS_FALSE;
}

return_int(y);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - gets the position of the person on the map
      The position is based on the middle of the spriteset's base
      rectangle.
*/
begin_func(GetPersonXFloat, 1)
arg_str(name);

double x;
if (!This->m_Engine->GetMapEngine()->GetPersonXFloat(name, x))
{
    This->ReportMapEngineError("GetPersonXFloat() failed");
    return JS_FALSE;
}

return_double(x);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - gets the position of the person on the map
      The position is based on the middle of the spriteset's base
      rectangle.
*/
begin_func(GetPersonYFloat, 1)
arg_str(name);

double y;
if (!This->m_Engine->GetMapEngine()->GetPersonYFloat(name, y))
{
    This->ReportMapEngineError("GetPersonYFloat() failed");
    return JS_FALSE;
}

return_double(y);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - gets the position of the person on the map
*/
begin_func(GetPersonLayer, 1)
arg_str(name);

int layer;
if (!This->m_Engine->GetMapEngine()->GetPersonLayer(name, layer))
{
    This->ReportMapEngineError("GetPersonLayer() failed");
    return JS_FALSE;
}

return_int(layer);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Sets whether 'person' should ignore other spriteset bases
*/
begin_func(IgnorePersonObstructions, 2)
arg_str(name);
arg_bool(ignoring);

if (!This->m_Engine->GetMapEngine()->IgnorePersonObstructions(name, ignoring))
{
    This->ReportMapEngineError("IgnorePersonObstructions() failed");
    return JS_FALSE;
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Returns true if 'person' is ignoring person obstructions, else false
*/
begin_func(IsIgnoringPersonObstructions, 1)
arg_str(name);
bool ignoring = false;

if (!This->m_Engine->GetMapEngine()->IsIgnoringPersonObstructions(name, ignoring))
{
    This->ReportMapEngineError("IsIgnoringPersonObstructions() failed");
    return JS_FALSE;
}
return_bool(ignoring);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Sets whether 'person' should ignore tile obstructions
*/
begin_func(IgnoreTileObstructions, 2)
arg_str(name);
arg_bool(ignoring);

if (!This->m_Engine->GetMapEngine()->IgnoreTileObstructions(name, ignoring))
{
    This->ReportMapEngineError("IgnoreTileObstructions() failed");
    return JS_FALSE;
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Returns true if 'person' is ignoring tile obstructions, else false
*/
begin_func(IsIgnoringTileObstructions, 1)
arg_str(name);
bool ignoring = false;

if (!This->m_Engine->GetMapEngine()->IsIgnoringTileObstructions(name, ignoring))
{
    This->ReportMapEngineError("IsIgnoringTileObstructions() failed");
    return JS_FALSE;
}
return_bool(ignoring);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Returns a list of people that 'name' is ignoring
*/
begin_func(GetPersonIgnoreList, 1)
arg_str(name);
std::vector<std::string> ignore_list;
if (!This->m_Engine->GetMapEngine()->GetPersonIgnoreList(name, ignore_list))
{

    This->ReportMapEngineError("GetPersonIgnoreList() failed");
    return JS_FALSE;
}
// create an array of JS strings with which to initialize the array
jsval* valarray = new jsval[ignore_list.size()];
if (!valarray)
{
    return JS_FALSE;
}
*rval = *valarray;
for (unsigned int i = 0; i < ignore_list.size(); i++)
{

    valarray[i] = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, ignore_list[i].c_str()));
}
// create the array object
JSObject* array = JS_NewArrayObject(cx, ignore_list.size(), valarray);
if (array == NULL)
{

    JS_ReportError(cx, "Fatal Error!  JS_NewArrayObject() failed!");
    delete[] valarray;
    return JS_FALSE;
}
// delete our temporary jsval array and return the JS array
return_object(array);
delete[] valarray;
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - Tells 'name' to ignore everyone in list
      e.g. SetPersonIgnoreList("White-Bomberman", ["bomb", "powerup"]);
      Tells White-Bomberman to not be obstructed by bombs or powerups
*/
begin_func(SetPersonIgnoreList, 2)
arg_str(name);
arg_array(array);
std::vector<std::string> ignore_list;
jsuint length;
JS_GetArrayLength(cx, array, &length);
ignore_list.resize(length);
for (unsigned int i = 0; i < length; i++)
{

    jsval val;
    if (JS_LookupElement(cx, array, i, &val))
    {
// not sure if we need to protect val from GB?
        if (JSVAL_IS_STRING(val))
        {

            ignore_list[i] = argStr(cx, val);
        }
    }
}
if (!This->m_Engine->GetMapEngine()->SetPersonIgnoreList(name, ignore_list))
{

    This->ReportMapEngineError("SetPersonIgnoreList() failed");
    return JS_FALSE;
}
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - makes the sprite 'name' follow 'pixels' pixels behind sprite 'leader'.
      If this function is called like:
      FollowPerson(name, "", 0),
      the person will detach from anyone it is following.
*/
begin_func(FollowPerson, 3)
arg_str(person);
arg_str(leader);
arg_int(pixels);

if (!This->m_Engine->GetMapEngine()->FollowPerson(person, leader, pixels))
{
    This->ReportMapEngineError("FollowPerson() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - sets which direction to show
*/
begin_func(SetPersonDirection, 2)
arg_str(name);
arg_str(direction);

if (!This->m_Engine->GetMapEngine()->SetPersonDirection(name, direction))
{
    This->ReportMapEngineError("SetPersonDirection() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - gets which direction is being shown
*/
begin_func(GetPersonDirection, 1)
arg_str(name);

std::string direction;
if (!This->m_Engine->GetMapEngine()->GetPersonDirection(name, direction))
{
    This->ReportMapEngineError("GetPersonDirection() failed");
    return JS_FALSE;
}

return_str(direction.c_str());
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - sets which frame to show
      It will retain the frame delay count of the previous frame, unless
      the third optional parameter is false
*/
begin_func(SetPersonFrame, 2)
arg_str(name);
arg_int(frame);
bool resetFrameDelay = true;
if (argc >= 3)
{
    resetFrameDelay = argBool(cx, argv[2]);
}
int delay;
if(!resetFrameDelay)
    This->m_Engine->GetMapEngine()->GetPersonFrameNext(name, delay);
if (!This->m_Engine->GetMapEngine()->SetPersonFrame(name, frame))
{
    This->ReportMapEngineError("SetPersonFrame() failed");
    return JS_FALSE;
}
if(!resetFrameDelay)
    This->m_Engine->GetMapEngine()->SetPersonFrameNext(name, delay);

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - gets which frame is being shown
*/
begin_func(GetPersonFrame, 1)
arg_str(name);

int frame;
if (!This->m_Engine->GetMapEngine()->GetPersonFrame(name, frame))
{
    This->ReportMapEngineError("GetPersonFrame() failed");
    return JS_FALSE;
}

return_int(frame);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - sets the remaining frame delay for the current frame
*/
begin_func(SetPersonFrameNext, 2)
arg_str(name);
arg_int(frame);

if (!This->m_Engine->GetMapEngine()->SetPersonFrameNext(name, frame))
{
    This->ReportMapEngineError("SetPersonFrameNext() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - gets the remaining frame delay for the current frame
    @see GetPersonSpriteset
*/
begin_func(GetPersonFrameNext, 1)
arg_str(name);

int frame;
if (!This->m_Engine->GetMapEngine()->GetPersonFrameNext(name, frame))
{
    This->ReportMapEngineError("GetPersonFrameNext() failed");
    return JS_FALSE;
}

return_int(frame);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - sets the delay between when the person last moved and returning to
      first frame. 0 disables this behaviour.
*/
begin_func(SetPersonFrameRevert, 2)
arg_str(name);
arg_int(frames);

if (!This->m_Engine->GetMapEngine()->SetPersonFrameRevert(name, frames))
{
    This->ReportMapEngineError("SetPersonFrameRevert() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - gets the delay between when the person last moved and returning to
      first frame. 0 disables this behaviour.
*/
begin_func(GetPersonFrameRevert, 1)
arg_str(name);

int frames;
if (!This->m_Engine->GetMapEngine()->GetPersonFrameRevert(name, frames))
{
    This->ReportMapEngineError("GetPersonFrameRevert() failed");
    return JS_FALSE;
}

return_int(frames);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - sets the speed at which a person moves at
*/
begin_func(SetPersonSpeed, 2)
arg_str(name);
arg_double(speed);

if (!This->m_Engine->GetMapEngine()->SetPersonSpeedXY(name, speed, speed))
{
    This->ReportMapEngineError("SetPersonSpeed() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - sets the speed at which a person moves at
*/
begin_func(SetPersonSpeedXY, 3)
arg_str(name);
arg_double(x);
arg_double(y);

if (!This->m_Engine->GetMapEngine()->SetPersonSpeedXY(name, x, y))
{
    This->ReportMapEngineError("SetPersonSpeedXY() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - gets the speed at which a person moves at
*/
begin_func(GetPersonSpeedX, 1)
arg_str(name);

double x;
if (!This->m_Engine->GetMapEngine()->GetPersonSpeedX(name, x))
{
    This->ReportMapEngineError("GetPersonSpeedX() failed");
    return JS_FALSE;
}

return_double(x);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - gets the speed at which a person moves at
*/
begin_func(GetPersonSpeedY, 1)
arg_str(name);

double y;
if (!This->m_Engine->GetMapEngine()->GetPersonSpeedY(name, y))
{
    This->ReportMapEngineError("GetPersonSpeedY() failed");
    return JS_FALSE;
}

return_double(y);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - gets the unitarian directional vector as a single number: x+(y<<2)
      The directional vector is 0 when the person is not moving, use 
      historical to retrieve the previous value
      Convert Vector to direction:
      function V2D(v){
        switch(v){
            case -5: return 'northwest'; break;
            case -4: return 'north'; break;
            case -3: return 'northeast'; break;
            case -1: return 'west'; break;
            case 1: return 'east'; break;
            case 3: return 'southwest'; break;
            case 4: return 'south'; break;
            case 5: return 'southeast'; break;
            default: return '';
        }
      }      
      Convert Vector to coordinates:    
      function V2C(v){var y=(1+v>>2)&-1;return {x:v-(y<<2),y:y};}
*/
begin_func(GetPersonVector, 2)
arg_str(name);
arg_bool(historical)

int x;
int y;
if (!This->m_Engine->GetMapEngine()->GetPersonVectorXY(name,historical, x, y))
{
    This->ReportMapEngineError("GetPersonVectorXY() failed");
    return JS_FALSE;
}

return_int(x+(y<<2));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - rescales the sprite to a certain scale specified by scale_w and scale_h.
      Scaling is determined by floating point numbers like, 1.5 would scale
      the person to 1.5 times its normal size based on his original sprite
      size.

*/
begin_func(SetPersonScaleFactor, 3)
arg_str(name);
arg_double(scale_w);
arg_double(scale_h);

if(scale_w<0) scale_w=0;
if(scale_h<0) scale_h=0;

if (!This->m_Engine->GetMapEngine()->SetPersonScaleFactor(name, scale_w, scale_h))
{
    This->ReportMapEngineError("SetPersonScaleFactor() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - rescales the sprite to width pixels and height pixels.
*/
begin_func(SetPersonScaleAbsolute, 3)
arg_str(name);
arg_int(width);
arg_int(height);

if (!This->m_Engine->GetMapEngine()->SetPersonScaleAbsolute(name, width, height))
{
    This->ReportMapEngineError("SetPersonScaleAbsolute() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - gets a data object assiocated with the person 'name'
      There are certain default properties/values filled in by the engine, they are:
      num_frames - the number of frames for the person's current direction
      num_directions - the number of directions for the person
      width - the width of the spriteset's current frame
      height - the height of the spriteset's current frame
      leader - the person that this person is following, or "" if no-one...
      destroy_with_map - if the person is destroyed with the map or not
      obs_person - obstructed person index (-1 if not obstructed) 
      Any other properties are free for you to fill with values
      e.g. var data = GetPersonData("Jimmy");
      var num_frames = data["num_frames"];
      note: Objects can not be stored, only numbers, booleans and strings.
*/
begin_func(GetPersonData, 1)
arg_str(name);
std::vector<struct PersonData> data;
if (!This->m_Engine->GetMapEngine()->GetPersonData(name, data))
{

    This->ReportMapEngineError("GetPersonData() failed");
    return JS_FALSE;
}
// define class
static JSClass clasp =
    {

        "person_data", 0,
        JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
        JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    };
// create object
JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);
if (!object || !JS_AddRoot(cx, &object))
{
    return_object(JSVAL_NULL);
}
for (int i = 0; i < int(data.size()); i++)
{

    switch (data[i].type)
    {

    case 1:
        if (JS_DefineProperty(cx, object, data[i].name.c_str(), DOUBLE_TO_JSVAL(JS_NewDouble(cx, data[i].double_value)),              JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE) == JS_TRUE)
        { }

        break;
    case 2:
        if (JS_DefineProperty(cx, object, data[i].name.c_str(), INT_TO_JSVAL(data[i].double_value),                  JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE) == JS_TRUE)
        { }

        break;
    case 3:
        if (JS_DefineProperty(cx, object, data[i].name.c_str(), BOOLEAN_TO_JSVAL((int)data[i].double_value == 0 ? false : true),              JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE) == JS_TRUE)
        { }

        break;
    default:
        if (JS_DefineProperty(cx, object, data[i].name.c_str(), STRING_TO_JSVAL(JS_NewStringCopyZ(cx, data[i].string_value.c_str())), JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE) == JS_TRUE)
        { }

    }
}
return_object(object);
JS_RemoveRoot(cx, &object);
end_func()
////////////////////////////////////////////////////////////////////////////////
static void
ParsePersonData(JSContext* cx, jsval val, std::string& string_value, double& double_value, int& type)
{
    bool is_object = JSVAL_IS_OBJECT(val);
    string_value = "";
    double_value = 0;
    type = -1;
    if ( !is_object
            && (JSVAL_IS_INT(val) || JSVAL_IS_DOUBLE(val))
            && JS_ValueToNumber(cx, val, &double_value) != JS_FALSE)
    {

        if (JSVAL_IS_DOUBLE(val))
            type = 1;
        else
            if (JSVAL_IS_INT(val))
                type = 2;
    }
    else
        if ( !is_object && JSVAL_IS_BOOLEAN(val) )
        {

            if (JSVAL_TO_BOOLEAN(val))
                double_value = 1;
            else
                double_value = 0;
            type = 3;
        }
        else
        { // anything else is a string

            string_value = argStr(cx, val);
            type = 0;
        }
}
////////////////////////////////////////////////////////////////////////////////
/**
    - sets the 'data' object associated with the person 'name'
      e.g.
      var data = GetPersonData("Jimmy");
      data["talked_to_jimmy"] = true;
      SetPersonData("Jimmy", data);
      note: Objects can not be stored, only numbers, booleans and strings.
*/
begin_func(SetPersonData, 2)
arg_str(name);
arg_object(data_object);
std::vector<struct PersonData> person_data;
JSIdArray* ids = JS_Enumerate(cx, data_object);

if (!ids || !JS_AddRoot(cx, &ids))
    return JS_FALSE;

jsval id, val;
int i = ids->length;
while (i--) {
  JS_IdToValue(cx, ids->vector[i], &id);
  if (JSVAL_IS_STRING(id)) {
	struct PersonData data;
    data.name = JS_GetStringBytes(JSVAL_TO_STRING(id));
    if (JS_LookupProperty(cx, data_object, data.name.c_str(), &val)) {
		ParsePersonData(cx, val, data.string_value, data.double_value, data.type);
    }
	person_data.push_back(data);
  }
}
/*
// This code sometimes data.name = argStr() was empty, and JS_GetProperty failed.
for (jsint i = ids->length -1; i>=0; --i)
{

    jsval val;
    if (JS_IdToValue(cx, ids->vector[i], &val) == JS_TRUE)
    {

        struct PersonData data;
        data.name = argStr(cx, val);
        if ( JS_GetProperty(cx, data_object, data.name.c_str(), &val) == JS_TRUE )
        {
            ParsePersonData(cx, val, data.string_value, data.double_value, data.type);
        }
        person_data.push_back(data);
    }
}
*/
JS_RemoveRoot(cx, &ids);
JS_DestroyIdArray(cx, ids);

if (!This->m_Engine->GetMapEngine()->SetPersonData(name, person_data))
{

    This->ReportMapEngineError("SetPersonData() failed");
    return JS_FALSE;
}
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - get a single person value. Example: GetPersonValue("Jimmy", "num_frames");
    @see  GetPersonData
*/
begin_func(GetPersonValue, 2)
arg_str(name);
arg_str(key);
std::string string_value;
double double_value;
int type = -1;
if (!This->m_Engine->GetMapEngine()->GetPersonValue(name, key, string_value, double_value, type))
{
    This->ReportMapEngineError("GetPersonValue() failed");
    return JS_FALSE;
}
switch (type)
{

case 1:
    return_double(double_value);
    break;
case 2:
    return_int((int)double_value);
    break;
case 3:
    return_bool((int)double_value == 0 ? false : true);
    break;
default:
    return_str(string_value.c_str());
}
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - set a person value. You can set boolean, numeric and string values
      SetPersonValue("Jimmy", "talked_to_jimmy", true);
*/
begin_func(SetPersonValue, 3)
arg_str(name);
arg_str(key);
std::string string_value;
double double_value;
int type = -1;
ParsePersonData(cx, argv[2], string_value, double_value, type);
if (!This->m_Engine->GetMapEngine()->SetPersonValue(name, key, string_value, double_value, type))
{
    This->ReportMapEngineError("SetPersonValue() failed");
    return JS_FALSE;
}
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - returns the person's spriteset.
      Although you can't add/delete directions, or add/delete frames you can change
      all properties. You can create a new spriteset using @see CreateSpriteset
      ss.images[] - An array of images
      ss.base.x1, ss.base.y1, ss.base.x2, ss.base.y2 - The _real_ obstruction base
      ss.directions[0].name - the name of direction 0
      ss.directions[0].frames[0].index  - the index to images for the first frame
      ss.directions[0].frames[0].delay - the delay showing that frame
    @see CreateSpriteset
*/
begin_func(GetPersonSpriteset, 1)
arg_str(name);

SSPRITESET* spriteset = This->m_Engine->GetMapEngine()->GetPersonSpriteset(name);
if (spriteset == NULL)
{
    JS_ReportError(cx, "Could not find person '%s'", name);
    return JS_FALSE;
}

spriteset->AddRef();
return_object(CreateSpritesetObject(cx, spriteset));
end_func()

/////////////////////////////////////////////////////////////////////////////////
/**
    - set's the person's spriteset to spriteset
      e.g. SetPersonSpriteset("Jimmy", LoadSpriteset("jimmy.running.rss"));
*/
begin_func(SetPersonSpriteset, 2)
arg_str(name);
arg_spriteset(spriteset);

if (spriteset->GetFrameHeight() == 0)
{
    JS_ReportError(cx, "Invalid spriteset for person '%s'", name);
    return JS_FALSE;
}
if (!This->m_Engine->GetMapEngine()->SetPersonSpriteset(name, *spriteset))
{
    JS_ReportError(cx, "Could not find person '%s'", name);
    delete spriteset;
    return JS_FALSE;
}

delete spriteset;
// spritesets can take a lot of memory, so do a little GC
JS_MaybeGC(cx);

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - returns the person's base obstruction object. The values are sensitive to scaling,
      unless you set 'real' to true.
      The object contains the following properties: x1, x2, y1 and y2 (base coordinates)
      You also can use GetPersonSpriteset and fetch the .base properties of it.
    @see GetPersonSpriteset
*/
begin_func(GetPersonBase, 1)
arg_str(name);
bool real = false;
if (argc >= 2)
{
    real = argBool(cx, argv[1]);
}

SSPRITESET* spriteset = This->m_Engine->GetMapEngine()->GetPersonSpriteset(name);
if (spriteset == NULL)
{
    JS_ReportError(cx, "Could not find person '%s'", name);
    return JS_FALSE;
}

return_object(CreateSpritesetBaseObject(cx, spriteset, real));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Sets the person's base obstruction object.
      Note: If the person is zoomed, you still need to define the base as if the person
      was not scaled, and use SetPersonScaleFactor(person,GetPersonScaleFactor(person));
      to update the base.
*/
begin_func(SetPersonBase, 5)
arg_str(name);
arg_int(x1);
arg_int(y1);
arg_int(x2);
arg_int(y2);

if ( !This->m_Engine->GetMapEngine()->SetPersonBase(name, x1, y1, x2, y2) )
{
    JS_ReportError(cx, "Could not set the personbase for person '%s'", name);
    return JS_FALSE;
}
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns the person's angle that is used
*/
begin_func(GetPersonAngle, 1)
arg_str(name);
double angle;

if ( !This->m_Engine->GetMapEngine()->GetPersonAngle(name, angle) )
{
    JS_ReportError(cx, "Could not find person '%s'", name);
    return JS_FALSE;
}

return_double(angle);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - sets the angle which the person should be drawn at
      Note:
        Zero is no rotation, and angles are in radians.
        It does not rotate the spritesets obstruction base.
*/
begin_func(SetPersonAngle, 2)
arg_str(name);
arg_double(angle);

if ( !This->m_Engine->GetMapEngine()->SetPersonAngle(name, angle) )
{
    JS_ReportError(cx, "Could not find person '%s'", name);
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - sets a color multiplier to use when drawing sprites.  if the color is
      RGBA:(255, 0, 0, 255), only the red elements of the sprite are drawn.
      If the color is RGBA:(255, 255, 255, 128), the sprite is drawn at
      half transparency.
*/
begin_func(SetPersonMask, 2)
arg_str(name);
arg_color(mask);

if (!This->m_Engine->GetMapEngine()->SetPersonMask(name, mask))
{
    This->ReportMapEngineError("SetPersonMask() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - returns the person's current mask
*/
begin_func(GetPersonMask, 1)
arg_str(name);

RGBA mask;
if (!This->m_Engine->GetMapEngine()->GetPersonMask(name, mask))
{
    This->ReportMapEngineError("GetPersonMask() failed");
    return JS_FALSE;
}

return_object(CreateColorObject(cx, mask));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns the person's visible status
*/
begin_func(IsPersonVisible, 1)
arg_str(name);
bool visible;
if (!This->m_Engine->GetMapEngine()->IsPersonVisible(name, visible))
{

    This->ReportMapEngineError("IsPersonVisible() failed");
    return JS_FALSE;
}
return_bool(visible);
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - sets the person's visible status, true = visible, false = not visible
      e.g. SetPersonVisible(GetCurrentPerson(), !IsPersonVisible(GetCurrentPerson()));
*/
begin_func(SetPersonVisible, 2)
arg_str(name);
arg_bool(visible);
if (!This->m_Engine->GetMapEngine()->SetPersonVisible(name, visible))
{

    This->ReportMapEngineError("SetPersonVisible() failed");
    return JS_FALSE;
}
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - sets 'script' as the thing 'name' does in a certain event
      the five events are
      SCRIPT_ON_CREATE
      SCRIPT_ON_DESTROY
      SCRIPT_ON_ACTIVATE_TOUCH
      SCRIPT_ON_ACTIVATE_TALK
      SCRIPT_COMMAND_GENERATOR
      (SCRIPT_COMMAND_GENERATOR will be called when the command queue for
       the person runs out (for random movement thingies, etc))
*/
begin_func(SetPersonScript, 3)
arg_str(name);
arg_int(which);
arg_str(script);

if (!This->m_Engine->GetMapEngine()->SetPersonScript(name, which, script))
{
    This->ReportMapEngineError("SetPersonScript() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - sets 'script' as the thing that everyone does in a certain event
      the five events are see above
*/
begin_func(SetDefaultPersonScript, 2)
arg_int(which);
arg_str(script);
if (!This->m_Engine->GetMapEngine()->SetDefaultPersonScript(which, script))
{

    This->ReportMapEngineError("SetDefaultPersonScript() failed");
    return JS_FALSE;
}
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - calls a person's script from code
      'which' constants are the same as for SetPersonScript()
*/
begin_func(CallPersonScript, 2)
arg_str(name);
arg_int(which);

if (!This->m_Engine->GetMapEngine()->CallPersonScript(name, which))
{
    This->ReportMapEngineError("CallPersonScript() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - best when called from inside a PersonScript handler
      it will return the name of the person for whom the current script
      is running
*/
begin_func(GetCurrentPerson, 0)

std::string person;
if (!This->m_Engine->GetMapEngine()->GetCurrentPerson(person))
{
    This->ReportMapEngineError("GetCurrentPerson() failed");
    return JS_FALSE;
}

return_str(person.c_str());
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - adds a command to the person's command queue
      the commands are:
      COMMAND_WAIT
      COMMAND_FACE_NORTH
      COMMAND_FACE_NORTHEAST
      COMMAND_FACE_EAST
      COMMAND_FACE_SOUTHEAST
      COMMAND_FACE_SOUTH
      COMMAND_FACE_SOUTHWEST
      COMMAND_FACE_WEST
      COMMAND_FACE_NORTHWEST
      COMMAND_MOVE_NORTH
      COMMAND_MOVE_EAST
      COMMAND_MOVE_SOUTH
      COMMAND_MOVE_WEST
      (note: these *might* change in a future release
       'immediate', if true, will execute the command go right away
       if false, it will wait until the next frame)
*/
begin_func(QueuePersonCommand, 3)
arg_str(name);
arg_int(command);
arg_bool(immediate);

if (!This->m_Engine->GetMapEngine()->QueuePersonCommand(name, command, immediate))
{
    This->ReportMapEngineError("QueuePersonCommand() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - adds a script command to the person's queue
*/
begin_func(QueuePersonScript, 3)
arg_str(name);
arg_str(script);
arg_bool(immediate);

if (!This->m_Engine->GetMapEngine()->QueuePersonScript(name, script, immediate))
{
    This->ReportMapEngineError("QueuePersonScript() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - clears the command queue of sprite with the 'name'
*/
begin_func(ClearPersonCommands, 1)
arg_str(name);

if (!This->m_Engine->GetMapEngine()->ClearPersonCommands(name))
{
    This->ReportMapEngineError("ClearPersonCommands() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - returns true if the person 'name' has an empty command queue
*/
begin_func(IsCommandQueueEmpty, 1)
arg_str(name);

bool result;
if (!This->m_Engine->GetMapEngine()->IsCommandQueueEmpty(name, result))
{
    This->ReportMapEngineError("IsCommandQueueEmpty() failed");
    return JS_FALSE;
}

return_bool(result);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns -1 if name isn't obstructed by a tile at x, y,
    - returns the tile index of the tile if name is obstructed at x, y
*/
begin_func(GetObstructingTile, 3)
arg_str(name);
arg_int(x);
arg_int(y);

int result;
if (!This->m_Engine->GetMapEngine()->GetObstructingTile(name, x, y, result))
{
    This->ReportMapEngineError("GetObstructingTile() failed");
    return JS_FALSE;
}

return_int(result);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns "" if name isn't obstructed by person at x, y,
    - returns the name of the person if name is obstructed at x, y
      To check if more persons obstruct, move the first one away.
*/
begin_func(GetObstructingPerson, 3)
arg_str(name);
arg_int(x);
arg_int(y);

std::string result;
if (!This->m_Engine->GetMapEngine()->GetObstructingPerson(name, x, y, result))
{
    This->ReportMapEngineError("GetObstructingPerson() failed");
    return JS_FALSE;
}

return_str(result.c_str());
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns true if person 'name' would be obstructed at (x, y)
*/
begin_func(IsPersonObstructed, 3)
arg_str(name);
arg_int(x);
arg_int(y);

bool result;
if (!This->m_Engine->GetMapEngine()->IsPersonObstructed(name, x, y, result))
{
    This->ReportMapEngineError("IsPersonObstructed() failed");
    return JS_FALSE;
}

return_bool(result);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - set key used to activate talk scripts
*/
begin_func(SetTalkActivationKey, 1)
arg_int(key);

if (!This->m_Engine->GetMapEngine()->SetTalkActivationKey(key))
{
    This->ReportMapEngineError("SetTalkActivationKey() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - get key used to activate talk scripts
*/
begin_func(GetTalkActivationKey, 0)
return_int(This->m_Engine->GetMapEngine()->GetTalkActivationKey());
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - get button used to activate talk scripts
*/
begin_func(GetTalkActivationButton, 0)
return_int(This->m_Engine->GetMapEngine()->GetTalkActivationButton());
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - set button used to activate talk scripts
*/
begin_func(SetTalkActivationButton, 1)
arg_int(button);
if (!This->m_Engine->GetMapEngine()->SetTalkActivationButton(button))
{

    This->ReportMapEngineError("SetTalkActivationButton() failed");
    return JS_FALSE;
}
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - set distance to check for talk script activation
*/
begin_func(SetTalkDistance, 1)
arg_int(pixels);

if (!This->m_Engine->GetMapEngine()->SetTalkDistance(pixels))
{
    This->ReportMapEngineError("SetTalkDistance() failed");
    return JS_FALSE;
}

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - get distance to check for talk script activation
*/
begin_func(GetTalkDistance, 0)
return_int(This->m_Engine->GetMapEngine()->GetTalkDistance());
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns a spriteset object from 'filename'. If Sphere is unable to open
      the file, the engine will give an error message and exit.
*/
begin_func(LoadSpriteset, 1)
arg_str(filename);

// spritesets can take a lot of memory, so do a little GC
JS_MaybeGC(cx);

if (IsValidPath(filename) == false)
{

    JS_ReportError(cx, "Invalid filename: '%s'", filename);
    return JS_FALSE;
}
// load spriteset
SSPRITESET* spriteset = This->m_Engine->LoadSpriteset(filename);
if (spriteset == NULL)
{
    JS_ReportError(cx, "Could not load spriteset '%s'", filename);
    return JS_FALSE;
}

JS_MaybeGC(cx);
return_object(CreateSpritesetObject(cx, spriteset));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns a new spriteset.
*/
begin_func(CreateSpriteset, 5)
arg_int(frame_width);
arg_int(frame_height);
arg_int(num_images);
arg_int(num_directions);
arg_int(num_frames);
if (frame_width < 1 || frame_height < 1)
{

    JS_ReportError(cx, "Frame width and height must be greater than 1 (w=%d, h=%d)", frame_width, frame_height);
    return JS_FALSE;
}
if (num_images < 1 || num_directions < 1 || num_frames < 1)
{

    JS_ReportError(cx, "Must be atleast one image/direction/frame in a spriteset");
    return JS_FALSE;
}
sSpriteset s;
if (!s.Create(frame_width, frame_height, num_images, num_directions, num_frames))
{

    return JS_FALSE;
}
s.SetBase(0, 0, frame_width, frame_height);
s.Base2Real();
SSPRITESET* spriteset = new SSPRITESET(s);
if (!spriteset)
{

    return JS_FALSE;
}
spriteset->AddRef();
return_object(CreateSpritesetObject(cx, spriteset));
end_func()


////////////////////////////////////////////////////////////////////////////////
// section: Sound related functions //
////////////////////////////////////////////////////////////////////////////////
/**
    - returns a sound object from 'filename'. If Sphere is unable to open
      the file, the engine will give an error message and exit.  If the optional
      argument 'streaming' is true or unspecified, the sound is streamed from the
      hard drive.  Otherwise, it's loaded into memory.

*/
begin_func(LoadSound, 1)
arg_str(filename);
bool streaming = true;
if (argc > 1)
{
    streaming = argBool(cx, argv[arg++]);
}

if (IsValidPath(filename) == false)
{

    JS_ReportError(cx, "Invalid filename: '%s'", filename);
    return JS_FALSE;
}
// load sound
audiere::OutputStream* sound = NULL;

if ( !IsMidi(filename) )
{

    sound = This->m_Engine->LoadSound(filename, streaming);
}
#if defined(WIN32) && defined(USE_MIDI)
audiere::MIDIStream* midi    = NULL;
if ( !sound && IsMidi(filename) )
{

    midi = This->m_Engine->LoadMIDI(filename);
}
if (!sound && !midi)
#else

// In Linux midi filetypes will work but be sound = NULL
if (!sound && !IsMidi(filename))
#endif
{
    JS_ReportError(cx, "Could not load sound '%s'", filename);
    return JS_FALSE;
}

#if defined(WIN32) && defined(USE_MIDI)
return_object(CreateSoundObject(cx, sound, midi, NULL));
#else

return_object(CreateSoundObject(cx, sound, NULL));
#endif
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns a sound effect object from 'filename'. If Sphere is unable to open
      the file, the engine will give an error message and exit.
      There are two types of sound effects: SE_SINGLE and SE_MULTIPLE.
      SE_SINGLE sound effects only allow the sound to be played once at a time.
      SE_MULTIPLE sound effects always open a new stream to the audio device
      for each time it is played (cleaning up or reusing old streams if possible).

*/
begin_func(LoadSoundEffect, 1)
arg_str(filename);

int type = audiere::SINGLE;
if (argc > 1)
{
    type = argInt(cx, argv[1]);
}

if (IsValidPath(filename) == false)
{
    JS_ReportError(cx, "Invalid filename: '%s'", filename);
    return JS_FALSE;
}

// load sound effect
audiere::SoundEffect* sound = NULL;

if (type == audiere::MULTIPLE)
    sound = This->m_Engine->LoadSoundEffect(filename, audiere::MULTIPLE);
else
    sound = This->m_Engine->LoadSoundEffect(filename, audiere::SINGLE);

if (!sound)
{
    JS_ReportError(cx, "Could not load sound effect '%s'", filename);
    return JS_FALSE;
}

return_object(CreateSoundEffectObject(cx, sound, NULL));
end_func()


////////////////////////////////////////////////////////////////////////////////
/**
    - returns a sound  from bytearray.
*/
begin_func(CreateSound, 1)
arg_byte_array(array);
bool streaming = true;
if (argc > 1)
{
    streaming = argBool(cx, argv[arg++]);
}

if (array->size <= 0)
{
    JS_ReportError(cx, "Invalid bytearray.");
    return JS_FALSE;
}

audiere::File* memoryfile = audiere::CreateMemoryFile(array->array, array->size);

if (!memoryfile)
{
    JS_ReportError(cx, "Could not convert bytearray to memoryfile.");
    return JS_FALSE;
}
//audiere::FilePtr adrfile(memoryfile);

audiere::OutputStream* sound = NULL;
sound = This->m_Engine->CreateSound(memoryfile, streaming);
if (!sound)
{
    if(memoryfile){
        //memoryfile->ref();
        memoryfile->unref();
        memoryfile=NULL;
    }

    JS_ReportError(cx, "Could not create sound from bytearray");
    return JS_FALSE;
}

#if defined(WIN32) && defined(USE_MIDI)
audiere::MIDIStream* midi    = NULL;
return_object(CreateSoundObject(cx, sound, midi, memoryfile));
#else
return_object(CreateSoundObject(cx, sound, memoryfile));
#endif
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns a soundeffect from bytearray.
*/
begin_func(CreateSoundEffect, 1)
arg_byte_array(ba);

audiere::SoundEffectType type = audiere::SINGLE;
if (argc > 1)
{
    type = audiere::SoundEffectType( argInt(cx, argv[1]) );
}

if (ba->size <= 0)
{
    JS_ReportError(cx, "Invalid bytearray.");
    return JS_FALSE;
}


audiere::File* memoryfile = This->m_Engine->CreateMemoryFile(ba->array, ba->size);

if (!memoryfile)
{
    JS_ReportError(cx, "Could not convert bytearray to memoryfile.");
    return JS_FALSE;
}
memoryfile->ref();

audiere::SoundEffect* sound = This->m_Engine->CreateSoundEffect(memoryfile, type);

if (!sound)
{
    if(memoryfile){
        //memoryfile->ref();
        memoryfile->unref();
    }
    JS_ReportError(cx, "Could not create sound effect from bytearray");
    return JS_FALSE;
}

JS_MaybeGC(cx);
return_object(CreateSoundEffectObject(cx, sound, memoryfile) );
end_func()


////////////////////////////////////////////////////////////////////////////////
/**
    - returns an Sfxr object. The parameters are optional and can be set later on.
      bitrate, samplerate, soundvolume, wavetype, basefrequency, minfrequency, 
      frequencyslide, frequencyslidedelta, squareduty, squaredutysweep, vibratodepth,
      vibratospeed, attack, sustain, detay, release, filter, lowpassfiltercutoff,
      lowpassfiltercutoffsweep, filterresonance, highpassfiltercutoff, highpassfiltercutoffsweep,
      phaseroffset, phaseroffsetsweep, repeatspeed, arpeggio, arpeggiospeed
*/

begin_func(CreateSfxr, 0)
SSFXR* sound = NULL;
sound = This->m_Engine->CreateSfxr();
if (!sound)
{
    JS_ReportError(cx, "Could not create sound effect from sfxr");
    return JS_FALSE;
}
unsigned int n = 1; // This way you can swap around the parameters without having to renumber
if (argc >= n){    sound->setBitrate(int( argInt(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setSampleRate(int( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setSoundVolume(float( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setWaveType(int( argInt(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setBaseFrequency(float( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setMinFrequency(float( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setFrequencySlide(float( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setFrequencySlideDelta(float( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setSquareDuty(float( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setSquareDutySweep(float( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setVibratoDepth(float( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setVibratoSpeed(float( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setVibratoDelay(float( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setAttack(float( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setSustain(float( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setDecay(float( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setRelease(float( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setFilter(bool( argBool(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setLowPassFilterCutoff(float( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setLowPassFilterCutoffSweep(float( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setFilterResonance(float( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setHighPassFilterCutoff(float( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setHighPassFilterCutoffSweep(float( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setPhaserOffset(float( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setPhaserOffsetSweep(float( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setRepeatSpeed(float( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setArpeggio(float( argDouble(cx, argv[n++ -1]) )); }
if (argc >= n){    sound->setArpeggioSpeed(float( argDouble(cx, argv[n++ -1]) )); }
//if (argc >= n){    sound->setMasterVolume(float( argDouble(cx, argv[n++ -1]) )); }

return_object(CreateSfxrObject(cx, sound));
end_func()


/**
    - Returns a soundobject of the current map background music
*/
begin_func(GetMapMusic,0)

#if defined(WIN32) && defined(USE_MIDI)
if(This->m_Engine->GetMapEngine()->getMidi())
return_object(CreateSoundObject(cx, NULL, This->m_Engine->GetMapEngine()->getMidi().get(), NULL));
else if(This->m_Engine->GetMapEngine()->getMusic())
return_object(CreateSoundObject(cx, This->m_Engine->GetMapEngine()->getMusic().get(), NULL, NULL));
else
	return_null();
#else
if(This->m_Engine->GetMapEngine()->getMusic())
	return_object(CreateSoundObject(cx, This->m_Engine->GetMapEngine()->getMusic().get(), NULL));
else
	return_null();
#endif

end_func()


/**
    - Returns the name of the current map background music (could be a m3u list)
*/
begin_func(GetMapMusicName, 0)
    std::string result;
    if (!This->m_Engine->GetMapEngine()->GetMapMusicName(result))
    {
        This->ReportMapEngineError("GetMapMusicName() failed");
        return JS_FALSE;
    }
    if(This->m_Engine->GetMapEngine()->GetMapMusicType())
        result = "";

    return_str(result.c_str());
end_func()

/**
    - Returns the type of the current map background music: 0: none 1: midi 2: wav/mp3/it/xm
*/
begin_func(GetMapMusicType, 0)
    return_int( This->m_Engine->GetMapEngine()->GetMapMusicType() );
end_func()

////////////////////////////////////////////////////////////////////////////////
// section: Font functions //
/**
    - returns a font object of the font that the engine currently uses.
*/
begin_func(GetSystemFont, 0)
return_object(CreateFontObject(cx, This->m_Engine->GetSystemFont(), false));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns a font object from 'filename'. If Sphere is unable to open
      the file, the engine will give an error message and exit.
*/
begin_func(LoadFont, 1)
arg_str(filename);

if (IsValidPath(filename) == false)
{

    JS_ReportError(cx, "Invalid filename: '%s'", filename);
    return JS_FALSE;
}
// load font
SFONT* font = This->m_Engine->LoadFont(filename);
if (font == NULL)
{
    JS_ReportError(cx, "Could not load font '%s'", filename);
    return JS_FALSE;
}

return_object(CreateFontObject(cx, font, true));
end_func()

////////////////////////////////////////////////////////////////////////////////
// section: windowstyle functions //
/**
    - returns a windowstyle object of the windowstyle that the engine currently
      uses.

*/
begin_func(GetSystemWindowStyle, 0)
return_object(CreateWindowStyleObject(cx, This->m_Engine->GetSystemWindowStyle(), false));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns a windowstyle object from 'filename'. If Sphere is unable to open
      the file, the engine will give an error message and exit.
*/
begin_func(LoadWindowStyle, 1)
arg_str(filename);

if (IsValidPath(filename) == false)
{

    JS_ReportError(cx, "Invalid filename: '%s'", filename);
    return JS_FALSE;
}
// load window style
SWINDOWSTYLE* windowstyle = This->m_Engine->LoadWindowStyle(filename);
if (windowstyle == NULL)
{
    JS_ReportError(cx, "Could not load windowstyle '%s'", filename);
    return JS_FALSE;
}

return_object(CreateWindowStyleObject(cx, windowstyle, true));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns an image object of the System Arrow that the engine currently uses.
*/
begin_func(GetSystemArrow, 0)
return_object(CreateImageObject(cx, This->m_Engine->GetSystemArrow(), false));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns an image object of the System Arrow(up) that the engine currently
      uses.
*/
begin_func(GetSystemUpArrow, 0)
return_object(CreateImageObject(cx, This->m_Engine->GetSystemUpArrow(), false));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns an image object of the System Arrow(down) that the engine
      currently uses.
*/
begin_func(GetSystemDownArrow, 0)
return_object(CreateImageObject(cx, This->m_Engine->GetSystemDownArrow(), false));
end_func()

////////////////////////////////////////////////////////////////////////////////
// section: image functions //
/**
    - returns an image object from 'filename'. If Sphere is unable to open or
      read the image, the engine will give an error message and exit. The
      image type that the engine supports are either PCX, BMP, JPG, and PNG.
*/
begin_func(LoadImage, 1)
arg_str(filename);

if (IsValidPath(filename) == false)
{

    JS_ReportError(cx, "Invalid filename: '%s'", filename);
    return JS_FALSE;
}
// load image
IMAGE image = This->m_Engine->LoadImage(filename);
if (image == NULL)
{
    JS_ReportError(cx, "Could not load image '%s'", filename);
    return JS_FALSE;
}

return_object(CreateImageObject(cx, image, true));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns an image object from a section of the video buffer
      defined by the parameters
*/
begin_func(GrabImage, 4)
arg_int(x);
arg_int(y);
arg_int(w);
arg_int(h);

if (w <= 0 || h <= 0)
{

    JS_ReportError(cx, "Invalid width or height: (%d, %d)\n", w, h);
    return JS_FALSE;
}
// make sure we don't go off the screen
if (x < 0)
{
    w += x;
    x = 0;
}

if (y < 0)
{
    h += y;
    y = 0;
}

if (x + w > GetScreenWidth())
{
    w = GetScreenWidth() - x;
}

if (y + h > GetScreenHeight())
{
    h = GetScreenHeight() - y;
}

return_object(CreateImageObject(cx, GrabImage(x, y, w, h), true));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns a surface object with width x height, filled with color
*/
begin_func(CreateSurface, 3)
arg_int(w);
arg_int(h);
arg_color(c);

if (w < 0 || h < 0)
{
    JS_ReportError(cx, "CreateSurface() failed\nSurfaces cannot have negative sizes...\n'%d by %d'", w, h);
    return JS_FALSE;
}

CImage32* surface = new CImage32(w, h);
if (!surface || surface->GetWidth() != w || surface->GetHeight() != h)
{

    if (surface != NULL)
    {

        delete surface;
        surface = NULL;
    }
    JS_ReportError(cx, "CreateSurface() failed!!");
    return JS_FALSE;
}
surface->SetBlendMode(CImage32::REPLACE);
surface->Rectangle(0, 0, w, h, c);
surface->SetBlendMode(CImage32::BLEND);

return_object(CreateSurfaceObject(cx, surface));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns a surface object with an image with the 'filename'
*/
begin_func(LoadSurface, 1)
arg_str(filename);

if (IsValidPath(filename) == false)
{

    JS_ReportError(cx, "Invalid filename: '%s'", filename);
    return JS_FALSE;
}
// load surface
CImage32* surface = This->m_Engine->LoadSurface(filename);
if (surface == NULL)
{
    JS_ReportError(cx, "Could not load surface '%s'", filename);
    return JS_FALSE;
}

return_object(CreateSurfaceObject(cx, surface));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - returns a surface object captured from an area of the video buffer,
      at (x, y) with the width w and height h.
    @see GrabImage
*/
begin_func(GrabSurface, 4)
arg_int(x);
arg_int(y);
arg_int(w);
arg_int(h);

if (w <= 0 || h <= 0)
{

    JS_ReportError(cx, "Invalid width or height: (%d, %d)\n", w, h);
    return JS_FALSE;
}
// make sure we don't go off the screen
if (x < 0)
{
    w += x;
    x = 0;
}

if (y < 0)
{
    h += y;
    y = 0;
}

if (x + w > GetScreenWidth())
{
    w = GetScreenWidth() - x;
}

if (y + h > GetScreenHeight())
{
    h = GetScreenHeight() - y;
}

// create surface and grab pixels from the backbuffer
CImage32* surface = new CImage32(w, h);
if (!surface || surface->GetWidth() != w || surface->GetHeight() != h || surface->GetPixels() == NULL)
{

    if (surface != NULL)
    {

        delete surface;
        surface = NULL;
    }
    JS_ReportError(cx, "GrabSurface() failed!!");
    return JS_FALSE;
}
DirectGrab(x, y, w, h, surface->GetPixels());
return_object(CreateSurfaceObject(cx, surface));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - Creates a colormatrix that is used to transform the colors
      contained in a pixel with the following formula:
        newcolor.red   = rn + (rr * oldcolor.red + rg * oldcolor.green + rb * oldcolor.blue) / 255;
        newcolor.green = gn + (gr * oldcolor.red + gg * oldcolor.green + gb * oldcolor.blue) / 255;
        newcolor.blue  = bn + (br * oldcolor.red + bg * oldcolor.green + bb * oldcolor.blue) / 255;
    @see surface.applyColorFX
    @see surface.applyColorFX4
*/
begin_func(CreateColorMatrix, 12)
arg_int(rn);
arg_int(rr);
arg_int(rg);
arg_int(rb);
arg_int(gn);
arg_int(gr);
arg_int(gg);
arg_int(gb);
arg_int(bn);
arg_int(br);
arg_int(bg);
arg_int(bb);

// create surface and grab pixels from the backbuffer
CColorMatrix* colormatrix = new CColorMatrix(rn, rr, rg, rb, gn, gr, gg, gb, bn, br, bg, bb);
if (colormatrix == NULL)
{

    return JS_FALSE;
}
return_object(CreateColorMatrixObject(cx, colormatrix));
end_func()

////////////////////////////////////////////////////////////////////////////////
// section: animation functions //
/**
    - Returns an animation object with the filename. If Sphere is unable to
      open the file, the engine will give an error message and exit. Sphere
      supports animation formats of .flic, .fli, .flc and .mng, .jng
*/
begin_func(LoadAnimation, 1)
arg_str(filename);

if (IsValidPath(filename) == false)
{

    JS_ReportError(cx, "Invalid path: '%s'", filename);
    return JS_FALSE;
}
// load animation
IAnimation* animation = This->m_Engine->LoadAnimation(filename);
if (animation == NULL)
{
    JS_ReportError(cx, "Could not load animation '%s'", filename);
    return JS_FALSE;
}

return_object(CreateAnimationObject(cx, animation));
end_func()

////////////////////////////////////////////////////////////////////////////////
// section: File related functions //
/**
    - rename or move a file or directory
*/
begin_func(Rename, 2)

arg_str(old_path);
arg_str(new_path);

if (IsValidPath(old_path, true) == false)
{
    JS_ReportError(cx, "Invalid path: '%s'", old_path);
    return JS_FALSE;
}
if (IsValidPath(new_path, true) == false)
{
    JS_ReportError(cx, "Invalid path: '%s'", new_path);
    return JS_FALSE;
}

return_bool(This->m_Engine->Rename(old_path, new_path));

end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - create a new directory
*/
begin_func(CreateDirectory, 1)

arg_str(directory);

if (IsValidPath(directory, true) == false)
{
    JS_ReportError(cx, "Invalid path: '%s'", directory);
    return JS_FALSE;
}

return_bool(This->m_Engine->CreateDirectory(directory));

end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - remove the specified directory
*/
begin_func(RemoveDirectory, 1)

arg_str(directory);

if (IsValidPath(directory, true) == false)
{
    JS_ReportError(cx, "Invalid path: '%s'", directory);
    return JS_FALSE;
}

return_bool(This->m_Engine->RemoveDirectory(directory));

end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - remove the specified file.
      This start from your game directory, so if you want to delete a logfile:
      RemoveFile('logs/mylog.log');
*/
begin_func(RemoveFile, 1)

arg_str(filename);

if (IsValidPath(filename, true) == false)
{
    JS_ReportError(cx, "Invalid path: '%s'", filename);
    return JS_FALSE;
}

return_bool(This->m_Engine->RemoveFile(filename));

end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - directory = directory in which to enumerate directories,
      current game's directory if not specified

    - returns an array of strings, which contains the directory names that resides
      in the 'directory' directory of the game.
*/
begin_func(GetDirectoryList, 0)

const char* directory = "~/";
if (argc > 0)
{
    directory = argStr(cx, argv[0]);
    __arg_error_check__("string");
}

if (IsValidPath(directory) == false)
{
    JS_ReportError(cx, "Invalid path: '%s'", directory);
    return JS_FALSE;
}

// get the list of directories
std::vector<std::string> directories;
This->m_Engine->GetDirectoryList(directory, directories);

// convert it to an array of jsvals
jsval* js_vs = new jsval[directories.size()];
if (!js_vs)
{
    return JS_FALSE;
}
*rval = *js_vs;
for (unsigned int i = 0; i < directories.size(); i++)
{
    js_vs[i] = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, directories[i].c_str()));
}

return_object(JS_NewArrayObject(cx, directories.size(), js_vs));
delete[] js_vs;

end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - directory = directory in which to enumerate files, "save" if not specified

    - returns an array of strings, which contains the filenames that resides
      in the 'directory' directory of the game.
*/
begin_func(GetFileList, 0)

const char* directory = "save";
if (argc > 0)
{
    directory = argStr(cx, argv[0]);
    __arg_error_check__("string")
}

if (IsValidPath(directory) == false)
{
    JS_ReportError(cx, "Invalid path: '%s'", directory);
    return JS_FALSE;
}

// enumerate the files and return an array of string objects
// get list of files from engine
std::vector<std::string> vs;
This->m_Engine->GetFileList(directory, vs);

// convert it to an array of jsvals
jsval* js_vs = new jsval[vs.size()];
if (!js_vs)
{
    return JS_FALSE;
}
*rval = *js_vs;

for (unsigned int i = 0; i < vs.size(); i++)
{
    js_vs[i] = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, vs[i].c_str()));
}

return_object(JS_NewArrayObject(cx, vs.size(), js_vs));
delete[] js_vs;

end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - returns a file object with the filename. The file is created/loaded
      from the "save" directory of the game. Note that any changes in the keys
      will not be saved until the file object is destroyed.
*/
begin_func(OpenFile, 1)
arg_str(filename);

if (IsValidPath(filename) == false)
{
    JS_ReportError(cx, "Invalid path: '%s'", filename);
    return JS_FALSE;
}

// open file
CConfigFile* file = This->m_Engine->OpenFile(filename);
if (file == NULL)
{
    JS_ReportError(cx, "Could not open file '%s'", filename);
    return JS_FALSE;
}

return_object(CreateFileObject(cx, file));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - opens a file with the filename.
      Unless writeable is true, the file *must* exist and reside
      in the "other" directory of the game otherwise Sphere will give an error.
      If the file is opened successfully, the function will return a rawfile
      object. If the optional argument 'writeable' is true,
      the contents of the file will be destroyed,
      and you will be able to write to the file.
*/
begin_func(OpenRawFile, 1)
arg_str(filename);

if (IsValidPath(filename) == false)
{
    JS_ReportError(cx, "Invalid path: '%s'", filename);
    return JS_FALSE;
}

bool writeable = false;
if (argc > 1)
{
    writeable = argBool(cx, argv[arg++]);
}

// open file
IFile* file = This->m_Engine->OpenRawFile(filename, writeable);
if (file == NULL)
{
    JS_ReportError(cx, "Could not open raw file '%s'", filename);
    return JS_FALSE;
}

return_object(CreateRawFileObject(cx, file, writeable));
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - opens a log file for use under the filename. If Sphere is unable to open
      the file for logging, the engine will give an error message and exit.
      If Sphere is successful in opening the file, it will return a log object
      for use.
      ex: var myLog = OpenLog("game.log");
*/
begin_func(OpenLog, 1)
arg_str(filename);
if (IsValidPath(filename) == false)
{

    JS_ReportError(cx, "Invalid path: '%s'", filename);
    return JS_FALSE;
}
CLog* log = This->m_Engine->OpenLog(filename);
if (log == NULL)
{

    JS_ReportError(cx, "OpenLog() failed: Could not open file '%s'", filename);
    return JS_FALSE;
}
return_object(CreateLogObject(cx, log));
end_func()
///////////////////////////////////////////////////////////
/**
    - Gets the currently open mapengine object...
*/
begin_func(GetMapEngine, 0)
if ( !This->m_Engine->GetMapEngine()->IsRunning() )
{

    This->ReportMapEngineError("GetMapEngine() failed");
    return JS_FALSE;
}
static JSClass clasp =
    {

        "map", JSCLASS_HAS_PRIVATE,
        JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
        JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    };
// create the object
JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);
if (!object || !JS_AddRoot(cx, &object))
    return NULL;

// assign methods to the object
static JSFunctionSpec fs[] =
    {

        { "save",        ssMapEngineSave,        0, 0, 0
        },

        { "appendLayer", ssMapEngineLayerAppend, 1, 0, 0 },
        { 0, 0, 0, 0, 0 },
    };
JS_DefineFunctions(cx, object, fs);
SS_MAPENGINE* mapengine_object = new SS_MAPENGINE;
if (!mapengine_object)
{
    JS_RemoveRoot(cx, &object);
    return_object(JSVAL_NULL);
}
mapengine_object->__value__ = 1;
JS_SetPrivate(cx, object, mapengine_object);
JS_DefineProperty(cx, object, "tileset",
                  OBJECT_TO_JSVAL(CreateTilesetObject(cx, This->m_Engine->GetMapEngine()->GetMap().GetMap().GetTileset())),
                  JS_PropertyStub, JS_PropertyStub, JSPROP_READONLY | JSPROP_PERMANENT);
return_object(object);
JS_RemoveRoot(cx, &object);
end_func()
////////////////////////////////////////////////////////////////////////////////
/**
    - generates an MD5 of the specified raw file, which, by backtracking
      with a ../ may refer to any file within the game directory structure.
      The result is a 32 character string containing the hexadecimal
      representation of the resulting 128-bit MD5 fingerprint.  Identical
      files produce the same MD5 hash so it is an effective way to determine
      if a file has become corrupt or altered
*/
begin_func(HashFromFile, 1)
arg_str(filename);

if (IsValidPath(filename) == false)
{
    JS_ReportError(cx, "Invalid path: '%s'", filename);
    return JS_FALSE;
}

IFile* infile=This->m_Engine->OpenRawFile(filename,false);
if (infile==NULL)
{
    JS_ReportError(cx, "HashFromFile could not open '%s'", filename);
    return JS_FALSE;
}

MD5_CTX ctx;
MD5Init(&ctx);

// Read file and update hash as blocks are read
byte chunk[4096];
int todo=0;
for (todo=infile->Size(); todo>4095; todo-=4096)
{
    infile->Read(chunk,4096);
    MD5Update(&ctx,chunk,4096);
}
// read last bit
if (todo>0)
{
    infile->Read(chunk,todo);
    MD5Update(&ctx,chunk,todo);
}
delete infile;

// get the hash
unsigned char digest[16];
MD5Final(digest,&ctx);

char retval[33];
char* sptr;
// Format into a string
sptr=retval;
for (int i=0;i<16;++i)
{
    sprintf(sptr,"%02x",digest[i]);
    ++sptr;
    ++sptr;
}
retval[32]='\0';

return_str(retval);
end_func()

////////////////////////////////////////////////////////////////////////////////
/**
    - generates an MD5 fingerprint as a hexadecimal string whose output
      is the same as the RSA reference implementation of RFC 1321 which
      means that the unix md5 command will return the same string for the
      identical input.  The resulting string contains the hexadecimal
      representation of the MD5 fingerprint for the specified byte
      array object
*/
begin_func(HashByteArray, 1)
arg_byte_array(array);
int len,i;
unsigned char* bptr;
char* sptr;
unsigned char digest[16];
char retval[33];
MD5_CTX ctx;

// Set up data
len=array->size;
bptr=(unsigned char*)array->array;

// Generate MD5 hash on it
MD5Init(&ctx);
MD5Update(&ctx,bptr,len);
MD5Final(digest,&ctx);

// Format into a string
sptr=retval;
for (i=0;i<16;++i)
{
    sprintf(sptr,"%02x",digest[i]);
    ++sptr;
    ++sptr;
}
retval[32]='\0';

return_str(retval);
end_func()


/**
    - Decompress a file to another file using deflate
      The default base directory is other/, use ../ to reach other directories.
      compressionlevel must be between 0 and 9, and is 6 by default
      Returns zero on succes, nonzero if it failed.
      The return numbers are (the constants are not defined in Sphere):
      Z_OK            0
      Z_STREAM_END    1
      Z_NEED_DICT     2
      Z_ERRNO        (-1)
      Z_STREAM_ERROR (-2)
      Z_DATA_ERROR   (-3)
      Z_MEM_ERROR    (-4)
      Z_BUF_ERROR    (-5)
      Z_VERSION_ERROR (-6)
*/
begin_func(DeflateFile, 2)
arg_str(filename_source);
arg_str(filename_dest);
int compressionlevel = 6;
if (argc > 2)
{
    compressionlevel = argInt(cx, argv[arg++]);
    if ( compressionlevel < 0 )
        compressionlevel = 0;
    if ( compressionlevel > 9 )
        compressionlevel = 9;
}

if (filename_source == filename_dest)
{
    JS_ReportError(cx, "Cannot copy file onto itself: '%s'", filename_source);
    return JS_FALSE;
}

std::string source = "other/";
source += filename_source;
filename_source = source.c_str();

std::string dest = "other/";
dest += filename_dest;
filename_dest = dest.c_str();

if (IsValidPath(filename_source) == false)
{
    JS_ReportError(cx, "Invalid source path: '%s'", filename_source);
    return JS_FALSE;
}

if (!DoesFileExist(filename_source))
{
    JS_ReportError(cx, "Source file not found: '%s'", filename_source);
    return JS_FALSE;
}

if (IsValidPath(filename_dest) == false)
{
    JS_ReportError(cx, "Invalid destination path: '%s'", filename_dest);
    return JS_FALSE;
}


// open file, and create the 'other' directory if its not there
std::string slash = "../";
slash += filename_dest;
IFile* file = This->m_Engine->OpenRawFile(slash.c_str(), true);
if (file == NULL)
{
    JS_ReportError(cx, "Could not write to deflated file '%s'", slash.c_str());
    return JS_FALSE;
}
delete file;

ZlibEngine engine;
return_int( engine.compress( filename_source, filename_dest, compressionlevel) );
JS_MaybeGC(cx);
end_func()

/**
    - Decompress a file to another file using deflate
      The default base directory is other/, use ../ to reach other directories.
      Returns zero on succes, nonzero if it failed. (same codes as DeflateFile)
*/
begin_func(InflateFile, 1)
arg_str(filename_source);
arg_str(filename_dest);

if (filename_source == filename_dest)
{
    JS_ReportError(cx, "Cannot copy file onto itself: '%s'", filename_source);
    return JS_FALSE;
}

std::string source = "other/";
source += filename_source;
filename_source = source.c_str();

std::string dest = "other/";
dest += filename_dest;
filename_dest = dest.c_str();

if (IsValidPath(filename_source) == false)
{
    JS_ReportError(cx, "Invalid source path: '%s'", filename_source);
    return JS_FALSE;
}

if (!DoesFileExist(filename_source))
{
    JS_ReportError(cx, "File not found: '%s'", filename_source);
    return JS_FALSE;
}

if (IsValidPath(filename_dest) == false)
{
    JS_ReportError(cx, "Invalid destination path: '%s'", filename_dest);
    return JS_FALSE;
}

// open file, and create the 'other' directory if its not there
std::string slash = "../";
slash += filename_dest;
IFile* file = This->m_Engine->OpenRawFile(slash.c_str(), true);
if (file == NULL)
{
    JS_ReportError(cx, "Could not write to inflated file '%s'", slash.c_str());
    return JS_FALSE;
}
delete file;

ZlibEngine engine;
return_int( engine.decompress( filename_source, filename_dest) );
JS_MaybeGC(cx);
end_func()

/**
    - Compress a bytearray and return a deflated version.
      compressionlevel must be between 0 and 9, and is 6 by default.
      You can limit the maximum allowed size with maxsize.
      Returns null on failure.
      Note: for small strings, the compressed version could actually be bigger.
*/
begin_func(DeflateByteArray, 1)
arg_byte_array(ba);
int compressionlevel = 6;
if (argc > 1)
{
    compressionlevel = argInt(cx, argv[arg++]);
    if ( compressionlevel < 0 )
        compressionlevel = 0;
    if ( compressionlevel > 9 )
        compressionlevel = 9;
}

ZlibEngine engine;

unsigned int maxsize = engine.compressBound2(ba->size);
if (argc > 2)
{
    maxsize = argInt(cx, argv[arg++]);
    if (maxsize<=0)
        maxsize = engine.compressBound2(ba->size);
}

if (maxsize < engine.compressBound2(ba->size) )
{
    return_null();
} else {

    unsigned char* buffer = new unsigned char[maxsize];
    if (buffer == 0)
    {
        JS_ReportError(cx, "Failed to allocate %i bytes for ByteArray deflation", maxsize);
        return JS_FALSE;
    }

    int outputsize = engine.compressInMemory(ba->array, ba->size,(unsigned char *) buffer, maxsize, compressionlevel);
    if (outputsize < 0)
    {
        delete[] buffer;
        return_null();
    } else 
    {
        JSObject* array_object = CreateByteArrayObject(cx, outputsize, (byte *) buffer);
        delete[] buffer; // unsigned char* buffer = new unsigned char[maxsize];

        return_object(array_object);
    }
}
end_func()

/**
    - Decompress a bytearray and return an inflated version.
      You can limit the maximum allowed size with maxsize, if you don't
      it will set an estimation for you, which could be too low.
      Returns null on failure.
*/
begin_func(InflateByteArray, 1)
arg_byte_array(ba);

ZlibEngine engine;

unsigned int maxsize = engine.compressBound2(ba->size);

if (argc > 1)
{
    maxsize = argInt(cx, argv[arg++]);
    if (maxsize<=0)
        maxsize = engine.compressBound2(ba->size);
}

if (maxsize < engine.compressBound2(ba->size) )
{
    return_null();
} else 
{

    unsigned char* buffer = new unsigned char[maxsize];
    if (buffer == 0)
    {
        JS_ReportError(cx, "Failed to allocate %i bytes for ByteArray inflation", maxsize);
        return JS_FALSE;
    }

    int outputsize = engine.decompressInMemory(ba->array, ba->size,(unsigned char *) buffer, maxsize);
    if (outputsize < 0)
    {
        delete[] buffer;
        return_null();
    }else{

        JSObject* byte_array_object = CreateByteArrayObject(cx, outputsize, (byte *) buffer);
        delete[] buffer;

        return_object(byte_array_object);
    }
}
end_func()


////////////////////////////////////////////////////////////////////////////////
// OBJECTS
#define begin_finalizer(Object, name)                   \
  void CScript::name(JSContext* cx, JSObject* obj)      \
  {                                                     \
    CScript* This = (CScript*)JS_GetContextPrivate(cx); \
    USED(This);                                         \
    Object* object = (Object*)JS_GetPrivate(cx, obj);   \
    if (!object) {                                      \
      return;                                           \
    }

#define end_finalizer()           \
    delete object;                \
    JS_SetPrivate(cx, obj, NULL); \
  }

#define begin_property(Object, name)                                      \
  JSBool CScript::name(JSContext* cx, JSObject* obj, jsval id, jsval* vp) \
  {                                                                       \
    CScript* This = (CScript*)JS_GetContextPrivate(cx);                   \
    USED(This);                                                           \
    Object* object = (Object*)JS_GetPrivate(cx, obj);

#define end_property()                                \
    return (This->m_ShouldExit ? JS_FALSE : JS_TRUE); \
  }

#define begin_method(Object, name, minargs)                                                            \
  JSBool CScript::name(JSContext* cx, JSObject* obj, uintN argc, jsval* argv, jsval* rval) {           \
    const char* script_name = #name;                                                                   \
    CScript* This = (CScript*)JS_GetContextPrivate(cx);                                                \
    NoGCBlock no_gc__(This);                                                                           \
    Object* object = (Object*)JS_GetPrivate(cx, obj);                                                  \
    if (object == NULL || object->magic != Object##_MAGIC) {                /* invalid object */       \
      JS_ReportError(cx, "%s called on invalid object", #name, #minargs);                              \
      return JS_FALSE;                                                                                 \
    }                                                                                                  \
    if (argc < minargs) {                                                                              \
      JS_ReportError(cx, "%s called with less than %s parameters", #name, #minargs);                   \
      return JS_FALSE;                                                                                 \
    }                                                                                                  \
    int arg = 0;                                                                                       \
    USED(arg);

#define end_method()                                  \
    return (This->m_ShouldExit ? JS_FALSE : JS_TRUE); \
  }


////////////////////////////////////////
// PARTICLE ENGINE OBJECTS /////////////
////////////////////////////////////////

////////////////////////////////////////
// PARTICLE BODY OBJECT ////////////////
////////////////////////////////////////

JSObject*
CScript::CreateParticleBodyObject(JSContext* cx, ParticleSystemBase* system)
{
    if (!system)
        return NULL;

    static JSClass clasp =
    {
        "particle body", JSCLASS_HAS_PRIVATE,
        JS_PropertyStub,  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
        JS_EnumerateStub, JS_ResolveStub,  JS_ConvertStub,  ssFinalizeParticleBody,
    };

    // create the object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);

    if (!object || !JS_AddRoot(cx, &object))
        return NULL;

    // assign properties to the object
    static JSPropertySpec ps[] =
    {
        { "x",     0, JSPROP_PERMANENT, ssParticleBodyGetProperty, ssParticleBodySetProperty },
        { "y",     1, JSPROP_PERMANENT, ssParticleBodyGetProperty, ssParticleBodySetProperty },
        { "vel_x", 2, JSPROP_PERMANENT, ssParticleBodyGetProperty, ssParticleBodySetProperty },
        { "vel_y", 3, JSPROP_PERMANENT, ssParticleBodyGetProperty, ssParticleBodySetProperty },
        { "life",  4, JSPROP_PERMANENT, ssParticleBodyGetProperty, ssParticleBodySetProperty },
        { "aging", 5, JSPROP_PERMANENT, ssParticleBodyGetProperty, ssParticleBodySetProperty },
        { 0, 0, 0, 0, 0 },
    };
    JS_DefineProperties(cx, object, ps);

    SS_PARTICLE_BODY* body_object = new SS_PARTICLE_BODY;

    if (!body_object)
    {
        // balancing call to JS_RemoveRoot
        JS_RemoveRoot(cx, &object);
        return NULL;
    }

    body_object->system = system;
    JS_SetPrivate(cx, object, body_object);

    // balancing call to JS_RemoveRoot
    JS_RemoveRoot(cx, &object);

    return object;
}

////////////////////////////////////////
begin_finalizer(SS_PARTICLE_BODY, ssFinalizeParticleBody)
// the body object should not delete the superior particle system
object->system = NULL;
end_finalizer()

////////////////////////////////////////
begin_property(SS_PARTICLE_BODY, ssParticleBodyGetProperty)
int prop_id = argInt(cx, id);
switch (prop_id)
{
case 0:
    *vp = DOUBLE_TO_JSVAL(JS_NewDouble(cx, (double)object->system->GetBody().Pos.X));
    break;
case 1:
    *vp = DOUBLE_TO_JSVAL(JS_NewDouble(cx, (double)object->system->GetBody().Pos.Y));
    break;
case 2:
    *vp = DOUBLE_TO_JSVAL(JS_NewDouble(cx, (double)object->system->GetBody().Vel.X));
    break;
case 3:
    *vp = DOUBLE_TO_JSVAL(JS_NewDouble(cx, (double)object->system->GetBody().Vel.Y));
    break;
case 4:
    *vp = DOUBLE_TO_JSVAL(JS_NewDouble(cx, (double)object->system->GetBody().Life));
    break;
case 5:
    *vp = DOUBLE_TO_JSVAL(JS_NewDouble(cx, (double)object->system->GetBody().Aging));
    break;
default:
    *vp = JSVAL_NULL;
    break;
}
end_property()

////////////////////////////////////////
begin_property(SS_PARTICLE_BODY, ssParticleBodySetProperty)
int prop_id = argInt(cx, id);
switch (prop_id)
{
case 0:
    object->system->GetBody().Pos.X = (float)argDouble(cx, *vp);
    break;
case 1:
    object->system->GetBody().Pos.Y = (float)argDouble(cx, *vp);
    break;
case 2:
    object->system->GetBody().Vel.X = (float)argDouble(cx, *vp);
    break;
case 3:
    object->system->GetBody().Vel.Y = (float)argDouble(cx, *vp);
    break;
case 4:
    object->system->GetBody().Life  = (float)argDouble(cx, *vp);
    break;
case 5:
    object->system->GetBody().Aging = (float)argDouble(cx, *vp);
    break;
}
end_property()

////////////////////////////////////////
// PARTICLE INITIALIZER OBJECT /////////
////////////////////////////////////////

JSObject*
CScript::CreateParticleInitializerObject(JSContext* cx, ParticleSystemBase* system)
{
    if (!system)
        return NULL;

    static JSClass clasp =
    {
        "particle initializer", JSCLASS_HAS_PRIVATE,
        JS_PropertyStub,  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
        JS_EnumerateStub, JS_ResolveStub,  JS_ConvertStub,  ssFinalizeParticleInitializer,
    };

    // create the object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);

    if (!object || !JS_AddRoot(cx, &object))
        return NULL;

    // assign methods to the object
    static JSFunctionSpec fs[] =
    {
        { "getAgingParams",   ssParticleInitializerGetAgingParams,   0, 0, 0 },
        { "setAgingParams",   ssParticleInitializerSetAgingParams,   1, 0, 0 },
        { "getPosParams",     ssParticleInitializerGetPosParams,     0, 0, 0 },
        { "setPosParams",     ssParticleInitializerSetPosParams,     2, 0, 0 },
        { "getVelParams",     ssParticleInitializerGetVelParams,     0, 0, 0 },
        { "setVelParams",     ssParticleInitializerSetVelParams,     1, 0, 0 },
        { 0, 0, 0, 0, 0 },
    };
    JS_DefineFunctions(cx, object, fs);

    SS_PARTICLE_INITIALIZER* initializer_object = new SS_PARTICLE_INITIALIZER;

    if (!initializer_object)
    {
        // balancing call to JS_RemoveRoot
        JS_RemoveRoot(cx, &object);

        return NULL;
    }

    initializer_object->system = system;
    JS_SetPrivate(cx, object, initializer_object);

    // balancing call to JS_RemoveRoot
    JS_RemoveRoot(cx, &object);

    return object;
}

////////////////////////////////////////
begin_finalizer(SS_PARTICLE_INITIALIZER, ssFinalizeParticleInitializer)
// the initializer object should not delete the superior particle system
object->system = NULL;
end_finalizer()

////////////////////////////////////////
/*
 * HELPER FUNCTIONS
 */
 ////////////////////////////////////////
static void
fill_range_object(JSContext* cx, JSObject* rooted_out, Range<float> range)
{
    if (!rooted_out)
        return;

    JS_DefineProperty(cx, rooted_out, "min", DOUBLE_TO_JSVAL(JS_NewDouble(cx, (double)range.Min)),
                      JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE);

    JS_DefineProperty(cx, rooted_out, "max", DOUBLE_TO_JSVAL(JS_NewDouble(cx, (double)range.Max)),
                      JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE);

}

////////////////////////////////////////
static void
fill_params_from_rectangular_range(JSContext* cx, JSObject* rooted_out, RectangularRange<float> range)
{
    if (!rooted_out)
        return;

    JSObject* x_obj = JS_NewObject(cx, NULL, NULL, NULL);
    JS_DefineProperty(cx, rooted_out, "x", OBJECT_TO_JSVAL(x_obj),
                      JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE);
    fill_range_object(cx, x_obj, range.X);

    JSObject* y_obj = JS_NewObject(cx, NULL, NULL, NULL);
    JS_DefineProperty(cx, rooted_out, "y", OBJECT_TO_JSVAL(y_obj),
                      JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE);
    fill_range_object(cx, y_obj, range.Y);
}

////////////////////////////////////////
static void
fill_params_from_elliptical_range(JSContext* cx, JSObject* rooted_out, EllipticalRange<float> range)
{
    if (!rooted_out)
        return;

    JSObject* angle_obj = JS_NewObject(cx, NULL, NULL, NULL);
    JS_DefineProperty(cx, rooted_out, "angle", OBJECT_TO_JSVAL(angle_obj),
                      JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE);
    fill_range_object(cx, angle_obj, range.Angle);

    JSObject* a_obj = JS_NewObject(cx, NULL, NULL, NULL);
    JS_DefineProperty(cx, rooted_out, "a", OBJECT_TO_JSVAL(a_obj),
                      JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE);
    fill_range_object(cx, a_obj, range.A);

    JSObject* b_obj = JS_NewObject(cx, NULL, NULL, NULL);
    JS_DefineProperty(cx, rooted_out, "b", OBJECT_TO_JSVAL(b_obj),
                      JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE);
    fill_range_object(cx, b_obj, range.B);

}

////////////////////////////////////////
static void
fill_range_from_params(JSContext* cx, JSObject* rooted_in, Range<float>& range)
{
    if (!rooted_in)
        return;

    jsval val;
    jsdouble jsd;

    if (JS_GetProperty(cx, rooted_in, "min", &val) &&
        JSVAL_IS_NUMBER(val) &&
        JS_ValueToNumber(cx, val, &jsd))
    {
        range.Min = (float)jsd;
    }

    if (JS_GetProperty(cx, rooted_in, "max", &val) &&
        JSVAL_IS_NUMBER(val) &&
        JS_ValueToNumber(cx, val, &jsd))
    {
        range.Max = (float)jsd;
    }

}

////////////////////////////////////////
static void
fill_rectangular_range_from_params(JSContext* cx, JSObject* rooted_in, RectangularRange<float>& range)
{
    if (!rooted_in)
        return;

    jsval val;

    if (JS_GetProperty(cx, rooted_in, "x", &val) &&
        JSVAL_IS_OBJECT(val) &&
        !JSVAL_IS_NULL(val))
    {
        fill_range_from_params(cx, JSVAL_TO_OBJECT(val), range.X);
    }

    if (JS_GetProperty(cx, rooted_in, "y", &val) &&
        JSVAL_IS_OBJECT(val) &&
        !JSVAL_IS_NULL(val))
    {
        fill_range_from_params(cx, JSVAL_TO_OBJECT(val), range.Y);
    }

}

////////////////////////////////////////
static void
fill_elliptical_range_from_params(JSContext* cx, JSObject* rooted_in, EllipticalRange<float>& range)
{
    if (!rooted_in)
        return;

    jsval val;

    if (JS_GetProperty(cx, rooted_in, "angle", &val) &&
        JSVAL_IS_OBJECT(val) &&
        !JSVAL_IS_NULL(val))
    {
        fill_range_from_params(cx, JSVAL_TO_OBJECT(val), range.Angle);
    }

    if (JS_GetProperty(cx, rooted_in, "a", &val) &&
        JSVAL_IS_OBJECT(val) &&
        !JSVAL_IS_NULL(val))
    {
        fill_range_from_params(cx, JSVAL_TO_OBJECT(val), range.A);
    }

    if (JS_GetProperty(cx, rooted_in, "b", &val) &&
        JSVAL_IS_OBJECT(val) &&
        !JSVAL_IS_NULL(val))
    {
        fill_range_from_params(cx, JSVAL_TO_OBJECT(val), range.B);
    }

}

////////////////////////////////////////
/* README!
 *
 * Kyuu: I'm using here rval and argv[i] explicitly, so it's clear what I want to do:
 *       rval, as well as argv[i], are always rooted, so I'm rooting the objects by
 *       an immediate assignment, which is a safe strategy and very convenient.
 *       Macro'ing rval and argv was a bad decision, IMO, as 'return_type()' and the
 *       argument macros are misleading and hide rval's and argv's usability.
 *       Not rooting the temporary values is the typical cause for mysterious crashes
 *       which can be VERY hard to debug if you don't know it's source: The JS GC.
 *       It will collect all unrooted JS values (using a already collected value will
 *       unavoidably lead to a segfault!) and can be invoked by almost all calls into
 *       the JSAPI, so you are most of the time only on the safe side if you root all
 *       your values carefully.
 *
 *       Note: Rooting strategies like using rval or argv[i] are highly encouraged
 *             by Mozilla's developers. argv can even be given more elements
 *             in the JSFunctionSpec declarations, just so the additional
 *             elements can be used to root values!
 *
 */
////////////////////////////////////////
/**
    - returns an wrapper object filled with aging initialization parameters
    - following signature is possible:
      {min: -, max: -}
*/
begin_method(SS_PARTICLE_INITIALIZER, ssParticleInitializerGetAgingParams, 0)
JSObject* params = JS_NewObject(cx, NULL, NULL, NULL);
if (!params)
{
    JS_ReportError(cx, "Could not create object.");
    return JS_FALSE;
}
// params is now rooted
*rval = OBJECT_TO_JSVAL(params);
fill_range_object(cx, params, object->system->GetInitializer().GetAging());
end_method()

////////////////////////////////////////
/**
    - accepts a wrapper object filled with aging initialization parameters to set
    - the same parameters are accepted as returned by getAgingParams()
    - parameters which should not be set, can be omitted
*/
begin_method(SS_PARTICLE_INITIALIZER, ssParticleInitializerSetAgingParams, 1)
// it is not guaranteed that this macro expansion won't create a new object
arg_object(params);
// params is now definitely rooted
argv[0] = OBJECT_TO_JSVAL(params);
fill_range_from_params(cx, params, object->system->GetInitializer().GetAging());
end_method()

////////////////////////////////////////
/**
    - returns a wrapper object filled with position initialization parameters
    - if 'shape' is not specified, the parameters for the current shape are returned,
      or if 'shape' is specified, the parameters belonging to the 'shape' are returned
    - following signatures are possible:

        {shape: PS_SHAPE_NULL}

        {shape: PS_SHAPE_RECTANGLE,
             x: {min: -, max: -},
             y: {min: -, max: -}}

        {shape: PS_SHAPE_ELLIPSE,
         angle: {min: -, max: -},
             a: {min: -, max: -},
             b: {min: -, max: -}}
*/
begin_method(SS_PARTICLE_INITIALIZER, ssParticleInitializerGetPosParams, 0)
int shape = object->system->GetInitializer().GetPositionMode();
if (argc >= 1)
{
    shape = argInt(cx, argv[0]);
    argv[0] = INT_TO_JSVAL(shape); // root as you go
}
JSObject* params = JS_NewObject(cx, NULL, NULL, NULL);
if (!params)
{
    JS_ReportError(cx, "Could not create object!");
    return JS_FALSE;
}
// params is now rooted
*rval = OBJECT_TO_JSVAL(params);
switch (shape)
{
    case ParticleInitializer::NULL_SHAPE:
    {
        JS_DefineProperty(cx, params, "shape",
                          INT_TO_JSVAL(ParticleInitializer::NULL_SHAPE),
                          JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE);
    }
    break;

    case ParticleInitializer::RECTANGULAR_SHAPE:
    {
        fill_params_from_rectangular_range(cx, params, object->system->GetInitializer().GetPosRectangle());
        JS_DefineProperty(cx, params, "shape",
                          INT_TO_JSVAL(ParticleInitializer::RECTANGULAR_SHAPE),
                          JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE);
    }
    break;

    case ParticleInitializer::ELLIPTICAL_SHAPE:
    {
        fill_params_from_elliptical_range(cx, params, object->system->GetInitializer().GetPosEllipse());
        JS_DefineProperty(cx, params, "shape",
                          INT_TO_JSVAL(ParticleInitializer::ELLIPTICAL_SHAPE),
                          JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE);
    }
    break;

    default:
    {
        JS_ReportError(cx, "Invalid shape specified!");
        return JS_FALSE;
    }
    break;
}
end_method()

////////////////////////////////////////
/**
    - accepts a wrapper object filled with position initialization parameters to set
    - the same parameters are accepted as returned by getPosParams()
    - parameters which should not be set, can be omitted
    - if 'shape' is omitted, the parameters are assumed to belong to the current shape
*/
begin_method(SS_PARTICLE_INITIALIZER, ssParticleInitializerSetPosParams, 1)
// it is not guaranteed that this macro expansion won't create a new object
arg_object(params);
// params is now definitely rooted
argv[0] = OBJECT_TO_JSVAL(params);
jsval val;
int shape = object->system->GetInitializer().GetPositionMode();
if (JS_GetProperty(cx, params, "shape", &val) &&
    JSVAL_IS_INT(val))
{
    shape = JSVAL_TO_INT(val);
}
switch (shape)
{
    case ParticleInitializer::NULL_SHAPE:
        // nothing to set...
        break;

    case ParticleInitializer::RECTANGULAR_SHAPE:
    {
        fill_rectangular_range_from_params(cx, params, object->system->GetInitializer().GetPosRectangle());
    }
    break;

    case ParticleInitializer::ELLIPTICAL_SHAPE:
    {
        fill_elliptical_range_from_params(cx, params, object->system->GetInitializer().GetPosEllipse());
    }
    break;

    default:
    {
        JS_ReportError(cx, "Invalid shape specified!");
        return JS_FALSE;
    }
    break;
}
object->system->GetInitializer().SetPositionMode(shape);
end_method()

////////////////////////////////////////
/**
    - returns a wrapper object filled with velocity initialization parameters
    - following signature is possible:

          {mode: -,
           angle: {min: -, max: -},
               a: {min: -, max: -},
               b: {min: -, max: -}}

      mode can be either PS_ORIENTATION_EXPLICIT or PS_ORIENTATION_IMPLICIT
*/
begin_method(SS_PARTICLE_INITIALIZER, ssParticleInitializerGetVelParams, 0)
JSObject* params = JS_NewObject(cx, NULL, NULL, NULL);
if (!params)
{
    JS_ReportError(cx, "Could not create object!");
    return JS_FALSE;
}
// params is now rooted
*rval = OBJECT_TO_JSVAL(params);
fill_params_from_elliptical_range(cx, params, object->system->GetInitializer().GetVelEllipse());
JS_DefineProperty(cx, params, "mode", INT_TO_JSVAL(object->system->GetInitializer().GetVelocityMode()), JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE);
end_method()

////////////////////////////////////////
/**
    - accepts a wrapper object filled with position initialization parameters to set
    - the same parameters are accepted as returned by getVelParams()
    - parameters which should not be set, can be omitted
*/
begin_method(SS_PARTICLE_INITIALIZER, ssParticleInitializerSetVelParams, 1)
// it is not guaranteed that this macro expansion won't create a new object
arg_object(params);
// params is now definitely rooted
argv[0] = OBJECT_TO_JSVAL(params);
jsval val;
if (JS_GetProperty(cx, params, "mode", &val) &&
    JSVAL_IS_INT(val))
{
    object->system->GetInitializer().SetVelocityMode(JSVAL_TO_INT(val));
}
fill_elliptical_range_from_params(cx, params, object->system->GetInitializer().GetVelEllipse());
end_method()

////////////////////////////////////////
// PARTICLE UPDATER OBJECT /////////////
////////////////////////////////////////

JSObject*
CScript::CreateParticleUpdaterObject(JSContext* cx, ParticleSystemBase* system)
{
    if (!system)
        return NULL;

    static JSClass clasp =
    {
        "particle updater", JSCLASS_HAS_PRIVATE,
        JS_PropertyStub,  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
        JS_EnumerateStub, JS_ResolveStub,  JS_ConvertStub,  ssFinalizeParticleUpdater,
    };

    // create the object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);

    if (!object || !JS_AddRoot(cx, &object))
        return NULL;

    // assign properties to the object
    static JSPropertySpec ps[] =
    {
        { "force_x", 0, JSPROP_PERMANENT, ssParticleUpdaterGetProperty, ssParticleUpdaterSetProperty },
        { "force_y", 1, JSPROP_PERMANENT, ssParticleUpdaterGetProperty, ssParticleUpdaterSetProperty },
        { "damping", 2, JSPROP_PERMANENT, ssParticleUpdaterGetProperty, ssParticleUpdaterSetProperty },
        { 0, 0, 0, 0, 0 },
    };
    JS_DefineProperties(cx, object, ps);

    SS_PARTICLE_UPDATER* updater_object = new SS_PARTICLE_UPDATER;

    if (!updater_object)
    {
        // balancing call to JS_RemoveRoot
        JS_RemoveRoot(cx, &object);

        return NULL;
    }

    updater_object->system = system;
    JS_SetPrivate(cx, object, updater_object);

    // balancing call to JS_RemoveRoot
    JS_RemoveRoot(cx, &object);

    return object;
}

////////////////////////////////////////
begin_finalizer(SS_PARTICLE_UPDATER, ssFinalizeParticleUpdater)
// the updater object should not delete the superior particle system
object->system = NULL;
end_finalizer()

////////////////////////////////////////
/**
    -
*/
begin_property(SS_PARTICLE_UPDATER, ssParticleUpdaterGetProperty)
int prop_id = argInt(cx, id);
switch (prop_id)
{
case 0:
    *vp = DOUBLE_TO_JSVAL(JS_NewDouble(cx, (double)object->system->GetUpdater().GetGlobalForce().X));
    break;
case 1:
    *vp = DOUBLE_TO_JSVAL(JS_NewDouble(cx, (double)object->system->GetUpdater().GetGlobalForce().Y));
    break;
case 2:
    *vp = DOUBLE_TO_JSVAL(JS_NewDouble(cx, (double)object->system->GetUpdater().GetDampingConstant()));
    break;
default:
    *vp = JSVAL_NULL;
    break;
}
end_property()

////////////////////////////////////////
/**
    -
*/
begin_property(SS_PARTICLE_UPDATER, ssParticleUpdaterSetProperty)
int prop_id = argInt(cx, id);
switch (prop_id)
{
case 0:
    object->system->GetUpdater().GetGlobalForce().X = (float)argDouble(cx, *vp);
    break;
case 1:
    object->system->GetUpdater().GetGlobalForce().Y = (float)argDouble(cx, *vp);
    break;
case 2:
    object->system->GetUpdater().SetDampingConstant((float)argDouble(cx, *vp));
    break;
}
end_property()

////////////////////////////////////////
// PARTICLE RENDERER OBJECT ////////////
////////////////////////////////////////

JSObject*
CScript::CreateParticleRendererObject(JSContext* cx, ParticleSystemBase* system)
{
    if (!system)
        return NULL;

    static JSClass clasp =
    {
        "particle renderer", JSCLASS_HAS_PRIVATE,
        JS_PropertyStub,  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
        JS_EnumerateStub, JS_ResolveStub,  JS_ConvertStub,  ssFinalizeParticleRenderer,
    };

    // create the object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);

    if (!object || !JS_AddRoot(cx, &object))
        return NULL;

    // assign properties to the object
    static JSPropertySpec ps[] =
    {
        { "texture",    0, JSPROP_PERMANENT, ssParticleRendererGetProperty, ssParticleRendererSetProperty },
        { "blend_mode", 1, JSPROP_PERMANENT, ssParticleRendererGetProperty, ssParticleRendererSetProperty },
        { "offset_x",   2, JSPROP_PERMANENT, ssParticleRendererGetProperty, ssParticleRendererSetProperty },
        { "offset_y",   3, JSPROP_PERMANENT, ssParticleRendererGetProperty, ssParticleRendererSetProperty },
        { "disabled",   4, JSPROP_PERMANENT, ssParticleRendererGetProperty, ssParticleRendererSetProperty },
        { 0, 0, 0, 0, 0 },
    };
    JS_DefineProperties(cx, object, ps);

    SS_PARTICLE_RENDERER* renderer_object = new SS_PARTICLE_RENDERER;

    if (!renderer_object)
    {
        // balancing call to JS_RemoveRoot
        JS_RemoveRoot(cx, &object);

        return NULL;
    }

    renderer_object->system = system;
    JS_SetPrivate(cx, object, renderer_object);

    // balancing call to JS_RemoveRoot
    JS_RemoveRoot(cx, &object);

    return object;
}

////////////////////////////////////////
begin_finalizer(SS_PARTICLE_RENDERER, ssFinalizeParticleRenderer)
// the renderer object should not delete the superior particle system
object->system = NULL;
end_finalizer()

////////////////////////////////////////
/**
    -
*/
begin_property(SS_PARTICLE_RENDERER, ssParticleRendererGetProperty)
int prop_id = argInt(cx, id);
switch (prop_id)
{
case 0:
    *vp = OBJECT_TO_JSVAL(object->system->GetScriptInterface().GetTextureObject());
    break;
case 1:
    *vp = INT_TO_JSVAL(object->system->GetRenderer().GetBlendMode());
    break;
case 2:
    *vp = INT_TO_JSVAL(object->system->GetRenderer().GetOffsetX());
    break;
case 3:
    *vp = INT_TO_JSVAL(object->system->GetRenderer().GetOffsetY());
    break;
case 4:
    *vp = INT_TO_JSVAL(object->system->GetRenderer().IsDisabled());
    break;
default:
    *vp = JSVAL_NULL;
    break;
}
end_property()

////////////////////////////////////////
begin_property(SS_PARTICLE_RENDERER, ssParticleRendererSetProperty)
int prop_id = argInt(cx, id);
switch (prop_id)
{
case 0:
    {
        SS_IMAGE* image = argImage(cx, *vp);

        if (image)
        {
            object->system->GetScriptInterface().SetTextureObject(JSVAL_TO_OBJECT(*vp));
            object->system->GetRenderer().SetTexture(image->image);
        }
        else
        {
            object->system->GetScriptInterface().SetTextureObject(NULL);
            object->system->GetRenderer().SetTexture(NULL);
        }
    }
    break;
case 1:
    object->system->GetRenderer().SetBlendMode(argInt(cx, *vp));
    break;
case 2:
    object->system->GetRenderer().SetOffsetX(argInt(cx, *vp));
    break;
case 3:
    object->system->GetRenderer().SetOffsetY(argInt(cx, *vp));
    break;
case 4:
    object->system->GetRenderer().Disable(argBool(cx, *vp));
    break;
}
end_property()

////////////////////////////////////////
// PARTICLE SYSTEM CALLBACK OBJECT /////
////////////////////////////////////////

JSObject*
CScript::CreateParticleCallbackObject(JSContext* cx, ParticleSystemBase* system)
{
    if (!system)
        return NULL;

    static JSClass clasp =
    {
        "particle callback", JSCLASS_HAS_PRIVATE,
        JS_PropertyStub,  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
        JS_EnumerateStub, JS_ResolveStub,  JS_ConvertStub,  ssFinalizeParticleCallback,
    };

    // create the object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);

    if (!object || !JS_AddRoot(cx, &object))
        return NULL;

    // assign properties to the object
    static JSPropertySpec ps[] =
    {
        { "on_update", 0, JSPROP_PERMANENT, ssParticleCallbackGetProperty, ssParticleCallbackSetProperty },
        { "on_render", 1, JSPROP_PERMANENT, ssParticleCallbackGetProperty, ssParticleCallbackSetProperty },
        { "on_birth",  2, JSPROP_PERMANENT, ssParticleCallbackGetProperty, ssParticleCallbackSetProperty },
        { "on_death",  3, JSPROP_PERMANENT, ssParticleCallbackGetProperty, ssParticleCallbackSetProperty },
        { 0, 0, 0, 0, 0 },
    };
    JS_DefineProperties(cx, object, ps);

    SS_PARTICLE_CALLBACK* callback_object = new SS_PARTICLE_CALLBACK;

    if (!callback_object)
    {
        // balancing call to JS_RemoveRoot
        JS_RemoveRoot(cx, &object);

        return NULL;
    }

    callback_object->system = system;
    JS_SetPrivate(cx, object, callback_object);

    // balancing call to JS_RemoveRoot
    JS_RemoveRoot(cx, &object);

    return object;
}

////////////////////////////////////////
begin_finalizer(SS_PARTICLE_CALLBACK, ssFinalizeParticleCallback)
// the callback object should not delete the superior particle system
object->system = NULL;
end_finalizer()

////////////////////////////////////////
begin_property(SS_PARTICLE_CALLBACK, ssParticleCallbackGetProperty)
int prop_id = argInt(cx, id);
switch (prop_id)
{
    case 0: *vp = object->system->GetScriptInterface().GetOnUpdate(); break;
    case 1: *vp = object->system->GetScriptInterface().GetOnRender(); break;
    case 2: *vp = object->system->GetScriptInterface().GetOnBirth();  break;
    case 3: *vp = object->system->GetScriptInterface().GetOnDeath();  break;
    default:*vp = JSVAL_NULL;                                         break;
}
end_property()

////////////////////////////////////////
begin_property(SS_PARTICLE_CALLBACK, ssParticleCallbackSetProperty)
int prop_id    = argInt(cx, id);
jsval callback = JSVAL_NULL;
JSObject* func = argObject(cx, *vp);

if (!JSVAL_IS_NULL(OBJECT_TO_JSVAL(func)) && JS_ObjectIsFunction(cx, func) == JS_TRUE)
    callback = OBJECT_TO_JSVAL(func);

switch (prop_id)
{
    case 0: object->system->GetScriptInterface().SetOnUpdate(callback); break;
    case 1: object->system->GetScriptInterface().SetOnRender(callback); break;
    case 2: object->system->GetScriptInterface().SetOnBirth(callback);  break;
    case 3: object->system->GetScriptInterface().SetOnDeath(callback);  break;
}
end_property()

////////////////////////////////////////
// PARTICLE DESCENDANTS OBJECT /////////
////////////////////////////////////////

JSObject*
CScript::CreateParticleDescendantsObject(JSContext* cx, ParticleSystemParent* system)
{
    if (!system)
        return NULL;

    static JSClass clasp =
    {
        "particle system parent descendants", JSCLASS_HAS_PRIVATE,
        JS_PropertyStub,  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
        JS_EnumerateStub, JS_ResolveStub,  JS_ConvertStub,  ssFinalizeParticleDescendants,
    };

    // create the object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);

    if (!object || !JS_AddRoot(cx, &object))
        return NULL;

    // assign methods to the object
    static JSFunctionSpec fs[] =
    {
        { "size",          ssParticleDescendantsSize,           0, 0, 0 },
        { "unique",        ssParticleDescendantsUnique,         0, 0, 0 },
        { "apply",         ssParticleDescendantsApply,          1, 0, 0 },
        { "sort",          ssParticleDescendantsSort,           1, 0, 0 },
        { "contains",      ssParticleDescendantsContains,       1, 0, 0 },
        { "containsGroup", ssParticleDescendantsContainsGroup,  1, 0, 0 },
        { "get",           ssParticleDescendantsGet,            1, 0, 0 },
        { "getGroup",      ssParticleDescendantsGetGroup,       1, 0, 0 },
        { "extract",       ssParticleDescendantsExtract,        1, 0, 0 },
        { "extractGroup",  ssParticleDescendantsExtractGroup,   1, 0, 0 },
        { "remove",        ssParticleDescendantsRemove,         1, 0, 0 },
        { "removeGroup",   ssParticleDescendantsRemoveGroup,    1, 0, 0 },
        { "clear",         ssParticleDescendantsClear,          0, 0, 0 },
        { 0, 0, 0, 0, 0 },
    };
    JS_DefineFunctions(cx, object, fs);

    SS_PARTICLE_DESCENDANTS* descendants_object = new SS_PARTICLE_DESCENDANTS;

    if (!descendants_object)
    {
        // balancing call to JS_RemoveRoot
        JS_RemoveRoot(cx, &object);

        return NULL;
    }

    descendants_object->system = system;
    JS_SetPrivate(cx, object, descendants_object);

    // balancing call to JS_RemoveRoot
    JS_RemoveRoot(cx, &object);

    return object;
}

////////////////////////////////////////
begin_finalizer(SS_PARTICLE_DESCENDANTS, ssFinalizeParticleDescendants)
// the swarm object should not delete the superior particle system
object->system = NULL;
end_finalizer()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_DESCENDANTS, ssParticleDescendantsSize, 0)
return_int(object->system->Size());
end_method()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_DESCENDANTS, ssParticleDescendantsUnique, 0)
object->system->Unique();
end_method()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_DESCENDANTS, ssParticleDescendantsApply, 1)
arg_function_object(func);
object->system->Apply(ScriptInterface::Applicator(cx, OBJECT_TO_JSVAL(func)));
end_method()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_DESCENDANTS, ssParticleDescendantsSort, 1)
arg_function_object(func);
object->system->Sort(ScriptInterface::Comparator(cx, This->m_Global, OBJECT_TO_JSVAL(func)));
end_method()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_DESCENDANTS, ssParticleDescendantsContains, 1)
arg_int(id);
if (id < 0)
    return_bool(JS_FALSE);
return_bool(object->system->ContainsDescendant(id));
end_method()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_DESCENDANTS, ssParticleDescendantsContainsGroup, 1)
arg_int(group);
return_bool(object->system->ContainsDescendantGroup(group));
end_method()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_DESCENDANTS, ssParticleDescendantsGet, 1)
arg_int(id);
if (id < 0)
    return_null();
ParticleSystemBase* system = object->system->GetDescendant(id);
if (system)
    return_object(system->GetScriptInterface().GetObject());
else
    return_null();
end_method()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_DESCENDANTS, ssParticleDescendantsGetGroup, 1)
arg_int(group);
JSObject* ret_array = JS_NewArrayObject(cx, 0, NULL);
if (!ret_array || !JS_AddRoot(cx, &ret_array))
{
    JS_ReportError(cx, "Could not set up array, getGroup() failed.");
    return JS_FALSE;
}
std::vector<ParticleSystemBase*> obj_array = object->system->GetDescendantGroup(group);
for (dword i = 0; i < obj_array.size(); ++i)
{
    jsval element = OBJECT_TO_JSVAL(obj_array[i]->GetScriptInterface().GetObject());
    if (!JS_SetElement(cx, ret_array, i, &element))
    {
        JS_RemoveRoot(cx, &ret_array);
        JS_ReportError(cx, "Could not set element, getGroup() failed.");
        return JS_FALSE;
    }
}
return_object(ret_array);
JS_RemoveRoot(cx, &ret_array);
end_method()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_DESCENDANTS, ssParticleDescendantsExtract, 1)
arg_int(id);
if (id < 0)
    return_null();
ParticleSystemBase* system = object->system->ExtractDescendant(id);
if (system)
    return_object(system->GetScriptInterface().GetObject());
else
    return_null();
end_method()

////////////////////////////////////////
begin_method(SS_PARTICLE_DESCENDANTS, ssParticleDescendantsExtractGroup, 1)
arg_int(group);
JSObject* ret_array = JS_NewArrayObject(cx, 0, NULL);
if (!ret_array || !JS_AddRoot(cx, &ret_array))
{
    JS_ReportError(cx, "Could not set up array, extractGroup() failed.");
    return JS_FALSE;
}
std::vector<ParticleSystemBase*> obj_array = object->system->ExtractDescendantGroup(group);
for (dword i = 0; i < obj_array.size(); ++i)
{
    jsval element = OBJECT_TO_JSVAL(obj_array[i]->GetScriptInterface().GetObject());
    if (!JS_SetElement(cx, ret_array, i, &element))
    {
        JS_RemoveRoot(cx, &ret_array);
        JS_ReportError(cx, "Could not set element, extractGroup() failed.");
        return JS_FALSE;
    }
}
return_object(ret_array);
JS_RemoveRoot(cx, &ret_array);
end_method()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_DESCENDANTS, ssParticleDescendantsRemove, 1)
arg_int(id);
if (id >= 0)
    object->system->RemoveDescendant(id);
end_method()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_DESCENDANTS, ssParticleDescendantsRemoveGroup, 1)
arg_int(group);
object->system->RemoveDescendantGroup(group);
end_method()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_DESCENDANTS, ssParticleDescendantsClear, 0)
object->system->Clear();
end_method()

////////////////////////////////////////
// PARTICLE SYSTEM PARENT OBJECT ///////
////////////////////////////////////////

JSObject*
CScript::CreateParticleSystemParentObject(JSContext* cx, ParticleSystemParent* system)
{
    // this function is responsible for freeing memory used by the system

    if (!system)
        return NULL;

    static JSClass clasp =
    {
        "particle system parent", JSCLASS_HAS_PRIVATE,
        JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
        JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, ssFinalizeParticleSystemParent,
    };

    // create the object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);

    if (!object)
    {
        delete system;
        return NULL;
    }

    // add a local root to prevent the JS_GC from premature gc-ing
    jsval local_root = OBJECT_TO_JSVAL(object);

    if (JS_AddRoot(cx, &local_root) == JS_FALSE)
    {
        delete system;
        return NULL;
    }

    // assign the particle body object
    JS_DefineProperty(
        cx,
        object,
        "body",
        OBJECT_TO_JSVAL(CreateParticleBodyObject(cx, system)),
        JS_PropertyStub,
        JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    // assign the particle initializer object
    JS_DefineProperty(
        cx,
        object,
        "initializer",
        OBJECT_TO_JSVAL(CreateParticleInitializerObject(cx, system)),
        JS_PropertyStub,
        JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    // assign the particle updater object
    JS_DefineProperty(
        cx,
        object,
        "updater",
        OBJECT_TO_JSVAL(CreateParticleUpdaterObject(cx, system)),
        JS_PropertyStub,
        JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    // assign the particle renderer object
    JS_DefineProperty(
        cx,
        object,
        "renderer",
        OBJECT_TO_JSVAL(CreateParticleRendererObject(cx, system)),
        JS_PropertyStub,
        JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    // assign the callback object
    JS_DefineProperty(
        cx,
        object,
        "callback",
        OBJECT_TO_JSVAL(CreateParticleCallbackObject(cx, system)),
        JS_PropertyStub,
        JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    // assign the descendants object
    JS_DefineProperty(
        cx,
        object,
        "descendants",
        OBJECT_TO_JSVAL(CreateParticleDescendantsObject(cx, system)),
        JS_PropertyStub,
        JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    // assign the type property
    JS_DefineProperty(
        cx,
        object,
        "type",
        INT_TO_JSVAL(system->GetType()),
        JS_PropertyStub,
        JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    // assign the id property
    JS_DefineProperty(
        cx,
        object,
        "id",
        INT_TO_JSVAL(system->GetID()),
        JS_PropertyStub,
        JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    // assign properties to the object
    static JSPropertySpec ps[] =
    {
        { "group",  0, JSPROP_PERMANENT, ssParticleSystemParentGetProperty, ssParticleSystemParentSetProperty },
        { "halted", 1, JSPROP_PERMANENT, ssParticleSystemParentGetProperty, ssParticleSystemParentSetProperty },
        { "hidden", 2, JSPROP_PERMANENT, ssParticleSystemParentGetProperty, ssParticleSystemParentSetProperty },
        { "cursed", 3, JSPROP_PERMANENT, ssParticleSystemParentGetProperty, ssParticleSystemParentSetProperty },
        { 0, 0, 0, 0, 0 },
    };
    JS_DefineProperties(cx, object, ps);

    // assign methods to the object
    static JSFunctionSpec fs[] =
    {
        { "update",        ssParticleSystemParentUpdate,         0, 0, 0 },
        { "render",        ssParticleSystemParentRender,         0, 0, 0 },
        { "adopt",         ssParticleSystemParentAdopt,          1, 0, 0 },
        { "host",          ssParticleSystemParentHost,           1, 0, 0 },
        { "isDead",        ssParticleSystemParentIsDead,         0, 0, 0 },
        { "kill",          ssParticleSystemParentKill,           0, 0, 0 },
        { "revive",        ssParticleSystemParentRevive,         0, 0, 0 },
        { 0, 0, 0, 0, 0 },
    };
    JS_DefineFunctions(cx, object, fs);

    SS_PARTICLE_SYSTEM_PARENT* system_object = new SS_PARTICLE_SYSTEM_PARENT;

    if (!system_object)
    {
        JS_RemoveRoot(cx, &local_root);
        delete system;
        return NULL;
    }

    system_object->system = system;
    JS_SetPrivate(cx, object, system_object);

    // if initialization of the script interface fails, we can't use it and need to quit
    if (!system->GetScriptInterface().Init(cx, object))
    {
        JS_RemoveRoot(cx, &local_root);
        delete system;
        return NULL;
    }

    // balancing call to JS_RemoveRoot
    JS_RemoveRoot(cx, &local_root);

    return object;
}

////////////////////////////////////////
begin_finalizer(SS_PARTICLE_SYSTEM_PARENT, ssFinalizeParticleSystemParent)
// this finalizer is responsible for freeing up memory used by the system
delete object->system;
object->system = NULL;
end_finalizer()

////////////////////////////////////////
/**
    -
*/
begin_property(SS_PARTICLE_SYSTEM_PARENT, ssParticleSystemParentGetProperty)
int prop_id = argInt(cx, id);
switch (prop_id)
{
    case 0: *vp =     INT_TO_JSVAL(object->system->GetGroup()); break;
    case 1: *vp = BOOLEAN_TO_JSVAL(object->system->IsHalted()); break;
    case 2: *vp = BOOLEAN_TO_JSVAL(object->system->IsHidden()); break;
    case 3: *vp = BOOLEAN_TO_JSVAL(object->system->IsCursed()); break;
    default:*vp = JSVAL_NULL;                                   break;
}
end_property()

////////////////////////////////////////
/**
    -
*/
begin_property(SS_PARTICLE_SYSTEM_PARENT, ssParticleSystemParentSetProperty)
int prop_id = argInt(cx, id);
switch (prop_id)
{
    case 0: object->system->SetGroup(argInt(cx, *vp)); break;
    case 1: object->system->Halt(argBool(cx, *vp));    break;
    case 2: object->system->Hide(argBool(cx, *vp));    break;
    case 3: object->system->Curse(argBool(cx, *vp));   break;
}
end_property()

////////////////////////////////////////
/**
    - ask particle system to update
*/
begin_method(SS_PARTICLE_SYSTEM_PARENT, ssParticleSystemParentUpdate, 0)
object->system->Update();
end_method()

////////////////////////////////////////
/**
    - ask particle system to render
*/
begin_method(SS_PARTICLE_SYSTEM_PARENT, ssParticleSystemParentRender, 0)
object->system->Render();
end_method()

////////////////////////////////////////
/**
    - adopt the particle_system
*/
begin_method(SS_PARTICLE_SYSTEM_PARENT, ssParticleSystemParentAdopt, 1)
arg_particle_system(system);
object->system->Adopt(system);
end_method()

////////////////////////////////////////
/**
    - host the particle_system, hosted systems' bodies are neither initialized nor updated
*/
begin_method(SS_PARTICLE_SYSTEM_PARENT, ssParticleSystemParentHost, 1)
arg_particle_system(system);
object->system->Host(system);
end_method()

////////////////////////////////////////
/**
    - return true, if the system is dead
*/
begin_method(SS_PARTICLE_SYSTEM_PARENT, ssParticleSystemParentIsDead, 0)
return_bool(object->system->IsDead());
end_method()

////////////////////////////////////////
/**
    - kills the system, this will trigger the on_death callback, if set
*/
begin_method(SS_PARTICLE_SYSTEM_PARENT, ssParticleSystemParentKill, 0)
object->system->Kill();
end_method()

////////////////////////////////////////
/**
    - revives the system, this will trigger the on_birth callback, if set
*/
begin_method(SS_PARTICLE_SYSTEM_PARENT, ssParticleSystemParentRevive, 0)
object->system->Revive();
end_method()

////////////////////////////////////////
// PARTICLE SWARM RENDERER OBJECT //////
////////////////////////////////////////

JSObject*
CScript::CreateParticleSwarmRendererObject(JSContext* cx, ParticleSystemChild* system)
{
    if (!system)
        return NULL;

    static JSClass clasp =
    {
        "particle swarm renderer", JSCLASS_HAS_PRIVATE,
        JS_PropertyStub,  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
        JS_EnumerateStub, JS_ResolveStub,  JS_ConvertStub,  ssFinalizeParticleSwarmRenderer,
    };

    // create the object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);

    if (!object || !JS_AddRoot(cx, &object))
        return NULL;

    // assign properties to the object
    static JSPropertySpec ps[] =
    {
        { "texture",    0, JSPROP_PERMANENT, ssParticleSwarmRendererGetProperty, ssParticleSwarmRendererSetProperty },
        { "blend_mode", 1, JSPROP_PERMANENT, ssParticleSwarmRendererGetProperty, ssParticleSwarmRendererSetProperty },
        { "offset_x",   2, JSPROP_PERMANENT, ssParticleSwarmRendererGetProperty, ssParticleSwarmRendererSetProperty },
        { "offset_y",   3, JSPROP_PERMANENT, ssParticleSwarmRendererGetProperty, ssParticleSwarmRendererSetProperty },
        { "disabled",   4, JSPROP_PERMANENT, ssParticleSwarmRendererGetProperty, ssParticleSwarmRendererSetProperty },
        { 0, 0, 0, 0, 0 },
    };
    JS_DefineProperties(cx, object, ps);

    SS_PARTICLE_SWARM_RENDERER* renderer_object = new SS_PARTICLE_SWARM_RENDERER;

    if (!renderer_object)
    {
        // balancing call to JS_RemoveRoot
        JS_RemoveRoot(cx, &object);

        return NULL;
    }

    renderer_object->system = system;
    JS_SetPrivate(cx, object, renderer_object);

    // balancing call to JS_RemoveRoot
    JS_RemoveRoot(cx, &object);

    return object;
}

////////////////////////////////////////
begin_finalizer(SS_PARTICLE_SWARM_RENDERER, ssFinalizeParticleSwarmRenderer)
// the swarm renderer object should not delete the superior particle system
object->system = NULL;
end_finalizer()

////////////////////////////////////////
/**
    -
*/
begin_property(SS_PARTICLE_SWARM_RENDERER, ssParticleSwarmRendererGetProperty)
int prop_id = argInt(cx, id);
switch (prop_id)
{
case 0:
    *vp = OBJECT_TO_JSVAL(object->system->GetScriptInterface().GetTextureObject());
    break;
case 1:
    *vp = INT_TO_JSVAL(object->system->GetSwarmRenderer().GetBlendMode());
    break;
case 2:
    *vp = INT_TO_JSVAL(object->system->GetSwarmRenderer().GetOffsetX());
    break;
case 3:
    *vp = INT_TO_JSVAL(object->system->GetSwarmRenderer().GetOffsetY());
    break;
case 4:
    *vp = INT_TO_JSVAL(object->system->GetSwarmRenderer().IsDisabled());
    break;
default:
    *vp = JSVAL_NULL;
    break;
}
end_property()

////////////////////////////////////////
/**
    -
*/
begin_property(SS_PARTICLE_SWARM_RENDERER, ssParticleSwarmRendererSetProperty)
int prop_id = argInt(cx, id);
switch (prop_id)
{
case 0:
    {
        SS_IMAGE* image = argImage(cx, *vp);

        if (image)
        {
            object->system->GetScriptInterface().SetTextureObject(JSVAL_TO_OBJECT(*vp));
            object->system->GetSwarmRenderer().SetTexture(image->image);
        }
        else
        {
            object->system->GetScriptInterface().SetTextureObject(NULL);
            object->system->GetSwarmRenderer().SetTexture(NULL);
        }
    }
    break;
case 1:
    object->system->GetSwarmRenderer().SetBlendMode(argInt(cx, *vp));
    break;
case 2:
    object->system->GetSwarmRenderer().SetOffsetX(argInt(cx, *vp));
    break;
case 3:
    object->system->GetSwarmRenderer().SetOffsetY(argInt(cx, *vp));
    break;
case 4:
    object->system->GetSwarmRenderer().Disable(argBool(cx, *vp));
    break;
}
end_property()

////////////////////////////////////////
// PARTICLE SWARM OBJECT ///////////////
////////////////////////////////////////

JSObject*
CScript::CreateParticleSwarmObject(JSContext* cx, ParticleSystemChild* system)
{
    if (!system)
        return NULL;

    static JSClass clasp =
    {
        "particle system child swarm", JSCLASS_HAS_PRIVATE,
        JS_PropertyStub,  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
        JS_EnumerateStub, JS_ResolveStub,  JS_ConvertStub,  ssFinalizeParticleSwarm,
    };

    // create the object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);

    if (!object || !JS_AddRoot(cx, &object))
        return NULL;

    // assign the particle swarm renderer object
    JS_DefineProperty(
        cx,
        object,
        "renderer",
        OBJECT_TO_JSVAL(CreateParticleSwarmRendererObject(cx, system)),
        JS_PropertyStub,
        JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    // assign methods to the object
    static JSFunctionSpec fs[] =
    {
        { "size",          ssParticleSwarmSize,           0, 0, 0 },
        { "capacity",      ssParticleSwarmCapacity,       0, 0, 0 },
        { "grow",          ssParticleSwarmGrow,           0, 0, 0 },
        { "shrink",        ssParticleSwarmShrink,         0, 0, 0 },
        { "resize",        ssParticleSwarmResize,         1, 0, 0 },
        { "reserve",       ssParticleSwarmReserve,        1, 0, 0 },
        { "clear",         ssParticleSwarmClear,          0, 0, 0 },
        { 0, 0, 0, 0, 0 },
    };
    JS_DefineFunctions(cx, object, fs);

    SS_PARTICLE_SWARM* swarm_object = new SS_PARTICLE_SWARM;

    if (!swarm_object)
    {
        // balancing call to JS_RemoveRoot
        JS_RemoveRoot(cx, &object);

        return NULL;
    }

    swarm_object->system = system;
    JS_SetPrivate(cx, object, swarm_object);

    // balancing call to JS_RemoveRoot
    JS_RemoveRoot(cx, &object);

    return object;
}

////////////////////////////////////////
/**
    -
*/
begin_finalizer(SS_PARTICLE_SWARM, ssFinalizeParticleSwarm)
// the swarm object should not delete the superior particle system
object->system = NULL;
end_finalizer()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_SWARM, ssParticleSwarmSize, 0)
return_int(object->system->Size());
end_method()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_SWARM, ssParticleSwarmCapacity, 0)
return_int(object->system->Capacity());
end_method()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_SWARM, ssParticleSwarmGrow, 0)
object->system->Grow();
end_method()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_SWARM, ssParticleSwarmShrink, 0)
object->system->Shrink();
end_method()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_SWARM, ssParticleSwarmResize, 1)
arg_int(new_size);
if (new_size < 0) new_size = 0;
object->system->Resize(new_size);
end_method()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_SWARM, ssParticleSwarmReserve, 1)
arg_int(new_capacity);
if (new_capacity > 0)
    object->system->Reserve(new_capacity);
end_method()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_SWARM, ssParticleSwarmClear, 0)
object->system->Clear();
end_method()

////////////////////////////////////////
// PARTICLE SYSTEM CHILD OBJECT ////////
////////////////////////////////////////

JSObject*
CScript::CreateParticleSystemChildObject(JSContext* cx, ParticleSystemChild* system)
{
    // this function is responsible for freeing memory used by the system

    if (!system)
        return NULL;

    static JSClass clasp =
    {
        "particle system child", JSCLASS_HAS_PRIVATE,
        JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
        JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, ssFinalizeParticleSystemChild,
    };

    // create the object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);

    if (!object)
    {
        delete system;
        return NULL;
    }

    // add a local root to prevent the JS_GC from premature gc-ing
    jsval local_root = OBJECT_TO_JSVAL(object);

    if (JS_AddRoot(cx, &local_root) == JS_FALSE)
    {
        delete system;
        return NULL;
    }

    // assign the particle body object
    JS_DefineProperty(
        cx,
        object,
        "body",
        OBJECT_TO_JSVAL(CreateParticleBodyObject(cx, system)),
        JS_PropertyStub,
        JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    // assign the particle initializer object
    JS_DefineProperty(
        cx,
        object,
        "initializer",
        OBJECT_TO_JSVAL(CreateParticleInitializerObject(cx, system)),
        JS_PropertyStub,
        JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    // assign the particle updater object
    JS_DefineProperty(
        cx,
        object,
        "updater",
        OBJECT_TO_JSVAL(CreateParticleUpdaterObject(cx, system)),
        JS_PropertyStub,
        JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    // assign the particle renderer object
    JS_DefineProperty(
        cx,
        object,
        "renderer",
        OBJECT_TO_JSVAL(CreateParticleRendererObject(cx, system)),
        JS_PropertyStub,
        JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    // assign the callback object
    JS_DefineProperty(
        cx,
        object,
        "callback",
        OBJECT_TO_JSVAL(CreateParticleCallbackObject(cx, system)),
        JS_PropertyStub,
        JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    // assign the swarm object
    JS_DefineProperty(
        cx,
        object,
        "swarm",
        OBJECT_TO_JSVAL(CreateParticleSwarmObject(cx, system)),
        JS_PropertyStub,
        JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    // assign the type property
    JS_DefineProperty(
        cx,
        object,
        "type",
        INT_TO_JSVAL(system->GetType()),
        JS_PropertyStub,
        JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    // assign the id property
    JS_DefineProperty(
        cx,
        object,
        "id",
        INT_TO_JSVAL(system->GetID()),
        JS_PropertyStub,
        JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    // assign properties to the object
    static JSPropertySpec ps[] =
    {
        { "group",  0, JSPROP_PERMANENT, ssParticleSystemChildGetProperty, ssParticleSystemChildSetProperty },
        { "halted", 1, JSPROP_PERMANENT, ssParticleSystemChildGetProperty, ssParticleSystemChildSetProperty },
        { "hidden", 2, JSPROP_PERMANENT, ssParticleSystemChildGetProperty, ssParticleSystemChildSetProperty },
        { "cursed", 3, JSPROP_PERMANENT, ssParticleSystemChildGetProperty, ssParticleSystemChildSetProperty },
        { 0, 0, 0, 0, 0 },
    };
    JS_DefineProperties(cx, object, ps);

    // assign methods to the object
    static JSFunctionSpec fs[] =
    {
        { "update",        ssParticleSystemChildUpdate,         0, 0, 0 },
        { "render",        ssParticleSystemChildRender,         0, 0, 0 },
        { "clone",         ssParticleSystemChildClone,          0, 0, 0 },
        { "isDead",        ssParticleSystemChildIsDead,         0, 0, 0 },
        { "kill",          ssParticleSystemChildKill,           0, 0, 0 },
        { "revive",        ssParticleSystemChildRevive,         0, 0, 0 },
        { 0, 0, 0, 0, 0 },
    };
    JS_DefineFunctions(cx, object, fs);

    SS_PARTICLE_SYSTEM_CHILD* system_object = new SS_PARTICLE_SYSTEM_CHILD;

    if (!system_object)
    {
        // balancing call to JS_RemoveRoot
        JS_RemoveRoot(cx, &local_root);

        delete system;
        return NULL;
    }

    system_object->system = system;
    JS_SetPrivate(cx, object, system_object);

    // if initialization of the script interface fails, we can't use it and need to quit
    if (!system->GetScriptInterface().Init(cx, object))
    {
        JS_RemoveRoot(cx, &local_root);
        delete system;
        return NULL;
    }

    // balancing call to JS_RemoveRoot
    JS_RemoveRoot(cx, &local_root);

    return object;
}

////////////////////////////////////////
begin_finalizer(SS_PARTICLE_SYSTEM_CHILD, ssFinalizeParticleSystemChild)
// this finalizer is responsible for freeing up memory used by the system
delete object->system;
object->system = NULL;
end_finalizer()

////////////////////////////////////////
/**
    -
*/
begin_property(SS_PARTICLE_SYSTEM_CHILD, ssParticleSystemChildGetProperty)
int prop_id = argInt(cx, id);
switch (prop_id)
{
    case 0: *vp =     INT_TO_JSVAL(object->system->GetGroup()); break;
    case 1: *vp = BOOLEAN_TO_JSVAL(object->system->IsHalted()); break;
    case 2: *vp = BOOLEAN_TO_JSVAL(object->system->IsHidden()); break;
    case 3: *vp = BOOLEAN_TO_JSVAL(object->system->IsCursed()); break;
    default:*vp = JSVAL_NULL;                                   break;
}
end_property()

////////////////////////////////////////
/**
    -
*/
begin_property(SS_PARTICLE_SYSTEM_CHILD, ssParticleSystemChildSetProperty)
int prop_id = argInt(cx, id);
switch (prop_id)
{
    case 0: object->system->SetGroup(argInt(cx, *vp)); break;
    case 1: object->system->Halt(argBool(cx, *vp));    break;
    case 2: object->system->Hide(argBool(cx, *vp));    break;
    case 3: object->system->Curse(argBool(cx, *vp));   break;
}
end_property()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_SYSTEM_CHILD, ssParticleSystemChildUpdate, 0)
object->system->Update();
end_method()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_SYSTEM_CHILD, ssParticleSystemChildRender, 0)
object->system->Render();
end_method()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_SYSTEM_CHILD, ssParticleSystemChildClone, 0)
ParticleSystemChild* clone = new ParticleSystemChild(*(object->system));
return_object(CreateParticleSystemChildObject(cx, clone));
end_method()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_SYSTEM_CHILD, ssParticleSystemChildIsDead, 0)
return_bool(object->system->IsDead());
end_method()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_SYSTEM_CHILD, ssParticleSystemChildKill, 0)
object->system->Kill();
end_method()

////////////////////////////////////////
/**
    -
*/
begin_method(SS_PARTICLE_SYSTEM_CHILD, ssParticleSystemChildRevive, 0)
object->system->Revive();
end_method()

////////////////////////////////////////
// END PARTICLE ENGINE OBJECTS /////////
////////////////////////////////////////

////////////////////////////////////////
// SOCKET OBJECTS //////////////////////
////////////////////////////////////////

JSObject*
CScript::CreateSocketObject(JSContext* cx, NSOCKET socket)
{
    static JSClass clasp =
        {
            "socket", JSCLASS_HAS_PRIVATE,
            JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
            JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, ssFinalizeSocket,
        };

    // create the object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);
    if (!object || !JS_AddRoot(cx, &object))
        return NULL;

    // assign methods to the object
    static JSFunctionSpec fs[] =
        {
            { "isConnected",        ssSocketIsConnected,        0, 0, 0 },
            { "getPendingReadSize", ssSocketGetPendingReadSize, 0, 0, 0 },
            { "read",               ssSocketRead,               1, 0, 0 },
            { "write",              ssSocketWrite,              1, 0, 0 },
            { "close",              ssSocketClose,              0, 0, 0 },
            { 0, 0, 0, 0, 0 },
        };
    JS_DefineFunctions(cx, object, fs);

    // attach the socket to this object
    SS_SOCKET* socket_object = new SS_SOCKET;

    if (!socket_object)
    {
        JS_RemoveRoot(cx, &object);
        return NULL;
    }
    socket_object->socket = socket;
    socket_object->is_open = true;
    JS_SetPrivate(cx, object, socket_object);
    JS_RemoveRoot(cx, &object);
    return object;
}

////////////////////////////////////////
begin_finalizer(SS_SOCKET, ssFinalizeSocket)
if (object->socket && object->is_open)
{
    CloseSocket(object->socket);

}
object->socket = NULL;

end_finalizer()
////////////////////////////////////////
/**
    - returns true if the socket is connected
*/
begin_method(SS_SOCKET, ssSocketIsConnected, 0)
if (object->socket && object->is_open)
{
    return_bool(IsConnected(object->socket));
}
else
{
    return_bool(false);
}
end_method()

////////////////////////////////////////
/**
    - returns the size of the next array to be read in the socket
*/
begin_method(SS_SOCKET, ssSocketGetPendingReadSize, 0)
if (!object->is_open)
{
    JS_ReportError(cx, "socket is closed!");
    return JS_FALSE;
}

if (object->socket)
{
    return_int(GetPendingReadSize(object->socket));
}
else
{
    return_int(-1);
}
end_method()

////////////////////////////////////////
/**
    - reads from the socket, returns a ByteArray object

      or returns null if it fails
      e.g.
      var data = socket.read(size);
      if (data != null) {
        // do stuff with data
      }
*/
begin_method(SS_SOCKET, ssSocketRead, 1)
if (!object->is_open)
{
    JS_ReportError(cx, "socket is closed!");
    return JS_FALSE;
}

if (object->socket)
{
    arg_int(size);

    if (size <= 0)
    {
        return_object(CreateByteArrayObject(cx, 0));
    }
    else
    {
        void* buffer = malloc(size);
        int read = SocketRead(object->socket, buffer, size);
        if (read < 0)
        { // error!

            free(buffer);
            //JS_ReportError(cx, "socket.read() failed miserably!");
            //return JS_FALSE;

            return_object(JSVAL_NULL);
        }

        else
        {

            JSObject* array_object = CreateByteArrayObject(cx, read, buffer);
            free(buffer);

            return_object(array_object);
        }
    }
}
end_method()

////////////////////////////////////////
/**
    - writes a ByteArray object into the socket
*/
begin_method(SS_SOCKET, ssSocketWrite, 1)
if (!object->is_open)
{
    JS_ReportError(cx, "socket is closed!");
    return JS_FALSE;
}

if (object->socket)
{
    arg_byte_array(array);
    SocketWrite(object->socket, array->array, array->size);
}
end_method()

////////////////////////////////////////
/**
    - closes the socket object, after this, the socket cannot be used.
*/
begin_method(SS_SOCKET, ssSocketClose, 0)
if (object->socket && object->is_open)
{
    CloseSocket(object->socket);
}
object->is_open = false;
end_method()

////////////////////////////////////////
////////////////////////////////////////
// LOG OBJECTS /////////////////////////
////////////////////////////////////////

JSObject*
CScript::CreateLogObject(JSContext* cx, CLog* log)
{
    // define log class
    static JSClass clasp =
        {
            "log", JSCLASS_HAS_PRIVATE,
            JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
            JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, ssFinalizeLog,
        };

    // create the object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);
    if (!object || !JS_AddRoot(cx, &object))
        return NULL;

    // assign methods to the object
    static JSFunctionSpec fs[] =
        {
            { "write",      ssLogWrite,      2, 0, 0 },
            { "beginBlock", ssLogBeginBlock, 1, 0, 0 },
            { "endBlock",   ssLogEndBlock,   0, 0, 0 },
            { 0, 0, 0, 0, 0 },
        };
    JS_DefineFunctions(cx, object, fs);

    // attach the log to this object
    SS_LOG* log_object = new SS_LOG;

    if (!log_object)
    {
        JS_RemoveRoot(cx, &object);
        return NULL;
    }
    log_object->log = log;
    JS_SetPrivate(cx, object, log_object);
    JS_RemoveRoot(cx, &object);
    return object;
}

////////////////////////////////////////
begin_finalizer(SS_LOG, ssFinalizeLog)
This->m_Engine->CloseLog(object->log);

object->log = NULL;
end_finalizer()

////////////////////////////////////////
/**
    - writes a string of text under the current block.
      ex: myLog.write("Starting system...");
*/
begin_method(SS_LOG, ssLogWrite, 1)
arg_str(text);
object->log->Send(text);
end_method()

////////////////////////////////////////
/**
    - creates a "block" which is indent inside the log with the name as the
      title of the block. Any subsequent write commands will go under the newly
      created block.
      ex: myLog.beginBlock("Video Information");
*/
begin_method(SS_LOG, ssLogBeginBlock, 1)
arg_str(name);
object->log->BeginBlock(name);
end_method();

////////////////////////////////////////
/**
    - closes the current log block.
*/
begin_method(SS_LOG, ssLogEndBlock, 0)
object->log->EndBlock();
end_method()

////////////////////////////////////////
////////////////////////////////////////
// COLOR OBJECTS ///////////////////////
////////////////////////////////////////

JSObject*
CScript::CreateColorObject(JSContext* cx, RGBA color)
{
    // define color class
    static JSClass clasp =
        {
            "color", JSCLASS_HAS_PRIVATE,
            JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
            JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, ssFinalizeColor,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        };

    // create the object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);
    if (!object || !JS_AddRoot(cx, &object))
        return NULL;

    // assign properties
    static JSPropertySpec ps[] =
        {
            { "red",   0, JSPROP_PERMANENT, ssColorGetProperty, ssColorSetProperty },
            { "green", 1, JSPROP_PERMANENT, ssColorGetProperty, ssColorSetProperty },
            { "blue",  2, JSPROP_PERMANENT, ssColorGetProperty, ssColorSetProperty },
            { "alpha", 3, JSPROP_PERMANENT, ssColorGetProperty, ssColorSetProperty },
            { 0, 0, 0, 0, 0 },
        };
    JS_DefineProperties(cx, object, ps);

    // assign methods to the object
    static JSFunctionSpec fs[] =
        {
            { "toJSON",        ssColorToJSON,    0, },
            { 0, 0, 0, 0, 0 },
        };
    JS_DefineFunctions(cx, object, fs);

    // attach the color to this object
    SS_COLOR* color_object = new SS_COLOR;

    if (!color_object)
    {
        JS_RemoveRoot(cx, &object);
        return NULL;
    }
    color_object->color = color;
    JS_SetPrivate(cx, object, color_object);
    JS_RemoveRoot(cx, &object);
    return object;
}

////////////////////////////////////////
begin_finalizer(SS_COLOR, ssFinalizeColor)
end_finalizer()
////////////////////////////////////////
begin_property(SS_COLOR, ssColorGetProperty)
int prop_id = argInt(cx, id);
switch (prop_id)
{
case 0:
    *vp = INT_TO_JSVAL(object->color.red);
    break;
case 1:
    *vp = INT_TO_JSVAL(object->color.green);
    break;
case 2:
    *vp = INT_TO_JSVAL(object->color.blue);
    break;
case 3:
    *vp = INT_TO_JSVAL(object->color.alpha);
    break;
default:
    *vp = JSVAL_NULL;
    break;
}
end_property()

////////////////////////////////////////
/**
    - Colors must be between 0 and 255.
    And are automatically clamped.
*/
begin_property(SS_COLOR, ssColorSetProperty)
int prop_id = argInt(cx, id);
switch (prop_id)
{
case 0:
    object->color.red   = argInt(cx, *vp);
    break;
case 1:
    object->color.green = argInt(cx, *vp);
    break;
case 2:
    object->color.blue  = argInt(cx, *vp);
    break;
case 3:
    object->color.alpha = argInt(cx, *vp);
    break;
}
end_property()

/**
    - exports the Color to a string, used internally by JSON
      This string can be eval()'ed to recreate the color
*/
begin_method(SS_COLOR, ssColorToJSON, 0)
char json2[64];
sprintf(json2,
    // If the alpha is 255, then dont print it
    object->color.alpha == 255 ? "CreateColor(%d,%d,%d)" : "CreateColor(%d,%d,%d,%d)",
    object->color.red,
    object->color.green,
    object->color.blue,
    object->color.alpha
);

return_str(json2);
// This works too (), especially if sprintf cannot handle your type conversion. It is also handles its own buffer
//std::string json = "CreateColor(";
//json.append( to_string((int)object->color.red) ); json.append( "," );
//json.append( to_string((int)object->color.green) ); json.append( "," );
//json.append( to_string((int)object->color.blue) ); json.append( "," );
//json.append( to_string((int)object->color.alpha) ); json.append( ")" );
//return_str( json.c_str() );
end_method()

////////////////////////////////////////
////////////////////////////////////////
// SPRITESET OBJECTS ///////////////////
////////////////////////////////////////

JSObject*
CScript::CreateSpritesetBaseObject(JSContext* cx, SSPRITESET* spriteset, bool real)
{
    static JSClass base_clasp =
        {
            "base", 0,
            JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
            JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
        };

    JSObject* base_object = JS_NewObject(cx, &base_clasp, NULL, NULL);
    if (!base_object || !JS_AddRoot(cx, &base_object))
        return NULL;

    int x1, y1, x2, y2;
    if(real)
        spriteset->GetSpriteset().GetRealBase(x1, y1, x2, y2);
    else
        spriteset->GetSpriteset().GetBase(x1, y1, x2, y2);

    JS_DefineProperty(cx, base_object, "x1", INT_TO_JSVAL(x1), JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    JS_DefineProperty(cx, base_object, "y1", INT_TO_JSVAL(y1), JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    JS_DefineProperty(cx, base_object, "x2", INT_TO_JSVAL(x2), JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    JS_DefineProperty(cx, base_object, "y2", INT_TO_JSVAL(y2), JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE | JSPROP_PERMANENT);
    JS_RemoveRoot(cx, &base_object);
    return base_object;
}

JSObject*
CScript::CreateSpritesetObject(JSContext* cx, SSPRITESET* spriteset)
{
    // define class
    static JSClass clasp =
        {
            "spriteset", JSCLASS_HAS_PRIVATE,
            JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
            JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, ssFinalizeSpriteset,
        };

    static JSClass direction_clasp =
        {
            "direction", 0,
            JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
            JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
        };

    static JSClass frame_clasp =
        {
            "frame", 0,
            JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
            JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
        };

    JSObject* local_roots = JS_NewArrayObject(cx, 0, 0);
    if (!local_roots || !JS_AddRoot(cx, &local_roots))
        return NULL;
    
    // CREATE SPRITESET OBJECT
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);
    if (!object)
        return NULL;
    jsval object_val = OBJECT_TO_JSVAL(object);
    JS_SetElement(cx, local_roots, 0, &object_val);

    // assign methods to the object
    static JSFunctionSpec fs[] =
        {
            { "save",        ssSpritesetSave,    1, },
            { "clone",       ssSpritesetClone,   0, },
            { 0, 0, 0, 0, 0 },
        };
    JS_DefineFunctions(cx, object, fs);

    // DEFINE MEMBER IMAGES ARRAY
    int num_images = spriteset->GetSpriteset().GetNumImages();
    JSObject* image_array = JS_NewArrayObject(cx, 0, 0);

    if (!image_array)
        return NULL;
    jsval image_val = OBJECT_TO_JSVAL(image_array);
    JS_SetElement(cx, local_roots, 1, &image_val);

    int i;

    for (i = 0; i < num_images; i++)
    {
        JSObject* image = CreateImageObject(cx, spriteset->GetImage(i), false);

        if (!image)
        {

            return NULL;
        }
        jsval val = OBJECT_TO_JSVAL(image);
        JS_SetElement(cx, image_array, i, &val);

        // define a reference back to this spriteset
        // we need this, because the spriteset owns the images, and we don't want them
        // to be GC'd while there is an active reference to this image
        JS_DefineProperty(
            cx,
            image,
            "spriteset",
            OBJECT_TO_JSVAL(object),
            JS_PropertyStub,
            JS_PropertyStub,
            JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);
    }

    // DEFINE MEMBER DIRECTIONS ARRAY
    int num_directions = spriteset->GetSpriteset().GetNumDirections();
    JSObject* direction_array = JS_NewArrayObject(cx, 0, 0);

    if (!direction_array)
        return NULL;
    jsval direction_val = OBJECT_TO_JSVAL(direction_array);
    JS_SetElement(cx, local_roots, 2, &direction_val);

    for (i = 0; i < num_directions; i++)
    {
        JSObject* direction = JS_NewObject(cx, &direction_clasp, NULL, NULL);

        if (!direction)
        {

            return NULL;
        }
        jsval val = OBJECT_TO_JSVAL(direction);
        JS_SetElement(cx, direction_array, i, &val);

        // set name property
        JS_DefineProperty(
            cx,
            direction,
            "name",
            STRING_TO_JSVAL(JS_NewStringCopyZ(cx, spriteset->GetSpriteset().GetDirectionName(i))),
            JS_PropertyStub,
            JS_PropertyStub,
            JSPROP_ENUMERATE | JSPROP_PERMANENT);

        // create the frames array
        int num_frames = spriteset->GetSpriteset().GetNumFrames(i);

        JSObject* frame_array = JS_NewArrayObject(cx, 0, 0);
        if (!frame_array)
        {

            return NULL;
        }
        JS_DefineProperty(
            cx,
            direction,
            "frames",
            OBJECT_TO_JSVAL(frame_array),
            JS_PropertyStub,
            JS_PropertyStub,
            JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

        for (int j = 0; j < num_frames; j++)
        {
            JSObject* frame_object = JS_NewObject(cx, &frame_clasp, NULL, NULL);

            if (!frame_object)
                return NULL;
            jsval frame_object_val = OBJECT_TO_JSVAL(frame_object);
            JS_SetElement(cx, frame_array, j, &frame_object_val);

            JS_DefineProperty(cx, frame_object, "index", INT_TO_JSVAL(spriteset->GetSpriteset().GetFrameIndex(i, j)), JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE | JSPROP_PERMANENT);
            JS_DefineProperty(cx, frame_object, "delay", INT_TO_JSVAL(spriteset->GetSpriteset().GetFrameDelay(i, j)), JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE | JSPROP_PERMANENT);
        }
    }

    // define the base object
    JSObject* base_object = CreateSpritesetBaseObject(cx, spriteset, true);
    if (!base_object)
        return NULL;
    jsval base_val = OBJECT_TO_JSVAL(base_object);
    JS_SetElement(cx, local_roots, 3, &base_val);

    // define the properties for this object
    JS_DefineProperty(
        cx,
        object,
        "images",
        OBJECT_TO_JSVAL(image_array),
        JS_PropertyStub,
        JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    JS_DefineProperty(
        cx,
        object,
        "directions",
        OBJECT_TO_JSVAL(direction_array),
        JS_PropertyStub,
        JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    JS_DefineProperty(
        cx,
        object,
        "base",
        OBJECT_TO_JSVAL(base_object),
        JS_PropertyStub,
        JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    JS_DefineProperty(
        cx,
        object,
        "filename",
        STRING_TO_JSVAL(JS_NewStringCopyZ(cx, spriteset->GetFilename().c_str())),
        JS_PropertyStub,
        JS_PropertyStub,
        JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    // attach the spriteset to this object
    SS_SPRITESET* spriteset_object = new SS_SPRITESET;
    if (!spriteset_object)
        return NULL;
    spriteset_object->spriteset = spriteset;
    spriteset_object->object    = object;
    JS_SetPrivate(cx, object, spriteset_object);

    JS_RemoveRoot(cx, &local_roots);
    return object;
}

////////////////////////////////////////
begin_finalizer(SS_SPRITESET, ssFinalizeSpriteset)
// destroy the spriteset
This->m_Engine->DestroySpriteset(object->spriteset);
end_finalizer()

////////////////////////////////////////
/**
    - saves the spriteset object to 'filename'
*/
begin_method(SS_SPRITESET, ssSpritesetSave, 1)
arg_str(filename);

if (IsValidPath(filename) == false)
{
    JS_ReportError(cx, "Invalid path: '%s'", filename);
    return JS_FALSE;
}

std::string path = "spritesets/";
path += filename;

sSpriteset* s = argSpriteset(cx, OBJECT_TO_JSVAL(obj));
if (s == NULL)
{
    return JS_FALSE;
}
*rval = PRIVATE_TO_JSVAL(s); // ROOT

bool saved = s->Save(path.c_str());
delete s;

s = NULL;
// spritesets can take a lot of memory, so do a little GC
JS_MaybeGC(cx);

return_bool ( saved );
end_method()

////////////////////////////////////////
/**
    - returns a copy of the spriteset object
*/
begin_method(SS_SPRITESET, ssSpritesetClone, 0)

sSpriteset* s = argSpriteset(cx, OBJECT_TO_JSVAL(obj));
if (s == NULL)
{
    return JS_FALSE;
}
*rval = PRIVATE_TO_JSVAL(s); // ROOT

SSPRITESET* clone = new SSPRITESET(*s);
delete s;
s = NULL;
if (!clone)
    return JS_FALSE;

return_object(CreateSpritesetObject(cx, clone));
// spritesets can take a lot of memory, so do a little GC
JS_MaybeGC(cx);

end_method()

////////////////////////////////////////
// SOUND OBJECTS ///////////////////////
////////////////////////////////////////

JSObject*
#if defined(WIN32) && defined(USE_MIDI)

CScript::CreateSoundObject(JSContext* cx, audiere::OutputStream* sound, audiere::MIDIStream* midi, audiere::File* memoryfile)
#else
CScript::CreateSoundObject(JSContext* cx, audiere::OutputStream* sound, audiere::File* memoryfile)
#endif
{
    if (sound)
        sound->ref();
#if defined(WIN32) && defined(USE_MIDI)
    if (midi)
        midi->ref();
#endif
    if (memoryfile)
        memoryfile->ref();

    // define class
    static JSClass clasp =
        {
            "sound", JSCLASS_HAS_PRIVATE,
            JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
            JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, ssFinalizeSound,
        };

    // create object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);
    if (object == NULL)
    {
        if (sound)
        {
            sound->unref();
            sound = NULL;
        }
#if defined(WIN32) && defined(USE_MIDI)

        if (midi)
        {
            midi->unref();
            midi = NULL;
        }

#endif
        if (memoryfile)
        {
            memoryfile->unref();
            memoryfile = NULL;
        }
        return NULL;
    }
    if (!JS_AddRoot(cx, &object))
        return NULL;

    // assign methods to the object
    static JSFunctionSpec fs[] =
        {
            { "play",        ssSoundPlay,        0, },
            { "pause",       ssSoundPause,       0, },
            { "stop",        ssSoundStop,        0, },
            { "reset",       ssSoundReset,       0, },
            { "setRepeat",   ssSoundSetRepeat,   1, },
            { "getRepeat",   ssSoundGetRepeat,   0, },
            { "setVolume",   ssSoundSetVolume,   1, },
            { "getVolume",   ssSoundGetVolume,   0, },
            { "setPan",      ssSoundSetPan,      1, },
            { "getPan",      ssSoundGetPan,      0, },
            { "setPitch",    ssSoundSetPitch,    1, },
            { "getPitch",    ssSoundGetPitch,    0, },
            { "getPosition", ssSoundGetPosition, 0, },
            { "setPosition", ssSoundSetPosition, 1, },
            { "getLength",   ssSoundGetLength,   0, },
            { "isPlaying",   ssSoundIsPlaying,   0, },
            { "isSeekable",  ssSoundIsSeekable,  0, },
            { "clone",       ssSoundClone,       0, },
            { 0, 0, 0, 0, 0 },
        };
    JS_DefineFunctions(cx, object, fs);

    // attach the sound to this object
    SS_SOUND* sound_object = new SS_SOUND;

    if (!sound_object)
    {
        if (sound)
        {

            sound->unref();
            sound = NULL;
        }
#if defined(WIN32) && defined(USE_MIDI)
        if (midi)
        {

            midi->unref();
            midi = NULL;
        }
#endif
        if (memoryfile)
        {
            memoryfile->unref();
            memoryfile = NULL;
        }
        JS_RemoveRoot(cx, &object);
        return NULL;
    }
    sound_object->sound = sound;
    sound_object->memoryfile = memoryfile;
#if defined(WIN32) && defined(USE_MIDI)
    sound_object->midi = midi;
#endif
    JS_SetPrivate(cx, object, sound_object);
    JS_RemoveRoot(cx, &object);
    return object;
}

////////////////////////////////////////
begin_finalizer(SS_SOUND, ssFinalizeSound)

if (object->sound){
    //object->sound->stop();
    object->sound->unref();
}

object->sound = NULL;

if(object->memoryfile){
    object->memoryfile->unref();
}



object->memoryfile = NULL;

#if defined(WIN32) && defined(USE_MIDI)

if (object->midi)
    object->midi->unref();

object->midi = NULL;

#endif
end_finalizer()

////////////////////////////////////////
/**
    - plays the sound. repeat is a boolean (true/false), that indicates if
      the sound should be looped
*/
begin_method(SS_SOUND, ssSoundPlay, 0)

if (argc >= 1)
{
    bool repeat = argBool(cx, argv[0]);
    argv[0] = BOOLEAN_TO_JSVAL(repeat); // ROOT as you go

    if (object->sound)
        object->sound->setRepeat(repeat);

#if defined(WIN32) && defined(USE_MIDI)

    if (object->midi)
        object->midi->setRepeat(repeat);

#endif
}

if (object->sound)
    object->sound->play();

#if defined(WIN32) && defined(USE_MIDI)

if (object->midi)
    object->midi->play();

#endif
end_method()

////////////////////////////////////////
/**
    - pauses playback. call play() again to resume playback.
*/
begin_method(SS_SOUND, ssSoundPause, 0)

if (object->sound)
{
    object->sound->stop();
}
#if defined(WIN32) && defined(USE_MIDI)

if (object->midi)
{
    object->midi->pause();
}
#endif
end_method()

////////////////////////////////////////
/**
    - stops playback
*/
begin_method(SS_SOUND, ssSoundStop, 0)

if (object->sound)
{
    object->sound->stop();
    object->sound->reset();
}

#if defined(WIN32) && defined(USE_MIDI)

if (object->midi)
    object->midi->stop();

#endif
end_method()

////////////////////////////////////////
/**
    - resets playback
    - no effect on MIDIs
*/
begin_method(SS_SOUND, ssSoundReset, 0)

if (object->sound)
    object->sound->reset();

end_method()

////////////////////////////////////////
/**
    - sets if the sound should be repeated
*/
begin_method(SS_SOUND, ssSoundSetRepeat, 1)
arg_bool(repeat);

if (object->sound)
    object->sound->setRepeat(repeat);

#if defined(WIN32) && defined(USE_MIDI)

if (object->midi)
    object->midi->setRepeat(repeat);

#endif
end_method()

////////////////////////////////////////
/**
    - returns true if sound is set to repeat, otherwise false
*/
begin_method(SS_SOUND, ssSoundGetRepeat, 0)
if (object->sound)
    return_bool(object->sound->getRepeat());

#if defined(WIN32) && defined(USE_MIDI)

if (object->midi)
    return_bool(object->midi->getRepeat());

#endif
end_method()

////////////////////////////////////////
/**
    - sets the volume for the sound (0-255)
    - no effect on MIDIs
*/
begin_method(SS_SOUND, ssSoundSetVolume, 1)
arg_int(volume);

if (object->sound)
{
    if (volume < 0)
        volume = 0;
    if (volume > 255)
        volume = 255;

    object->sound->setVolume(volume / 255.0f);
}

end_method()

////////////////////////////////////////
/**
    - returns the sound's volume (0-255)
    - no effect on MIDIs
*/
begin_method(SS_SOUND, ssSoundGetVolume, 0)

if (object->sound)
    return_int(object->sound->getVolume() * 255);
else
    return_int(255);

end_method()

////////////////////////////////////////
/**
    - pan can be from -255 to 255.  -255 = left, 255 = right. pan defaults to 0 (center).
    - no effect on MIDIs
*/
begin_method(SS_SOUND, ssSoundSetPan, 1)
arg_int(pan);

if (object->sound)
    object->sound->setPan(pan / 255.0f);

end_method()
////////////////////////////////////////
/**
    - returns the current pan of the sound
    - no effect on MIDIs
*/
begin_method(SS_SOUND, ssSoundGetPan, 0)

if (object->sound)
    return_int(object->sound->getPan() * 255);
else
    return_int(0);

end_method()

////////////////////////////////////////
/**
    - pitch ranges from 0.5 to 2.0.  0.5 is an octave down (and half as fast)
      while 2.0 is an octave up (and twice as fast).  pitch defaults to 1.0
    - no effect on MIDIs
*/
begin_method(SS_SOUND, ssSoundSetPitch, 1)
arg_double(pitch);

if (object->sound)
    object->sound->setPitchShift((float)pitch);

end_method()

////////////////////////////////////////
/**
    - returns the current pitch
    - no effect on MIDIs
*/
begin_method(SS_SOUND, ssSoundGetPitch, 0)

if (object->sound)
    return_double(object->sound->getPitchShift());
else
    return_double(1.0);

end_method()

////////////////////////////////////////
/**
    - returns true if the sound is currently playing
*/
begin_method(SS_SOUND, ssSoundIsPlaying, 0)

if (object->sound)
    return_bool(object->sound->isPlaying());

#if defined(WIN32) && defined(USE_MIDI)

else if (object->midi)
    return_bool(object->midi->isPlaying());

#endif

else
    return_bool(false);

end_method()

////////////////////////////////////////
/**
    - returns true if the sound is seekable
      Not all sound types are seekable, Ogg is.
*/
begin_method(SS_SOUND, ssSoundIsSeekable, 0)

if (object->sound)
    return_bool(object->sound->isSeekable());

#if defined(WIN32) && defined(USE_MIDI)

else if (object->midi)
    return_bool(true);

#endif

else
    return_bool(false);

end_method()
////////////////////////////////////////
/**
    - returns the position of the sound
      returns zero if the sound isn't seekable
*/
begin_method(SS_SOUND, ssSoundGetPosition, 0)

if (object->sound && object->sound->isSeekable())
    return_int(object->sound->getPosition());

#if defined(WIN32) && defined(USE_MIDI)

else if (object->midi)
    return_int(object->midi->getPosition());

#endif

else
    return_int(0);

end_method()

////////////////////////////////////////
/**
    - sets the position of the sound
      if the sound isn't seekable, this does nothing
*/
begin_method(SS_SOUND, ssSoundSetPosition, 1)
arg_int(pos);

if (object->sound)

    object->sound->setPosition(pos);

#if defined(WIN32) && defined(USE_MIDI)

if (object->midi)
    object->midi->setPosition(pos);

#endif
end_method()

////////////////////////////////////////
/**
    - gets the length of the sound
*/
begin_method(SS_SOUND, ssSoundGetLength, 0)

if (object->sound)
    return_int(object->sound->getLength());

#if defined(WIN32) && defined(USE_MIDI)

else if (object->midi)
    return_int(object->midi->getLength());

#endif

else
    return_int(0);

end_method()

////////////////////////////////////////
/**
    - creates a copy of the sound object (currently doesn't really work)
*/
begin_method(SS_SOUND, ssSoundClone, 0)
#if defined(WIN32) && defined(USE_MIDI)
return_object(CreateSoundObject(cx, object->sound, object->midi, object->memoryfile));
#else
return_object(CreateSoundObject(cx, object->sound, object->memoryfile));
#endif
end_method()


////////////////////////////////////////
// SOUND EFFECT OBJECT /////////////////
////////////////////////////////////////

JSObject*
CScript::CreateSoundEffectObject(JSContext* cx, audiere::SoundEffect* sound, audiere::File* memoryfile)
{
    if (sound)
        sound->ref();
    else{
        if(memoryfile)
            memoryfile->unref();
        return NULL;
    }
    if (memoryfile)
        memoryfile->ref();

    // define class
    static JSClass clasp =
        {
            "sound effect", JSCLASS_HAS_PRIVATE,
            JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
            JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, ssFinalizeSoundEffect,
        };

    // create object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);

    if (object == NULL)
    {
        if (sound)
        {
            sound->unref();
            sound = NULL;
        }

        if (memoryfile)
        {
            memoryfile->unref();
            memoryfile = NULL;
        }
        return NULL;
    }
    if (!JS_AddRoot(cx, &object))
        return NULL;

    // assign methods to the object
    static JSFunctionSpec fs[] =
        {
            { "play",        ssSoundEffectPlay,        0, },
            { "stop",        ssSoundEffectStop,        0, },
            { "setVolume",   ssSoundEffectSetVolume,   1, },
            { "getVolume",   ssSoundEffectGetVolume,   0, },
            { "setPan",      ssSoundEffectSetPan,      1, },
            { "getPan",      ssSoundEffectGetPan,      0, },
            { "setPitch",    ssSoundEffectSetPitch,    1, },
            { "getPitch",    ssSoundEffectGetPitch,    0, },
            { 0, 0, 0, 0, 0 },
        };
    JS_DefineFunctions(cx, object, fs);

    // attach the sound to this object
    SS_SOUNDEFFECT* sound_object = new SS_SOUNDEFFECT;

    if (!sound_object)
    {
        if (sound)
        {
            sound->unref();
            sound = NULL;
        }

        if (memoryfile)
        {
            memoryfile->unref();
            memoryfile = NULL;
        }
        JS_RemoveRoot(cx, &object);
        return NULL;
    }

    sound_object->sound = sound;
    sound_object->memoryfile = memoryfile;
    JS_SetPrivate(cx, object, sound_object);
    JS_RemoveRoot(cx, &object);
    return object;
}

////////////////////////////////////////
begin_finalizer(SS_SOUNDEFFECT, ssFinalizeSoundEffect)

if (object->sound)
{
    //object->sound->stop();
    object->sound->unref();
}
object->sound = NULL;

if(object->memoryfile)
    object->memoryfile->unref();

object->memoryfile = NULL;
end_finalizer()

////////////////////////////////////////
/**
    - plays the sound effect.
      If the sound effect is of type SE_SINGLE, this plays the sound
      if it isn't playing yet, and starts it again if it is.
      If the sound effect is of type SE_MULTIPLE, play() simply starts
      playing the sound again.
*/
begin_method(SS_SOUNDEFFECT, ssSoundEffectPlay, 0)

if (object->sound)
    object->sound->play();

end_method()

////////////////////////////////////////
/**
    - stops playback
      If the sound is of type SE_SINGLE, stop the sound.
      If it is of type SE_MULTIPLE, stop all playing instances of the sound.
*/
begin_method(SS_SOUNDEFFECT, ssSoundEffectStop, 0)

if (object->sound)
    object->sound->stop();

end_method()

////////////////////////////////////////
/**
    - sets the volume for the sound effect (0-255)
*/
begin_method(SS_SOUNDEFFECT, ssSoundEffectSetVolume, 1)
arg_int(volume);

if (object->sound)
{
    if (volume < 0)
        volume = 0;
    if (volume > 255)
        volume = 255;

    object->sound->setVolume(volume / 255.0f);
}

end_method()

////////////////////////////////////////
/**
    - returns the sound effect's volume (0-255)
*/
begin_method(SS_SOUNDEFFECT, ssSoundEffectGetVolume, 0)

if (object->sound)
    return_int(object->sound->getVolume() * 255);
else
    return_int(255);

end_method()

////////////////////////////////////////
/**
    - pan can be from -255 to 255.  -255 = left, 255 = right. pan defaults to 0 (center).
*/
begin_method(SS_SOUNDEFFECT, ssSoundEffectSetPan, 1)
arg_int(pan);

if (object->sound)
    object->sound->setPan(pan / 255.0f);

end_method()

////////////////////////////////////////
/**
    - returns the current pan of the sound effect
*/
begin_method(SS_SOUNDEFFECT, ssSoundEffectGetPan, 0)

if (object->sound)
    return_int(object->sound->getPan() * 255);
else
    return_int(0);

end_method()

////////////////////////////////////////
/**
    - pitch ranges from 0.5 to 2.0.  0.5 is an octave down (and half as fast)
      while 2.0 is an octave up (and twice as fast).  pitch defaults to 1.0
*/
begin_method(SS_SOUNDEFFECT, ssSoundEffectSetPitch, 1)
arg_double(pitch);

if (object->sound)
    object->sound->setPitchShift((float)pitch);

end_method()

////////////////////////////////////////
/**
    - returns the current pitch
*/
begin_method(SS_SOUNDEFFECT, ssSoundEffectGetPitch, 0)

if (object->sound)
    return_double(object->sound->getPitchShift());
else
    return_double(1.0);

end_method()


////////////////////////////////////////
////////////////////////////////////////
// SFXR OBJECTS ////////////////////////
////////////////////////////////////////

JSObject*
CScript::CreateSfxrObject(JSContext* cx, SSFXR* sfxr)
{
    // define class
    static JSClass clasp =
        {
            "sfxr", JSCLASS_HAS_PRIVATE,
            JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
            JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, ssFinalizeSfxr,
        };

    // create object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);
    if (!object || !JS_AddRoot(cx, &object))
        return NULL;

    // assign methods to the object
    static JSFunctionSpec fs[] =
        {
        { "saveWav",			ssSfxrSaveWav,			1, 0, 0 },
        { "reset",				ssSfxrReset,			0, 0, 0 },
        { "getSampleSize",		ssSfxrCalcSampleSize,	0, 0, 0 },
        { "getSoundEffect",		ssSfxrGetSoundEffect,	1, 0, 0 },
        { "getMasterVolume",	ssSfxrGetMasterVolume,	0, 0, 0 },
        { "setMasterVolume",	ssSfxrSetMasterVolume,	1, 0, 0 },
        { "getSoundVolume",		ssSfxrGetSoundVolume,	0, 0, 0 },
        { "setSoundVolume",		ssSfxrSetSoundVolume,	1, 0, 0 },
        { "getBitrate",			ssSfxrGetBitrate,		0, 0, 0 },
        { "setBitrate",			ssSfxrSetBitrate,		1, 0, 0 },
        { "getSampleRate",		ssSfxrGetSampleRate,	0, 0, 0 },
        { "setSampleRate",		ssSfxrSetSampleRate,	1, 0, 0 },
        { "getWaveType",		ssSfxrGetWaveType,      0, 0, 0 },
        { "setWaveType",		ssSfxrSetWaveType,      1, 0, 0 },
        { "getBaseFrequency",	ssSfxrGetBaseFrequency,	0, 0, 0 },
        { "setBaseFrequency",	ssSfxrSetBaseFrequency,	1, 0, 0 },
        { "getMinFrequency",	ssSfxrGetMinFrequency,	0, 0, 0 },
        { "setMinFrequency",	ssSfxrSetMinFrequency,	1, 0, 0 },
        { "getFrequencySlide",	ssSfxrGetFrequencySlide,	0, 0, 0 },
        { "setFrequencySlide",	ssSfxrSetFrequencySlide,	1, 0, 0 },
        { "getFrequencySlideDelta",	ssSfxrGetFrequencySlideDelta,	0, 0, 0 },
        { "setFrequencySlideDelta",	ssSfxrSetFrequencySlideDelta,	1, 0, 0 },
        { "getSquareDuty",	ssSfxrGetSquareDuty,	0, 0, 0 },
        { "setSquareDuty",	ssSfxrSetSquareDuty,	1, 0, 0 },
        { "getSquareDutySweep",	ssSfxrGetSquareDutySweep,	0, 0, 0 },
        { "setSquareDutySweep",	ssSfxrSetSquareDutySweep,	1, 0, 0 },
        { "getVibratoDepth",	ssSfxrGetVibratoDepth,	0, 0, 0 },
        { "setVibratoDepth",	ssSfxrSetVibratoDepth,	1, 0, 0 },
        { "getVibratoSpeed",	ssSfxrGetVibratoSpeed,	0, 0, 0 },
        { "setVibratoSpeed",	ssSfxrSetVibratoSpeed,	1, 0, 0 },
        { "getVibratoDelay",	ssSfxrGetVibratoDelay,	0, 0, 0 },
        { "setVibratoDelay",	ssSfxrSetVibratoDelay,	1, 0, 0 },
        { "getAttack",	ssSfxrGetAttack,	0, 0, 0 },
        { "setAttack",	ssSfxrSetAttack,	1, 0, 0 },
        { "getSustain",	ssSfxrGetSustain,	0, 0, 0 },
        { "setSustain",	ssSfxrSetSustain,	1, 0, 0 },
        { "getDecay",		ssSfxrGetDecay,	0, 0, 0 },
        { "setDecay",		ssSfxrSetDecay,	1, 0, 0 },
        { "getRelease",	ssSfxrGetRelease,	0, 0, 0 },
        { "setRelease",	ssSfxrSetRelease,	1, 0, 0 },
        { "getFilter",	ssSfxrGetFilter,	0, 0, 0 },
        { "setFilter",	ssSfxrSetFilter,	1, 0, 0 },
        { "getLowPassFilterCutoff",	ssSfxrGetLowPassFilterCutoff,	0, 0, 0 },
        { "setLowPassFilterCutoff",	ssSfxrSetLowPassFilterCutoff,	1, 0, 0 },
        { "getLowPassFilterCutoffSweep",	ssSfxrGetLowPassFilterCutoffSweep,	0, 0, 0 },
        { "setLowPassFilterCutoffSweep",	ssSfxrSetLowPassFilterCutoffSweep,	1, 0, 0 },
        { "getFilterResonance",	ssSfxrGetFilterResonance,	0, 0, 0 },
        { "setFilterResonance",	ssSfxrSetFilterResonance,	1, 0, 0 },
        { "getHighPassFilterCutoff",	ssSfxrGetHighPassFilterCutoff,	0, 0, 0 },
        { "setHighPassFilterCutoff",	ssSfxrSetHighPassFilterCutoff,	1, 0, 0 },
        { "getHighPassFilterCutoffSweep",	ssSfxrGetHighPassFilterCutoffSweep,	0, 0, 0 },
        { "setHighPassFilterCutoffSweep",	ssSfxrSetHighPassFilterCutoffSweep,	1, 0, 0 },
        { "getPhaserOffset",	ssSfxrGetPhaserOffset,	0, 0, 0 },
        { "setPhaserOffset",	ssSfxrSetPhaserOffset,	1, 0, 0 },
        { "getPhaserOffsetSweep",	ssSfxrGetPhaserOffsetSweep,	0, 0, 0 },
        { "setPhaserOffsetSweep",	ssSfxrSetPhaserOffsetSweep,	1, 0, 0 },
        { "getRepeatSpeed",	ssSfxrGetRepeatSpeed,	0, 0, 0 },
        { "setRepeatSpeed",	ssSfxrSetRepeatSpeed,	1, 0, 0 },
        { "getArpeggio",	ssSfxrGetArpeggio,	0, 0, 0 },
        { "setArpeggio",	ssSfxrSetArpeggio,	1, 0, 0 },
        { "getArpeggioSpeed",	ssSfxrGetArpeggioSpeed,	0, 0, 0 },
        { "setArpeggioSpeed",	ssSfxrSetArpeggioSpeed,	1, 0, 0 },
        { "clone",	ssSfxrClone,	0, 0, 0 },
        { "toJSON",	ssSfxrToJSON,	0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        };
    JS_DefineFunctions(cx, object, fs);

    // attach the sfxr to this object
    SS_SFXR* sfxr_object = new SS_SFXR;

    if (!sfxr_object)
    {
        JS_RemoveRoot(cx, &object);
        return NULL;
    }
    sfxr_object->sfxr       = sfxr;
    sfxr_object->destroy_me = true;//always destroy
    JS_SetPrivate(cx, object, sfxr_object);
    JS_RemoveRoot(cx, &object);
    return object;
}

///////////////////////////////////////
begin_finalizer(SS_SFXR, ssFinalizeSfxr)
if (object->destroy_me)
{
    if (object->sfxr)
    {
        delete object->sfxr;

    }
}

object->sfxr = NULL;
end_finalizer()

///////////////////////////////////////
/**
    - saves the sfxr as a wav file
      It will use the ./sounds/ directory by default
*/
begin_method(SS_SFXR, ssSfxrSaveWav, 1)
arg_str(filename);
bool saved = false;
if (IsValidPath(filename) == false)
{

    JS_ReportError(cx, "Invalid path: '%s'", filename);
    return JS_FALSE;
}
std::string path = "sounds/";
path += filename;
saved = object->sfxr->Save(path.c_str());
return_bool( saved );
end_method()

/**
    - resets all the values of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrReset, 0)
return_bool(object->sfxr->Reset());
end_method()

/**
    - Creates a soundeffect. The parameter can be SE_MULTIPLE or SE_SINGLE
*/
begin_method(SS_SFXR, ssSfxrGetSoundEffect, 0)
arg_int(type);
audiere::SoundEffect* sound = NULL;
audiere::File* memoryfile = NULL;
if (type == audiere::MULTIPLE)
    sound = object->sfxr->getSoundEffect(audiere::MULTIPLE, memoryfile);
else
    sound = object->sfxr->getSoundEffect(audiere::SINGLE, memoryfile);

if (!sound)
{
    if(memoryfile)
        memoryfile->unref();
    JS_ReportError(cx, "Sfxr could not export sound effect. ");
    return JS_FALSE;
}

return_object(CreateSoundEffectObject(cx, sound, memoryfile));
end_method()

/**
    - Get the sample size
*/
begin_method(SS_SFXR, ssSfxrCalcSampleSize, 0)
return_int(object->sfxr->GetSampleSize());
end_method()


/**
    - Set the master volume of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrSetMasterVolume, 1)
arg_double(volume);
object->sfxr->setMasterVolume((float)volume);
end_method()

/**
    - Get the master volume of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetMasterVolume, 0)
return_double(object->sfxr->getMasterVolume());
end_method()

/**
    - Get the volume of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetSoundVolume, 0)
return_double(object->sfxr->getSoundVolume());
end_method()

/**
    - Set the volume of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrSetSoundVolume, 1)
arg_double(v);
object->sfxr->setSoundVolume((float)v);
end_method()

/**
    - Get the bitrate of the sfxr object
      This value can be 8 or 16
*/
begin_method(SS_SFXR, ssSfxrGetBitrate, 0)
return_int(object->sfxr->getBitrate());
end_method()

/**
    - Set the bitrate of the sfxr object
      This value can be 8 or 16
*/
begin_method(SS_SFXR, ssSfxrSetBitrate, 1)
arg_int(v);
if ( (v != 8) && (v != 16))
{
    JS_ReportError(cx, "setBitrate: Bitrate must be 8 or 16, received '%i' instead", v);
    return JS_FALSE;
}else{
    object->sfxr->setBitrate(v);
}
end_method()

/**
    - Get the samplerate of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetSampleRate, 0)
return_int(object->sfxr->getSampleRate());
end_method()

/**
    - Set the sample rate of the sfxr object
      For example: 44100, 22500, 11000 or 8000.
      Changing the samplerate effectively changes the pitch
*/
begin_method(SS_SFXR, ssSfxrSetSampleRate, 1)
arg_int(v);
object->sfxr->setSampleRate(v);
end_method()

/**
    - Get the wave type of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetWaveType, 0)
    return_int(object->sfxr->getWaveType());
end_method()

/**
    - Set the wave type of the sfxr object
      Valid values are: SQUAREWAVE, SAWTOOTH, SINEWAVE and NOISE
*/
begin_method(SS_SFXR, ssSfxrSetWaveType, 1)
    arg_int(wavetype);
    if ( wavetype >= object->sfxr->getMaxWaveTypes() || wavetype < 0 )
    {
        JS_ReportError(cx, "Invalid Wavetype: '%d'", wavetype);
        return JS_FALSE;
    }
    object->sfxr->setWaveType(wavetype);
end_method()

/**
    - Get the base frequency of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetBaseFrequency, 0)
return_double(object->sfxr->getBaseFrequency());
end_method()

/**
    - Set the base frequency of the sfxr object
      This is for sliding the sound in pitch
      Value must be between 0 and 1
*/
begin_method(SS_SFXR, ssSfxrSetBaseFrequency, 1)
arg_double(v);
object->sfxr->setBaseFrequency((float)v);
end_method()

/**
    - Get the minimal frequency of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetMinFrequency, 0)
return_double(object->sfxr->getMinFrequency());
end_method()

/**
    - Set the minimal frequency of the sfxr object
      Value must be between 0 and 1
*/
begin_method(SS_SFXR, ssSfxrSetMinFrequency, 1)
arg_double(v);
object->sfxr->setMinFrequency((float)v);
end_method()

/**
    - Get the frequency slide of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetFrequencySlide, 0)
return_double(object->sfxr->getFrequencySlide());
end_method()

/**
    - Set the frequency slide of the sfxr object
      Value must be between -1 and 1
*/
begin_method(SS_SFXR, ssSfxrSetFrequencySlide, 1)
arg_double(v);
object->sfxr->setFrequencySlide((float)v);
end_method()

/**
    - Get the frequency slide delta of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetFrequencySlideDelta, 0)
return_double(object->sfxr->getFrequencySlideDelta());
end_method()

/**
    - Set the frequency slide delta of the sfxr object
      Value must be between -1 and 1
*/
begin_method(SS_SFXR, ssSfxrSetFrequencySlideDelta, 1)
arg_double(v);
object->sfxr->setFrequencySlideDelta((float)v);
end_method()

/**
    - Get the square duty of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetSquareDuty, 0)
return_double(object->sfxr->getSquareDuty());
end_method()

/**
    - Set the square duty of the sfxr object
      Value must be between 0 and 1
*/
begin_method(SS_SFXR, ssSfxrSetSquareDuty, 1)
arg_double(v);
object->sfxr->setSquareDuty((float)v);
end_method()

/**
    - Get the square duty sweep of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetSquareDutySweep, 0)
return_double(object->sfxr->getSquareDutySweep());
end_method()

/**
    - Set the square duty sweep of the sfxr object
      Value must be between 0 and 1
*/
begin_method(SS_SFXR, ssSfxrSetSquareDutySweep, 1)
arg_double(v);
object->sfxr->setSquareDutySweep((float)v);
end_method()

/**
    - Get the vibrato depth of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetVibratoDepth, 0)
return_double(object->sfxr->getVibratoDepth());
end_method()

/**
    - Set the vibrato depth of the sfxr object
      Value must be between 0 and 1
*/
begin_method(SS_SFXR, ssSfxrSetVibratoDepth, 1)
arg_double(v);
object->sfxr->setVibratoDepth((float)v);
end_method()

/**
    - Get the vibrato speed of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetVibratoSpeed, 0)
return_double(object->sfxr->getVibratoSpeed());
end_method()

/**
    - Set the vibrato speed of the sfxr object
      Value must be between 0 and 1
*/
begin_method(SS_SFXR, ssSfxrSetVibratoSpeed, 1)
arg_double(v);
object->sfxr->setVibratoSpeed((float)v);
end_method()

/**
    - Get the vibrato delay of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetVibratoDelay, 0)
return_double(object->sfxr->getVibratoDelay());
end_method()

/**
    - Set the vibrato delay of the sfxr object
      Value must be between 0 and 1
*/
begin_method(SS_SFXR, ssSfxrSetVibratoDelay, 1)
arg_double(v);
object->sfxr->setVibratoDelay((float)v);
end_method()

/**
    - Get the attack envelope of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetAttack, 0)
return_double(object->sfxr->getAttack());
end_method()

/**
    - Set the attack envelope of the sfxr object
      Value must be between 0 and 1
*/
begin_method(SS_SFXR, ssSfxrSetAttack, 1)
arg_double(v);
object->sfxr->setAttack((float)v);
end_method()

/**
    - Get the sustain envelope of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetSustain, 0)
return_double(object->sfxr->getSustain());
end_method()

/**
    - Set the sustain envelope of the sfxr object
      Value must be between 0 and 1
*/
begin_method(SS_SFXR, ssSfxrSetSustain, 1)
arg_double(v);
object->sfxr->setSustain((float)v);
end_method()

/**
    - Get the decay envelope of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetDecay, 0)
return_double(object->sfxr->getDecay());
end_method()

/**
    - Set the decay envelope of the sfxr object
      Value must be between 0 and 1
*/
begin_method(SS_SFXR, ssSfxrSetDecay, 1)
arg_double(v);
object->sfxr->setDecay((float)v);
end_method()

/**
    - Get the release envelope of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetRelease, 0)
return_double(object->sfxr->getRelease());
end_method()

/**
    - Set the release envelope of the sfxr object
      Value must be between 0 and 1
*/
begin_method(SS_SFXR, ssSfxrSetRelease, 1)
arg_double(v);
object->sfxr->setRelease((float)v);
end_method()

/**
    - Get the filter of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetFilter, 0)
return_bool(object->sfxr->getFilter());
end_method()

/**
    - Set the filter of the sfxr object
      Not sure what this does
*/
begin_method(SS_SFXR, ssSfxrSetFilter, 1)
arg_bool(isOn);
object->sfxr->setFilter(isOn);
end_method()

/**
    - Get the low pass filter cutoff of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetLowPassFilterCutoff, 0)
return_double(object->sfxr->getLowPassFilterCutoff());
end_method()

/**
    - Set the low pass filter cutoff of the sfxr object
      Value must be between 0 and 1
*/
begin_method(SS_SFXR, ssSfxrSetLowPassFilterCutoff, 1)
arg_double(v);
object->sfxr->setLowPassFilterCutoff((float)v);
end_method()

/**
    - Get the low pass filter cutoff sweep of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetLowPassFilterCutoffSweep, 0)
return_double(object->sfxr->getLowPassFilterCutoffSweep());
end_method()

/**
    - Set the low pass filter cutoff sweep of the sfxr object
      Value must be between -1 and 1
*/
begin_method(SS_SFXR, ssSfxrSetLowPassFilterCutoffSweep, 1)
arg_double(v);
object->sfxr->setLowPassFilterCutoffSweep((float)v);
end_method()

/**
    - Get the filter resonance of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetFilterResonance, 0)
return_double(object->sfxr->getFilterResonance());
end_method()

/**
    - Set the filter resonance of the sfxr object
      Value must be between 0 and 1
*/
begin_method(SS_SFXR, ssSfxrSetFilterResonance, 1)
arg_double(v);
object->sfxr->setFilterResonance((float)v);
end_method()

/**
    - Get the high pass filter cutoff of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetHighPassFilterCutoff, 0)
return_double(object->sfxr->getHighPassFilterCutoff());
end_method()

/**
    - Set the high pass filter cutoff of the sfxr object
      Value must be between 0 and 1
*/
begin_method(SS_SFXR, ssSfxrSetHighPassFilterCutoff, 1)
arg_double(v);
object->sfxr->setHighPassFilterCutoff((float)v);
end_method()

/**
    - Get the high pass filter cutoff sweep of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetHighPassFilterCutoffSweep, 0)
return_double(object->sfxr->getHighPassFilterCutoffSweep());
end_method()

/**
    - Set the high pass filter cutoff sweep of the sfxr object
      Value must be between -1 and 1
*/
begin_method(SS_SFXR, ssSfxrSetHighPassFilterCutoffSweep, 1)
arg_double(v);
object->sfxr->setHighPassFilterCutoffSweep((float)v);
end_method()

/**
    - Get the phaser offset of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetPhaserOffset, 0)
return_double(object->sfxr->getPhaserOffset());
end_method()

/**
    - Set the phaser offset of the sfxr object
      Value must be between -1 and 1
*/
begin_method(SS_SFXR, ssSfxrSetPhaserOffset, 1)
arg_double(v);
object->sfxr->setPhaserOffset((float)v);
end_method()

/**
    - Get the phaser offset sweep of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetPhaserOffsetSweep, 0)
return_double(object->sfxr->getPhaserOffsetSweep());
end_method()

/**
    - Set the phaser offset sweep of the sfxr object
      Value must be between -1 and 1
*/
begin_method(SS_SFXR, ssSfxrSetPhaserOffsetSweep, 1)
arg_double(v);
object->sfxr->setPhaserOffsetSweep((float)v);
end_method()

/**
    - Get the repeat speed of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetRepeatSpeed, 0)
return_double(object->sfxr->getRepeatSpeed());
end_method()

/**
    - Set the repeat speed of the sfxr object
      Value must be between 0 and 1
*/
begin_method(SS_SFXR, ssSfxrSetRepeatSpeed, 1)
arg_double(v);
object->sfxr->setRepeatSpeed((float)v);
end_method()

/**
    - Get the arpeggio of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetArpeggio, 0)
return_double(object->sfxr->getArpeggio());
end_method()

/**
    - Set the arpeggio of the sfxr object
      Value must be between 0 and 1
*/
begin_method(SS_SFXR, ssSfxrSetArpeggio, 1)
arg_double(v);
object->sfxr->setArpeggio((float)v);
end_method()

/**
    - Get the arpeggio speed of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrGetArpeggioSpeed, 0)
return_double(object->sfxr->getArpeggioSpeed());
end_method()

/**
    - Set the arpeggio speed of the sfxr object
      Value must be between -1 and 1
*/
begin_method(SS_SFXR, ssSfxrSetArpeggioSpeed, 1)
arg_double(v);
object->sfxr->setArpeggioSpeed((float)v);
end_method()

////////////////////////////////////////
/**
    - creates a copy of the sfxr object
*/
begin_method(SS_SFXR, ssSfxrClone, 0)
return_object(CreateSfxrObject(cx, object->sfxr->Clone()));
end_method()

/**
    - exports the Sfxr to a string, used internally by JSON
      This string can be eval()'ed to recreate the sfxr.
      note the float roundup: For .setBaseFrequency(0.123456789) the functions
      .getBaseFrequency() returns 0.123456789104328156 and toJSON returns 0.123457
*/
begin_method(SS_SFXR, ssSfxrToJSON, 0)
char json2[512];
// Although getFilter() should be bool, it will work just fine.
sprintf(json2,"CreateSfxr(%d,%d,%g,%d,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%d,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g)",
    object->sfxr->getBitrate(),
    object->sfxr->getSampleRate(),
    object->sfxr->getSoundVolume(),
    object->sfxr->getWaveType(),
    object->sfxr->getBaseFrequency(),
    object->sfxr->getMinFrequency(),
    object->sfxr->getFrequencySlide(),
    object->sfxr->getFrequencySlideDelta(),
    object->sfxr->getSquareDuty(),
    object->sfxr->getSquareDutySweep(),
    object->sfxr->getVibratoDepth(),
    object->sfxr->getVibratoSpeed(),
    object->sfxr->getVibratoDelay(),
    object->sfxr->getAttack(),
    object->sfxr->getSustain(),
    object->sfxr->getDecay(),
    object->sfxr->getRelease(),
    object->sfxr->getFilter(),
    object->sfxr->getLowPassFilterCutoff(),
    object->sfxr->getLowPassFilterCutoffSweep(),
    object->sfxr->getFilterResonance(),
    object->sfxr->getHighPassFilterCutoff(),
    object->sfxr->getHighPassFilterCutoffSweep(),
    object->sfxr->getPhaserOffset(),
    object->sfxr->getPhaserOffsetSweep(),
    object->sfxr->getRepeatSpeed(),
    object->sfxr->getArpeggio(),
    object->sfxr->getArpeggioSpeed()
);
return_str(json2);
end_method()


////////////////////////////////////////
////////////////////////////////////////
// FONT OBJECTS ////////////////////////
////////////////////////////////////////

JSObject*
CScript::CreateFontObject(JSContext* cx, SFONT* font, bool destroy)
{

    // define class
    static JSClass clasp =
        {
            "font", JSCLASS_HAS_PRIVATE,
            JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
            JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, ssFinalizeFont,
        };

    // create object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);
    if (!object || !JS_AddRoot(cx, &object))
        return NULL;

    // assign methods to the object
    static JSFunctionSpec fs[] =
        {
            { "setColorMask",      ssFontSetColorMask,      1, 0, 0 },
            { "getColorMask",      ssFontGetColorMask,      0, 0, 0 },
            { "drawText",          ssFontDrawText,          3, 0, 0 },
            { "drawZoomedText",    ssFontDrawZoomedText,    4, 0, 0 },
            { "drawTextBox",       ssFontDrawTextBox,       6, 0, 0 },
            { "wordWrapString",    ssFontWordWrapString,    2, 0, 0 },
            { "getHeight",         ssFontGetHeight,         0, 0, 0 },
            { "getStringWidth",    ssFontGetStringWidth,    1, 0, 0 },
            { "getStringHeight",   ssFontGetStringHeight,   2, 0, 0 },
            { "clone",             ssFontClone,             0, 0, 0 },
            { "save",              ssFontSave,              0, 0, 0 },
            { "getCharacterImage", ssFontGetCharacterImage, 1, 0, 0 },
            { "setCharacterImage", ssFontSetCharacterImage, 2, 0, 0 },
            { 0, 0, 0, 0, 0 },
        };
    JS_DefineFunctions(cx, object, fs);

    // attach the font to this object
    SS_FONT* font_object = new SS_FONT;

    if (!font_object)
    {
        JS_RemoveRoot(cx, &object);
        return NULL;
    }
    font_object->font       = font;
    font_object->destroy_me = destroy;
    font_object->mask       = CreateRGBA(255, 255, 255, 255);
    JS_SetPrivate(cx, object, font_object);
    JS_RemoveRoot(cx, &object);
    return object;
}

///////////////////////////////////////
begin_finalizer(SS_FONT, ssFinalizeFont)
if (object->destroy_me)
{

    if (object->font)
    {
        delete object->font;

    }
}

object->font = NULL;
end_finalizer()
///////////////////////////////////////
/**
    - Sets the color mask for a font
    @see ApplyColorMask
*/
begin_method(SS_FONT, ssFontSetColorMask, 1)
arg_color(mask);
object->mask = mask;
end_method()

///////////////////////////////////////
/**
    - Gets the color mask being used by the font object
*/
begin_method(SS_FONT, ssFontGetColorMask, 0)
return_object(CreateColorObject(cx, object->mask));
end_method()

///////////////////////////////////////
/**
    - draws 'text' at x, y with the font.
      You can use octal notation to write characters that can not be written.
      for example: \222 for right single quotation mark. Sphere Font is CP1252. 
*/
begin_method(SS_FONT, ssFontDrawText, 3)
if (This->ShouldRender())
{
    arg_int(x);
    arg_int(y);
    arg_str(text);
    object->font->DrawString(x, y, text, object->mask);
}
end_method()

///////////////////////////////////////
/**
    - draws scaled text (1.0 = normal) with (x,y) as the upper left corner
*/
begin_method(SS_FONT, ssFontDrawZoomedText, 4)
if (This->ShouldRender())
{
    arg_int(x);
    arg_int(y);
    arg_double(scale);
    arg_str(text);

    object->font->DrawZoomedString(x, y, scale, text, object->mask);
}
end_method()

///////////////////////////////////////
/**
    - draws a word-wrapped text at (x, y) with the width w and height h. The
      offset is the number of pixels which the number of pixels from y which
      the actual drawing starts at.
    Note: 'text' can have the following special characters within it:
      \n - newline
      \t - tab
      \" - double quote
      \' - single quote

     For example: font_object.drawTextBox(16, 16, 200, 200, 0, "Line One\nLine Two");
*/
begin_method(SS_FONT, ssFontDrawTextBox, 6)
if (This->ShouldRender())
{
    arg_int(x);
    arg_int(y);
    arg_int(w);
    arg_int(h);
    arg_int(offset);
    arg_str(text);
    object->font->DrawTextBox(x, y, w, h, offset, text, object->mask);
}
end_method()

///////////////////////////////////////
/**
    - Splits a string into an array of lines as if it were wrapped using
      font_object.drawTextBox().
*/
begin_method(SS_FONT, ssFontWordWrapString, 2)
arg_str(string);
arg_int(width);

// Split the string into lines.
std::vector<std::string> lines = object->font->WordWrapString(string, width);

// Build array of strings.
int array_size = lines.size();
jsval* array = new jsval[array_size];
if (!array)
{
    return JS_FALSE;
}
*rval = *array;
for (int i = 0; i < array_size; ++i)
{
    array[i] = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, lines[i].c_str()));
}

// Convert array to JavaScript Array object.
JSObject* line_array_object = JS_NewArrayObject(cx, array_size, array);
delete[] array;
return_object(line_array_object);

end_method()

///////////////////////////////////////
/**
    - returns the height of the font, in pixels
*/
begin_method(SS_FONT, ssFontGetHeight, 0)
return_int(object->font->GetMaxHeight());
end_method()

///////////////////////////////////////
/**
    - returns the width of a given string, in pixels
*/
begin_method(SS_FONT, ssFontGetStringWidth, 1)
arg_str(text);
return_int(object->font->GetStringWidth(text));
end_method()

///////////////////////////////////////
/**
    - returns the height of the string as if it was drawn by drawTextBox
*/
begin_method(SS_FONT, ssFontGetStringHeight, 2)
arg_str(text);
arg_int(width);
return_int(object->font->GetStringHeight(text, width));
end_method()

///////////////////////////////////////
/**
    - returns a copy of the font object
*/
begin_method(SS_FONT, ssFontClone, 0)

SFONT* font = object->font->Clone();
if (!font)
{

    return_object(JSVAL_NULL);
}
else
{

    return_object(CreateFontObject(cx, font, true));
}
end_method()

///////////////////////////////////////
begin_method(SS_FONT, ssFontSave, 1)
arg_str(filename);
const char* type = "rfn";
bool saved = false;
if (IsValidPath(filename) == false)
{

    JS_ReportError(cx, "Invalid path: '%s'", filename);
    return JS_FALSE;
}
std::string path = "fonts/";
path += filename;
if (strcmp(type, "rfn") == 0)
{

    saved = object->font->Save(path.c_str());
}
return_bool( saved );
end_method()
///////////////////////////////////////
begin_method(SS_FONT, ssFontGetCharacterImage, 1)
arg_int(index);
IMAGE image;
if ( !object->font->GetCharacterImage(index, image) )
{

    This->ReportMapEngineError("font.getCharacter() failed");
    return JS_FALSE;
}
return_object(CreateImageObject(cx, image, true));
end_method()
///////////////////////////////////////
begin_method(SS_FONT, ssFontSetCharacterImage, 2)
arg_int(index);
arg_image(image);
if ( !object->font->SetCharacterImage(index, image->image) )
{

    This->ReportMapEngineError("font.setCharacter() failed");
    return JS_FALSE;
}
end_method()
///////////////////////////////////////
///////////////////////////////////////
// WINDOW STYLE OBJECTS ///////////////
///////////////////////////////////////

JSObject*
CScript::CreateWindowStyleObject(JSContext* cx, SWINDOWSTYLE* ws, bool destroy)
{
    // define class
    static JSClass clasp =
        {
            "windowstyle", JSCLASS_HAS_PRIVATE,
            JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
            JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, ssFinalizeWindowStyle,
        };

    // create object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);
    if (!object || !JS_AddRoot(cx, &object))
        return NULL;

    // assign methods to the object
    static JSFunctionSpec fs[] =
        {
            { "drawWindow",   ssWindowStyleDrawWindow,      4, 0, 0 },
            { "setColorMask", ssWindowStyleSetColorMask,    1, 0, 0 },
            { "getColorMask", ssWindowStyleGetColorMask,    0, 0, 0 },
            { "clone",        ssWindowStyleClone,           0, 0, 0 },
            { "save",         ssWindowStyleSave,            1, 0, 0 },
            { "getBorder",    ssWindowStyleGetBorder,       1, 0, 0 },
            { 0, 0, 0, 0, 0 },
        };
    JS_DefineFunctions(cx, object, fs);

    // attach the window style to this object
    SS_WINDOWSTYLE* ws_object = new SS_WINDOWSTYLE;

    if (!ws_object)
    {
        JS_RemoveRoot(cx, &object);
        return NULL;
    }
    ws_object->windowstyle = ws;
    ws_object->destroy_me  = destroy;
    ws_object->mask = CreateRGBA(255, 255, 255, 255);
    JS_SetPrivate(cx, object, ws_object);
    JS_RemoveRoot(cx, &object);
    return object;
}

///////////////////////////////////////
begin_finalizer(SS_WINDOWSTYLE, ssFinalizeWindowStyle)
if (object->destroy_me)
{

    if (object->windowstyle)
    {
        delete object->windowstyle;

    }
}
object->windowstyle = NULL;

end_finalizer()
///////////////////////////////////////
/**
    - draws the window at (x, y) with the width and height of w and h.
      Note that window corners and edges are drawn outside of the width
      and height of the window.
*/
begin_method(SS_WINDOWSTYLE, ssWindowStyleDrawWindow, 4)
if (This->ShouldRender())
{
    arg_int(x);
    arg_int(y);
    arg_int(w);
    arg_int(h);

    object->windowstyle->DrawWindow(x, y, w, h, object->mask, !(object->mask == CreateRGBA(255, 255, 255, 255)));
}
end_method()

///////////////////////////////////////
/**
    - sets the color mask for a windowstyle
    @see ApplyColorMask
*/
begin_method(SS_WINDOWSTYLE, ssWindowStyleSetColorMask, 1)
arg_color(color);
object->mask = color;
end_method()

///////////////////////////////////////
/**
    - gets the color mask being used by the windowstyle object
*/
begin_method(SS_WINDOWSTYLE, ssWindowStyleGetColorMask, 0)
return_object(CreateColorObject(cx, object->mask));
end_method()

///////////////////////////////////////
/**
    - [DISABLED] save the windowstyle object (this just returns false...)
*/
begin_method(SS_WINDOWSTYLE, ssWindowStyleSave, 1)
arg_str(filename);
return_bool(false);
end_method()
///////////////////////////////////////
/**
    - [DISABLED] clone the windowstyle object
*/
begin_method(SS_WINDOWSTYLE, ssWindowStyleClone, 0)
return_object(object);
end_method()
///////////////////////////////////////
/**
    - get the border size in pixels for the windowstyle object.
      You can use EDGE_LEFT, EDGE_TOP, EDGE_RIGHT and EDGE_BOTTOM as parameters.
      Note that the corner sizes are ignored.
*/
begin_method(SS_WINDOWSTYLE, ssWindowStyleGetBorder, 1)
arg_int(index);
return_int(object->windowstyle->GetBorder(index));
end_method()

///////////////////////////////////////
///////////////////////////////////////
// IMAGE OBJECTS //////////////////////
///////////////////////////////////////

JSObject*
CScript::CreateImageObject(JSContext* cx, IMAGE image, bool destroy)
{
    if (!image)
        return NULL;

    // define image class
    static JSClass clasp =
        {
            "image", JSCLASS_HAS_PRIVATE,
            JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
            JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, ssFinalizeImage,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        };

    // create the object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);
    if (!object || !JS_AddRoot(cx, &object))
        return NULL;

    // assign methods to the object
    static JSFunctionSpec fs[] =
        {
            { "blit",              ssImageBlit,              2, 0, 0 },
            { "blitMask",          ssImageBlitMask,          3, 0, 0 },
            { "rotateBlit",        ssImageRotateBlit,        3, 0, 0 },
            { "rotateBlitMask",    ssImageRotateBlitMask,    4, 0, 0 },
            { "zoomBlit",          ssImageZoomBlit,          3, 0, 0 },
            { "zoomBlitMask",      ssImageZoomBlitMask,      4, 0, 0 },
            { "transformBlit",     ssImageTransformBlit,     8, 0, 0 },
            { "transformBlitMask", ssImageTransformBlitMask, 9, 0, 0 },
            { "createSurface",     ssImageCreateSurface,     0, 0, 0 },
            { "clone",             ssImageClone,             0, 0, 0 },
            { 0, 0, 0, 0, 0 },
        };
    JS_DefineFunctions(cx, object, fs);

    // define width and height properties
    JS_DefineProperty(cx, object, "width",  INT_TO_JSVAL(GetImageWidth(image)),  JS_PropertyStub, JS_PropertyStub, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineProperty(cx, object, "height", INT_TO_JSVAL(GetImageHeight(image)), JS_PropertyStub, JS_PropertyStub, JSPROP_READONLY | JSPROP_PERMANENT);

    // attach the image to this object
    SS_IMAGE* image_object = new SS_IMAGE;

    if (!image_object)
    {
        JS_RemoveRoot(cx, &object);
        return NULL;
    }
    image_object->image       = image;
    image_object->destroy_me  = destroy;
    JS_SetPrivate(cx, object, image_object);
    JS_RemoveRoot(cx, &object);
    return object;
}

////////////////////////////////////////////////////////////////////////////////
begin_finalizer(SS_IMAGE, ssFinalizeImage)
if (object->destroy_me)
{
    DestroyImage(object->image);

}
object->image = NULL;

end_finalizer()

///////////////////////////////////////
static CImage32::BlendMode
int_to_image_blendmode(int blendmode)
{
    switch (blendmode)
    {
        case CImage32::BLEND:    return CImage32::BLEND;
        case CImage32::ADD:      return CImage32::ADD;
        case CImage32::SUBTRACT: return CImage32::SUBTRACT;
        case CImage32::MULTIPLY: return CImage32::MULTIPLY;
        default:                 return CImage32::BLEND;
    }
}

///////////////////////////////////////
static CImage32::BlendMode
int_to_image_mask_blendmode(int blendmode)
{
    switch (blendmode)
    {
        case CImage32::ADD:      return CImage32::ADD;
        case CImage32::SUBTRACT: return CImage32::SUBTRACT;
        case CImage32::MULTIPLY: return CImage32::MULTIPLY;
        default:                 return CImage32::MULTIPLY;
    }
}

///////////////////////////////////////
/**
    - draws the image onto the video buffer at x,y
*/
begin_method(SS_IMAGE, ssImageBlit, 2)
if (This->ShouldRender())
{
    arg_int(x);
    arg_int(y);

    if (argc >= 3)
    {
		int blendmode = argInt(cx, argv[2]);
        BlitImage(object->image, x, y, int_to_image_blendmode(blendmode));
	}else
        BlitImage(object->image, x, y, CImage32::BLEND);
}
end_method()

///////////////////////////////////////
/**
    - draws the image into the video buffer, except that the color passed
      as 'mask' tints the image
*/
begin_method(SS_IMAGE, ssImageBlitMask, 3)
if (This->ShouldRender())
{
    arg_int(x);
    arg_int(y);
    arg_color(col);

    int blendmode = CImage32::BLEND;
    if (argc >= 4)
        blendmode = argInt(cx, argv[3]);

    int mask_blendmode = CImage32::MULTIPLY;
    if (argc >= 5)
        mask_blendmode = argInt(cx, argv[4]);

    BlitImageMask(object->image, x, y, int_to_image_blendmode(blendmode),
                  col, int_to_image_mask_blendmode(mask_blendmode));
}
end_method()

///////////////////////////////////////
void CalculateRotateBlitPoints(int tx[4], int ty[4], double x, double y, double w, double h, double radians)
{
    double r = sqrt(w * w + h * h) / (double)2.0;

    // various useful angles
    const double PI = 3.14159265358979323846;
    const double angle = atan((double)w / (double)h);  // h shouldn't be zero...
    double upper_left_angle  = -angle;
    double upper_right_angle = angle;
    double lower_right_angle = PI - angle;
    double lower_left_angle  = PI + angle;

    // center of the image
    double cx = (double)x + (double)w / 2.0;
    double cy = (double)y + (double)h / 2.0;

    tx[0] = int(cx + r * sin(upper_left_angle  + radians));
    tx[1] = int(cx + r * sin(upper_right_angle + radians));
    tx[2] = int(cx + r * sin(lower_right_angle + radians));
    tx[3] = int(cx + r * sin(lower_left_angle  + radians));

    // I'm not sure why we're doing subtraction here...
    // one of those "just smile and nod" things
    ty[0] = int(cy - r * cos(upper_left_angle  + radians));
    ty[1] = int(cy - r * cos(upper_right_angle + radians));
    ty[2] = int(cy - r * cos(lower_right_angle + radians));
    ty[3] = int(cy - r * cos(lower_left_angle  + radians));
}

///////////////////////////////////////
/**
    - draws the image into the video buffer, except that the image is rotates
      in  anti-clockwise in radians, which have a range of 0-2*pi.
      (x,y) is the center of the blit.
*/
begin_method(SS_IMAGE, ssImageRotateBlit, 3)
if (This->ShouldRender())
{
    arg_int(x);
    arg_int(y);
    arg_double(radians);

    int blendmode = CImage32::BLEND;
    if (argc >= 4)
    {
        blendmode = argInt(cx, argv[3]);
    }

    int tx[4];
    int ty[4];

    int w = GetImageWidth(object->image);
    int h = GetImageHeight(object->image);

    CalculateRotateBlitPoints(tx, ty, x, y, w, h, radians);
    TransformBlitImage(object->image, tx, ty, int_to_image_blendmode(blendmode));
}

end_method()
///////////////////////////////////////
/**
    - rotateBlit + mask
*/
begin_method(SS_IMAGE, ssImageRotateBlitMask, 4)
if (This->ShouldRender())
{
    arg_int(x);
    arg_int(y);
    arg_double(radians);
    arg_color(color);

    int blendmode = CImage32::BLEND;
    if (argc >= 5)
    {
        blendmode = argInt(cx, argv[4]);
    }

    int mask_blendmode = CImage32::MULTIPLY;
    if (argc >= 6)
        mask_blendmode = argInt(cx, argv[5]);

    int tx[4];
    int ty[4];
    int w = GetImageWidth(object->image);
    int h = GetImageHeight(object->image);
    CalculateRotateBlitPoints(tx, ty, x, y, w, h, radians);
    TransformBlitImageMask(object->image, tx, ty, int_to_image_blendmode(blendmode),
                           color, int_to_image_mask_blendmode(mask_blendmode));
}
end_method()
///////////////////////////////////////
/**
    - draws the image into the video buffer with zooming, with the scaling
      depending on factor. Normally a factor of 1 will blit a normal looking
      image. Between 0 and 1 will shrink the image. Any values greater than 1
      will stretch the size of the image.
*/
begin_method(SS_IMAGE, ssImageZoomBlit, 3)
if (This->ShouldRender())
{
    arg_int(x);
    arg_int(y);
    arg_double(factor);

    int blendmode = CImage32::BLEND;
    if (argc >= 4)
    {
        blendmode = argInt(cx, argv[3]);
    }

    int w = GetImageWidth(object->image);
    int h = GetImageHeight(object->image);

    int tx[4] = { x, x + (int)(w * factor), x + (int)(w * factor), x };
    int ty[4] = { y, y, y + (int)(h * factor), y + (int)(h * factor) };

    TransformBlitImage(object->image, tx, ty, int_to_image_blendmode(blendmode));
}
end_method()

///////////////////////////////////////
/**
    - zoomBlit + mask
*/
begin_method(SS_IMAGE, ssImageZoomBlitMask, 4)
if (This->ShouldRender())
{

    arg_int(x);
    arg_int(y);
    arg_double(factor);
    arg_color(color);

    int blendmode = CImage32::BLEND;
    if (argc >= 5)
    {
        blendmode = argInt(cx, argv[4]);
    }

    int mask_blendmode = CImage32::MULTIPLY;
    if (argc >= 6)
        mask_blendmode = argInt(cx, argv[5]);

    int w = GetImageWidth(object->image);
    int h = GetImageHeight(object->image);
    int tx[4] = { x, x + (int)(w * factor), x + (int)(w * factor), x };
    int ty[4] = { y, y, y + (int)(h * factor), y + (int)(h * factor) };

    TransformBlitImageMask(object->image, tx, ty, int_to_image_blendmode(blendmode),
                           color, int_to_image_mask_blendmode(mask_blendmode));
}
end_method()
///////////////////////////////////////
/**
    - draws the image into the video buffer with "transformation", where
      (x1, y1) is the upper left corner, (x2, y2) the upper right corner,
      (x3, y3) is the lower right corner, and (x4, y4) is the lower left
      corner.
*/
begin_method(SS_IMAGE, ssImageTransformBlit, 8)
if (This->ShouldRender())
{
    arg_int(x1);
    arg_int(y1);
    arg_int(x2);
    arg_int(y2);
    arg_int(x3);
    arg_int(y3);
    arg_int(x4);
    arg_int(y4);

    int blendmode = CImage32::BLEND;
    if (argc >= 9)
    {
        blendmode = argInt(cx, argv[8]);
    }

    int x[4] = { x1, x2, x3, x4 };
    int y[4] = { y1, y2, y3, y4 };
    TransformBlitImage(object->image, x, y, int_to_image_blendmode(blendmode));
}
end_method()

///////////////////////////////////////
/**
    - transformBlit + blitMask
*/
begin_method(SS_IMAGE, ssImageTransformBlitMask, 9)
if (This->ShouldRender())
{
    arg_int(x1);
    arg_int(y1);
    arg_int(x2);
    arg_int(y2);
    arg_int(x3);
    arg_int(y3);
    arg_int(x4);
    arg_int(y4);
    arg_color(mask);

    int blendmode = CImage32::BLEND;
    if (argc >= 10)
    {
        blendmode = argInt(cx, argv[9]);
    }

    int mask_blendmode = CImage32::MULTIPLY;
    if (argc >= 11)
        mask_blendmode = argInt(cx, argv[10]);

    int x[4] = { x1, x2, x3, x4 };
    int y[4] = { y1, y2, y3, y4 };
    TransformBlitImageMask(object->image, x, y, int_to_image_blendmode(blendmode),
    mask, int_to_image_mask_blendmode(mask_blendmode));
}
end_method()

///////////////////////////////////////
/**
    - returns a new surface object from the image
*/
begin_method(SS_IMAGE, ssImageCreateSurface, 0)
int width  = GetImageWidth(object->image);
int height = GetImageHeight(object->image);
RGBA* pixels = LockImage(object->image);

CImage32* surface = new CImage32(width, height, pixels);
if (!surface || surface->GetWidth() != width || surface->GetHeight() != height)
{

    if (surface)
    {
        delete surface;
        surface = NULL;
    }

    JS_ReportError(cx, "createSurface() failed!!");
    return JS_FALSE;
}
UnlockImage(object->image, false);
return_object(CreateSurfaceObject(cx, surface));
end_method()

////////////////////////////////////////
/**
    - returns a image object, which is a copy of this image object
*/
begin_method(SS_IMAGE, ssImageClone, 0)
return_object(JSVAL_NULL);
end_method()
///////////////////////////////////////
///////////////////////////////////////
// SURFACE OBJECTS ////////////////////
///////////////////////////////////////

JSObject*
CScript::CreateSurfaceObject(JSContext* cx, CImage32* surface)
{

    // define surface class
    static JSClass clasp =
        {
            "surface", JSCLASS_HAS_PRIVATE,
            JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
            JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, ssFinalizeSurface,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        };

    // create the object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);
    if (!object || !JS_AddRoot(cx, &object))
        return NULL;


    // assign the methods to the object
    static JSFunctionSpec fs[] =
        {
            { "applyColorFX",     ssSurfaceApplyColorFX,     5, 0, 0 },
            { "applyColorFX4",    ssSurfaceApplyColorFX4,    8, 0, 0 },
            { "blit",             ssSurfaceBlit,             2, 0, 0 },

            { "blitSurface",      ssSurfaceBlitSurface,      3, 0, 0 },
            { "blitMaskSurface",  ssSurfaceBlitMaskSurface,  4, 0, 0 },
            { "rotateBlitSurface",        ssSurfaceRotateBlitSurface,         4, 0, 0 },
            { "rotateBlitMaskSurface",    ssSurfaceRotateBlitMaskSurface,     5, 0, 0 },
            { "zoomBlitSurface",          ssSurfaceZoomBlitSurface,           4, 0, 0 },
            { "zoomBlitMaskSurface",      ssSurfaceZoomBlitMaskSurface,       5, 0, 0 },
            { "transformBlitSurface",     ssSurfaceTransformBlitSurface,      9, 0, 0 },
            { "transformBlitMaskSurface", ssSurfaceTransformBlitMaskSurface, 10, 0, 0 },
        //  { "blitImage",        ssSurfaceBlitImage,        3, 0, 0 },

            { "createImage",      ssSurfaceCreateImage,      0, 0, 0 },
            { "setBlendMode",     ssSurfaceSetBlendMode,     1, 0, 0 },
            { "getPixel",         ssSurfaceGetPixel,         2, 0, 0 },
            { "setPixel",         ssSurfaceSetPixel,         3, 0, 0 },
            { "setAlpha",         ssSurfaceSetAlpha,         1, 0, 0 },
            { "replaceColor",     ssSurfaceReplaceColor,     2, 0, 0 },
            { "findColor",        ssSurfaceFindColor,        1, 0, 0 },
            { "floodFill",        ssSurfaceFloodFill,        3, 0, 0 },

            { "pointSeries",       ssSurfacePointSeries,       2, 0, 0 },
            { "line",              ssSurfaceLine,              5, 0, 0 },
            { "gradientLine",      ssSurfaceGradientLine,      6, 0, 0 },
            { "lineSeries",        ssSurfaceLineSeries,        2, 0, 0 },
            { "bezierCurve",       ssSurfaceBezierCurve,       8, 0, 0 },
            { "outlinedRectangle", ssSurfaceOutlinedRectangle, 5, 0, 0 },
            { "rectangle",         ssSurfaceRectangle,         5, 0, 0 },
            { "gradientRectangle", ssSurfaceGradientRectangle, 8, 0, 0 },
            { "triangle",          ssSurfaceTriangle,          7, 0, 0 },
            { "gradientTriangle",  ssSurfaceGradientTriangle,  9, 0, 0 },
            { "polygon",           ssSurfacePolygon,           2, 0, 0 },
            { "outlinedEllipse",   ssSurfaceOutlinedEllipse,   5, 0, 0 },
            { "filledEllipse",     ssSurfaceFilledEllipse,     5, 0, 0 },
            { "outlinedCircle",    ssSurfaceOutlinedCircle,    4, 0, 0 },
            { "filledCircle",      ssSurfaceFilledCircle,      4, 0, 0 },
            { "gradientCircle",    ssSurfaceGradientCircle,    5, 0, 0 },

            { "rotate",           ssSurfaceRotate,           2, 0, 0 },
            { "resize",           ssSurfaceResize,           2, 0, 0 },
            { "rescale",          ssSurfaceRescale,          2, 0, 0 },
            { "flipHorizontally", ssSurfaceFlipHorizontally, 0, 0, 0 },
            { "flipVertically",   ssSurfaceFlipVertically,   0, 0, 0 },
            { "clone",            ssSurfaceClone,            0, 0, 0 },
            { "cloneSection",     ssSurfaceCloneSection,     4, 0, 0 },

            { "drawText",         ssSurfaceDrawText,         4, 0, 0 },
            { "drawZoomedText",   ssSurfaceDrawZoomedText,   5, 0, 0 },
            { "drawTextBox",      ssSurfaceDrawTextBox,      7, 0, 0 },
            { "applyLookup",      ssSurfaceApplyLookup,      7, 0, 0 },
            { "save",             ssSurfaceSave,             1, 0, 0 },
            { 0, 0, 0, 0, 0 },
        };
    JS_DefineFunctions(cx, object, fs);

    // define width and height properties
    JS_DefineProperty(cx, object, "width",  INT_TO_JSVAL(surface->GetWidth()),  JS_PropertyStub, JS_PropertyStub, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineProperty(cx, object, "height", INT_TO_JSVAL(surface->GetHeight()), JS_PropertyStub, JS_PropertyStub, JSPROP_READONLY | JSPROP_PERMANENT);

    // attach the surface to this object
    SS_SURFACE* surface_object = new SS_SURFACE;

    if (!surface_object)
    {
        JS_RemoveRoot(cx, &object);
        return NULL;
    }
    surface_object->surface = surface;
    JS_SetPrivate(cx, object, surface_object);
    JS_RemoveRoot(cx, &object);
    return object;
}

////////////////////////////////////////
begin_finalizer(SS_SURFACE, ssFinalizeSurface)
if (object->surface)
{

    delete object->surface;
}
object->surface = NULL;
end_finalizer()

////////////////////////////////////////
/**
    - Apply the colormatrix to the pixels contained in x, y, w, h
    @see CreateColorMatrix
*/
begin_method(SS_SURFACE, ssSurfaceApplyColorFX, 5)
arg_int(x);
arg_int(y);
arg_int(w);
arg_int(h);
arg_colormatrix(cm);

object->surface->ApplyColorFX(x, y, w, h, *cm);
end_method()

////////////////////////////////////////
/**
    - Apply 4 color matrixes. Each corner has a seperate color matrix.
    @see CreateColorMatrix
*/
begin_method(SS_SURFACE, ssSurfaceApplyColorFX4, 8)
arg_int(x);
arg_int(y);
arg_int(w);
arg_int(h);
arg_colormatrix(c1);
arg_colormatrix(c2);
arg_colormatrix(c3);
arg_colormatrix(c4);

object->surface->ApplyColorFX4(x, y, w, h, *c1, *c2, *c3, *c4);
end_method()

////////////////////////////////////////
/**
    - draws the surface to the video buffer at (x,y)
*/
begin_method(SS_SURFACE, ssSurfaceBlit, 2)
if (This->ShouldRender())
{

    arg_int(x);
    arg_int(y);

    DirectBlit(
        x,
        y,
        object->surface->GetWidth(),
        object->surface->GetHeight(),
        object->surface->GetPixels()
    );
}
end_method()

////////////////////////////////////////
/**
    - draws 'surface' onto the surface_object at (x, y)
*/
begin_method(SS_SURFACE, ssSurfaceBlitSurface, 3)

arg_surface(surface);
arg_int(x);
arg_int(y);

if (surface)
{
    object->surface->BlitImage(*surface, x, y);
}

end_method()

////////////////////////////////////////
static CImage32::BlendMode
int_to_surface_mask_blendmode(int blendmode)
{
    switch (blendmode)
    {
        case CImage32::BLEND:      return CImage32::BLEND;      break;
        case CImage32::REPLACE:    return CImage32::REPLACE;    break;
        case CImage32::RGB_ONLY:   return CImage32::RGB_ONLY;   break;
        case CImage32::ALPHA_ONLY: return CImage32::ALPHA_ONLY; break;
        case CImage32::ADD:        return CImage32::ADD;        break;
        case CImage32::SUBTRACT:   return CImage32::SUBTRACT;   break;
        case CImage32::MULTIPLY:   return CImage32::MULTIPLY;   break;
        case CImage32::AVERAGE:    return CImage32::AVERAGE;    break;
        case CImage32::INVERT:     return CImage32::INVERT;     break;
        default:                   return CImage32::MULTIPLY;   break;
    }
}

////////////////////////////////////////
/**
    - draws 'surface' onto the surface_object at (x, y), except that the color passed
      as 'mask' tints 'surface'
*/
begin_method(SS_SURFACE, ssSurfaceBlitMaskSurface, 4)

arg_surface(surface);
arg_int(x);
arg_int(y);
arg_color(mask);

int mask_bmode = CImage32::MULTIPLY;
if (argc >= 5)
{
    mask_bmode = int_to_surface_mask_blendmode(argInt(cx, argv[4]));
}

if (surface)
{
    object->surface->BlitImageMask(*surface, x, y, mask, mask_bmode);
}

end_method()

////////////////////////////////////////
/**
    - draws 'surface' onto the surface_object at (x, y), except that 'surface' is rotated
      anti-clockwise in radians, with -2*PI <= angle <= 2*PI
*/
begin_method(SS_SURFACE, ssSurfaceRotateBlitSurface, 4)

arg_surface(surface);
arg_int(x);
arg_int(y);
arg_double(angle);

if (surface)
{
    if (angle == 0)
    {
        object->surface->BlitImage(*surface, x, y);
    }
    else
    {
        int w = surface->GetWidth();
        int h = surface->GetHeight();
        int tx[4];
        int ty[4];

        CalculateRotateBlitPoints(tx, ty, x, y, w, h, angle);

        object->surface->TransformBlitImage(*surface, tx, ty);
    }
}

end_method()

////////////////////////////////////////
/**
    - rotateBlitSurface + blitMaskSurface
*/
begin_method(SS_SURFACE, ssSurfaceRotateBlitMaskSurface, 5)

arg_surface(surface);
arg_int(x);
arg_int(y);
arg_double(angle);
arg_color(mask);

int mask_bmode = CImage32::MULTIPLY;
if (argc >= 6)
{
    mask_bmode = int_to_surface_mask_blendmode(argInt(cx, argv[5]));
}

if (surface)
{
    if (angle == 0)
    {
        object->surface->BlitImageMask(*surface, x, y, mask, mask_bmode);
    }
    else
    {
        int w = surface->GetWidth();
        int h = surface->GetHeight();
        int tx[4];
        int ty[4];

        CalculateRotateBlitPoints(tx, ty, x, y, w, h, angle);

        object->surface->TransformBlitImageMask(*surface, tx, ty, mask, mask_bmode);
    }
}

end_method()

////////////////////////////////////////
/**
    - draws 'surface' onto the surface_object at (x, y) with zooming, with the scaling
      depending on factor. Normally a factor of 1 will draw a normal looking
      'surface' and between 0 and 1 will shrink it. Any values greater than 1
      will stretch it's size.
*/
begin_method(SS_SURFACE, ssSurfaceZoomBlitSurface, 4)

arg_surface(surface);
arg_int(x);
arg_int(y);
arg_double(factor);

if (surface)
{
    int w = surface->GetWidth();
    int h = surface->GetHeight();

    int tx[4] = { x, x + (int)(w * factor), x + (int)(w * factor), x };
    int ty[4] = { y, y, y + (int)(h * factor), y + (int)(h * factor) };

    object->surface->TransformBlitImage(*surface, tx, ty);
}

end_method()

////////////////////////////////////////
/**
    - zoomBlitSurface + blitMaskSurface
*/
begin_method(SS_SURFACE, ssSurfaceZoomBlitMaskSurface, 5)

arg_surface(surface);
arg_int(x);
arg_int(y);
arg_double(factor);
arg_color(mask);

int mask_bmode = CImage32::MULTIPLY;
if (argc >= 6)
{
    mask_bmode = int_to_surface_mask_blendmode(argInt(cx, argv[5]));
}

if (surface)
{
    int w = surface->GetWidth();
    int h = surface->GetHeight();

    int tx[4] = { x, x + (int)(w * factor), x + (int)(w * factor), x };
    int ty[4] = { y, y, y + (int)(h * factor), y + (int)(h * factor) };

    object->surface->TransformBlitImageMask(*surface, tx, ty, mask, mask_bmode);
}

end_method()

////////////////////////////////////////
/**
    - draws 'surface' onto the surface_object with "transformation", where
      (x1, y1) is the upper left corner, (x2, y2) the upper right corner,
      (x3, y3) is the lower right corner, and (x4, y4) is the lower left
      corner.
*/
begin_method(SS_SURFACE, ssSurfaceTransformBlitSurface, 9)

arg_surface(surface);
arg_int(x1);
arg_int(y1);
arg_int(x2);
arg_int(y2);
arg_int(x3);
arg_int(y3);
arg_int(x4);
arg_int(y4);

int x[4] = { x1, x2, x3, x4 };
int y[4] = { y1, y2, y3, y4 };

if (surface)
{
    object->surface->TransformBlitImage(*surface, x, y);
}

end_method()

////////////////////////////////////////
/**
    - transformBlitSurface + blitMaskSurface
*/
begin_method(SS_SURFACE, ssSurfaceTransformBlitMaskSurface, 10)

arg_surface(surface);
arg_int(x1);
arg_int(y1);
arg_int(x2);
arg_int(y2);
arg_int(x3);
arg_int(y3);
arg_int(x4);
arg_int(y4);
arg_color(mask);

int mask_bmode = CImage32::MULTIPLY;
if (argc >= 11)
{
    mask_bmode = int_to_surface_mask_blendmode(argInt(cx, argv[10]));
}

int x[4] = { x1, x2, x3, x4 };
int y[4] = { y1, y2, y3, y4 };

if (surface)
{
    object->surface->TransformBlitImageMask(*surface, x, y, mask, mask_bmode);
}

end_method()

////////////////////////////////////////
#if 0
/**
    - [DISABLED] draws 'image' onto the surface_object at (x, y)
*/
begin_method(SS_SURFACE, ssSurfaceBlitImage, 3)
arg_image(image_object);
arg_int(x);
arg_int(y);
CImage32* surface = NULL;
int width  = GetImageWidth(image_object->image);
int height = GetImageHeight(image_object->image);
RGBA* pixels = LockImage(image_object->image);
if (pixels)
{

    surface = new CImage32(width, height, pixels);
    if (!surface || surface->GetWidth() != width || surface->GetHeight() != height)
    {

        if (surface)
        {
            delete surface;
            surface = NULL;
        }

        JS_ReportError(cx, "createSurface() failed!!");
        return JS_FALSE;
    }
}
UnlockImage(image_object->image, false);
if (surface)
{

    object->surface->BlitImage(*surface, x, y);
}
delete surface;
surface = NULL;
end_method()
#endif
////////////////////////////////////////
/**
    - returns an image object from the surface object
*/
begin_method(SS_SURFACE, ssSurfaceCreateImage, 0)

IMAGE image = CreateImage(
                  object->surface->GetWidth(),
                  object->surface->GetHeight(),
                  object->surface->GetPixels()
              );

if (image == NULL)
{
    JS_ReportError(cx, "Fatal Error: CreateImage() failed");
    return JS_FALSE;
}

return_object(CreateImageObject(cx, image, true));
end_method()

////////////////////////////////////////
/**
    - pass it one of the following constants:
    	- BLEND:		DestColor = (DestColor * (255 - SrcAlpha) / 255) + ((SrcColor * SrcAlpha) / 255)
    	- ADD:			DestColor = DestColor + ((SrcColor * SrcAlpha) / 255)
    	- SUBTRACT:		DestColor = DestColor - ((SrcColor * SrcAlpha) / 255)
    	- MULTIPLY:		DestColor = DestColor * ((SrcColor * SrcAlpha) / 255) / 255
    	- AVERAGE:		DestColor = DestColor + ((SrcColor * SrcAlpha) / 255) / 2
    	- INVERT:		DestColor = DestColor * (255 - ((SrcColor * SrcAlpha) / 255)) / 255
    	- REPLACE:		DestColor = SrcColor
    	- RGB_ONLY:		DestRGB   = SrcRGB
    	- ALPHA_ONLY:	DestAlpha = SrcAlpha
*/
begin_method(SS_SURFACE, ssSurfaceSetBlendMode, 1)
arg_int(mode);

switch (mode)
{
    case 0:
        object->surface->SetBlendMode(CImage32::BLEND);
        break;
    case 1:
        object->surface->SetBlendMode(CImage32::REPLACE);
        break;
    case 2:
        object->surface->SetBlendMode(CImage32::RGB_ONLY);
        break;
    case 3:
        object->surface->SetBlendMode(CImage32::ALPHA_ONLY);
        break;
    case 4:
        object->surface->SetBlendMode(CImage32::ADD);
        break;
    case 5:
        object->surface->SetBlendMode(CImage32::SUBTRACT);
        break;
    case 6:
        object->surface->SetBlendMode(CImage32::MULTIPLY);
        break;
    case 7:
        object->surface->SetBlendMode(CImage32::AVERAGE);
        break;
    case 8:
        object->surface->SetBlendMode(CImage32::INVERT);
        break;
}

end_method()

////////////////////////////////////////
/**
    - returns the color of the pixel at (x,y)
*/
begin_method(SS_SURFACE, ssSurfaceGetPixel, 2)
arg_int(x);
arg_int(y);
CImage32* surface = object->surface;
if (x < 0 || x >= surface->GetWidth() || y < 0 || y >= surface->GetHeight())
{
    char string[520];
    sprintf(string, "Invalid coordinates in surface.getPixel() call\n x: %d y: %d w: %d h: %d", x, y, surface->GetWidth(), surface->GetHeight());
    JS_ReportError(cx, string);
    return JS_FALSE;
}

return_object(CreateColorObject(cx, surface->GetPixel(x, y)));
end_method()

////////////////////////////////////////
/**
    - sets the pixel at (x,y) to 'color'
*/
begin_method(SS_SURFACE, ssSurfaceSetPixel, 3)
arg_int(x);
arg_int(y);
arg_color(c);

CImage32* surface = object->surface;
if (x < 0 || x >= surface->GetWidth() || y < 0 || y >= surface->GetHeight())
{
    char string[520];
    sprintf(string, "Invalid coordinates in surface.setPixel() call\n x: %d y: %d w: %d h: %d", x, y, surface->GetWidth(), surface->GetHeight());
    JS_ReportError(cx, string);
    return JS_FALSE;
}

object->surface->SetPixel(x, y, c);
end_method()

////////////////////////////////////////
/**
    - sets the alpha of the surface
    Set the second parameter to false to skip transparent pixels
*/
begin_method(SS_SURFACE, ssSurfaceSetAlpha, 1)
arg_int(alpha);
bool all = true;
if (argc >= 2) all = argBool(cx, argv[1]);
object->surface->SetAlpha(alpha, all);
end_method()

////////////////////////////////////////
/**
    - replace all pixels of the color oldColor in the surface with newColor
*/
begin_method(SS_SURFACE, ssSurfaceReplaceColor, 2)
arg_color(oldColor);
arg_color(newColor);
object->surface->ReplaceColor(oldColor, newColor);
end_method()

////////////////////////////////////////
/**
    - Tells us if the color aColor is in the surface.
      Returns an object containing x and y elements.
*/
begin_method(SS_SURFACE, ssSurfaceFindColor, 1)
arg_color(aColor);
int x, y;

if (object->surface->FindColor(aColor,x,y))
{
    static JSClass a_clasp =
    {
        "point", 0,
        JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
        JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    };

    JSObject* a_obj = JS_NewObject(cx, &a_clasp, NULL, NULL);
    if (!a_obj || !JS_AddRoot(cx, &a_obj))
        return NULL;

    JS_DefineProperty(cx, a_obj, "x",      INT_TO_JSVAL(x), JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineProperty(cx, a_obj, "y",      INT_TO_JSVAL(y), JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

    return_object(a_obj);
	JS_RemoveRoot(cx, &a_obj);
} 
else 
{
	return_null();
}

end_method()

////////////////////////////////////////
/**
    - Flood Fills the area with a color
*/
begin_method(SS_SURFACE, ssSurfaceFloodFill, 3)
arg_int(x);
arg_int(y);
arg_color(aColor);
if( x < 0 || x > object->surface->GetWidth() || y < 0 || y > object->surface->GetHeight() )
{
    JS_ReportError(cx, "floodFill: point outside surface");
    return JS_FALSE;
}
object->surface->FloodFill(x, y, aColor);
end_method()

////////////////////////////////////////
/**
    - draws a series of points onto the surface
    @see PointSeries
*/
begin_method(SS_SURFACE, ssSurfacePointSeries, 2)

arg_array(arr);
arg_color(c);

jsval  v;
jsval* vp = &v;
jsuint length;

JS_GetArrayLength(cx, arr, &length);

if (length < 1)
{
    JS_ReportError(cx, "pointSeries() failed: Not enough points in array");
    return JS_FALSE;
}

VECTOR_INT** points = new VECTOR_INT*[length];

for (unsigned int i = 0; i < length; i++)
{
    JS_GetElement(cx, arr, i, vp);

    points[i] = getObjCoordinates(cx, v);

    if (points[i] == NULL)
    {
        JS_ReportError(cx, "pointSeries() failed: Invalid object at array index %d", i);
        for (unsigned int i = 0; i < length; i++) delete points[i];
        delete [] points;
        return JS_FALSE;
    }
}

object->surface->PointSeries(points, length, c);
for (unsigned int i = 0; i < length; i++)
    delete points[i];
delete [] points;

end_method()

////////////////////////////////////////
/**
    - draws a line onto the surface starting from (x1, y1) to (x2, y2) with
      the color
*/
begin_method(SS_SURFACE, ssSurfaceLine, 5)
arg_int(x1);
arg_int(y1);
arg_int(x2);
arg_int(y2);
arg_color(c);

object->surface->Line(x1, y1, x2, y2, c);
end_method()

//////
/**
    - draws a gradient line onto the surface starting from (x1, y1) to (x2, y2)
    @see GradientLine
*/
begin_method(SS_SURFACE, ssSurfaceGradientLine, 6)
arg_int(x1);
arg_int(y1);
arg_int(x2);
arg_int(y2);
arg_color(c1);
arg_color(c2);
RGBA c[2];
c[0] = c1;
c[1] = c2;
object->surface->GradientLine(x1, y1, x2, y2, c);
end_method()

////////////////////////////////////////
/**
    - draws a series of lines onto the surface
    @see LineSeries
*/
begin_method(SS_SURFACE, ssSurfaceLineSeries, 2)

arg_array(arr);
arg_color(c);

int type = 0;
if (argc >= 3)
{
    type = argInt(cx, argv[2]);
    if (type < 0)
    {
        type = 0;
    }
    else if (type > 2)
    {
        type = 2;
    }
}

jsval  v;
jsval* vp = &v;
jsuint length;

JS_GetArrayLength(cx, arr, &length);

if (length < 2)
{
    JS_ReportError(cx, "lineSeries() failed: Not enough points in array");
    return JS_FALSE;
}
if (type == 0 && length % 2)
{
    length--;
}
if (type == 2 && length < 3)
{
    type = 0;
}

VECTOR_INT** points = new VECTOR_INT*[length];

for (unsigned int i = 0; i < length; i++)
{
    JS_GetElement(cx, arr, i, vp);

    points[i] = getObjCoordinates(cx, v);

    if (points[i] == NULL)
    {
        JS_ReportError(cx, "lineSeries() failed: Invalid object at array index %d", i);
        for (unsigned int j = 0; j < length; j++) delete points[j];
        delete [] points;
        return JS_FALSE;
    }
}

object->surface->LineSeries(points, length, c, type);
for (unsigned int i = 0; i < length; i++)
    delete points[i];
delete [] points;

end_method()

////////////////////////////////////////
/**
    - draws a Bezier Curve onto the surface
    @see BezierCurve
*/
begin_method(SS_SURFACE, ssSurfaceBezierCurve, 8)
arg_color(c);
arg_double(step);
arg_int(x1);
arg_int(y1);
arg_int(x2);
arg_int(y2);
arg_int(x3);
arg_int(y3);
int x4 = 0;
int y4 = 0;
int cubic = 0;
if (argc >= 10)
{
    x4 = argInt(cx, argv[8]);
    y4 = argInt(cx, argv[9]);
    cubic = 1;
}
int x[4] = { x1, x2, x3, x4 };
int y[4] = { y1, y2, y3, y4 };
object->surface->BezierCurve(x, y, step, c, cubic);
end_method()

////////////////////////////////////////
/**
    - draws an outlined rectangle onto the surface
    @see OutlinedRectangle
*/
begin_method(SS_SURFACE, ssSurfaceOutlinedRectangle, 5)
arg_int(x);
arg_int(y);
arg_int(w);
arg_int(h);
arg_color(c);
int size = 1;
if (argc >= 6)
{
    size = argInt(cx, argv[5]);
    if (size < 0)
    {
        size = 1;
    }
}
if (size > h / 2)
{
    object->surface->Rectangle(x, y, w, h, c);
}
else
{
    object->surface->OutlinedRectangle(x, y, w, h, size, c);
}
end_method()

////////////////////////////////////////
/**
    - draws a filled rectangle onto the surface from (x, y) to (x+w, y+h)
      with 'color'
*/
begin_method(SS_SURFACE, ssSurfaceRectangle, 5)
arg_int(x);

arg_int(y);
arg_int(w);
arg_int(h);
arg_color(c);
object->surface->Rectangle(x, y, w, h, c);
end_method()
////////////////////////////////////////
/**
    - draws a filled triangle with the points (x1, y1), (x2, y2), (x3, y3),
      with 'color'
*/
begin_method(SS_SURFACE, ssSurfaceTriangle, 7)
arg_int(x1);
arg_int(y1);
arg_int(x2);
arg_int(y2);
arg_int(x3);
arg_int(y3);
arg_color(c);

object->surface->Triangle(x1, y1, x2, y2, x3, y3, c);
end_method()

////////////////////////////////////////
/**
    - draws a filled gradient rectangle onto the surface from (x, y) to (x+w, y+h)
    @see GradientRectangle
*/
begin_method(SS_SURFACE, ssSurfaceGradientRectangle, 8)
arg_int(x);
arg_int(y);
arg_int(w);
arg_int(h);
arg_color(c1);
arg_color(c2);
arg_color(c3);
arg_color(c4);
RGBA c[4];
c[0] = c1;
c[1] = c2;
c[2] = c3;
c[3] = c4;
object->surface->GradientRectangle(x, y, w, h, c);
end_method()

////////////////////////////////////////
/**
    - draws a filled gradient triangle with the points (x1, y1), (x2, y2), (x3, y3),
    @see GradientTriangle
*/
begin_method(SS_SURFACE, ssSurfaceGradientTriangle, 9)
arg_int(x1);
arg_int(y1);
arg_int(x2);
arg_int(y2);
arg_int(x3);
arg_int(y3);
arg_color(c1);
arg_color(c2);
arg_color(c3);
RGBA c[3];
c[0] = c1;
c[1] = c2;
c[2] = c3;
object->surface->GradientTriangle(x1, y1, x2, y2, x3, y3, c);
end_method()

////////////////////////////////////////
/**
    - draws a filled polygon onto the surface
    @see Polygon
*/
begin_method(SS_SURFACE, ssSurfacePolygon, 2)

arg_array(arr);
arg_color(c);

int invert = 0;
if (argc >= 3)
{
    invert = argInt(cx, argv[2]);
    if (invert != 0 && invert != 1)
    {
        invert = 0;
    }
}

jsval  v;
jsval* vp = &v;
jsuint length;

JS_GetArrayLength(cx, arr, &length);

if (length < 3)
{
    JS_ReportError(cx, "Polygon() failed: Not enough points in array");
    return JS_FALSE;
}

VECTOR_INT** points = new VECTOR_INT*[length];

for (unsigned int i = 0; i < length; i++)
{
    JS_GetElement(cx, arr, i, vp);

    points[i] = getObjCoordinates(cx, v);

    if (points[i] == NULL)
    {
        JS_ReportError(cx, "Polygon() failed: Invalid object at array index %d", i);
        for (unsigned int i = 0; i < length; i++) delete points[i];
        delete [] points;
        return JS_FALSE;
    }
}

object->surface->Polygon(points, length, invert, c);
for (unsigned int i = 0; i < length; i++)
    delete points[i];
delete [] points;

end_method()

////////////////////////////////////////
/**
    - draws an outlined ellipse onto the surface
    @see OutlinedEllipse
*/
begin_method(SS_SURFACE, ssSurfaceOutlinedEllipse, 5)
arg_int(x);
arg_int(y);
arg_int(rx);
arg_int(ry);
arg_color(c);

object->surface->OutlinedEllipse(x, y, rx, ry, c);
end_method()

////////////////////////////////////////
/**
    - draws a filled ellipse onto the surface
    @see FilledEllipse
*/
begin_method(SS_SURFACE, ssSurfaceFilledEllipse, 5)
arg_int(x);
arg_int(y);
arg_int(rx);
arg_int(ry);
arg_color(c);

object->surface->FilledEllipse(x, y, rx, ry, c);
end_method()

////////////////////////////////////////
/**
    - draws an outlined circle onto the surface
    @see OutlinedCircle
*/
begin_method(SS_SURFACE, ssSurfaceOutlinedCircle, 4)
arg_int(x);
arg_int(y);
arg_int(r);
arg_color(c);

int antialias = 0;
if (argc >= 5)
{
    antialias = argInt(cx, argv[4]);
}

object->surface->OutlinedCircle(x, y, r, c, antialias);
end_method()

////////////////////////////////////////
/**
    - draws a filled circle onto the surface
    @see FilledCircle
*/
begin_method(SS_SURFACE, ssSurfaceFilledCircle, 4)
arg_int(x);
arg_int(y);
arg_int(r);
arg_color(c);

int antialias = 0;
if (argc >= 5)
{
    antialias = argInt(cx, argv[4]);
}

object->surface->FilledCircle(x, y, r, c, antialias);
end_method()

////////////////////////////////////////
/**
    - draws a gradient circle onto the surface
    @see GradientCircle
*/
begin_method(SS_SURFACE, ssSurfaceGradientCircle, 5)
arg_int(x);
arg_int(y);
arg_int(r);
arg_color(c1);
arg_color(c2);

int antialias = 0;
if (argc >= 6)
{
    antialias = argInt(cx, argv[5]);
}
RGBA c[2];
c[0] = c1;
c[1] = c2;

object->surface->GradientCircle(x, y, r, c, antialias);
end_method()

////////////////////////////////////////
/**
    - rotates the surface anti-clockwise with the range 0 - 2*pi. The resize
      flag is a boolean to tell the engine to resize the surface if needed
      to accomodate the rotated image.
      this function returns the rotated object with the new .width and .height.
*/
begin_method(SS_SURFACE, ssSurfaceRotate, 2)
arg_double(radians);
arg_bool(autosize);

object->surface->Rotate(radians, autosize);
if (autosize)
{
    // redefine width and height properties
    JS_DefineProperty(cx, obj, "width",  INT_TO_JSVAL(object->surface->GetWidth()),  JS_PropertyStub, JS_PropertyStub, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineProperty(cx, obj, "height", INT_TO_JSVAL(object->surface->GetHeight()), JS_PropertyStub, JS_PropertyStub, JSPROP_READONLY | JSPROP_PERMANENT);
}

return_object(obj);
end_method()
////////////////////////////////////////
/**
    - resizes the surface images. This does not stretch or shrink the image
      inside the surface.
      this function returns the resized object with the new .width and .height.
*/
begin_method(SS_SURFACE, ssSurfaceResize, 2)
arg_int(w);
arg_int(h);

if (w < 0 || h < 0)
{
    JS_ReportError(cx, "surface_object.resize() failed\nSurfaces cannot have negative sizes...\n'%d by %d'", w, h);
    return JS_FALSE;
}

if (w != object->surface->GetWidth() || h != object->surface->GetHeight())
{
    object->surface->Resize(w, h);
    // redefine width and height properties
    JS_DefineProperty(cx, obj, "width",  INT_TO_JSVAL(object->surface->GetWidth()),  JS_PropertyStub, JS_PropertyStub, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineProperty(cx, obj, "height", INT_TO_JSVAL(object->surface->GetHeight()), JS_PropertyStub, JS_PropertyStub, JSPROP_READONLY | JSPROP_PERMANENT);
}

return_object(obj);
end_method()
////////////////////////////////////////
/**
    - stretches or shrinks the surface to the new width w and height h
      this function returns the rescaled object with the new .width and .height.
*/
begin_method(SS_SURFACE, ssSurfaceRescale, 2)
arg_int(w);
arg_int(h);

if (w < 0 || h < 0)
{
    JS_ReportError(cx, "surface_object.rescale() failed\nSurfaces cannot have negative sizes...\n'%d by %d'", w, h);
    return JS_FALSE;
}

if (w != object->surface->GetWidth() || h != object->surface->GetHeight())
{
    object->surface->Rescale(w, h);
    // redefine width and height properties
    JS_DefineProperty(cx, obj, "width",  INT_TO_JSVAL(object->surface->GetWidth()),  JS_PropertyStub, JS_PropertyStub, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineProperty(cx, obj, "height", INT_TO_JSVAL(object->surface->GetHeight()), JS_PropertyStub, JS_PropertyStub, JSPROP_READONLY | JSPROP_PERMANENT);
}

return_object(obj);
end_method();

////////////////////////////////////////
/**
    - flips the surface horizontally
*/
begin_method(SS_SURFACE, ssSurfaceFlipHorizontally, 0)
object->surface->FlipHorizontal();
end_method()

////////////////////////////////////////
/**
    - flips the surface vertically
*/
begin_method(SS_SURFACE, ssSurfaceFlipVertically, 0)
object->surface->FlipVertical();
end_method()

////////////////////////////////////////
/**
    - returns a surface object, which is a copy of this surface object
*/
begin_method(SS_SURFACE, ssSurfaceClone, 0)
// create the surface
CImage32* surface = new CImage32(*object->surface);

if (surface)
{

    return_object(CreateSurfaceObject(cx, surface));
}
else
{

    return_object(JSVAL_NULL);
}
end_method()

///////////////////////////////////////
/**
    - returns a new surface object with the height and width of h and w, with
      part of image at (x,y) from the surface_object with the width w and
      height h.
*/
begin_method(SS_SURFACE, ssSurfaceCloneSection, 4)
arg_int(x);
arg_int(y);
arg_int(w);
arg_int(h);

if (x < 0 || y < 0 || x > object->surface->GetWidth() || y > object->surface->GetHeight())
{

    JS_ReportError(cx, "surface_object.cloneSection(%d, %d, %d, %d) failed\nInvalid x, y...", x, y, w, h);
    return JS_FALSE;
}
if (w < 0 || h < 0 || x + w > object->surface->GetWidth() || y + h > object->surface->GetHeight())
{

    JS_ReportError(cx, "surface_object.cloneSection(%d, %d, %d, %d) failed\nInvalid w, h...\n", x, y, w, h);
    return JS_FALSE;
}
// create surface object
CImage32* surface = new CImage32(w, h);

if (surface)
{
    for (int iy = 0; iy < h; iy++)
    {
        memcpy(
            surface->GetPixels() + iy * w,
            object->surface->GetPixels() + (iy + y) * object->surface->GetWidth() + x,
            w * sizeof(RGBA));
    }

}
return_object(CreateSurfaceObject(cx, surface));
end_method()

///////////////////////////////////////
typedef uint32 jsuint;
/*
  returns whether the lookup table created is a null lookup table
  i.e. it can be skipped over
*/
bool GetLookUpTable(JSContext* cx, JSObject* array, unsigned char lookup[256])

{
    unsigned int i;
    jsuint length;

    // initialize the lookup to a null-lookup
    for (i = 0; i < 256; i++)
    {
        lookup[i] = i;
    }

    if (array == NULL)
        return true;

    JS_GetArrayLength(cx, array, &length);
    if (length > 256)
        length = 256;

    bool is_null_lookup = true;
    for (i = 0; i < length; i++)
    {
        jsval val;
        int32 value;

        if (JS_LookupElement(cx, array, i, &val))
        {
            if (JSVAL_IS_INT(val) && JS_ValueToInt32(cx, val, &value))
            {

                lookup[i] = (unsigned char) value;
                if (is_null_lookup && lookup[i] != i)
                {
                    is_null_lookup = false;
                }
            }
        }
    }

    return is_null_lookup;
}

///////////////////////////////////////
/**
    The lookup parameters are arrays of 256 elements containg the new pixel values.
    e.g. var invert_lookup = [255, 254, 253, 252, 251, ..., 4, 3, 2, 1, 0];
*/
begin_method(SS_SURFACE, ssSurfaceApplyLookup, 8)

arg_int(x);
arg_int(y);
arg_int(w);
arg_int(h);

if (x < 0)
{
    JS_ReportError(cx, "Invalid x: %d", x);
    return JS_FALSE;
}
if (y < 0)
{
    JS_ReportError(cx, "Invalid y: %d", y);
    return JS_FALSE;
}

if (x + w > object->surface->GetWidth())
{
    JS_ReportError(cx, "Invalid width: %d", w);
    return JS_FALSE;
}

if (y + h > object->surface->GetHeight())
{
    JS_ReportError(cx, "Invalid height: %d", h);
    return JS_FALSE;
}

arg_array(rlookup);
arg_array(glookup);
arg_array(blookup);
arg_array(alookup);

unsigned char rlut[256];
unsigned char glut[256];
unsigned char blut[256];
unsigned char alut[256];

GetLookUpTable(cx, rlookup, rlut);
GetLookUpTable(cx, glookup, glut);
GetLookUpTable(cx, blookup, blut);
GetLookUpTable(cx, alookup, alut);

object->surface->ApplyLookup(x, y, w, h, rlut, glut, blut, alut);
end_method()
///////////////////////////////////////
/**
    - draws 'text' at x, y with the font onto the surface_object
*/
begin_method(SS_SURFACE, ssSurfaceDrawText, 4)
arg_font(font_obj);
arg_int(x);
arg_int(y);
arg_str(text);

font_obj->font->DrawString(x, y, text, font_obj->mask, object->surface);
end_method()
///////////////////////////////////////
/**
    - draws scaled text (1.0 = normal) with (x,y) as the upper left corner
      onto the surface_object
*/
begin_method(SS_SURFACE, ssSurfaceDrawZoomedText, 5)
arg_font(font_obj);
arg_int(x);
arg_int(y);
arg_double(scale);
arg_str(text);

font_obj->font->DrawZoomedString(x, y, scale, text, font_obj->mask, object->surface);
end_method()
///////////////////////////////////////
/**
    - draws a word-wrapped text at (x, y) onto the surface_object with the width w and height h.
      The offset is the number of pixels which the number of pixels from y which
      the actual drawing starts at.
      See font_object.drawTextBox for more detail.
*/
begin_method(SS_SURFACE, ssSurfaceDrawTextBox, 7)
arg_font(font_obj);
arg_int(x);
arg_int(y);
arg_int(w);
arg_int(h);
arg_int(offset);
arg_str(text);

font_obj->font->DrawTextBox(x, y, w, h, offset, text, font_obj->mask, object->surface);
end_method()
///////////////////////////////////////
/**
    - saves the surface_object as a png image using the filename 'filename'
*/
begin_method(SS_SURFACE, ssSurfaceSave, 1)
arg_str(filename);
const char* type = "png";
bool saved = false;

if (IsValidPath(filename) == false)
{
    JS_ReportError(cx, "Invalid path: '%s'", filename);
    return JS_FALSE;
}

if (argc >= 2)
{
    //  type = arg Str(cx, argv[1]);
}

std::string path = "images/";
path += filename;

if (strcmp(type, "png") == 0)
{
    saved = object->surface->Save(path.c_str());
}
// delete type; // Does const char* need delete?
return_bool( saved );
end_method()

///////////////////////////////////////
///////////////////////////////////////
// SURFACE OBJECTS ////////////////////
///////////////////////////////////////

JSObject*
CScript::CreateColorMatrixObject(JSContext* cx, CColorMatrix* colormatrix)
{
    // define surface class
    static JSClass clasp =
        {
            "colormatrix", JSCLASS_HAS_PRIVATE,
            JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
            JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, ssFinalizeColorMatrix,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        };

    // create the object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);
    if (!object || !JS_AddRoot(cx, &object))
        return NULL;

    // assign the methods to the object
    static JSFunctionSpec fs[] =
        {
            //{ "set",              ssColorMatrixSet,         12, 0, 0 },
            { "toJSON",        ssColorMatrixToJSON,    0, },
            { 0, 0, 0, 0, 0 },
        };
    JS_DefineFunctions(cx, object, fs);

    // assign properties
    static JSPropertySpec ps[] =
        {
            { "rn",  0, JSPROP_PERMANENT, ssColorMatrixGetProperty, ssColorMatrixSetProperty },
            { "rr",  1, JSPROP_PERMANENT, ssColorMatrixGetProperty, ssColorMatrixSetProperty },
            { "rg",  2, JSPROP_PERMANENT, ssColorMatrixGetProperty, ssColorMatrixSetProperty },
            { "rb",  3, JSPROP_PERMANENT, ssColorMatrixGetProperty, ssColorMatrixSetProperty },
            { "gn",  4, JSPROP_PERMANENT, ssColorMatrixGetProperty, ssColorMatrixSetProperty },
            { "gr",  5, JSPROP_PERMANENT, ssColorMatrixGetProperty, ssColorMatrixSetProperty },
            { "gg",  6, JSPROP_PERMANENT, ssColorMatrixGetProperty, ssColorMatrixSetProperty },
            { "gb",  7, JSPROP_PERMANENT, ssColorMatrixGetProperty, ssColorMatrixSetProperty },
            { "bn",  8, JSPROP_PERMANENT, ssColorMatrixGetProperty, ssColorMatrixSetProperty },
            { "br",  9, JSPROP_PERMANENT, ssColorMatrixGetProperty, ssColorMatrixSetProperty },
            { "bg", 10, JSPROP_PERMANENT, ssColorMatrixGetProperty, ssColorMatrixSetProperty },
            { "bb", 11, JSPROP_PERMANENT, ssColorMatrixGetProperty, ssColorMatrixSetProperty },
            { 0, 0, 0, 0, 0 },
        };
    JS_DefineProperties(cx, object, ps);

    // attach the colormatrix to this object
    SS_COLORMATRIX* colormatrix_object = new SS_COLORMATRIX;

    if (!colormatrix_object)
    {
        JS_RemoveRoot(cx, &object);
        return NULL;
    }
    colormatrix_object->colormatrix = colormatrix;
    JS_SetPrivate(cx, object, colormatrix_object);
    JS_RemoveRoot(cx, &object);
    return object;
}

////////////////////////////////////////
begin_property(SS_COLORMATRIX, ssColorMatrixGetProperty)
int prop_id = argInt(cx, id);
switch (prop_id)
{
case  0: *vp = INT_TO_JSVAL(object->colormatrix->rn); break;
case  1: *vp = INT_TO_JSVAL(object->colormatrix->rr); break;
case  2: *vp = INT_TO_JSVAL(object->colormatrix->rg); break;
case  3: *vp = INT_TO_JSVAL(object->colormatrix->rb); break;
case  4: *vp = INT_TO_JSVAL(object->colormatrix->gn); break;
case  5: *vp = INT_TO_JSVAL(object->colormatrix->gr); break;
case  6: *vp = INT_TO_JSVAL(object->colormatrix->gg); break;
case  7: *vp = INT_TO_JSVAL(object->colormatrix->gb); break;
case  8: *vp = INT_TO_JSVAL(object->colormatrix->bn); break;
case  9: *vp = INT_TO_JSVAL(object->colormatrix->br); break;
case 10: *vp = INT_TO_JSVAL(object->colormatrix->bg); break;
case 11: *vp = INT_TO_JSVAL(object->colormatrix->bb); break;
default:
    *vp = JSVAL_NULL;
    break;
}
end_property()

////////////////////////////////////////
begin_property(SS_COLORMATRIX, ssColorMatrixSetProperty)
int prop_id = argInt(cx, id);
switch (prop_id)
{
case  0: object->colormatrix->rn = argInt(cx, *vp); break;
case  1: object->colormatrix->rr = argInt(cx, *vp); break;
case  2: object->colormatrix->rg = argInt(cx, *vp); break;
case  3: object->colormatrix->rb = argInt(cx, *vp); break;
case  4: object->colormatrix->gn = argInt(cx, *vp); break;
case  5: object->colormatrix->gr = argInt(cx, *vp); break;
case  6: object->colormatrix->gg = argInt(cx, *vp); break;
case  7: object->colormatrix->gb = argInt(cx, *vp); break;
case  8: object->colormatrix->bn = argInt(cx, *vp); break;
case  9: object->colormatrix->br = argInt(cx, *vp); break;
case 10: object->colormatrix->bg = argInt(cx, *vp); break;
case 11: object->colormatrix->bb = argInt(cx, *vp); break;
}
end_property()


/**
    - exports the ColorMatrix to a string, used internally by JSON
      This string can be eval()'ed to recreate the colormatrix
*/
begin_method(SS_COLORMATRIX, ssColorMatrixToJSON, 0)
char json2[512];
sprintf(json2,
    "CreateColorMatrix(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
    object->colormatrix->rn,
	object->colormatrix->rr,
	object->colormatrix->rg,
	object->colormatrix->rb,
	object->colormatrix->gn,
	object->colormatrix->gr,
	object->colormatrix->gg,
	object->colormatrix->gb,
	object->colormatrix->bn,
	object->colormatrix->br,
	object->colormatrix->bg,
	object->colormatrix->bb
);
return_str(json2);
end_method()

////////////////////////////////////////
begin_finalizer(SS_COLORMATRIX, ssFinalizeColorMatrix)
if (object->colormatrix)
{

    delete object->colormatrix;
}
object->colormatrix = NULL;

end_finalizer()
////////////////////////////////////////
///////////////////////////////////////
// ANIMATION OBJECTS //////////////////
///////////////////////////////////////

JSObject*
CScript::CreateAnimationObject(JSContext* cx, IAnimation* animation)
{
    // define animation class
    static JSClass clasp =
        {
            "animation", JSCLASS_HAS_PRIVATE,
            JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
            JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, ssFinalizeAnimation,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        };

    // create the object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);
    if (object == NULL)
    {
        return NULL;
    }

    // assign the methods to the object
    static JSFunctionSpec fs[] =
        {
            { "getNumFrames",    ssAnimationGetNumFrames,    0, 0, 0 },
            { "getDelay",        ssAnimationGetDelay,        0, 0, 0 },
            { "getTicks",        ssAnimationGetTicks,        0, 0, 0 },
            { "getPlaytime",     ssAnimationGetPlaytime,     0, 0, 0 },
            { "readNextFrame",   ssAnimationReadNextFrame,   0, 0, 0 },
            { "drawFrame",       ssAnimationDrawFrame,       2, 0, 0 },
            { "drawZoomedFrame", ssAnimationDrawZoomedFrame, 3, 0, 0 },
            { 0, 0, 0, 0, 0 },
        };
    JS_DefineFunctions(cx, object, fs);

    // define width and height properties
    JS_DefineProperty(cx, object, "width",  INT_TO_JSVAL(animation->GetWidth()),  JS_PropertyStub, JS_PropertyStub, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineProperty(cx, object, "height", INT_TO_JSVAL(animation->GetHeight()), JS_PropertyStub, JS_PropertyStub, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineProperty(cx, object, "done",   BOOLEAN_TO_JSVAL(animation->IsEndOfAnimation()), JS_PropertyStub, JS_PropertyStub, JSPROP_READONLY | JSPROP_PERMANENT);
    // attach the animation to this object
    SS_ANIMATION* animation_object = new SS_ANIMATION;

    if (!animation_object)
        return NULL;
    animation_object->animation = animation;
    animation_object->frame = new RGBA[animation->GetWidth() * animation->GetHeight()];

    if (!animation_object->frame)
    {

        delete animation_object;
        return NULL;
    }
    animation->ReadNextFrame(animation_object->frame);
    JS_SetPrivate(cx, object, animation_object);

    return object;
}

////////////////////////////////////////
begin_finalizer(SS_ANIMATION, ssFinalizeAnimation)
if (object->animation)
{

    delete object->animation;
}
object->animation = NULL;
if (object->frame)
{

    delete[] object->frame;
}
object->frame = NULL;
end_finalizer()

////////////////////////////////////////
/**
    - returns the number of frames the animation contains.
*/
begin_method(SS_ANIMATION, ssAnimationGetNumFrames, 0)
return_int(object->animation->GetNumFrames());
end_method()

////////////////////////////////////////
/**
    - returns the amount of ticks
*/
begin_method(SS_ANIMATION, ssAnimationGetTicks, 0)
return_int(object->animation->GetTicks());
end_method()

////////////////////////////////////////
/**
    - returns the total playtime, if defined.
      (FLC's dont have this property, MNG's do)
*/
begin_method(SS_ANIMATION, ssAnimationGetPlaytime, 0)
return_int(object->animation->GetPlaytime());
end_method()

////////////////////////////////////////
/**
    - returns the delay between frames, in milliseconds
*/
begin_method(SS_ANIMATION, ssAnimationGetDelay, 0)
return_int(object->animation->GetDelay());
end_method()

////////////////////////////////////////
/**
    - readies the next frame for drawing
*/
begin_method(SS_ANIMATION, ssAnimationReadNextFrame, 0)
object->animation->ReadNextFrame(object->frame);
end_method()

////////////////////////////////////////
/**
    - draws the current frame into the video buffer
*/
begin_method(SS_ANIMATION, ssAnimationDrawFrame, 2)
arg_int(x);
arg_int(y);
if (This->ShouldRender())
{
    DirectBlit(x, y, object->animation->GetWidth(), object->animation->GetHeight(), object->frame);
}
end_method()

///////////////////////////////////////
/**
    - draws the current frame into the video buffer with a specified
      zoom scale
*/
begin_method(SS_ANIMATION, ssAnimationDrawZoomedFrame, 3)
arg_int(x);
arg_int(y);
arg_double(factor);
if (This->ShouldRender())
{
    int w = object->animation->GetWidth();
    int h = object->animation->GetHeight();

    int tx[4] = { x, x + (int)(w * factor), x + (int)(w * factor), x };
    int ty[4] = { y, y, y + (int)(h * factor), y + (int)(h * factor) };

    DirectTransformBlit(tx, ty, w, h, object->frame);
}
end_method()

///////////////////////////////////////
///////////////////////////////////////
// FILE OBJECTS ///////////////////////
///////////////////////////////////////

JSObject*
CScript::CreateFileObject(JSContext* cx, CConfigFile* file)
{
    // define file class
    static JSClass clasp =
        {
            "file", JSCLASS_HAS_PRIVATE,
            JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
            JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, ssFinalizeFile,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        };

    // create the object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);
    if (object == NULL)
    {
        return NULL;
    }

    // assign the methods to the object
    static JSFunctionSpec fs[] =
        {
            { "write",      ssFileWrite,        2, 0, 0 },
            { "read",       ssFileRead,         2, 0, 0 },
            { "flush",      ssFileFlush,        0, 0, 0 },
            { "close",      ssFileClose,        0, 0, 0 },
            { "getNumKeys", ssFileGetNumKeys,   0, 0, 0 },
            { "getKey",     ssFileGetKey,       1, 0, 0 },
			{ "removeKey",  ssFileRemoveKey,    1, 0, 0 },
            { 0, 0, 0, 0, 0 },
        };
    JS_DefineFunctions(cx, object, fs);

    // attach the file to this object
    SS_FILE* file_object = new SS_FILE;

    if (!file_object)
        return NULL;
    file_object->file = file;
    JS_SetPrivate(cx, object, file_object);

    return object;
}

////////////////////////////////////////
begin_finalizer(SS_FILE, ssFinalizeFile)
This->m_Engine->CloseFile(object->file);

object->file = NULL;
end_finalizer()

////////////////////////////////////////
/**
    - writes a value (string, number, boolean) to the file under the key name
*/
begin_method(SS_FILE, ssFileWrite, 2)
arg_str(key);

if (JSVAL_IS_INT(argv[1]))
{
    object->file->WriteInt("", key, JSVAL_TO_INT(argv[1]));
}
else if (JSVAL_IS_BOOLEAN(argv[1]))
{
    object->file->WriteBool("", key, (JSVAL_TO_BOOLEAN(argv[1]) == JS_TRUE));
}
else if (JSVAL_IS_DOUBLE(argv[1]))
{
    double* d = JSVAL_TO_DOUBLE(argv[1]);
    object->file->WriteDouble("", key, *d);
}
else
{ // anything else is a string
    object->file->WriteString("", key, argStr(cx, argv[1]));
}
end_method()

////////////////////////////////////////
/**
    - reads a value from the key
      the value type returned depends on the default value.
      + if the default is a number, read will return a number.
      + if the default is a text or string, read will return a string.
      + if the default is a boolean, read will return a boolean
      + if the key is not present in the file, it will return the default value.
*/
begin_method(SS_FILE, ssFileRead, 2)
arg_str(key);

if (JSVAL_IS_INT(argv[1]))
{
    int i = object->file->ReadInt("", key, JSVAL_TO_INT(argv[1]));
    return_int(i);
}
else if (JSVAL_IS_BOOLEAN(argv[1]))
{
    bool b = object->file->ReadBool("", key, (JSVAL_TO_BOOLEAN(argv[1]) == JS_TRUE));
    return_bool(b);
}
else if (JSVAL_IS_DOUBLE(argv[1]))
{
    double* def = JSVAL_TO_DOUBLE(argv[1]);
    double d = object->file->ReadDouble("", key, *def);
    return_double(d);
}
else
{ // anything else is a string
    std::string str = object->file->ReadString("", key, argStr(cx, argv[1]));
    return_str(str.c_str());
}
end_method()

////////////////////////////////////////
/**
    - returns the number of keys in the file
*/
begin_method(SS_FILE, ssFileGetNumKeys, 0)
int i = object->file->GetNumKeys(-1);
return_int(i);
end_method()
////////////////////////////////////////
/**
    - returns a string of the key at 'index'
*/
begin_method(SS_FILE, ssFileGetKey, 1)
arg_int(index);
if (index < 0)
{

    JS_ReportError(cx, "Index must be greater than zero... %d", index);
    return JS_FALSE;
}
std::string str = object->file->GetKey(-1, index);
return_str(str.c_str());
end_method()
///////////////////////////////////////
/**
    - Removes the key at 'index' (returns false if failed)
*/
begin_method(SS_FILE, ssFileRemoveKey, 1)
arg_int(index);
if (index < 0)
{
    JS_ReportError(cx, "Index must be greater than zero... %d", index);
    return JS_FALSE;
}
bool b = object->file->RemoveKey(-1,index);
return_bool(b);
end_method()
///////////////////////////////////////
/**
    - writes the file to disk immediately...  this way you don't have
      to wait for it to save when the file object is garbage collected
*/
begin_method(SS_FILE, ssFileFlush, 0)
This->m_Engine->FlushFile(object->file);
end_method()

///////////////////////////////////////
/**
    - closes the file object, after this, the file_object cannot be used anymore.
*/
begin_method(SS_FILE, ssFileClose, 0)
if (object->file)
{
    This->m_Engine->CloseFile(object->file);
}
end_method()

///////////////////////////////////////
///////////////////////////////////////
// RAW FILE OBJECTS ///////////////////
///////////////////////////////////////

JSObject*
CScript::CreateRawFileObject(JSContext* cx, IFile* file, bool writeable)
{
    // define raw file class
    static JSClass clasp =
        {
            "rawfile", JSCLASS_HAS_PRIVATE,
            JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
            JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, ssFinalizeRawFile,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        };

    // create the object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);
    if (object == NULL)
    {
        return NULL;
    }

    // add the methods into the object
    static JSFunctionSpec fs[] =
        {
            { "setPosition", ssRawFileSetPosition, 1, 0, 0 },
            { "getPosition", ssRawFileGetPosition, 0, 0, 0 },
            { "getSize",     ssRawFileGetSize,     0, 0, 0 },
            { "read",        ssRawFileRead,        1, 0, 0 },
            { "write",       ssRawFileWrite,       1, 0, 0 },
            { "close",       ssRawFileClose,       0, 0, 0 },
            { 0, 0, 0, 0, 0 },
        };
    JS_DefineFunctions(cx, object, fs);

    // attach the file to this object
    SS_RAWFILE* file_object = new SS_RAWFILE;

    if (!file_object)
        return NULL;
    file_object->file = file;
    file_object->is_open = true;

    file_object->is_writeable = writeable;
    JS_SetPrivate(cx, object, file_object);

    return object;
}

////////////////////////////////////////
begin_finalizer(SS_RAWFILE, ssFinalizeRawFile)
if (object->file && object->is_open)
{
    delete object->file;

}
object->file = NULL;

end_finalizer()
////////////////////////////////////////
/**
    - sets the position that the file will be read from
*/
begin_method(SS_RAWFILE, ssRawFileSetPosition, 1)
if (!object->is_open)
{
    JS_ReportError(cx, "rawfile is closed!");
    return JS_FALSE;
}
arg_int(position);
object->file->Seek(position);
end_method()

////////////////////////////////////////
/**
    - returns the current position which the data is read from
*/
begin_method(SS_RAWFILE, ssRawFileGetPosition, 0)
if (!object->is_open)
{
    JS_ReportError(cx, "rawfile is closed!");
    return JS_FALSE;
}
return_int(object->file->Tell());
end_method()

////////////////////////////////////////
/**
    - returns the number of bytes in the file
*/
begin_method(SS_RAWFILE, ssRawFileGetSize, 0)
if (!object->is_open)
{
    JS_ReportError(cx, "rawfile is closed!");
    return JS_FALSE;
}
return_int(object->file->Size());
end_method()

////////////////////////////////////////
/**
    - reads the number of bytes that is specified by num_bytes. It will create
      and return an array of data the rawfile object has read. The array of
      data are numbers representation of each byte (0-255). Note that if the
      number of bytes that will be read, exceeds the filesize from the current
      position, it will only return an array of data of that is
      actually read.  The returned object is a ByteArray, so you can do
      the same things with it as you can with any other ByteArray.
*/
begin_method(SS_RAWFILE, ssRawFileRead, 1)
arg_int(size);

if (!object->is_open)
{
    JS_ReportError(cx, "rawfile is closed!");
    return JS_FALSE;
}

// read the data
byte* data = new byte[size];

if (!data)
    return_object(JSVAL_NULL);
int bytes_read = object->file->Read(data, size);
JSObject* array_object = CreateByteArrayObject(cx, bytes_read, data);
delete[] data;

return_object(array_object);
end_method()

///////////////////////////////////////
/**
    - writes the byte array to the file at the current position
      The file must have been opened as writeable for this to work.
*/
begin_method(SS_RAWFILE, ssRawFileWrite, 1)
arg_byte_array(data);

if (!object->is_open)
{
    JS_ReportError(cx, "rawfile is closed!");
    return JS_FALSE;
}

if (!object->is_writeable)
{

    JS_ReportError(cx, "rawfile is not writeable!");
    return JS_FALSE;
}
int wrote = object->file->Write(data->array, data->size);
if (wrote < data->size)
{ // error!
    JS_ReportError(cx, "rawfile.write() failed miserably!");
    return JS_FALSE;
}

end_method()
///////////////////////////////////////
/**
    - closes the rawfile object, after this, the rawfile_object cannot be used anymore.
*/
begin_method(SS_RAWFILE, ssRawFileClose, 0)
if (object->file && object->is_open)
{
    delete object->file;
}
object->is_open = false;
end_method()

///////////////////////////////////////
// BYTE_ARRAY OBJECTS /////////////////
///////////////////////////////////////

JSObject*
CScript::CreateByteArrayObject(JSContext* cx, int size, const void* data)
{
    // define file class
    static JSClass clasp =
        {
            "byte_array", JSCLASS_HAS_PRIVATE,
            JS_PropertyStub, JS_PropertyStub, ssByteArrayGetProperty, ssByteArraySetProperty,
            JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, ssFinalizeByteArray,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        };

    // create the object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);
    if (object == NULL)
    {
        return NULL;
    }

    // add the methods into the object
    static JSFunctionSpec fs[] =
        {
            { "concat",       ssByteArrayConcat,    1, 0, 0 },
            { "slice",        ssByteArraySlice,     1, 0, 0 },
            { 0, 0, 0, 0, 0 },
        };
    JS_DefineFunctions(cx, object, fs);

    // give the object a "length" property
    JS_DefineProperty(cx, object, "length", INT_TO_JSVAL(size), JS_PropertyStub, JS_PropertyStub, JSPROP_READONLY | JSPROP_PERMANENT);

    // attach the file to this object
    SS_BYTEARRAY* array_object = new SS_BYTEARRAY;

    if (!array_object)
        return NULL;
    array_object->size = size;
    array_object->array = new byte[size];

    if (!array_object->array)
    {

        delete array_object;
        return NULL;
    }
    if (data)
    {
        memcpy(array_object->array, data, size);
    }
    else
    {
        memset(array_object->array, 0, size);
    }

    JS_SetPrivate(cx, object, array_object);
    return object;
}

///////////////////////////////////////
begin_finalizer(SS_BYTEARRAY, ssFinalizeByteArray)
if (object->array)
{

    delete[] object->array;
}
object->array = NULL;
end_finalizer()

///////////////////////////////////////
/**
    - returns bytearray with bytearray_to_append attached to the end of it
*/
begin_method(SS_BYTEARRAY, ssByteArrayConcat, 1)
arg_byte_array(byte_array_to_append);

int size =  object->size + byte_array_to_append->size;
byte* data = new byte[size];

if (!data)
{

    return_object(JSVAL_NULL);
}
else
{

    memcpy(data, object->array, object->size);
    memcpy(data + object->size, byte_array_to_append->array, byte_array_to_append->size);

    JSObject* concated_byte_array = CreateByteArrayObject(cx, size, data);
    delete[] data;

    return_object(concated_byte_array);
}
end_method()
///////////////////////////////////////
/**
    - returns a slice of the bytearray starting at start, and ending at end
      or the end of the bytearray if end is omitted.
      If end is a negative number, the end point is started from the end of the bytearray.
*/
begin_method(SS_BYTEARRAY, ssByteArraySlice, 1)
arg_int(start_slice);
int end_slice = object->size;

if (argc >= 2)
    end_slice = argInt(cx, argv[1]);

int end_pos = end_slice;
if (end_slice < 0)
{
    end_pos = object->size - 1 - end_slice - 1;
}

if (start_slice < 0 || start_slice >= object->size)
{
    JS_ReportError(cx, "Invalid start position in bytearray.slice call\n%d", start_slice);
    return JS_FALSE;
}

if (end_pos < 0 || end_pos > object->size)
{
    JS_ReportError(cx, "Invalid end position in bytearray.slice call\n%d", end_slice);
    return JS_FALSE;
}

int size = end_pos - start_slice;
if (size < 0 || size > object->size)
{
    JS_ReportError(cx, "Invalid size generated in bytearray.slice call\n%d", size);
    return JS_FALSE;
}

byte* data = new byte[size];
if (!data)
{

    return_object(JSVAL_NULL);
}
else
{
    memcpy(data, object->array + start_slice, size);
    JSObject* byte_array = CreateByteArrayObject(cx, size, data);
    delete[] data;
    return_object(byte_array);

}
end_method()

////////////////////////////////////////
begin_property(SS_BYTEARRAY, ssByteArrayGetProperty)
if (JSVAL_IS_INT(id))
{
    int prop_id = argInt(cx, id);
    if (prop_id < 0 || prop_id >= object->size)
    {
        JS_ReportError(cx, "Byte array access out of bounds (%d)", prop_id);
        return JS_FALSE;
    }

    *vp = INT_TO_JSVAL(object->array[prop_id]);
}
else
{
    const char* prop_id = argStr(cx, id);

    if (strcmp(prop_id, "concat") == 0)
    {
        JSFunction* func = JS_NewFunction(cx, ssByteArrayConcat, 1, 0, NULL, "concat");
        *vp = OBJECT_TO_JSVAL(JS_GetFunctionObject(func));
    }
    else
        if (strcmp(prop_id, "slice") == 0)
        {
            JSFunction* func = JS_NewFunction(cx, ssByteArraySlice, 1, 0, NULL, "slice");
            *vp = OBJECT_TO_JSVAL(JS_GetFunctionObject(func));
        }
}

end_property()
////////////////////////////////////////
begin_property(SS_BYTEARRAY, ssByteArraySetProperty)
int prop_id = argInt(cx, id);
if (prop_id < 0 || prop_id >= object->size)
{
    JS_ReportError(cx, "Byte array access out of bounds (%d)", prop_id);
    return JS_FALSE;
}

object->array[prop_id] = (byte)argInt(cx, *vp);
end_property()

///////////////////////////////////////////////////////////
JSObject*
CScript::CreateTilesetObject(JSContext* cx, const sTileset& tileset)
{
    // define raw file class
    static JSClass clasp =
        {

            "tileset", JSCLASS_HAS_PRIVATE,
            JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
            JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
        };
    // create the object
    JSObject* object = JS_NewObject(cx, &clasp, NULL, NULL);
    if (!object || !JS_AddRoot(cx, &object))
        return NULL;

    // add the methods into the object
    static JSFunctionSpec fs[] =
        {

            { "appendTiles", ssTilesetAppendTiles,  1, 0, 0 },
            { "save",        ssTilesetSave,         1, 0, 0 },
            { 0, 0, 0, 0, 0 },
        };
    JS_DefineFunctions(cx, object, fs);
    // attach the file to this object
    SS_TILESET* tileset_object = new SS_TILESET;
    if (!tileset_object)
    {
        JS_RemoveRoot(cx, &object);
        return NULL;
    }
    tileset_object->__value__ = 0;
    JS_SetPrivate(cx, object, tileset_object);
    JS_RemoveRoot(cx, &object);
    return object;
}
////////////////////////////////////////
/**
    -
*/
begin_method(SS_MAPENGINE, ssMapEngineSave, 1)
arg_str(filename);
std::string path = "maps/" + std::string(filename);

if (IsValidPath(path.c_str()) == false)
{
    JS_ReportError(cx, "Invalid path: '%s'", filename);
    return JS_FALSE;
}

if ( !This->m_Engine->GetMapEngine()->IsRunning() )
{
    This->ReportMapEngineError("map_engine.save() failed");
    return JS_FALSE;
}

return_bool (  This->m_Engine->GetMapEngine()->SaveMap(path.c_str()) );
end_method()

////////////////////////////////////////////////////////////////////////////////
/**
    - Adds a new layer filled with tile index
      The mapengine must be running.
*/
begin_method(SS_MAPENGINE, ssMapEngineLayerAppend, 3)
arg_int(layer_width);
arg_int(layer_height);
arg_int(tile_index);
if ( !This->m_Engine->GetMapEngine()->IsRunning() )
{

    This->ReportMapEngineError("map_engine.appendLayer() failed: MapEngine not running");
    return JS_FALSE;
}
if ( !(layer_width >= 0 && layer_width < 4096
        && layer_height >= 0 && layer_height < 4096) )
{

    This->ReportMapEngineError("map_engine.appendLayer() failed");
    return JS_FALSE;
}
if ( !(tile_index >= 0 && tile_index < This->m_Engine->GetMapEngine()->GetMap().GetMap().GetTileset().GetNumTiles()) )
{

    This->ReportMapEngineError("map_engine.appendLayer() failed");
    return JS_FALSE;
}
sLayer layer;
layer.Resize(layer_width, layer_height);
if (layer.GetWidth() != layer_width
        && layer.GetHeight() != layer_height)
{

    This->ReportMapEngineError("map_engine.appendLayer() failed");
    return JS_FALSE;
}
for (int y = 0; y < layer.GetHeight(); y++)
{

    for (int x = 0; x < layer.GetWidth(); x++)
    {

        layer.SetTile(x, y, tile_index);
    }
}
This->m_Engine->GetMapEngine()->GetMap().GetMap().AppendLayer(layer);
end_method()
////////////////////////////////////////////////////////////////////////////////
/**
    - [INTERNAL] appends num_tiles tiles to the tileset
*/
begin_method(SS_TILESET, ssTilesetAppendTiles, 1)
arg_int(num_tiles);
if ( !This->m_Engine->GetMapEngine()->IsRunning() )
{

    This->ReportMapEngineError("tileset.appendTiles() failed");
    return JS_FALSE;
}
This->m_Engine->GetMapEngine()->GetMap().GetMap().GetTileset().AppendTiles(num_tiles);
end_method()
////////////////////////////////////////////////////////////////////////////////
/**
    - [INTERNAL] saves the tileset from the current map to ./maps/<filename>
*/
begin_method(SS_TILESET, ssTilesetSave, 1)
arg_str(filename);
std::string path = "maps/" + std::string(filename);
if (IsValidPath(path.c_str()) == false)
{

    JS_ReportError(cx, "Invalid path: '%s'", filename);
    return JS_FALSE;
}
if ( !This->m_Engine->GetMapEngine()->IsRunning() )
{

    This->ReportMapEngineError("tileset.save() failed");
    return JS_FALSE;
}
This->m_Engine->GetMapEngine()->GetMap().GetMap().GetTileset().Save(filename);
end_method()

////////////////////////////////////////////////////////////////////////////////

