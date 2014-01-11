#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP
#pragma warning(disable : 4786)  // identifier too long
#include <vector>
#include <afxwin.h>
#include <afxext.h>
#include "Project.hpp"
#include "SwatchServer.hpp"
#include "resource.h"
#include "../common/Entities.hpp"

#ifdef TABBED_WINDOW_LIST
#include "MDITabs.h"
#endif
class CDocumentWindow;  // DocumentWindow.hpp
class CProjectWindow;   // ProjectWindow.hpp
// send this to the main window to insert a file
#define WM_INSERT_PROJECT_FILE (WM_APP + 923) // wparam = 0, path = (char*)lparam
#define WM_REFRESH_PROJECT     (WM_APP + 924) // wparam = 0, lparam = 0
//#define WM_UPDATE_TOOLBARS     (WM_APP + 925) // wparam = 0, lparam = 0
#define CD_OPEN_GAME_FILE			 1							// WM_COPYDATA dwData constant
class CMainWindow : public CMDIFrameWnd
{
public:
  CMainWindow();
  BOOL Create();
#ifdef I_SUCK
  void CreateProject(const char* projectname, const char* gametitle);
  void __OpenProject__(const char* pathname);
  void OpenProject(const char* filename);
  void CloseProject();
#endif
  const char* GetProjectDirectory() const;
  static const char* GetDefaultWindowText();
	static BOOL IsProjectFile(const char * filename);
#ifdef I_SUCK
  void OpenGameSettings();
#endif
  void OpenGameFile(const char* filename);
  void OpenDocumentWindow(int grouptype, const char* filename);

	void ViewPalette(int paletteNum);
#ifdef I_SUCK
	CProjectWindow* GetProjectWindow() {return m_ProjectWindow;}
#endif
private:
  bool AddToDirectory(const char* pathname, const char* sub_directory);
  bool CheckDirectory(const char* filename, const char* sub_directory);
  void InsertProjectFile(CFileDialog* file_dialog, int grouptype, const char* predefined_path = NULL);
  void GetGamesDirectory(char games_directory[MAX_PATH]);
  void UpdateMenu();
  void UpdatePaletteMenu();
  afx_msg void OnDropFiles(HDROP hDropInfo);
  afx_msg void OnClose();
#ifdef I_SUCK
  afx_msg void OnFileBrowse();
#endif

  afx_msg void OnFileClose();
  afx_msg void OnFileOpen();
#ifdef I_SUCK
  afx_msg void OnFileNewFile();
  afx_msg void OnFileNewProject();
  afx_msg void OnFileOpenProject();
  afx_msg void OnFileCloseProject();
  afx_msg void OnFileOpenLastProject();
#endif
#ifdef I_SUCK
  afx_msg void OnFileNewMap();
  afx_msg void OnFileNewSpriteset();
#endif
  afx_msg void OnFileNewScript();
#ifdef I_SUCK
  afx_msg void OnFileNewFont();
  afx_msg void OnFileNewWindowStyle();
  afx_msg void OnFileNewImage();
  afx_msg void OnFileNewDoodad();
#endif
#ifdef I_SUCK
  afx_msg void OnFileOpenMap();
  afx_msg void OnFileOpenSpriteset();
#endif
  afx_msg void OnFileOpenScript();
#ifdef I_SUCK
  afx_msg void OnFileOpenSound();
  afx_msg void OnFileOpenFont();
  afx_msg void OnFileOpenWindowStyle();
  afx_msg void OnFileOpenImage();
  afx_msg void OnFileOpenAnimation();
  // afx_msg void OnFileOpenDoodad();
  afx_msg void OnFileOpenTileset();
#endif
#ifdef I_SUCK
  afx_msg void OnFileImportImageToMap();
  afx_msg void OnFileImportBitmapToRWS();
  afx_msg void OnFileImportBitmapToRSS();
  afx_msg void OnFileImportBitmapToRTS();
  afx_msg void OnFileImportImageToFont();
  afx_msg void OnFileImportVergeFontTemplate();
  afx_msg void OnFileImportVergeMap();
  afx_msg void OnFileImportVergeSpriteset();
  afx_msg void OnFileImportMergeRGBA();
  afx_msg void OnFileImportWindowsFont();
  afx_msg void OnFileImportRM2KCharsetToRSS();
  afx_msg void OnFileImportRM2KChipsetToRTS();
#endif
  afx_msg void OnFileSaveAll();
  afx_msg void OnFileOptions();
  afx_msg void OnFileLanguageEnglish();
  afx_msg void OnFileLanguageBulgarian();
  afx_msg void OnFileLanguageDutch();
  afx_msg void OnFileLanguageGerman();
  afx_msg void OnFileLanguageItalian();
  afx_msg void OnUpdateFileLanguageEnglish(CCmdUI* cmdui);
  afx_msg void OnUpdateFileLanguageBulgarian(CCmdUI* cmdui);
  afx_msg void OnUpdateFileLanguageDutch(CCmdUI* cmdui);
  afx_msg void OnUpdateFileLanguageGerman(CCmdUI* cmdui);
  afx_msg void OnUpdateFileLanguageItalian(CCmdUI* cmdui);
#ifdef I_SUCK
	afx_msg void OnViewProject();
  afx_msg void OnProjectInsertMap();
  afx_msg void OnProjectInsertSpriteset();
  afx_msg void OnProjectInsertScript();
  afx_msg void OnProjectInsertSound();
  afx_msg void OnProjectInsertFont();
  afx_msg void OnProjectInsertWindowStyle();
  afx_msg void OnProjectInsertImage();
  afx_msg void OnProjectInsertAnimation();
  afx_msg void OnProjectInsertDoodad();
  afx_msg void OnProjectRefresh();
  afx_msg void OnProjectRunSphere();
  afx_msg void OnProjectConfigureSphere();
  afx_msg void OnProjectOpenDirectory();
  afx_msg void OnProjectGameSettings();
  afx_msg void OnProjectPackageGame();
#endif
  afx_msg void OnWindowCloseAll();
#ifdef I_SUCK
  afx_msg void OnToolsJSConsole();
  afx_msg void OnToolsImagesToMNG();
#ifdef USE_IRC
  afx_msg void OnToolsIRCClient();
#endif
  afx_msg void OnCDAudio(UINT id);
  afx_msg void CDAudio(int cdNum);
#endif
  afx_msg void OnHelpSphereSite();
  afx_msg void OnHelpSphericalSite();
  afx_msg void OnHelpSphericalWiki();
  afx_msg void OnHelpGettingStarted();
  afx_msg void OnHelpFliksSite();
  afx_msg void OnHelpAegisKnightsSite();
  afx_msg void OnHelpLocalDocumentation();
  afx_msg void OnHelpLocalDocFunctions();
  afx_msg void OnHelpLocalChangelog();
  afx_msg void OnHelpLocalTutorial();
  afx_msg void OnHelpAbout();

  afx_msg BOOL OnNeedText(UINT id, NMHDR* nmhdr, LRESULT* result);
  afx_msg void OnUpdateOpenLastProject(CCmdUI* cmdui);
  afx_msg void OnUpdateProjectCommand(CCmdUI* cmdui);
  afx_msg void OnUpdateSaveCommand(CCmdUI* cmdui);
  afx_msg void OnUpdateSaveAllCommand(CCmdUI* cmdui);
  afx_msg void OnUpdateWindowCloseAll(CCmdUI* cmdui);
	afx_msg void OnUpdatePaletteMenu(CCmdUI* cmdui);
#ifdef I_SUCK
  afx_msg void OnUpdateViewProject(CCmdUI* cmdui);
#endif
  afx_msg void OnUpdateFileCloseCommand(CCmdUI* cmdui);
  afx_msg void OnUpdateImageCommand(CCmdUI* cmdui);
  afx_msg void OnImageFillOutToolChanged();
  afx_msg void OnImageAntialiasToolChanged();
  afx_msg void OnImageToolChanged();
  afx_msg void OnUpdateMapCommand(CCmdUI* cmdui, UINT id);
  afx_msg void OnZoomIn();
  afx_msg void OnZoomOut();
  afx_msg void OnCopy();
  afx_msg void OnPaste();
  afx_msg void OnUndo();
  afx_msg void OnRedo();
  afx_msg void OnUpdateCopy(CCmdUI* cmdui);
  afx_msg void OnUpdatePaste(CCmdUI* cmdui);
  afx_msg void OnUpdateUndo(CCmdUI* cmdui);
  afx_msg void OnUpdateRedo(CCmdUI* cmdui);
  afx_msg void UpdateToolBars(WPARAM wparam = 0, LPARAM lparam = 0);
  void DockControlBarLeftOf(CToolBar* Bar, CToolBar* LeftOf);
#define DEFINE_UPDATE_MAP_COMMAND(tool_id) afx_msg void OnUpdateMapCommand_##tool_id(CCmdUI* cmdui);
DEFINE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_ZOOM_1X)
DEFINE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_ZOOM_2X)
DEFINE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_ZOOM_4X)
DEFINE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_ZOOM_8X)
DEFINE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_GRID_TILE)
DEFINE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_1X1)
DEFINE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_3X3)
DEFINE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_5X5)
DEFINE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_SELECTTILE)
DEFINE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_FILLRECTAREA)
DEFINE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_FILLAREA)
DEFINE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_FILL_LAYER)
DEFINE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_COPYAREA)
DEFINE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_PASTE)
DEFINE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_MOVEENTITY)
DEFINE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_OBS_SEGMENT)
DEFINE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_OBS_DELETE)
DEFINE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_OBS_MOVE_PT)
DEFINE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_ZONEADD)
DEFINE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_ZONEEDIT)
DEFINE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_ZONEMOVE)
DEFINE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_ZONEDELETE)
DEFINE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_SCRIPT)
#undef DEFINE_UPDATE_MAP_COMMAND
  afx_msg void OnMapGridToolChanged();
  afx_msg void OnMapZoomToolChanged();
  afx_msg void OnMapToolChanged();
private:
  int m_NumImageToolsAllowed;
  int m_SelectedImageTools[2];
public:
  afx_msg UINT IsMapGridToolChecked();
  afx_msg UINT IsImageFillShapeToolChecked();
  afx_msg UINT IsImageAntialiasToolChecked();
  afx_msg UINT GetImageTool(int index);
  afx_msg UINT GetMapGridTool(int index);
  afx_msg UINT GetMapZoomTool(int index);
  afx_msg UINT GetMapTool(int index);
  int GetNumImageToolsAllowed() const;
private:
#ifdef I_SUCK
  afx_msg LRESULT OnInsertProjectFile(WPARAM wparam, LPARAM lparam);
  afx_msg LRESULT OnRefreshProject(WPARAM wparam, LPARAM lparam);
#endif
  afx_msg LRESULT OnCopyData(WPARAM wparam, LPARAM lparam);
  afx_msg LRESULT OnDocumentWindowClosing(WPARAM wparam, LPARAM lparam);
  afx_msg LRESULT OnSetChildMenu(WPARAM wparam, LPARAM lparam);
  afx_msg LRESULT OnClearChildMenu(WPARAM wparam, LPARAM lparam);
  afx_msg void OnViewPalette(UINT id);
  void OnLanguageChanged();
//  afx_msg void OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult);
private:
  HWND m_NextClipboardViewer;
  afx_msg void OnChangeCbChain(HWND remove, HWND after);
  afx_msg void OnDrawClipboard();

public:
  std::vector<std::string> m_ClipboardHistory;
  const char* GetDefaultFolder() const;
  std::string GetDefaultFolder(int type) const;

  // entity clipboard
  int            m_EntityClipboardType;
  sPersonEntity  m_EntityClipboardPerson;
  sTriggerEntity m_EntityClipboardTrigger;

private:
  CToolBar   m_MainToolBar;
  CToolBar   m_ImageToolBar;
  CToolBar   m_MapToolBar;
  CStatusBar m_StatusBar;
#ifdef TABBED_WINDOW_LIST
public:
  virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
private:
  CMDITabs m_wndMDITabs;
#endif
  bool     m_ProjectOpen;
  CProject m_Project;
  CProjectWindow* m_ProjectWindow;
  std::string m_DefaultFolder; // file dialogs folder
  int m_NewFileType;
  // manage pointers to document windows so we can enumerate and send messages to them
  std::vector<CDocumentWindow*> m_DocumentWindows;
  int m_ChildMenuResource;
  DECLARE_MESSAGE_MAP()
};
#endif
