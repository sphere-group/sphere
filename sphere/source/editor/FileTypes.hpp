#ifndef FILE_TYPES_HPP
#define FILE_TYPES_HPP
#include <vector>
#include <string>
// supported file groups
enum
{
  GT_MAPS,
  GT_SPRITESETS,
  GT_SCRIPTS,
  GT_SOUNDS,
  GT_FONTS,
  GT_WINDOWSTYLES,
  GT_IMAGES,
  GT_ANIMATIONS,
  GT_TILESETS,
  GT_PLAYLISTS,
  // special codes
  NUM_GROUP_TYPES,
  //GT_UNKNOWN,
  // GT_TILESETS = NUM_GROUP_TYPES,  // this one shouldn't be listed in projects
  GT_PACKAGES = NUM_GROUP_TYPES,
};
class CFileTypeLibrary
{
public:
  CFileTypeLibrary();
  int GetNumFileTypes();
  const char* GetFileTypeLabel(int file_type, bool save);
  void        GetFileTypeExtensions(int file_type, bool save, std::vector<std::string>& extensions);
  const char* GetDefaultExtension(int file_type, bool save);
  int         GetNumSubTypes(int file_type, bool save);
  const char* GetSubTypeLabel(int file_type, int sub_type, bool save);
  void        GetSubTypeExtensions(int file_type, int sub_type, bool save, std::vector<std::string>& extensions); 
private:
  struct SubType
  {
    std::string label;
    std::vector<std::string> extensions;
  };
  struct FileType
  {
    std::string name;
    std::string default_extension;
    std::vector<SubType> sub_types;
  };
private:
  std::vector<FileType> m_FileTypes;
};
extern CFileTypeLibrary FTL;
#endif
