#ifndef ENGINE_HPP
#define ENGINE_HPP

// identifier too long
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

//#define USE_SPRITESET_SERVER
#include "engineinterface.hpp"
#include "script.hpp"
#include "sphere.hpp"
#include "../common/IFileSystem.hpp"

#ifdef USE_SPRITESET_SERVER
#include "spritesetserver.hpp"
#endif

int pre_process_filename(const char* filename, std::string &path, bool &need_leave);
bool IsMidi(const char* filename);
class CGameEngine : private IEngine
{
public:
    CGameEngine(
        IFileSystem& fs,
        const SSystemObjects& system_objects,
        const std::vector<Game>& game_list,
        const char* script_directory,
        const char* parameters);

    /**
     * Runs the game.  If this returns a value, run a new game and then
     * run this game again (push the new game on a disk).  Otherwise, do
     * nothing.
     */
    std::string Run();

    bool Restarted();
private:
    void ShowError(const char* message);
    void CreateSystemObjects();
    void DestroySystemObjects();

    // IEngine implementation
    virtual bool IsScriptEvaluated(const char* filename);
    virtual bool IsSystemScriptEvaluated(const char* filename);
    virtual void AddEvaluatedScript(const char* filename);
    virtual void AddEvaluatedSystemScript(const char* filename);
    virtual bool GetScriptText(const char* filename, std::string& text);
    virtual bool GetSystemScript(const char* filename, std::string& text);

    virtual void GetGameList(std::vector<Game>& games);
    virtual void ExecuteGame(const char* directory);
    virtual void RestartGame();

    virtual script CompileScript(const char* script, std::string& error);
    virtual bool IsScriptBeingUsed(script s);
    virtual bool ExecuteScript(script s, bool& should_exit, std::string& error);
    virtual void DestroyScript(script s);

    virtual CMapEngine* GetMapEngine();
    virtual CLog* OpenLog(const char* filename);
    virtual void CloseLog(CLog* log);

    virtual SSPRITESET* LoadSpriteset(const char* filename);
    virtual void DestroySpriteset(SSPRITESET* spriteset);
    virtual SFONT* GetSystemFont();
    virtual SFONT* LoadFont(const char* filename);
    virtual void DestroyFont(SFONT* font);

    enum {
        EDGE_LEFT   = 1,
        EDGE_TOP    = 3,
        EDGE_RIGHT  = 5,
        EDGE_BOTTOM = 7
    };

    virtual SWINDOWSTYLE* GetSystemWindowStyle();
    virtual SWINDOWSTYLE* LoadWindowStyle(const char* filename);
    virtual void DestroyWindowStyle(SWINDOWSTYLE* ws);

	virtual audiere::OutputStream* CreateSound(audiere::File* memoryfile, bool streaming);
	virtual audiere::SoundEffect*  CreateSoundEffect(audiere::File* memoryfile, audiere::SoundEffectType type);
	virtual audiere::File* CreateMemoryFile(const void* buffer, int size);

    virtual audiere::OutputStream* LoadSound(const char* filename, bool streaming);
    virtual audiere::SoundEffect*  LoadSoundEffect(const char* filename, audiere::SoundEffectType type);
#if defined(WIN32) && defined(USE_MIDI)
    virtual audiere::MIDIStream* LoadMIDI(const char* filename);
#endif
	virtual SSFXR* CreateSfxr();

    virtual IMAGE GetSystemArrow();
    virtual IMAGE GetSystemUpArrow();
    virtual IMAGE GetSystemDownArrow();
    virtual IMAGE LoadImage(const char* filename);
    virtual void DestroyImage(IMAGE image);

    virtual CImage32* LoadSurface(const char* filename);
    virtual void DestroySurface(CImage32* surface);

    virtual IAnimation* LoadAnimation(const char* filename);

    virtual void GetDirectoryList(const char* directory, std::vector<std::string>& directories);
    virtual void GetFileList(const char* directory, std::vector<std::string>& vs);

    virtual CConfigFile* OpenFile(const char* filename);
    virtual void FlushFile(CConfigFile* file);
    virtual void CloseFile(CConfigFile* file);
    virtual IFile* OpenRawFile(const char* filename, bool writeable);

    virtual bool CreateDirectory(const char* directory);
    virtual bool RemoveDirectory(const char* directory);
    virtual bool RemoveFile(const char* filename);
    virtual bool Rename(const char* old_path, const char* new_path);

private:
    struct SFileInfo
    {
        std::string filename;
        int         refcount;
    };

private:
    IFileSystem& m_FileSystem;
    const SSystemObjects& m_SystemObjects;
    const std::vector<Game>& m_GameList;
    std::string m_SystemScriptDirectory;
    const char* m_Parameters;

    // loaded system objects
    SFONT        m_SystemFont;
    SWINDOWSTYLE m_SystemWindowStyle;
    IMAGE        m_SystemArrow;
    IMAGE        m_SystemUpArrow;
    IMAGE        m_SystemDownArrow;

    CScript* m_Script;
#ifdef USE_SPRITESET_SERVER
    CSpritesetServer m_SpritesetServer;
#endif

    // the next game to be executed
    std::string m_NextGame;

    bool m_RestartGame;
    // opened file list
    std::map<CConfigFile*, SFileInfo> m_OpenFiles;

    std::vector<std::string> m_EvaluatedScripts;
    std::vector<std::string> m_EvaluatedSystemScripts;
    CMapEngine* m_MapEngine;
};

#endif
