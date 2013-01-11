#pragma warning(disable : 4786)

#include <Scintilla.h>
#include <SciLexer.h>
#include "Editor.hpp"
#include "ScriptWindow.hpp"
#include "FileDialogs.hpp"
#include "NumberDialog.hpp"
//#include "Project.hpp"
#include "Scripting.hpp"
//#include "FileSystem.hpp"
#include "../common/system.hpp"
#include "Configuration.hpp"
#include "Keys.hpp"
#include "filename_comparer.hpp"
#include "resource.h"
//#include "MainWindow.hpp"
#include "LineSorterDialog.hpp"
#include "LineSorter.hpp"
#include <afxdlgs.h>

static const int ID_EDIT = 900;
static const UINT s_FindReplaceMessage = ::RegisterWindowMessage(FINDMSGSTRING);

#ifdef SPELLING_CHECKER
const int ANIMATION_TIMER = 9000;
#include <aspell.h>
#endif

////////////////////////////////////////////////////////////////////////////////
static const char* sFunctionDefinitions =
#include "scintillasphere.api"
;


////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CScriptWindow, CSaveableDocumentWindow)
  ON_WM_SIZE()
  ON_WM_SETFOCUS()
  ON_WM_CONTEXTMENU()
#ifdef SPELLING_CHECKER
  ON_WM_TIMER()
#endif
  ON_COMMAND(ID_SCRIPT_CHECKSYNTAX,      OnScriptCheckSyntax)
  ON_COMMAND(ID_SCRIPT_FIND,             OnScriptFind)
  ON_COMMAND(ID_SCRIPT_REPLACE,          OnScriptReplace)
  ON_COMMAND(ID_SCRIPT_GOTOLINE,         OnScriptGotoLine)
  ON_COMMAND(ID_SCRIPT_OPTIONS_SET_FONT,      OnOptionsSetScriptFont)
  ON_COMMAND(ID_SCRIPT_OPTIONS_TOGGLE_COLORS, OnOptionsToggleColors)
  ON_COMMAND(ID_SCRIPT_OPTIONS_SET_TAB_SIZE,  OnOptionsSetTabSize)
  ON_COMMAND(ID_SCRIPT_OPTIONS_TOGGLE_LINE_NUMBERS,  OnOptionsToggleLineNumbers)
  ON_COMMAND(ID_SCRIPT_OPTIONS_SHOW_WHITESPACE, OnOptionsShowWhitespace)
  ON_COMMAND(ID_SCRIPT_OPTIONS_WORD_WRAP, OnOptionsWordWrap)
  ON_COMMAND(ID_SCRIPT_OPTIONS_SHOW_WORD_FLAGS, OnOptionsShowWrapFlags)
  ON_COMMAND(ID_SCRIPT_OPTIONS_TOGGLE_AUTOCOMPLETE, OnOptionsToggleAutoComplete)
  ON_COMMAND(ID_SCRIPT_OPTIONS_HIGHLIGHT_CURRENT_LINE, OnOptionsHighlightCurrentLine)
#if SPELLING_CHECKER
  ON_COMMAND(ID_SCRIPT_OPTIONS_CHECK_SPELLING, OnOptionsCheckSpelling) 
  ON_COMMAND(ID_SCRIPT_OPTIONS_SET_SPELLING_LANGUAGE, OnOptionsSetSpellingLanguage)
  ON_UPDATE_COMMAND_UI(ID_SCRIPT_OPTIONS_CHECK_SPELLING, OnUpdateOptionsCheckSpelling)
#endif
  ON_COMMAND(ID_SCRIPT_OPTIONS_VIEW_LIST, OnOptionsViewList) 
  ON_UPDATE_COMMAND_UI(ID_SCRIPT_OPTIONS_VIEW_LIST, OnUpdateOptionsViewList) 
  ON_COMMAND(ID_SCRIPT_OPTIONS_SELECTION_STREAM, OnOptionsSelectionStream)
  ON_COMMAND(ID_SCRIPT_OPTIONS_SELECTION_RECTANGLE, OnOptionsSelectionRectangle)
  ON_COMMAND(ID_SCRIPT_OPTIONS_SELECTION_LINE, OnOptionsSelectionLine)
  ON_UPDATE_COMMAND_UI(ID_SCRIPT_OPTIONS_SELECTION_STREAM, OnUpdateOptionsSelectionStream)
  ON_UPDATE_COMMAND_UI(ID_SCRIPT_OPTIONS_SELECTION_RECTANGLE, OnUpdateOptionsSelectionRectangle)
  ON_UPDATE_COMMAND_UI(ID_SCRIPT_OPTIONS_SELECTION_LINE, OnUpdateOptionsSelectionLine)
  ON_UPDATE_COMMAND_UI(ID_SCRIPT_CHECKSYNTAX,      OnUpdateScriptCheckSyntax)
  ON_UPDATE_COMMAND_UI(ID_SCRIPT_OPTIONS_TOGGLE_LINE_NUMBERS, OnUpdateOptionsToggleLineNumbers)
  ON_UPDATE_COMMAND_UI(ID_SCRIPT_OPTIONS_TOGGLE_COLORS, OnUpdateOptionsToggleColors)
  ON_UPDATE_COMMAND_UI(ID_SCRIPT_OPTIONS_SHOW_WHITESPACE, OnUpdateOptionsShowWhitespace)
  ON_UPDATE_COMMAND_UI(ID_SCRIPT_OPTIONS_WORD_WRAP, OnUpdateOptionsWordWrap)
  ON_UPDATE_COMMAND_UI(ID_SCRIPT_OPTIONS_SHOW_WORD_FLAGS, OnUpdateOptionsShowWrapFlags)
  ON_UPDATE_COMMAND_UI(ID_SCRIPT_OPTIONS_TOGGLE_AUTOCOMPLETE, OnUpdateOptionsToggleAutoComplete)
  ON_UPDATE_COMMAND_UI(ID_SCRIPT_OPTIONS_HIGHLIGHT_CURRENT_LINE, OnUpdateOptionsHighlightCurrentLine)
  ON_COMMAND(ID_SCRIPT_TOOLS_SORT, OnScriptLineSorter)
  ON_COMMAND(ID_SCRIPT_TOOLS_FIXINDENTATION, OnScriptFixIndentation)
  ON_UPDATE_COMMAND_UI(ID_SCRIPT_TOOLS_FIXINDENTATION, OnUpdateScriptFixIndentation)
  ON_COMMAND(ID_SCRIPT_TOOLS_TO_LOWER_CASE, OnScriptToLowerCase)
  ON_COMMAND(ID_SCRIPT_TOOLS_TO_UPPER_CASE, OnScriptToUpperCase)
  ON_NOTIFY(SCN_SAVEPOINTREACHED, ID_EDIT, OnSavePointReached)
  ON_NOTIFY(SCN_SAVEPOINTLEFT,    ID_EDIT, OnSavePointLeft)
  ON_NOTIFY(SCN_UPDATEUI,         ID_EDIT, OnPosChanged)
  ON_NOTIFY(SCN_CHARADDED,        ID_EDIT, OnCharAdded)
  ON_NOTIFY(SCN_MARGINCLICK,      ID_EDIT, OnMarginClick)
  ON_COMMAND(ID_FILE_ZOOM_IN, OnZoomIn)
  ON_COMMAND(ID_FILE_ZOOM_OUT, OnZoomOut)
  ON_COMMAND(ID_FILE_COPY,  OnCopy)
  ON_COMMAND(ID_FILE_PASTE, OnPaste)
  ON_COMMAND(ID_FILE_UNDO,  OnUndo)
  ON_COMMAND(ID_FILE_REDO,  OnRedo)
  ON_REGISTERED_MESSAGE(s_FindReplaceMessage, OnFindReplace)
  ON_COMMAND(ID_SCRIPTVIEW_INSERT,  OnScriptViewInsert)
  ON_COMMAND(ID_SCRIPTVIEW_DELETE,  OnScriptViewDelete)
  ON_COMMAND(ID_SCRIPTVIEW_REFRESH, OnScriptViewRefresh)
  ON_UPDATE_COMMAND_UI(ID_SCRIPTVIEW_INSERT, OnUpdateScriptViewInsert)
  ON_UPDATE_COMMAND_UI(ID_SCRIPTVIEW_DELETE, OnUpdateScriptViewDelete)
  ON_COMMAND(ID_SCRIPTVIEW_CURRENTSCRIPTFUNCTIONS, OnScriptViewCurrentScriptFunctions)
  ON_COMMAND(ID_SCRIPTVIEW_CLIPBOARDHISTORY, OnScriptViewClipboardHistory)
  ON_COMMAND(ID_SCRIPTVIEW_CONTROL_CHARACTERS, OnScriptViewControlCharacters)
  ON_UPDATE_COMMAND_UI(ID_SCRIPTVIEW_CONTROL_CHARACTERS, OnUpdateScriptViewControlCharacters)
  ON_UPDATE_COMMAND_UI(ID_SCRIPTVIEW_CURRENTSCRIPTFUNCTIONS, OnUpdateScriptViewCurrentScriptFunctions)
  ON_UPDATE_COMMAND_UI(ID_SCRIPTVIEW_CLIPBOARDHISTORY, OnUpdateScriptViewClipboardHistory)
  ON_COMMAND(ID_SCRIPTVIEW_SPHERE_JS_FUNCTIONS,  OnScriptViewSphereJsFunctions)
  ON_COMMAND(ID_SCRIPTVIEW_SPHERE_JS_CONTROLSTRUCTURES,  OnScriptViewSphereJsControlStructures)
  ON_UPDATE_COMMAND_UI(ID_SCRIPTVIEW_SPHERE_JS_FUNCTIONS,  OnUpdateScriptViewSphereJsFunctions)
  ON_UPDATE_COMMAND_UI(ID_SCRIPTVIEW_SPHERE_JS_CONTROLSTRUCTURES,  OnUpdateScriptViewSphereJsControlStructures)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////

CScriptWindow::CScriptWindow(const char* filename, bool create_from_clipboard)
: CSaveableDocumentWindow(filename, IDR_SCRIPT)
, m_Created(false)
, m_SearchDialog(0)
, m_SyntaxHighlighted(true)
, m_ShowWhitespace(false)
, m_WordWrap(false)
, m_ShowWrapFlags(false)
, m_AllowAutoComplete(false)
, m_HighlightCurrentLine(false)
, m_SelectionType(SC_SEL_STREAM)
, m_ListType(SPHERE_JS_FUNCTIONS)
, m_ShowList(false)
, m_CheckSpelling(false)
, m_SearchDown(FR_DOWN)
{
  m_DocumentType = WA_SCRIPT;
  SetSaved(filename != NULL);
  SetModified(false);
  if (!Create())
    return;

  if (!filename && create_from_clipboard) {
    OnPaste();
    SendEditor(SCI_SETSEL,  0, 0);
  } else if (filename) {
    LoadScript(filename);
  }
  //int HScrollWidth = 640; // scintilla's default scroll width of 2000 is too big
  /*
  int num_lines = SendEditor(SCI_GETLINECOUNT);
  for (int i = 0; i < num_lines; i++) {
    int linewidth =  SendEditor(SCI_LINELENGTH, i) * 8; // 8 = font size
    if (linewidth > HScrollWidth)
      HScrollWidth = linewidth;
  }
  */
  //SendEditor(SCI_SETSCROLLWIDTH, HScrollWidth);
}
////////////////////////////////////////////////////////////////////////////////
bool
CScriptWindow::Create()
{
  static HINSTANCE scintilla = NULL;
  if (!scintilla) {
    scintilla = LoadLibrary("SciLexer.dll");
    if (!scintilla) {
      ::MessageBox(
        NULL,
        "Could not load Scintilla component (SciLexer.dll)",
        "Script Editor",
        MB_OK | MB_ICONERROR);
      delete this;
      return false;
    }
  }
  
  // create the child window
  CSaveableDocumentWindow::Create(
    AfxRegisterWndClass(0, LoadCursor(NULL, IDC_ARROW), NULL, AfxGetApp()->LoadIcon(IDI_SCRIPT)));
  RECT Rect;
  GetClientRect(&Rect);
  int cx = Rect.right - Rect.left;
  int cy = Rect.bottom - Rect.top;
  int sidebar_width = 0;
  m_ListType = Configuration::Get(KEY_SCRIPT_LIST_TYPE);
  m_ShowList = Configuration::Get(KEY_SCRIPT_SHOW_LIST);
  if ( !(m_ListType >= 0 && m_ListType < MAX_LIST_TYPE) ) { m_ListType = SPHERE_JS_FUNCTIONS; }
  if ( (m_ListType >= 0 && m_ListType < MAX_LIST_TYPE) ) {
    sidebar_width = 120;
  }
  // creates the script view
  m_Editor = ::CreateWindow(
    "Scintilla",
    "Source",
    WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_CLIPCHILDREN,
    sidebar_width, 0,
    cx - sidebar_width, cy,
    m_hWnd,
    (HMENU)ID_EDIT,
    AfxGetApp()->m_hInstance,
    0);
  // creates the list view
  m_List = ::CreateWindow(      
    "LISTBOX",
    "LISTBOX",
    WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_CLIPCHILDREN | WS_BORDER |
    LBS_EXTENDEDSEL | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT,
    0,
    0,
    sidebar_width,
    cy,
    m_hWnd,
    (HMENU)ID_EDIT,
    AfxGetApp()->m_hInstance,
    0);
  Initialize();
  CreateList(m_ListType);
  
  ::ShowWindow(m_Editor, SW_SHOW);
  ::UpdateWindow(m_Editor);
  if (::IsWindow(m_List)) {
    if (m_ShowList) {
      ::ShowWindow(m_List, SW_SHOW);
    }
    ::UpdateWindow(m_List);
  }
  m_Created = true;
  // update the size of the view
  OnSize(0, Rect.right /*- Rect.left*/, Rect.bottom /*- Rect.top*/);
  // give the view focus
  ::SetFocus(m_Editor);
#ifdef SPELLING_CHECKER
  if (m_CheckSpelling) {
    m_Timer = SetTimer(ANIMATION_TIMER, 1000, NULL);
  }
#endif
  return true;
}
////////////////////////////////////////////////////////////////////////////////
static void
GenerateFunctionsList(HWND list, const char* functions_definitions)
{
  unsigned int last_match = 0;
  unsigned int functions_length = strlen(functions_definitions);
  for (unsigned int i = 0; i < functions_length; i++)
  {
    if (functions_definitions[i] == ' ') 
	{
      char buffer[4096] = {0};
      memcpy(buffer, functions_definitions + last_match, i - last_match);
      ::SendMessage(list, LB_ADDSTRING, 0, (LPARAM)buffer);
      last_match = i + 1;
    }
  };
  if (1) // the last function does not end in space
  {
    char buffer[4096] = {0};
    memcpy(buffer, functions_definitions + last_match, strlen(functions_definitions) - last_match);
    ::SendMessage(list, LB_ADDSTRING, 0, (LPARAM)buffer);
  }
}
////////////////////////////////////////////////////////////////////////////////
void
CScriptWindow::CreateList(int type)
{
  m_ListType = type;
  if (!::IsWindow(m_List))
    return;
  ::SendMessage(m_List, LB_RESETCONTENT, 0, 0);
  if (m_ListType == CURRENT_SCRIPT_FUNCTIONS) {
    ::SendMessage(m_List, LB_ADDSTRING, 0, (LPARAM)"todo todo...");
  }
  if (m_ListType == CLIPBOARD_HISTORY) {
    CMainWindow* main_window = GetMainWindow();
    if (main_window) {
      for (unsigned int i = 0; i < main_window->m_ClipboardHistory.size(); i++) {
        ::SendMessage(m_List, LB_ADDSTRING, 0, (LPARAM)main_window->m_ClipboardHistory[i].c_str());
      }
    }
  }
  if (m_ListType == CONTROL_CHARACTERS) {
    const struct {
      const char* name;
      int code;
    } control_characters[] = {
      "SOH - (Start of Heading)",	0x1,
      "STX - (Start of Text)",	0x2,
      "ETX - (End of Text)",	0x3,
      "EOT - (End of Transmission)",	0x4,
      "ENQ - (Enquire)",	0x5,
      "ACK - (Acknowledge)",	0x6,
      "BEL - (Bell)",	0x7,
      "BS - (Backspace)",	0x8,
      "HT - (Horizontal Tab)",	(int)'\t',
      "VT - (Vertical Tab)",	(int)'\v',
      "FF - (Form Feed)",	0xc,
      "SO - (Shift Out)",	0xe,
      "SI - (Shift In)",	0xf,
      "DLE - (Data Link Escape)",	0x10,
      "DC1 - (Device Control 1/X-On)",	0x11,
      "DC2 - (Device Control 2)",	0x12,
      "DC3 - (Device Control 3/X-off)",	0x13,
      "DC4 - (Device Control 4)",	0x14,
      "NAK - (Negative Acknowledgement)",	0x15,
      "SYN - (Synchronous Idle)",	0x16,
      "ETB - (End of Transmission Block)",	0x17,
      "CAN - (Cancel)",	0x18,
      "EM - (End of Medium)",	0x19,
      "SUB - (Substitute)",	0x1a,
      "ESC - (Escape)",	0x1b,
      "FS - (File Separator)",	0x1c,
      "GS - (Group Separator)",	0x1d,
      "RS - (Record Separator)",	0x1e,
      "US - (Unit Separator)",	0x1f,
      "LF - (Line Feed)", '\n',
      "CR - (Carriage Return)", '\r',
    };
    
    const int num_control_characters = sizeof(control_characters) / sizeof(*control_characters);
    for (unsigned int i = 0; i < num_control_characters; i++) {
      ::SendMessage(m_List, LB_ADDSTRING, 0, (LPARAM)control_characters[i].name);
      ::SendMessage(m_List, LB_SETITEMDATA, i, (LPARAM)control_characters[i].code);
    }
  }
  if (m_ListType == SPHERE_JS_FUNCTIONS) {
    GenerateFunctionsList(m_List, sFunctionDefinitions);
  }
  if (m_ListType == SPHERE_JS_CONTROL_STRUCTURES)
  {
    ::SendMessage(m_List, LB_ADDSTRING, 0, (LPARAM)"if (1)\n{\n  // ...\n}\n");
    ::SendMessage(m_List, LB_ADDSTRING, 0, (LPARAM)"if (1)\n{\n  // ...\n}\nelse\n{\n  // ...\n}\n");
    ::SendMessage(m_List, LB_ADDSTRING, 0, (LPARAM)"for (var i = 0; i < 10; i++)\n{\n  // ...\n}\n");
    ::SendMessage(m_List, LB_ADDSTRING, 0, (LPARAM)"while (1)\n{\n  // ...\n}\n");
    ::SendMessage(m_List, LB_ADDSTRING, 0, (LPARAM)"switch (1)\n{\n  case (1):\n    // ...\n  break;\n}\n");
    ::SendMessage(m_List, LB_ADDSTRING, 0, (LPARAM)"function func_name()\n{\n  // ...\n}\n");
  }
  /*
  // todo: sphere+python functions
  if (m_ListType == SPHERE_PYTHON_FUNCTIONS)
  {
    ::SendMessage(m_List, LB_ADDSTRING, 0, (LPARAM)"sphere.video.FlipScreen()");
    ::SendMessage(m_List, LB_ADDSTRING, 0, (LPARAM)"sphere.input.IsKeyPressed(key)");
  }
  */
  /*
  // todo: sphere+python control structures
  if (m_ListType == SPHERE_PYTHON_CONTROL_STRUCTURES)
  {
    ::SendMessage(m_List, LB_ADDSTRING, 0, (LPARAM)"if (1):\n  pass\n");
    ::SendMessage(m_List, LB_ADDSTRING, 0, (LPARAM)"if (1):\n  pass\nelse:\n  pass\n");
    ::SendMessage(m_List, LB_ADDSTRING, 0, (LPARAM)"for i in range(0, 10):\n  pass\n");
    ::SendMessage(m_List, LB_ADDSTRING, 0, (LPARAM)"while (1):\n  pass\n");
    ::SendMessage(m_List, LB_ADDSTRING, 0, (LPARAM)"def func_name():\n  pass\n");
  }
  */
}
////////////////////////////////////////////////////////////////////////////////
CScriptWindow::ScriptType
CScriptWindow::GetScriptType()
{
  if (strlen(GetDocumentPath()) == 0) {
    return SCRIPT_TYPE_UNDETERMINABLE;
  }
  if (extension_compare(GetDocumentPath(), ".js")) {
    return SCRIPT_TYPE_JS;
  }
  if (extension_compare(GetDocumentPath(), ".py")) {
    return SCRIPT_TYPE_PY;
  }
  if (extension_compare(GetDocumentPath(), ".cpp")
   || extension_compare(GetDocumentPath(), ".c")
   || extension_compare(GetDocumentPath(), ".hpp")
   || extension_compare(GetDocumentPath(), ".h")
   || extension_compare(GetDocumentPath(), ".cxx")
   || extension_compare(GetDocumentPath(), ".hxx")) {
    return SCRIPT_TYPE_CPP;
  }
  if (extension_compare(GetDocumentPath(), ".java")) {
    return SCRIPT_TYPE_JAVA;
  }
  if (extension_compare(GetDocumentPath(), ".txt")) {
    return SCRIPT_TYPE_TXT;
  }
  /*
  if (1) {
    char line_text[1024] = {0};
    int line_number = 0;
    int line_length = ::SendMessage(m_Editor, SCI_LINELENGTH, line_number, 0);
    if (line_length > 0 && line_length < sizeof(line_text)) {
      ::SendMessage(m_Editor, SCI_GETLINE, line_number, (LRESULT)line_text);
      if (line_text[0] == '#' && line_text[1] == '!')
        return SCRIPT_TYPE_PERL;
    }
  }
  */
  return SCRIPT_TYPE_UNKNOWN;
}
////////////////////////////////////////////////////////////////////////////////
bool
CScriptWindow::IsSyntaxHighlightable()
{
  CScriptWindow::ScriptType type = GetScriptType();
  return (type == SCRIPT_TYPE_JS
       || type == SCRIPT_TYPE_UNDETERMINABLE
       || type == SCRIPT_TYPE_CPP
       || type == SCRIPT_TYPE_JAVA
       || type == SCRIPT_TYPE_PY
       || type == SCRIPT_TYPE_PERL);
}
////////////////////////////////////////////////////////////////////////////////
bool
CScriptWindow::IsSyntaxCheckable()
{
  CScriptWindow::ScriptType type = GetScriptType();
  return (type == SCRIPT_TYPE_JS
       || type == SCRIPT_TYPE_UNDETERMINABLE);
}
////////////////////////////////////////////////////////////////////////////////
bool
CScriptWindow::IsIndentationFixable()
{
  CScriptWindow::ScriptType type = GetScriptType();
  return (type == SCRIPT_TYPE_JS
       || type == SCRIPT_TYPE_UNDETERMINABLE
       || type == SCRIPT_TYPE_CPP
       || type == SCRIPT_TYPE_JAVA
       || type == SCRIPT_TYPE_PERL);
}
////////////////////////////////////////////////////////////////////////////////
static const int MARGIN_LINE_NUMBER_INDEX = 0;
static const int MARGIN_SCRIPT_FOLD_INDEX = 1;
static const int MARGIN_UNUSED = 2;
void
CScriptWindow::SetScriptStyles()
{
  static const char key_words[] = 
    "break case catch continue default delete do else "
    "finally for function if in instanceof new return "
    "switch this throw try typeof var void while with "
    "true false undefined ";
  static const char reserved_words[] =
    "abstract enum int short "
    "boolean export interface static "
    "byte extends long super "
    "char final native synchronized "
    "class float package throws "
    "const goto private transient "
    "debugger implements protected volatile "
    "double import public ";
  static const char key_words_python[] =
    "and as assert break class continue def del elif else "
    "except exec finally for from global if import in "
    "is lambda None not or pass print raise return try while ";
    // "as" and "None" are kind of reversed words, but not keywords
  static const COLORREF black   = 0x000000;
  static const COLORREF white   = 0xFFFFFF;
  static const COLORREF red     = RGB(0xFF, 0, 0);
  static const COLORREF green   = RGB(0, 0x80, 0);
  static const COLORREF blue    = RGB(0, 0, 0xFF);
  static const COLORREF purple  = RGB(0xFF, 0, 0xFF);
  static const COLORREF yellow  = RGB(0xFF, 0xFF, 128);
  static const COLORREF brown   = RGB(0xB5, 0x6F, 0x32);
  static const COLORREF darkred = RGB(0x80, 0, 0);
  if (GetScriptType() == SCRIPT_TYPE_UNDETERMINABLE
   || GetScriptType() == SCRIPT_TYPE_JS
   || GetScriptType() == SCRIPT_TYPE_CPP
   || GetScriptType() == SCRIPT_TYPE_JAVA)
  {
    SendEditor(SCI_SETLEXER, SCLEX_CPP);  // JavaScript uses the C++ lexer
    SendEditor(SCI_SETSTYLEBITS, 5);
    SendEditor(SCI_SETKEYWORDS, 0, (LPARAM)key_words);
    SendEditor(SCI_SETKEYWORDS, 1, (LPARAM)reserved_words);
  }
  else
  if (GetScriptType() == SCRIPT_TYPE_PY) {
    SendEditor(SCI_SETLEXER, SCLEX_PYTHON);
    SendEditor(SCI_SETSTYLEBITS, 5);
    SendEditor(SCI_SETKEYWORDS, 0, (LPARAM)key_words_python);
    //SendEditor(SCI_SETKEYWORDS, 1, (LPARAM)reserved_words_python);
  }
  else
  if (GetScriptType() == SCRIPT_TYPE_PERL) {
    SendEditor(SCI_SETLEXER, SCLEX_PERL);
    SendEditor(SCI_SETSTYLEBITS, 5);
    SendEditor(SCI_SETKEYWORDS, 0, (LPARAM)key_words);
    SendEditor(SCI_SETKEYWORDS, 1, (LPARAM)reserved_words);
  }
  else {
    SendEditor(SCI_SETLEXER, SCLEX_NULL);
  }
  if (!(m_FontSize >= 1 && m_FontSize <= 72))
    m_FontSize = 10;
  SetStyle(STYLE_DEFAULT, black, white, m_FontSize, m_Fontface.c_str());
  SendEditor(SCI_STYLECLEARALL);
  // set all margins to zero width
  SendEditor(SCI_SETMARGINWIDTHN, MARGIN_LINE_NUMBER_INDEX, 0);
  SendEditor(SCI_SETMARGINWIDTHN, MARGIN_SCRIPT_FOLD_INDEX, 0);
  SendEditor(SCI_SETMARGINWIDTHN, MARGIN_UNUSED, 0);
  // set margin type for line numbers
  SendEditor(SCI_SETMARGINTYPEN,  MARGIN_LINE_NUMBER_INDEX, SC_MARGIN_NUMBER);
  // for code folding
  SendEditor(SCI_SETPROPERTY, (WPARAM)"fold", (LPARAM)"1");
  SendEditor(SCI_SETPROPERTY, (WPARAM)"fold.compact", (LPARAM)"0");
  
  SendEditor(SCI_SETPROPERTY, (WPARAM)"fold.comment", (LPARAM)"1");
  SendEditor(SCI_SETPROPERTY, (WPARAM)"fold.preprocessor", (LPARAM)"1");
  // set margin type and mask and width for folding...
  SendEditor(SCI_SETMARGINTYPEN, MARGIN_SCRIPT_FOLD_INDEX, SC_MARGIN_SYMBOL);
  SendEditor(SCI_SETMARGINMASKN, MARGIN_SCRIPT_FOLD_INDEX, SC_MASK_FOLDERS);
  SendEditor(SCI_SETMARGINWIDTHN, MARGIN_SCRIPT_FOLD_INDEX, 20);
  // make the fold margin sensitive to click events
  SendEditor(SCI_SETMARGINSENSITIVEN, MARGIN_SCRIPT_FOLD_INDEX, 1);
  // define some markers for symbols for folding...
  SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDER, SC_MARK_PLUS);
  SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPEN, SC_MARK_MINUS);
  SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEREND, SC_MARK_EMPTY);
  SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_EMPTY);
  SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPENMID, SC_MARK_EMPTY);
  SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERSUB, SC_MARK_EMPTY);
  SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERTAIL, SC_MARK_EMPTY);
  SendEditor(SCI_SETFOLDFLAGS, 16, 0); // 16  	Draw line below if not expanded
  if (m_ShowLineNumbers) {  // resize the line number margin so we can see it
    int margin_size = SendEditor(SCI_TEXTWIDTH, STYLE_LINENUMBER, (LPARAM) "_99999");
    SendEditor(SCI_SETMARGINWIDTHN, 0, margin_size);
  }
  SendEditor(SCI_SETWRAPMODE, ((m_WordWrap) ? (SC_WRAP_WORD) : (SC_WRAP_NONE)));
  SendEditor(SCI_SETWRAPVISUALFLAGS, ((m_ShowWrapFlags) ? (SC_WRAPVISUALFLAG_END) : (SC_WRAPVISUALFLAG_NONE)));
  SetStyle(SCE_C_DEFAULT, black, white, m_FontSize, m_Fontface.c_str());
  if (m_TabWidth > 0) {
    SendEditor(SCI_SETTABWIDTH, m_TabWidth);
  }
  if (IsSyntaxHighlightable()) {
    SetStyle(STYLE_BRACELIGHT,  green);
    SetStyle(STYLE_BRACEBAD,    red);
  }
  if (GetScriptType() == SCRIPT_TYPE_UNDETERMINABLE
   || GetScriptType() == SCRIPT_TYPE_JS
   || GetScriptType() == SCRIPT_TYPE_CPP
   || GetScriptType() == SCRIPT_TYPE_JAVA) {
    if (m_SyntaxHighlighted) {
      SetStyle(SCE_C_COMMENT,     green);
      SetStyle(SCE_C_COMMENTLINE, green);
      SetStyle(SCE_C_COMMENTDOC,  green);
      SetStyle(SCE_C_COMMENTLINEDOC, green);
      
      SetStyle(SCE_C_NUMBER,      darkred);
      SetStyle(SCE_C_WORD,        blue);
      if (m_KeyWordStyleIsBold) {
        SendEditor(SCI_STYLESETBOLD, SCE_C_WORD, 1);
      }
      SetStyle(SCE_C_STRING,      blue);
      SetStyle(SCE_C_CHARACTER,   green);
      SetStyle(SCE_C_OPERATOR,    purple);
      SetStyle(SCE_C_IDENTIFIER,  black);
      SetStyle(SCE_C_WORD2,       red);
    }
  }
  else
  if (GetScriptType() == SCRIPT_TYPE_PY)
  {
    SetStyle(SCE_P_COMMENTLINE, green);
    SetStyle(SCE_P_WORD,        blue);
    SetStyle(SCE_P_STRING,      blue);
    SetStyle(SCE_P_CHARACTER,   green);
    SetStyle(SCE_P_TRIPLE, purple);
    SetStyle(SCE_P_TRIPLEDOUBLE, purple);
    //SetStyle(SCE_P_CLASSNAME, purple);
    //SetStyle(SCE_P_DEFNAME, purple);
    SetStyle(SCE_P_OPERATOR, purple);
    //SetStyle(SCE_P_IDENTIFIER, black);
    SetStyle(SCE_P_COMMENTBLOCK, green);
    SetStyle(SCE_P_STRINGEOL, green);
  }
  else
  if (GetScriptType() == SCRIPT_TYPE_PERL)
  {
//#define SCE_PL_DEFAULT 0
//#define SCE_PL_ERROR 1
    SetStyle(SCE_PL_COMMENTLINE, green);
//#define SCE_PL_POD 3
    SetStyle(SCE_PL_NUMBER,      darkred);
    SetStyle(SCE_PL_WORD,        blue);
    SetStyle(SCE_PL_STRING,      blue);
/*
#define SCE_PL_CHARACTER 7
#define SCE_PL_PUNCTUATION 8
#define SCE_PL_PREPROCESSOR 9
*/
    SetStyle(SCE_PL_OPERATOR,   purple);
/*
#define SCE_PL_IDENTIFIER 11
#define SCE_PL_SCALAR 12
#define SCE_PL_ARRAY 13
#define SCE_PL_HASH 14
#define SCE_PL_SYMBOLTABLE 15
#define SCE_PL_REGEX 17
#define SCE_PL_REGSUBST 18
#define SCE_PL_LONGQUOTE 19
#define SCE_PL_BACKTICKS 20
#define SCE_PL_DATASECTION 21
#define SCE_PL_HERE_DELIM 22
#define SCE_PL_HERE_Q 23
#define SCE_PL_HERE_QQ 24
#define SCE_PL_HERE_QX 25
#define SCE_PL_STRING_Q 26
#define SCE_PL_STRING_QQ 27
#define SCE_PL_STRING_QX 28
#define SCE_PL_STRING_QR 29
#define SCE_PL_STRING_QW 30
*/
  }
  SendEditor(SCI_SETVIEWWS, ((m_ShowWhitespace) ? (SCWS_VISIBLEALWAYS) : (SCWS_INVISIBLE)));
  SendEditor(SCI_SETCARETLINEVISIBLE, m_HighlightCurrentLine ? 1 : 0);
  SendEditor(SCI_SETCARETLINEBACK, yellow);
}
////////////////////////////////////////////////////////////////////////////////
void
CScriptWindow::Initialize()
{
  m_Fontface = Configuration::Get(KEY_SCRIPT_FONT_NAME);
  m_FontSize = Configuration::Get(KEY_SCRIPT_FONT_SIZE);
  m_SyntaxHighlighted = Configuration::Get(KEY_SCRIPT_SYNTAX_HIGHLIGHTED);
  m_TabWidth = Configuration::Get(KEY_SCRIPT_TAB_SIZE);
  m_KeyWordStyleIsBold = Configuration::Get(KEY_SCRIPT_KEYWORDS_IN_BOLD);
  m_ShowLineNumbers    = Configuration::Get(KEY_SCRIPT_SHOW_LINE_NUMBERS);
  m_ShowWhitespace     = Configuration::Get(KEY_SCRIPT_SHOW_WHITESPACE);
  m_WordWrap           = Configuration::Get(KEY_SCRIPT_WORD_WRAP);
  m_ShowWrapFlags      = Configuration::Get(KEY_SCRIPT_SHOW_WRAP_FLAGS);
  m_HighlightCurrentLine = Configuration::Get(KEY_SCRIPT_HIGHLIGHT_CURRENT_LINE);
  m_AllowAutoComplete  = Configuration::Get(KEY_SCRIPT_ALLOW_AUTOCOMPLETE);
  m_CheckSpelling      = Configuration::Get(KEY_SCRIPT_CHECK_SPELLING);
  SetScriptStyles();
  SetLineNumber(0);
}
////////////////////////////////////////////////////////////////////////////////
void
CScriptWindow::SetStyle(
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

afx_msg bool
CScriptWindow::LoadScript(const char* filename)
{
  // get the file size
  unsigned long file_size = FileSize(GetDocumentPath());
  if (file_size == 0) {
    return false;
  }
  // open the file
  FILE* file = fopen(filename, "rb");
  if (file == NULL) {
    return false;
  }
  /*
  // allocate a temporary storage buffer and read it
  char* buffer = new char[file_size + 1];
  if (buffer == NULL) return false;
  fread(buffer, sizeof(char), file_size, file);
  
  // null-terminate the string
  buffer[file_size] = 0;
  // put the buffer into the edit control
  SendEditor(SCI_SETSEL,  0, 0);
  SendEditor(SCI_SETTEXT, 0, (LPARAM)"");
  SendEditor(SCI_ADDTEXT, file_size, (LPARAM)buffer);
  // delete the buffer and close the file
  delete[] buffer;
  */
  int total_read = 0;
  char status_text[1024] = {0};
  sprintf (status_text, "%d%%...", (int)(((double)total_read / (double)file_size) * 100));
  GetStatusBar()->SetWindowText(status_text);

  while (!feof(file)) {
    char buffer[4096] = {0};
    int read_size = fread(buffer, sizeof(char), 4096, file);
    if (read_size > 0) {
      SendEditor(SCI_ADDTEXT, read_size, (LPARAM)buffer);
      total_read += read_size;
    }
    sprintf (status_text, "%d%%...", (int)(((double)total_read / (double)file_size) * 100));
    GetStatusBar()->SetWindowText(status_text);
  }

  sprintf (status_text, "%d%%...", (int)(((double)total_read / (double)file_size) * 100));
  GetStatusBar()->SetWindowText(status_text);
   
  fclose(file);
  file = NULL;

  SetModified(false);
  SendEditor(SCI_SETSAVEPOINT);
  SendEditor(SCI_EMPTYUNDOBUFFER);
  SendEditor(SCI_SETSEL,  0, 0);
  return true;
}

////////////////////////////////////////////////////////////////////////////////
void
CScriptWindow::GetEditorText(CString& text)
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
CScriptWindow::GetSelection()
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
CScriptWindow::OnSize(UINT type, int cx, int cy)
{
  if (m_Created) {
    int sidebar_width = 0;
    bool list_visible = ::IsWindow(m_List) && m_ShowList;
    if ( list_visible ) {
      RECT rect;
      if ( ::GetWindowRect(m_List, &rect) ) {
        sidebar_width = rect.right - rect.left;
      }
    }
 
    ::MoveWindow(m_Editor, sidebar_width, 0, cx - sidebar_width, cy, TRUE);
    if ( list_visible )
      ::MoveWindow(m_List, 0, 0, sidebar_width, cy, TRUE);
  }
  CSaveableDocumentWindow::OnSize(type, cx, cy);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnSetFocus(CWnd* old)
{
  CSaveableDocumentWindow::OnSetFocus(old);
  if (m_Created) {
    ::SetFocus(m_Editor);
  }
}
////////////////////////////////////////////////////////////////////////////////
#ifdef SPELLING_CHECKER
afx_msg void
CScriptWindow::OnTimer(UINT event)
{
  if (m_CheckSpelling) {
    int pos  = SendEditor(SCI_GETCURRENTPOS);
    int line = SendEditor(SCI_LINEFROMPOSITION, pos);
    SpellCheck(line, 1);
  }
}
#endif
////////////////////////////////////////////////////////////////////////////////
class ListBoxUtil
{
public:
  static int GetNumSelected(HWND list)
  {
    int i;
    int total = 0;  
    const int list_count = ::SendMessage(list, LB_GETCOUNT, 0, 0);
      
    for (i = 0; i < list_count; i++) {
      int selected = ::SendMessage(list, LB_GETSEL, (WPARAM) i, 0);
      if (selected > 0) {
        total += 1;
      }
    }
    return total;
  }
  static bool AreAnySelected(HWND list)
  {
    int i;
    const int list_count = ::SendMessage(list, LB_GETCOUNT, 0, 0);
      
    for (i = 0; i < list_count; i++) {
      int selected = ::SendMessage(list, LB_GETSEL, (WPARAM) i, 0);
      if (selected > 0) {
        return true;
      }
    }
    return false;
  }
  static void RemoveSelected(HWND list)
  {
    int i;
    const int list_count = ::SendMessage(list, LB_GETCOUNT, 0, 0);
    
    for (i = list_count - 1; i >= 0; i--) {
      int selected = ::SendMessage(list, LB_GETSEL, (WPARAM) i, 0);
      if (selected > 0) {
        ::SendMessage(list, LB_DELETESTRING, i, 0);
      }
    }
  }
  static void ProcessSelected(HWND list,
              void (*process_selected)(HWND list, int list_index, int process_index, void* data),
              void* data)
  {
    int i;
    int p;
    const int list_count = ::SendMessage(list, LB_GETCOUNT, 0, 0);
      
    for (i = 0, p = 0; i < list_count; i++)
    {
      int selected = ::SendMessage(list, LB_GETSEL, (WPARAM) i, 0);
      if (selected > 0) {
        process_selected(list, i, p, data);
        p++;
      }
    }
  }
};
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnScriptViewInsert()
{
  struct Local {
    static void process_selected(HWND list, int list_index, int process_index, void* data)
    {
      HWND editor = (HWND) data;
      int pos  = ::SendMessage(editor, SCI_GETCURRENTPOS, 0, 0);
      int line = ::SendMessage(editor, SCI_LINEFROMPOSITION, pos, 0);
      std::string indent = "";
      if (line >= 0) {
        char text[1024 * 16];
        int line_length = ::SendMessage(editor, SCI_LINELENGTH, line, 0);
        if (line_length > 0 && line_length < sizeof(text)) {
          ::SendMessage(editor, SCI_GETLINE, line, (LRESULT)text);
          int i = 0;
          while (text[i] == ' ' || text[i] == '\t') {
            indent += text[i++];
          }
        }
      }
      int len  = ::SendMessage(list, LB_GETTEXTLEN, list_index, 0);
      int item = ::SendMessage(list, LB_GETITEMDATA, list_index, 0);
      if (item != 0) {
        char string[2] = {0}; string[0] = (char)item;
        ::SendMessage(editor, SCI_ADDTEXT, 1, (WPARAM)string); 
      }
      else
      if (len > 0) {
        char* buffer = new char[len + 1];
        if (buffer) {
          ::SendMessage(list, LB_GETTEXT, list_index, (WPARAM)buffer);
          buffer[len] = 0;
          std::string str = "";
          if (process_index == 1) {
            str += "\n" + indent;
          }
          for (unsigned int i = 0; i < strlen(buffer); i++) {
            if (buffer[i] == '\n') {
              str += "\n";
              str += indent;
            }
            else {
              str += buffer[i];
            }
          }
          if (process_index > 0) {
            str += "\n" + indent;
          }
          delete[] buffer;
          if (process_index == 0) {
            ::SendMessage(editor, SCI_REPLACESEL, 0, (LRESULT)"");
          }
  
          ::SendMessage(editor, SCI_ADDTEXT, str.size(), (WPARAM)str.c_str());
        }
      }
    }
  };
  ListBoxUtil::ProcessSelected(m_List, Local::process_selected, m_Editor);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnScriptViewDelete()
{
  ListBoxUtil::RemoveSelected(m_List);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnScriptViewRefresh()
{
  CreateList(m_ListType);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnUpdateScriptViewInsert(CCmdUI* cmdui)
{
  cmdui->Enable((ListBoxUtil::GetNumSelected(m_List) > 0 && ListBoxUtil::GetNumSelected(m_List) < 15) ? TRUE : FALSE);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnUpdateScriptViewDelete(CCmdUI* cmdui)
{
  cmdui->Enable(ListBoxUtil::AreAnySelected(m_List) ? TRUE : FALSE);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnScriptViewCurrentScriptFunctions()
{
  CreateList(CURRENT_SCRIPT_FUNCTIONS);
  RememberConfiguration();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnScriptViewClipboardHistory()
{
  CreateList(CLIPBOARD_HISTORY);
  RememberConfiguration();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnScriptViewControlCharacters()
{
  CreateList(CONTROL_CHARACTERS);
  RememberConfiguration();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnScriptViewSphereJsFunctions()
{
  CreateList(SPHERE_JS_FUNCTIONS);
  RememberConfiguration();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnScriptViewSphereJsControlStructures()
{
  CreateList(SPHERE_JS_CONTROL_STRUCTURES);
  RememberConfiguration();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnUpdateScriptViewSphereJsFunctions(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_ListType == SPHERE_JS_FUNCTIONS ? TRUE : FALSE);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnUpdateScriptViewSphereJsControlStructures(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_ListType == SPHERE_JS_CONTROL_STRUCTURES ? TRUE : FALSE);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnUpdateScriptViewCurrentScriptFunctions(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_ListType == CURRENT_SCRIPT_FUNCTIONS ? TRUE : FALSE);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnUpdateScriptViewClipboardHistory(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_ListType == CLIPBOARD_HISTORY ? TRUE : FALSE);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnUpdateScriptViewControlCharacters(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_ListType == CONTROL_CHARACTERS ? TRUE : FALSE);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnContextMenu(CWnd* pWnd, CPoint point)
{
  HMENU _menu = LoadMenu(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_SCRIPTVIEW));
  HMENU menu = GetSubMenu(_menu, 0);
  TrackPopupMenu(menu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, point.x, point.y, 0, m_hWnd, NULL);
  DestroyMenu(_menu);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnScriptCheckSyntax()
{
  CString text;
  GetEditorText(text);
  // verify the script
  sCompileError error;
  if (!sScripting::VerifyScript(text, error))
  {
    // show the error message
    MessageBox(error.m_Message.c_str(), "Check syntax", MB_OK);
    if (error.m_Token.length() > 0)
      SendEditor(
        SCI_SETSEL,
        error.m_TokenStart,
        error.m_TokenStart + error.m_Token.length());
  }
  else
    MessageBox("Script is valid");
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnScriptFind()
{
  if (!m_SearchDialog) {
    m_SearchDialog = new CFindReplaceDialog;
    if (m_SearchDialog) {
      
      SendEditor(SCI_SETSELECTIONMODE, SC_SEL_STREAM);
      CString text = GetSelection();
      SendEditor(SCI_SETSELECTIONMODE, m_SelectionType);
      //m_SearchDialog->Create(true, text, NULL, FR_DOWN, this);
      m_SearchDialog->Create(true, ((text.GetLength() == 0) ? m_SearchString : text), NULL, m_SearchDown, this);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnScriptReplace()
{
  if (!m_SearchDialog) {
    m_SearchDialog = new CFindReplaceDialog;
    if (m_SearchDialog) {
      SendEditor(SCI_SETSELECTIONMODE, SC_SEL_STREAM);
      CString text = GetSelection();
      SendEditor(SCI_SETSELECTIONMODE, m_SelectionType);
      m_SearchDialog->Create(false, ((text.GetLength() == 0) ? m_SearchString : text), NULL, FR_DOWN, this);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnScriptGotoLine()
{
  int pos = SendEditor(SCI_GETCURRENTPOS);
  int cur_line = SendEditor(SCI_LINEFROMPOSITION, pos) + 1;
  int max_line = SendEditor(SCI_GETLINECOUNT);
  char string[1024];
  sprintf(string, "Line Number [%d / %d]", 1, max_line);
  CNumberDialog dialog("Goto Line", string, cur_line, 1, max_line);
  if (dialog.DoModal()) {
    int line_number = dialog.GetValue() - 1;
    if (line_number != cur_line) {
      SendEditor(SCI_ENSUREVISIBLEENFORCEPOLICY, line_number);
      SendEditor(SCI_GOTOLINE, line_number);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnSavePointReached(NMHDR* nmhdr, LRESULT* result)
{
  SetModified(false);
  SetScriptStyles();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnSavePointLeft(NMHDR* nmhdr, LRESULT* result)
{
  SetModified(true);
}
////////////////////////////////////////////////////////////////////////////////
void
CScriptWindow::OnToolChanged(UINT id, int tool_index) {
}
BOOL
CScriptWindow::IsToolAvailable(UINT id)
{
  BOOL available = FALSE;
  
  switch (id) {
    case ID_FILE_COPY:
      if (SendEditor(SCI_GETSELECTIONSTART) - SendEditor(SCI_GETSELECTIONEND)) available = TRUE;
    break;
    case ID_FILE_PASTE:
      if (SendEditor(SCI_CANPASTE) != 0 || IsClipboardFormatAvailable(CF_HDROP)) available = TRUE;
    break;
    case ID_FILE_UNDO: if (SendEditor(SCI_CANUNDO)) available = TRUE; break;
    case ID_FILE_REDO: if (SendEditor(SCI_CANREDO)) available = TRUE; break;
  }
  return available;
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnCopy()
{
  SendEditor(SCI_COPY);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnPaste()
{
  if (SendEditor(SCI_CANPASTE) != 0) {
    SendEditor(SCI_PASTE);
  }
  else if (IsClipboardFormatAvailable(CF_HDROP))
  {
    if (OpenClipboard()) {
      HDROP hdrop = (HDROP) GetClipboardData(CF_HDROP);
      if (hdrop) {
        UINT num_files = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, 0);
        SendEditor(SCI_BEGINUNDOACTION);
        SendEditor(SCI_REPLACESEL, 0, (LRESULT)"");
        for (unsigned int i = 0; i < num_files; i++)
        {
          char filename[MAX_PATH + 3] = {0};
          DragQueryFile(hdrop, i, filename, MAX_PATH);
          
          if (i < num_files - 1)
            sprintf (filename + strlen(filename), "\n");
          SendEditor(SCI_ADDTEXT, strlen(filename), (LPARAM)filename);
        }
        SendEditor(SCI_ENDUNDOACTION);
      }
      CloseClipboard();
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnUndo()
{
  SendEditor(SCI_UNDO);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnRedo()
{
  SendEditor(SCI_REDO);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnZoomIn()
{
  SendEditor(SCI_ZOOMIN);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnZoomOut()
{
  SendEditor(SCI_ZOOMOUT);
}
////////////////////////////////////////////////////////////////////////////////
static
bool is_brace(char ch) {
  return (ch == '{' || ch == '[' || ch == '('
       || ch == '}' || ch == ']' || ch == ')');
}
void
CScriptWindow::UpdateBraceHighlight()
{
  int pos = SendEditor(SCI_GETCURRENTPOS);
  if (is_brace((char) SendEditor(SCI_GETCHARAT, pos)))
  {
    int m = SendEditor(SCI_BRACEMATCH, pos, 0);
    if (m != -1) {
      SendEditor(SCI_BRACEHIGHLIGHT, pos, m);
    } 
    else {
      SendEditor(SCI_BRACEBADLIGHT, pos);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnPosChanged(NMHDR* nmhdr, LRESULT* result) {
  SCNotification* notify = (SCNotification*)nmhdr;
  int pos  = SendEditor(SCI_GETCURRENTPOS);
  int line = SendEditor(SCI_LINEFROMPOSITION, pos);
  SetLineNumber(line);
  if (m_SyntaxHighlighted)
    if (IsSyntaxHighlightable())
      UpdateBraceHighlight();
}
////////////////////////////////////////////////////////////////////////////////
void CScriptWindow::Expand(int &line, const bool doExpand, bool force, int visLevels, int level)
{
	const int lineMaxSubord = SendEditor(SCI_GETLASTCHILD, line, level & SC_FOLDLEVELNUMBERMASK);
	
  line++;
	
  while (line <= lineMaxSubord)
  {
		if (force) {
			if (visLevels > 0)
				SendEditor(SCI_SHOWLINES, line, line);
			else
				SendEditor(SCI_HIDELINES, line, line);
		} else {
			if (doExpand)
				SendEditor(SCI_SHOWLINES, line, line);
		}
		
    const int levelLine = level == -1 ? SendEditor(SCI_GETFOLDLEVEL, line) : level;
    if ( !(levelLine & SC_FOLDLEVELHEADERFLAG) ) {
      line++;
    }
    else
    {
			if (force) {
				if (visLevels > 1)
					SendEditor(SCI_SETFOLDEXPANDED, line, 1);
				else
					SendEditor(SCI_SETFOLDEXPANDED, line, 0);
				Expand(line, doExpand, force, visLevels - 1);
			} else {
				if (doExpand) {
					if (!SendEditor(SCI_GETFOLDEXPANDED, line))
						SendEditor(SCI_SETFOLDEXPANDED, line, 1);
				}
				
				Expand(line, doExpand, force, visLevels - 1);
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
void CScriptWindow::FoldAll() {
	SendEditor(SCI_COLOURISE, 0, -1);
	int maxLine = SendEditor(SCI_GETLINECOUNT);
	bool expanding = true;
	for (int lineSeek = 0; lineSeek < maxLine; lineSeek++) {
		if (SendEditor(SCI_GETFOLDLEVEL, lineSeek) & SC_FOLDLEVELHEADERFLAG) {
			expanding = !SendEditor(SCI_GETFOLDEXPANDED, lineSeek);
			break;
		}
	}
	for (int line = 0; line < maxLine; line++) {
		int level = SendEditor(SCI_GETFOLDLEVEL, line);
		if ((level & SC_FOLDLEVELHEADERFLAG)
     && (SC_FOLDLEVELBASE == (level & SC_FOLDLEVELNUMBERMASK))) {
			if (expanding) {
				SendEditor(SCI_SETFOLDEXPANDED, line, 1);
				Expand(line, true, false, 0, level);
				line--;
			} else {
				int lineMaxSubord = SendEditor(SCI_GETLASTCHILD, line, -1);
				SendEditor(SCI_SETFOLDEXPANDED, line, 0);
				if (lineMaxSubord > line)
					SendEditor(SCI_HIDELINES, line + 1, lineMaxSubord);
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
bool CScriptWindow::MarginClick(int position, int modifiers) {
	int lineClick = SendEditor(SCI_LINEFROMPOSITION, position);
	//Platform::DebugPrintf("Margin click %d %d %x\n", position, lineClick,
	//	SendEditor(SCI_GETFOLDLEVEL, lineClick) & SC_FOLDLEVELHEADERFLAG);
	if ((modifiers & SCMOD_SHIFT) && (modifiers & SCMOD_CTRL)) {
		FoldAll();
	} else {
		int levelClick = SendEditor(SCI_GETFOLDLEVEL, lineClick);
		if (levelClick & SC_FOLDLEVELHEADERFLAG) {
			if (modifiers & SCMOD_SHIFT) {
				// Ensure all children visible
				SendEditor(SCI_SETFOLDEXPANDED, lineClick, 1);
				Expand(lineClick, true, true, 100, levelClick);
			} else if (modifiers & SCMOD_CTRL) {
				if (SendEditor(SCI_GETFOLDEXPANDED, lineClick)) {
					// Contract this line and all children
					SendEditor(SCI_SETFOLDEXPANDED, lineClick, 0);
					Expand(lineClick, false, true, 0, levelClick);
				} else {
					// Expand this line and all children
					SendEditor(SCI_SETFOLDEXPANDED, lineClick, 1);
					Expand(lineClick, true, true, 100, levelClick);
				}
			} else {
				// Toggle this line
				SendEditor(SCI_TOGGLEFOLD, lineClick);
			}
		}
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnMarginClick(NMHDR* nmhdr, LRESULT* result)
{
  SCNotification* notify = (SCNotification*)nmhdr;
  const int modifiers = notify->modifiers;
  const int position = notify->position;
  const int margin = notify->margin;
  const int line_number = SendEditor(SCI_LINEFROMPOSITION, position);
  switch (margin) {
    case MARGIN_SCRIPT_FOLD_INDEX:
    {
      MarginClick(position, modifiers);
      //SendEditor(SCI_TOGGLEFOLD, line_number);
    }
    break;
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnCharAdded(NMHDR* nmhdr, LRESULT* result) {
  SCNotification* notify = (SCNotification*)nmhdr;
#if 1
  //if (m_AllowAutoComplete && notify->ch != '\r' && notify->ch != '\n' && notify->ch != ' ')
  if (m_AllowAutoComplete && isalpha(notify->ch))
  {
    if (GetScriptType() == SCRIPT_TYPE_UNDETERMINABLE || GetScriptType() == SCRIPT_TYPE_JS)
    {
      //SendEditor(SCI_SETWORDCHARS, 0, (LPARAM)"_.abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
      
      LPARAM cur_pos  = SendEditor(SCI_GETCURRENTPOS);
      LPARAM word_pos = SendEditor(SCI_WORDSTARTPOSITION, cur_pos);
      long len = cur_pos - word_pos;
	  if(len<0) len=0;
      if (word_pos >= 0 && len > 0) {
        // 
        char  linebuf[1024*8];
        LRESULT  current = SendEditor(SCI_GETCURLINE,  sizeof(linebuf),	reinterpret_cast<LPARAM>(static_cast<char  *>(linebuf)));
		if(current>=sizeof(linebuf)) current = 0;
		if(current<0) current = 0;
        linebuf[current]  =  '\0';
        char*  word  =  linebuf + (current-len);

        // Ok, now we have 'word' we need to check ourselves if it matches, because if it doesnt, it still will make SCintilla flikker.
        // THIS CAUSES THE FLICKERING, we need to make sure first IF we need to show the box with syntax. SCI-team ignored this error.
        if ((len >0) && strstr(sFunctionDefinitions,word)) // horribly inefficient find-string-in-string function (but it works)
          SendEditor(SCI_AUTOCSHOW, cur_pos - word_pos, (LPARAM)sFunctionDefinitions);
      }
      //SendEditor(SCI_SETCHARSDEFAULT);
    }
  }
#endif
  if (notify->ch == '\n') {
    int pos  = SendEditor(SCI_GETCURRENTPOS);
    int line = SendEditor(SCI_LINEFROMPOSITION, pos);
    if (line > 0) {
      char text[1024 * 8];
      int line_length = SendEditor(SCI_LINELENGTH, line - 1);
      if (line_length > 0 && line_length < sizeof(text)) {
        SendEditor(SCI_GETLINE, line - 1, (LRESULT)text);
        std::string str;
        int i = 0;
        while (text[i] == ' ' || text[i] == '\t') {
          str += text[i++];
        }
        /*
        if (1) {
          i = line_length;
          while (text[i] != '{' && i >= 0 && i > line_length - 5) { i--; }
          if (text[i] == '{') {
            str += "  ";
          }
          i = 0;
          while (i++ < 100) {
            int m = SendEditor(SCI_BRACEMATCH, i, 0);
            if (m != -1) {
              pos = pos;
            }
          }
          if (1) {
            while (text[i] != '}' && i >= 0 && i > line_length - 5) { i--; }
            if (text[i] == '}') {
              str.erase(0, 2);
            }
          }
        }
        */
        SendEditor(SCI_REPLACESEL, 0, (LRESULT)str.c_str());
      }
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
void
CScriptWindow::SetLineNumber(int line) {
  char str[120];
  int pos = SendEditor(SCI_GETCURRENTPOS) - SendEditor(SCI_POSITIONFROMLINE, line);
  line += 1;
  sprintf(str, "Line: %d Position: %d", line, pos);
  GetStatusBar()->SetPaneText(1, str);
}
///////////////////////////////////////////////////////////////////////////////
#ifdef SPELLING_CHECKER
class SpellCheckUtil {
public:
  AspellCanHaveError* ret;
  AspellConfig* config;
  AspellSpeller* speller;
  AspellDocumentChecker* checker;
  SpellCheckUtil() {
    ret = NULL;
    config = NULL;
    speller = NULL;
    checker = NULL;
  }
  bool Create() {
    if (config == NULL) {
      config = new_aspell_config();
      if (config == NULL)
        return false;
      aspell_config_replace(config, "dict-dir", "C:\\WINDOWS\\Desktop\\sphere\\source\\editor\\output\\Profile\\dict");
      aspell_config_replace(config, "data-dir", "C:\\WINDOWS\\Desktop\\sphere\\source\\editor\\output\\Profile\\data");
      aspell_config_replace(config, "lang", Configuration::Get(KEY_SCRIPT_SPELLING_LANGUAGE).c_str());
    }
    if (0) {
      char error[100000];
      strcpy(error, aspell_config_retrieve(config, "lang"));
      strcpy(error, aspell_config_retrieve(config, "jargon"));
      strcpy(error, aspell_config_retrieve(config, "size"));
      strcpy(error, aspell_config_retrieve(config, "module"));
    }
    ret = new_aspell_speller(config);
    delete_aspell_config(config);
    config = NULL;
    if (aspell_error(ret) != 0) {
      char error[100000];
      strcpy(error, aspell_error_message(ret));
      GetStatusBar()->SetWindowText(error);
      return false;
    }
    speller = to_aspell_speller(ret);
    /* Set up the document checker */
    ret = new_aspell_document_checker(speller);
    if (aspell_error(ret) != 0) {
      char error[100000];
      strcpy(error, aspell_error_message(ret));
      //  printf("Error: %s\n",aspell_error_message(ret));
        return false;
    }
    checker = to_aspell_document_checker(ret);
    return true;
  } 
  void Destroy() {
    delete_aspell_document_checker(checker);
    checker = NULL;
    delete_aspell_speller(speller);
    speller = NULL;
  }
};
void
CScriptWindow::SpellCheck(const int start_line_number, const int num_lines_to_check)
{
  static const int BAD_SPELLING_INDICTOR_INDEX = 0;
  static const int BAD_SPELLING_INDICTOR_MASK = INDIC0_MASK;
  static const int GOOD_SPELLING_INDICTOR_INDEX = 2;
  static const int GOOD_SPELLING_INDICTOR_MASK = INDIC2_MASK;
  SpellCheckUtil spell_check;
  if (!spell_check.Create())
    return;
  SendEditor(SCI_INDICSETSTYLE, BAD_SPELLING_INDICTOR_INDEX, INDIC_SQUIGGLE);
  SendEditor(SCI_INDICSETSTYLE, GOOD_SPELLING_INDICTOR_INDEX, INDIC_HIDDEN);
  static const COLORREF darkred = RGB(0x80, 0, 0);;
  //static const COLORREF white   = 0xFFFFFF;
  SendEditor(SCI_INDICSETFORE, BAD_SPELLING_INDICTOR_INDEX,  darkred);
  //SendEditor(SCI_INDICSETFORE, GOOD_SPELLING_INDICTOR_INDEX, white);
  //SendEditor(SCI_STARTSTYLING, start_pos, 0x1f);
  //SendEditor(SCI_COLOURISE, start_pos, start_pos + line_length);
  //char string[10000] = {0};
  const int end_line_number = (num_lines_to_check == -1) ? (SendEditor(SCI_GETLINECOUNT) - start_line_number) : (start_line_number + 1);
  for (int line_number = start_line_number; line_number < end_line_number; line_number++)
  {
    const int start_pos   = SendEditor(SCI_POSITIONFROMLINE, (WPARAM)line_number);
    const int line_length = SendEditor(SCI_LINELENGTH, SendEditor(SCI_LINEFROMPOSITION, start_pos));
    char text[1024 * 16];
    if (line_length > 0 && line_length < sizeof(text))
    {
      SendEditor(SCI_STARTSTYLING, start_pos, 0x1f);
      SendEditor(SCI_COLOURISE, start_pos, start_pos + line_length);
      memset(text, 0, sizeof(text));
      SendEditor(SCI_GETLINE, line_number, (LRESULT)text);
      SendEditor(SCI_STARTSTYLING, start_pos, INDICS_MASK);
      aspell_document_checker_process(spell_check.checker, text, -1);
      int last_offset = 0;
      AspellToken token;
      SendEditor(SCI_SETSTYLING, 0, GOOD_SPELLING_INDICTOR_MASK);
      while (token = aspell_document_checker_next_misspelling(spell_check.checker), token.len != 0)
      {
        SendEditor(SCI_SETSTYLING, token.offset - last_offset, GOOD_SPELLING_INDICTOR_MASK);
        SendEditor(SCI_SETSTYLING, token.len, BAD_SPELLING_INDICTOR_MASK);
        last_offset = token.offset + token.len;
        //sprintf (string + strlen(string), "[token.offset=%d token.len=%d] ", token.offset, token.len);
      }
      SendEditor(SCI_SETSTYLING, line_length - last_offset, GOOD_SPELLING_INDICTOR_MASK);
    }
  }
  //GetStatusBar()->SetWindowText(string);
  spell_check.Destroy();
}
#endif
////////////////////////////////////////////////////////////////////////////////
afx_msg LRESULT
CScriptWindow::OnFindReplace(WPARAM, LPARAM)
{
  CString find_string(m_SearchDialog->GetFindString());
  CString replace_string(m_SearchDialog->GetReplaceString());
  
  BOOL search_down = m_SearchDialog->SearchDown();
  
  TextToFind ttf;
  ttf.lpstrText  = find_string.GetBuffer(0);
  ttf.chrg.cpMax = SendEditor(SCI_GETLENGTH);
  
  int flags = 0;
  flags |= m_SearchDialog->MatchCase()      ? SCFIND_MATCHCASE : 0;
  flags |= m_SearchDialog->MatchWholeWord() ? SCFIND_WHOLEWORD : 0;

  if (m_SearchDialog->IsTerminating())
  {
    m_SearchDown   = m_SearchDialog->SearchDown();
    m_SearchString = m_SearchDialog->GetFindString();
    m_SearchDialog = NULL;
  } 
  
  else if (m_SearchDialog->FindNext()) 
  {
    ttf.chrg.cpMin = SendEditor(SCI_GETSELECTIONSTART) + 1;
    
    if (!search_down) 
    {
      ttf.chrg.cpMin -= 2;
      ttf.chrg.cpMax  = 0;
    }
    
    if (SendEditor(SCI_FINDTEXT, flags, (LPARAM)&ttf) == -1) 
    {
      GetStatusBar()->SetWindowText("No more matches!");
    } 
    else 
    {
      SendEditor(SCI_SETSEL, ttf.chrgText.cpMin, ttf.chrgText.cpMax);
      GetStatusBar()->SetWindowText("");
    }
  } 
  
  else if (m_SearchDialog->ReplaceCurrent()) 
  {
    ttf.chrg.cpMin = SendEditor(SCI_GETSELECTIONSTART);
    ttf.chrg.cpMax = SendEditor(SCI_GETSELECTIONEND);
    
    if (SendEditor(SCI_FINDTEXT, flags, (LPARAM)&ttf) != -1)
    {
      SendEditor(SCI_SETSEL, ttf.chrgText.cpMin, ttf.chrgText.cpMax);
      SendEditor(SCI_REPLACESEL, 0, (LPARAM)(const char*)replace_string);
    }
    
    ttf.chrg.cpMin = SendEditor(SCI_GETSELECTIONEND);
    ttf.chrg.cpMax = SendEditor(SCI_GETLENGTH);
    
    if (SendEditor(SCI_FINDTEXT, flags, (LPARAM)&ttf) == -1)
    {
      GetStatusBar()->SetWindowText("No more matches!");
    }
    else
    {
      SendEditor(SCI_SETSEL, ttf.chrgText.cpMin, ttf.chrgText.cpMax);
      GetStatusBar()->SetWindowText("");
    }
  } 
  
  else if (m_SearchDialog->ReplaceAll()) 
  {
    ttf.chrg.cpMin = 0;

    while (SendEditor(SCI_FINDTEXT, flags, (LPARAM)&ttf) != -1) 
    {
      SendEditor(SCI_SETTARGETSTART, ttf.chrgText.cpMin);
      SendEditor(SCI_SETTARGETEND, ttf.chrgText.cpMax);
      
      SendEditor(SCI_REPLACETARGET, strlen(replace_string), (LPARAM)(const char*)replace_string);
      
      ttf.chrg.cpMin = SendEditor(SCI_GETTARGETEND);
      ttf.chrg.cpMax = SendEditor(SCI_GETLENGTH);
    }
  }
  
  return 0;
}
////////////////////////////////////////////////////////////////////////////////
bool
CScriptWindow::GetSavePath(char* path)
{
  std::string directory = GetMainWindow()->GetDefaultFolder(m_DocumentType);
  SetCurrentDirectory(directory.c_str());
  CScriptFileDialog Dialog(FDM_SAVE);
  // set current directory on Win98/2000
  Dialog.m_ofn.lpstrInitialDir = directory.c_str();
  if (Dialog.DoModal() != IDOK)
    return false;
  strcpy(path, Dialog.GetPathName());
  return true;
}
////////////////////////////////////////////////////////////////////////////////
bool
CScriptWindow::SaveDocument(const char* path)
{
  FILE* file = fopen(path, "wb");
  if (file == NULL)
    return false;
  bool saved = false;
  int length = SendEditor(SCI_GETLENGTH);
  char* str = new char[length + 1];
  if (str) {
    str[length] = 0;
    SendEditor(SCI_GETTEXT, length + 1, (LPARAM)str);
    fwrite(str, 1, length, file);
    fclose(file);
    delete[] str;
    saved = true;
    SendEditor(SCI_SETSAVEPOINT);
  }
  return saved;
}
////////////////////////////////////////////////////////////////////////////////
void
CScriptWindow::TransformText(int method)
{
  int start = SendEditor(SCI_GETSELECTIONSTART);
  int end   = SendEditor(SCI_GETSELECTIONEND);
  if (end < start) {
    end = start;
  }
  int length = end - start;
  if (length <= 0)
    return;
  SendEditor(SCI_BEGINUNDOACTION);
  char buffer[1024] = {0};
  int buffer_length = 0;
  int buffer_size = sizeof(buffer);
  // method == 0 -> to lowercase
  // method == 1 -> to uppercase
  for (int i = 0; i < length; i++) {
    int ch = SendEditor(SCI_GETCHARAT, start + i);
    int is_upper = isupper(ch);
    int is_lower = islower(ch);
    int pos = start + i;
    if ((is_upper && method == 0)
     || (is_lower && method == 1)) {
      if (buffer_length >= buffer_size) {
        SendEditor(SCI_SETTARGETSTART, pos - buffer_length);
        SendEditor(SCI_SETTARGETEND, pos);
        SendEditor(SCI_REPLACETARGET, buffer_length, (LPARAM)buffer);
        if (0) {
          char msg[1024] = {0};
          sprintf (msg, "1: %d %d\n", pos - buffer_length, pos);
          SendEditor(SCI_APPENDTEXT, strlen(msg), (WPARAM)msg);
        }
        buffer_length = 0;
      } 
      if (method == 0)
        buffer[buffer_length] = tolower(ch);
      else
      if (method == 1)
        buffer[buffer_length] = toupper(ch);
      buffer_length += 1;
    }
    if ((!is_upper && method == 0)
     || (!is_lower && method == 1)) {
      if (buffer_length > 0) {
        SendEditor(SCI_SETTARGETSTART, pos - buffer_length);
        SendEditor(SCI_SETTARGETEND, pos);
        SendEditor(SCI_REPLACETARGET, buffer_length, (LPARAM)buffer);
        if (0) {
          char msg[1024] = {0};
          sprintf (msg, "2: %d %d\n", pos - buffer_length, pos);
          SendEditor(SCI_APPENDTEXT, strlen(msg), (WPARAM)msg);
        }
        buffer_length = 0;
      }
    }
  }
  if (buffer_length > 0) {
    buffer[buffer_length] = 0;
    SendEditor(SCI_SETTARGETSTART, end - buffer_length);
    SendEditor(SCI_SETTARGETEND, end);
    SendEditor(SCI_REPLACETARGET, buffer_length, (LPARAM)buffer);
    if (0) {
      char msg[1024] = {0};
      sprintf (msg, "3: %d %d\n", end - buffer_length, end);
      SendEditor(SCI_APPENDTEXT, strlen(msg), (WPARAM)msg);
    }
    buffer_length = 0;
  }
  SendEditor(SCI_SETSELECTIONSTART, start);
  SendEditor(SCI_SETSELECTIONEND, end);
  SendEditor(SCI_ENDUNDOACTION);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnScriptToLowerCase()
{
  TransformText(0);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnScriptToUpperCase()
{
  TransformText(1);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnScriptLineSorter()
{
  CLineSorterDialog dialog;
  if (dialog.DoModal() != IDOK) {
    return;
  }
  CLineSorter line_sorter;
  line_sorter.m_Editor = m_Editor;
  line_sorter.delete_duplicates = dialog.ShouldRemoveDuplicateLines();
  line_sorter.sort_lines        = dialog.ShouldSortLines();
  line_sorter.reverse_lines     = dialog.ShouldReverseLines();
  line_sorter.ignore_case       = dialog.ShouldIgnoreCase();
  line_sorter.compare_numeric   = dialog.ShouldCompareNumeric();
  line_sorter.start_character   = dialog.GetStartCharacter();
  line_sorter.start_tab   = dialog.GetStartTab();
  line_sorter.Sort();
}
////////////////////////////////////////////////////////////////////////////////
void
CScriptWindow::__WriteCharacter__(char* buffer, const int buffer_size, int& output_buffer_length, char ch)
{
  if (buffer == NULL) {
    //printf ("%c", ch);
    output_buffer_length += 1;
  } else {
    if (output_buffer_length < buffer_size) {
      buffer[output_buffer_length] = ch;
      output_buffer_length += 1;
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
void
CScriptWindow::__WriteNewline__(char* buffer, const int buffer_size, int& output_buffer_length)
{
  if (buffer == NULL) {
    //printf ("\n");
    output_buffer_length += 1;
  } else {
    if (output_buffer_length < buffer_size) {
      buffer[output_buffer_length] = '\n';
      output_buffer_length += 1;
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
int
CScriptWindow::__IsEndOfLine__(int c, int n)
{
  int end_of_line = 0;
  // \r\n\r\r\n\n\n
  //    ^ ^   ^ ^ ^ <-- shows where IsEndOfLine returns true
  if (c == '\r' || c == '\n') {
    if (c == '\r' && n == '\n') {
 
    } else {
      end_of_line = 1;
    }
  }
  return end_of_line;
}
////////////////////////////////////////////////////////////////////////////////
int
CScriptWindow::__SkipWhitespace__(int& l, int& c, int& n,  int& end_of_line, int cur_pos, int end_pos)
{
  int skipped = 0;
  while ((n == '\t' || n == ' ' || n == '\r' || n == '\n') && (cur_pos + 1 < end_pos)) {
    l = c;
    c = n;
    n = SendEditor(SCI_GETCHARAT, cur_pos + skipped + 1);
    if (__IsEndOfLine__(c, n)) {
      end_of_line = 1;   
    }
    skipped += 1;
  }
  return skipped;
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnScriptFixIndentation()
{
  const int selection_start = SendEditor(SCI_GETSELECTIONSTART);
  const int selection_end = SendEditor(SCI_GETSELECTIONEND);
  const int start_line = SendEditor(SCI_LINEFROMPOSITION, selection_start);
  const int end_line   = SendEditor(SCI_LINEFROMPOSITION, selection_end);
  if (start_line - end_line == 0) {
    GetStatusBar()->SetWindowText("No lines selected...");
    return;
  }
  const int start_pos = SendEditor(SCI_POSITIONFROMLINE, start_line);
  const int end_pos   = SendEditor(SCI_POSITIONFROMLINE, end_line + 1);
  int buffer_size = 0;
  int output_buffer_length = 0;
  int state = 0;
  char* buffer = NULL;
  if (0) {
    printf ("selection_start = %d, selection_end = %d\n", selection_start, selection_end);
    printf ("start_line = %d, end_line = %d\n", start_line, end_line);
    printf ("start_pos = %d, end_pos = %d\n", start_pos, end_pos);
  }
  int num_open_brackets = 0;
  int num_close_brackets = 0;
  int indent = 0;
  for (state = 0; state < 2; state++)
  {
    int c = '\0';
    int l = '\0';
    int n = SendEditor(SCI_GETCHARAT, start_pos);
    int in_comment_line = 0;
    int in_comment_block = 0;
    int line_length = 0;
    num_open_brackets = 0;
    num_close_brackets = 0;
    indent = 0;
    if (state == 1) {
      if ( !(output_buffer_length > 0)) {
        return;
      }
      buffer = new char[output_buffer_length + 1];
      if (!buffer) {
        return;
      }
      memset(buffer, ' ', output_buffer_length);
      buffer_size = output_buffer_length;
      output_buffer_length = 0;
    }
    for (int i = start_pos; i < end_pos; i++) {
      l = c;
      c = n;
      if (i + 1 < end_pos) {
        n = SendEditor(SCI_GETCHARAT, i + 1);
      } else {
        n = '\0';
      }
      int end_of_line = 0;
      /////////////////////////////////////////////////////
      if (__IsEndOfLine__(c, n)) {
        end_of_line = 1;
        in_comment_line = 0;
      } else {
        line_length += 1;
      }
      if (!in_comment_block) {
        if (l == '/' && c == '*') {
          in_comment_block = 1;
        }
        else
        if (!in_comment_line) {
          if (l == '/' && c == '/') {
            in_comment_line = 1;
          }
        }
      } else {
        if (l == '*' && c == '/') {
          in_comment_block = 0;
        }
      }
      /////////////////////////////////////////////////////
      if (in_comment_block || in_comment_line) {
        __WriteCharacter__(buffer, buffer_size, output_buffer_length, c);
      } else {
        /////////////////////////////////////////////////////
        if (c == '{')
        {
          num_open_brackets += 1;
          if (buffer == NULL) {
            int line = SendEditor(SCI_LINEFROMPOSITION, i);
            int pos  = i - SendEditor(SCI_POSITIONFROMLINE, line);
            //printf ("Line %3d, pos %3d, %3d, [%c]... { found. open = %3d close = %3d indent = %3d\n", (line + 1), pos, i, l, num_open_brackets, num_close_brackets, indent);
          }
          int skipped = __SkipWhitespace__(l, c, n, end_of_line, i, end_pos);
          if (skipped > 1) {
            i += skipped - 1;
            //printf ("Skipped %d characters...\n", skipped - 1);
          }
          if (line_length > 1) {
            __WriteNewline__(buffer, buffer_size, output_buffer_length); line_length = 0;
            for (int j = 0; j < (indent * 2); j++) {
              __WriteCharacter__(buffer, buffer_size, output_buffer_length, ' ');
            }
          }
          __WriteCharacter__(buffer, buffer_size, output_buffer_length, '{');
          __WriteNewline__(buffer, buffer_size, output_buffer_length); line_length = 0;
        
          int indent_level = indent;
          if (n != '}') { indent_level += 1; }
          for (int j = 0; j < (indent_level * 2); j++) {
            __WriteCharacter__(buffer, buffer_size, output_buffer_length, ' ');
          }
          indent += 1;
        }
        else
        if (c == '}')
        {
          num_close_brackets += 1;
          if (buffer == NULL) {
            int line = SendEditor(SCI_LINEFROMPOSITION, i);
            int pos  = i - SendEditor(SCI_POSITIONFROMLINE, line);
            if (indent == 0) {           
              char message[1024] = {0};
              sprintf (message, "Line %d, pos %d unexpected } found.", line, pos);
              //printf ("Line %3d, pos %3d, %3d... } found. open = %3d close = %3d indent = %3d\n", (line + 1), pos, i, num_open_brackets, num_close_brackets, indent);
              GetStatusBar()->SetWindowText(message);
              return;
            }
          }
          int skipped = __SkipWhitespace__(l, c, n, end_of_line, i, end_pos);
          if (skipped > 1) {
            i += skipped - 1;
            //printf ("Skipped %d characters...\n", skipped - 1);
          }
          if (end_of_line) {
            in_comment_line = 0;
          } 
          if (indent > 0) {
            indent -= 1;
            if (line_length > 1) {
              __WriteNewline__(buffer, buffer_size, output_buffer_length); line_length = 0;
              for (int j = 0; j < (indent * 2); j++) {
                __WriteCharacter__(buffer, buffer_size, output_buffer_length, ' ');
              }
            }
            __WriteCharacter__(buffer, buffer_size, output_buffer_length, '}');
            __WriteNewline__(buffer, buffer_size, output_buffer_length); line_length = 0;
            if (indent == 0) {
              __WriteNewline__(buffer, buffer_size, output_buffer_length); line_length = 0;
            }
          }
          else {
            __WriteCharacter__(buffer, buffer_size, output_buffer_length, '}');
          }
          if (indent > 0) {
            int indent_level = indent;
            if (n == '}') { indent_level -= 1; }
            for (int j = 0; j < (indent_level * 2); j++) {
              __WriteCharacter__(buffer, buffer_size, output_buffer_length, ' ');
            }
          }
        }
        else
        {
          __WriteCharacter__(buffer, buffer_size, output_buffer_length, c);
          if (end_of_line) {
            int skipped = __SkipWhitespace__(l, c, n, end_of_line, i, end_pos);
            if (skipped > 1) {
              i += skipped - 1;
            }
 
            if (indent > 0) {
              int indent_level = indent;
              if (n == '}') {
                indent_level -= 1;
              }
              for (int j = 0; j < (indent_level * 2); j++) {
                __WriteCharacter__(buffer, buffer_size, output_buffer_length, ' ');
              }
            }
 
            line_length = 0;
          }
        }
        /////////////////////////////////////////////////////
      }
    }
  }
  if (1) {
    SendEditor(SCI_SETTARGETSTART, start_pos);
    SendEditor(SCI_SETTARGETEND, end_pos);
    SendEditor(SCI_REPLACETARGET, output_buffer_length, (LPARAM)buffer);
    SendEditor(SCI_SETSELECTIONSTART, start_pos);
    SendEditor(SCI_SETSELECTIONEND, start_pos + output_buffer_length);
    if (0) {
      printf ("start_pos = %d, end_pos = %d, output_length = %d\n", start_pos, start_pos + output_buffer_length, output_buffer_length);
      printf ("num_open_brackets = %d, num_close_brackets = %d, indent = %d\n\n", num_open_brackets, num_close_brackets, indent);
      printf ("/////////////////////////////////////////////////////\n\n");
    }
  }
  if (buffer != NULL) {
    delete[] buffer;
    buffer = NULL;
  }
  //printf ("indent: %d\n", indent);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnOptionsToggleColors()
{
  m_SyntaxHighlighted = !m_SyntaxHighlighted;
  SetScriptStyles();
  RememberConfiguration();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnOptionsSetScriptFont()
{
  // Show the font dialog with all the default settings.
  LOGFONT lf;
  memset((void*)&lf, 0, sizeof(lf));
  if (m_Fontface.size() < 31) {
    memcpy(lf.lfFaceName, m_Fontface.c_str(), m_Fontface.size());
  }
  CFontDialog dlg(&lf);
  if (dlg.DoModal() == IDOK) {
    m_Fontface = std::string(dlg.GetFaceName());
    m_FontSize = dlg.GetSize() / 10; // for a 12pt font dlg.GetSize() is 120
    SetScriptStyles();
    RememberConfiguration();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnOptionsSetTabSize()
{
  CNumberDialog dialog("Enter new tab width value", "Value", m_TabWidth, 2, 8);
  if (dialog.DoModal() == IDOK) {
    m_TabWidth = dialog.GetValue();
    SetScriptStyles();
    RememberConfiguration();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnOptionsToggleLineNumbers()
{
  m_ShowLineNumbers = !m_ShowLineNumbers;
  SetScriptStyles();
  RememberConfiguration();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnOptionsShowWhitespace()
{
  m_ShowWhitespace = !m_ShowWhitespace;
  SetScriptStyles();
  RememberConfiguration();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnOptionsWordWrap()
{
  m_WordWrap = !m_WordWrap;
  SetScriptStyles();
  RememberConfiguration();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnOptionsShowWrapFlags()
{
  m_ShowWrapFlags = !m_ShowWrapFlags;
  SetScriptStyles();
  RememberConfiguration();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnOptionsHighlightCurrentLine()
{
  m_HighlightCurrentLine = !m_HighlightCurrentLine;
  SetScriptStyles();
  RememberConfiguration();
}
////////////////////////////////////////////////////////////////////////////////
#ifdef SPELLING_CHECKER
afx_msg void
CScriptWindow::OnOptionsCheckSpelling()
{
  if (m_CheckSpelling) {
    KillTimer(m_Timer);
  }
  m_CheckSpelling = !m_CheckSpelling;
  SetScriptStyles();
  RememberConfiguration();
  if (m_CheckSpelling) {
    m_Timer = SetTimer(ANIMATION_TIMER, 1000, NULL);
  }
}
#endif
////////////////////////////////////////////////////////////////////////////////
#ifdef SPELLING_CHECKER
afx_msg void
CScriptWindow::OnUpdateOptionsCheckSpelling(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_CheckSpelling ? TRUE : FALSE);
}
#endif
////////////////////////////////////////////////////////////////////////////////
#ifdef SPELLING_CHECKER
#include "ListDialog.hpp"
afx_msg void
CScriptWindow::OnOptionsSetSpellingLanguage()
{
  AspellCanHaveError* ret = NULL;
  AspellConfig* config = NULL;
  if (config == NULL) {
    config = new_aspell_config();
    if (config == NULL)
      return;
    aspell_config_replace(config, "dict-dir", "C:\\WINDOWS\\Desktop\\sphere\\source\\editor\\output\\Profile\\dict");
    aspell_config_replace(config, "data-dir", "C:\\WINDOWS\\Desktop\\sphere\\source\\editor\\output\\Profile\\data");
    aspell_config_replace(config, "lang", Configuration::Get(KEY_SCRIPT_SPELLING_LANGUAGE).c_str());
  }
  CListDialog dialog;
  dialog.SetCaption("Set Language");
  ////////
  /* the returned pointer should _not_ need to be deleted */
  AspellDictInfoList* dlist = get_aspell_dict_info_list(config);
  AspellDictInfoEnumeration* dels = aspell_dict_info_list_elements(dlist);
  const AspellDictInfo* entry;
  /*
  printf("%-30s%-8s%-20s%-6s%-10s\n", "NAME", "CODE", "JARGON", "SIZE", "MODULE");
  while ( (entry = aspell_dict_info_enumeration_next(dels)) != 0) 
  {
    printf("%-30s%-8s%-20s%-6s%-10s\n",
	    entry->name,
	    entry->code, entry->jargon, 
	    entry->size_str, entry->module->name);
  }
  */
  while ( (entry = aspell_dict_info_enumeration_next(dels)) != 0) 
  {
    int index = dialog.AddItem(entry->name);
    if (strcmp(entry->name, aspell_config_retrieve(config, "lang")) == 0) {
      dialog.SetSelection(index);
    }
  }
  delete_aspell_dict_info_enumeration(dels);
  ////////
  delete_aspell_config(config);
  if (dialog.DoModal() == IDOK) {
    Configuration::Set(KEY_SCRIPT_SPELLING_LANGUAGE, dialog.GetItemText(dialog.GetSelection()));
  }
}
#endif
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnOptionsViewList()
{
  if (::IsWindow(m_List)) {
    m_ShowList = !m_ShowList;
    ::ShowWindow(m_List, m_ShowList ? SW_SHOW : SW_HIDE);
    RECT Rect;
    GetClientRect(&Rect);
    OnSize(0, Rect.right /*- Rect.left*/, Rect.bottom /*- Rect.top*/);
    RememberConfiguration();
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnUpdateOptionsViewList(CCmdUI* cmdui)
{
  cmdui->Enable(::IsWindow(m_List) ? TRUE : FALSE);
  cmdui->SetCheck(m_ShowList ? TRUE : FALSE);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnOptionsSelectionStream()
{
  m_SelectionType = SC_SEL_STREAM;
  SendEditor(SCI_SETSELECTIONMODE, m_SelectionType);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnOptionsSelectionRectangle()
{
  m_SelectionType = SC_SEL_RECTANGLE;
  SendEditor(SCI_SETSELECTIONMODE, m_SelectionType);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnOptionsSelectionLine()
{
  m_SelectionType = SC_SEL_LINES;
  SendEditor(SCI_SETSELECTIONMODE, m_SelectionType);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnUpdateOptionsSelectionStream(CCmdUI* cmdui)
{
  cmdui->SetCheck((SendEditor(SCI_GETSELECTIONMODE) == SC_SEL_STREAM) ? TRUE : FALSE);
}
afx_msg void
CScriptWindow::OnUpdateOptionsSelectionRectangle(CCmdUI* cmdui)
{
  cmdui->SetCheck((SendEditor(SCI_GETSELECTIONMODE) == SC_SEL_RECTANGLE) ? TRUE : FALSE);
}
afx_msg void
CScriptWindow::OnUpdateOptionsSelectionLine(CCmdUI* cmdui)
{
  cmdui->SetCheck((SendEditor(SCI_GETSELECTIONMODE) == SC_SEL_LINES) ? TRUE : FALSE);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnUpdateScriptCheckSyntax(CCmdUI* cmdui)
{
  if ( !IsSyntaxCheckable() ) {
    cmdui->Enable(FALSE);
  } else {
    cmdui->Enable(TRUE);
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnUpdateOptionsToggleLineNumbers(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_ShowLineNumbers ? TRUE : FALSE);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnUpdateOptionsToggleColors(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_SyntaxHighlighted ? TRUE : FALSE);
  if ( !IsSyntaxHighlightable() ) {
    cmdui->Enable(FALSE);
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnUpdateScriptFixIndentation(CCmdUI* cmdui)
{
  if ( !IsIndentationFixable() ) {
    cmdui->Enable(FALSE);
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnUpdateOptionsShowWhitespace(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_ShowWhitespace ? TRUE : FALSE);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnUpdateOptionsWordWrap(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_WordWrap ? TRUE : FALSE);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnUpdateOptionsShowWrapFlags(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_ShowWrapFlags ? TRUE : FALSE);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnUpdateOptionsHighlightCurrentLine(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_HighlightCurrentLine ? TRUE : FALSE);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnOptionsToggleAutoComplete()
{
  m_AllowAutoComplete = !m_AllowAutoComplete;
  RememberConfiguration();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CScriptWindow::OnUpdateOptionsToggleAutoComplete(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_AllowAutoComplete ? TRUE : FALSE);
}
////////////////////////////////////////////////////////////////////////////////
void
CScriptWindow::RememberConfiguration()
{
  Configuration::Set(KEY_SCRIPT_FONT_NAME, m_Fontface);
  Configuration::Set(KEY_SCRIPT_FONT_SIZE, m_FontSize);
  Configuration::Set(KEY_SCRIPT_TAB_SIZE, m_TabWidth);
  Configuration::Set(KEY_SCRIPT_SYNTAX_HIGHLIGHTED, m_SyntaxHighlighted);
  Configuration::Set(KEY_SCRIPT_SHOW_LINE_NUMBERS, m_ShowLineNumbers);
  Configuration::Set(KEY_SCRIPT_SHOW_WHITESPACE, m_ShowWhitespace);
  Configuration::Set(KEY_SCRIPT_WORD_WRAP, m_WordWrap);
  Configuration::Set(KEY_SCRIPT_SHOW_WRAP_FLAGS, m_ShowWrapFlags);
  Configuration::Set(KEY_SCRIPT_HIGHLIGHT_CURRENT_LINE, m_HighlightCurrentLine);
  Configuration::Set(KEY_SCRIPT_ALLOW_AUTOCOMPLETE, m_AllowAutoComplete);
  Configuration::Set(KEY_SCRIPT_LIST_TYPE, m_ListType);
  Configuration::Set(KEY_SCRIPT_SHOW_LIST, m_ShowList ? true : false);
  Configuration::Set(KEY_SCRIPT_CHECK_SPELLING, m_CheckSpelling);
}
////////////////////////////////////////////////////////////////////////////////
