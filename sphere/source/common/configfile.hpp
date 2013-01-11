#ifndef CONFIG_FILE_HPP
#define CONFIG_FILE_HPP
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif
#include <map>
#include <string>
#include "DefaultFileSystem.hpp"
class CConfigFile
{
public:
    CConfigFile(const char* filename = NULL, IFileSystem& fs = g_DefaultFileSystem);
    bool Load(const char* filename, IFileSystem& fs = g_DefaultFileSystem);
    bool Save(const char* filename, IFileSystem& fs = g_DefaultFileSystem) const;
    std::string ReadString(const char* section, const char* key, const char* def);
    int         ReadInt   (const char* section, const char* key, int def);
    double      ReadDouble(const char* section, const char* key, double def);
    bool        ReadBool  (const char* section, const char* key, bool def);
    void WriteString(const char* section, const char* key, const char* string);
    void WriteInt   (const char* section, const char* key, int i);
    void WriteDouble(const char* section, const char* key, double f);
    void WriteBool  (const char* section, const char* key, bool b);
    int GetNumSections() const;
    const char* GetSectionName(const int index) const;
    int GetNumKeys(const int section) const;
    std::string GetKey(const int section, const int index) const;
	bool RemoveKey(const int section, const int index) const;
    std::string GetValue(const int section, const int index) const;
private:
    struct Section
    {

        std::map<std::string, std::string> entries;
    };
private:
    std::map<std::string, Section> m_sections;
};
#endif
