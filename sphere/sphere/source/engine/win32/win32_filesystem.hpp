#ifndef WIN32_FILESYSTEM_HPP
#define WIN32_FILESYSTEM_HPP

#include <string>

struct DIRECTORYLISTimp;
typedef DIRECTORYLISTimp* DIRECTORYLIST;

struct FILELISTimp;
typedef FILELISTimp* FILELIST;

extern std::string GetSphereDirectory();

extern bool _Rename(const char* old_path, const char* new_path);
extern bool GetDirectory(std::string& directory);
extern bool MakeDirectory(const char* directory);
extern bool _RemoveDirectory(const char* directory);

extern bool EnterDirectory(const char* directory);
extern bool LeaveDirectory();

extern DIRECTORYLIST BeginDirectoryList(const char* mask);
extern void          EndDirectoryList(DIRECTORYLIST dl);
extern bool          DirectoryListDone(DIRECTORYLIST dl);
extern void          GetNextDirectory(DIRECTORYLIST dl, char directory[FILENAME_MAX]);

extern FILELIST BeginFileList(const char* mask);
extern void     EndFileList(FILELIST fl);
extern bool     FileListDone(FILELIST fl);
extern void     GetNextFile(FILELIST fl, char directory[FILENAME_MAX]);
extern bool     _RemoveFile(const char* filename);
#endif
