// disable 'identifier too long' warning
#pragma warning(disable : 4786)

// standard
#include <set>

// system
#include <afxdlgs.h>

// core
#include "Editor.hpp"
#include "WindowCommands.hpp"
#include "Configuration.hpp"
#include "Keys.hpp"
#include "Package.hpp"
#include "resource.h"
#include "translate.hpp"
#include "audiere.h"

// document windows
#include "MainWindow.hpp"

#ifdef I_SUCK
#include "ProjectWindow.hpp"
#include "MapWindow.hpp"
#include "SpritesetWindow.hpp"
#include "SoundWindow.hpp"
#include "ImageWindow.hpp"
#include "AnimationWindow.hpp"
#include "WindowStyleWindow.hpp"
#include "FontWindow.hpp"
#include "BrowseWindow.hpp"
#include "ConsoleWindow.hpp"
#endif
#include "ScriptWindow.hpp"
#include "PaletteWindow.hpp"
#include "filename_comparer.hpp"

#ifdef USE_IRC
// Delete irc from the Workspace editor if these includes fail for you
// (because you don't have my http/irc/dcc code)
// then remove the USE_IRC from the define list:
// Project->Settings... C/C++, Preprocessor Definitions.
//#include "../../../http/src/irc/irc.hpp"
//#include "../../../http/src/irc/IRCWindow.hpp"
#endif

// dialogs
#ifdef I_SUCK
#include "NewProjectDialog.hpp"
#include "GameSettingsDialog.hpp"
#include "ResizeDialog.hpp"
#include "FontGradientDialog.hpp"
#endif
#include "FileDialogs.hpp"
#include "StringDialog.hpp"
#include "OptionsDialog.hpp"
#include "CheckListDialog.hpp"

// common
#include "../common/sphere_version.h"
#include "../common/strcmp_ci.hpp"
#include "../common/ends_with.hpp"
#include "../common/system.hpp"
#ifdef I_SUCK
#include "../common/Map.hpp"
#endif

// libraries
// libraries are down here because of symbol conflicts with other headers
#ifdef I_SUCK
#include <jsapi.h>
#include <zlib.h>
#include <libmng.h>
#include <audiere.h>
#include <corona.h>
#endif

#include <assert.h>

// base for palette menu items
const int PALETTE_COMMAND = 17133;
const int NUM_PALETTES    = 100;
const int CDAUDIO_COMMAND = 17234;
const int NUM_CDAUDIOS    = 26;
static CDocumentWindow* s_JS_Console = NULL;
const char szBarState[] = "SDE_BarState";

BEGIN_MESSAGE_MAP(CMainWindow, CMDIFrameWnd)
  ON_WM_DROPFILES()
  ON_WM_CLOSE()
  ON_COMMAND(ID_FILE_ZOOM_IN,  OnZoomIn)
  ON_COMMAND(ID_FILE_ZOOM_OUT, OnZoomOut)
  ON_COMMAND(ID_FILE_COPY,     OnCopy)
  ON_COMMAND(ID_FILE_PASTE,    OnPaste)
  ON_COMMAND(ID_FILE_UNDO,    OnUndo)
  ON_COMMAND(ID_FILE_REDO,    OnRedo)
  ON_UPDATE_COMMAND_UI(ID_FILE_COPY,  OnUpdateCopy)
  ON_UPDATE_COMMAND_UI(ID_FILE_PASTE, OnUpdatePaste)
  ON_UPDATE_COMMAND_UI(ID_FILE_UNDO,  OnUpdateUndo)
  ON_UPDATE_COMMAND_UI(ID_FILE_REDO, OnUpdateRedo)
  // generic file open
  ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
  ON_COMMAND(ID_FILE_OPTIONS, OnFileOptions)
  ON_COMMAND(ID_FILE_LANGUAGE_ENGLISH, OnFileLanguageEnglish)
  ON_COMMAND(ID_FILE_LANGUAGE_DUTCH,  OnFileLanguageDutch)
  ON_COMMAND(ID_FILE_LANGUAGE_GERMAN,  OnFileLanguageGerman)
  ON_COMMAND(ID_FILE_LANGUAGE_ITALIAN, OnFileLanguageItalian)
  ON_COMMAND(ID_FILE_LANGUAGE_BULGARIAN, OnFileLanguageBulgarian)
  ON_UPDATE_COMMAND_UI(ID_FILE_LANGUAGE_ENGLISH, OnUpdateFileLanguageEnglish)
  ON_UPDATE_COMMAND_UI(ID_FILE_LANGUAGE_DUTCH, OnUpdateFileLanguageDutch)
  ON_UPDATE_COMMAND_UI(ID_FILE_LANGUAGE_GERMAN,  OnUpdateFileLanguageGerman)
  ON_UPDATE_COMMAND_UI(ID_FILE_LANGUAGE_ITALIAN, OnUpdateFileLanguageItalian)
  ON_UPDATE_COMMAND_UI(ID_FILE_LANGUAGE_BULGARIAN, OnUpdateFileLanguageBulgarian)
#ifdef I_SUCK
  // project
  ON_COMMAND(ID_FILE_NEW_PROJECT,     OnFileNewProject)
  ON_COMMAND(ID_FILE_NEW_FILE,        OnFileNewFile)
  ON_COMMAND(ID_FILE_OPEN_PROJECT,    OnFileOpenProject)
  ON_COMMAND(ID_FILE_CLOSEPROJECT,    OnFileCloseProject)
  ON_COMMAND(ID_FILE_OPENLASTPROJECT, OnFileOpenLastProject)
#endif
#ifdef I_SUCK
  // file | new
  ON_COMMAND(ID_FILE_NEW_MAP,         OnFileNewMap)
  ON_COMMAND(ID_FILE_NEW_SPRITESET,   OnFileNewSpriteset)
#endif
  ON_COMMAND(ID_FILE_NEW_SCRIPT,      OnFileNewScript)
#ifdef I_SUCK
  ON_COMMAND(ID_FILE_NEW_FONT,        OnFileNewFont)
  ON_COMMAND(ID_FILE_NEW_WINDOWSTYLE, OnFileNewWindowStyle)
  ON_COMMAND(ID_FILE_NEW_IMAGE,       OnFileNewImage)
#endif
#ifdef I_SUCK
  // file | open
  ON_COMMAND(ID_FILE_OPEN_MAP,         OnFileOpenMap)
  ON_COMMAND(ID_FILE_OPEN_SPRITESET,   OnFileOpenSpriteset)
#endif
  ON_COMMAND(ID_FILE_OPEN_SCRIPT,      OnFileOpenScript)
#ifdef I_SUCK
  ON_COMMAND(ID_FILE_OPEN_SOUND,       OnFileOpenSound)
  ON_COMMAND(ID_FILE_OPEN_FONT,        OnFileOpenFont)
  ON_COMMAND(ID_FILE_OPEN_WINDOWSTYLE, OnFileOpenWindowStyle)
  ON_COMMAND(ID_FILE_OPEN_IMAGE,       OnFileOpenImage)
  ON_COMMAND(ID_FILE_OPEN_ANIMATION,   OnFileOpenAnimation)
  ON_COMMAND(ID_FILE_OPEN_TILESET,     OnFileOpenTileset)
#endif
#ifdef I_SUCK
  // file | import
  ON_COMMAND(ID_FILE_IMPORT_IMAGETOMAPTILESET, OnFileImportImageToMap)
  ON_COMMAND(ID_FILE_IMPORT_BITMAPTORWS,       OnFileImportBitmapToRWS)
  ON_COMMAND(ID_FILE_IMPORT_BITMAPTORSS,       OnFileImportBitmapToRSS)
  ON_COMMAND(ID_FILE_IMPORT_BITMAPTORTS,       OnFileImportBitmapToRTS)
  ON_COMMAND(ID_FILE_IMPORT_IMAGETOFONT,       OnFileImportImageToFont)
  ON_COMMAND(ID_FILE_IMPORT_VERGEFONTTEMPLATE, OnFileImportVergeFontTemplate)
  ON_COMMAND(ID_FILE_IMPORT_VERGEMAP,          OnFileImportVergeMap)
  ON_COMMAND(ID_FILE_IMPORT_VERGESPRITESET,    OnFileImportVergeSpriteset)
  ON_COMMAND(ID_FILE_IMPORT_MERGE_RGBA,        OnFileImportMergeRGBA)
  ON_COMMAND(ID_FILE_IMPORT_WINDOWSFONT,       OnFileImportWindowsFont)
  ON_COMMAND(ID_FILE_IMPORT_RM2KCHARSETTORSS,  OnFileImportRM2KCharsetToRSS)
  ON_COMMAND(ID_FILE_IMPORT_RM2KCHIPSETTORTS,  OnFileImportRM2KChipsetToRTS)
#endif
  ON_COMMAND(ID_FILE_SAVEALL, OnFileSaveAll)
  ON_COMMAND(ID_FILE_EXIT,    OnClose)
#ifdef I_SUCK
  ON_COMMAND(ID_FILE_BROWSE,  OnFileBrowse)
#endif
  ON_COMMAND(ID_FILE_CLOSE,   OnFileClose)
#ifdef I_SUCK
  // insert
  ON_COMMAND(ID_PROJECT_INSERT_MAP,         OnProjectInsertMap)
  ON_COMMAND(ID_PROJECT_INSERT_SPRITESET,   OnProjectInsertSpriteset)
  ON_COMMAND(ID_PROJECT_INSERT_SCRIPT,      OnProjectInsertScript)
  ON_COMMAND(ID_PROJECT_INSERT_SOUND,       OnProjectInsertSound)
  ON_COMMAND(ID_PROJECT_INSERT_FONT,        OnProjectInsertFont)
  ON_COMMAND(ID_PROJECT_INSERT_WINDOWSTYLE, OnProjectInsertWindowStyle)
  ON_COMMAND(ID_PROJECT_INSERT_IMAGE,       OnProjectInsertImage)
  ON_COMMAND(ID_PROJECT_INSERT_ANIMATION,   OnProjectInsertAnimation)
#endif
#ifdef I_SUCK
  ON_COMMAND(ID_PROJECT_REFRESH,         OnProjectRefresh)
  ON_COMMAND(ID_PROJECT_RUNSPHERE,       OnProjectRunSphere)
  ON_COMMAND(ID_PROJECT_CONFIGURESPHERE, OnProjectConfigureSphere)
  ON_COMMAND(ID_PROJECT_OPENDIRECTORY,   OnProjectOpenDirectory)
  ON_COMMAND(ID_PROJECT_GAMESETTINGS,    OnProjectGameSettings)
  ON_COMMAND(ID_PROJECT_PACKAGE_GAME,    OnProjectPackageGame)
	ON_COMMAND(ID_VIEW_PROJECT,            OnViewProject)
#endif
  ON_COMMAND(ID_WINDOW_CLOSEALL, OnWindowCloseAll)
#ifdef I_SUCK
  ON_COMMAND(ID_TOOLS_IMAGES_TO_MNG, OnToolsImagesToMNG)
  ON_COMMAND(ID_TOOLS_JS_CONSOLE, OnToolsJSConsole)
#ifdef USE_IRC
  ON_COMMAND(ID_TOOLS_IRC_CLIENT, OnToolsIRCClient)
#endif
#endif
  ON_COMMAND(ID_HELP_SPHERESITE,          OnHelpSphereSite)
  ON_COMMAND(ID_HELP_SPHERICALSITE,       OnHelpSphericalSite)
  ON_COMMAND(ID_HELP_SPHERICALWIKI,       OnHelpSphericalWiki)
  ON_COMMAND(ID_HELP_GETTINGSTARTED,      OnHelpGettingStarted)
  ON_COMMAND(ID_HELP_FLIKSSITE,           OnHelpFliksSite)
  ON_COMMAND(ID_HELP_AEGISKNIGHTSSITE,    OnHelpAegisKnightsSite)
  ON_COMMAND(ID_HELP_LOCALDOCUMENTATION,  OnHelpLocalDocumentation)
  ON_COMMAND(ID_HELP_LOCAL_DOC_FUNCTIONS, OnHelpLocalDocFunctions)
  ON_COMMAND(ID_HELP_LOCAL_TUTORIAL,      OnHelpLocalTutorial)
  ON_COMMAND(ID_HELP_LOCAL_CHANGELOG,     OnHelpLocalChangelog)
  ON_COMMAND(ID_HELP_ABOUT,               OnHelpAbout)
  ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnNeedText)
#ifdef I_SUCK
  ON_UPDATE_COMMAND_UI(ID_FILE_OPENLASTPROJECT, OnUpdateOpenLastProject)
  ON_UPDATE_COMMAND_UI(ID_FILE_CLOSEPROJECT,    OnUpdateProjectCommand)
#endif
  ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE,       OnUpdateFileCloseCommand)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVE,        OnUpdateSaveCommand)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVEAS,      OnUpdateSaveCommand)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVECOPYAS,  OnUpdateSaveCommand)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVEALL,     OnUpdateSaveAllCommand)
  ON_UPDATE_COMMAND_UI(IDI_IMAGETOOL_PENCIL,        OnUpdateImageCommand)
  ON_UPDATE_COMMAND_UI(IDI_IMAGETOOL_LINE,          OnUpdateImageCommand)
  ON_UPDATE_COMMAND_UI(IDI_IMAGETOOL_RECTANGLE,     OnUpdateImageCommand)
  ON_UPDATE_COMMAND_UI(IDI_IMAGETOOL_CIRCLE,        OnUpdateImageCommand)
  ON_UPDATE_COMMAND_UI(IDI_IMAGETOOL_ELLIPSE,       OnUpdateImageCommand)
  ON_UPDATE_COMMAND_UI(IDI_IMAGETOOL_FILL_SHAPE,    OnUpdateImageCommand)
  ON_UPDATE_COMMAND_UI(IDI_IMAGETOOL_ANTIALIAS,     OnUpdateImageCommand)
  ON_UPDATE_COMMAND_UI(IDI_IMAGETOOL_FILL,          OnUpdateImageCommand)
  ON_UPDATE_COMMAND_UI(IDI_IMAGETOOL_SELECTION,     OnUpdateImageCommand)
  ON_UPDATE_COMMAND_UI(IDI_IMAGETOOL_FREESELECTION, OnUpdateImageCommand)
  ON_COMMAND(IDI_IMAGETOOL_PENCIL,        OnImageToolChanged)
  ON_COMMAND(IDI_IMAGETOOL_LINE,          OnImageToolChanged)
  ON_COMMAND(IDI_IMAGETOOL_RECTANGLE,     OnImageToolChanged)
  ON_COMMAND(IDI_IMAGETOOL_CIRCLE,        OnImageToolChanged)
  ON_COMMAND(IDI_IMAGETOOL_ELLIPSE,       OnImageToolChanged)
  ON_COMMAND(IDI_IMAGETOOL_FILL_SHAPE,    OnImageFillOutToolChanged)
  ON_COMMAND(IDI_IMAGETOOL_ANTIALIAS,     OnImageAntialiasToolChanged)
  ON_COMMAND(IDI_IMAGETOOL_FILL,          OnImageToolChanged)
  ON_COMMAND(IDI_IMAGETOOL_SELECTION,     OnImageToolChanged)
  ON_COMMAND(IDI_IMAGETOOL_FREESELECTION, OnImageToolChanged)
  ON_UPDATE_COMMAND_UI(IDI_MAPTOOL_ZOOM_1X,      OnUpdateMapCommand_IDI_MAPTOOL_ZOOM_1X)
  ON_UPDATE_COMMAND_UI(IDI_MAPTOOL_ZOOM_2X,      OnUpdateMapCommand_IDI_MAPTOOL_ZOOM_2X)
  ON_UPDATE_COMMAND_UI(IDI_MAPTOOL_ZOOM_4X,      OnUpdateMapCommand_IDI_MAPTOOL_ZOOM_4X)
  ON_UPDATE_COMMAND_UI(IDI_MAPTOOL_ZOOM_8X,      OnUpdateMapCommand_IDI_MAPTOOL_ZOOM_8X)
  ON_UPDATE_COMMAND_UI(IDI_MAPTOOL_GRID_TILE,    OnUpdateMapCommand_IDI_MAPTOOL_GRID_TILE)
  ON_UPDATE_COMMAND_UI(IDI_MAPTOOL_1X1,          OnUpdateMapCommand_IDI_MAPTOOL_1X1)
  ON_UPDATE_COMMAND_UI(IDI_MAPTOOL_3X3,          OnUpdateMapCommand_IDI_MAPTOOL_3X3)
  ON_UPDATE_COMMAND_UI(IDI_MAPTOOL_5X5,          OnUpdateMapCommand_IDI_MAPTOOL_5X5)
  ON_UPDATE_COMMAND_UI(IDI_MAPTOOL_SELECTTILE,   OnUpdateMapCommand_IDI_MAPTOOL_SELECTTILE)
  ON_UPDATE_COMMAND_UI(IDI_MAPTOOL_FILLRECTAREA, OnUpdateMapCommand_IDI_MAPTOOL_FILLRECTAREA)
  ON_UPDATE_COMMAND_UI(IDI_MAPTOOL_FILLAREA,     OnUpdateMapCommand_IDI_MAPTOOL_FILLAREA)
  ON_UPDATE_COMMAND_UI(IDI_MAPTOOL_FILL_LAYER,   OnUpdateMapCommand_IDI_MAPTOOL_FILL_LAYER)
  ON_UPDATE_COMMAND_UI(IDI_MAPTOOL_COPYAREA,     OnUpdateMapCommand_IDI_MAPTOOL_COPYAREA)
  ON_UPDATE_COMMAND_UI(IDI_MAPTOOL_PASTE,        OnUpdateMapCommand_IDI_MAPTOOL_PASTE)
  ON_UPDATE_COMMAND_UI(IDI_MAPTOOL_MOVEENTITY,   OnUpdateMapCommand_IDI_MAPTOOL_MOVEENTITY)
  ON_UPDATE_COMMAND_UI(IDI_MAPTOOL_OBS_SEGMENT,  OnUpdateMapCommand_IDI_MAPTOOL_OBS_SEGMENT)
  ON_UPDATE_COMMAND_UI(IDI_MAPTOOL_OBS_DELETE,   OnUpdateMapCommand_IDI_MAPTOOL_OBS_DELETE)
  ON_UPDATE_COMMAND_UI(IDI_MAPTOOL_OBS_MOVE_PT,  OnUpdateMapCommand_IDI_MAPTOOL_OBS_MOVE_PT)
  ON_UPDATE_COMMAND_UI(IDI_MAPTOOL_ZONEADD,      OnUpdateMapCommand_IDI_MAPTOOL_ZONEADD)
  ON_UPDATE_COMMAND_UI(IDI_MAPTOOL_ZONEEDIT,     OnUpdateMapCommand_IDI_MAPTOOL_ZONEEDIT)
  ON_UPDATE_COMMAND_UI(IDI_MAPTOOL_ZONEMOVE,     OnUpdateMapCommand_IDI_MAPTOOL_ZONEMOVE)
  ON_UPDATE_COMMAND_UI(IDI_MAPTOOL_ZONEDELETE,   OnUpdateMapCommand_IDI_MAPTOOL_ZONEDELETE)
  ON_UPDATE_COMMAND_UI(IDI_MAPTOOL_SCRIPT,       OnUpdateMapCommand_IDI_MAPTOOL_SCRIPT)
  ON_COMMAND(IDI_MAPTOOL_GRID_TILE,     OnMapGridToolChanged)
  ON_COMMAND(IDI_MAPTOOL_ZOOM_2X,       OnMapZoomToolChanged)
  ON_COMMAND(IDI_MAPTOOL_ZOOM_4X,       OnMapZoomToolChanged)
  ON_COMMAND(IDI_MAPTOOL_ZOOM_8X,       OnMapZoomToolChanged)
  ON_COMMAND(IDI_MAPTOOL_ZOOM_1X,       OnMapZoomToolChanged)
  ON_COMMAND(IDI_MAPTOOL_1X1,           OnMapToolChanged)
  ON_COMMAND(IDI_MAPTOOL_3X3,           OnMapToolChanged)
  ON_COMMAND(IDI_MAPTOOL_5X5,           OnMapToolChanged)
  ON_COMMAND(IDI_MAPTOOL_SELECTTILE,    OnMapToolChanged)
  ON_COMMAND(IDI_MAPTOOL_FILLRECTAREA,  OnMapToolChanged)
  ON_COMMAND(IDI_MAPTOOL_FILLAREA,      OnMapToolChanged)
  ON_COMMAND(IDI_MAPTOOL_FILL_LAYER,    OnMapToolChanged)
  ON_COMMAND(IDI_MAPTOOL_COPYAREA,      OnMapToolChanged)
  ON_COMMAND(IDI_MAPTOOL_PASTE,         OnMapToolChanged)
  ON_COMMAND(IDI_MAPTOOL_MOVEENTITY,    OnMapToolChanged)
  ON_COMMAND(IDI_MAPTOOL_OBS_SEGMENT,   OnMapToolChanged)
  ON_COMMAND(IDI_MAPTOOL_OBS_DELETE,    OnMapToolChanged)
  ON_COMMAND(IDI_MAPTOOL_OBS_MOVE_PT,   OnMapToolChanged)
  ON_COMMAND(IDI_MAPTOOL_ZONEADD,       OnMapToolChanged)
  ON_COMMAND(IDI_MAPTOOL_ZONEEDIT,      OnMapToolChanged)
  ON_COMMAND(IDI_MAPTOOL_ZONEMOVE,      OnMapToolChanged)
  ON_COMMAND(IDI_MAPTOOL_ZONEDELETE,    OnMapToolChanged)
  ON_COMMAND(IDI_MAPTOOL_SCRIPT,        OnMapToolChanged)
  ON_UPDATE_COMMAND_UI(ID_PROJECT_RUNSPHERE, OnUpdateProjectCommand)
  ON_UPDATE_COMMAND_UI(ID_WINDOW_CLOSEALL,   OnUpdateWindowCloseAll)
  ON_UPDATE_COMMAND_UI(ID_VIEW_PALETTES,     OnUpdatePaletteMenu)
  ON_UPDATE_COMMAND_UI(PALETTE_COMMAND,	     OnUpdatePaletteMenu)
#ifdef I_SUCK
  ON_UPDATE_COMMAND_UI(ID_VIEW_PROJECT,	     OnUpdateViewProject)
#endif
  // project window message
#ifdef I_SUCK
  ON_MESSAGE(WM_INSERT_PROJECT_FILE, OnInsertProjectFile)
  ON_MESSAGE(WM_REFRESH_PROJECT,     OnRefreshProject)
#endif
  // document window messages
  ON_MESSAGE(WM_DW_CLOSING,          OnDocumentWindowClosing)
  ON_MESSAGE(WM_SET_CHILD_MENU,      OnSetChildMenu)
  ON_MESSAGE(WM_CLEAR_CHILD_MENU,    OnClearChildMenu)
  ON_MESSAGE(WM_COPYDATA,            OnCopyData)
  ON_COMMAND_RANGE(PALETTE_COMMAND, PALETTE_COMMAND + NUM_PALETTES, OnViewPalette)

  ON_COMMAND_RANGE(CDAUDIO_COMMAND, CDAUDIO_COMMAND + NUM_CDAUDIOS, OnCDAudio)
  ON_WM_CHANGECBCHAIN()
  ON_WM_DRAWCLIPBOARD()
//  ON_NOTIFY(NM_RCLICK, AFX_IDW_TOOLBAR, OnNMRclick)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////

CMainWindow::CMainWindow()
: m_ProjectOpen(false)
, m_ProjectWindow(NULL)
, m_ChildMenuResource(-1)
, m_NextClipboardViewer(NULL)
, m_EntityClipboardType(sEntity::NONE)
, m_NumImageToolsAllowed(1)
, m_NewFileType(-1)
{
  m_NewFileType = GT_SCRIPTS;
}

////////////////////////////////////////////////////////////////////////////////

#ifdef TABBED_WINDOW_LIST
void CMainWindow::OnUpdateFrameTitle(BOOL bAddToTitle)
{
  CMDIFrameWnd::OnUpdateFrameTitle(bAddToTitle);
  m_wndMDITabs.Update(); // sync the mditabctrl with all views
}
#endif

////////////////////////////////////////////////////////////////////////////////

/*
void ShowWPConfig(WINDOWPLACEMENT& wp)
{
  return;
  char buffer[200];
  char size[10];
  int X, Y;
  if(wp.showCmd == SW_MAXIMIZE)
  {
    wsprintf(size, "Maximized");
    X = wp.ptMaxPosition.x;
    Y = wp.ptMaxPosition.y;
  }
  else
  {
    if(wp.showCmd == SW_MINIMIZE)
    {
      wsprintf(size, "Minimized");
      X = wp.ptMinPosition.x;
      Y = wp.ptMinPosition.y;
    }
    else
    {
      wsprintf(size, "Normal");
      X = wp.rcNormalPosition.left;
      Y = wp.rcNormalPosition.top;
    }
  }

  wsprintf(buffer, "Window is %s at (%i, %i)", size, X, Y);
  GetMainWindow()->MessageBox(buffer, "Window Settings", MB_OK);
}
*/

////////////////////////////////////////////////////////////////////////////////

void
CMainWindow::DockControlBarLeftOf(CToolBar* Bar, CToolBar* LeftOf)
{
	CRect rect;
	DWORD dw;
	UINT n;
  RecalcLayout(TRUE);

	LeftOf->GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	dw=LeftOf->GetBarStyle();
	n = 0;
	if (n == 0 && dw & CBRS_ALIGN_TOP)    n = AFX_IDW_DOCKBAR_TOP;
	if (n == 0 && dw & CBRS_ALIGN_BOTTOM) n = AFX_IDW_DOCKBAR_BOTTOM;
	if (n == 0 && dw & CBRS_ALIGN_LEFT)   n = AFX_IDW_DOCKBAR_LEFT;
	if (n == 0 && dw & CBRS_ALIGN_RIGHT)  n = AFX_IDW_DOCKBAR_RIGHT;

	DockControlBar(Bar,n,&rect);
}

////////////////////////////////////////////////////////////////////////////////

const char*
CMainWindow::GetDefaultWindowText() {
  return "Sphere " SPHERE_VERSION " Development Environment";
}

////////////////////////////////////////////////////////////////////////////////

BOOL
CMainWindow::Create()
{
  std::string title = "";
  bool use_defaulttext = Configuration::Get(KEY_MAINWINDOW_USE_DEFAULTTEXT);
  if (!use_defaulttext) {
    title = Configuration::Get(KEY_MAINWINDOW_CUSTOM_TEXT);
    if (title.length() == 0)
      title = GetDefaultWindowText();
  }

  // create the window
  CMDIFrameWnd::Create(
    AfxRegisterWndClass(0, NULL, NULL, LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_SDE))),
    (use_defaulttext) ? (GetDefaultWindowText()) : (title.c_str()),
    WS_OVERLAPPEDWINDOW,
    rectDefault,
    NULL,
    MAKEINTRESOURCE(IDR_MAIN));
  LoadAccelTable(MAKEINTRESOURCE(IDR_ACCELERATOR));

  // create the toolbar
  if (m_MainToolBar.CreateEx(
    this,
    TBSTYLE_FLAT,
    WS_CHILD | WS_VISIBLE | CBRS_SIZE_DYNAMIC | CBRS_TOP | CBRS_GRIPPER | CBRS_FLYBY | CBRS_TOOLTIPS)) {
    m_MainToolBar.SetWindowText("Main");
    m_MainToolBar.LoadToolBar(IDR_TOOLBAR);
    m_MainToolBar.EnableDocking(CBRS_ALIGN_ANY);
  }

  // create the toolbar
  m_ImageToolBar.CreateEx(
    this,
    TBSTYLE_FLAT,
    WS_CHILD | WS_VISIBLE | CBRS_SIZE_DYNAMIC | CBRS_TOP | CBRS_GRIPPER | CBRS_FLYBY | CBRS_TOOLTIPS);
  m_ImageToolBar.SetWindowText("Image");
  m_ImageToolBar.LoadToolBar(IDR_IMAGETOOLBAR);
  m_ImageToolBar.EnableDocking(CBRS_ALIGN_ANY);
  m_ImageToolBar.GetToolBarCtrl().CheckButton(IDI_IMAGETOOL_PENCIL, TRUE);
  m_SelectedImageTools[0] = m_SelectedImageTools[1] = IDI_IMAGETOOL_PENCIL;

  // create the toolbar
  m_MapToolBar.CreateEx(
    this,
    TBSTYLE_FLAT,
    WS_CHILD | WS_VISIBLE | CBRS_SIZE_DYNAMIC | CBRS_TOP | CBRS_GRIPPER | CBRS_FLYBY | CBRS_TOOLTIPS);
  m_MapToolBar.SetWindowText("Map");
  m_MapToolBar.LoadToolBar(IDR_MAPTOOLBAR);
  m_MapToolBar.EnableDocking(CBRS_ALIGN_ANY);
  m_MapToolBar.GetToolBarCtrl().CheckButton(IDI_MAPTOOL_ZOOM_1X, TRUE);
  m_MapToolBar.GetToolBarCtrl().CheckButton(IDI_MAPTOOL_1X1, TRUE);

  // status bar indicators
  static const UINT indicators[] =
  {
    ID_SEPARATOR,           // status line indicator
    ID_SEPARATOR,
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
  };

  // create the statusbar
  m_StatusBar.Create(this);
  m_StatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
  m_StatusBar.SetBarStyle(m_StatusBar.GetBarStyle() | CBRS_FLYBY | CBRS_TOOLTIPS);
  m_StatusBar.SetPaneText(0, "");
  m_StatusBar.SetPaneText(1, "");
  SetStatusBar(&m_StatusBar);

  // enable docking
  EnableDocking(CBRS_ALIGN_ANY);
  DockControlBar(&m_MainToolBar,  AFX_IDW_DOCKBAR_TOP);
  DockControlBarLeftOf(&m_ImageToolBar, &m_MainToolBar);
  DockControlBarLeftOf(&m_MapToolBar, &m_ImageToolBar);
#ifdef TABBED_WINDOW_LIST
  m_wndMDITabs.Create(this, MT_IMAGES);
#endif
#ifndef USE_SIZECBAR
  // load the command bar state
  // this is commented out because it's causing crashes for me
  // LoadBarState(szBarState);
#endif
  // enable drag and drop
  DragAcceptFiles(true);
  // show the window in the initial position
  WINDOWPLACEMENT wp = Configuration::Get(KEY_STARTUP);
  if (wp.length != 0) {
    if (wp.showCmd == SW_SHOWMINIMIZED)
      wp.showCmd = SW_SHOW;
    SetWindowPlacement(&wp);
  } else {
    ShowWindow(SW_SHOW);
  }
  /*
  wp = Configuration::Get(KEY_STANDARDTOOLBAR_PLACEMENT);
  if (wp.length != 0) {
    m_MainToolBar.SetWindowPlacement(&wp);
  } else {
    m_MainToolBar.ShowWindow(SW_SHOW);
  }
  wp = Configuration::Get(KEY_IMAGETOOLBAR_PLACEMENT);
  if (wp.length != 0) {
    m_ImageToolBar.SetWindowPlacement(&wp);
  } else {
    m_ImageToolBar.ShowWindow(SW_SHOW);
  }
  wp = Configuration::Get(KEY_MAPTOOLBAR_PLACEMENT);
  if (wp.length != 0) {
    m_MapToolBar.SetWindowPlacement(&wp);
  } else {
    m_MapToolBar.ShowWindow(SW_SHOW);
  }
  */
  UpdateWindow();
  UpdateMenu();

  char games_directory[MAX_PATH];
  GetGamesDirectory(games_directory);
  m_DefaultFolder = games_directory;
  SetCurrentDirectory(m_DefaultFolder.c_str());
  m_NextClipboardViewer = SetClipboardViewer();
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnChangeCbChain(HWND remove, HWND after)
{
  // If the next window is closing, repair the chain.
  if ((HWND) remove == m_NextClipboardViewer)
    m_NextClipboardViewer = after;
  // Otherwise, pass the message to the next link.
  else if (m_NextClipboardViewer != NULL)
    ::SendMessage(m_NextClipboardViewer, WM_CHANGECBCHAIN, (WPARAM)remove, (LPARAM)after);
}

/////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnDrawClipboard()
{
  if (IsClipboardFormatAvailable(CF_TEXT)) {
    if (OpenClipboard()) {
      HGLOBAL hglobal = GetClipboardData(CF_TEXT);
      if (hglobal != NULL) {
        LPSTR lpstr = (LPSTR) GlobalLock(hglobal);

        if (lpstr != NULL)
        {
          m_ClipboardHistory.push_back(lpstr);
        }
        GlobalUnlock(hglobal);
      }
      CloseClipboard();
    }
  }
  if (m_NextClipboardViewer != NULL)
    ::SendMessage(m_NextClipboardViewer, WM_DRAWCLIPBOARD, 0, 0);
}

////////////////////////////////////////////////////////////////////////////////

/*
afx_msg void
CMainWindow::OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult)
{
  if ( !(m_NumImageToolsAllowed > 1) )
    return;
  POINT point;

  GetCursorPos(&point);
  ::ScreenToClient(m_ImageToolBar.GetToolBarCtrl().m_hWnd, &point);
  const UINT tools[] = {IDI_IMAGETOOL_PENCIL,
                        IDI_IMAGETOOL_LINE,
                        IDI_IMAGETOOL_RECTANGLE,
                        IDI_IMAGETOOL_CIRCLE,
                        IDI_IMAGETOOL_ELLIPSE,
                        IDI_IMAGETOOL_FILL,
                        IDI_IMAGETOOL_SELECTION,
                        IDI_IMAGETOOL_FREESELECTION};
  const int num_tools = sizeof(tools) / sizeof(*tools);
  int index = m_ImageToolBar.GetToolBarCtrl().HitTest(&point);
  if (index >= 0 && index < num_tools) {
    const unsigned int id = tools[index];
    m_SelectedImageTools[1] = id;
    CToolBarCtrl& ctrl = m_ImageToolBar.GetToolBarCtrl();
    for (int i = 0; i < num_tools; i++) {
      ctrl.CheckButton(tools[i], (m_SelectedImageTools[0] == tools[i] || m_SelectedImageTools[1] == tools[i]) ? TRUE : FALSE);
    }
    char string[1000];
    sprintf (string, "Left=%d, Right=%d", m_SelectedImageTools[0], m_SelectedImageTools[1]);
    GetStatusBar()->SetWindowText(string);
  }
}
*/

////////////////////////////////////////////////////////////////////////////////

const char*
CMainWindow::GetDefaultFolder() const
{
  return m_DefaultFolder.c_str();
}

////////////////////////////////////////////////////////////////////////////////

std::string
CMainWindow::GetDefaultFolder(int type) const
{
  if (m_ProjectOpen)
  {
    std::string directory = m_Project.GetDirectory();
    if (type == WA_MAP)
    {
      directory += "\\"; directory += "maps";
    }
    if (type == WA_IMAGE)
    {
      directory += "\\"; directory += "images";
    }

    if (type == WA_SCRIPT)
    {
      directory += "\\"; directory += "scripts";
    }
    if (type == WA_SPRITESET)
    {
      directory += "\\"; directory += "spritesets";
    }
    if (type == WA_SOUND)
    {
      directory += "\\"; directory += "sounds";
    }
    if (type == WA_WINDOWSTYLE)
    {
      directory += "\\"; directory += "windowstyles";
    }
    if (type == WA_ANIMATION)
    {
      directory += "\\"; directory += "animations";
    }
    if (type == WA_FONT)
    {
      directory += "\\"; directory += "fonts";
    }
    return directory;
  }
  return m_DefaultFolder;
}

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
void
CMainWindow::CreateProject(const char* projectname, const char* gametitle)
{
  CloseProject();
  char games_directory[MAX_PATH];
  GetGamesDirectory(games_directory);
  // this may fail, but we don't care
  CreateDirectory(games_directory, NULL);

  if (!m_Project.Create(games_directory, projectname))
  {
    MessageBox("Error: Could not create project");
    return;
  }
  m_Project.SetGameTitle(gametitle);
  m_ProjectOpen = true;
  m_ProjectWindow = new CProjectWindow(this, &m_Project);
  if (m_ProjectWindow) {
    m_ProjectWindow->Create();
  }
  UpdateMenu();
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
void
CMainWindow::OpenProject(const char* filename)
{
  CloseProject();
  if (!IsProjectFile(filename))
  {
    return;
  }

  if (m_Project.Open(filename) == false)
  {
    char message[MAX_PATH + 100];
    sprintf (message, "Could not open project\n'%s'", filename);
    MessageBox(message);
    return;
  }
  Configuration::Set(KEY_LAST_PROJECT, filename);
  //printf ("Set last project: '%s'\n", filename);
  m_ProjectOpen = true;
  m_ProjectWindow = new CProjectWindow(this, &m_Project);
  if (m_ProjectWindow) {
    m_ProjectWindow->Create();
  }
  UpdateMenu();
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
void
CMainWindow::CloseProject()
{
  if (m_ProjectOpen)
  {
    m_ProjectOpen = false;
    if (m_ProjectWindow)
    {
#ifdef FLOATING_PALETTE_WINDOW
			m_ProjectWindow->ShowPalette(false);
      m_ProjectWindow->CreateBar(false);
#endif
			m_ProjectWindow->DestroyWindow();
      m_ProjectWindow = NULL;
    }
    UpdateMenu();
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
void
CMainWindow::OpenGameSettings()
{
  CGameSettingsDialog(&m_Project).DoModal();
}
#endif

////////////////////////////////////////////////////////////////////////////////

BOOL
CMainWindow::IsProjectFile(const char* filename)
{
  // test if it's a project file
  static const char* game_sgm = "game.sgm";
  int filename_length = strlen(filename);
  int game_sgm_length = strlen(game_sgm);
  return filename_length > game_sgm_length &&
		strcmp(filename + filename_length - game_sgm_length, game_sgm) == 0;
}

////////////////////////////////////////////////////////////////////////////////

const char*
CMainWindow::GetProjectDirectory() const
{
  return !m_ProjectOpen ? "" : m_Project.GetDirectory();
}

////////////////////////////////////////////////////////////////////////////////

void
CMainWindow::OpenGameFile(const char* filename)
{
  unsigned int i;

  if (!filename || strlen(filename) == 0)
    return;

#ifdef I_SUCK
  if (IsProjectFile(filename))
  {
    OpenProject(filename);
    return;
  }
#endif

  for (i = 0; i < NUM_GROUP_TYPES; i++)
  {
    std::vector<std::string> extensions;
    FTL.GetFileTypeExtensions(i, false, extensions);

    for (unsigned int k = 0; k < extensions.size(); k++) {
      std::string ext = "." + extensions[k];
      if (strcmp_ci(filename + strlen(filename) - ext.length(), ext.c_str()) == 0) {
        OpenDocumentWindow(i, filename);
        return;
      }
    }
  }

  char proto[100] = "";
  for (i = 0; i < strlen(filename) && i < (sizeof(proto) - strlen("://")); i++) {
    if (strncmp(filename + i, "://", 3) == 0) {
      strncpy(proto, filename, i);
      break;
    }
  }

  if (strcmp(proto, "") == 0) {
    if (Configuration::Get(OPEN_UNKNOWN_FILETYPES_AS_TEXT)) {
      OpenDocumentWindow(GT_SCRIPTS, filename);
    }
  }
#ifdef USE_IRC
  if (strcmp(proto, "irc") == 0) {
    const char* address = filename + strlen("irc://");
    CDocumentWindow* window = new CIRCWindow(address);
    if (window) {
      m_DocumentWindows.push_back(window);
    }
    UpdateToolBars();
    std::string f = filename;
    Configuration::Set(KEY_IRC_ADDRESS, f);
    printf ("{%s}\n", f.c_str());
  }
#endif
}

////////////////////////////////////////////////////////////////////////////////

void
CMainWindow::OpenDocumentWindow(int grouptype, const char* filename)
{
  // if a document window has the same filename, just give it focus
  for (unsigned int i = 0; i < m_DocumentWindows.size(); i++) {
    if (m_DocumentWindows[i]) {
      const char* __filename__ = m_DocumentWindows[i]->GetFilename();
      if (__filename__ != NULL && filename != NULL) {
        if (strcmp(__filename__, filename) == 0) {
          m_DocumentWindows[i]->SetFocus();
          return;
        }
      }
    }
  }

  // actually open the window now
  CDocumentWindow* window = NULL;
  switch (grouptype)
  {
#ifdef I_SUCK
    case GT_MAPS:         window = new CMapWindow(filename);         break;
    case GT_SPRITESETS:   window = new CSpritesetWindow(filename);   break;
#endif
    case GT_SCRIPTS:      window = new CScriptWindow(filename);      break;
#ifdef I_SUCK
    case GT_SOUNDS:       window = new CSoundWindow(filename);       break;
    case GT_FONTS:        window = new CFontWindow(filename);        break;
    case GT_WINDOWSTYLES: window = new CWindowStyleWindow(filename); break;
    case GT_IMAGES:       window = new CImageWindow(filename);       break;
    case GT_ANIMATIONS:   window = new CAnimationWindow(filename);   break;
    case GT_TILESETS:     window = new CMapWindow(NULL, filename);   break;
    case GT_PLAYLISTS:    window = new CSoundWindow(filename);       break;
#endif
  }

  if (window != NULL) {
    m_DocumentWindows.push_back(window);
  }

  UpdateToolBars();
}

////////////////////////////////////////////////////////////////////////////////

bool
CMainWindow::AddToDirectory(const char* pathname, const char* sub_directory)
{
  // if file isn't already in the subdirectory
  if (!CheckDirectory(pathname, sub_directory))
  {
    // ask the user if we can copy it
    char message[1024 + MAX_PATH + MAX_PATH];
    sprintf(message, "The file '%s' must be copied into the game '%s' directory.  Is this okay?", pathname, sub_directory);
    if (MessageBox(message, NULL, MB_YESNO) == IDNO)
      return false;
    char szDestination[MAX_PATH];
    strcpy(szDestination, m_Project.GetDirectory());
    strcat(szDestination, "\\");
    strcat(szDestination, sub_directory);
    // create the directory
    CreateDirectory(szDestination, NULL);
    // append the filename
    strcat(szDestination, "\\");
    strcat(szDestination, strrchr(pathname, '\\') + 1);
    // copy it
    if (CopyFile(pathname, szDestination, TRUE) == FALSE)
    {
      sprintf (message, "File '%s' appears to already exist, overwrite?", pathname);
      if (MessageBox(message, NULL, MB_YESNO) == IDNO)
        return false;
      if (CopyFile(pathname, szDestination, FALSE) == FALSE)
      {
        sprintf (message, "Error: Could not copy file '%s'", pathname);
        MessageBox(message);
        return false;
      }
    }
  }
  return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CMainWindow::CheckDirectory(const char* filename, const char* sub_directory)
{
  // remove the file name from szFileName
  char szDirectory[MAX_PATH];
  strcpy(szDirectory, filename);
  if (*strrchr(szDirectory, '\\'))
    *strrchr(szDirectory, '\\') = 0;
  // add the sub directory to the project directory
  char szProjectDirectory[MAX_PATH];
  strcpy(szProjectDirectory, m_Project.GetDirectory());
  strcat(szProjectDirectory, "\\");
  strcat(szProjectDirectory, sub_directory);
  // compare the path with the project directory + szSubDirectory

#if (defined(_MSC_VER)&&_MSC_VER==1200)
  strlwr(szDirectory);
  strlwr(szProjectDirectory);
#else
  _strlwr_s(szDirectory, MAX_PATH);
  _strlwr_s(szProjectDirectory, MAX_PATH);
#endif

  return (strcmp(szDirectory, szProjectDirectory) == 0);
}

////////////////////////////////////////////////////////////////////////////////

void
CMainWindow::InsertProjectFile(CFileDialog* file_dialog, int grouptype, const char* predefined_path)
{
  // determine filetype directory
  char path[MAX_PATH];
  sprintf(path, "%s\\%s", m_Project.GetDirectory(), m_Project.GetGroupDirectory(grouptype));
  if (!PathExists(path))
  {
    char szMessage[80];
    sprintf(szMessage, "'%s' subdirectory does not exist, create?", m_Project.GetGroupDirectory(grouptype));
    if (MessageBox(szMessage, NULL, MB_YESNO) == IDNO)
      return;
    // create the directory
    if (!CreateDirectory(path, NULL))
    {
      char szMessage[MAX_PATH + 80];
      sprintf(szMessage, "Error: Could not create directory '%s'", path);
      MessageBox(szMessage);
      return;
    }
  }
  CString sPathName;
  if (file_dialog) {
    // start the file dialog in the correct directory
    SetCurrentDirectory(path);
    if (file_dialog->DoModal() != IDOK)
      return;
    // we've got a full file path
    sPathName = file_dialog->GetPathName();
  } else {
    sPathName = predefined_path;
  }
  char path_name[MAX_PATH];
  strcpy(path_name, sPathName);
  *strrchr(path_name, '\\') = 0;
  if (strcmp_ci(path, path_name) == 0)
  {
    // if the file is in the same directory, and the file already exists
    if (FileExists(sPathName))
    {
      MessageBox("File is already in project");
      return;
    }
    else
    {
      // create an empty file that will not be valid
      FILE* file = fopen(sPathName, "wb");
      if (file) {
        fclose(file);
      }
    }
  }
  else
  {
    // if file exists, use AddToDirectory() in case we need to copy it
    AddToDirectory(sPathName, CProject::GetGroupDirectory(grouptype));
  }
}

////////////////////////////////////////////////////////////////////////////////

void
CMainWindow::GetGamesDirectory(char games_directory[MAX_PATH])
{
  strcpy(games_directory, GetSphereDirectory().c_str());
  if (games_directory[strlen(games_directory) - 1] != '\\')
    strcat(games_directory, "\\");
  strcat(games_directory, "games");
}

////////////////////////////////////////////////////////////////////////////////
afx_msg void
CMainWindow::UpdateMenu()
{
  int iWindowMenu = 3;
  // destroy the old menu
  // FIXME: how to delete menu right
  // note: gives 0 byte memory leak if not enabled but...
  //       gives win9x MDI windowing problems if enabled...
  //  if (GetMenu())
  //    GetMenu()->DestroyMenu();
  // create the new menu
  HINSTANCE hInstance = AfxGetApp()->m_hInstance;
  HMENU hNewMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MAIN));
  if (0) {
    do {
      int menu_count = GetMenuItemCount(hNewMenu);
      int i = menu_count - 2; // tools menu
      if (i < 0 || i >= menu_count)
        break;
      HMENU hCDMenu = CreateMenu();
      if (!hCDMenu)
        break;
      HMENU hSubMenu = GetSubMenu(hNewMenu, i);
      if (!hCDMenu)
        break;
      std::vector<std::string> devices;
      audiere::EnumerateCDDevices(devices);
      for (unsigned int j = 0; j < devices.size(); j++) {
        if (devices[j].size() > 0) {
          std::string title = "CD Audio " + devices[j];
          AppendMenu(hCDMenu, 0, CDAUDIO_COMMAND + j, title.c_str());
        }
      }
      if (GetMenuItemCount(hCDMenu) == 0) {
        AppendMenu(hCDMenu, 0, 0, "...");
      }
      AppendMenu(hSubMenu, MF_POPUP | MF_STRING, (UINT_PTR) hCDMenu, "CD Player");
    } while (false);
  }

  int menu_count = GetMenuItemCount(hNewMenu);
  //char buffer[100000];
  //char temp[80];
  //sprintf (buffer, "%d", menu_count);

  for (int i = 0; i < menu_count; i++)
  {
    char szPopupTitle[80] = {0};
    GetMenuString(hNewMenu, i, szPopupTitle, 80, MF_BYPOSITION);
    //sprintf (buffer + strlen(buffer), " <%s>", szPopupTitle);
    HMENU hSubMenu = GetSubMenu(hNewMenu, i);
    //GetMenuString(hSubMenu, 0, temp, 80, MF_BYPOSITION);
    //sprintf (buffer + strlen(buffer), " [%s]", temp);
    TranslateMenu(hSubMenu);
    //GetMenuString(hSubMenu, 0, temp, 80, MF_BYPOSITION);
    //sprintf (buffer + strlen(buffer), " [%s]", temp);
    //sprintf (buffer + strlen(buffer), " (%d) ", GetMenuItemCount(hSubMenu));
    RemoveMenu(hNewMenu, i, MF_BYPOSITION);
    InsertMenu(hNewMenu,
        i,
        MF_POPUP | MF_BYPOSITION | MF_STRING,
        (UINT_PTR) hSubMenu,
        (szPopupTitle));
  }
  //MessageBox(buffer);

  // if a project is open, add the project menu
  if (m_ProjectOpen)
  {
    HMENU hProjectMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_PROJECT));
    char szPopupTitle[80] = {0};
    GetMenuString(hProjectMenu, 0, szPopupTitle, 80, MF_BYPOSITION);
    HMENU hProjectMenuSubMenu = GetSubMenu(hProjectMenu, 0);
    //TranslateMenu(hProjectMenuSubMenu);
    InsertMenu(hNewMenu,
               iWindowMenu,
               MF_POPUP | MF_BYPOSITION | MF_STRING,
               (UINT_PTR) hProjectMenuSubMenu,
               TranslateString(szPopupTitle));
    iWindowMenu++;
  }
  // if a child menu is set, add it
  if (m_ChildMenuResource != -1)
  {
    HMENU hChildMenu = LoadMenu(hInstance, MAKEINTRESOURCE(m_ChildMenuResource));
    char szPopupTitle[80] = {0};
    GetMenuString(hChildMenu, 0, szPopupTitle, 80, MF_BYPOSITION);
    HMENU hChildMenuSubMenu = GetSubMenu(hChildMenu, 0);
    //TranslateMenu(hChildMenuSubMenu);
    InsertMenu(hNewMenu,
               iWindowMenu,
               MF_POPUP | MF_BYPOSITION | MF_STRING,
               (UINT_PTR) hChildMenuSubMenu,
               TranslateString(szPopupTitle));
    iWindowMenu++;
  }
  // set the new menu
  CMenu* pNewMenu = CMenu::FromHandle(hNewMenu);
  MDISetMenu(pNewMenu, pNewMenu->GetSubMenu(iWindowMenu))->DestroyMenu();
  DrawMenuBar();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnDropFiles(HDROP hDropInfo)
{
  int     i;
  int     numfiles;
  int     length;
  LPTSTR  lpFilename;
  if (hDropInfo == NULL)
    return;
  numfiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, NULL);
  for (i = 0; i < numfiles; i++)
  {
    // here's what happens, we grab the details of the file...
    // then we transfer the details to OpenGameFile
    length = DragQueryFile(hDropInfo, i, NULL, NULL);
    length++;
    lpFilename = new char[length];
    if (lpFilename == NULL)
      return;
    DragQueryFile(hDropInfo, i, lpFilename, length);
    OpenGameFile(lpFilename);
    delete[] lpFilename;
  }

  DragFinish(hDropInfo);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnClose()
{
  // ask if the child windows should be destroyed
  for (unsigned int i = 0; i < m_DocumentWindows.size(); i++)
  {
    if (m_DocumentWindows[i]->Close()) {
      m_DocumentWindows[i]->DestroyWindow();
    }
    else
      return;
  }
  m_DocumentWindows.clear();
  // save the command bar state
#ifndef USE_SIZECBAR
  // SaveBarState(szBarState);
#endif
  if (s_JS_Console) {
    s_JS_Console->DestroyWindow();
    s_JS_Console = NULL;
  }
#ifdef I_SUCK
  // close the project
  CloseProject();
#endif
  // store the window state
  WINDOWPLACEMENT wp;
  GetWindowPlacement(&wp);
  Configuration::Set(KEY_STARTUP, wp);
  m_MainToolBar.GetWindowPlacement(&wp);
  Configuration::Set(KEY_STANDARDTOOLBAR_PLACEMENT, wp);
  //ShowWPConfig(wp);
  m_ImageToolBar.GetWindowPlacement(&wp);
  Configuration::Set(KEY_IMAGETOOLBAR_PLACEMENT, wp);
  //ShowWPConfig(wp);
  m_MapToolBar.GetWindowPlacement(&wp);
  Configuration::Set(KEY_MAPTOOLBAR_PLACEMENT, wp);
  //ShowWPConfig(wp);
  ChangeClipboardChain(m_NextClipboardViewer);
  m_ClipboardHistory.clear();
  // finally, destroy the window
  DestroyWindow();
}

////////////////////////////////////////////////////////////////////////////////

std::string GenerateSupportedExtensionsFilter()
{
  int i;

  // generate list of all supported extensions
  std::set<std::string> extensions;
  for (i = 0; i < NUM_GROUP_TYPES; i++) {
    std::vector<std::string> e;
    FTL.GetFileTypeExtensions(i, false, e);
    for (unsigned int j = 0; j < e.size(); j++) {
      std::string poop = e[j];
      extensions.insert(poop);
    }
  }
  // convert extension set into a string
  std::string all_filter;
  std::set<std::string>::iterator j;
  for (j = extensions.begin(); j != extensions.end(); j++) {
    all_filter += ";";
    all_filter += "*.";
    all_filter += *j;
  }
  // generate a huge filter that supports all file types!
  std::string filter;
  filter += "All Sphere Files|game.sgm" + all_filter + "|";
  filter += "Project File (game.sgm)|game.sgm|";
  for (i = 0; i < NUM_GROUP_TYPES; i++) {
    std::vector<std::string> e;
    FTL.GetFileTypeExtensions(i, false, e);
    std::string type_filter;
    for (unsigned int j = 0; j < e.size(); j++) {
      if (j != 0) {
        type_filter += ";";
      }
      type_filter += "*." + e[j];
    }
    filter += FTL.GetFileTypeLabel(i, false);
    filter += " (" + type_filter + ")|" + type_filter + "|";
  }
  filter += "All Files (*.*)|*.*||";
  return filter;
}

////////////////////////////////////////////////////////////////////////////////

static
std::string GetFolderFromPathName(CString thePath)
{
  int p = thePath.ReverseFind('\\');
  std::string folder = thePath;
  if (p != -1) {
    thePath.Delete(p, thePath.GetLength() - p);
    folder = thePath;
  }
  return folder;
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnFileOpen()
{
  std::string filter = GenerateSupportedExtensionsFilter();
  SetCurrentDirectory(m_DefaultFolder.c_str());
  CFileDialog FileDialog(
    TRUE,
    "",
    NULL,
    OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT,
    filter.c_str());
  // set current directory on Win98/2000
  FileDialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  // execute the dialog
  if (FileDialog.DoModal() == IDOK)
  {
    POSITION pos = FileDialog.GetStartPosition();
    if (pos != NULL)
    {
      CString thePath = FileDialog.GetNextPathName(pos);
      m_DefaultFolder = GetFolderFromPathName(thePath);
    }
    pos = FileDialog.GetStartPosition();

    while (pos != NULL)
    {
      CString thePath = FileDialog.GetNextPathName(pos);
#ifdef I_SUCK
      if (!strcmp_ci(thePath.Right(8), "game.sgm"))
        OpenProject(thePath);
      else
#endif
        OpenGameFile(thePath);
    }
  }

}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnFileOptions()
{
  COptionsDialog dialog;
  dialog.Execute();
}

////////////////////////////////////////////////////////////////////////////////

void
CMainWindow::OnLanguageChanged()
{
  UpdateMenu();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnFileLanguageEnglish()
{
  SetLanguage("English");
  OnLanguageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnFileLanguageBulgarian()
{
  SetLanguage("Bulgarian");
  OnLanguageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnFileLanguageDutch()
{
  SetLanguage("Dutch");
  OnLanguageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnFileLanguageGerman()
{
  SetLanguage("German");
  OnLanguageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnFileLanguageItalian()
{
  SetLanguage("Italian");
  OnLanguageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnUpdateFileLanguageEnglish(CCmdUI* cmdui)
{
  cmdui->SetCheck( (strcmp(GetLanguage(), "English") == 0 ) ? TRUE : FALSE );
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnUpdateFileLanguageBulgarian(CCmdUI* cmdui)
{
  cmdui->SetCheck( (strcmp(GetLanguage(), "Bulgarian") == 0 ) ? TRUE : FALSE );
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnUpdateFileLanguageDutch(CCmdUI* cmdui)
{
  cmdui->SetCheck( (strcmp(GetLanguage(), "Dutch") == 0 ) ? TRUE : FALSE );
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnUpdateFileLanguageGerman(CCmdUI* cmdui)
{
  cmdui->SetCheck( (strcmp(GetLanguage(), "German") == 0 ) ? TRUE : FALSE );
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnUpdateFileLanguageItalian(CCmdUI* cmdui)
{
  cmdui->SetCheck( (strcmp(GetLanguage(), "Italian") == 0 ) ? TRUE : FALSE );
}

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnFileNewProject()
{
  CNewProjectDialog newprojectdialog(this);

  if (newprojectdialog.DoModal() == IDOK)
  {
    char projectname[32];
    char gametitle[32];
    strncpy(projectname, newprojectdialog.GetProjectName(), sizeof(projectname));
    strncpy(gametitle,   newprojectdialog.GetGameTitle(),   sizeof(gametitle));
    CreateProject(projectname, gametitle);
    char path_name[MAX_PATH];
    GetGamesDirectory(path_name);
    sprintf (path_name, "%s\\%s\\game.sgm", path_name, projectname);
    Configuration::Set(KEY_LAST_PROJECT, path_name);
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnFileNewFile()
{
  if (m_NewFileType >= 0 && m_NewFileType < NUM_GROUP_TYPES) {
    OpenDocumentWindow(m_NewFileType, NULL);
  }
  else {
    OnFileNewProject();
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
void
CMainWindow::__OpenProject__(const char* pathname)
{
  std::string filename = pathname;
              filename += "\\";
              filename += "game.sgm";
  if ( FileExists(filename.c_str()) )
  {
    OpenProject(filename.c_str());
  }
  else
  {
    std::string msg = "No game.sgm found in directory.\n";
                msg += "Open "; msg += filename; msg += " anyway?";
    if (MessageBox(msg.c_str(), "Sphere", MB_OKCANCEL) == IDOK)
    {
      OpenProject(filename.c_str());
    }
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnFileOpenProject()
{
  char games_directory[MAX_PATH];
  GetGamesDirectory(games_directory);
  SetCurrentDirectory(games_directory);
  CFileDialog FileDialog(TRUE,
                         "inf",
                         "game.sgm",
                         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
                         "Sphere Game Files (game.sgm)|game.sgm||");
  // set current directory on Win98/2000
  FileDialog.m_ofn.lpstrInitialDir = games_directory;
  if (FileDialog.DoModal() == IDOK) {
    std::string pathname = FileDialog.GetPathName();
    std::string folder = pathname;
    if (folder.rfind('\\') != std::string::npos) {
      folder[folder.rfind('\\')] = 0;
    }

    __OpenProject__(folder.c_str());
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnFileCloseProject()
{
  CloseProject();
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnFileOpenLastProject()
{
  if (Configuration::Get(KEY_LAST_PROJECT).length() != 0)
  {
    std::string folder = Configuration::Get(KEY_LAST_PROJECT);
    if (folder.rfind('\\') != std::string::npos) {
      folder[folder.rfind('\\')] = 0;
    }
    __OpenProject__(folder.c_str());
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnFileClose()
{

}

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnFileBrowse()
{
  std::string filter = GenerateSupportedExtensionsFilter();
  SetCurrentDirectory(m_DefaultFolder.c_str());
  CFileDialog FileDialog(
    TRUE,
    "",
    NULL,
    OFN_FILEMUSTEXIST,
    filter.c_str());
  // set current directory on Win98/2000
  FileDialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  if (FileDialog.DoModal() == IDOK)
  {
    m_DefaultFolder = GetFolderFromPathName(FileDialog.GetPathName());
    char szDirectory[MAX_PATH];
    std::string t = FileDialog.GetPathName();
    strcpy(szDirectory, t.c_str());
    if (*strrchr(szDirectory, '\\'))
      *strrchr(szDirectory, '\\') = 0;
    CBrowseWindow* browse_window = new CBrowseWindow(szDirectory, "*");
    if (browse_window)
      m_DocumentWindows.push_back(browse_window);
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////

#define FILE_NEW_HANDLER(name, construct)    \
  afx_msg void                               \
  CMainWindow::OnFileNew##name()             \
{                                            \
  CDocumentWindow* dw = construct;           \
  if (dw) {                                  \
    m_DocumentWindows.push_back(dw);         \
    UpdateToolBars();                        \
  }                                          \
}

#ifdef I_SUCK
FILE_NEW_HANDLER(Map,         new CMapWindow())
#endif
FILE_NEW_HANDLER(Script,      new CScriptWindow())
#ifdef I_SUCK
FILE_NEW_HANDLER(Spriteset,   new CSpritesetWindow())
FILE_NEW_HANDLER(Font,        new CFontWindow())
FILE_NEW_HANDLER(WindowStyle, new CWindowStyleWindow())
FILE_NEW_HANDLER(Image,       new CImageWindow())
#endif
#undef FILE_NEW_HANDLER

////////////////////////////////////////////////////////////////////////////////

#define FILE_OPEN_HANDLER(name, construct)                    \
  afx_msg void                                                \
  CMainWindow::OnFileOpen##name()                             \
  {                                                           \
    C##name##FileDialog Dialog(FDM_OPEN | FDM_MULTISELECT);   \
    SetCurrentDirectory(m_DefaultFolder.c_str());             \
    Dialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();   \
    if (Dialog.DoModal() == IDOK) {                           \
      POSITION pos = Dialog.GetStartPosition();               \
      if (pos != NULL) {                                      \
        CString thePath = Dialog.GetNextPathName(pos);        \
        m_DefaultFolder = GetFolderFromPathName(thePath);     \
      }                                                       \
      pos = Dialog.GetStartPosition();                        \
      while (pos != NULL) {                                   \
        CString path_ = Dialog.GetNextPathName(pos);          \
        const char* path = path_;                             \
        CDocumentWindow* dw = construct;                      \
        if (dw) {                                             \
          m_DocumentWindows.push_back(dw);                    \
          UpdateToolBars();                                   \
        }                                                     \
      }                                                       \
    }                                                         \
  }

#ifdef I_SUCK
FILE_OPEN_HANDLER(Map,         new CMapWindow(path))
#endif
FILE_OPEN_HANDLER(Script,      new CScriptWindow(path))
#ifdef I_SUCK
FILE_OPEN_HANDLER(Spriteset,   new CSpritesetWindow(path))
FILE_OPEN_HANDLER(Sound,       new CSoundWindow(path))
FILE_OPEN_HANDLER(Font,        new CFontWindow(path))
FILE_OPEN_HANDLER(WindowStyle, new CWindowStyleWindow(path))
FILE_OPEN_HANDLER(Image,       new CImageWindow(path))
FILE_OPEN_HANDLER(Animation,   new CAnimationWindow(path))
#endif
#undef FILE_OPEN_HANDLER

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnFileOpenTileset()
{
  CTilesetFileDialog Dialog(FDM_OPEN);
  Dialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  SetCurrentDirectory(m_DefaultFolder.c_str());
  if (Dialog.DoModal() == IDOK)
  {
    POSITION pos = Dialog.GetStartPosition();
    if (pos != NULL)
    {
      CString thePath = Dialog.GetNextPathName(pos);
      m_DefaultFolder = GetFolderFromPathName(thePath);
    }
    pos = Dialog.GetStartPosition();
    while (pos != NULL) {
      CString path_ = Dialog.GetNextPathName(pos);
      const char* path = path_;
      CDocumentWindow* dw = new CMapWindow(NULL, path);
      if (dw) {
        m_DocumentWindows.push_back(dw);
        UpdateToolBars();
      }
    }
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////

static void GetFilePathWithoutExtension(std::string& thepath)
{
  int pos = thepath.rfind(".");
  if (pos == -1) {
    return;
  }
  thepath = thepath.substr(0, pos);
}

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnFileImportImageToMap()
{
  // get name of image
  CImageFileDialog FileDialog(FDM_OPEN);

  FileDialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  SetCurrentDirectory(m_DefaultFolder.c_str());
  if (FileDialog.DoModal() != IDOK)
    return;
  CString filename = FileDialog.GetPathName();
  m_DefaultFolder = GetFolderFromPathName(filename);
  CMapFileDialog OutFileDialog(FDM_SAVE);
  OutFileDialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  SetCurrentDirectory(m_DefaultFolder.c_str());
  char output_filename[MAX_PATH + 1024] = {0};
  if (1) {
    std::string filename = FileDialog.GetPathName();
    GetFilePathWithoutExtension(filename);
    sprintf (output_filename, "%s.rmp", filename.c_str());
    OutFileDialog.m_ofn.lpstrFile = output_filename;
  }
  if (OutFileDialog.DoModal() == IDCANCEL)
    return;
  CResizeDialog resize_dialog("Tile Dimensions", 16, 16);
  if (resize_dialog.DoModal() != IDOK)
    return;
  bool allow_duplicates = true;
  if (MessageBox("Do you want to remove duplicate tiles?", "Remove Duplicates", MB_ICONQUESTION | MB_YESNO) == IDYES) {
    allow_duplicates = false;
  }
  // load image
  CImage32 image;
  if (image.Load(filename) == false)
  {
    MessageBox("Error: Could not load image '" + FileDialog.GetFileName() + "'");
    return;
  }
  // build map from image
  sMap map;
  if (map.BuildFromImage(image, resize_dialog.GetWidth(), resize_dialog.GetHeight(), allow_duplicates) == false)
  {
    MessageBox("Error: Could not build map from image '" + FileDialog.GetFileName() + "'");
    return;
  }
  if (map.GetNumLayers() > 0) {
    map.GetLayer(0).SetName(FileDialog.GetFileName());
  }
  bool saved = map.Save(OutFileDialog.GetPathName());
  if (saved) {
    char message[MAX_PATH + 2048] = {0};
    sprintf (message, "Conversion successful\nMap saved as '%s'", OutFileDialog.GetPathName());
    MessageBox(message);
  }
  else {
    char message[MAX_PATH + 2048] = {0};
    sprintf (message, "Error saving map: '%s'", OutFileDialog.GetPathName());
    MessageBox(message);
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnFileImportBitmapToRWS()
{
  CImageFileDialog InFileDialog(FDM_OPEN);

  InFileDialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  SetCurrentDirectory(m_DefaultFolder.c_str());
  if (InFileDialog.DoModal() == IDCANCEL)
    return;
  m_DefaultFolder = GetFolderFromPathName(InFileDialog.GetPathName());
  CWindowStyleFileDialog OutFileDialog(FDM_SAVE);
  OutFileDialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  SetCurrentDirectory(m_DefaultFolder.c_str());
  char output_filename[MAX_PATH + 1024] = {0};
  if (1) {
    std::string filename = InFileDialog.GetPathName();
    GetFilePathWithoutExtension(filename);
    sprintf (output_filename, "%s.rws", filename.c_str());
    OutFileDialog.m_ofn.lpstrFile = output_filename;
  }
  if (OutFileDialog.DoModal() == IDCANCEL)
    return;
  // m_DefaultFolder = GetFolderFromPathName(OutFileDialog.GetPathName());
  sWindowStyle ws;
  if (ws.Import(InFileDialog.GetPathName(), CreateRGBA(255, 0, 255, 255)) == false)
  {
    MessageBox("Can't Import file '" + InFileDialog.GetPathName() + "'\neither file is invalid or not a 3x3 bitmap", "Error");
    return;
  }
  if (ws.Save(OutFileDialog.GetPathName()) == false)
  {
    MessageBox("Can't Save file! '" + OutFileDialog.GetPathName() + "'");
    return;
  }
  MessageBox("Import Successful!\nWindowStyle saved as '" + OutFileDialog.GetPathName() + "'");
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnFileImportBitmapToRSS()
{
  CImageFileDialog InFileDialog(FDM_OPEN);
  InFileDialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  SetCurrentDirectory(m_DefaultFolder.c_str());
  if (InFileDialog.DoModal() != IDOK)
    return;
  m_DefaultFolder = GetFolderFromPathName(InFileDialog.GetPathName());
  CSpritesetFileDialog OutFileDialog(FDM_SAVE);
  char output_filename[MAX_PATH + 1024] = {0};
  if (1) {
    std::string filename = InFileDialog.GetPathName();
    GetFilePathWithoutExtension(filename);
    sprintf (output_filename, "%s.rss", filename.c_str());
    OutFileDialog.m_ofn.lpstrFile = output_filename;
  }
  if (OutFileDialog.DoModal() != IDOK)
    return;
  CResizeDialog ResizeDialog("Frame Size", 16, 32);
  ResizeDialog.SetRange(1, 4096, 1, 4096);
  if (ResizeDialog.DoModal() != IDOK)
    return;
  CFontGradientDialog transparent_color_dialog("Transparent color", "In", "Out", CreateRGBA(255, 0, 255, 255), CreateRGBA(255, 0, 255, 0));
  if (transparent_color_dialog.DoModal() != IDOK)
    return;
  sSpriteset sprite;
  if (sprite.Import_BMP(InFileDialog.GetPathName(), ResizeDialog.GetWidth(), ResizeDialog.GetHeight(),
      transparent_color_dialog.GetTopColor(), transparent_color_dialog.GetBottomColor()) == false)
  {
    MessageBox("Cannot Import file '" + InFileDialog.GetPathName() + "'", "Error");
    return;
  }
  if (sprite.Save(OutFileDialog.GetPathName()) == false)
  {
    MessageBox("Can't Save file! '" + OutFileDialog.GetPathName() + "'");
    return;
  }
  MessageBox("Import Successful!\nSpriteset saved as '" + OutFileDialog.GetPathName() + "'");
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnFileImportRM2KCharsetToRSS()
{
  CImageFileDialog InFileDialog(FDM_OPEN | FDM_MULTISELECT);
  InFileDialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  SetCurrentDirectory(m_DefaultFolder.c_str());
  if (InFileDialog.DoModal() != IDOK)
    return;
  RGBA color1;
  RGBA color2;
  bool ask_for_color_once = true;
  bool always_ask_for_color = false;
  POSITION pos = InFileDialog.GetStartPosition();

  while (pos != NULL)
  {
    CString path_name = InFileDialog.GetNextPathName(pos);
    m_DefaultFolder = GetFolderFromPathName(path_name);
    std::string title_name = path_name;
    GetFilePathWithoutExtension(title_name);
    CImage32 image;
    if (!image.Load(path_name)) {
      MessageBox("Could not load image: " + path_name);
      return;
    }
    if (image.GetWidth() != 288 || image.GetHeight() != 256) {
      MessageBox("Invalid image size\nRM2K charsets are 288 by 256 images");
      return;
    }
    if (ask_for_color_once || always_ask_for_color) {
      CFontGradientDialog transparent_color_dialog("Transparent color", "In", "Out", image.GetPixel(0, 0), CreateRGBA(image.GetPixel(0, 0).red, image.GetPixel(0, 0).green, image.GetPixel(0, 0).blue, 0));
      if (transparent_color_dialog.DoModal() != IDOK)
        return;
      ask_for_color_once = false;
      color1 = transparent_color_dialog.GetTopColor();
      color2 = transparent_color_dialog.GetBottomColor();
    }
    const int frame_width = 288/12;
    const int frame_height = 256/8;
    const int num_frames = 3;
    const int num_directions = 4;
    const int num_images = num_frames * num_directions;
    sSpriteset sprite;
    // there are eight spritesets per chipset, 4 rows, 2 columns
    for (int sy = 0; sy < 2; sy++) {
      for (int sx = 0; sx < 4; sx++) {

        int d;

        // create the spriteset
        sprite.Create(frame_width, frame_height, num_images, num_directions, num_frames + 1);
        sprite.SetDirectionName(0, "north");
        sprite.SetDirectionName(1, "east");
        sprite.SetDirectionName(2, "south");
        sprite.SetDirectionName(3, "west");

        for (d = 0; d < num_directions; d++) {
          for (int f = 0; f < num_frames; f++) {
            CImage32& frame = sprite.GetImage(d * num_frames + f);
            int offset_x = (sx * frame_width * num_frames) + f * frame_width;
            int offset_y = (sy * frame_height * num_directions) + d * frame_height;
            for (int y = 0; y < frame_height; y++) {
              for (int x = 0; x < frame_height; x++) {
                frame.SetPixel(x, y, image.GetPixel(x + offset_x, y + offset_y));
              }
            }
            frame.ReplaceColor(color1, color2);
          }
        }
        for (d = 0; d < num_directions; d++) {
          sprite.SetFrameIndex(d, 0, d * num_frames +  1);
          sprite.SetFrameIndex(d, 1, d * num_frames +  0);
          sprite.SetFrameIndex(d, 2, d * num_frames +  1);
          sprite.SetFrameIndex(d, 3, d * num_frames +  2);
        }
        // the following converts a four direction spriteset to an eight direction spriteset
        sprite.InsertDirection(1);
        sprite.InsertDirection(3);
        sprite.InsertDirection(5);
        sprite.InsertDirection(7);
        if (sprite.GetNumDirections() == 8) {
          int j;

          sprite.SetDirectionName(1, "northeast");
          sprite.SetDirectionName(3, "southeast");
          sprite.SetDirectionName(5, "southwest");
          sprite.SetDirectionName(7, "northwest");
          const char* directions[] = {"east", "northeast",
                                      "east", "southeast",
                                      "west", "southwest",
                                      "west", "northwest"};
          for (j = 0; j < num_directions; j++) {
            const int d1 = sprite.GetDirectionNum(directions[(j * 2) + 0]);
            const int d2 = sprite.GetDirectionNum(directions[(j * 2) + 1]);
            const int __num_frames__= sprite.GetNumFrames(d1) - 1;
            for (int f = 0; f < __num_frames__; f++) {
              //if (f > 0 || sprite.GetNumFrames(d2) == 0)
                sprite.InsertFrame(d2, f);
            }
          }
          for (j = 0; j < num_directions; j++) {
            const int d1 = sprite.GetDirectionNum(directions[(j * 2) + 0]);
            const int d2 = sprite.GetDirectionNum(directions[(j * 2) + 1]);
            const int __num_frames__= sprite.GetNumFrames(d2);
            for (int f = 0; f < __num_frames__; f++) {
              sprite.SetFrameIndex(d2, f, sprite.GetFrameIndex(d1, f));
            }
          }
          // I'm assuming that the base is the bottom part of the frame
          sprite.SetBase(0, frame_height/2, frame_width, frame_height);

          char filename[MAX_PATH] = {0};
          sprintf(filename, "%s.%d.rss", title_name.c_str(), sy * 4 + sx);
          if (!sprite.Save(filename))
          {
            MessageBox("Can't save spriteset!");
            return;
          }
        }
      }
    }
  }
  MessageBox("Charsets Converted!");
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnFileImportRM2KChipsetToRTS()
{
  CImageFileDialog InFileDialog(FDM_OPEN | FDM_MULTISELECT);
  InFileDialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  SetCurrentDirectory(m_DefaultFolder.c_str());
  if (InFileDialog.DoModal() != IDOK)
    return;
  bool allow_duplicates = true;
  bool ask_for_color_once = true;
  bool always_ask_for_color = false;
  RGBA color1;
  RGBA color2;
  POSITION pos = InFileDialog.GetStartPosition();
  while (pos != NULL)
  {
    CString path_name = InFileDialog.GetNextPathName(pos);
    m_DefaultFolder = GetFolderFromPathName(path_name);
    std::string title_name = path_name;
    GetFilePathWithoutExtension(title_name);
    CImage32 image;
    if (!image.Load(path_name))
    {
      MessageBox("Could not load image: " + path_name);
      return;
    }
    if (image.GetWidth() != 480 || image.GetHeight() != 256) {
      MessageBox("Invalid image size\nRM2K chipsets are 480 by 256 images");
      return;
    }
    if (ask_for_color_once || always_ask_for_color) {
      CFontGradientDialog transparent_color_dialog("Transparent color", "In", "Out", image.GetPixel(image.GetWidth() - 1, 0), CreateRGBA(image.GetPixel(image.GetWidth() - 1, 0).red, image.GetPixel(image.GetWidth() - 1, 0).green, image.GetPixel(image.GetWidth() - 1, 0).blue, 0));
      if (transparent_color_dialog.DoModal() != IDOK)
        return;
      color1 = transparent_color_dialog.GetTopColor();
      color2 = transparent_color_dialog.GetBottomColor();
      ask_for_color_once = false;
    }
    if (1) {
      int width  = image.GetWidth()  / 5;
      int height = image.GetHeight() * 5;
      CImage32 temp = image;
      if (temp.GetWidth() == image.GetWidth() && temp.GetHeight() == image.GetHeight()) {
        image.Resize(width, height);
        if (image.GetWidth() != width || image.GetHeight() != height)
        {
          MessageBox("Cannot resize image!");
          return;
        }
        else {
          image.SetBlendMode(CImage32::REPLACE);
          for (int i = 0; i < 5; i++) {
            for (int y = 0; y < temp.GetHeight(); y++) {
              for (int x = 0; x < width; x++) {
                image.SetPixel(x, y + (i * temp.GetHeight()), temp.GetPixel(x + (i * width), y));
              }
            }
          }
        }
      }
    }
    sTileset tileset;
    if (!tileset.BuildFromImage(image, 16, 16, allow_duplicates))
    {
      MessageBox("Can't convert image!");
      return;
    }
    for (int i = 0; i < tileset.GetNumTiles(); i++) {
      tileset.GetTile(i).ReplaceColor(color1, color2);
    }
    // make with the animated tiles
    if (1) {
      int tiles_per_row = image.GetWidth() / 16;
      for (int i = 0; i < 8; i++) {
        tileset.GetTile((i * tiles_per_row) + 0).SetNextTile((i * tiles_per_row) + 1);
        tileset.GetTile((i * tiles_per_row) + 1).SetNextTile((i * tiles_per_row) + 2);
        tileset.GetTile((i * tiles_per_row) + 2).SetNextTile((i * tiles_per_row) + 0);
        tileset.GetTile((i * tiles_per_row) + 0).SetAnimated(true);
        tileset.GetTile((i * tiles_per_row) + 1).SetAnimated(true);
        tileset.GetTile((i * tiles_per_row) + 2).SetAnimated(true);
        tileset.GetTile((i * tiles_per_row) + 0).SetDelay(8);
        tileset.GetTile((i * tiles_per_row) + 1).SetDelay(8);
        tileset.GetTile((i * tiles_per_row) + 2).SetDelay(8);
        if (i < 4) {
          tileset.GetTile((i * tiles_per_row) + 3).SetNextTile((i * tiles_per_row) + 4);
          tileset.GetTile((i * tiles_per_row) + 4).SetNextTile((i * tiles_per_row) + 5);
          tileset.GetTile((i * tiles_per_row) + 5).SetNextTile((i * tiles_per_row) + 3);
        } else {
          tileset.GetTile((i * tiles_per_row) + 3).SetNextTile((i * tiles_per_row) + tiles_per_row);
          tileset.GetTile((i * tiles_per_row) + 4).SetNextTile((i * tiles_per_row) + tiles_per_row);
          tileset.GetTile((i * tiles_per_row) + 5).SetNextTile((i * tiles_per_row) + tiles_per_row);
        }
        tileset.GetTile((i * tiles_per_row) + 3).SetAnimated(true);
        tileset.GetTile((i * tiles_per_row) + 4).SetAnimated(true);
        tileset.GetTile((i * tiles_per_row) + 5).SetAnimated(true);
        tileset.GetTile((i * tiles_per_row) + 3).SetDelay(8);
        tileset.GetTile((i * tiles_per_row) + 4).SetDelay(8);
        tileset.GetTile((i * tiles_per_row) + 5).SetDelay(8);
      }
    }
    char filename[MAX_PATH];
    sprintf (filename, "%s.rts", title_name.c_str());
    if (!tileset.Save(filename))
    {
      MessageBox("Can't save tileset!");
      return;
    }
    char message[MAX_PATH + 1024] = {0};
    sprintf (message, "Chipset Converted!\nTileset saved as '%s'", filename);
    MessageBox(message);
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnFileImportBitmapToRTS()
{
  CImageFileDialog InFileDialog(FDM_OPEN);
  InFileDialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  SetCurrentDirectory(m_DefaultFolder.c_str());
  if (InFileDialog.DoModal() != IDOK)
    return;
  m_DefaultFolder = GetFolderFromPathName(InFileDialog.GetPathName());
  CTilesetFileDialog OutFileDialog(FDM_SAVE);
  char output_filename[MAX_PATH + 1024] = {0};
  if (1) {
    std::string filename = InFileDialog.GetPathName();
    GetFilePathWithoutExtension(filename);
    sprintf (output_filename, "%s.rts", filename.c_str());
    OutFileDialog.m_ofn.lpstrFile = output_filename;
  }
  if (OutFileDialog.DoModal() != IDOK)
    return;
  CResizeDialog ResizeDialog("Tile size", 16, 16);
  if (ResizeDialog.DoModal() != IDOK)
    return;
  bool allow_duplicates = true;
  if (MessageBox("Do you want to remove duplicate tiles?", "Remove Duplicates", MB_ICONQUESTION | MB_YESNO) == IDYES) {
    allow_duplicates = false;
  }
  CImage32 image;
  if (!image.Load(InFileDialog.GetPathName()) )
  {
    MessageBox("Can't Load image!");
    return;
  }
  sTileset tileset;
  if (!tileset.BuildFromImage(image, ResizeDialog.GetWidth(), ResizeDialog.GetHeight(), allow_duplicates))
  {
    MessageBox("Can't convert image!");
    return;
  }
  if (!tileset.Save(OutFileDialog.GetPathName()))
  {
    MessageBox("Can't save tileset!");
    return;
  }
  MessageBox("Image Converted!\nTileset saved as '" + OutFileDialog.GetPathName() + "'");
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnFileImportImageToFont()
{
  CImageFileDialog InFileDialog(FDM_OPEN);

  InFileDialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  SetCurrentDirectory(m_DefaultFolder.c_str());
  if (InFileDialog.DoModal() != IDOK)
    return;
  m_DefaultFolder = GetFolderFromPathName(InFileDialog.GetPathName());
  CFontFileDialog OutFileDialog(FDM_SAVE);
  char output_filename[MAX_PATH + 1024] = {0};
  if (1) {
    std::string filename = InFileDialog.GetPathName();
    GetFilePathWithoutExtension(filename);
    sprintf (output_filename, "%s.rfn", filename.c_str());
    OutFileDialog.m_ofn.lpstrFile = output_filename;
  }
  if (OutFileDialog.DoModal() != IDOK)
    return;
  CImage32 image;
  if (!image.Load(InFileDialog.GetPathName())) {
    MessageBox("Could not load image");
    return;
  }
  RGBA no_font_color = CreateRGBA(255, 0, 0, 255);
  int border_size = 0;
  if (MessageBox("Does this image use a 1 pixel border?", "Image", MB_ICONQUESTION | MB_YESNO) == IDYES) {
    border_size = 1;
  }
  const int characters_per_row = 16;
  const int characters_per_col = 16;
  int font_width  = (image.GetWidth()  - ((characters_per_row + 1) * border_size)) / characters_per_row;
  int font_height = (image.GetHeight() - ((characters_per_row + 1) * border_size)) / characters_per_col;
  if (image.GetWidth()  != (font_width  * characters_per_row) + ((characters_per_row + 1) * border_size)
   || image.GetHeight() != (font_height * characters_per_col) + ((characters_per_col + 1) * border_size)) {
    MessageBox("Invalid image width/height");
    return;
  }
  sFont sfont;
  if (!sfont.Create(256, font_width, font_height))
    return;
  int i = 0;
  for (int fy = 0; fy < characters_per_col; fy++) {
    for (int fx = 0; fx < characters_per_row; fx++) {
      CImage32& c = sfont.GetCharacter(i);
      c.SetBlendMode(CImage32::REPLACE);
      int offset_x = ((fx + 1) * border_size) + (font_width  * fx);
      int offset_y = ((fy + 1) * border_size) + (font_height * fy);
      // extract the character from the image
      for (int iy = 0; iy < c.GetHeight(); iy++) {
        for (int ix = 0; ix < c.GetWidth(); ix++) {
          c.SetPixel(ix, iy, image.GetPixel(offset_x + ix, offset_y + iy));
        }
      }
      // now work out the size of the character
      int width  = c.GetWidth();
      int height = c.GetHeight();
      bool done = false;
      for (int x = width - 1; x >= 0; x--) {
        for (int y = 0; y < c.GetHeight(); y++) {
          if (c.GetPixel(x, y).red   != no_font_color.red
           || c.GetPixel(x, y).green != no_font_color.green
           || c.GetPixel(x, y).blue  != no_font_color.blue
           || c.GetPixel(x, y).alpha != no_font_color.alpha) {
            done = true;
            break;
          }
        }
        if (done) { break; } else { width--; }
      }
      done = false;
      for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < c.GetWidth(); x++) {
          if (c.GetPixel(x, y).red   != no_font_color.red
           || c.GetPixel(x, y).green != no_font_color.green
           || c.GetPixel(x, y).blue  != no_font_color.blue
           || c.GetPixel(x, y).alpha != no_font_color.alpha) {
            done = true;
            break;
          }
        }
        if (done) { break; } else { height--; }
      }
      // resize the character
      c.Resize(width, height);
      if (c.GetWidth() != width || c.GetHeight() != height) {
        char string[1024];
        sprintf(string, "Error resizing character %d to %d %d from %d %d", i, width, height, c.GetWidth(), c.GetHeight());
        MessageBox(string);
        return;
      }
      i += 1;
    }
  }
  if (!sfont.Save(OutFileDialog.GetPathName()))
  {
    MessageBox("Can't save font! '" + OutFileDialog.GetPathName() + "'");
    return;
  }
  MessageBox("Image Converted!\nFont saved as '" + OutFileDialog.GetPathName() + "'");
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnFileImportVergeFontTemplate()
{
  CImageFileDialog image_dialog(FDM_OPEN);
  image_dialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  SetCurrentDirectory(m_DefaultFolder.c_str());
  if (image_dialog.DoModal() != IDOK) {
    return;
  }
  m_DefaultFolder = GetFolderFromPathName(image_dialog.GetPathName());
  CFontFileDialog font_dialog(FDM_SAVE);
  if (font_dialog.DoModal() != IDOK) {
    return;
  }
  CResizeDialog resize_dialog("Font character size", 8, 12);
  if (resize_dialog.DoModal() != IDOK) {
    return;
  }
  sFont font;
  if (!font.ImportVergeTemplate(
        image_dialog.GetPathName(),
        resize_dialog.GetWidth(),
        resize_dialog.GetHeight())) {
    MessageBox("Could not import VERGE font template", "Import VERGE font template");
    return;
  }
  if (!font.Save(font_dialog.GetPathName())) {
    MessageBox("Could not save font", "Import VERGE font template");
    return;
  }
  MessageBox("Font imported successfully!", "Import VERGE font template");
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnFileImportVergeMap()
{
  #define all_files "All Files (*.*)|*.*||"
  int flags = OFN_FILEMUSTEXIST;
  CFileDialog verge_map_dialog    (TRUE, "map", NULL, flags, "VERGE Map Files (*.map)|*.map|"     all_files, this);
  CFileDialog verge_tileset_dialog(TRUE, "vsp", NULL, flags, "VERGE Tileset Files (*.vsp)|*.vsp|" all_files, this);
  CMapFileDialog map_dialog(FDM_SAVE);
  #undef all_files
  verge_map_dialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  verge_tileset_dialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  map_dialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  SetCurrentDirectory(m_DefaultFolder.c_str());
  // load VERGE map
  if (verge_map_dialog.DoModal() != IDOK) {
    return;
  }
  // load VERGE tileset
  if (verge_tileset_dialog.DoModal() != IDOK) {
    return;
  }
  // import
  sMap map;
  if (!map.Import_VergeMAP(
        verge_map_dialog.GetPathName(),
        verge_tileset_dialog.GetPathName()
      )
  ) {
    MessageBox("Could not import Verge map");
    return;
  }
  // save
  if (map_dialog.DoModal() == IDOK) {
    if (map.Save(map_dialog.GetPathName())) {
      MessageBox("Import successful!");
    } else {
      MessageBox("Could not save Sphere map");
    }
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnFileImportVergeSpriteset()
{
  CFileDialog verge_spriteset_dialog(TRUE, "chr", NULL, OFN_FILEMUSTEXIST,
    "VERGE Spriteset Files (*.chr)|*.chr|All Files (*.*)|*.*||", this);
  CFileDialog verge_palette_dialog(TRUE, "pal", NULL, OFN_FILEMUSTEXIST,
    "VERGE Palette Files (*.pal)|*.pal|All Files (*.*)|*.*||", this);
  CSpritesetFileDialog spriteset_dialog(FDM_SAVE);
  verge_spriteset_dialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  verge_palette_dialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  spriteset_dialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  SetCurrentDirectory(m_DefaultFolder.c_str());
  // load VERGE spriteset
  if (verge_spriteset_dialog.DoModal() != IDOK) {
    return;
  }
  // load VERGE palette
  if (verge_palette_dialog.DoModal() != IDOK) {
    return;
  }
  // import spriteset
  sSpriteset spriteset;
  if (!spriteset.Import_CHR(
    verge_spriteset_dialog.GetPathName(),
    verge_palette_dialog.GetPathName()
  )) {
    MessageBox("Could not import VERGE spriteset");
    return;
  }
  // save
  if (spriteset_dialog.DoModal() == IDOK) {
    if (spriteset.Save(spriteset_dialog.GetPathName())) {
      MessageBox("Import successful!");
    } else {
      MessageBox("Could not save Sphere spriteset");
    }
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnFileImportMergeRGBA()
{
  CImageFileDialog rgb_dialog  (FDM_OPEN, "Select RGB Image");
  CImageFileDialog alpha_dialog(FDM_OPEN, "Select Alpha Image");
  CImageFileDialog out_dialog  (FDM_SAVE, "Select Output Image");
  SetCurrentDirectory(m_DefaultFolder.c_str());
  rgb_dialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  alpha_dialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  out_dialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  if (rgb_dialog.DoModal()   != IDOK ||
      alpha_dialog.DoModal() != IDOK ||
      out_dialog.DoModal()   != IDOK) {
    return;
  }
  CImage32 rgb;
  CImage32 alpha;

  if (!rgb.Load(rgb_dialog.GetPathName())) {
    MessageBox("Can't load image '" + rgb_dialog.GetFileName() + "'");
    return;
  }
  if (!alpha.Load(alpha_dialog.GetPathName())) {
    MessageBox("Can't load image '" + alpha_dialog.GetFileName() + "'");
    return;
  }
  if (rgb.GetWidth() != alpha.GetWidth() ||
      rgb.GetHeight() != alpha.GetHeight()) {
    MessageBox("RGB and alpha images aren't same size");
    return;
  }
  CImage32 out(rgb.GetWidth(), rgb.GetHeight());
  for (int i = 0; i < rgb.GetWidth() * rgb.GetHeight(); i++) {
    RGBA* dest      = out.GetPixels() + i;
    RGBA* src_rgb   = rgb.GetPixels() + i;
    RGBA* src_alpha = alpha.GetPixels() + i;
    dest->red   = src_rgb->red;
    dest->green = src_rgb->green;
    dest->blue  = src_rgb->blue;
    dest->alpha = (src_alpha->red + src_alpha->green + src_alpha->blue) / 3;
  }
  if (!out.Save(out_dialog.GetPathName())) {
    MessageBox("Can't save image '" + out_dialog.GetPathName() + "'");
    return;
  }
  MessageBox("Image merged successfully\nImage saved as '" + out_dialog.GetPathName() + "'");
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
static bool WindowsFontToSphereFont(LOGFONT lf, COLORREF color, sFont* sphere_font)
{
  // grab the font
  HDC dc = CreateCompatibleDC(NULL);
  HFONT font = CreateFontIndirect(&lf);
  HFONT old_font = (HFONT)SelectObject(dc, font);
  if (!sphere_font->Create(256))
    return false;

  for (int i = 0; i < 256; i++) {
    char c = (char)i;
    SIZE size;
    if (GetTextExtentPoint32(dc, &c, 1, &size) == FALSE)
      return false;
    // grab the character
    CDIBSection* dib = new CDIBSection(size.cx, size.cy, 32);
    if (dib == NULL)
      return false;
    memset(dib->GetPixels(), 255, size.cx * size.cy * 4); // opaque white
    RECT rect = { 0, 0, size.cx, size.cy };
    SetTextColor(dib->GetDC(), 0);  // black
    SetBkMode(dib->GetDC(), TRANSPARENT);
    HFONT old_font = (HFONT)SelectObject(dib->GetDC(), font);
    DrawText(dib->GetDC(), &c, 1, &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
    SelectObject(dib->GetDC(), old_font);
    // put it into the font
    int width = size.cx + 1;
    sphere_font->GetCharacter(i).Resize(width, size.cy);

    if (sphere_font->GetCharacter(i).GetWidth() != width
     || sphere_font->GetCharacter(i).GetHeight() != size.cy) {
      delete dib;
      return false;
    }
    memset(sphere_font->GetCharacter(i).GetPixels(), 0, width * size.cy * 4);
    BGRA* src = (BGRA*)dib->GetPixels();
    RGBA* dst = sphere_font->GetCharacter(i).GetPixels();
    for (int iy = 0; iy < size.cy; iy++) {
      for (int ix = 0; ix < size.cx; ix++) {
        dst[iy * width + ix].red   = GetRValue(color);
        dst[iy * width + ix].green = GetGValue(color);
        dst[iy * width + ix].blue  = GetBValue(color);
        dst[iy * width + ix].alpha = 255 - (src->red + src->green + src->blue) / 3;
        src++;
      }
    }
    delete dib;
  }
  SelectObject(dc, old_font);
  DeleteObject(font);
  DeleteDC(dc);
  return true;
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnFileImportWindowsFont()
{
  // windows font dialog
  CFontDialog font_dialog(NULL, CF_FORCEFONTEXIST | CF_EFFECTS | CF_SCREENFONTS);
  if (font_dialog.DoModal() != IDOK) {
    return;
  }
  COLORREF color = font_dialog.GetColor();
  // get the font from the dialog
  LOGFONT lf;
  font_dialog.GetCurrentFont(&lf);
  sFont sphere_font;
  if (!WindowsFontToSphereFont(lf, color, &sphere_font))
    return;
  // save the Sphere font
  CFontFileDialog file_dialog(FDM_SAVE, "Select Output Font");
  file_dialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  SetCurrentDirectory(m_DefaultFolder.c_str());
  if (file_dialog.DoModal() != IDOK) {
    return;
  }
  m_DefaultFolder = GetFolderFromPathName(file_dialog.GetPathName());
  if (!sphere_font.Save(file_dialog.GetPathName())) {
    MessageBox("Could not save font '" + file_dialog.GetPathName() + "'");
  } else {
    MessageBox("Font imported!\nFont saved as '" + file_dialog.GetPathName() + "'");
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnFileSaveAll()
{
  for (unsigned int i = 0; i < m_DocumentWindows.size(); i++) {
    CDocumentWindow* dw = m_DocumentWindows[i];
    if (dw->IsSaveable()) {
      CSaveableDocumentWindow* sdw = (CSaveableDocumentWindow*)dw;
      sdw->Save();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
#define PROJECT_INSERT_HANDLER(type, group_type)                        \
afx_msg void                                                            \
CMainWindow::OnProjectInsert##type()                                    \
{                                                                       \
  C##type##FileDialog Dialog(FDM_OPEN | FDM_MULTISELECT);               \
  Dialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();               \
  if (Dialog.DoModal() == IDOK) {                                       \
    POSITION pos = Dialog.GetStartPosition();                           \
    if (pos != NULL) {                                                  \
      CString thePath = Dialog.GetNextPathName(pos);                    \
      m_DefaultFolder = GetFolderFromPathName(thePath);                 \
    }                                                                   \
    pos = Dialog.GetStartPosition();                                    \
    while (pos != NULL) {                                               \
      CString path_ = Dialog.GetNextPathName(pos);                      \
      const char* path = path_;                                         \
      OnInsertProjectFile(group_type, (WPARAM)path);                    \
    }                                                                   \
  }                                                                     \
  /* save the project and update the view                     */        \
  /* (in case user changed filename extensions in the dialog) */        \
  m_Project.RefreshItems();                                             \
  m_ProjectWindow->Update();                                            \
}

PROJECT_INSERT_HANDLER(Map,         GT_MAPS)
PROJECT_INSERT_HANDLER(Spriteset,   GT_SPRITESETS)
PROJECT_INSERT_HANDLER(Script,      GT_SCRIPTS)
PROJECT_INSERT_HANDLER(Sound,       GT_SOUNDS)
PROJECT_INSERT_HANDLER(Font,        GT_FONTS)
PROJECT_INSERT_HANDLER(WindowStyle, GT_WINDOWSTYLES)
PROJECT_INSERT_HANDLER(Image,       GT_IMAGES)
PROJECT_INSERT_HANDLER(Animation,   GT_ANIMATIONS)
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnProjectRefresh()
{
  if (m_ProjectWindow) {
    m_ProjectWindow->Update();
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnProjectRunSphere()
{
  if (true) {
    const char* script = m_Project.GetGameScript();
    if (script == NULL || strlen(script) == NULL) {
      MessageBox("No game script set.\nChoose one in Game Settings...");
      return;
    }
  }
  char szCommandLine[MAX_PATH + 128];
  strcpy(szCommandLine, GetSphereDirectory().c_str());
  strcat(szCommandLine, "\\engine.exe");

  if ( !FileExists(szCommandLine) ) {
    std::string error = "Could not run Sphere because engine.exe doesn't exist...\n";
    error += szCommandLine;
    MessageBox(error.c_str());
    return;
  }
  strcat(szCommandLine, " -game ");
  strcat(szCommandLine, "\"");
  strcat(szCommandLine, m_Project.GetDirectory());
  strcat(szCommandLine, "\"");

  if (true)
  {
    char filename[MAX_PATH];
    memset(filename, 0, MAX_PATH);

    // find the currently open map
    CDocumentWindow* dw = GetCurrentDocumentWindow();
    if (dw) {
      const char* document_filename = dw->GetFilename();
      if (document_filename) {
        strcpy(filename, document_filename);
        char* ptr = strrchr(filename, '\\');
        if (ptr != NULL)
        {
          strcpy(filename, ptr + 1);
        }
      }
    }

    if (ends_with_ci(filename, ".rmp"))
    {
      strcat(szCommandLine, " -parameters ");
      strcat(szCommandLine, "\"'");
      strcat(szCommandLine, filename);
      strcat(szCommandLine, "'\"");
    }
  }

  STARTUPINFO si;
  memset(&si, 0, sizeof(si));
  si.cb = sizeof(si);
  PROCESS_INFORMATION pi;

  char sphere_directory[MAX_PATH];
  strcpy(sphere_directory, GetSphereDirectory().c_str());
  BOOL retval = CreateProcess(
    NULL,                  // lpApplicationName
    szCommandLine,         // lpCommandLine
    NULL,                  // lpProcessAttributes
    NULL,                  // lpThreadAttributes
    FALSE,                 // bInheritHandles
    0,                     // dwCreationFlags
    NULL,                  // lpEnvironment
    sphere_directory,      // lpCurrentDirectory
    &si,                   // lpStartupInfo
    &pi);                  // lpProcessInformation

  if (retval == FALSE)
    MessageBox("Error: Could not execute Sphere engine");
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnProjectConfigureSphere()
{
  char sphere_directory[MAX_PATH];
  strcpy(sphere_directory, GetSphereDirectory().c_str());
  char szCommandLine[MAX_PATH + 80];
  strcpy(szCommandLine, sphere_directory);
  strcat(szCommandLine, "\\config.exe");
  if ( !FileExists(szCommandLine) ) {
    std::string error = "Could not configure Sphere because config.exe doesn't exist...\n";
    error += szCommandLine;
    MessageBox(error.c_str());
    return;
  }
  STARTUPINFO si;
  memset(&si, 0, sizeof(si));
  si.cb = sizeof(si);
  PROCESS_INFORMATION pi;
  BOOL retval = CreateProcess(
    NULL,                  // lpApplicationName
    szCommandLine,         // lpCommandLine
    NULL,                  // lpProcessAttributes
    NULL,                  // lpThreadAttributes
    FALSE,                 // bInheritHandles
    0,                     // dwCreationFlags
    NULL,                  // lpEnvironment
    sphere_directory,      // lpCurrentDirectory
    &si,                   // lpStartupInfo
    &pi);                  // lpProcessInformation
  if (retval == FALSE)
    MessageBox("Error: Could not configure Sphere");
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnProjectGameSettings()
{
  OpenGameSettings();
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnProjectOpenDirectory()
{
  if ((int)ShellExecute(NULL, "open", m_Project.GetDirectory(), 0, 0, SW_SHOWNORMAL) <= 32) {
    MessageBox("Could not open folder", m_Project.GetDirectory(), MB_OK);
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
void OnPackageFileWritten(const char* filename, int index, int total)
{
  char string[MAX_PATH + 1024] = {0};
  if (index == -1) {
    sprintf (string, "Packaging... %80d...", total);
  }
  else {
    int percent = (int)( ((double)index / (double)total) * 100);
    if (strcmp(filename, "") == 0) {
      sprintf (string, "%3d%% Complete", percent);
    }
    else {
      sprintf (string, "Packaging '%s'... %3d%% Complete", filename, percent);
    }
  }
  GetStatusBar()->SetWindowText(string);
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnProjectPackageGame()
{
  if (true) {
    const char* script = m_Project.GetGameScript();
    if (script == NULL || strlen(script) == NULL) {
      MessageBox("No game script set.\nChoose one in Game Settings...");
      return;
    }
  }
  struct Local {
    static bool IsGameFileType(const char* filename)
    {
      if (ends_with_ci(filename, ".ini")) { // if (strcmp_ci(filename + strlen(filename) - 3, "ini") == 0) {
        return true;
      }
      if (ends_with_ci(filename, ".dat")) { // if (strcmp_ci(filename + strlen(filename) - 3, "dat") == 0) {
        return true;
      }
      if (ends_with_ci(filename, ".sgm")) { // if (strcmp_ci(filename + strlen(filename) - 3, "sgm") == 0) {
        return true;
      }
      for (int i = 0; i < NUM_GROUP_TYPES; i++)
      {
        std::vector<std::string> extensions;
        FTL.GetFileTypeExtensions(i, false, extensions);

        for (unsigned int k = 0; k < extensions.size(); k++) {
          std::string ext = "." + extensions[k];
          if (ends_with_ci(filename, ext.c_str())) { // if (strcmp_ci(filename + strlen(filename) - ext.length(), ext.c_str()) == 0) {
            return true;
          }
        }
      }
      return false;
    }
  };
  char old_directory[MAX_PATH];
  GetCurrentDirectory(MAX_PATH, old_directory);
  if (SetCurrentDirectory(m_Project.GetDirectory()) == 0) {
    char string[1024 + MAX_PATH];
    sprintf (string, "Cannot open game directory\n'%s'", m_Project.GetDirectory());
    MessageBox(string, "Package Game", MB_OK | MB_ICONERROR);
    return;
  }
  // go into game directory and add all files
  std::list<std::string> files;
  EnumerateFiles("*", "", "", files);
  if (files.size() == 0)
    return;
  CPackage package;
  std::list<std::string>::iterator i;
  int j = 0;
  for (i = files.begin(); i != files.end(); i++) {
    package.AddFile(i->c_str());
  }
  CCheckListDialog checklist;
  checklist.SetCaption("Select Package Files...");
  checklist.SetMinChecked(1);
  for (i = files.begin(); i != files.end(); i++) {
    if ( !checklist.AddItem(i->c_str(), Local::IsGameFileType(i->c_str())) ) {
      return;
    }
  }
  if (checklist.DoModal() != IDOK) {
    return;
  }
  for (j = int(files.size()) - 1; j >= 0; j--) {
    if (checklist.IsChecked(j) == false) {
      package.RemoveFile(j);
    }
  }

  j = 0;
  std::string game_script = "scripts/";
              game_script += m_Project.GetGameScript();
  for (i = files.begin(); i != files.end(); i++, j++) {
    if (checklist.IsChecked(j) == false) {
      if ( strcmp_ci(i->c_str(), "game.sgm") == 0 ) {
        MessageBox("You must include game.sgm in the package", "Package Game", MB_OK | MB_ICONERROR);
        return;
      }
      if ( strcmp_ci(i->c_str(), game_script.c_str()) == 0 ) {
        MessageBox("You must include the game script in the package", "Package Game", MB_OK | MB_ICONERROR);
        return;
      }
    }
  }
  if (package.GetNumFiles() == 0) {
    MessageBox("You must include atleast one file in the package", "Package Game", MB_OK | MB_ICONERROR);
    return;
  }
  // TODO:  show a wait dialog (no cancel)
  // it now shows "xyz% Complete" in the status bar...
  CPackageFileDialog dialog(FDM_SAVE);
  dialog.m_ofn.Flags |= OFN_NOCHANGEDIR;
  if (dialog.DoModal() == IDOK) {
    // write the package!
    std::string filename = dialog.GetPathName();
    if ( !package.Write(filename.c_str(), &OnPackageFileWritten) ) {
      MessageBox("Package creation failed", "Package Game", MB_OK | MB_ICONERROR);
    } else {
      MessageBox("Package creation succeeded!", "Package Game", MB_OK);
    }
  }
  SetCurrentDirectory(old_directory);
}
#endif

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnWindowCloseAll()
{
  for (unsigned int i = 0; i < m_DocumentWindows.size(); i++) {
    CDocumentWindow* dw = m_DocumentWindows[i];
    if (dw->Close() == false) {
      break;
    }
    dw->DestroyWindow();
  }
}

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
#include "../common/write_mng.hpp"
mng_bool MNG_DECL GetNextImageFromFileList(int index, CImage32& image, void* data) {
  // std::vector<CImage32>& images = (std::vector<CImage32>&) data;
  std::vector<std::string>* filelist = (std::vector<std::string>*) data;
  if (index < 0 || (unsigned int) index >= filelist->size())
    return MNG_FALSE;
  return image.Load((*filelist)[index].c_str()) ? MNG_TRUE : MNG_TRUE;
}

mng_uint32 MNG_DECL GetDelayFromImageFileList(int index, void* data) {
  return 10 * 1000;
}

static mng_bool MNG_DECL ContinueProcessing(int index, int total)
{
  mng_bool ret = MNG_TRUE;
  if (1) {
    char string[255] = {0};
    if (index == -1) {
      sprintf (string, "%d...", total);
    }
    else {
      int percent = (int)( ((double)index / (double)total) * 100);
      sprintf (string, "%3d%% Complete", percent);
    }
    GetStatusBar()->SetWindowText(string);
  }
  /*
  MSG msg;
  int count = 0;
  while (count++ < 4 && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
  {
    if (msg.message != WM_QUIT) {
      DispatchMessage(&msg);
    }
    else {
      ret = false;
      break;
    }
  }
  */
  return ret;
}

mng_retcode TestAnimationCode(std::vector<std::string> filelist, const char* filename) {
  return SaveMNGAnimationFromImages(filename, GetNextImageFromFileList, GetDelayFromImageFileList, ContinueProcessing, (void*) &filelist);
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnToolsImagesToMNG()
{
  struct Local {
    static bool IsGameFileType(const char* filename)
    {
      std::vector<std::string> extensions;
      FTL.GetFileTypeExtensions(GT_IMAGES, false, extensions);

      for (unsigned int k = 0; k < extensions.size(); k++) {
        std::string ext = "." + extensions[k];
        if (ends_with_ci(filename, ext.c_str())) { //if (strcmp_ci(filename + strlen(filename) - ext.length(), ext.c_str()) == 0) {
          return true;
        }
      }
      return false;
    }
  };
  // get name of image
  CImageFileDialog FileDialog(FDM_OPEN);

  FileDialog.m_ofn.lpstrInitialDir = m_DefaultFolder.c_str();
  SetCurrentDirectory(m_DefaultFolder.c_str());
  if (FileDialog.DoModal() != IDOK)
    return;
  CString filename = FileDialog.GetPathName();
  m_DefaultFolder = GetFolderFromPathName(filename);
  std::vector<std::string> filelist = GetFileList("*");
  if (filelist.size() == 0) {
    MessageBox("No files found", "Images To MNG", MB_OK);
    return;
  }
  CCheckListDialog checklist;
  checklist.SetCaption("Select Files...");
  checklist.SetMinChecked(1);
  for (int i = 0; i < int(filelist.size()); i++) {
    if ( !checklist.AddItem(filelist[i].c_str(), Local::IsGameFileType(filelist[i].c_str())) ) {
      return;
    }
  }
  if (checklist.DoModal() != IDOK) {
    return;
  }
  for (int j = int(filelist.size()) - 1; j >= 0; j--) {
    if (checklist.IsChecked(j) == false) {
      filelist.erase(filelist.begin() + j);
    }
  }
  if (filelist.size() == 0) {
    MessageBox("No files to convert", "Images To MNG", MB_OK);
    return;
  }
  CAnimationFileDialog dialog(FDM_SAVE, "Export Direction As Animation");
  if (dialog.DoModal() == IDOK) {
    bool is_mng = strcmp_ci(dialog.GetFileExt(), "mng") == 0;
    bool is_fli = strcmp_ci(dialog.GetFileExt(), "flic") == 0
               || strcmp_ci(dialog.GetFileExt(), "flc")  == 0
               || strcmp_ci(dialog.GetFileExt(), "fli")  == 0;

    if (is_mng) {
      SetCurrentDirectory(m_DefaultFolder.c_str());
      mng_retcode iRC = TestAnimationCode(filelist, dialog.GetPathName());
      if (iRC == 0) {
        MessageBox("Exported Animation!", "Export Direction As Animation", MB_OK);
      }
      else {
        MessageBox(mng_get_error_message(iRC), "Error Exporting Direction As Animation", MB_OK);
      }
    }
    else {
      MessageBox("Unsupported save mode", "Error Exporting Direction As Animation", MB_OK);
    }
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnToolsJSConsole()
{
  if (s_JS_Console == NULL) {
    s_JS_Console = new CConsoleWindow();
  }
  else {
    s_JS_Console->SetFocus();
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
#ifdef USE_IRC
afx_msg void
CMainWindow::OnToolsIRCClient()
{
  std::string address = Configuration::Get(KEY_IRC_ADDRESS);
  if (address.length() == 0) {
    address = "irc://irc.esper.net";
  }
  else
  if (address.find("irc://") != 0) {
    address = "irc://irc.esper.net";
  }
  OpenGameFile(address.c_str());
}
#endif
#endif

////////////////////////////////////////////////////////////////////////////////

void OpenURL(const std::string& url, const std::string& label)
{
  if ((int)ShellExecute(NULL, "open", url.c_str(), 0, 0, SW_SHOW) <= 32) {
    MessageBox(
      NULL,
      ("Could not open web site.  Visit " + url).c_str(),
      label.c_str(),
      MB_OK);
  }
}

////////////////////////////////////////////////////////////////////////////////

void PromptURL(const std::string& url, const std::string& label)
{
  CStringDialog dialog(label.c_str(), url.c_str());
  if (dialog.DoModal() == IDOK) {
    OpenURL(url, label);
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnHelpSphereSite()
{
  PromptURL("http://sphere.sourceforge.net/", "Open Sphere Website");
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnHelpSphericalSite()
{
  PromptURL("http://www.spheredev.org/forums/", "Open Spherical Forums");
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnHelpSphericalWiki()
{
  PromptURL("http://www.spheredev.org/", "Open Spherical Wiki (Spheriki)");
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnHelpGettingStarted()
{
  PromptURL("http://www.spheredev.org/wiki/Getting_started", "Open Getting Started Guide");
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnHelpFliksSite()
{
  PromptURL("http://sphere.sourceforge.net/flik/docs", "Open Flikky's Tutorial Site");
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnHelpAegisKnightsSite()
{
  PromptURL("http://aegisknight.org/sphere", "Open AegisKnight's Site");
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnHelpLocalDocumentation()
{
  std::string docdir = GetSphereDirectory() + "\\docs";
  if ((int)ShellExecute(NULL, "open", docdir.c_str(), 0, 0, SW_SHOW) <= 32)
    MessageBox("Could not open documentation directory.", "Local Documentation");
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnHelpLocalDocFunctions()
{
  std::string filestr = GetSphereDirectory() + "\\docs\\development\\api.txt";
  OpenDocumentWindow(GT_SCRIPTS, filestr.c_str());
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnHelpLocalChangelog()
{
  std::string filestr = GetSphereDirectory() + "\\docs\\changelog.txt";
  OpenDocumentWindow(GT_SCRIPTS, filestr.c_str());
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnHelpLocalTutorial()
{
  std::string filestr = GetSphereDirectory() + "\\docs\\development\\tutorial.txt";
  OpenDocumentWindow(GT_SCRIPTS, filestr.c_str());
}

////////////////////////////////////////////////////////////////////////////////

class CAboutDialog : public CDialog
{
public:
  CAboutDialog(std::string text);
  ~CAboutDialog();
private:
  std::string m_text;
private:
  BOOL OnInitDialog();
private:
  DECLARE_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CAboutDialog, CDialog)
END_MESSAGE_MAP()
CAboutDialog::CAboutDialog(std::string text)
: CDialog(IDD_ABOUT)
, m_text(text)
{
}
CAboutDialog::~CAboutDialog()
{
  m_text.resize(0);
}
BOOL
CAboutDialog::OnInitDialog()
{
  SetDlgItemText(IDC_ABOUT_TEXT, m_text.c_str());
  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnHelpAbout()
{
  char message[1024+512];
  sprintf(message,
    "Sphere Development Environment "
    SPHERE_VERSION "\r\n"
    "\r\n"
    "Chad Austin (c) 1999-2003\r\n"
    "\r\n"
	"Additional code by:\r\n"
    "Jacky Chong (Darklich)\r\n"
	"Brian Robb (Flik)\r\n"
    "Tung Nguyen (Tunginobi)\r\n"
	"Anatoli Steinmark (Kyuu)\r\n"
	"Nilton Castillo (FBnil)\r\n"
	"\r\n"
    "Icons by DRosen and Khadgar\r\n"
	"\r\n"
    "Special thanks to Alex Rosario (NeoLogiX)\r\n"
	"and many beta testers (you know who you are)\r\n"
    "Build Date: " __DATE__   "   Build Time: " __TIME__ "\r\n"
    "\r\n");
#ifdef I_SUCK
  sprintf (message + strlen(message), "Audiere: %s   ", audiere::GetVersion());
  sprintf (message + strlen(message), "Corona: %s\r\n", corona::GetVersion());
#ifdef USE_SIZECBAR
  sprintf (message + strlen(message), "CSizingControlBar: %s\r\n", SIZECBAR_VERSION);
#endif
  sprintf (message + strlen(message), "JavaScript: %s\r\n", JS_GetImplementationVersion());
  sprintf (message + strlen(message), "libmng: DLL %s - header %s\r\n", mng_version_text(), MNG_VERSION_TEXT);
#endif
  sprintf (message + strlen(message), "scintilla: 1.60\r\n");
#ifdef I_SUCK
  sprintf (message + strlen(message), "zlib: %s\r\n", zlibVersion());
#endif
  MessageBox(message, "About"); // At least all the text is being displayed.
  //CAboutDialog dialog(message);
  //dialog.EnableScrollBar(255);
  //dialog.DoModal();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg BOOL
CMainWindow::OnNeedText(UINT /*id*/, NMHDR* nmhdr, LRESULT* result)
{
  TOOLTIPTEXT* ttt = (TOOLTIPTEXT*)nmhdr;
  UINT id = nmhdr->idFrom;
  if (ttt->uFlags & TTF_IDISHWND) {
    id = ::GetDlgCtrlID((HWND)id);
  }
  switch (id) {
    case ID_FILE_NEW_PROJECT:
      if (m_NewFileType >= 0 && m_NewFileType < NUM_GROUP_TYPES)
      {
        if (m_NewFileType == GT_SCRIPTS)
          ttt->lpszText =  "New Script File";
        else
          ttt->lpszText =  "New Sphere File";
      }
      else
        ttt->lpszText = "New Sphere Project";
      break;
    case ID_PROJECT_RUNSPHERE:        ttt->lpszText = "Run Sphere";                     break;
    case IDI_IMAGETOOL_PENCIL:        ttt->lpszText = "Pencil";                         break;
    case IDI_IMAGETOOL_LINE:          ttt->lpszText = "Line";                           break;
    case IDI_IMAGETOOL_RECTANGLE:     ttt->lpszText = "Rectangle";                      break;
    case IDI_IMAGETOOL_CIRCLE:        ttt->lpszText = "Circle";                         break;
    case IDI_IMAGETOOL_ELLIPSE:       ttt->lpszText = "Ellipse";                        break;
    case IDI_IMAGETOOL_FILL_SHAPE:    ttt->lpszText = "Fill Shape";                     break;
    case IDI_IMAGETOOL_ANTIALIAS:     ttt->lpszText = "Antialias Shape";                break;
    case IDI_IMAGETOOL_FILL:          ttt->lpszText = "Fill Area";                      break;
    case IDI_IMAGETOOL_SELECTION:     ttt->lpszText = "Rectangular Selection";          break;
    case IDI_IMAGETOOL_FREESELECTION: ttt->lpszText = "Free Selection";                 break;
    case IDI_MAPTOOL_ZOOM_1X:         ttt->lpszText = "Map Zoom 1x";                    break;
    case IDI_MAPTOOL_ZOOM_2X:         ttt->lpszText = "Map Zoom 2x";                    break;
    case IDI_MAPTOOL_ZOOM_4X:         ttt->lpszText = "Map Zoom 4x";                    break;
    case IDI_MAPTOOL_ZOOM_8X:         ttt->lpszText = "Map Zoom 8x";                    break;
    case IDI_MAPTOOL_GRID_TILE:       ttt->lpszText = "Show Tile Grid";                 break;
    case IDI_MAPTOOL_1X1:             ttt->lpszText = "Pencil Tool (1x1)";              break;
    case IDI_MAPTOOL_3X3:             ttt->lpszText = "Pencil Tool (3x3)";              break;
    case IDI_MAPTOOL_5X5:             ttt->lpszText = "Pencil Tool (5x5)";              break;
    case IDI_MAPTOOL_SELECTTILE:      ttt->lpszText = "Select Tile";                    break;
    case IDI_MAPTOOL_FILLRECTAREA:    ttt->lpszText = "Fill Rectangular Area";          break;
    case IDI_MAPTOOL_FILLAREA:        ttt->lpszText = "Fill Area";                      break;
    case IDI_MAPTOOL_FILL_LAYER:      ttt->lpszText = "Fill Layer";                     break;
    case IDI_MAPTOOL_COPYAREA:        ttt->lpszText = "Copy Area";                      break;
    case IDI_MAPTOOL_PASTE:           ttt->lpszText = "Paste Area";                     break;
    case IDI_MAPTOOL_MOVEENTITY:      ttt->lpszText = "Move Entity";                    break;
    case IDI_MAPTOOL_OBS_SEGMENT:     ttt->lpszText = "Obstruction Segment";            break;
    case IDI_MAPTOOL_OBS_DELETE:      ttt->lpszText = "Delete Obstruction Segment";     break;
    case IDI_MAPTOOL_OBS_MOVE_PT:     ttt->lpszText = "Move Obstruction Segment Point"; break;
    case IDI_MAPTOOL_ZONEADD:         ttt->lpszText = "Add a Zone";                     break;
    case IDI_MAPTOOL_ZONEEDIT:        ttt->lpszText = "Edit a Zone";                    break;
    case IDI_MAPTOOL_ZONEMOVE:        ttt->lpszText = "Move a Zone";                    break;
    case IDI_MAPTOOL_ZONEDELETE:      ttt->lpszText = "Delete a Zone";                  break;
    case IDI_MAPTOOL_SCRIPT:          ttt->lpszText = "Execute a script";               break;
    default:                          ttt->lpszText = "";                               break;
  }
  *result = 0;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnUpdateOpenLastProject(CCmdUI* cmdui)
{
  if (Configuration::Get(KEY_LAST_PROJECT).length() == 0) {
    cmdui->Enable(FALSE);
  } else {
    cmdui->Enable(TRUE);
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnUpdateProjectCommand(CCmdUI* cmdui)
{
  cmdui->Enable(m_ProjectOpen ? TRUE : FALSE);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnUpdateFileCloseCommand(CCmdUI* cmdui)
{
  cmdui->Enable(FALSE);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnUpdateSaveCommand(CCmdUI* cmdui)
{
  CWnd* pWindow = MDIGetActive();
  if (pWindow == NULL)
    cmdui->Enable(FALSE);
  else
  {
    if (GetWindowLong(pWindow->m_hWnd, GWL_USERDATA) & WA_SAVEABLE)
      cmdui->Enable(TRUE);
    else
      cmdui->Enable(FALSE);
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnUpdateSaveAllCommand(CCmdUI* cmdui)
{
  bool can_save = false;
  // if any window is modified or unsaved, enable it!
  for (unsigned int i = 0; i < m_DocumentWindows.size(); i++) {
    CDocumentWindow* dw = m_DocumentWindows[i];
    if (dw->IsSaveable()) {
      CSaveableDocumentWindow* sdw = (CSaveableDocumentWindow*)dw;
      if (sdw->IsSaved() == false || sdw->IsModified() == true) {
        can_save = true;
        break;
      }
    }
  }
  cmdui->Enable(can_save);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnUpdateWindowCloseAll(CCmdUI* cmdui)
{
  cmdui->Enable(m_DocumentWindows.size() > 0);
}

////////////////////////////////////////////////////////////////////////////////

int
CMainWindow::GetNumImageToolsAllowed() const
{
  return m_NumImageToolsAllowed;
}

////////////////////////////////////////////////////////////////////////////////

afx_msg UINT
CMainWindow::IsImageFillShapeToolChecked()
{
  return m_ImageToolBar.GetToolBarCtrl().IsButtonChecked(IDI_IMAGETOOL_FILL_SHAPE);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg UINT
CMainWindow::IsImageAntialiasToolChecked()
{
  return m_ImageToolBar.GetToolBarCtrl().IsButtonChecked(IDI_IMAGETOOL_ANTIALIAS);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg UINT
CMainWindow::GetImageTool(int index)
{
  return m_SelectedImageTools[index];
}

////////////////////////////////////////////////////////////////////////////////

afx_msg UINT
CMainWindow::IsMapGridToolChecked()
{
  return m_MapToolBar.GetToolBarCtrl().IsButtonChecked(IDI_MAPTOOL_GRID_TILE);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg UINT
CMainWindow::GetMapGridTool(int index)
{
  return IDI_MAPTOOL_GRID_TILE;
}

////////////////////////////////////////////////////////////////////////////////

afx_msg UINT
CMainWindow::GetMapZoomTool(int index)
{
  UINT tools[] = {IDI_MAPTOOL_ZOOM_1X,
                  IDI_MAPTOOL_ZOOM_2X,
                  IDI_MAPTOOL_ZOOM_4X,
                  IDI_MAPTOOL_ZOOM_8X};

  int num_tools = sizeof(tools) / sizeof(*tools);
  UINT tool = IDI_MAPTOOL_ZOOM_1X;

  for (int i = 0; i < num_tools; i++) {
    if (m_MapToolBar.GetToolBarCtrl().IsButtonChecked(tools[i]) == TRUE) {
      tool = tools[i];
    }
  }

  return tool;
}

////////////////////////////////////////////////////////////////////////////////

afx_msg UINT
CMainWindow::GetMapTool(int index)
{
  UINT tools[] = {IDI_MAPTOOL_1X1,
                  IDI_MAPTOOL_3X3,
                  IDI_MAPTOOL_5X5,
                  IDI_MAPTOOL_SELECTTILE,
                  IDI_MAPTOOL_FILLRECTAREA,
                  IDI_MAPTOOL_FILLAREA,
                  IDI_MAPTOOL_FILL_LAYER,
                  IDI_MAPTOOL_COPYAREA,
                  IDI_MAPTOOL_PASTE,
                  IDI_MAPTOOL_MOVEENTITY,
                  IDI_MAPTOOL_OBS_SEGMENT,
                  IDI_MAPTOOL_OBS_DELETE,
                  IDI_MAPTOOL_OBS_MOVE_PT,
                  IDI_MAPTOOL_ZONEADD,
                  IDI_MAPTOOL_ZONEEDIT,
                  IDI_MAPTOOL_ZONEMOVE,
                  IDI_MAPTOOL_ZONEDELETE,
                  IDI_MAPTOOL_SCRIPT};
  int num_tools = sizeof(tools) / sizeof(*tools);
  UINT tool = IDI_MAPTOOL_1X1;
  for (int i = 0; i < num_tools; i++) {
    if (m_MapToolBar.GetToolBarCtrl().IsButtonChecked(tools[i]) == TRUE) {
      tool = tools[i];
    }
  }
  return tool;
}

//////////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::UpdateToolBars(WPARAM, LPARAM)
{
  CWnd* pWindow = MDIGetActive();
  if (pWindow != NULL)
  {
    long userdata = GetWindowLong(pWindow->m_hWnd, GWL_USERDATA);
    if (userdata & WA_DOCUMENT_WINDOW) {
      CDocumentWindow* dw = GetCurrentDocumentWindow();
      if (dw) {
        dw->OnToolChanged(GetImageTool(0),   0);
        dw->OnToolChanged(GetMapTool(0),     0);
        dw->OnToolChanged(GetMapZoomTool(0), 0);
        dw->OnToolChanged(GetMapGridTool(0), 0);
        dw->OnToolChanged(GetImageTool(1),   1);
        dw->OnToolChanged(GetMapTool(1),     1);
        dw->OnToolChanged(GetMapZoomTool(1), 1);
        dw->OnToolChanged(GetMapGridTool(1), 1);
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnUpdateImageCommand(CCmdUI* cmdui)
{
  BOOL enable = FALSE;
  CWnd* pWindow = MDIGetActive();
  if (pWindow != NULL)
  {
    long userdata = GetWindowLong(pWindow->m_hWnd, GWL_USERDATA);
    if (userdata & WA_IMAGE
     || userdata & WA_FONT
     || userdata & WA_SPRITESET
     || userdata & WA_MAP) {
      CDocumentWindow* dw = GetCurrentDocumentWindow();
      if (dw) {
        enable = dw->IsToolAvailable(IDI_IMAGETOOL_PENCIL);
      }
    }
  }
  cmdui->Enable(enable);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnImageFillOutToolChanged()
{

  const unsigned int id = GetCurrentMessage()->wParam;

  CToolBarCtrl& ctrl = m_ImageToolBar.GetToolBarCtrl();
  ctrl.CheckButton(id, !ctrl.IsButtonChecked(id));

  UpdateToolBars();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnImageAntialiasToolChanged()
{

  const unsigned int id = GetCurrentMessage()->wParam;

  CToolBarCtrl& ctrl = m_ImageToolBar.GetToolBarCtrl();
  ctrl.CheckButton(id, !ctrl.IsButtonChecked(id));

  UpdateToolBars();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnImageToolChanged()
{
  const unsigned int id = GetCurrentMessage()->wParam;
  const UINT tools[] = {IDI_IMAGETOOL_PENCIL,
                        IDI_IMAGETOOL_LINE,
                        IDI_IMAGETOOL_RECTANGLE,
                        IDI_IMAGETOOL_CIRCLE,
                        IDI_IMAGETOOL_ELLIPSE,
                        IDI_IMAGETOOL_FILL,
                        IDI_IMAGETOOL_SELECTION,
                        IDI_IMAGETOOL_FREESELECTION};

  const int num_tools = sizeof(tools) / sizeof(*tools);
  CToolBarCtrl& ctrl = m_ImageToolBar.GetToolBarCtrl();
  m_SelectedImageTools[0] = id;

  if (m_NumImageToolsAllowed == 1)
    m_SelectedImageTools[1] = id;

  for (int i = 0; i < num_tools; i++)
    ctrl.CheckButton(tools[i], (m_SelectedImageTools[0] == tools[i] || m_SelectedImageTools[1] == tools[i]) ? TRUE : FALSE);

  UpdateToolBars();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnUpdateMapCommand(CCmdUI* cmdui, UINT tool_id)
{
  BOOL enable = FALSE;
  CWnd* pWindow = MDIGetActive();
  if (pWindow != NULL) {
    if (GetWindowLong(pWindow->m_hWnd, GWL_USERDATA) & WA_MAP) {
      CDocumentWindow* dw = GetCurrentDocumentWindow();
      if (dw) {
        enable = dw->IsToolAvailable(tool_id);
      }
    }
  }
  cmdui->Enable(enable);
}

////////////////////////////////////////////////////////////////////////////////

#define MAKE_UPDATE_MAP_COMMAND(tool_id)                 \
afx_msg void                                             \
CMainWindow::OnUpdateMapCommand_##tool_id(CCmdUI* cmdui) \
{                                                        \
  OnUpdateMapCommand(cmdui, tool_id);                    \
}

MAKE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_ZOOM_1X)
MAKE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_ZOOM_2X)
MAKE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_ZOOM_4X)
MAKE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_ZOOM_8X)
MAKE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_GRID_TILE)
MAKE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_1X1)
MAKE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_3X3)
MAKE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_5X5)
MAKE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_SELECTTILE)
MAKE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_FILLRECTAREA)
MAKE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_FILLAREA)
MAKE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_FILL_LAYER)
MAKE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_COPYAREA)
MAKE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_PASTE)
MAKE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_MOVEENTITY)
MAKE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_OBS_SEGMENT)
MAKE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_OBS_DELETE)
MAKE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_OBS_MOVE_PT)
MAKE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_ZONEADD)
MAKE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_ZONEEDIT)
MAKE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_ZONEMOVE)
MAKE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_ZONEDELETE)
MAKE_UPDATE_MAP_COMMAND(IDI_MAPTOOL_SCRIPT)
#undef MAKE_UPDATE_MAP_COMMAND

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnMapGridToolChanged()
{

  const unsigned int id = GetCurrentMessage()->wParam;
  CToolBarCtrl& ctrl = m_MapToolBar.GetToolBarCtrl();

  ctrl.CheckButton(id, !ctrl.IsButtonChecked(id));
  UpdateToolBars();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnMapZoomToolChanged()
{

  const unsigned int id = GetCurrentMessage()->wParam;
  const UINT tools[] = {IDI_MAPTOOL_ZOOM_1X,
                        IDI_MAPTOOL_ZOOM_2X,
                        IDI_MAPTOOL_ZOOM_4X,
                        IDI_MAPTOOL_ZOOM_8X};

  int num_tools = sizeof(tools) / sizeof(*tools);
  CToolBarCtrl& ctrl = m_MapToolBar.GetToolBarCtrl();

  for (int i = 0; i < num_tools; i++)
    ctrl.CheckButton(tools[i], FALSE);

  ctrl.CheckButton(id, TRUE);
  UpdateToolBars();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnMapToolChanged()
{

  const unsigned int id = GetCurrentMessage()->wParam;
  const UINT tools[] = {IDI_MAPTOOL_1X1,
                        IDI_MAPTOOL_3X3,
                        IDI_MAPTOOL_5X5,
                        IDI_MAPTOOL_SELECTTILE,
                        IDI_MAPTOOL_FILLRECTAREA,
                        IDI_MAPTOOL_FILLAREA,
                        IDI_MAPTOOL_FILL_LAYER,
                        IDI_MAPTOOL_COPYAREA,
                        IDI_MAPTOOL_PASTE,
                        IDI_MAPTOOL_MOVEENTITY,
                        IDI_MAPTOOL_OBS_SEGMENT,
                        IDI_MAPTOOL_OBS_DELETE,
                        IDI_MAPTOOL_OBS_MOVE_PT,
                        IDI_MAPTOOL_ZONEADD,
                        IDI_MAPTOOL_ZONEEDIT,
                        IDI_MAPTOOL_ZONEMOVE,
                        IDI_MAPTOOL_ZONEDELETE,
                        IDI_MAPTOOL_SCRIPT};

  int num_tools = sizeof(tools) / sizeof(*tools);
  CToolBarCtrl& ctrl = m_MapToolBar.GetToolBarCtrl();

  for (int i = 0; i < num_tools; i++)
    ctrl.CheckButton(tools[i], FALSE);

  ctrl.CheckButton(id, TRUE);
  UpdateToolBars();
}

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg LRESULT
CMainWindow::OnInsertProjectFile(WPARAM wparam, LPARAM lparam)
{
  const char* path = (const char*)lparam;
  // figure out what filetype the file is
  for (int i = 0; i < NUM_GROUP_TYPES; i++ ) {
    std::vector<std::string> extensions;
    FTL.GetFileTypeExtensions(i, false, extensions);
    for (unsigned int j = 0; j < extensions.size(); j++) {
      if (extension_compare(path, extensions[j].c_str())) {
        InsertProjectFile(NULL, i, path);
        return 0;
      }
    }
  }
  return 0;
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg LRESULT
CMainWindow::OnRefreshProject(WPARAM /*wparam*/, LPARAM /*lparam*/)
{
  m_Project.RefreshItems();
  if (m_ProjectWindow) {
    m_ProjectWindow->Update();
  }
  return 0;
}
#endif

////////////////////////////////////////////////////////////////////////////////

afx_msg LRESULT
CMainWindow::OnCopyData(WPARAM wparam, LPARAM lparam)
{
	PCOPYDATASTRUCT pcds = (PCOPYDATASTRUCT) lparam;
	if (pcds->dwData == CD_OPEN_GAME_FILE)
	{
		const char* path = (const char*)pcds->lpData;
		OpenGameFile(path);
	}
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

afx_msg LRESULT
CMainWindow::OnDocumentWindowClosing(WPARAM wparam, LPARAM lparam)
{
  CDocumentWindow* window = (CDocumentWindow*)lparam;
  // remove window from list
  for (unsigned int i = 0; i < m_DocumentWindows.size(); i++)
    if (m_DocumentWindows[i] == window)
    {
      m_DocumentWindows.erase(m_DocumentWindows.begin() + i);
      return 0;
    }
  if (window == s_JS_Console) {
    s_JS_Console = NULL;
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

afx_msg LRESULT
CMainWindow::OnSetChildMenu(WPARAM wparam, LPARAM lparam)
{
  m_ChildMenuResource = wparam;
  UpdateMenu();
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

afx_msg LRESULT
CMainWindow::OnClearChildMenu(WPARAM wparam, LPARAM lparam)
{
  m_ChildMenuResource = -1;
  UpdateMenu();
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

void
CMainWindow::OnUpdatePaletteMenu(CCmdUI* cmdui)
{
	if (cmdui->m_pSubMenu == NULL) return;
	// remove the old palette menu
	while(cmdui->m_pSubMenu->GetMenuItemCount() > 0)
	{
		cmdui->m_pSubMenu->RemoveMenu(0, MF_BYPOSITION);
	}
  // add a new one
  bool empty = true;
  // get the active MDI document window
  CWnd* child = MDIGetActive();
  if (child) {
    if (GetWindowLong(child->m_hWnd, GWL_USERDATA) & WA_DOCUMENT_WINDOW) {
      CDocumentWindow* window = (CDocumentWindow*)child;
      // go through each palette and add it to the menu
      for (int i = 0; i < window->GetNumPalettes(); i++) {
        CPaletteWindow* palette = window->GetPalette(i);
        CString title;
        palette->GetWindowText(title);
        // check the palette if it's visible
        UINT visible = 0;
#ifdef USE_SIZECBAR
				if (palette->IsVisible()) {
#else
        if (palette->IsWindowVisible()) {
#endif
          visible = MF_CHECKED;
        }
        cmdui->m_pSubMenu->AppendMenu(visible, PALETTE_COMMAND + i, title);
      }
      empty = (window->GetNumPalettes() == 0);
    }
  }
  // if nothing was added, make an empty palette menu
  if (empty) {
    cmdui->m_pSubMenu->AppendMenu(0, ID_VIEW_PALETTES, "...");
    cmdui->m_pSubMenu->EnableMenuItem(0, MF_BYPOSITION | MF_GRAYED);
  }
  return;
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnViewPalette(UINT id)
{
  ViewPalette(id - PALETTE_COMMAND);
}

////////////////////////////////////////////////////////////////////////////////

void
CMainWindow::ViewPalette(int paletteNum)
{
  // toggle the visibility of the palette
  CWnd* child = MDIGetActive();
  if (child) {
    if (GetWindowLong(child->m_hWnd, GWL_USERDATA) & WA_DOCUMENT_WINDOW) {
      CDocumentWindow* window = (CDocumentWindow*)child;
      // find the palette to toggle
      CPaletteWindow* palette = window->GetPalette(paletteNum);
#ifdef USE_SIZECBAR
			palette->ShowPalette(!palette->IsVisible());
#else
      palette->ShowPalette(!palette->IsWindowVisible());
#endif
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnCDAudio(UINT id)
{
  CDAudio(id - CDAUDIO_COMMAND);
}

////////////////////////////////////////////////////////////////////////////////

void
CMainWindow::CDAudio(int cdNum)
{
  std::vector<std::string> devices;
  audiere::EnumerateCDDevices(devices);
  for (unsigned int j = 0; j < devices.size(); j++) {
    if (j == cdNum && devices[j].size() > 0) {
      std::string title = "cda://" + devices[j];
      if (title.find(":", title.length() - 1) == title.length() - 1)
        title[title.find(":", title.length() - 1)] = '\0';
      //MessageBox(title.c_str(), NULL, MB_OK);
      OpenDocumentWindow(GT_SOUNDS, title.c_str());
      break;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnUpdateViewProject(CCmdUI* cmdui)
{
#ifdef FLOATING_PALETTE_WINDOW
	cmdui->Enable(m_ProjectOpen);
#else
  cmdui->Enable(FALSE);
#endif
	if (m_ProjectOpen)
	{
#ifdef FLOATING_PALETTE_WINDOW
	cmdui->SetCheck(m_ProjectWindow->IsVisible() ? 1 : 0);
#else
	cmdui->SetCheck(m_ProjectWindow->IsWindowVisible() ? 1 : 0);
#endif
	}
}
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef I_SUCK
afx_msg void
CMainWindow::OnViewProject()
{
#ifdef FLOATING_PALETTE_WINDOW
#ifdef USE_SIZECBAR
	m_ProjectWindow->ShowPalette(!m_ProjectWindow->IsVisible());
#else
	m_ProjectWindow->ShowPalette(!m_ProjectWindow->IsWindowVisible());
#endif
#endif
}
#endif

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnZoomIn()
{
  CDocumentWindow* dw = GetCurrentDocumentWindow();
  if (dw != NULL) {
    dw->SendMessage(ID_FILE_ZOOM_IN, 0, 0);
  }
  else {
    //MessageBox("MainWindow::ZoomIn");
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnZoomOut()
{
  CDocumentWindow* dw = GetCurrentDocumentWindow();
  if (dw != NULL) {
    dw->SendMessage(ID_FILE_ZOOM_OUT, 0, 0);
  }
  else {
    //MessageBox("MainWindow::ZoomOut");
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnPaste()
{
  CDocumentWindow* dw = GetCurrentDocumentWindow();
  if (dw != NULL) {
    dw->SendMessage(ID_FILE_PASTE, 0, 0);
  }
  else {
    CDocumentWindow* dw = NULL;
    if (IsClipboardFormatAvailable(CF_TEXT)) {
      dw = new CScriptWindow(NULL, true);
    }
#ifdef I_SUCK
    else
    if (IsClipboardFormatAvailable(CF_BITMAP)) {
      dw = new CImageWindow(NULL, true);
    }
#endif
    if (dw) {
      m_DocumentWindows.push_back(dw);
      UpdateToolBars();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnCopy()
{
  CDocumentWindow* dw = GetCurrentDocumentWindow();
  if (dw != NULL) {
    dw->SendMessage(ID_FILE_COPY, 0, 0);
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnUpdateCopy(CCmdUI* cmdui)
{
  BOOL enable = FALSE;
  CDocumentWindow* dw = GetCurrentDocumentWindow();
  if (dw) {
    enable = dw->IsToolAvailable(ID_FILE_COPY);
  }
  cmdui->Enable(enable);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnUpdatePaste(CCmdUI* cmdui)
{
  BOOL enable = FALSE;
  CDocumentWindow* dw = GetCurrentDocumentWindow();
  if (dw) {
    enable = dw->IsToolAvailable(ID_FILE_PASTE);
  }
  else {
    if (IsClipboardFormatAvailable(CF_TEXT)) {
      enable = TRUE;
    }
#ifdef I_SUCK
    else
    if (IsClipboardFormatAvailable(CF_BITMAP)) {
      enable = TRUE;
    }
#endif
  }
  cmdui->Enable(enable);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnUndo()
{
  CDocumentWindow* dw = GetCurrentDocumentWindow();
  if (dw != NULL) {
    dw->SendMessage(ID_FILE_UNDO, 0, 0);
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnUpdateUndo(CCmdUI* cmdui)
{
  BOOL enable = FALSE;
  CDocumentWindow* dw = GetCurrentDocumentWindow();
  if (dw) {
    enable = dw->IsToolAvailable(ID_FILE_UNDO);
  }
  cmdui->Enable(enable);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnRedo()
{
  CDocumentWindow* dw = GetCurrentDocumentWindow();
  if (dw != NULL) {
    dw->SendMessage(ID_FILE_REDO, 0, 0);
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CMainWindow::OnUpdateRedo(CCmdUI* cmdui)
{
  BOOL enable = FALSE;
  CDocumentWindow* dw = GetCurrentDocumentWindow();
  if (dw) {
    enable = dw->IsToolAvailable(ID_FILE_REDO);
  }
  cmdui->Enable(enable);
}

////////////////////////////////////////////////////////////////////////////////
