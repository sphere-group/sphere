#ifndef VM_INTERFACE_HPP
#define VM_INTERFACE_HPP

#include "audio.hpp"
#include "sfont.hpp"
#include "ssfxr.hpp"
#include "sspriteset.hpp"
#include "swindowstyle.hpp"
#include "log.hpp"
#include "sphere.hpp"
#include "../common/configfile.hpp"
#include "../common/IAnimation.hpp"
#include "../common/IFileSystem.hpp"

class CMapEngine;
int pre_process_filename(const char* filename, std::string &path, bool &need_leave);
bool IsMidi(const char* filename);
struct IEngine
{
    virtual ~IEngine()
    { }

    // types
    typedef void* script;

    // methods
    virtual bool IsScriptEvaluated(const char* filename) = 0;

    virtual bool IsSystemScriptEvaluated(const char* filename) = 0;
    virtual void AddEvaluatedScript(const char* filename) = 0;
    virtual void AddEvaluatedSystemScript(const char* filename) = 0;
    virtual bool GetScriptText(const char* filename, std::string& text) = 0;
    virtual bool GetSystemScript(const char* filename, std::string& text) = 0;

    virtual script CompileScript(const char* script, std::string& error) = 0;
    virtual bool IsScriptBeingUsed(script s) = 0;
    virtual bool ExecuteScript(script s, bool& should_exit, std::string& error) = 0;
    virtual void DestroyScript(script s) = 0;

    virtual void GetGameList(std::vector<Game>& games) = 0;
    virtual void ExecuteGame(const char* directory) = 0;

    virtual void RestartGame() = 0; // tell the engine to stop the game and set the restart flag...
    virtual CMapEngine* GetMapEngine() = 0;
    virtual CLog* OpenLog(const char* filename) = 0;
    virtual void CloseLog(CLog* log) = 0;

    virtual SSPRITESET* LoadSpriteset(const char* filename) = 0;
    virtual void DestroySpriteset(SSPRITESET* spriteset) = 0;

    virtual SFONT* GetSystemFont() = 0;
    virtual SFONT* LoadFont(const char* filename) = 0;
    virtual void DestroyFont(SFONT* font) = 0;

    virtual SWINDOWSTYLE* GetSystemWindowStyle() = 0;
    virtual SWINDOWSTYLE* LoadWindowStyle(const char* filename) = 0;
    virtual void DestroyWindowStyle(SWINDOWSTYLE* windowstyle) = 0;

    virtual audiere::OutputStream* LoadSound(const char* filename, bool streaming) = 0;
    virtual audiere::SoundEffect*  LoadSoundEffect(const char* filename, audiere::SoundEffectType type) = 0;
    virtual audiere::OutputStream* CreateSound(audiere::File* memoryfile, bool streaming) =0;
    virtual audiere::SoundEffect*  CreateSoundEffect(audiere::File* memoryfile, audiere::SoundEffectType type) =0;
	virtual audiere::File* CreateMemoryFile(const void* buffer, int size) =0;

#if defined(WIN32) && defined(USE_MIDI)
    virtual audiere::MIDIStream* LoadMIDI(const char* filename) = 0;
#endif
    virtual SSFXR* CreateSfxr() = 0;
    virtual IMAGE GetSystemArrow() = 0;
    virtual IMAGE GetSystemUpArrow() = 0;
    virtual IMAGE GetSystemDownArrow() = 0;
    virtual IMAGE LoadImage(const char* filename) = 0;
    virtual void DestroyImage(IMAGE image) = 0;

    virtual CImage32* LoadSurface(const char* filename) = 0;
    virtual void DestroySurface(CImage32* surface) = 0;

    virtual IAnimation* LoadAnimation(const char* filename) = 0;

    virtual void GetDirectoryList(const char* directory, std::vector<std::string>& directories) = 0;
    virtual void GetFileList(const char* directory, std::vector<std::string>& vs) = 0;

    virtual CConfigFile* OpenFile(const char* filename) = 0;
    virtual void FlushFile(CConfigFile* file) = 0;
    virtual void CloseFile(CConfigFile* file) = 0;
    virtual IFile* OpenRawFile(const char* filename, bool writeable) = 0;

    virtual bool CreateDirectory(const char* directory) = 0;
    virtual bool RemoveDirectory(const char* directory) = 0;
    virtual bool RemoveFile(const char* filename) = 0;
    virtual bool Rename(const char* old_path, const char* new_path) = 0;

};
#endif
