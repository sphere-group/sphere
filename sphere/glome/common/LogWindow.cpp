#include <string.h>
#include "LogWindow.hpp"


#define STRING_HEIGHT 16


////////////////////////////////////////////////////////////////////////////////

CLogWindow::CLogWindow(HINSTANCE instance, const char* caption)
{
  // zero out the strings
  m_TopString = 0;
  m_NumStrings = 0;
  memset(m_CurrentString, 0, sizeof(m_CurrentString));
  memset(m_Strings, 0, sizeof(m_Strings));

  InitializeCriticalSection(&StringManagement);

  // thread routine needs access to parameters
  m_Instance = instance;
  m_Caption  = new char[strlen(caption) + 1];
  strcpy(m_Caption, caption);

  // create thread
  DWORD thread_id;
  CreateThread(
    NULL,
    0,
    ThreadRoutine,
    this,
    0,
    &thread_id);
}

////////////////////////////////////////////////////////////////////////////////

CLogWindow::~CLogWindow()
{
  // tell thread to shut down


  delete[] m_Caption;
}

////////////////////////////////////////////////////////////////////////////////

void
CLogWindow::WriteString(const char* string)
{
  // combine the two strings
  char* concatenated = new char[strlen(m_CurrentString) + strlen(string) + 1];
  char* delete_me = concatenated;
  strcpy(concatenated, m_CurrentString);
  strcat(concatenated, string);

  // remove sections that end with \n and send them to the window
  while (strchr(concatenated, '\n'))
  {
    *strchr(concatenated, '\n') = 0;
    AddString(concatenated);
    concatenated += strlen(concatenated) + 1;
  }

  // cut out chunks that are bigger than the max string width
  while (strlen(concatenated) > MAX_STRING_LENGTH)
  {
    // grab the part we're going to send to the window
    char deleted = concatenated[MAX_STRING_LENGTH];
    concatenated[MAX_STRING_LENGTH] = 0;
    AddString(concatenated);

    // advance the string pointer to the next chunk
    concatenated += MAX_STRING_LENGTH;
    concatenated[0] = deleted;
  }

  // put the rest of the string into the current string
  strcpy(m_CurrentString, concatenated);

  delete[] delete_me;
}

////////////////////////////////////////////////////////////////////////////////

void
CLogWindow::AddString(const char* string)
{
  EnterCriticalSection(&StringManagement);

  // if we have reached the end, scroll all other strings back
  if (m_NumStrings == MAX_STRINGS)
  {
    for (int i = 0; i < MAX_STRINGS - 1; i++)
      strcpy(m_Strings[i], m_Strings[i + 1]);
    strcpy(m_Strings[MAX_STRINGS - 1], string);
  }
  // otherwise, stick the string on the end
  else
  {
    strcpy(m_Strings[m_NumStrings], string);
    m_NumStrings++;
  }

  LeaveCriticalSection(&StringManagement);

  InvalidateRect(m_Window, NULL, TRUE);

  RECT ClientRect;
  GetClientRect(m_Window, &ClientRect);
  int page_size = ClientRect.bottom / STRING_HEIGHT;
  UpdateScrollBar(page_size);
}

////////////////////////////////////////////////////////////////////////////////

DWORD WINAPI
CLogWindow::ThreadRoutine(LPVOID parameter)
{
  CLogWindow* This = (CLogWindow*)parameter;

  // register window class
  WNDCLASS wc;
  memset(&wc, 0, sizeof(wc));
  wc.lpfnWndProc   = WindowProc;
  wc.hInstance     = This->m_Instance;
  wc.hIcon         = NULL;
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = "LogWindowClass";
  RegisterClass(&wc);

  // get the font so the window can use it
  This->m_Font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

  // create window
  This->m_Window = CreateWindowEx(
    WS_EX_CLIENTEDGE,
    "LogWindowClass",
    This->m_Caption,
    WS_OVERLAPPEDWINDOW | WS_VSCROLL,
    CW_USEDEFAULT, 0,
    CW_USEDEFAULT, 0,
    NULL,
    NULL,
    This->m_Instance,
    NULL);

  // attach this object to the window
  SetWindowLong(This->m_Window, GWL_USERDATA, (LONG)This);

  if (This->m_Window == NULL)
    return 0;

  // display window
  ShowWindow(This->m_Window, SW_SHOW);
  UpdateWindow(This->m_Window);

  // message loop
  MSG msg;
  while (GetMessage(&msg, This->m_Window, 0, 0) > 0)
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK
CLogWindow::WindowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
  // get log window object associated with window
  CLogWindow* This = (CLogWindow*)GetWindowLong(window, GWL_USERDATA);

  switch (message)
  {
    case WM_VSCROLL:
      This->OnVScroll(LOWORD(wparam), HIWORD(wparam));
      return 0;

    ////////////////////////////////////////////////////////////////////////////

    case WM_SIZE:
      This->OnSize(LOWORD(lparam), HIWORD(lparam));
      return 0;

    ////////////////////////////////////////////////////////////////////////////

    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      BeginPaint(window, &ps);
      This->OnPaint(ps.hdc);
      EndPaint(window, &ps);
      return 0;
    }

    ////////////////////////////////////////////////////////////////////////////

    default:
      return DefWindowProc(window, message, wparam, lparam);
  }
}

////////////////////////////////////////////////////////////////////////////////

void
CLogWindow::OnVScroll(int code, int pos)
{
  RECT ClientRect;
  GetClientRect(m_Window, &ClientRect);
  int PageSize = ClientRect.bottom / STRING_HEIGHT;

  switch (code)
  {
    case SB_LINEDOWN:      m_TopString++;                         break;
    case SB_LINEUP:        m_TopString--;                         break;
    case SB_PAGEDOWN:      m_TopString += PageSize;               break;
    case SB_PAGEUP:        m_TopString -= PageSize;               break;
    case SB_TOP:           m_TopString = 0;                       break;
    case SB_BOTTOM:        m_TopString = m_NumStrings - PageSize; break;
    case SB_THUMBPOSITION: m_TopString = pos;                     break;
    case SB_THUMBTRACK:    m_TopString = pos;                     break;
  }

  BracketTopString(0, m_NumStrings - PageSize);
  UpdateScrollBar(PageSize);
  InvalidateRect(m_Window, NULL, TRUE);
}

////////////////////////////////////////////////////////////////////////////////

void
CLogWindow::OnSize(int cx, int cy)
{
  int page_size = cy / STRING_HEIGHT;
  BracketTopString(0, m_NumStrings - page_size);
  UpdateScrollBar(page_size);
  InvalidateRect(m_Window, NULL, TRUE);
}

////////////////////////////////////////////////////////////////////////////////

void
CLogWindow::OnPaint(HDC dc)
{
  EnterCriticalSection(&StringManagement);

  RECT ClientRect;
  GetClientRect(m_Window, &ClientRect);

  // initialize the DC
  SaveDC(dc);
  SetTextColor(dc, RGB(0, 0, 0));
  SetBkMode(dc, TRANSPARENT);
  SelectObject(dc, m_Font);

  int visible_strings = ClientRect.bottom / STRING_HEIGHT + 1;
  
  // draw the strings
  for (int i = m_TopString; i < m_TopString + visible_strings; i++)
    if (i < m_NumStrings)
    {
      RECT TextRect;
      TextRect.top    = (i - m_TopString) * STRING_HEIGHT;
      TextRect.left   = 0;
      TextRect.bottom = TextRect.top + STRING_HEIGHT;
      TextRect.right  = ClientRect.right;
      DrawText(dc, m_Strings[i], -1, &TextRect, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_VCENTER);
    }

  // restore the DC and end the painting operation
  RestoreDC(dc, -1);

  LeaveCriticalSection(&StringManagement);
}

////////////////////////////////////////////////////////////////////////////////

void
CLogWindow::BracketTopString(int min, int max)
{
  if (m_TopString > max)
    m_TopString = max;
  if (m_TopString < min)
    m_TopString = min;
}

////////////////////////////////////////////////////////////////////////////////

void
CLogWindow::UpdateScrollBar(int page_size)
{
  SCROLLINFO si;
  si.cbSize = sizeof(si);
  si.fMask  = SIF_ALL;
  si.nMin   = 0;
  si.nMax   = m_NumStrings;
  si.nPage  = page_size;
  si.nPos   = m_TopString;
  SetScrollInfo(m_Window, SB_VERT, &si, TRUE);
}

////////////////////////////////////////////////////////////////////////////////
