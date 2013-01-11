#pragma warning(disable : 4786)  // identifier too long

#include "ProjectWindow.hpp"
#include "MainWindow.hpp"
#include "Project.hpp"
#include "FileDialogs.hpp"
#include "Scripting.hpp"
#include "../common/types.h"
#include "resource.h"

#include "Configuration.hpp"
#include "keys.hpp"
#include "FileTypes.hpp"

#include "translate.hpp"
#include "../common/system.hpp"
#include "../common/strcmp_ci.hpp"

static const int TreeID = 9865;
static const int tiGameSettings = 100;
static const int tiMaps         = 101;
static const int tiSpritesets   = 102;
static const int tiSounds       = 103;
static const int tiScripts      = 104;
static const int tiImages       = 105;
static const int tiAnimations   = 106;
static const int tiWindowStyles = 107;
static const int tiFonts        = 108;

static const int IDI_FILETYPE_BASE = 4;

BEGIN_MESSAGE_MAP(CProjectWindow, BaseProjectWindow)

  ON_WM_SYSCOMMAND()
  ON_WM_SIZE()
  ON_WM_SETFOCUS()
  ON_WM_DROPFILES()

  ON_COMMAND(ID_PROJECTGROUP_NEW, OnProjectGroupNew)
  ON_COMMAND(ID_PROJECTGROUP_INSERT, OnProjectGroupInsert)

  ON_COMMAND(ID_PROJECTITEM_OPEN,   OnProjectItemOpen)
  ON_COMMAND(ID_PROJECTITEM_DELETE, OnProjectItemDelete)

  ON_NOTIFY(TVN_KEYDOWN, TreeID, OnKeyDown)
  ON_NOTIFY(NM_DBLCLK,   TreeID, OnDoubleClick)
  ON_NOTIFY(NM_RCLICK,   TreeID, OnRightClick)

END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////

CProjectWindow::CProjectWindow(CMainWindow* main_window, CProject* project)
: m_MainWindow(main_window)
, m_Project(project)
#ifdef FLOATING_PROJECT_WINDOW
, CPaletteWindow(NULL, "Project", Configuration::Get(KEY_PROJECT_RECT), true)
#endif
{
}

////////////////////////////////////////////////////////////////////////////////

BOOL
CProjectWindow::Create()
{
#ifndef FLOATING_PROJECT_WINDOW
  char szProjectName[512];
  sprintf(szProjectName, "Project [%s]", m_Project->GetGameSubDirectory());

  // create the window
  CMDIChildWnd::Create(
      AfxRegisterWndClass(CS_NOCLOSE, NULL, NULL, AfxGetApp()->LoadIcon(IDI_PROJECT)),
      szProjectName,
      WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW);
#endif
  
  // create widgets
  m_TreeControl.Create(
      WS_VISIBLE | WS_CHILD | TVS_SHOWSELALWAYS | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_DISABLEDRAGDROP,
      CRect(0, 0, 0, 0),
      this,
      TreeID);
  //m_TreeControl.SetFocus();

  HIMAGELIST image_list = ImageList_Create(16, 16, ILC_COLOR8 | ILC_MASK, 3, 1);
  ImageList_AddIcon(image_list, AfxGetApp()->LoadIcon(IDI_PV_GAME_SETTINGS));
  ImageList_AddIcon(image_list, AfxGetApp()->LoadIcon(IDI_PV_CLOSED_FOLDER));
  ImageList_AddIcon(image_list, AfxGetApp()->LoadIcon(IDI_PV_OPEN_FOLDER));  
  ImageList_AddIcon(image_list, AfxGetApp()->LoadIcon(IDI_UNKNOWN));

  // add filetype icons
  ImageList_AddIcon(image_list, AfxGetApp()->LoadIcon(IDI_MAP));
  ImageList_AddIcon(image_list, AfxGetApp()->LoadIcon(IDI_SPRITESET));
  ImageList_AddIcon(image_list, AfxGetApp()->LoadIcon(IDI_SCRIPT));
  ImageList_AddIcon(image_list, AfxGetApp()->LoadIcon(IDI_SOUND));
  ImageList_AddIcon(image_list, AfxGetApp()->LoadIcon(IDI_FONT));
  ImageList_AddIcon(image_list, AfxGetApp()->LoadIcon(IDI_WINDOWSTYLE));
  ImageList_AddIcon(image_list, AfxGetApp()->LoadIcon(IDI_IMAGE));
  ImageList_AddIcon(image_list, AfxGetApp()->LoadIcon(IDI_ANIMATION));
  ImageList_AddIcon(image_list, AfxGetApp()->LoadIcon(IDI_TILESET));
  
  m_TreeControl.SetImageList(CImageList::FromHandle(image_list), TVSIL_NORMAL);

  // make sure they are in the right place
  RECT rect;
  GetClientRect(&rect);
  OnSize(0, rect.right, rect.bottom);

  Update();
  DragAcceptFiles();
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////

BOOL
CProjectWindow::DestroyWindow()
{
  RECT rect;
  GetWindowRect(&rect);
  Configuration::Set(KEY_PROJECT_RECT, rect);

  m_TreeControl.DestroyWindow();
#ifdef FLOATING_PROJECT_WINDOW
  return CPaletteWindow::DestroyWindow();
#else
  return CMDIChildWnd::DestroyWindow();
#endif
}

////////////////////////////////////////////////////////////////////////////////

static int GetFolderType(CTreeCtrl& m_TreeControl, const HTREEITEM item)
{
  HTREEITEM parent = item;
  int type = 0;
  std::string folder = "";

  // these lines are commented out so that the
  // non-root subfolders do not have the right click menu
  //do {
    if (parent)
      folder = m_TreeControl.GetItemText(parent);
  //  parent = m_TreeControl.GetParentItem(parent);
  //} while (parent != NULL);

  if (folder == "maps")         type = tiMaps;
  if (folder == "spritesets")   type = tiSpritesets;
  if (folder == "scripts")      type = tiScripts;
  if (folder == "fonts")        type = tiFonts;
  if (folder == "sounds")       type = tiSounds;
  if (folder == "windowstyles") type = tiWindowStyles;
  if (folder == "images")       type = tiImages;
  if (folder == "animations")   type = tiAnimations;

  return type;
}

////////////////////////////////////////////////////////////////////////////////

static bool IsTreeItemFolder(CTreeCtrl& m_TreeControl, HTREEITEM item)
{
  int image = 0, selected = 0;
  m_TreeControl.GetItemImage(item, image, selected);
  return (image == 1 && selected == 2) ? true : false;
}

////////////////////////////////////////////////////////////////////////////////

static bool IsTreeItemFile(CTreeCtrl& m_TreeControl, HTREEITEM item) {
  HTREEITEM root = m_TreeControl.GetParentItem(item);
  HTREEITEM child = m_TreeControl.GetChildItem(item);

  return (root != NULL && child == NULL);
}

////////////////////////////////////////////////////////////////////////////////

static std::string GetTreeItemPathName(CTreeCtrl& m_TreeControl, HTREEITEM item)
{
  std::string pathname = m_TreeControl.GetItemText(item);
  HTREEITEM parent = m_TreeControl.GetParentItem(item);

  do {
    if (parent) {
      std::string temp = pathname;
      pathname = m_TreeControl.GetItemText(parent);
      pathname += "\\";
      pathname += temp;
    }
    parent = m_TreeControl.GetParentItem(parent);
  } while (parent != NULL);

  return pathname;
}

////////////////////////////////////////////////////////////////////////////////

static void UpdateTreeStructure(CTreeCtrl& m_TreeControl, HTREEITEM root);

static void UpdateTreeStructure(CTreeCtrl& m_TreeControl, HTREEITEM root) {
  const int FOLDER_CLOSED = 1;
  const int FOLDER_OPENED = 2;

  std::vector<std::string> folderlist = GetFolderList("*");
  for (unsigned int i = 0; i < folderlist.size(); i++) {
    if (!strcmp(folderlist[i].c_str(), ".")
        || !strcmp(folderlist[i].c_str(), "..")) {
      continue;
    }

    // insert folder
    HTREEITEM item = m_TreeControl.InsertItem(
      TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE,
      folderlist[i].c_str(),
      FOLDER_CLOSED, // image,
      FOLDER_OPENED, // selected_image,
      0,
      0,
      0, // item,
      root == NULL ? TVI_ROOT : root,
      TVI_SORT
    );

    // insert files into that folder
    char directory[MAX_PATH] = {0};
    GetCurrentDirectory(MAX_PATH, directory);
    SetCurrentDirectory(folderlist[i].c_str());
    std::vector<std::string> filelist = GetFileList("*");

    for (unsigned int j = 0; j < filelist.size(); j++)
    {
      // don't show thumbs.db files
      if (filelist[j].size() >= strlen("thumbs.db") && strcmp_ci(filelist[j].c_str(), "thumbs.db") == 0)
        continue;

      // don't show folder.htt files
      if (filelist[j].size() >= strlen("folder.htt") && strcmp_ci(filelist[j].c_str(), "folder.htt") == 0)
        continue;

      // don't show desktop.ini files
      if (filelist[j].size() >= strlen("desktop.ini") && strcmp_ci(filelist[j].c_str(), "desktop.ini") == 0)
        continue;

      // don't show .pyc files
      if (filelist[j].size() >= 4 && filelist[j].rfind(".pyc") == filelist[j].size() - 4) {
        continue;
      }

      // don't show .class files
      if (filelist[j].size() >= 6 && filelist[j].rfind(".class") == filelist[j].size() - 6) {
        continue;
      }

      int image_index = IDI_FILETYPE_BASE - 1; // Unknown icon type
      std::string filename = filelist[j];
      for (int file_type = 0; file_type < FTL.GetNumFileTypes(); file_type++) {
        std::vector<std::string> extensions;
        FTL.GetFileTypeExtensions(file_type, false, extensions);

        for (unsigned int i = 0; i < extensions.size(); i++) {
          if (filename.rfind(extensions[i]) == filename.size() - extensions[i].size()) {
            image_index = IDI_FILETYPE_BASE + file_type;
            file_type = FTL.GetNumFileTypes();
            break;
          }
        }
      }

      m_TreeControl.InsertItem(
        TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE,
        filelist[j].c_str(),
        image_index, image_index, //image, image,
        0, 0, 0, item, TVI_SORT
      );
    }

    filelist.clear();
    if (GetFolderList("*").size() > 0)
      UpdateTreeStructure(m_TreeControl, item);

    SetCurrentDirectory(directory);
  }
}

////////////////////////////////////////////////////////////////////////////////

void
CProjectWindow::Update()
{
  m_Project->RefreshItems();
  m_TreeControl.SetRedraw(FALSE);
  // todo remember scroll position and restore it at the end of function
  // int pos = m_TreeControl.GetScrollPos(SBS_VERT);

  // todo, remember tree states properly
  // currently we just remember one folder deep
  std::vector<CString> Names;
  std::vector<DWORD> States;

  HTREEITEM item = m_TreeControl.GetRootItem();
  do {
    item = m_TreeControl.GetNextItem(item, TVGN_NEXT);
    if (item != NULL) {
      DWORD state = m_TreeControl.GetItemState(item, TVIS_EXPANDED);
      CString text = m_TreeControl.GetItemText(item);
      Names.push_back(text);
      States.push_back(state);
    }
  } while (item != NULL);
  m_TreeControl.DeleteAllItems();

  char last_directory[MAX_PATH] = {0};
  GetCurrentDirectory(MAX_PATH, last_directory);
  if (SetCurrentDirectory(m_Project->GetDirectory()) != 0)
    UpdateTreeStructure(m_TreeControl, NULL);
  SetCurrentDirectory(last_directory);

  m_TreeControl.InsertItem(
    TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE,
    "Game Settings", // text,
    0, // image
    0, // selected_image
    0,
    0,
    tiGameSettings,
    TVI_ROOT,
    TVI_FIRST
  );

  // todo, restore tree states properly
  // currently we just restore one folder deep
  item = m_TreeControl.GetRootItem();
  do {
    item = m_TreeControl.GetNextItem(item, TVGN_NEXT);
    if (item != NULL) {
      CString text = m_TreeControl.GetItemText(item);
      for (unsigned int i = 0; i < Names.size(); i++) {
        if (text == Names[i]) {
          m_TreeControl.SetItemState(item, States[i], States[i]);
          break;
        }
      }
    }
  } while (item != NULL);

  // m_TreeControl.SetScrollPos(SBS_VERT, pos, FALSE);
	m_TreeControl.SetRedraw(TRUE);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CProjectWindow::OnSysCommand(UINT id, LPARAM param)
{
  if (id == SC_CLOSE) {
    if (!IsZoomed()) {
      // do not close it damnit!
      // and oh, Ctrl-F4 calls close MDI window, if ya don't know.
      ShowWindow(SW_HIDE);
    }
	} else {
    CWnd::OnSysCommand(id, param);
	}
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CProjectWindow::OnSize(UINT type, int cx, int cy)
{
  if (m_TreeControl.m_hWnd)
    m_TreeControl.MoveWindow(CRect(0, 0, cx, cy), TRUE);

#ifdef FLOATING_PROJECT_WINDOW
  CPaletteWindow::OnSize(type, cx, cy);
#else
  CMDIChildWnd::OnSize(type, cx, cy); 
#endif
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CProjectWindow::OnSetFocus(CWnd* old_window)
{
  if (old_window != NULL && old_window->m_hWnd == m_hWnd && m_TreeControl.m_hWnd)
    m_TreeControl.SetFocus();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CProjectWindow::OnDropFiles(HDROP drop_info)
{
  if (!drop_info)
    return;

  UINT num_files = DragQueryFile(drop_info, 0xFFFFFFFF, NULL, 0);

  // add all files to the project
  for (unsigned int i = 0; i < num_files; i++) {
    char path[MAX_PATH];
    DragQueryFile(drop_info, i, path, MAX_PATH);

    // send message immediately (so path doesn't go out of scope)
    ::SendMessage(m_MainWindow->m_hWnd, WM_INSERT_PROJECT_FILE, 0, (LPARAM)path);
  }

  DragFinish(drop_info);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CProjectWindow::OnProjectGroupInsert()
{
  HTREEITEM item = m_TreeControl.GetSelectedItem();
  if (!IsTreeItemFolder(m_TreeControl, item))
    return;

  switch (GetFolderType(m_TreeControl, item))
  {
    case tiMaps:         ::PostMessage(m_MainWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(ID_PROJECT_INSERT_MAP,         0), 0); break;
    case tiSpritesets:   ::PostMessage(m_MainWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(ID_PROJECT_INSERT_SPRITESET,   0), 0); break;
    case tiScripts:      ::PostMessage(m_MainWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(ID_PROJECT_INSERT_SCRIPT,      0), 0); break;
    case tiSounds:       ::PostMessage(m_MainWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(ID_PROJECT_INSERT_SOUND,       0), 0); break;
    case tiFonts:        ::PostMessage(m_MainWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(ID_PROJECT_INSERT_FONT,        0), 0); break;
    case tiWindowStyles: ::PostMessage(m_MainWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(ID_PROJECT_INSERT_WINDOWSTYLE, 0), 0); break;
    case tiImages:       ::PostMessage(m_MainWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(ID_PROJECT_INSERT_IMAGE,       0), 0); break;
    case tiAnimations:   ::PostMessage(m_MainWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(ID_PROJECT_INSERT_ANIMATION,   0), 0); break;
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CProjectWindow::OnProjectGroupNew()
{
  HTREEITEM item = m_TreeControl.GetSelectedItem();

  if (!IsTreeItemFolder(m_TreeControl, item))
    return;

  switch (GetFolderType(m_TreeControl, item))
  {
    case tiMaps:         ::PostMessage(m_MainWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(ID_FILE_NEW_MAP,         0), 0); break;
    case tiSpritesets:   ::PostMessage(m_MainWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(ID_FILE_NEW_SPRITESET,   0), 0); break;
    case tiScripts:      ::PostMessage(m_MainWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(ID_FILE_NEW_SCRIPT,      0), 0); break;
    case tiFonts:        ::PostMessage(m_MainWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(ID_FILE_NEW_FONT,        0), 0); break;
    case tiWindowStyles: ::PostMessage(m_MainWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(ID_FILE_NEW_WINDOWSTYLE, 0), 0); break;
    case tiImages:       ::PostMessage(m_MainWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(ID_FILE_NEW_IMAGE,       0), 0); break;
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CProjectWindow::OnProjectItemOpen()
{
  OnDoubleClick(NULL, NULL);
}

////////////////////////////////////////////////////////////////////////////////

/*
bool RecycleFile(const char* filename)
{
  bool removed = false;
  char directory[MAX_PATH];

  SetCurrentDirectory("c:\recycled");
  if (GetCurrentDirectory(MAX_PATH, directory)
    && true) {
    removed = !MoveFile(filename, "removed.png");
    SetCurrentDirectory(directory);
  }
  else {
    removed = !DeleteFile(filename);
  }

  return removed;
}
*/

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CProjectWindow::OnProjectItemDelete()
{
  HTREEITEM item = m_TreeControl.GetSelectedItem();

  if (item == NULL)
    return;

  // check to see if user clicked on an item in a folder
  if (!IsTreeItemFile(m_TreeControl, item))
    return;

  char filename[MAX_PATH];
  strcpy(filename, m_Project->GetDirectory());
  strcat(filename, "\\");
  strcat(filename, GetTreeItemPathName(m_TreeControl, item).c_str());

  std::string message = "This will permanently delete the file: \n\""
      + std::string(filename)
      + "\"\nAre you sure you want to continue?";
  if (MessageBox(message.c_str(), "Delete Project Item", MB_YESNO) == IDNO)
    return;

  // TODO: Move to recycle bin rather than delete
  if (!DeleteFile(filename))
    MessageBox("Error: Could not delete file");

  m_Project->RefreshItems();
  Update();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CProjectWindow::OnKeyDown(NMHDR* notify, LRESULT* result)
{
  NMTVKEYDOWN* key = (NMTVKEYDOWN*)notify;
  if (!key || !result) return;

  switch (key->wVKey)
  {
    case VK_RETURN:
      __OnDoubleClick__(true);
      *result = 0;
    break;

    case VK_APPS:
      __OnRightClick__();
      *result = 0;
    break;

    case VK_INSERT:
      OnProjectGroupInsert();
      *result = 0;
    break;
  }
}

////////////////////////////////////////////////////////////////////////////////

void
CProjectWindow::__OnDoubleClick__(bool allow_expand)
{
  HTREEITEM item = m_TreeControl.GetSelectedItem();

  if (item == NULL) return;

  switch (m_TreeControl.GetItemData(item))
  {
#ifdef I_SUCK
    case tiGameSettings:
      // open game window
      if (m_MainWindow) m_MainWindow->OpenGameSettings();
    break;
#endif

    default: {
      if (IsTreeItemFolder(m_TreeControl, item)) {
        if (allow_expand)
          m_TreeControl.Expand(item, TVE_TOGGLE);
        return;
      }

      // check to see if user clicked on an item in a folder
      if (!IsTreeItemFile(m_TreeControl, item))
        return;

      char szFilename[MAX_PATH];
      strcpy(szFilename, m_Project->GetDirectory());
      strcat(szFilename, "\\");
      strcat(szFilename, GetTreeItemPathName(m_TreeControl, item).c_str());

      if (m_MainWindow) m_MainWindow->OpenGameFile(szFilename);
      break;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CProjectWindow::OnDoubleClick(NMHDR* /*notify*/, LRESULT* /*result*/)
{
  __OnDoubleClick__(false);
}

////////////////////////////////////////////////////////////////////////////////

void
CProjectWindow::__OnRightClick__()
{
  HTREEITEM item = m_TreeControl.GetSelectedItem();
  if (item == NULL)
    return;

  // if the user right clicked on a folder
  if (IsTreeItemFolder(m_TreeControl, item))
  {
    // if the user right-clicked on a group, open the "group" popup menu
    switch (GetFolderType(m_TreeControl, item))
    {
      case tiMaps:
      case tiSpritesets:
      case tiScripts:
      case tiSounds:
      case tiFonts:
      case tiWindowStyles:
      case tiImages:
      case tiAnimations:
      {
        CMenu menu;
        menu.LoadMenu(IDR_PROJECTGROUP);

        POINT Point;
        GetCursorPos(&Point);

        CMenu* sub_menu = menu.GetSubMenu(0);
        if (sub_menu) {
          TranslateMenu(sub_menu->GetSafeHmenu());
          sub_menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, Point.x, Point.y, this);
        }
      
        menu.DestroyMenu();
        return;
      }
    }

    return;
  }

  if (IsTreeItemFile(m_TreeControl, item)) {
    CMenu menu;
    menu.LoadMenu(IDR_PROJECTITEM);

    int image = 0, selected = 0;
    m_TreeControl.GetItemImage(item, image, selected);
    if (image == IDI_FILETYPE_BASE - 1 && selected == IDI_FILETYPE_BASE - 1) {
      // this should work !! but doesn't
      menu.EnableMenuItem(ID_PROJECTITEM_OPEN, MF_BYCOMMAND | MF_DISABLED);
    }

    // get the mouse coordinates
    POINT Point;
    GetCursorPos(&Point);

    CMenu* sub_menu = menu.GetSubMenu(0);
    if (sub_menu) {
      TranslateMenu(sub_menu->GetSafeHmenu());
      sub_menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, Point.x, Point.y, this);
    }

    menu.DestroyMenu();
  }
}

afx_msg void
CProjectWindow::OnRightClick(NMHDR* /*notify*/, LRESULT* /*result*/)
{
  SelectItemAtCursor();
  __OnRightClick__();
}

////////////////////////////////////////////////////////////////////////////////

void
CProjectWindow::SelectItemAtCursor()
{
  // select the item the user right-clicked on
  POINT Point;
  GetCursorPos(&Point);
  ::ScreenToClient(m_TreeControl.m_hWnd, &Point);

  UINT uFlags;
  HTREEITEM item = m_TreeControl.HitTest(Point, &uFlags);

  // if we're on the item label, select it
  if (uFlags & TVHT_ONITEM)
    m_TreeControl.SelectItem(item);
  else
    m_TreeControl.SelectItem(NULL);
}

////////////////////////////////////////////////////////////////////////////////
