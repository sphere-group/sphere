#ifndef FILE_DIALOGS_HPP
#define FILE_DIALOGS_HPP
// identifier too long
#pragma warning(disable : 4786)
#include <string>
#include <afxdlgs.h>
#include "FileTypes.hpp"
enum EFileDialogMode {
  FDM_OPEN        = 0x01,
  FDM_SAVE        = 0x02,
  FDM_MAYNOTEXIST = 0x04,
  FDM_MULTISELECT = 0x08,
};
// fix VC++ internal compiler error  ;_;
// can't just pass integers into the template class
#define MAKE_FILETYPE(type, i)  \
  struct type {                 \
    enum { ft = i };            \
  }; 

  MAKE_FILETYPE(FDT_MAPS,         GT_MAPS)
  MAKE_FILETYPE(FDT_TILESETS,     GT_TILESETS)
  MAKE_FILETYPE(FDT_SPRITESETS,   GT_SPRITESETS)
  MAKE_FILETYPE(FDT_SCRIPTS,      GT_SCRIPTS)
  MAKE_FILETYPE(FDT_SOUNDS,       GT_SOUNDS)
  MAKE_FILETYPE(FDT_FONTS,        GT_FONTS)
  MAKE_FILETYPE(FDT_WINDOWSTYLES, GT_WINDOWSTYLES)
  MAKE_FILETYPE(FDT_IMAGES,       GT_IMAGES)
  MAKE_FILETYPE(FDT_ANIMATIONS,   GT_ANIMATIONS)
  MAKE_FILETYPE(FDT_PACKAGES,     GT_PACKAGES)

#undef MAKE_FILETYPE
template<typename filetype>
class CSphereFileDialog : public CFileDialog
{
public:
  CSphereFileDialog(int mode, const char* title = NULL)
  : CFileDialog(
      mode & FDM_OPEN ? TRUE : FALSE,
      FTL.GetDefaultExtension(filetype::ft, (mode & FDM_SAVE ? true : false)),
      NULL,
      OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | 
        (mode & FDM_OPEN && !(mode & FDM_MAYNOTEXIST) ? OFN_FILEMUSTEXIST : 0) |

        (mode & FDM_MULTISELECT ? OFN_ALLOWMULTISELECT : 0),
      GenerateFilter(mode),
      NULL
  )
  {
    if (title) {
      m_ofn.lpstrTitle = title;
    }
  }
  ~CSphereFileDialog()
  {
    delete[] m_filter;
  }
  virtual int DoModal()
  {
    // set the current directory to be the initial one
    char path[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, path);
    m_ofn.lpstrInitialDir = path;
    return CFileDialog::DoModal();
  }
private:
  const char* GenerateFilter(int mode)
  {
    // generate a filter with all supported types
    std::string filter = GenerateOverallFilter(mode);
    
    // generate filters for all of the subtypes 
    if (FTL.GetNumSubTypes(filetype::ft, (mode & FDM_SAVE ? true : false)) != 1) {
      for (int i = 0; i < FTL.GetNumSubTypes(filetype::ft, (mode & FDM_SAVE ? true : false)); i++) {
        filter += GenerateSubTypeFilter(mode, i);
      }
    }
    filter += "All Files (*.*)|*.*||";
    
    m_filter = new char[filter.length() + 1];
    if (m_filter == NULL)
      return NULL;
    strcpy(m_filter, filter.c_str());
    return m_filter;
  }
  static std::string GenerateOverallFilter(int mode)
  {
    // get all extensions
    std::vector<std::string> extensions;
    FTL.GetFileTypeExtensions(filetype::ft, (mode & FDM_SAVE ? true : false), extensions);
    // make a semicolon-separated string
    std::string filters;
    for (unsigned int i = 0; i < extensions.size(); i++) {
      filters += "*." + extensions[i];
      if (i < extensions.size() - 1) {
        filters += ";";
      }
    }
    std::string filter = FTL.GetFileTypeLabel(filetype::ft, (mode & FDM_SAVE ? true : false));
    filter += " (" + filters + ")";
    filter += "|" + filters + "|";
    return filter;
  }
  static std::string GenerateSubTypeFilter(int mode, int sub_type)
  {
    // get all extensions
    std::vector<std::string> extensions;
    FTL.GetSubTypeExtensions(filetype::ft, sub_type, (mode & FDM_SAVE ? true : false), extensions);
    if (extensions.size() == 0)
      return "";
    // make a semicolon-separated string
    std::string filters;
    for (unsigned int i = 0; i < extensions.size(); i++) {
      filters += "*." + extensions[i];
      if (i < extensions.size() - 1) {
        filters += ";";
      }
    }
    std::string filter = FTL.GetSubTypeLabel(filetype::ft, sub_type, (mode & FDM_SAVE ? true : false));
    filter += " (" + filters + ")";
    filter += "|" + filters + "|";
    return filter;
  }
private:
  char* m_filter;
};
#define DEFINE_FILE_DIALOG(name, filetype) \
typedef CSphereFileDialog<filetype> name;

  DEFINE_FILE_DIALOG(CMapFileDialog,         FDT_MAPS)
  DEFINE_FILE_DIALOG(CTilesetFileDialog,     FDT_TILESETS)
  DEFINE_FILE_DIALOG(CSpritesetFileDialog,   FDT_SPRITESETS)
  DEFINE_FILE_DIALOG(CSoundFileDialog,       FDT_SOUNDS)
  DEFINE_FILE_DIALOG(CScriptFileDialog,      FDT_SCRIPTS)
  DEFINE_FILE_DIALOG(CImageFileDialog,       FDT_IMAGES)
  DEFINE_FILE_DIALOG(CAnimationFileDialog,   FDT_ANIMATIONS)
  DEFINE_FILE_DIALOG(CWindowStyleFileDialog, FDT_WINDOWSTYLES)
  DEFINE_FILE_DIALOG(CFontFileDialog,        FDT_FONTS)
  DEFINE_FILE_DIALOG(CPackageFileDialog,     FDT_PACKAGES)

#undef DEFINE_FILE_DIALOG
#endif
