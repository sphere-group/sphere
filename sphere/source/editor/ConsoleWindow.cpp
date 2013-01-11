#pragma warning(disable : 4786)

#include <Scintilla.h>
#include <SciLexer.h>
#include "ConsoleWindow.hpp"
#include "Editor.hpp"
//#include "FileSystem.hpp"
#include "Configuration.hpp"
#include "Keys.hpp"
#include "resource.h"
#include "FileDialogs.hpp"
#include "StringDialog.hpp"
#include <afxdlgs.h>
#include "Scripting.hpp"

////////////////////////////////////////////////////////////////////////////////

static const int ID_EDIT = 900;
static const int IRC_TIMER = 9001;
static const UINT s_FindReplaceMessage = ::RegisterWindowMessage(FINDMSGSTRING);

////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CConsoleWindow, CDocumentWindow)
  ON_WM_SIZE()
  ON_WM_SETFOCUS()
  ON_WM_CONTEXTMENU()
  ON_NOTIFY(SCN_UPDATEUI,         ID_EDIT, OnPosChanged)
  ON_NOTIFY(SCN_CHARADDED,        ID_EDIT, OnCharAdded)
  ON_COMMAND(ID_FILE_COPY,  OnCopy)
  ON_COMMAND(ID_FILE_PASTE, OnPaste)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////

static CConsoleWindow* s_ConsoleWindow = NULL;
CConsoleWindow::CConsoleWindow()
: CDocumentWindow("Javascript Console", -1, CSize(400, 100))
, m_Created(false)
, m_script_running(false)
{
  s_ConsoleWindow = this;
  if (!Create()) {
    return;
  }
}

////////////////////////////////////////////////////////////////////////////////

CConsoleWindow::~CConsoleWindow()
{
  m_Scripter.Destroy();
  s_ConsoleWindow = NULL;
}

////////////////////////////////////////////////////////////////////////////////

bool
CConsoleWindow::Create()
{
  static HINSTANCE scintilla = NULL;
  if (!scintilla) {
    scintilla = LoadLibrary("SciLexer.dll");
    if (!scintilla) {
      ::MessageBox(
        NULL,
        "Could not load Scintilla component (SciLexer.dll)",
        "IRC Window",
        MB_OK | MB_ICONERROR);
      delete this;
      return false;
    }
  }
  // create the child window
  CDocumentWindow::Create(
    AfxRegisterWndClass(0, LoadCursor(NULL, IDC_ARROW), NULL, AfxGetApp()->LoadIcon(IDI_SCRIPT)));
  int bar = 35;
  RECT Rect;
  GetClientRect(&Rect);
  int __cx__ = Rect.right - Rect.left;
  int __cy__ = Rect.bottom - Rect.top;
  // creates the script view
  __m_Editor__ = ::CreateWindow(
    "Scintilla",
    "Source",
    WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_CLIPCHILDREN,
    0,        0,
    __cx__, __cy__ - bar,
    m_hWnd,
    (HMENU)ID_EDIT,
    AfxGetApp()->m_hInstance,
    0);
  __m_InputBar__ = ::CreateWindow(
    "Scintilla",
    "Source",
    WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_CLIPCHILDREN | WS_BORDER,
    0,        __cy__ - bar,
    __cx__,       bar,
    m_hWnd,
    (HMENU)ID_EDIT,
    AfxGetApp()->m_hInstance,
    0);
  Initialize();
  ::ShowWindow(__m_Editor__, SW_SHOW);
  ::UpdateWindow(__m_Editor__);
  ::ShowWindow(__m_InputBar__, SW_SHOW);
  ::UpdateWindow(__m_InputBar__);
  m_Created = true;
  // update the size of the view
  OnSize(0, Rect.right - Rect.left, Rect.bottom - Rect.top);
  SendEditor(SCI_SETUNDOCOLLECTION, 0);
  SendEditor(SCI_EMPTYUNDOBUFFER);
  SendEditor(SCI_SETREADONLY, 1);
  // give the view focus
  ::SetFocus(__m_InputBar__);
///////////
  m_Scripter.Create();
/////////
 
  return true;
}

////////////////////////////////////////////////////////////////////////////////

void
CConsoleWindow::SetScriptStyles()
{
  static const COLORREF black   = 0x000000;
  static const COLORREF white   = 0xFFFFFF;
  static const COLORREF red     = RGB(0xFF, 0, 0);
  static const COLORREF blue    = RGB(0, 0, 0xFF);
  static const COLORREF purple  = RGB(0xFF, 0, 0xFF);
  static const COLORREF yellow  = RGB(0xFF, 0xFF, 0);
  static const COLORREF brown   = RGB(0xB5, 0x6F, 0x32);
  static const COLORREF darkred = RGB(0x80, 0, 0);
  static const COLORREF orange  = RGB(0xFF, 128, 0);
  static const COLORREF green = RGB(0, 0x80, 0x32);
  static const COLORREF darkgreen = RGB(0, 0x80, 0);
  // set default style
  SendEditor(SCI_STYLESETFORE, STYLE_DEFAULT, black);
  SendEditor(SCI_STYLESETBACK, STYLE_DEFAULT, white);
  SendEditor(SCI_STYLECLEARALL);
  // set all margins to zero width
  SendEditor(SCI_SETMARGINWIDTHN, 0, 0);
  SendEditor(SCI_SETMARGINWIDTHN, 1, 0);
  SendEditor(SCI_SETMARGINWIDTHN, 2, 0);
  // set all margin types
  SendEditor(SCI_SETMARGINTYPEN,  0, SC_MARGIN_NUMBER);
  SendEditor(SCI_SETMARGINWIDTHN, 1, SC_MARGIN_SYMBOL);
  SendEditor(SCI_SETMARGINWIDTHN, 2, SC_MARGIN_SYMBOL);
  // set wrap modes
  if (1) {
    SendEditor(SCI_SETWRAPMODE, SC_WRAP_WORD);
  }
  if (1) {
    SendInputBar(SCI_SETWRAPMODE, SC_WRAP_WORD);
  }
}

////////////////////////////////////////////////////////////////////////////////

void
CConsoleWindow::Initialize()
{
  m_Fontface = Configuration::Get(KEY_SCRIPT_FONT_NAME);
  SetScriptStyles();
}

////////////////////////////////////////////////////////////////////////////////

void
CConsoleWindow::SetStyle(
  int style,
  COLORREF fore,
  COLORREF back,
  int size,
  const char* face)
{
  SendEditor(SCI_STYLESETFORE, style, fore);
  SendEditor(SCI_STYLESETBACK, style, back);
  if (size >= 1) {
    SendEditor(SCI_STYLESETSIZE, style, size);
  }
  if (face) {
    SendEditor(SCI_STYLESETFONT, style, (LPARAM)face);
  }
}

////////////////////////////////////////////////////////////////////////////////

void
CConsoleWindow::GetEditorText(CString& text)
{
  int length = SendEditor(SCI_GETLENGTH);
  char* str = new char[length + 1];
  if (str) {
    str[length] = 0;
    SendEditor(SCI_GETTEXT, length + 1, (LPARAM)str);
    text = str;
    delete[] str;
  }
}

////////////////////////////////////////////////////////////////////////////////

CString
CConsoleWindow::GetSelection()
{
  int start = SendEditor(SCI_GETSELECTIONSTART);
  int end   = SendEditor(SCI_GETSELECTIONEND);
  if (end < start) {
    end = start;
  }
  int length = end - start;
  char* str = new char[length + 1];
  if (str) {
    str[length] = 0;
    SendEditor(SCI_GETSELTEXT, 0, (LPARAM)str);
    CString result(str);
    delete[] str;
    return result;
  }
  return "";
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CConsoleWindow::OnSize(UINT type, int cx, int cy)
{
  if (m_Created) {
    int bar = 35;
    ::MoveWindow(__m_Editor__,    0,        0,        cx, cy - bar, TRUE);
    ::MoveWindow(__m_InputBar__,  0,        cy - bar, cx, bar,      TRUE);
  }
  CDocumentWindow::OnSize(type, cx, cy);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CConsoleWindow::OnSetFocus(CWnd* old)
{
  CDocumentWindow::OnSetFocus(old);
  if (m_Created) {
    ::SetFocus(__m_InputBar__);
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CConsoleWindow::OnPosChanged(NMHDR* nmhdr, LRESULT* result) {
  /*
  SCNotification* notify = (SCNotification*)nmhdr;
  int pos = SendEditor(SCI_GETCURRENTPOS);
  int line = SendEditor(SCI_LINEFROMPOSITION, pos);
  SetLineNumber(line);
  */
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CConsoleWindow::OnCharAdded(NMHDR* nmhdr, LRESULT* result) {
  SCNotification* notify = (SCNotification*)nmhdr;
  if (nmhdr->hwndFrom == __m_InputBar__) {
    if (!m_script_running && notify->ch == '\n') {
      int line = 0;
      int len = SendInputBar(SCI_LINELENGTH, (WPARAM) line);
      if (len >= 0 && len < 512) {
        char buffer[512] = {0};
        int offset = 0;
        if (len > 0) {
          len = SendInputBar(SCI_GETLINE, (LPARAM) line, (WPARAM) buffer);
          if (len >= 2) {
            if (buffer[len + offset - 2] == '\r' || buffer[len + offset - 2] == '\n') {
              buffer[len + offset - 2] = '\0';
              len -= 1;
              offset += 1;
            }
            if (buffer[len + offset - 1] == '\n' || buffer[len + offset - 1] == '\r') {
              buffer[len + offset - 1] = '\0';
              len -= 1;
              offset += 1;
            }
          }
        }
        SendInputBar(SCI_SETTARGETSTART, SendInputBar(SCI_POSITIONFROMLINE, 0));
        SendInputBar(SCI_SETTARGETEND,   SendInputBar(SCI_POSITIONFROMLINE, 1));
        SendInputBar(SCI_REPLACETARGET, 0, (WPARAM)"");
        SendInputBar(SCI_GOTOPOS, SendInputBar(SCI_GETTEXTLENGTH));
        SendInputBar(SCI_EMPTYUNDOBUFFER);
        if (strlen(buffer) >= 1) {
          EvaluateString(buffer);
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CConsoleWindow::OnCopy()
{
  if (::GetFocus() == __m_Editor__) {
    SendEditor(SCI_COPY);
  }
  else
  if (::GetFocus() == __m_InputBar__) {
    SendInputBar(SCI_COPY);
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CConsoleWindow::OnPaste()
{
  if (::GetFocus() == __m_Editor__) {
    //SendEditor(SCI_PASTE);
  }
  else
  if (::GetFocus() == __m_InputBar__) {
    SendInputBar(SCI_PASTE);
  }
}

////////////////////////////////////////////////////////////////////////////////

void
CConsoleWindow::OnToolChanged(UINT id, int tool_index) {
}

////////////////////////////////////////////////////////////////////////////////

BOOL
CConsoleWindow::IsToolAvailable(UINT id)
{
  BOOL available = FALSE;
  
  switch (id) {
    case ID_FILE_COPY:
      if (::GetFocus() == __m_Editor__) {
        if (SendEditor(SCI_GETSELECTIONSTART) - SendEditor(SCI_GETSELECTIONEND)) available = TRUE;
      }
      else
      if (::GetFocus() == __m_InputBar__) {
        if (SendInputBar(SCI_GETSELECTIONSTART) - SendInputBar(SCI_GETSELECTIONEND)) available = TRUE;
      }
    break;
    case ID_FILE_PASTE:
      if (::GetFocus() == __m_Editor__) {
        //if (SendEditor(SCI_CANPASTE) != 0) available = TRUE;
      }
      if (::GetFocus() == __m_InputBar__) {
        if (SendInputBar(SCI_CANPASTE) != 0) available = TRUE;
      }
    break;
  }
  return available;
}

////////////////////////////////////////////////////////////////////////////////

DWORD WINAPI
CConsoleWindow::ThreadRoutine(LPVOID parameter)
{
  if (!s_ConsoleWindow)
    return 0;
  if (!s_ConsoleWindow->m_Scripter.rt || !s_ConsoleWindow->m_Scripter.cx || !s_ConsoleWindow->m_Scripter.global)
    return 0;
  sCompileError error; error.m_TokenLine = -1;
  bool show_message = true;
  if (strlen(s_ConsoleWindow->m_Scripter.m_Script) > strlen("var ")
    && memcmp(s_ConsoleWindow->m_Scripter.m_Script, "var ", strlen("var ")) == 0) {
    show_message = false;
  }
 
  bool has_error = !s_ConsoleWindow->m_Scripter.__VerifyScript__(NULL, error);
  if (!error.m_Message.empty()) {
    // "var x;" shouldn't print 'undefined'...
    if (!show_message && error.m_Message != "undefined")
      show_message = true;
  }
  if (s_ConsoleWindow && IsWindow(s_ConsoleWindow->m_hWnd)) {
    s_ConsoleWindow->SetCaption("Javascript Console");
  }
  if (show_message) {
    if (s_ConsoleWindow) {
      s_ConsoleWindow->AddString(error.m_Message.c_str());
    }
  }
  if (s_ConsoleWindow) {
    s_ConsoleWindow->m_script_running = false;
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

void
CConsoleWindow::EvaluateString(const char* string)
{
  if (m_script_running)
    return;
  m_script_running = true;
  SetCaption("Javascript Console [running]");
  if (!m_Scripter.SetScript(string)) {
    m_script_running = false;
    return;
  }
  DWORD thread_id;
  if (CreateThread(NULL, 0, ThreadRoutine, (LPVOID) NULL, 0, &thread_id) == NULL) {
    m_script_running = false;
  }
}

////////////////////////////////////////////////////////////////////////////////
