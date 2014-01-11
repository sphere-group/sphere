#ifndef LINE_SORTER_HPP
#define LINE_SORTER_HPP
#include <afxext.h>

///////////////////////////////////////////////////////////////////////////////

class ScintillaLine {
public:
  char* data;
  unsigned int size;
};

///////////////////////////////////////////////////////////////////////////////
class CLineSorter {
public:
  CLineSorter();
  ~CLineSorter();

public:
  HWND m_Editor;
  bool delete_duplicates;
  bool sort_lines;
  bool reverse_lines;
  bool ignore_case;
  bool compare_numeric;
  int start_character;
  int start_tab;

private:
  LRESULT SendEditor(UINT msg, WPARAM wparam = 0, LPARAM lparam = 0);
  void SetStatusText(const char* text);
  void DestroyLines();
  std::vector<ScintillaLine*> m_Lines;

public:
  void Sort();
};
///////////////////////////////////////////////////////////////////////////////
#endif
