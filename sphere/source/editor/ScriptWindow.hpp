#ifndef SCRIPT_WINDOW_HPP
#define SCRIPT_WINDOW_HPP
#include <afxext.h>
#include "SaveableDocumentWindow.hpp"
class CScriptWindow 
: public CSaveableDocumentWindow
{
public:
  CScriptWindow(const char* filename = NULL, bool create_from_clipboard = false);
private:
  bool Create();
  void Initialize();
  LRESULT SendEditor(UINT msg, WPARAM wparam = 0, LPARAM lparam = 0) {
    return ::SendMessage(m_Editor, msg, wparam, lparam);
  }
  void SetScriptStyles();
  void SetStyle(int style, COLORREF fore, COLORREF back = 0xFFFFFF, int size = -1, const char* face = 0);
  void RememberConfiguration();
  bool LoadScript(const char* filename);
  void GetEditorText(CString& text);
  CString GetSelection();
  afx_msg void OnSize(UINT type, int cx, int cy);
  afx_msg void OnSetFocus(CWnd* old);
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
  afx_msg void OnScriptCheckSyntax();
  afx_msg void OnScriptFind();
  afx_msg void OnScriptReplace();
  afx_msg void OnScriptGotoLine();
  /////////////////////////////////////
  afx_msg void OnUpdateScriptCheckSyntax(CCmdUI* cmdui);
  afx_msg void OnOptionsSetScriptFont();
  afx_msg void OnOptionsToggleColors();
  afx_msg void OnOptionsSetTabSize();
  afx_msg void OnOptionsToggleLineNumbers();
  afx_msg void OnOptionsShowWhitespace();
  afx_msg void OnOptionsWordWrap();
  afx_msg void OnOptionsShowWrapFlags();
  afx_msg void OnOptionsToggleAutoComplete();
  afx_msg void OnOptionsHighlightCurrentLine();
  afx_msg void OnOptionsViewList();
  afx_msg void OnOptionsSelectionStream();
  afx_msg void OnOptionsSelectionRectangle();
  afx_msg void OnOptionsSelectionLine();
  afx_msg void OnUpdateOptionsSelectionStream(CCmdUI* cmdui);
  afx_msg void OnUpdateOptionsSelectionRectangle(CCmdUI* cmdui);
  afx_msg void OnUpdateOptionsSelectionLine(CCmdUI* cmdui);
  afx_msg void OnUpdateOptionsHighlightCurrentLine(CCmdUI* cmdui);
  afx_msg void OnUpdateOptionsViewList(CCmdUI* cmdui);
#ifdef SPELLING_CHECKER
  afx_msg void OnOptionsCheckSpelling();
  afx_msg void OnOptionsSetSpellingLanguage();
  afx_msg void OnUpdateOptionsCheckSpelling(CCmdUI* cmdui);
#endif
  afx_msg void OnUpdateOptionsToggleLineNumbers(CCmdUI* cmdui);
  afx_msg void OnUpdateOptionsToggleColors(CCmdUI* cmdui);
  afx_msg void OnUpdateOptionsShowWhitespace(CCmdUI* cmdui);
  afx_msg void OnUpdateOptionsWordWrap(CCmdUI* cmdui);
  afx_msg void OnUpdateOptionsShowWrapFlags(CCmdUI* cmdui);
  afx_msg void OnUpdateOptionsToggleAutoComplete(CCmdUI* cmdui);
  /////////////////////////////////////
  afx_msg void OnScriptLineSorter();
  /////////////////////////////////////
  void __WriteCharacter__(char* buffer, const int buffer_size, int& output_buffer_length, char ch);
  void __WriteNewline__(char* buffer, const int buffer_size, int& output_buffer_length);
  int __IsEndOfLine__(int c, int n);
  int __SkipWhitespace__(int& l, int& c, int& n, int& end_of_line, int cur_pos, int end_pos);
  afx_msg void OnScriptFixIndentation();
  afx_msg void OnUpdateScriptFixIndentation(CCmdUI* cmdui);
  void TransformText(int method);
  afx_msg void OnScriptToLowerCase();
  afx_msg void OnScriptToUpperCase();
  /////////////////////////////////////
  afx_msg void OnSavePointReached(NMHDR* nmhdr, LRESULT* result);
  afx_msg void OnSavePointLeft(NMHDR* nmhdr, LRESULT* result);
  afx_msg void OnPosChanged(NMHDR* nmhdr, LRESULT* result);
  afx_msg void OnCharAdded(NMHDR* nmhdr, LRESULT* result);
  afx_msg void OnMarginClick(NMHDR* nmhdr, LRESULT* result);
  void SetLineNumber(int line);
  void SpellCheck(const int start_line_number, const int num_lines_to_check);
  bool MarginClick(int position, int modifiers);
  void Expand(int &line, const bool doExpand,
              bool force = false, int visLevels = 0, int level = -1);
  void FoldAll();
  afx_msg void OnCopy();
  afx_msg void OnPaste();
  afx_msg void OnUndo();
  afx_msg void OnRedo();
  afx_msg void OnZoomIn();
  afx_msg void OnZoomOut();
  void OnToolChanged(UINT id, int tool_index);
  BOOL IsToolAvailable(UINT id);
  afx_msg LRESULT OnFindReplace(WPARAM, LPARAM);
  virtual bool GetSavePath(char* path);
  virtual bool SaveDocument(const char* path);
  enum ScriptType {
    SCRIPT_TYPE_UNDETERMINABLE,
    SCRIPT_TYPE_UNKNOWN,
    SCRIPT_TYPE_JS,
    SCRIPT_TYPE_PY,
    SCRIPT_TYPE_JAVA,
    SCRIPT_TYPE_CPP,
    SCRIPT_TYPE_TXT,
    SCRIPT_TYPE_PERL,
  };
  ScriptType GetScriptType();
  bool IsSyntaxHighlightable();
  bool IsSyntaxCheckable();
  bool IsIndentationFixable();
private:
  afx_msg void OnScriptViewInsert();
  afx_msg void OnScriptViewDelete();
  afx_msg void OnScriptViewRefresh();
  afx_msg void OnUpdateScriptViewInsert(CCmdUI* cmdui);
  afx_msg void OnUpdateScriptViewDelete(CCmdUI* cmdui);
  afx_msg void OnScriptViewSphereJsFunctions();
  afx_msg void OnScriptViewSphereJsControlStructures();
  afx_msg void OnScriptViewCurrentScriptFunctions();
  afx_msg void OnScriptViewClipboardHistory();
  afx_msg void OnScriptViewControlCharacters();
  afx_msg void OnUpdateScriptViewSphereJsFunctions(CCmdUI* cmdui);
  afx_msg void OnUpdateScriptViewSphereJsControlStructures(CCmdUI* cmdui);
  afx_msg void OnUpdateScriptViewCurrentScriptFunctions(CCmdUI* cmdui);
  afx_msg void OnUpdateScriptViewClipboardHistory(CCmdUI* cmdui);
  afx_msg void OnUpdateScriptViewControlCharacters(CCmdUI* cmdui);
  void CreateList(int type);
#ifdef SPELLING_CHECKER
private:
  UINT m_Timer;
  afx_msg void OnTimer(UINT event);
#endif
private:
  BOOL m_SearchDown;
  CString m_SearchString;
private:
  bool m_Created;
  HWND m_Editor;
  HWND m_List;
public:
  enum ListType {
    CURRENT_SCRIPT_FUNCTIONS = 0,
    CLIPBOARD_HISTORY,
    CONTROL_CHARACTERS,
    SPHERE_JS_FUNCTIONS,
    SPHERE_JS_CONTROL_STRUCTURES,
    MAX_LIST_TYPE,
  };
private:
  int m_ListType;
  bool m_ShowList;
  std::string m_Fontface;
  bool m_SyntaxHighlighted;
  int m_TabWidth;
  bool m_KeyWordStyleIsBold;
  bool m_ShowLineNumbers;
  int m_FontSize;
  bool m_ShowWhitespace;
  bool m_WordWrap;
  bool m_AllowAutoComplete;
  int m_SelectionType;
  bool m_CheckSpelling;
  bool m_ShowWrapFlags;
  bool m_HighlightCurrentLine;
  void UpdateBraceHighlight();
  CFindReplaceDialog* m_SearchDialog;
  DECLARE_MESSAGE_MAP()
};
#endif
