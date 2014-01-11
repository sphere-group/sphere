#pragma warning(disable : 4786)
#include "WindowStyleWindow.hpp"
#include "ResizeDialog.hpp"
#include "FileDialogs.hpp"
#include "WindowStylePropertiesDialog.hpp"
#include "Editor.hpp"
#include "../common/minmax.hpp"
#include "resource.h"
const int ID_ALPHASLIDER = 7001;
const int ID_ALPHASTATIC = 7002;
static const int WINDOWSTYLE_TIMER = 9001;
#ifdef USE_SIZECBAR
IMPLEMENT_DYNAMIC(CWindowStyleWindow, CMDIChildWnd)
#endif
BEGIN_MESSAGE_MAP(CWindowStyleWindow, CSaveableDocumentWindow)
  ON_WM_LBUTTONDOWN()
  ON_WM_SIZE()
  ON_WM_VSCROLL()
  ON_WM_PAINT()
  ON_WM_KEYDOWN()
  ON_WM_KEYUP()
  ON_WM_TIMER()
  ON_COMMAND(ID_WINDOWSTYLE_EDIT_UPPERLEFT,  OnEditUpperLeft)
  ON_COMMAND(ID_WINDOWSTYLE_EDIT_TOP,        OnEditTop)
  ON_COMMAND(ID_WINDOWSTYLE_EDIT_UPPERRIGHT, OnEditUpperRight)
  ON_COMMAND(ID_WINDOWSTYLE_EDIT_RIGHT,      OnEditRight)
  ON_COMMAND(ID_WINDOWSTYLE_EDIT_LOWERRIGHT, OnEditLowerRight)
  ON_COMMAND(ID_WINDOWSTYLE_EDIT_BOTTOM,     OnEditBottom)
  ON_COMMAND(ID_WINDOWSTYLE_EDIT_LOWERLEFT,  OnEditLowerLeft)
  ON_COMMAND(ID_WINDOWSTYLE_EDIT_LEFT,       OnEditLeft)
  ON_COMMAND(ID_WINDOWSTYLE_EDIT_BACKGROUND, OnEditBackground)
  ON_COMMAND(ID_WINDOWSTYLE_RESIZESECTION,    OnResizeSection)
  ON_COMMAND(ID_WINDOWSTYLE_RESCALESECTION,   OnRescaleSection)
  ON_COMMAND(ID_WINDOWSTYLE_RESAMPLESECTION,  OnResampleSection)
  ON_COMMAND(ID_WINDOWSTYLE_ZOOM_1X, OnZoom1x)
  ON_COMMAND(ID_WINDOWSTYLE_ZOOM_2X, OnZoom2x)
  ON_COMMAND(ID_WINDOWSTYLE_ZOOM_4X, OnZoom4x)
  ON_COMMAND(ID_WINDOWSTYLE_ZOOM_8X, OnZoom8x)
  ON_COMMAND(ID_FILE_ZOOM_IN, OnZoomIn)
  ON_COMMAND(ID_FILE_ZOOM_OUT, OnZoomOut)
  ON_COMMAND(ID_FILE_COPY,  OnCopy)
  ON_COMMAND(ID_FILE_PASTE, OnPaste)
  ON_COMMAND(ID_FILE_UNDO,  OnUndo)
  ON_COMMAND(ID_FILE_REDO,  OnRedo)
  ON_COMMAND(ID_WINDOWSTYLE_PROPERTIES, OnProperties)
  ON_UPDATE_COMMAND_UI(ID_WINDOWSTYLE_EDIT_UPPERLEFT,  OnUpdateEditUpperLeft)
  ON_UPDATE_COMMAND_UI(ID_WINDOWSTYLE_EDIT_TOP,        OnUpdateEditTop)
  ON_UPDATE_COMMAND_UI(ID_WINDOWSTYLE_EDIT_UPPERRIGHT, OnUpdateEditUpperRight)
  ON_UPDATE_COMMAND_UI(ID_WINDOWSTYLE_EDIT_RIGHT,      OnUpdateEditRight)
  ON_UPDATE_COMMAND_UI(ID_WINDOWSTYLE_EDIT_LOWERRIGHT, OnUpdateEditLowerRight)
  ON_UPDATE_COMMAND_UI(ID_WINDOWSTYLE_EDIT_BOTTOM,     OnUpdateEditBottom)
  ON_UPDATE_COMMAND_UI(ID_WINDOWSTYLE_EDIT_LOWERLEFT,  OnUpdateEditLowerLeft)
  ON_UPDATE_COMMAND_UI(ID_WINDOWSTYLE_EDIT_LEFT,       OnUpdateEditLeft)
  ON_UPDATE_COMMAND_UI(ID_WINDOWSTYLE_EDIT_BACKGROUND, OnUpdateEditBackground)
  ON_UPDATE_COMMAND_UI(ID_WINDOWSTYLE_ZOOM_1X, OnUpdateZoom1x)
  ON_UPDATE_COMMAND_UI(ID_WINDOWSTYLE_ZOOM_2X, OnUpdateZoom2x)
  ON_UPDATE_COMMAND_UI(ID_WINDOWSTYLE_ZOOM_4X, OnUpdateZoom4x)
  ON_UPDATE_COMMAND_UI(ID_WINDOWSTYLE_ZOOM_8X, OnUpdateZoom8x)
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////
CWindowStyleWindow::CWindowStyleWindow(const char* window_style)
: CSaveableDocumentWindow(window_style, IDR_WINDOWSTYLE, CSize(200, 150))
, m_Created(false)
, m_SelectedBitmap(sWindowStyle::UPPER_LEFT)
, m_ZoomFactor(4)
, m_HighlightPen(new CPen(PS_SOLID, 1, 0xFF00FF))
{
  m_DocumentType = WA_WINDOWSTYLE;
  // allocate DIB sections and empty them
  for (int i = 0; i < 9; i++)
    m_DIBs[i] = NULL;
  if (window_style) {
    // load the window style
    if (m_WindowStyle.Load(window_style) == false)
    {
      char string[1024 + MAX_PATH];
      sprintf (string, "Could not load window style '%s'\nCreating new.", window_style);
      MessageBox(string);
      m_WindowStyle.Create(16, 16);
    }
    UpdateDIBSections();
    SetSaved(true);
    SetModified(false);
  } else {
    m_WindowStyle.Create(16, 16);
    UpdateDIBSections();
    SetSaved(false);
    SetModified(false);
  }
  // create the window and child widgets
  Create(AfxRegisterWndClass(0, ::LoadCursor(NULL, IDC_ARROW), NULL, AfxGetApp()->LoadIcon(IDI_WINDOWSTYLE)));
  m_ImageView.Create(this, this, this);
  m_PaletteView.Create(this, this);
  m_ColorView.SetNumColors(2);
  m_ColorView.Create(this, this);
  m_AlphaView.Create(this, this);
	m_WindowStylePreviewPalette = new CWindowStylePreviewPalette(this, &m_WindowStyle);

	SetBitmap();
  // we're done creating the windows, so make sure everything is in the right place
  m_Created = true;
  RECT ClientRect;
  GetClientRect(&ClientRect);
  OnSize(0, ClientRect.right, ClientRect.bottom);
  // make sure the various views start with matching values
  m_ColorView.SetColor(0, CreateRGB(255, 255, 255));
  m_ColorView.SetColor(1, CreateRGB(0, 0, 0));
  m_ImageView.SetColor(0, CreateRGBA(255, 255, 255, 255));
  m_ImageView.SetColor(1, CreateRGBA(0, 0, 0, 255));
  m_AlphaView.SetAlpha(255);
#ifdef USE_SIZECBAR
	LoadPaletteStates();
#endif

  m_Timer = SetTimer(WINDOWSTYLE_TIMER, 100, NULL);
}
////////////////////////////////////////////////////////////////////////////////
CWindowStyleWindow::~CWindowStyleWindow()
{
  for (int i = 0; i < 9; i++)
    delete m_DIBs[i];
  if (m_WindowStylePreviewPalette)
		m_WindowStylePreviewPalette->Destroy();
  if (m_HighlightPen) m_HighlightPen->DeleteObject();
  delete m_HighlightPen;
  m_HighlightPen = NULL;
}
////////////////////////////////////////////////////////////////////////////////
void
CWindowStyleWindow::UpdateDIBSection(int bitmap)
{
  // delete the old DIB section and allocate a new one
  delete m_DIBs[bitmap];
  int dib_width  = m_WindowStyle.GetBitmap(bitmap).GetWidth()  * m_ZoomFactor;
  int dib_height = m_WindowStyle.GetBitmap(bitmap).GetHeight() * m_ZoomFactor;
  m_DIBs[bitmap] = new CDIBSection(dib_width, dib_height, 32);

  if (!m_DIBs[bitmap] || m_DIBs[bitmap]->GetPixels() == NULL)
    return;
  // fill the DIB with data
  BGRA* dest  = (BGRA*)m_DIBs[bitmap]->GetPixels();
  CImage32& b = m_WindowStyle.GetBitmap(bitmap);
  RGBA* src   = b.GetPixels();
  for (int by = 0; by < b.GetHeight(); by++)
    for (int bx = 0; bx < b.GetWidth(); bx++)
    {
      // get the pixel to draw
      RGBA rgba = src[by * b.GetWidth() + bx];
      BGRA bgra =
      {
        rgba.blue  = rgba.alpha * rgba.blue  / 256,
        rgba.green = rgba.alpha * rgba.green / 256,
        rgba.red   = rgba.alpha * rgba.red   / 256,
        0,
      };
      // fill the square with it
      for (int dy = 0; dy < m_ZoomFactor; dy++)
        for (int dx = 0; dx < m_ZoomFactor; dx++)
        {
          dest[(by * m_ZoomFactor + dy) * dib_width + (bx * m_ZoomFactor + dx)] = bgra;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
void
CWindowStyleWindow::UpdateDIBSections()
{
  for (int i = 0; i < 9; i++)
    UpdateDIBSection(i);
}
////////////////////////////////////////////////////////////////////////////////
void
CWindowStyleWindow::SetBitmap()
{
  CImage32& b = m_WindowStyle.GetBitmap(m_SelectedBitmap);
  m_ImageView.SetImage(b.GetWidth(), b.GetHeight(), b.GetPixels(), true);
}
////////////////////////////////////////////////////////////////////////////////
void
CWindowStyleWindow::SetZoomFactor(int factor)
{
  m_ZoomFactor = factor;
  // resize the DIBs
  UpdateDIBSections();
  Invalidate();
  // move everything to its correct place
  RECT Rect;
  GetClientRect(&Rect);
  OnSize(0, Rect.right, Rect.bottom);
}
////////////////////////////////////////////////////////////////////////////////
void
CWindowStyleWindow::SelectBitmap(int bitmap)
{
  if (m_SelectedBitmap != bitmap) {
    m_SelectedBitmap = bitmap;
    SetBitmap();
    Invalidate();
  }
}
////////////////////////////////////////////////////////////////////////////////
static void BlackRect(CDC& dc, int x, int y, int w, int h)
{
  RECT Rect = { x, y, x + w, y + h };
  dc.FillRect(&Rect, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
}
////////////////////////////////////////////////////////////////////////////////
void
CWindowStyleWindow::DrawCorner(CDC& dc, int bitmap, int x, int y, int w, int h)
{
  CDIBSection* dib = m_DIBs[bitmap];
  if (!dib || dib->GetPixels() == NULL) {
    return;
  }
  // calculate correct corner offsets so the the images are flush with the image editor
  int offsetx = 0;
  int offsety = 0;
  switch (bitmap)
  {
    case sWindowStyle::UPPER_LEFT:
      offsetx = w - dib->GetWidth();
      offsety = h - dib->GetHeight();
      break;
    case sWindowStyle::UPPER_RIGHT:
      offsetx = 0;
      offsety = h - dib->GetHeight();
      break;
    case sWindowStyle::LOWER_LEFT:
      offsetx = w - dib->GetWidth();
      offsety = 0;
      break;
    case sWindowStyle::LOWER_RIGHT:
      offsetx = 0;
      offsety = 0;
      break;
  }
  // create a clipping region for the DIB and select it in
  CRgn region;
  region.CreateRectRgn(
    offsetx + x,
    offsety + y,
    offsetx + x + dib->GetWidth(),
    offsety + y + dib->GetHeight());
  dc.SelectClipRgn(&region);
  // draw the DIB
  dc.BitBlt(
    x + offsetx, y + offsety, dib->GetWidth(), dib->GetHeight(),
    CDC::FromHandle(dib->GetDC()),
    0, 0, SRCCOPY);
  // select the region opposite of the previous one
  dc.SelectClipRgn(NULL);
  dc.SelectClipRgn(&region, RGN_XOR);
  // fill the rest of the area with black
  BlackRect(dc, x, y, w, h);
  // remove the clipping region
  dc.SelectClipRgn(NULL);
  region.DeleteObject();
  // if it's the selected bitmap, put a pink rectangle around it
  if (m_HighlightPen && bitmap == m_SelectedBitmap)
  {
    dc.SaveDC();
    dc.SelectObject(m_HighlightPen);
    dc.SelectStockObject(NULL_BRUSH);
    dc.Rectangle(offsetx + x, offsety + y, x + offsetx + dib->GetWidth(), offsety + y + dib->GetHeight());
    dc.RestoreDC(-1);
  }
}
////////////////////////////////////////////////////////////////////////////////
void
CWindowStyleWindow::DrawEdgeH(CDC& dc, int bitmap, int x, int y, int x2, int h)
{
  CDIBSection* dib = m_DIBs[bitmap];
  if (!dib || dib->GetPixels() == NULL) {
    return;
  }
  // calculate correct edge offsets so the the images are flush with the image editor
  int offsety = 0;
  switch (bitmap)
  {
    case sWindowStyle::TOP:
      offsety = h - dib->GetHeight();
      break;
    case sWindowStyle::BOTTOM:
      offsety = 0;
      break;
  }
  // create a clipping region for the DIBs
  CRgn region;
  region.CreateRectRgn(
    x,
    offsety + y,
    x2,
    offsety + y + dib->GetHeight());
  dc.SelectClipRgn(&region);
  // draw the edge
  int x1 = x;
  while (x1 < x2)
  {
    dc.BitBlt(
      x1, offsety + y, dib->GetWidth(), dib->GetHeight(),
      CDC::FromHandle(dib->GetDC()),
      0, 0, SRCCOPY);
    x1 += dib->GetWidth();
  }
  // select the clipping region opposite of the previous one
  dc.SelectClipRgn(NULL);
  dc.SelectClipRgn(&region, RGN_XOR);
  // fill rest of area with black
  BlackRect(dc, x, y, x2 - x, h);
  // reset the clipping rectangle
  dc.SelectClipRgn(NULL);
  region.DeleteObject();
  // if bitmap is selected, draw pink selection square
  if (m_HighlightPen && bitmap == m_SelectedBitmap)
  {
    dc.SaveDC();
    dc.SelectObject(m_HighlightPen);
    dc.SelectStockObject(NULL_BRUSH);
    dc.Rectangle(x, offsety + y, x2, offsety + y + dib->GetHeight());
    dc.RestoreDC(-1);
  }
}
////////////////////////////////////////////////////////////////////////////////
void
CWindowStyleWindow::DrawEdgeV(CDC& dc, int bitmap, int x, int y, int y2, int w)
{
  CDIBSection* dib = m_DIBs[bitmap];
  if (!dib || dib->GetPixels() == NULL) {
    return;
  }
  // calculate correct edge offsets so the the images are flush with the image editor
  int offsetx = 0;
  switch (bitmap)
  {
    case sWindowStyle::LEFT:
      offsetx = w - dib->GetWidth();
      break;
    case sWindowStyle::RIGHT:
      offsetx = 0;
      break;
  }
  // create a clipping region for the DIBs
  CRgn region;
  region.CreateRectRgn(offsetx + x, y, offsetx + x + dib->GetWidth(), y2);
  dc.SelectClipRgn(&region);
  // draw the edge
  int y1 = y;
  while (y1 < y2)
  {
    dc.BitBlt(
      offsetx + x, y1, dib->GetWidth(), dib->GetHeight(),
      CDC::FromHandle(dib->GetDC()),
      0, 0, SRCCOPY);
    y1 += dib->GetHeight();
  }
  // select the clipping region opposite of the previous one
  dc.SelectClipRgn(NULL);
  dc.SelectClipRgn(&region, RGN_XOR);
  // fill the area with black
  BlackRect(dc, x, y, w, y2 - y);
  // reset the clipping rectangle
  dc.SelectClipRgn(NULL);
  region.DeleteObject();
  // if bitmap is selected, draw pink selection square
  if (m_HighlightPen && bitmap == m_SelectedBitmap)
  {
    dc.SaveDC();
    dc.SelectObject(m_HighlightPen);
    dc.SelectStockObject(NULL_BRUSH);
    dc.Rectangle(offsetx + x, y, offsetx + x + dib->GetWidth(), y2);
    dc.RestoreDC(-1);
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnLButtonDown(UINT flags, CPoint point)
{
  RECT EditRect;
  GetEditRect(&EditRect);
  if (point.x < EditRect.left &&
      point.y < EditRect.top)
    SelectBitmap(sWindowStyle::UPPER_LEFT);
  else if (point.x >= EditRect.right &&
           point.y < EditRect.top)
    SelectBitmap(sWindowStyle::UPPER_RIGHT);
  else if (point.x < EditRect.left &&
           point.y >= EditRect.bottom)
    SelectBitmap(sWindowStyle::LOWER_LEFT);
  else if (point.x >= EditRect.right &&
           point.y >= EditRect.bottom)
    SelectBitmap(sWindowStyle::LOWER_RIGHT);
  else if (point.x < EditRect.left)
    SelectBitmap(sWindowStyle::LEFT);
  else if (point.y < EditRect.top)
    SelectBitmap(sWindowStyle::TOP);
  else if (point.x >= EditRect.right)
    SelectBitmap(sWindowStyle::RIGHT);
  else if (point.y >= EditRect.bottom)
    SelectBitmap(sWindowStyle::BOTTOM);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnKeyDown(UINT vk, UINT repeat, UINT flags)
{
  m_ImageView.OnKeyDown(vk, repeat, flags);
}
afx_msg void
CWindowStyleWindow::OnKeyUp(UINT vk, UINT repeat, UINT flags)
{
  m_ImageView.OnKeyUp(vk, repeat, flags);
}
afx_msg void
CWindowStyleWindow::OnTimer(UINT event)
{
  m_ImageView.OnTimer(event);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnSize(UINT type, int cx, int cy)
{
  if (m_Created)
  {
    RECT EditRect;
    GetEditRect(&EditRect);
    m_ImageView.MoveWindow(&EditRect);
    m_PaletteView.MoveWindow(cx - 60 - 32, 0, 60, cy - 60);
    m_ColorView.MoveWindow(cx - 60 - 32, cy - 60, 60, 60);
    m_AlphaView.MoveWindow(cx - 32, 0, 32, cy);
    Invalidate();
  }
  CSaveableDocumentWindow::OnSize(type, cx, cy);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnPaint()
{
  CPaintDC dc(this);
  RECT EditRect;
  GetEditRect(&EditRect);
  // corners
  DrawCorner(dc, sWindowStyle::UPPER_LEFT,  0, 0,
    GetBorderWidth_Left(),
    GetBorderWidth_Top());
  DrawCorner(dc, sWindowStyle::UPPER_RIGHT, EditRect.right, 0,
    GetBorderWidth_Right(),
    GetBorderWidth_Top());
  DrawCorner(dc, sWindowStyle::LOWER_LEFT,  0, EditRect.bottom,
    GetBorderWidth_Left(),
    GetBorderWidth_Bottom());
  DrawCorner(dc, sWindowStyle::LOWER_RIGHT, EditRect.right, EditRect.bottom,
    GetBorderWidth_Right(),
    GetBorderWidth_Bottom());
  // edges
  DrawEdgeH(dc, sWindowStyle::TOP,    EditRect.left, 0, EditRect.right, GetBorderWidth_Top());
  DrawEdgeH(dc, sWindowStyle::BOTTOM, EditRect.left, EditRect.bottom, EditRect.right, GetBorderWidth_Bottom());
  DrawEdgeV(dc, sWindowStyle::LEFT,   0, EditRect.top, EditRect.bottom, GetBorderWidth_Left());
  DrawEdgeV(dc, sWindowStyle::RIGHT,  EditRect.right, EditRect.top, EditRect.bottom, GetBorderWidth_Right());
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnEditUpperLeft()
{
  SelectBitmap(sWindowStyle::UPPER_LEFT);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnEditTop()
{
  SelectBitmap(sWindowStyle::TOP);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnEditUpperRight()
{
  SelectBitmap(sWindowStyle::UPPER_RIGHT);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnEditRight()
{
  SelectBitmap(sWindowStyle::RIGHT);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnEditLowerRight()
{
  SelectBitmap(sWindowStyle::LOWER_RIGHT);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnEditBottom()
{
  SelectBitmap(sWindowStyle::BOTTOM);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnEditLowerLeft()
{
  SelectBitmap(sWindowStyle::LOWER_LEFT);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnEditLeft()
{
  SelectBitmap(sWindowStyle::LEFT);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnEditBackground()
{
  SelectBitmap(sWindowStyle::BACKGROUND);
}
////////////////////////////////////////////////////////////////////////////////
void
CWindowStyleWindow::ResizeSection(int width, int height, int method)
{
  CImage32& bitmap = m_WindowStyle.GetBitmap(m_SelectedBitmap);
  switch (method) {
    case 0: bitmap.Resize(width, height);   break;
    case 1: bitmap.Rescale(width, height);  break;
    case 2: bitmap.Resample(width, height); break;
  }
  SetModified(true);
  // update the window
  SetBitmap();
  UpdateDIBSections();
  Invalidate();
  // resize the window
  RECT Rect;
  GetClientRect(&Rect);
  OnSize(0, Rect.right, Rect.bottom);
  if (m_WindowStylePreviewPalette)
    m_WindowStylePreviewPalette->OnWindowStyleChanged();
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnResizeSection()
{
  CImage32& b = m_WindowStyle.GetBitmap(m_SelectedBitmap);
  CResizeDialog Dialog("Resize Window Style Section", b.GetWidth(), b.GetHeight());
  Dialog.SetRange(1, 1024, 1, 1024);
  if (Dialog.DoModal() == IDOK)
  {
    ResizeSection(Dialog.GetWidth(), Dialog.GetHeight(), 0);
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnRescaleSection()
{
  CImage32& b = m_WindowStyle.GetBitmap(m_SelectedBitmap);
  CResizeDialog Dialog("Rescale Window Style Section", b.GetWidth(), b.GetHeight());
  Dialog.SetRange(1, 1024, 1, 1024);
  if (Dialog.DoModal() == IDOK)
  {
    ResizeSection(Dialog.GetWidth(), Dialog.GetHeight(), 1);
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnResampleSection()
{
  CImage32& b = m_WindowStyle.GetBitmap(m_SelectedBitmap);
  CResizeDialog Dialog("Resample Window Style Section", b.GetWidth(), b.GetHeight());
  Dialog.SetRange(1, 1024, 1, 1024);
  if (Dialog.DoModal() == IDOK)
  {
    ResizeSection(Dialog.GetWidth(), Dialog.GetHeight(), 2);
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnZoom1x()
{
  SetZoomFactor(1);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnZoom2x()
{
  SetZoomFactor(2);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnZoom4x()
{
  SetZoomFactor(4);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnZoom8x()
{
  SetZoomFactor(8);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnZoomIn()
{
  bool focus_found = false;
  if (GetFocus() != this) {
    if ((m_WindowStylePreviewPalette != NULL) && (GetFocus() == m_WindowStylePreviewPalette)) {
      m_WindowStylePreviewPalette->SendMessage(WM_COMMAND, MAKEWPARAM(ID_FILE_ZOOM_IN, 0), 0);
      focus_found = true;
    }
  }

  if (!focus_found) {
    switch ((int)m_ZoomFactor) {
      case 1: SetZoomFactor(2); break;
      case 2: SetZoomFactor(4); break;
      case 4: SetZoomFactor(8); break;
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnZoomOut()
{
  bool focus_found = false;
  if (GetFocus() != this) {
    if ((m_WindowStylePreviewPalette != NULL) && (GetFocus() == m_WindowStylePreviewPalette)) {
      m_WindowStylePreviewPalette->SendMessage(WM_COMMAND, MAKEWPARAM(ID_FILE_ZOOM_OUT, 0), 0);
      focus_found = true;
    }
  }

  if (!focus_found) {
    switch ((int)m_ZoomFactor) {
      case 2: SetZoomFactor(1); break;
      case 4: SetZoomFactor(2); break;
      case 8: SetZoomFactor(4); break;
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnCopy()
{
  m_ImageView.SendMessage(WM_COMMAND, MAKEWPARAM(ID_IMAGEVIEW_COPY, 0), 0);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnPaste()
{
  m_ImageView.SendMessage(WM_COMMAND, MAKEWPARAM(ID_IMAGEVIEW_PASTE, 0), 0);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnUndo()
{
  m_ImageView.SendMessage(WM_COMMAND, MAKEWPARAM(ID_IMAGEVIEW_UNDO, 0), 0);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnRedo()
{
  m_ImageView.SendMessage(WM_COMMAND, MAKEWPARAM(ID_IMAGEVIEW_REDO, 0), 0);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnProperties()
{
  CWindowStylePropertiesDialog dialog(&m_WindowStyle);
  if (dialog.DoModal() == IDOK) {
    SetModified(true);
  }
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnUpdateEditUpperLeft(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_SelectedBitmap == sWindowStyle::UPPER_LEFT);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnUpdateEditTop(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_SelectedBitmap == sWindowStyle::TOP);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnUpdateEditUpperRight(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_SelectedBitmap == sWindowStyle::UPPER_RIGHT);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnUpdateEditRight(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_SelectedBitmap == sWindowStyle::RIGHT);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnUpdateEditLowerRight(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_SelectedBitmap == sWindowStyle::LOWER_RIGHT);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnUpdateEditBottom(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_SelectedBitmap == sWindowStyle::BOTTOM);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnUpdateEditLowerLeft(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_SelectedBitmap == sWindowStyle::LOWER_LEFT);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnUpdateEditLeft(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_SelectedBitmap == sWindowStyle::LEFT);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnUpdateEditBackground(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_SelectedBitmap == sWindowStyle::BACKGROUND);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnUpdateZoom1x(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_ZoomFactor == 1);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnUpdateZoom2x(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_ZoomFactor == 2);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnUpdateZoom4x(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_ZoomFactor == 4);
}
////////////////////////////////////////////////////////////////////////////////
afx_msg void
CWindowStyleWindow::OnUpdateZoom8x(CCmdUI* cmdui)
{
  cmdui->SetCheck(m_ZoomFactor == 8);
}
////////////////////////////////////////////////////////////////////////////////
bool
CWindowStyleWindow::GetSavePath(char* path)
{
  std::string directory = GetMainWindow()->GetDefaultFolder(m_DocumentType);
  SetCurrentDirectory(directory.c_str());
  CWindowStyleFileDialog Dialog(FDM_SAVE);
  // set current directory on Win98/2000
  Dialog.m_ofn.lpstrInitialDir = directory.c_str();
  if (Dialog.DoModal() != IDOK)
    return false;
  strcpy(path, Dialog.GetPathName());
  return true;
}
////////////////////////////////////////////////////////////////////////////////
bool
CWindowStyleWindow::SaveDocument(const char* path)
{
  return m_WindowStyle.Save(path);
}
////////////////////////////////////////////////////////////////////////////////
void
CWindowStyleWindow::IV_ImageChanged()
{
  CImage32& bitmap = m_WindowStyle.GetBitmap(m_SelectedBitmap);
  memcpy(
    bitmap.GetPixels(),
    m_ImageView.GetPixels(),
    bitmap.GetWidth() * bitmap.GetHeight() * sizeof(RGBA));
  UpdateDIBSection(m_SelectedBitmap);
  Invalidate();
  if (m_WindowStylePreviewPalette)
    m_WindowStylePreviewPalette->OnWindowStyleChanged();
  SetModified(true);
}
////////////////////////////////////////////////////////////////////////////////
void
CWindowStyleWindow::IV_ColorChanged(RGBA color)
{
  RGB rgb = { color.red, color.green, color.blue };
  m_ColorView.SetColor(0, rgb);
  m_AlphaView.SetAlpha(color.alpha);
}
////////////////////////////////////////////////////////////////////////////////
void
CWindowStyleWindow::PV_ColorChanged(int index, RGB color)
{
  RGBA rgba = { color.red, color.green, color.blue, m_AlphaView.GetAlpha() };
  m_ImageView.SetColor(index, rgba);
  m_ColorView.SetColor(index, color);
}
////////////////////////////////////////////////////////////////////////////////
void
CWindowStyleWindow::CV_ColorChanged(int index, RGB color)
{
  RGBA rgba = { color.red, color.green, color.blue, m_AlphaView.GetAlpha() };
  m_ImageView.SetColor(index, rgba);
}
////////////////////////////////////////////////////////////////////////////////
void
CWindowStyleWindow::AV_AlphaChanged(byte alpha)
{
  RGBA rgba = m_ImageView.GetColor();
  rgba.alpha = alpha;
  m_ImageView.SetColor(0, rgba);
}
////////////////////////////////////////////////////////////////////////////////
int
CWindowStyleWindow::GetBorderWidth_Left() const
{
  int w1 = m_WindowStyle.GetBitmap(sWindowStyle::LEFT).GetWidth();
  int w2 = m_WindowStyle.GetBitmap(sWindowStyle::UPPER_LEFT).GetWidth();
  int w3 = m_WindowStyle.GetBitmap(sWindowStyle::LOWER_LEFT).GetWidth();
  return std::max(w1, std::max(w2, w3)) * m_ZoomFactor;
}
////////////////////////////////////////////////////////////////////////////////
int
CWindowStyleWindow::GetBorderWidth_Top() const
{
  int h1 = m_WindowStyle.GetBitmap(sWindowStyle::TOP).GetHeight();
  int h2 = m_WindowStyle.GetBitmap(sWindowStyle::UPPER_LEFT).GetHeight();
  int h3 = m_WindowStyle.GetBitmap(sWindowStyle::UPPER_RIGHT).GetHeight();
  return std::max(h1, std::max(h2, h3)) * m_ZoomFactor;
}
////////////////////////////////////////////////////////////////////////////////
int
CWindowStyleWindow::GetBorderWidth_Right() const
{
  int w1 = m_WindowStyle.GetBitmap(sWindowStyle::RIGHT).GetWidth();
  int w2 = m_WindowStyle.GetBitmap(sWindowStyle::UPPER_RIGHT).GetWidth();
  int w3 = m_WindowStyle.GetBitmap(sWindowStyle::LOWER_RIGHT).GetWidth();
  return std::max(w1, std::max(w2, w3)) * m_ZoomFactor;
}
////////////////////////////////////////////////////////////////////////////////
int
CWindowStyleWindow::GetBorderWidth_Bottom() const
{
  int h1 = m_WindowStyle.GetBitmap(sWindowStyle::BOTTOM).GetHeight();
  int h2 = m_WindowStyle.GetBitmap(sWindowStyle::LOWER_LEFT).GetHeight();
  int h3 = m_WindowStyle.GetBitmap(sWindowStyle::LOWER_RIGHT).GetHeight();
  return std::max(h1, std::max(h2, h3)) * m_ZoomFactor;
}
////////////////////////////////////////////////////////////////////////////////
void
CWindowStyleWindow::GetEditRect(RECT* rect)
{
  RECT ClientRect;
  GetClientRect(&ClientRect);
  rect->left   = GetBorderWidth_Left();
  rect->top    = GetBorderWidth_Top();
  rect->right  = ClientRect.right  - GetBorderWidth_Right() - 60 - 32;
  rect->bottom = ClientRect.bottom - GetBorderWidth_Bottom();
}
////////////////////////////////////////////////////////////////////////////////

void
CWindowStyleWindow::OnToolChanged(UINT id, int tool_index) {
  m_ImageView.OnToolChanged(id, tool_index);
}
////////////////////////////////////////////////////////////////////////////////
BOOL
CWindowStyleWindow::IsToolAvailable(UINT id) {
  BOOL available = FALSE;
  available = m_ImageView.IsToolAvailable(id);
  return available;
}
////////////////////////////////////////////////////////////////////////////////
