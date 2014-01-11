#pragma warning(disable: 4786)

#include <Scintilla.h>
#include <SciLexer.h>
#include <algorithm>
#include <functional>
#include <vector>
#include <ctype.h>
#include "LineSorter.hpp"
#include "Editor.hpp"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/*
bool line_compare_ignore_case(ScintillaLine* a, ScintillaLine* b, bool ignore_case)
{
  if (!a || !b)
    return false;
  for (int i = 0; i < m_lines[a]->size && i < m_lines[b]->size; i++) {
    if (isalpha(a->data[i]) && isalpha(b->data[i])) {
      if (tolower(a->data[i]) <= tolower(b->data[i])) {
        //printf ("1. %d, %s belongs before %d, %s", m_lines[a]->size, m_lines[a]->data, m_lines[b]->size, m_lines[b]->data);
        return true;
      }
    }
    else {
      if (a->data[i] < b->data[i]) {
        //printf ("2. %d, %s belongs before %d, %s", m_lines[a]->size, m_lines[a]->data, m_lines[b]->size, m_lines[b]->data);
        return true;
      }
    }
  }
  return false;
}
bool line_compare(ScintillaLine* a, ScintillaLine* b, bool ignore_case)
{
  if (!a || !b)
    return false;
  for (int i = 0; i < a->size && i < b->size; i++) {
    if (a->data[i] < b->data[i]) {
      //printf ("%c belongs before %c\n", m_lines[a]->data[i], m_lines[b]->data[i]);
      return true;
    }
  }
  return false;
}
*/
class ScintillaLineComparer : public std::binary_function<int, int, bool> 
{
private:
  const std::vector<ScintillaLine*>& m_lines;
public:
  bool m_ignore_case;
  bool m_compare_numeric;
  int m_start_tab;
  int m_start_character;
public:
  ScintillaLineComparer(const std::vector<ScintillaLine*>& lines) : m_lines(lines) {
    m_ignore_case = false;
    m_compare_numeric = false;
    m_start_tab = 0;
    m_start_character = 0;
  }

  bool operator()(const int a, const int b) const
  {
    unsigned int i;

    if (a < 0) {
      return false;
    }
    if (b < 0) {
      return false;
    }
    if ((const unsigned int) a >= m_lines.size()) {
      return false;
    }
    
    if ((const unsigned int) b >= m_lines.size()) {
      return false;
    } 
    int start_position_a = 0;
    int start_position_b = 0;
   
    if (m_start_tab > 0)
    {
      int tab_num_a = 0;
      int tab_pos_a = 0;
      int tab_num_b = 0;
      int tab_pos_b = 0;

      for (i = 0; i < m_lines[a]->size; i++) {
        if (m_lines[a]->data[i] == '\t') {
          tab_num_a += 1;
          tab_pos_a = i;
          if (tab_num_a == m_start_tab) {
            break;
          }
        }
      }
      for (i = 0; i < m_lines[b]->size; i++) {
        if (m_lines[b]->data[i] == '\t') {
          tab_num_b += 1;
          tab_pos_b = i;
          if (tab_num_b == m_start_tab) {
            break;
          }
        }
      }
      if (tab_num_a != tab_num_b) {
        return false;
      }
      if ((unsigned int) (tab_pos_a + 1) >= m_lines[a]->size) {
        return false;
      }
      if ((unsigned int) (tab_pos_b + 1) >= m_lines[b]->size) {
        return false;
      }
      start_position_a = (tab_pos_a + 1); // plus 1 for the tab
      start_position_b = (tab_pos_b + 1); // plus 1 for the tab
    }
    
    if (m_start_character > 0) {
      if ((unsigned int) (start_position_a + m_start_character) >= m_lines[a]->size) {
        return false;
      }
      if ((unsigned int) (start_position_b + m_start_character) >= m_lines[b]->size) {
        return false;
      }
      start_position_a += m_start_character;
      start_position_b += m_start_character;
    }
    if (m_compare_numeric)
    {
      int x_value = 0;
      int y_value = 0;
      for (i = 0; (i < m_lines[a]->size - start_position_a); i++)
      {
        int x = m_lines[a]->data[start_position_a + i];
        if (isdigit(x))
        {
          for (unsigned int j = i; (j < m_lines[a]->size - start_position_a); j++)
          {
            x = m_lines[a]->data[start_position_a + j];
            if (isdigit(x))
            {
              x_value *= 10;
              x_value += (x - 0x30);
            }
            else
            {
              break;
            }
          }
          break;
        }
      }
      for (i = 0; (i < m_lines[b]->size - start_position_b); i++)
      {
        int y = m_lines[b]->data[start_position_b + i];
        if (isdigit(y))
        { 
          for (unsigned int j = i; (j < m_lines[b]->size - start_position_b); j++)
          {
            y = m_lines[b]->data[start_position_b + j];
            if (isdigit(y))
            {
              y_value *= 10;
              y_value += (y - 0x30);
            }
            else
            {
              break;
            }
          }
          break;
        }
      }
      printf ("%d and %d\n", x_value, y_value);
      return (x_value < y_value) ? true : false;
    }
    if (m_ignore_case)
    {
      for (i = 0; (i < m_lines[a]->size - start_position_a) && (i < m_lines[b]->size - start_position_b); i++)
      {
        int x = m_lines[a]->data[start_position_a + i];
        int y = m_lines[b]->data[start_position_b + i];
        
        if (isalpha(x) && isalpha(y))
        {
          if (tolower(x) != tolower(y)) {
            return (tolower(x) < tolower(y)) ? true : false;
          }
        }
        else
        {
          if (x != y) {
            return (x < y) ? true : false;
          }
        }
      }
    }
    else
    {
      for (i = 0; (i < m_lines[a]->size - start_position_a) && (i < m_lines[b]->size - start_position_b); i++) {
        int x = m_lines[a]->data[start_position_a + i];
        int y = m_lines[b]->data[start_position_b + i];
        if (x != y) {
          return (x < y) ? true : false;
        }
      }
    }
    return false;
  }
};

///////////////////////////////////////////////////////////////////////////////

CLineSorter::CLineSorter()
{
  delete_duplicates = false;
  sort_lines = true;
  reverse_lines = false;
  ignore_case = false;
  compare_numeric = false;
  start_character = 0;
  start_tab = 0;
}

///////////////////////////////////////////////////////////////////////////////

CLineSorter::~CLineSorter()
{
  DestroyLines();
}

///////////////////////////////////////////////////////////////////////////////

void
CLineSorter::DestroyLines()
{
  for (unsigned int i = 0; i < m_Lines.size(); i++) {
    if (m_Lines[i]->data) {
      delete[] m_Lines[i]->data;
      m_Lines[i]->data = NULL;
      m_Lines[i]->size = 0;
    }
    delete m_Lines[i];
    m_Lines[i] = NULL;
  }
  m_Lines.clear();
}

///////////////////////////////////////////////////////////////////////////////

LRESULT
CLineSorter::SendEditor(UINT msg, WPARAM wparam, LPARAM lparam) {
  return ::SendMessage(m_Editor, msg, wparam, lparam);
}

///////////////////////////////////////////////////////////////////////////////
void
CLineSorter::SetStatusText(const char* string)
{
  GetStatusBar()->SetWindowText(string);
}

///////////////////////////////////////////////////////////////////////////////

void
CLineSorter::Sort()
{
  const int selection_start = SendEditor(SCI_GETSELECTIONSTART);
  const int selection_end = SendEditor(SCI_GETSELECTIONEND);
  int start_line = SendEditor(SCI_LINEFROMPOSITION, selection_start);
  int end_line   = SendEditor(SCI_LINEFROMPOSITION, selection_end);
  int i;

  if (start_line - end_line == 0) {
    SetStatusText("No lines selected...");
    return;
  }
  if (!delete_duplicates && !sort_lines && !reverse_lines) {
    return;
  }
  SetStatusText("Retrieving lines for sorting...");
  for (unsigned int line_number = start_line; line_number <= (unsigned int) end_line; line_number++) 
  {
    unsigned int line_index = m_Lines.size();
    ScintillaLine* line_ptr = new ScintillaLine;
    if (!line_ptr) {
      //SetStatusText("fail error 1...");
      return;
    }
    m_Lines.push_back(line_ptr);
    if (line_index + 1 != m_Lines.size()) {
      delete line_ptr;
      line_ptr = NULL;
      //SetStatusText("fail error 2...");
      return;
    }
    m_Lines[line_index]->data = NULL;
    m_Lines[line_index]->size = 0;
    int line_length = SendEditor(SCI_LINELENGTH, line_number);
    if (line_length >= 0) {
      m_Lines[line_index]->data = new char[line_length + 1];
      if (m_Lines[line_index]->data != NULL) {
        m_Lines[line_index]->size = line_length;
        SendEditor(SCI_GETLINE, line_number, (LRESULT)m_Lines[line_index]->data);
        m_Lines[line_index]->data[line_length] = '\0';
      } else {
        //SetStatusText("fail error 3...");
        return;
      }
    }
    else {
      //SetStatusText("fail error 4...");
      return;
    }
  }
  //SetStatusText("Removing old lines...");
  SendEditor(SCI_BEGINUNDOACTION);
  // remove the old selection
  SendEditor(SCI_SETTARGETSTART, SendEditor(SCI_POSITIONFROMLINE, start_line));
  SendEditor(SCI_SETTARGETEND,   SendEditor(SCI_POSITIONFROMLINE, end_line + 1));
  SendEditor(SCI_REPLACETARGET, 0, (LRESULT) "");
  std::vector<int> line_indexes;
  for (i = 0; (unsigned int) i < m_Lines.size(); i++) {
    line_indexes.push_back(i);
  }
  /*
    SendEditor(SCI_ADDTEXT, strlen("Before sort...\n"), (LRESULT)"Before sort...\n");
    for (i = 0; i < m_Lines.size() && i < line_indexes.size(); i++) {
      //unsigned// int line_index = line_indexes[i];
      if (m_Lines[line_index]->data) {
        SendEditor(SCI_ADDTEXT, m_Lines[line_index]->size, (LRESULT)m_Lines[line_index]->data);
      }
    }
  */
  /*
    printf("Before sorting\n");
    std::vector<int>::iterator it;
    int i = 0;
    for (it = line_indexes.begin(); it != line_indexes.end(); it++)
    {
      printf("line_indexes[%d] = %d\n", i, (*it));
      i++;
    }
  */
  SetStatusText("Sorting lines...");
  ScintillaLineComparer line_comparer(m_Lines);
  line_comparer.m_ignore_case = ignore_case;
  line_comparer.m_compare_numeric = compare_numeric;
  line_comparer.m_start_character = start_character;
  line_comparer.m_start_tab = start_tab;
  std::stable_sort(line_indexes.begin(), line_indexes.end(), line_comparer);
  SetStatusText("Lines sorted...");
  /*
    printf("After sorting\n");
    std::vector<int>::iterator it;
    int i = 0;
    for (it = line_indexes.begin(); it != line_indexes.end(); it++)
    {
      printf("line_indexes[%d] = %d\n", i, (*it));
      i++;
    }
  */
  
    int max_lines = m_Lines.size() - 1;
    int last_index = 0;
    int line_index;
    // SendEditor(SCI_ADDTEXT, strlen("after sort...\n"), (LPARAM)"after sort...\n");

    for (i = 0; (unsigned int)i < m_Lines.size() && (unsigned int)i < line_indexes.size(); i++, last_index = line_index) {
      line_index = (reverse_lines ? max_lines - i : i);
      if (sort_lines) {
        line_index = line_indexes[line_index];
      }
      if (m_Lines[line_index]->data) {
        if (i > 0 && delete_duplicates) {
          if (m_Lines[line_index]->size == m_Lines[last_index]->size) {
            if (memcmp(m_Lines[line_index]->data,
                       m_Lines[last_index]->data,
                       m_Lines[line_index]->size) == 0) {
              continue;
            }
          }
        }
        SendEditor(SCI_ADDTEXT, m_Lines[line_index]->size, (LRESULT)m_Lines[line_index]->data);
      }
    }
  
  SendEditor(SCI_SETSELECTIONSTART, selection_start);
  SendEditor(SCI_SETSELECTIONEND, selection_end);
  SendEditor(SCI_ENDUNDOACTION);
  DestroyLines();
  SetStatusText("");
}

///////////////////////////////////////////////////////////////////////////////
