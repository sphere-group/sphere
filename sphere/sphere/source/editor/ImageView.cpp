#pragma warning(disable : 4786)

#include <stack>
#include <algorithm>

#include "ImageView.hpp"
#include "Editor.hpp"
#include "../common/Filters.hpp"
#include "../common/minmax.hpp"
#include "translate.hpp"
#include "resource.h"
#include "Configuration.hpp"
#include "Keys.hpp"
#include "../common/hsi.hpp"
#include "../common/primitives.hpp"
#include "ImageRender.hpp"
#include "NumberDialog.hpp"
#include "ConvolveListDialog.hpp"
#include "ColorAdjustDialog.hpp"
#include "../common/convolve.hpp"

////////////////////////////////////////////////////////////////////////////////

static int s_ImageViewID = 9000;

//static UINT s_ClipboardFormat;
//#define SCROLLABLE_IMAGE_WINDOW 1

////////////////////////////////////////////////////////////////////////////////

#ifdef SCROLLABLE_IMAGE_WINDOW
BEGIN_MESSAGE_MAP(CImageView, CScrollWindow)
#else
BEGIN_MESSAGE_MAP(CImageView, CWnd)
#endif
  ON_WM_PAINT()
  ON_WM_SIZE()
  ON_WM_CHAR()
  ON_WM_KEYDOWN()
  ON_WM_KEYUP()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_COMMAND(ID_IMAGEVIEW_COLORPICKER,           OnColorPicker)
  ON_COMMAND(ID_IMAGEVIEW_UNDO,                  OnUndo)
  ON_COMMAND(ID_IMAGEVIEW_REDO,                  OnRedo)
  ON_COMMAND(ID_IMAGEVIEW_COPY,                  OnCopy)
  ON_COMMAND(ID_IMAGEVIEW_PASTE,                 OnPaste)
  ON_COMMAND(ID_IMAGEVIEW_PASTE_RGB,             OnPasteRGB)
  ON_COMMAND(ID_IMAGEVIEW_PASTE_ALPHA,           OnPasteAlpha)
  ON_COMMAND(ID_IMAGEVIEW_PASTE_INTOSELECTION,   OnPasteIntoSelection)
  ON_COMMAND(ID_IMAGEVIEW_VIEWGRID,              OnViewGrid)
  ON_COMMAND(ID_IMAGEVIEW_TOGGLEALPHAMASK,       OnToggleViewAlphaMask)
  
  ON_COMMAND(ID_IMAGEVIEW_BLENDMODE_BLEND,          OnBlendModeBlend)
  ON_COMMAND(ID_IMAGEVIEW_BLENDMODE_REPLACE,        OnBlendModeReplace)
  ON_COMMAND(ID_IMAGEVIEW_BLENDMODE_RGBONLY,        OnBlendModeRGBOnly)
  ON_COMMAND(ID_IMAGEVIEW_BLENDMODE_ALPHAONLY,      OnBlendModeAlphaOnly)
  ON_COMMAND(ID_IMAGEVIEW_BLENDMODE_ADDITIVE,       OnBlendModeAdditive)
  ON_COMMAND(ID_IMAGEVIEW_BLENDMODE_SUBTRACTIVE,    OnBlendModeSubtractive)
  ON_COMMAND(ID_IMAGEVIEW_BLENDMODE_MULTIPLICATIVE, OnBlendModeMultiplicative)
  ON_COMMAND(ID_IMAGEVIEW_BLENDMODE_AVERAGE,        OnBlendModeAverage)
  ON_COMMAND(ID_IMAGEVIEW_BLENDMODE_INVERT,         OnBlendModeInvert)
  
  ON_COMMAND(ID_IMAGEVIEW_ROTATE_CW,             OnRotateCW)
  ON_COMMAND(ID_IMAGEVIEW_ROTATE_CCW,            OnRotateCCW)
  ON_COMMAND(ID_IMAGEVIEW_SLIDE_UP,              OnSlideUp)
  ON_COMMAND(ID_IMAGEVIEW_SLIDE_RIGHT,           OnSlideRight)
  ON_COMMAND(ID_IMAGEVIEW_SLIDE_DOWN,            OnSlideDown)
  ON_COMMAND(ID_IMAGEVIEW_SLIDE_LEFT,            OnSlideLeft)
  ON_COMMAND(ID_IMAGEVIEW_SLIDE_OTHER,           OnSlideOther)
  ON_COMMAND(ID_IMAGEVIEW_FLIP_HORIZONTALLY,     OnFlipHorizontally)
  ON_COMMAND(ID_IMAGEVIEW_FLIP_VERTICALLY,       OnFlipVertically)
  ON_COMMAND(ID_IMAGEVIEW_FILL_RGB,              OnFillRGB)
  ON_COMMAND(ID_IMAGEVIEW_FILL_ALPHA,            OnFillAlpha)
  ON_COMMAND(ID_IMAGEVIEW_FILL_BOTH,             OnFillBoth)
  ON_COMMAND(ID_IMAGEVIEW_REPLACE_RGBA,          OnReplaceRGBA)
  ON_COMMAND(ID_IMAGEVIEW_REPLACE_RGB,           OnReplaceRGB)
  ON_COMMAND(ID_IMAGEVIEW_REPLACE_ALPHA,         OnReplaceAlpha)
  ON_COMMAND(ID_IMAGEVIEW_FILTER_BLUR,           OnFilterBlur)
  ON_COMMAND(ID_IMAGEVIEW_FILTER_NOISE,          OnFilterNoise)
  ON_COMMAND(ID_IMAGEVIEW_FILTER_CUSTOM,         OnFilterCustom)
  ON_COMMAND(ID_IMAGEVIEW_FILTER_GRAYSCALE,      OnFilterGrayscale)
  ON_COMMAND(ID_IMAGEVIEW_FILTER_SATURATE,       OnFilterSaturate)
  ON_COMMAND(ID_IMAGEVIEW_FILTER_COLORADJUSTER,  OnFilterColorAdjuster)
  ON_COMMAND(ID_IMAGEVIEW_FILTER_ADJUST_BRIGHTNESS, OnFilterAdjustBrightness)
  ON_COMMAND(ID_IMAGEVIEW_FILTER_ADJUST_GAMMA, OnFilterAdjustGamma)
  ON_COMMAND(ID_IMAGEVIEW_FILTER_NEGATIVE_IMAGE_RGB, OnFilterNegativeImageRGB)
  ON_COMMAND(ID_IMAGEVIEW_FILTER_NEGATIVE_IMAGE_ALPHA, OnFilterNegativeImageAlpha)
  ON_COMMAND(ID_IMAGEVIEW_FILTER_NEGATIVE_IMAGE_RGBA, OnFilterNegativeImageRGBA)
  ON_COMMAND(ID_IMAGEVIEW_FILTER_SOLARIZE,       OnFilterSolarize)
  ON_COMMAND(ID_IMAGEVIEW_SETCOLORALPHA,         OnSetColorAlpha)
  ON_COMMAND(ID_IMAGEVIEW_SCALEALPHA,            OnScaleAlpha)
	ON_MESSAGE(WM_GETACCELERATOR, OnGetAccelerator)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////

CImageView::CImageView()
: m_SwatchPalette(NULL)
//, m_ToolPalette(NULL)
, m_CurrentTool(0)
, m_NumUndoImages(0)
, m_UndoImages(NULL)
, m_NumRedoImages(0)
, m_RedoImages(NULL)
, m_SelectionType(ST_Rectangle)
, m_SelectionX(0)
, m_SelectionY(0)
, m_SelectionWidth(0)
, m_SelectionHeight(0)
, m_ShowGrid(false)
, m_ShowAlphaMask(true)
, m_RedrawX(0)
, m_RedrawY(0)
, m_RedrawWidth(0)
, m_RedrawHeight(0)
, m_Clipboard(NULL)
, m_BlitTile(NULL)
#ifdef SCROLLABLE_IMAGE_WINDOW
, m_CurrentX(0)
, m_CurrentY(0)
#endif
{
  m_Colors[0] = CreateRGBA(255, 255, 255, 255);
  m_Colors[1] = CreateRGBA(0,   0,   0,   255);
  m_SelectedTools[0] = Tool_Pencil;
  m_SelectedTools[1] = Tool_Pencil;
  m_MouseDown[0] = m_MouseDown[1] = false;
  m_CurPoint.x = 0;
  m_CurPoint.y = 0;
  m_LastPoint.x = -1;
  m_LastPoint.y = -1;
  m_Image.SetBlendMode(CImage32::REPLACE);
  m_ShowGrid = false;
  m_Clipboard = new CClipboard();
  m_BlitTile = new CDIBSection(16, 16, 32);
  m_ColorMask1 = CreateRGBA(Configuration::Get(KEY_COLOR_MASK_1_RED), Configuration::Get(KEY_COLOR_MASK_1_GREEN), Configuration::Get(KEY_COLOR_MASK_1_BLUE), 255);
  m_ColorMask2 = CreateRGBA(Configuration::Get(KEY_COLOR_MASK_2_RED), Configuration::Get(KEY_COLOR_MASK_2_GREEN), Configuration::Get(KEY_COLOR_MASK_2_BLUE), 255);
  key_up = key_down = key_left = key_right = false;
}

////////////////////////////////////////////////////////////////////////////////

CImageView::~CImageView()
{
  // destroy the blit DIB
  delete m_BlitTile;
  m_BlitTile = NULL;
  delete m_Clipboard;
  m_Clipboard = NULL;
  m_SelectionPoints.clear();
  ResetUndoStates();
  ResetRedoStates();
  if (m_SwatchPalette) {
    m_SwatchPalette->Destroy();
  }
  //if (m_ToolPalette) {
  //  m_ToolPalette->Destroy();
  //}
  DestroyWindow();
}

////////////////////////////////////////////////////////////////////////////////

BOOL
CImageView::Create(CDocumentWindow* owner, IImageViewHandler* handler, CWnd* parent_window)
{
  m_Handler = handler;
  m_SwatchPalette = new CSwatchPalette(owner, this);
  //m_ToolPalette   = new CImageToolPalette(owner, this);
  BOOL retval = CWnd::Create(
  //AfxRegisterWndClass(0, LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)), NULL, LoadIcon(NULL, IDI_APPLICATION)),
  AfxRegisterWndClass(0, NULL, NULL, LoadIcon(NULL, IDI_APPLICATION)),
      "ImageView",
#ifdef SCROLLABLE_IMAGE_WINDOW
      WS_HSCROLL | WS_VSCROLL |
#endif
      WS_CHILD | WS_VISIBLE,
      CRect(0, 0, 0, 0),
      parent_window,
      s_ImageViewID++);
#ifdef SCROLLABLE_IMAGE_WINDOW
  UpdateScrollBars();
#endif

  return retval;
}

////////////////////////////////////////////////////////////////////////////////

#ifdef SCROLLABLE_IMAGE_WINDOW
void
CImageView::UpdateScrollBars()
{
  SetHScrollRange(GetTotalTilesX() + 1, GetPageSizeX());
  SetVScrollRange(GetTotalTilesY() + 1, GetPageSizeY());
}

////////////////////////////////////////////////////////////////////////////////

/**
 * Also controls the size of the slider (thumb tab). 0 = default size
 */
int
CImageView::GetPageSizeX()
{
  /*
  if (m_Image.GetWidth() > 0) {
    RECT ClientRect;
    GetClientRect(&ClientRect);
    // calculate size of pixel squares
    int width = m_Image.GetWidth();
    int height = m_Image.GetHeight();
    int hsize = ClientRect.right / width;
    int vsize = ClientRect.bottom / height;
    int size = std::min(hsize, vsize);
    if (size < 1)
      size = 1;
    return ClientRect.right  / (m_Image.GetWidth() * size);
  }
  */
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

/**
 * Amount the scrollbar scrolls when PAGE_UP/PAGE_DOWN are pressed
 * Also controls the size of the slider (thumb tab). 0 = default size
 */
int
CImageView::GetPageSizeY()
{
  /*
  if (m_Image.GetHeight() > 0) {
    RECT ClientRect;
    GetClientRect(&ClientRect);
    // calculate size of pixel squares
    int width = m_Image.GetWidth();
    int height = m_Image.GetHeight();
    int hsize = ClientRect.right / width;
    int vsize = ClientRect.bottom / height;
    int size = std::min(hsize, vsize);
    if (size < 1)
      size = 1;
    return ClientRect.bottom / (m_Image.GetHeight() * size);
  }
  */
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

int
CImageView::GetTotalTilesX()
{
  return m_Image.GetWidth() - 1;
}

////////////////////////////////////////////////////////////////////////////////

int
CImageView::GetTotalTilesY()
{
  return m_Image.GetHeight() - 1;
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::OnHScrollChanged(int x)
{
  // do the scrolling thing
  int old_x = m_CurrentX;
  int new_x = x;
  CDC* dc_ = GetDC();
  HDC dc = dc_->m_hDC;
  HRGN region = CreateRectRgn(0, 0, 0, 0);
  int factor = m_Image.GetWidth();
  m_CurrentX = x;
  //ScrollDC(dc, (old_x - new_x) * factor, 0, NULL, NULL, region, NULL);
  //::InvalidateRgn(m_hWnd, region, FALSE);
  Invalidate();
  //m_RedrawWindow = 1;
  DeleteObject(region);
  ReleaseDC(dc_);
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::OnVScrollChanged(int y)
{
  // do the scrolling thing
  int old_y = m_CurrentY;
  int new_y = y;
  CDC* dc_ = GetDC();
  HDC dc = dc_->m_hDC;
  HRGN region = CreateRectRgn(0, 0, 0, 0);
  int factor = m_Image.GetHeight();
  m_CurrentY = y;
  //ScrollDC(dc, 0, (old_y - new_y) * factor, NULL, NULL, region, NULL);
  //::InvalidateRgn(m_hWnd, region, FALSE);
  Invalidate();
  //m_RedrawWindow = 1;
  DeleteObject(region);
  ReleaseDC(dc_);
}
#endif

////////////////////////////////////////////////////////////////////////////////

bool
CImageView::SetImage(int width, int height, const RGBA* pixels, bool reset_undo_states)
{
  if (reset_undo_states) {
    ResetUndoStates();
    ResetRedoStates();
  }
  m_Image.Resize(width, height);
  if (m_Image.GetWidth() == width && m_Image.GetHeight() == height)
    memcpy(m_Image.GetPixels(), pixels, width * height * sizeof(RGBA));
  
#ifdef SCROLLABLE_IMAGE_WINDOW
  UpdateScrollBars();
#endif
  Invalidate();
  return true;
}

////////////////////////////////////////////////////////////////////////////////

int
CImageView::GetWidth() const
{
  return m_Image.GetWidth();
}

////////////////////////////////////////////////////////////////////////////////

int
CImageView::GetHeight() const
{
  return m_Image.GetHeight();
}

////////////////////////////////////////////////////////////////////////////////

RGBA*
CImageView::GetPixels()
{
  return m_Image.GetPixels();
}

////////////////////////////////////////////////////////////////////////////////

const RGBA*
CImageView::GetPixels() const
{
  return m_Image.GetPixels();
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::SetColor(int index, RGBA color)
{
  m_Colors[index] = color;
  m_SwatchPalette->SetColor(color);
}

////////////////////////////////////////////////////////////////////////////////

RGBA
CImageView::GetColor(int index) const
{
  return m_Colors[index];
}

////////////////////////////////////////////////////////////////////////////////

/*
 * Called before an image modification takes place...
 * Called from outside imageview...
 */
void
CImageView::BeforeImageChanged()
{
  AddUndoState();
}

////////////////////////////////////////////////////////////////////////////////

/*
 * Called after an image modification takes place...
 * Called from outside imageview...
 */
void
CImageView::AfterImageChanged()
{ // if the image dimensions change, the selection area needs to change too
  m_SelectionX = std::min(m_SelectionX, m_Image.GetWidth());
  m_SelectionY = std::min(m_SelectionY, m_Image.GetHeight());
  m_SelectionWidth = std::min(m_SelectionWidth, m_Image.GetWidth() - m_SelectionX);
  m_SelectionHeight = std::min(m_SelectionHeight, m_Image.GetHeight() - m_SelectionY);
  Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
// I don't think this method is ever called, ever?

void
CImageView::FillRGB()
{
  AddUndoState();
  for (int y = 0; y < m_Image.GetHeight(); y++)
    for (int x = 0; x < m_Image.GetWidth(); x++)
    {
      RGBA tColor = m_Image.GetPixel(x, y);
      tColor.red   = m_Colors[m_CurrentTool].red;
      tColor.green = m_Colors[m_CurrentTool].green;
      tColor.blue  = m_Colors[m_CurrentTool].blue;
      m_Image.SetPixel(x, y, tColor);
    }
  Invalidate();
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////
// I don't think this method is ever called, ever?

void
CImageView::FillAlpha()
{
  AddUndoState();
  for (int y = 0; y < m_Image.GetHeight(); y++)
    for (int x = 0; x < m_Image.GetWidth(); x++)
    {
      RGBA tColor = m_Image.GetPixel(x, y);
      tColor.alpha = m_Colors[m_CurrentTool].alpha;
      m_Image.SetPixel(x, y, tColor);
    }
  Invalidate();
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

bool
CImageView::Copy()
{
  if (!m_Clipboard) 
    return false;

  if (OpenClipboard() == FALSE)
    return false;

  int width = m_Image.GetWidth();
  int height = m_Image.GetHeight();

  // clear the previous contents of the clipboard
  EmptyClipboard();

  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();

  RGBA* pixels = GetSelectionPixels();

  m_Clipboard->PutFlatImageOntoClipboard(sw, sh, pixels);
  m_Clipboard->PutBitmapImageOntoClipboard(sw, sh, pixels);

  FreeSelectionPixels(pixels); 

  CloseClipboard();
  return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
CImageView::PasteChannels(bool red, bool green, bool blue, bool alpha, int merge_method)
{
  if (!m_Clipboard) 
    return false;

  if (OpenClipboard() == FALSE)
    return false;

  //int iWidth = m_Image.GetWidth();
  //int iHeight = m_Image.GetHeight();
  int cwidth = 0;
  int cheight = 0;
  RGBA* cpixels = NULL;

  // see if the flat image is in the clipboard
  cpixels = m_Clipboard->GetFlatImageFromClipboard(cwidth, cheight);

  if (cpixels == NULL)
    cpixels = m_Clipboard->GetBitmapImageFromClipboard(cwidth, cheight);

  CloseClipboard();

  if (cpixels != NULL)
  {
    AddUndoState();
    // and now we merge the clipboard image with the current image
    RGBA* iPixels = GetSelectionPixels();
    int sx = GetSelectionLeftX();
    int sy = GetSelectionTopY();
    int sw = GetSelectionWidth();
    int sh = GetSelectionHeight();
    int xoffset = 0;
    int yoffset = 0;

    if (merge_method == Merge_IntoSelection)
    {
      xoffset = sx;
      yoffset = sy;

      // rescale cpixels
      if (sw != cwidth || sh != cheight)
      {
        CImage32 tmp(cwidth, cheight, cpixels);
        delete[] cpixels;
        cpixels = NULL;
        if (tmp.GetWidth() != cwidth || tmp.GetHeight() != cheight || tmp.GetPixels() == NULL)
          return false;

        cpixels = new RGBA[sw * sh];
        if (cpixels == NULL)
          return false;

        tmp.Rescale(sw, sh);
        if (tmp.GetWidth() != sw || tmp.GetHeight() != sh || tmp.GetPixels() == NULL) {
          delete[] cpixels;
          return false;
        }

        const RGBA* temp_pixels = tmp.GetPixels();
        for (int i = 0; i < sw * sh; i++)
          cpixels[i] = temp_pixels[i];
        cwidth = sw;
        cheight = sh;
      }
    }

    int max_width  = cwidth;  if (sw < cwidth)  max_width  = sw;
    int max_height = cheight; if (sh < cheight) max_height = sh;

    for (int iy = 0; iy < max_height; iy++)
    {
      for (int ix = 0; ix < max_width; ix++)
      {
        /*
        if (red)   iPixels[(iy + yoffset) * iWidth + (ix + xoffset)].red   = cpixels[iy * cwidth + ix].red;
        if (green) iPixels[(iy + yoffset) * iWidth + (ix + xoffset)].green = cpixels[iy * cwidth + ix].green;
        if (blue)  iPixels[(iy + yoffset) * iWidth + (ix + xoffset)].blue  = cpixels[iy * cwidth + ix].blue;
        if (alpha) iPixels[(iy + yoffset) * iWidth + (ix + xoffset)].alpha = cpixels[iy * cwidth + ix].alpha;
        */
        m_Image.SetPixel((ix + xoffset), (iy + yoffset), cpixels[iy * cwidth + ix]);
      }
    }
    delete[] cpixels;
    cpixels = NULL;

    // things have changed
    Invalidate();
    m_Handler->IV_ImageChanged();
    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////

bool
CImageView::Paste()
{
  return PasteChannels(true, true, true, true);
}

////////////////////////////////////////////////////////////////////////////////

bool
CImageView::CanUndo() const
{
  return m_NumUndoImages > 0;
}

////////////////////////////////////////////////////////////////////////////////

bool
CImageView::CanRedo() const
{
  return m_NumRedoImages > 0;
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::Undo()
{
  if (m_UndoImages == 0)
    return;
  if (1) {
    Image* new_images = new Image[m_NumRedoImages + 1];
    if (new_images) {
  
      for (int i = 0; i < m_NumRedoImages; i++)
        new_images[i] = m_RedoImages[i];
      const int width = m_Image.GetWidth();
      const int height = m_Image.GetHeight();
      new_images[m_NumRedoImages].width = width;
      new_images[m_NumRedoImages].height = height;
      new_images[m_NumRedoImages].pixels = new RGBA[width * height];
      if (new_images[m_NumRedoImages].pixels) {
        memcpy(new_images[m_NumRedoImages].pixels, m_Image.GetPixels(), width * height * sizeof(RGBA));
        m_NumRedoImages++;
        delete[] m_RedoImages;
        m_RedoImages = new_images;
      }
    }
  }
  Image* img = m_UndoImages + m_NumUndoImages - 1;
  m_Image.Resize(img->width, img->height);
  // only copy the undo image if the resize has succeeded
  if (m_Image.GetWidth() == img->width && m_Image.GetHeight() == img->height) {
    memcpy(m_Image.GetPixels(), img->pixels, img->width * img->height * sizeof(RGBA));
  }
  delete[] img->pixels;
  Image* new_images = new Image[m_NumUndoImages - 1];
  if (new_images) {
    for (int i = 0; i < m_NumUndoImages - 1; i++) {
      new_images[i] = m_UndoImages[i];
    }
    m_NumUndoImages--;
    delete[] m_UndoImages;
    m_UndoImages = new_images;
  }
  Invalidate();
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::Redo()
{
  if (m_RedoImages == 0)
    return;
  if (1) {
    Image* new_images = new Image[m_NumUndoImages + 1];
    if (new_images) {
      for (int i = 0; i < m_NumUndoImages; i++)
        new_images[i] = m_UndoImages[i];
      const int width = m_Image.GetWidth();
      const int height = m_Image.GetHeight();
      new_images[m_NumUndoImages].width = width;
      new_images[m_NumUndoImages].height = height;
      new_images[m_NumUndoImages].pixels = new RGBA[width * height];
      if (new_images[m_NumUndoImages].pixels) {
        memcpy(new_images[m_NumUndoImages].pixels, m_Image.GetPixels(), width * height * sizeof(RGBA));
        m_NumUndoImages++;
        delete[] m_UndoImages;
        m_UndoImages = new_images;
      }
    }
  }
  Image* img = m_RedoImages + m_NumRedoImages - 1;
  m_Image.Resize(img->width, img->height);
  // only copy the undo image if the resize has succeeded
  if (m_Image.GetWidth() == img->width && m_Image.GetHeight() == img->height) {
    memcpy(m_Image.GetPixels(), img->pixels, img->width * img->height * sizeof(RGBA));
  }
  delete[] img->pixels;
  Image* new_images = new Image[m_NumRedoImages - 1];
  if (new_images) {
    for (int i = 0; i < m_NumRedoImages - 1; i++) {
      new_images[i] = m_RedoImages[i];
    }
    m_NumRedoImages--;
    delete[] m_RedoImages;
    m_RedoImages = new_images;
  }
  Invalidate();
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::SP_ColorSelected(RGBA color)
{
  m_Colors[m_CurrentTool] = color;
  m_Handler->IV_ColorChanged(color);
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::TP_ToolSelected(int tool, int tool_index)
{
  // do something with the tool
  m_SelectedTools[tool_index] = tool;
  
  switch (m_SelectedTools[tool_index]) 
  {
    case Tool_Selection:     m_SelectionType = ST_Rectangle; break;
    case Tool_FreeSelection: m_SelectionType = ST_Free;      break;
  }
}

////////////////////////////////////////////////////////////////////////////////

POINT
CImageView::ConvertToPixel(POINT point)
{
  // convert pixel coordinates to image coordinates
  POINT retPoint;
  retPoint.x = point.x;
  retPoint.y = point.y;

  // get client rectangle
  RECT ClientRect;
  GetClientRect(&ClientRect);

  // calculate size of pixel squares
  int width = m_Image.GetWidth();
  int height = m_Image.GetHeight();
  int hsize = ClientRect.right / width;
  int vsize = ClientRect.bottom / height;
  int size = std::min(hsize, vsize);
  if (size < 1)
    size = 1;
  int totalx = size * width;
  int totaly = size * height;
  int offsetx = (ClientRect.right - totalx) / 2;
  int offsety = (ClientRect.bottom - totaly) / 2;
  retPoint.x -= offsetx;
  retPoint.y -= offsety;
  retPoint.x = (retPoint.x + size) / size - 1;
  retPoint.y = (retPoint.y + size) / size - 1;
  return retPoint;
}

////////////////////////////////////////////////////////////////////////////////

bool
CImageView::InImage(POINT p)
{
  return (p.x >= 0 && p.x < m_Image.GetWidth() &&
          p.y >= 0 && p.y < m_Image.GetHeight());
}

////////////////////////////////////////////////////////////////////////////////

bool
CImageView::InSelection(POINT p)
{
  if (m_SelectionWidth <= 0 && m_SelectionHeight <= 0)
    return InImage(p);
  else
  if (p.x >= m_SelectionX && p.x < m_SelectionX + m_SelectionWidth &&
      p.y >= m_SelectionY && p.y < m_SelectionY + m_SelectionHeight) {
    return InImage(p);
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////

int
CImageView::GetSelectionWidth()
{
  if (m_SelectionWidth <= 0 && m_SelectionHeight <= 0)
    return m_Image.GetWidth();
  else
    return m_SelectionWidth;
}

///////////////////////////////////////////////////////////////////////////////

int
CImageView::GetSelectionHeight()
{
  if (m_SelectionWidth <= 0 && m_SelectionHeight <= 0)
    return m_Image.GetHeight();
  else
    return m_SelectionHeight;
}

///////////////////////////////////////////////////////////////////////////////

int
CImageView::GetSelectionLeftX() {
  return (m_SelectionWidth <= 0 && m_SelectionHeight <= 0) ? (0) : (m_SelectionX);
}

int
CImageView::GetSelectionTopY() {
  return (m_SelectionWidth <= 0 && m_SelectionHeight <= 0) ? (0) : (m_SelectionY);
}

///////////////////////////////////////////////////////////////////////////////

void
CImageView::FreeSelectionPixels(RGBA* pixels) {
  if (pixels != m_Image.GetPixels()) {
    delete[] pixels;
    pixels = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////

RGBA*
CImageView::GetSelectionPixels() {
  if (m_SelectionWidth <= 0 && m_SelectionHeight <= 0) {
    return m_Image.GetPixels();
  } else {
    int sx = GetSelectionLeftX();
    int sy = GetSelectionTopY();
    int sw = GetSelectionWidth();
    int sh = GetSelectionHeight();
    int xoffset = sx;
    int yoffset = sy;
    RGBA* pixels = new RGBA[sw * sh];
    if (pixels == NULL)
      return m_Image.GetPixels();
    const RGBA* image = m_Image.GetPixels();
    int iWidth = m_Image.GetWidth();
    for (int dy = sy; dy < (sy + sh); dy++) {
      for (int dx = sx; dx < (sx + sw); dx++) {
        pixels[(dy - yoffset) * sw + (dx - xoffset)] = image[dy * iWidth + dx];
      }
    }
    return pixels;
  }
}

///////////////////////////////////////////////////////////////////////////////

void
CImageView::UpdateSelectionPixels(const RGBA* pixels, int sx, int sy, int sw, int sh)
{
  RGBA* image = m_Image.GetPixels();
  int iWidth = m_Image.GetWidth();

  // if pixels point to image updating it wont do anything so don't bother
  if (pixels == image)
    return;

  if (m_SelectionType == ST_Rectangle)
  {
    for (int dy = sy; dy < (sy + sh); dy++)
      for (int dx = sx; dx < (sx + sw); dx++)
        image[dy * iWidth + dx] = pixels[(dy - sy) * sw + (dx - sx)];
  }
  else
  if (m_SelectionType == ST_Free)
  {
    int selection_width = m_SelectionWidth + 1;
    int selection_height = m_SelectionHeight + 1;

    bool* selection_points = new bool[selection_width * selection_height];
    if (selection_points == NULL)
      return;

    memset(selection_points, false, (selection_width * selection_height) * sizeof(bool));
    struct Local {
      struct Color {
        bool operator()(int, int) {
          return true;
        }
      };
      static inline void CopyBool(bool& dest, bool src) {
        dest = src;
      }
    };

    // draw true/false lines onto selection_points buffer
    for (unsigned int i = 1; i < m_SelectionPoints.size(); i++) {
      Local::Color c;
      clipper clip = {0, 0, selection_width - 1, selection_height - 1};
      CPoint p = m_SelectionPoints[i - 1];
      CPoint q = m_SelectionPoints[i];
      primitives::Line(selection_points, selection_width,
          p.x - m_SelectionX, p.y - m_SelectionY,
          q.x - m_SelectionX, q.y - m_SelectionY,
          c, clip, Local::CopyBool);
    }

    int dy;

    // fill in gaps between lines
    for (dy = 0; dy < selection_height; dy++) {
      int last_on = -1;
      for (int dx = 0; dx < selection_width; dx++) {
        int index = dy * selection_width + dx;
        if (selection_points[index]) {
          if (last_on == -1)
            last_on = dx;
        } else {
          for (int x = last_on; x < dx; x++) {
            selection_points[dy * selection_width + x] = true;
            last_on = -1;
          }
        }
      }
    }

    // update image
    for (dy = sy; dy < (sy + sh); dy++) {
      for (int dx = sx; dx < (sx + sw); dx++) {
        int selection_index = (dy - sy) * selection_width + (dx - sx);
        int pixel_index = (dy - sy) * sw + (dx - sx);
        if (selection_points[selection_index])
          image[dy * iWidth + dx] = pixels[pixel_index];
      }
    }

    delete[] selection_points;
    selection_points = NULL;
  }
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::InvalidateSelection(int sx, int sy, int sw, int sh)
{
  if (m_RedrawX == 0 && m_RedrawY == 0 && m_RedrawWidth == 0 && m_RedrawHeight == 0) {
    // assume a complete redraw is needed...
    m_RedrawX = sx;
    m_RedrawY = sy;
    m_RedrawWidth = sw;
    m_RedrawHeight = sh;
  } else {
    // only a partial redraw is needed...
    int x1 = m_RedrawX + m_RedrawWidth;
    int x2 = sx + sw;
    int y1 = m_RedrawY + m_RedrawHeight;
    int y2 = sy + sh;
    m_RedrawX = std::min(sx, m_RedrawX);
    m_RedrawY = std::min(sy, m_RedrawY);
    m_RedrawWidth  = std::max(x1, x2) - m_RedrawX;
    m_RedrawHeight = std::max(y1, y2) - m_RedrawY;
  }
  Invalidate();
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::Click(bool force_draw)
{
  if (m_Image.GetPixels() == NULL || m_Image.GetWidth() == 0 || m_Image.GetHeight() == 0)
    return;

  // convert pixel coordinates to image coordinates
  POINT start = ConvertToPixel(m_LastPoint);
  POINT end = ConvertToPixel(m_CurPoint);

  if (!InImage(end) || !InSelection(end)) {
    return;
  }

  if (!force_draw && start.x == end.x && start.y == end.y) {
    return;
  }

  RGBA old_pixel = m_Image.GetPixel(end.x, end.y);
  m_Image.SetPixel(end.x, end.y, m_Colors[m_CurrentTool]);
  RGBA new_pixel = m_Image.GetPixel(end.x, end.y);

  // has the image actually changed?
  if (old_pixel.red   != new_pixel.red
   || old_pixel.green != new_pixel.green
   || old_pixel.blue  != new_pixel.blue
   || old_pixel.alpha != new_pixel.alpha) {
    m_Handler->IV_ImageChanged();
  }

  InvalidateSelection(end.x, end.y, 1, 1);
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::Fill()
{
  if (m_MouseDown[m_CurrentTool])
    return;

  // convert pixel coordinates to image coordinates
  POINT startPoint = ConvertToPixel(m_CurPoint);

  // bounds check
  if (!InImage(startPoint) || !InSelection(startPoint))
    return;

  if (IsColorToReplace(m_Image.GetPixel(startPoint.x, startPoint.y), m_Colors[m_CurrentTool]))
    return;

  FillMe(startPoint.x, startPoint.y, m_Image.GetPixel(startPoint.x, startPoint.y));
  InvalidateSelection(GetSelectionLeftX(), GetSelectionTopY(), GetSelectionWidth(), GetSelectionHeight());
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

struct Point {
  Point(int x_, int y_) {
    x = x_;
    y = y_;
  }
  int x;
  int y;
};

////////////////////////////////////////////////////////////////////////////////

bool
CImageView::IsColorToReplace(RGBA pixel, RGBA colorToReplace) {
  bool r = false;
  switch (m_Image.GetBlendMode()) {
    case CImage32::BLEND:      r = (pixel == colorToReplace); break;
    case CImage32::REPLACE:    r = (pixel == colorToReplace); break;
    case CImage32::RGB_ONLY:   r = (pixel.red == colorToReplace.red && pixel.green == colorToReplace.green && pixel.blue == colorToReplace.blue); break;
    case CImage32::ALPHA_ONLY: r = (pixel.alpha == colorToReplace.alpha); break;
  }
  return r;
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::FillMe(int x, int y, RGBA colorToReplace)
{
  int sx = GetSelectionLeftX();
  int sy = GetSelectionTopY();
  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();
  const int width  = sx + sw;
  const int height = sy + sh;
  std::stack<Point> q;
  q.push(Point(x, y));
  m_Image.SetPixel(x, y, m_Colors[m_CurrentTool]);
  const int max_size = width * height;
  int current_size = 0;
  while (!q.empty()) {
    Point p = q.top();
    q.pop();
    // fill up
    if (p.y > sy && IsColorToReplace(m_Image.GetPixel(p.x, p.y - 1), colorToReplace)) {
      if (current_size < max_size) q.push( Point(p.x, p.y - 1) );
      m_Image.SetPixel(p.x, p.y - 1, m_Colors[m_CurrentTool]);
    }
    // fill down
    if (p.y < height - 1 && IsColorToReplace(m_Image.GetPixel(p.x, p.y + 1), colorToReplace)) {
      if (current_size < max_size) q.push( Point(p.x, p.y + 1) );
      m_Image.SetPixel(p.x, p.y + 1, m_Colors[m_CurrentTool]);
    }
    // fill left
    if (p.x > sx && IsColorToReplace(m_Image.GetPixel(p.x - 1, p.y), colorToReplace)) {
      if (current_size < max_size) q.push( Point(p.x - 1, p.y) );
      m_Image.SetPixel(p.x - 1, p.y, m_Colors[m_CurrentTool]);
    }
    // fill right
    if (p.x < width - 1 && IsColorToReplace(m_Image.GetPixel(p.x + 1, p.y), colorToReplace)) {
      if (current_size < max_size) q.push( Point(p.x + 1, p.y) );
      m_Image.SetPixel(p.x + 1, p.y, m_Colors[m_CurrentTool]);
    }
    current_size += 1;
  }
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::Line()
{
  if (!m_MouseDown[m_CurrentTool]) 
  {
    m_StartPoint = m_CurPoint;
    Invalidate();
  } 
  else 
  {
    // convert pixel coordinates to image coordinates
    POINT start = ConvertToPixel(m_StartPoint);
    POINT end = ConvertToPixel(m_CurPoint);

    int max_x = std::max(start.x, end.x);
    int max_y = std::max(start.y, end.y);
    int min_x = std::min(start.x, end.x);
    int min_y = std::min(start.y, end.y);

    int sx = GetSelectionLeftX();
    int sy = GetSelectionTopY();
    int sw = GetSelectionWidth();
    int sh = GetSelectionHeight();
    clipper clip = {sx, sy, (sx + sw) - 1, (sy + sh) - 1};
    
    if (min_x                   >= sx + sw ||
        min_x + (max_x - min_x) <  sx      ||
        min_y                   >= sy + sh ||
        min_y + (max_y - min_y) <  sy)
    {
      Invalidate();
      return;
    }
    
    m_Image.Line(start.x, start.y, end.x, end.y, m_Colors[m_CurrentTool], clip);
    
    Invalidate();
    m_Handler->IV_ImageChanged();
  }
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::Rectangle()
{
  if (!m_MouseDown[m_CurrentTool]) 
  {
    m_StartPoint = m_CurPoint;
    Invalidate();
  } 
  else 
  {
    // convert pixel coordinates to image coordinates
    POINT start = ConvertToPixel(m_StartPoint);
    POINT end = ConvertToPixel(m_CurPoint);

    int width  = abs(end.x - start.x) + 1;
    int height = abs(end.y - start.y) + 1;

    if (start.x > end.x)
      std::swap(start.x, end.x);
    if (start.y > end.y)
      std::swap(start.y, end.y);

    int sx = GetSelectionLeftX();
    int sy = GetSelectionTopY();
    int sw = GetSelectionWidth();
    int sh = GetSelectionHeight();
    clipper clip = { sx, sy, (sx + sw) - 1, (sy + sh) - 1 };
    
    if (start.x              >= sx + sw ||
        start.x + width  - 1 <  sx      ||
        start.y              >= sy + sh ||
        start.y + height - 1 <  sy)
    {
      Invalidate();
      return;
    }
    
    if (GetMainWindow()->IsImageFillShapeToolChecked())
      m_Image.Rectangle(start.x, start.y, width, height, m_Colors[m_CurrentTool], clip);
    else
      m_Image.OutlinedRectangle(start.x, start.y, width, height, 1, m_Colors[m_CurrentTool], clip);

    Invalidate();
    m_Handler->IV_ImageChanged();
  }
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::Selection()
{
  if (!m_MouseDown[m_CurrentTool])
    m_StartPoint = m_CurPoint;
  else
    UpdateSelection();

  Invalidate();
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::Circle()
{
  if (!m_MouseDown[m_CurrentTool]) 
  {
    m_StartPoint = m_CurPoint;
    Invalidate();
  } 
  else 
  {
    // convert pixel coordinates to image coordinates
    POINT start = ConvertToPixel(m_StartPoint);
    POINT end = ConvertToPixel(m_CurPoint);
    
    int radius;
    int antialias;
    
    if (abs(start.x - end.x) > abs(start.y - end.y))
      radius = abs(start.x - end.x) + 1;
    else
      radius = abs(start.y - end.y) + 1;
    
    if (GetMainWindow()->IsImageAntialiasToolChecked())
      antialias = 1;
    else
      antialias = 0;
    
    if (start.x > end.x)
      end.x = start.x + radius - 1;
    if (start.y > end.y)
      end.y = start.y + radius - 1;
    
    int sx = GetSelectionLeftX();
    int sy = GetSelectionTopY();
    int sw = GetSelectionWidth();
    int sh = GetSelectionHeight();
    clipper clip = { sx, sy, (sx + sw) - 1, (sy + sh) - 1 };
    
    if (start.x - radius     >= sx + sw ||
        start.x + radius - 1 <  sx      ||
        start.y - radius     >= sy + sh ||
        start.y + radius - 1 <  sy)
    {
      Invalidate();
      return;
    }
    
    if (GetMainWindow()->IsImageFillShapeToolChecked())
      m_Image.FilledCircle(start.x,   start.y, radius, m_Colors[m_CurrentTool], antialias, clip);
    else
      m_Image.OutlinedCircle(start.x, start.y, radius, m_Colors[m_CurrentTool], antialias, clip);
      
    Invalidate();
    m_Handler->IV_ImageChanged();
  }
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::Ellipse()
{
  if (!m_MouseDown[m_CurrentTool]) 
  {
    m_StartPoint = m_CurPoint;
    Invalidate();
  } 
  else 
  {
    // convert pixel coordinates to image coordinates
    POINT start = ConvertToPixel(m_StartPoint);
    POINT end   = ConvertToPixel(m_CurPoint);
    
    int rx = abs(end.x - start.x) + 1;
    int ry = abs(end.y - start.y) + 1;
    
    if (start.x > end.x)
      end.x = start.x + rx - 1;
    if (start.y > end.y)
      end.y = start.y + ry - 1;
      
    int sx = GetSelectionLeftX();
    int sy = GetSelectionTopY();
    int sw = GetSelectionWidth();
    int sh = GetSelectionHeight();
    clipper clip = { sx, sy, (sx + sw) - 1, (sy + sh) - 1 };
    
    if (start.x - rx     >= sx + sw ||
        start.x + rx - 1 <  sx      ||
        start.y - ry     >= sy + sh ||
        start.y + ry - 1 <  sy)
    {
      Invalidate();
      return;
    }
    
    if (GetMainWindow()->IsImageFillShapeToolChecked())
      m_Image.FilledEllipse(start.x,   start.y, rx, ry, m_Colors[m_CurrentTool], clip);
    else
      m_Image.OutlinedEllipse(start.x, start.y, rx, ry, m_Colors[m_CurrentTool], clip);
    
    Invalidate();
    m_Handler->IV_ImageChanged();
  }
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::GetColor(RGBA* color, int x, int y)
{
  // convert pixel coordinates to image coordinates
  POINT point = ConvertToPixel(m_CurPoint);

  // bounds check
  if (!InImage(point))
    return;

  RGBA* pImage = m_Image.GetPixels();

  // now that we have image coordinates, we can update the image
  if (memcmp(pImage + point.y * m_Image.GetWidth() + point.x, &m_Colors[m_CurrentTool], sizeof(RGBA)) != 0)
    m_Colors[m_CurrentTool] = pImage[point.y * m_Image.GetWidth() + point.x];
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::AddUndoState()
{
  ResetRedoStates();

  Image* new_images = new Image[m_NumUndoImages + 1];
  if (new_images)
  {
    for (int i = 0; i < m_NumUndoImages; i++)
      new_images[i] = m_UndoImages[i];

    const int width = m_Image.GetWidth();
    const int height = m_Image.GetHeight();
    new_images[m_NumUndoImages].width = width;
    new_images[m_NumUndoImages].height = height;
    new_images[m_NumUndoImages].pixels = new RGBA[width * height];

    if (new_images[m_NumUndoImages].pixels)
    {
      memcpy(new_images[m_NumUndoImages].pixels, m_Image.GetPixels(), width * height * sizeof(RGBA));
      m_NumUndoImages++;
      delete[] m_UndoImages;
      m_UndoImages = new_images;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::ResetUndoStates()
{
  for (int i = 0; i < m_NumUndoImages; i++) {
    delete[] m_UndoImages[i].pixels;
    m_UndoImages[i].pixels = NULL;
  }
  delete[] m_UndoImages;
  m_UndoImages = NULL;
  m_NumUndoImages = 0;
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::ResetRedoStates()
{
  for (int i = 0; i < m_NumRedoImages; i++) {
    delete[] m_RedoImages[i].pixels;
    m_RedoImages[i].pixels = NULL;
  }
  delete[] m_RedoImages;
  m_RedoImages = NULL;
  m_NumRedoImages = 0;
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnPaint()
{
  CPaintDC _dc(this);
  HDC dc = _dc.m_hDC;
  CImage32 drawImage(m_Image);

  const int current_tool = m_CurrentTool;

  int width = m_Image.GetWidth();
  int height = m_Image.GetHeight();
  const RGBA* drawPixels = m_Image.GetPixels();

  if (width == 0 || height == 0 || drawPixels == NULL || !m_BlitTile || m_BlitTile->GetPixels() == NULL)
  {
    RECT ClientRect;
    GetClientRect(&ClientRect);
    FillRect(dc, &ClientRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
    return;
  }

  m_CurrentTool = 0;
  switch(m_SelectedTools[m_CurrentTool])
  {
    case Tool_Pencil:        break;
    case Tool_Fill:          break;
    case Tool_Line:          PaintLine(drawImage);      break;
    case Tool_Rectangle:     PaintRectangle(drawImage); break;
    case Tool_Circle:        PaintCircle(drawImage);    break;
    case Tool_Ellipse:       PaintEllipse(drawImage);   break;
    case Tool_Selection:     UpdateSelection();         break;
    case Tool_FreeSelection: UpdateSelection();         break;
  }

  m_CurrentTool = 1;
  switch(m_SelectedTools[m_CurrentTool])
  {
    case Tool_Pencil:        break;
    case Tool_Fill:          break;
    case Tool_Line:          PaintLine(drawImage);      break;
    case Tool_Rectangle:     PaintRectangle(drawImage); break;
    case Tool_Circle:        PaintCircle(drawImage);    break;
    case Tool_Ellipse:       PaintEllipse(drawImage);   break;
    case Tool_Selection:     UpdateSelection();         break;
    case Tool_FreeSelection: UpdateSelection();         break;
  }

  m_CurrentTool = current_tool;

  // get client rectangle
  RECT ClientRect;
  GetClientRect(&ClientRect);

  // calculate size of pixel squares
  const int hsize = ClientRect.right / width;
  const int vsize = ClientRect.bottom / height;
  int size = std::min(hsize, vsize);
  if (size < 1)
    size = 1;

  const int totalx = size * width;
  const int totaly = size * height;
#ifdef SCROLLABLE_IMAGE_WINDOW
  const int offsetx = (ClientRect.right - totalx) / 2  - m_CurrentX;
  const int offsety = (ClientRect.bottom - totaly) / 2 - m_CurrentY;
#else
  const int offsetx = (ClientRect.right - totalx)  / 2;
  const int offsety = (ClientRect.bottom - totaly) / 2;
#endif

  // draw black rectangles in the empty parts
  HBRUSH black_brush = (HBRUSH)GetStockObject(BLACK_BRUSH);
  RECT Rect;
#ifdef SCROLLABLE_IMAGE_WINDOW
  const int SCROLLBAR_WIDTH  = GetSystemMetrics(SM_CXHSCROLL);
  const int SCROLLBAR_HEIGHT = GetSystemMetrics(SM_CXVSCROLL);
#else
  const int SCROLLBAR_WIDTH  = 0;
  const int SCROLLBAR_HEIGHT = 0;
#endif

  // top
  SetRect(&Rect, 0, 0, ClientRect.right, offsety - 1);
  FillRect(dc, &Rect, black_brush);
  // bottom
  SetRect(&Rect, 0, offsety + totaly + 1, ClientRect.right, ClientRect.bottom + SCROLLBAR_HEIGHT);
  FillRect(dc, &Rect, black_brush);
  // left
  SetRect(&Rect, 0, offsety - 1, offsetx - 1, offsety + totaly + 1);
  FillRect(dc, &Rect, black_brush);
  // right
  SetRect(&Rect, offsetx + totalx + 1, offsety - 1, ClientRect.right + SCROLLBAR_WIDTH, offsety + totaly + 1);
  FillRect(dc, &Rect, black_brush);

  // assume a complete redraw
  if (m_RedrawWidth == 0 && m_RedrawHeight == 0) {
    m_RedrawX = 0;
    m_RedrawY = 0;
    m_RedrawWidth = width;
    m_RedrawHeight = height;
  }

  int dib_width = 16;
  int dib_height = 16;
//  if (m_RedrawWidth > 8 || m_RedrawHeight > 8) {
  // ensure that we redraw dib_width by dib_height squares only

  if (0) {
    m_RedrawWidth += m_RedrawX % dib_width;
    m_RedrawHeight += m_RedrawY % dib_height;
    m_RedrawX -= m_RedrawX % dib_width;
    m_RedrawY -= m_RedrawY % dib_height;
    m_RedrawWidth  += dib_width; m_RedrawWidth  -= m_RedrawWidth  % dib_width;
    m_RedrawHeight += dib_height; m_RedrawHeight -= m_RedrawHeight % dib_height;
  }

  // clamp redraw values within image
  if (m_RedrawX < 0) m_RedrawX = 0;
  if (m_RedrawY < 0) m_RedrawY = 0;
  if (m_RedrawX + m_RedrawWidth > m_Image.GetWidth()) m_RedrawWidth = m_Image.GetWidth() - m_RedrawX;
  if (m_RedrawY + m_RedrawHeight > m_Image.GetHeight()) m_RedrawHeight = m_Image.GetHeight() - m_RedrawY;

  StretchedBlit(_dc, m_BlitTile, size, size, m_Image.GetWidth(), m_Image.GetHeight(),
    drawImage.GetPixels(),  m_ShowAlphaMask, &ClientRect,
    m_RedrawX, m_RedrawY, m_RedrawWidth, m_RedrawHeight,

#ifdef SCROLLABLE_IMAGE_WINDOW
    m_CurrentX, m_CurrentY);
#else
    0, 0);
#endif
/* // this is the old image drawing code
  // draw the image
  for (int ix = 0; ix < width; ix++)
    for (int iy = 0; iy < height; iy++)
    {
      RGBA color = drawPixels[iy * width + ix];
      if (color.alpha == 255) // opaque
      {
        HBRUSH brush = CreateSolidBrush(RGB(color.red, color.green, color.blue));
        RECT Rect = { ix * size, iy * size, ix * size + size, iy * size + size };
        OffsetRect(&Rect, offsetx, offsety);
        FillRect(dc, &Rect, brush);
        DeleteObject(brush);
      }
      else // translucent
      {
        // calculate background grid colors
        RGB Color1 = CreateRGB(255, 255, 255);
        RGB Color2 = CreateRGB(128, 128, 128);
        Color1.red   = (color.red   * color.alpha + Color1.red   * (256 - color.alpha)) / 256;
        Color1.green = (color.green * color.alpha + Color1.green * (256 - color.alpha)) / 256;
        Color1.blue  = (color.blue  * color.alpha + Color1.blue  * (256 - color.alpha)) / 256;
        Color2.red   = (color.red   * color.alpha + Color2.red   * (256 - color.alpha)) / 256;
        Color2.green = (color.green * color.alpha + Color2.green * (256 - color.alpha)) / 256;
        Color2.blue  = (color.blue  * color.alpha + Color2.blue  * (256 - color.alpha)) / 256;
        HBRUSH Brush1 = CreateSolidBrush(RGB(Color1.red, Color1.green, Color1.blue));
        HBRUSH Brush2 = CreateSolidBrush(RGB(Color2.red, Color2.green, Color2.blue));
        RECT Rect;
        // draw rectangles
        // upper left
        SetRect(&Rect,
          ix * size,
          iy * size,
          ix * size + size / 2,
          iy * size + size / 2);
        OffsetRect(&Rect, offsetx, offsety);
        FillRect(dc, &Rect, Brush1);
        // upper right
        SetRect(&Rect,
          ix * size + size / 2,
          iy * size,
          ix * size + size,
          iy * size + size / 2);
        OffsetRect(&Rect, offsetx, offsety);
        FillRect(dc, &Rect, Brush2);
        // lower left
        SetRect(&Rect,
          ix * size,
          iy * size + size / 2,
          ix * size + size / 2,
          iy * size + size);
        OffsetRect(&Rect, offsetx, offsety);
        FillRect(dc, &Rect, Brush2);
        // lower right
        SetRect(&Rect,
          ix * size + size / 2,
          iy * size + size / 2,
          ix * size + size,
          iy * size + size);
        OffsetRect(&Rect, offsetx, offsety);
        FillRect(dc, &Rect, Brush1);
        DeleteObject(Brush1);
        DeleteObject(Brush2);
      }
    }
*/
  // draw the grid if it is enabled
  if (size >= 3 && m_ShowGrid) {
    HPEN linepen = CreatePen(PS_SOLID, 1, RGB(255, 0, 255));
    HPEN oldpen = (HPEN)SelectObject(dc, linepen);
    for (int ix = 0; ix <= width; ++ix) {
      MoveToEx(dc, offsetx + ix * size, offsety, NULL);
      LineTo  (dc, offsetx + ix * size, offsety + height * size);
    }
    for (int iy = 0; iy <= height; ++iy) {
      MoveToEx(dc, offsetx,                offsety + iy * size, NULL);
      LineTo  (dc, offsetx + width * size, offsety + iy * size);
    }
    SelectObject(dc, oldpen);
    DeleteObject(linepen);
  }
  // draw the selection box if it exists
  if (m_SelectionType == ST_Rectangle) {
    if (m_SelectionWidth > 0 && m_SelectionHeight > 0) {
      HPEN linepen = CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
      HPEN oldpen = (HPEN)SelectObject(dc, linepen);
      MoveToEx(dc, offsetx + m_SelectionX * size, offsety + m_SelectionY * size, NULL);
      LineTo  (dc, offsetx + m_SelectionX * size, offsety + (m_SelectionY + m_SelectionHeight) * size);
      LineTo  (dc, offsetx + (m_SelectionX + m_SelectionWidth) * size, offsety + (m_SelectionY + m_SelectionHeight) * size);
      LineTo  (dc, offsetx + (m_SelectionX + m_SelectionWidth) * size, offsety + m_SelectionY * size);
      LineTo  (dc, offsetx + m_SelectionX * size, offsety + m_SelectionY * size);
      SelectObject(dc, oldpen);
      DeleteObject(linepen);
    }
  }
  else if (m_SelectionType == ST_Free) // if (m_SelectionWidth > 0 && m_SelectionHeight > 0)
  {
    HPEN linepen = CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
    HPEN oldpen = (HPEN)SelectObject(dc, linepen);
    if (m_SelectionPoints.size() > 0)
      MoveToEx(dc, offsetx + m_SelectionPoints[0].x * size, offsety + m_SelectionPoints[0].y * size, NULL);
    for (unsigned int i = 0; i < m_SelectionPoints.size(); i++)
      LineTo(dc, offsetx + m_SelectionPoints[i].x * size, offsety + m_SelectionPoints[i].y * size);
    
    SelectObject(dc, oldpen);
    DeleteObject(linepen);
  }
  
  // draw a white rectangle around the image
  SetRect(&Rect, offsetx - 1, offsety - 1, offsetx + totalx + 1, offsety + totaly + 1);
  HPEN white_pen = CreatePen(PS_SOLID, 1, RGB(0xFF, 0xFF, 0xFF));
  HBRUSH old_brush = (HBRUSH)SelectObject(dc, GetStockObject(NULL_BRUSH));
  HPEN old_pen = (HPEN)SelectObject(dc, white_pen);
  ::Rectangle(dc, Rect.left, Rect.top, Rect.right, Rect.bottom);
  SelectObject(dc, old_pen);
  SelectObject(dc, old_brush);
  DeleteObject(white_pen);
  
/*
  POINT p = ConvertToPixel(m_CurPoint);
  if (InImage(p)) {
    HBRUSH brush = CreateSolidBrush(RGB(255, 128, 128));
    RECT Rect = { p.x * size, p.y * size, p.x * size + size, p.y * size + size };
    OffsetRect(&Rect, offsetx, offsety);
    FrameRect(dc, &Rect, brush);
    DeleteObject(brush);
  }
*/
  m_RedrawX = m_RedrawY = m_RedrawWidth = m_RedrawHeight = 0;
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::PaintLine(CImage32& pImage)
{
  if (!m_MouseDown[m_CurrentTool])
    return;

  // convert pixel coordinates to image coordinates
  POINT start = ConvertToPixel(m_StartPoint);
  POINT end = ConvertToPixel(m_CurPoint);

  pImage.Line(start.x, start.y, end.x, end.y, m_Colors[m_CurrentTool]);
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::PaintRectangle(CImage32& pImage)
{
  if (!m_MouseDown[m_CurrentTool])
    return;

  // convert pixel coordinates to image coordinates
  POINT start = ConvertToPixel(m_StartPoint);
  POINT end = ConvertToPixel(m_CurPoint);

  int x      = std::min(start.x, end.x);
  int y      = std::min(start.y, end.y);
  int width  = abs(end.x - start.x) + 1;
  int height = abs(end.y - start.y) + 1;
  
  if (GetMainWindow()->IsImageFillShapeToolChecked())
    pImage.Rectangle(x, y, width, height, m_Colors[m_CurrentTool]);
  else
    pImage.OutlinedRectangle(x, y, width, height, 1, m_Colors[m_CurrentTool]);
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::PaintCircle(CImage32& pImage)
{
  if (!m_MouseDown[m_CurrentTool])
    return;

  // convert pixel coordinates to image coordinates
  POINT start = ConvertToPixel(m_StartPoint);
  POINT end = ConvertToPixel(m_CurPoint);
  
  int radius;
  int antialias;
  
  if (abs(start.x - end.x) > abs(start.y - end.y))
    radius = abs(start.x - end.x) + 1;
  else
    radius = abs(start.y - end.y) + 1;
    
  if (GetMainWindow()->IsImageAntialiasToolChecked())
    antialias = 1;
  else
    antialias = 0;
    
  if (GetMainWindow()->IsImageFillShapeToolChecked())
    pImage.FilledCircle(start.x,   start.y, radius, m_Colors[m_CurrentTool], antialias);
  else
    pImage.OutlinedCircle(start.x, start.y, radius, m_Colors[m_CurrentTool], antialias);
    
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::PaintEllipse(CImage32& pImage)
{
  if (!m_MouseDown[m_CurrentTool])
    return;

  // convert pixel coordinates to image coordinates
  POINT start = ConvertToPixel(m_StartPoint);
  POINT end   = ConvertToPixel(m_CurPoint);
  
  if (GetMainWindow()->IsImageFillShapeToolChecked())
    pImage.FilledEllipse(start.x,   start.y, abs(start.x - end.x) + 1, abs(start.y - end.y) + 1, m_Colors[m_CurrentTool]);
  else
    pImage.OutlinedEllipse(start.x, start.y, abs(start.x - end.x) + 1, abs(start.y - end.y) + 1, m_Colors[m_CurrentTool]);
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::ClipPointToWithinImage(POINT* point)
{
  if (point->x < 0) point->x = 0;
  if (point->y < 0) point->y = 0;
  if (point->x > m_Image.GetWidth())  point->x = m_Image.GetWidth();
  if (point->y > m_Image.GetHeight()) point->y = m_Image.GetHeight();
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::UpdateSelection()
{
  if (!m_MouseDown[m_CurrentTool])
    return;

  // convert pixel coordinates to image coordinates
  POINT start = ConvertToPixel(m_StartPoint);
  POINT end = ConvertToPixel(m_CurPoint);

  // bounds check
  if (!InImage(start))
    return;

  if (m_SelectionType == ST_Rectangle)
  {
    ClipPointToWithinImage(&end);
    m_SelectionX = std::min(start.x, end.x);
    m_SelectionY = std::min(start.y, end.y);
    m_SelectionWidth  = std::max(start.x, end.x) - m_SelectionX;
    m_SelectionHeight = std::max(start.y, end.y) - m_SelectionY;  
  }
  else if (m_SelectionType == ST_Free)
  {
    if (!InImage(end))
      return;

    if (m_SelectionPoints.size() > 0)
    {
      POINT last = m_SelectionPoints[m_SelectionPoints.size() - 1];
      int rise = end.y - last.y;
      int run = end.x - last.x;
      POINT a;
      POINT b;
      if (abs(run) > abs(rise)) {
        a.x = last.x + run; a.y = last.y;
      }
      else {
        a.x = last.x; a.y = last.y + rise;
      }
      b.x = end.x; b.y = end.y;
      m_SelectionPoints.push_back(a);
      m_SelectionPoints.push_back(b);
    }
    else
    {
      m_SelectionPoints.push_back(end);
    }

    // work out SX, SY, SW, SH
    if (m_SelectionPoints.size() > 0)
    {
      unsigned int i;

      m_SelectionX = m_SelectionPoints[0].x;
      m_SelectionY = m_SelectionPoints[0].y;
      m_SelectionWidth = 0;
      m_SelectionHeight = 0;

      for (i = 0; i < m_SelectionPoints.size(); i++) {
        if (m_SelectionPoints[i].x < m_SelectionX)
          m_SelectionX = m_SelectionPoints[i].x; 
        if (m_SelectionPoints[i].y < m_SelectionY)
          m_SelectionY = m_SelectionPoints[i].y; 
      }

      for (i = 0; i < m_SelectionPoints.size(); i++) {
        if(m_SelectionPoints[i].x - m_SelectionX > m_SelectionWidth)
          m_SelectionWidth = m_SelectionPoints[i].x - m_SelectionX;
        if(m_SelectionPoints[i].y - m_SelectionY > m_SelectionHeight)
          m_SelectionHeight = m_SelectionPoints[i].y - m_SelectionY;
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnSize(UINT type, int cx, int cy)
{
#ifdef SCROLLABLE_IMAGE_WINDOW
  UpdateScrollBars();
  CWnd::OnSize(type, cx, cy);
#else
  Invalidate();
  CWnd::OnSize(type, cx, cy);
#endif
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::OnMouseClick(int index, UINT flags, CPoint point)
{
  m_LastPoint = m_CurPoint;
  m_CurPoint = point;

  if (m_MouseDown[index])
  {
    switch (m_SelectedTools[index]) {
      case Tool_Pencil:    break;
      case Tool_Fill:      break;
      case Tool_Line:      Line();  break;
      case Tool_Rectangle: Rectangle(); break;
      case Tool_Circle:    Circle(); break;
      case Tool_Ellipse:   Ellipse(); break;
      case Tool_Selection: Selection(); break;
      case Tool_FreeSelection: Selection(); break;
    }
  }
  else
  {
    if (flags & MK_SHIFT)
    {
      OnColorPicker();
    }
    else
    {
      if (m_SelectedTools[index] != Tool_Selection
       && m_SelectedTools[index] != Tool_FreeSelection)
      {
        // perform a normal click operation
        AddUndoState();
      }

      if (m_SelectedTools[index] == Tool_FreeSelection)
      {
        if (!(flags & MK_SHIFT))
        {
          m_SelectionPoints.clear();
        }
      }

      switch (m_SelectedTools[index]) {
        case Tool_Pencil:    Click(true); break;
        case Tool_Fill:      Fill();      break;
        case Tool_Line:      Line();      break;
        case Tool_Rectangle: Rectangle(); break;
        case Tool_Circle:    Circle();    break;
        case Tool_Ellipse:   Ellipse();   break;
        case Tool_Selection: Selection(); break;
        case Tool_FreeSelection: Selection(); break;
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnLButtonDown(UINT flags, CPoint point)
{
  if (flags & MK_SHIFT)
  {
    OnMouseClick(0, flags, point);
  }
  else
  {
    m_CurrentTool = 0;
    OnMouseClick(0, flags, point);
    m_MouseDown[0] = true;
    SetCapture();
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnLButtonUp(UINT flags, CPoint point)
{
  if (!m_MouseDown[0])
    return;

  OnMouseClick(0, flags, point);
  m_MouseDown[0] = false;
  ReleaseCapture();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnRButtonDown(UINT flags, CPoint point)
{
  if ( !(GetMainWindow()->GetNumImageToolsAllowed() > 1) )
    return;

  if (flags & MK_SHIFT)
  {
    OnMouseClick(1, flags, point);
  }
  else
  {
    m_CurrentTool = 1;
    OnMouseClick(1, flags, point);
    m_MouseDown[1] = true;
    SetCapture();
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnRButtonUp(UINT flags, CPoint point)
{
  if ( GetMainWindow()->GetNumImageToolsAllowed() > 1)
  {
    if (!m_MouseDown[1])
      return;

    OnMouseClick(1, flags, point);
    m_MouseDown[1] = false;
    ReleaseCapture();
    return;
  }

  // make sure we clicked in the image
  if (!InImage(ConvertToPixel(point)))
  {
    return;
  }

  // show the image view menu
  HMENU menu = LoadMenu(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_IMAGEVIEW));
  HMENU submenu = GetSubMenu(menu, 0);
  TranslateMenu(menu);
  m_CurPoint = point;
  ClientToScreen(&point);

  // disable menu items if they aren't available
  if (m_NumUndoImages == 0) {
    EnableMenuItem(submenu, ID_IMAGEVIEW_UNDO, MF_BYCOMMAND | MF_GRAYED);
  }

  if (m_NumRedoImages == 0) {
    EnableMenuItem(submenu, ID_IMAGEVIEW_REDO, MF_BYCOMMAND | MF_GRAYED);
  }

  if (GetSelectionWidth() != GetSelectionHeight()) {
    EnableMenuItem(menu, ID_IMAGEVIEW_ROTATE_CW,  MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_IMAGEVIEW_ROTATE_CCW, MF_BYCOMMAND | MF_GRAYED);
  }

  if (m_ShowGrid) {
    CheckMenuItem(menu, ID_IMAGEVIEW_VIEWGRID, MF_BYCOMMAND | MF_CHECKED);
  }

  if (m_ShowAlphaMask) {
    CheckMenuItem(menu, ID_IMAGEVIEW_TOGGLEALPHAMASK, MF_BYCOMMAND | MF_CHECKED);
  }

  bool image_on_clipboard = false;
  if (m_Clipboard) {
    if (OpenClipboard()) {
      image_on_clipboard = m_Clipboard->IsFlatImageOnClipbard() || m_Clipboard->IsBitmapImageOnClipboard();
      CloseClipboard();
    }
  }

  if (!image_on_clipboard) {
    EnableMenuItem(menu, ID_IMAGEVIEW_PASTE,               MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_IMAGEVIEW_PASTE_RGB,           MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_IMAGEVIEW_PASTE_ALPHA,         MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem(menu, ID_IMAGEVIEW_PASTE_INTOSELECTION, MF_BYCOMMAND | MF_GRAYED);
  }

  switch (m_Image.GetBlendMode()) {
    case CImage32::BLEND:      CheckMenuItem(menu, ID_IMAGEVIEW_BLENDMODE_BLEND,          MF_BYCOMMAND | MF_CHECKED); break;
    case CImage32::REPLACE:    CheckMenuItem(menu, ID_IMAGEVIEW_BLENDMODE_REPLACE,        MF_BYCOMMAND | MF_CHECKED); break;
    case CImage32::RGB_ONLY:   CheckMenuItem(menu, ID_IMAGEVIEW_BLENDMODE_RGBONLY,        MF_BYCOMMAND | MF_CHECKED); break;
    case CImage32::ALPHA_ONLY: CheckMenuItem(menu, ID_IMAGEVIEW_BLENDMODE_ALPHAONLY,      MF_BYCOMMAND | MF_CHECKED); break;
    case CImage32::ADD:        CheckMenuItem(menu, ID_IMAGEVIEW_BLENDMODE_ADDITIVE,       MF_BYCOMMAND | MF_CHECKED); break;
    case CImage32::SUBTRACT:   CheckMenuItem(menu, ID_IMAGEVIEW_BLENDMODE_SUBTRACTIVE,    MF_BYCOMMAND | MF_CHECKED); break;
    case CImage32::MULTIPLY:   CheckMenuItem(menu, ID_IMAGEVIEW_BLENDMODE_MULTIPLICATIVE, MF_BYCOMMAND | MF_CHECKED); break;
    case CImage32::AVERAGE:    CheckMenuItem(menu, ID_IMAGEVIEW_BLENDMODE_AVERAGE,        MF_BYCOMMAND | MF_CHECKED); break;
    case CImage32::INVERT:     CheckMenuItem(menu, ID_IMAGEVIEW_BLENDMODE_INVERT,         MF_BYCOMMAND | MF_CHECKED); break;
  }

  TrackPopupMenu(submenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, point.x, point.y, 0, m_hWnd, NULL);
  DestroyMenu(menu);
}

////////////////////////////////////////////////////////////////////////////////

void
CImageView::UpdateCursor(UINT flags, CPoint point)
{
  if (flags & MK_SHIFT) {
    SetCursor(LoadCursor(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDC_IMAGETOOL_COLORPICKER)));
  } else {
    /*
    switch (m_SelectedTools[0])
    {
      case Tool_Fill:
        SetCursor(LoadCursor(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDC_IMAGETOOL_FILL)));
      break;
      default:
        SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
    }
    */
    SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnMouseMove(UINT flags, CPoint point)
{
  if (flags & MK_CONTROL)
  {
    m_StartPoint.x += point.x - m_CurPoint.x;
    m_StartPoint.y += point.y - m_CurPoint.y;
  }

  m_LastPoint = m_CurPoint;
  m_CurPoint  = point;
  
  UpdateCursor(flags, point);
  POINT current = ConvertToPixel(point);

  if (InImage(current))
  {
    char str[1024];
    RGBA color = m_Image.GetPixel(current.x, current.y);
    sprintf(str, "x,y=(%d, %d) color=[%d, %d, %d, %d] (%d %d %d)", current.x, current.y, 
                  color.red, color.green, color.blue, color.alpha, m_MouseDown[0], m_MouseDown[1], m_CurrentTool);
    GetStatusBar()->SetPaneText(1, str);
  } else {
    GetStatusBar()->SetPaneText(1, "");
  }

  const int current_tool = m_CurrentTool;
  for (int i = 0; i < 2; i++)
  {
    m_CurrentTool = i;
    if (m_MouseDown[m_CurrentTool])
    {
      switch (m_SelectedTools[m_CurrentTool])
      {
        case Tool_Pencil:
          Click(false);
        break;
 
        case Tool_Fill: break;
        case Tool_Line:
        case Tool_Rectangle:
        case Tool_Circle:
        case Tool_Ellipse:
        case Tool_Selection:
        case Tool_FreeSelection:
          Invalidate();
        break;
      }
    }
  }
  m_CurrentTool = current_tool;
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void 
CImageView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnKeyDown(UINT vk, UINT nRepCnt, UINT nFlags)
{
  if (vk == VK_APPS)
  {
    POINT point = m_CurPoint;
    ClientToScreen(&point);
    OnRButtonUp(nFlags, point);
    return;
  }

  RECT ClientRect;
  GetClientRect(&ClientRect);

  // calculate size of pixel squares
  int width = m_Image.GetWidth();
  int height = m_Image.GetHeight();
  int hsize = ClientRect.right / width;
  int vsize = ClientRect.bottom / height;
  int size = std::min(hsize, vsize);
  if (size < 1)
    size = 1;

  if (vk == VK_LEFT) {
    key_left = true;
  }

  if (vk == VK_RIGHT) {
    key_right = true;
  }

  if (vk == VK_UP) {
    key_up = true;
  }

  if (vk == VK_DOWN) {
    key_down = true;
  }

  if (vk == VK_SPACE)
  {
    if (!m_MouseDown[0])
    {
      OnMouseClick(0, nFlags, m_CurPoint);
      m_MouseDown[0] = true;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnKeyUp(UINT vk, UINT nRepCnt, UINT nFlags)
{
  if (vk == VK_SPACE)
  {
    if (nFlags & MK_SHIFT)
    {
      OnMouseClick(0, nFlags, m_CurPoint);
    }
    else
    {
      if (m_MouseDown[0])
      {
        OnMouseClick(0, nFlags, m_CurPoint);
        m_MouseDown[0] = false;
      }
    }
  }

  if (vk == VK_LEFT) {
    key_left = false;
  }

  if (vk == VK_RIGHT) {
    key_right = false;
  }

  if (vk == VK_UP) {
    key_up = false;
  }

  if (vk == VK_DOWN) {
    key_down = false;
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnTimer(UINT event)
{
  RECT ClientRect;
  GetClientRect(&ClientRect);

  // calculate size of pixel squares
  int width = m_Image.GetWidth();
  int height = m_Image.GetHeight();
  int hsize = ClientRect.right / width;
  int vsize = ClientRect.bottom / height;
  int size = std::min(hsize, vsize);
  if (size < 1)
    size = 1;
  int totalx = size * width;
  int totaly = size * height;
  int offsetx = (ClientRect.right - totalx) / 2;
  int offsety = (ClientRect.bottom - totaly) / 2;

  bool cursor_moved = false;

  if (key_left) {
    POINT temp = m_CurPoint;
    temp.x -= temp.x % size;
    temp.y -= temp.y % size;
    temp.x -= size;
    if (InImage(ConvertToPixel(temp))) {
      m_LastPoint = m_CurPoint; m_CurPoint.x = temp.x;
      cursor_moved = true;
    }
  }

  if (key_right) {
    POINT temp = m_CurPoint;
    temp.x -= temp.x % size;
    temp.y -= temp.y % size;
    temp.x += size;
    if (InImage(ConvertToPixel(temp))) {
      m_LastPoint = m_CurPoint; m_CurPoint.x = temp.x;
      cursor_moved = true;
    }
  }

  if (key_up) {
    POINT temp = m_CurPoint;
    temp.x -= temp.x % size;
    temp.y -= temp.y % size;
    temp.y -= size;
    if (InImage(ConvertToPixel(temp))) {
      m_LastPoint = m_CurPoint; m_CurPoint.y  = temp.y;
      cursor_moved = true;
    }
  }

  if (key_down) {
    POINT temp = m_CurPoint;
    temp.x -= temp.x % size;
    temp.y -= temp.y % size;
    temp.y += size;
    if (InImage(ConvertToPixel(temp))) {
      m_LastPoint = m_CurPoint; m_CurPoint.y = temp.y;
      cursor_moved = true;
    }
  }

  if (!m_MouseDown[0]) {
    if (cursor_moved) {
      POINT current = ConvertToPixel(m_CurPoint);
      if (InImage(current)) {
        char str[80];
        sprintf(str, "(%d, %d)", current.x, current.y);
        GetStatusBar()->SetPaneText(1, str);
      }
      if (m_RedrawWidth == 0 && m_RedrawHeight == 0) {
        InvalidateSelection(current.x - 1, current.y - 1, 3, 3);
      }
    }
  } else {
    switch (m_SelectedTools[0])
    {
      case Tool_Pencil:
        Click(false);
      break;
      case Tool_Fill: break;
      case Tool_Line:
      case Tool_Rectangle:
      case Tool_Circle:
      case Tool_Ellipse:
      //case Tool_Selection:
      //case Tool_FreeSelection:
        Invalidate();
      break;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnColorPicker()
{
  GetColor(&m_Colors[m_CurrentTool], m_CurPoint.x, m_CurPoint.y);
  m_Handler->IV_ColorChanged(m_Colors[m_CurrentTool]);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnUndo()
{
  if (CanUndo())
    Undo();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnRedo()
{
  if (CanRedo())
    Redo();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnCopy()
{
  Copy();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnPaste()
{
  Paste();
}

///////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnPasteRGB()
{
  PasteChannels(true, true, true, false);
}

///////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnPasteAlpha()
{
  PasteChannels(false, false, false, true);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnPasteIntoSelection()
{
  PasteChannels(true, true, true, true, Merge_IntoSelection);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnToggleViewAlphaMask()
{
  m_ShowAlphaMask = !m_ShowAlphaMask;
  Invalidate();
}

//////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnViewGrid()
{
  m_ShowGrid = !m_ShowGrid;
  Invalidate();
}

///////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnBlendModeBlend()
{
  m_Image.SetBlendMode(CImage32::BLEND);
}

afx_msg void
CImageView::OnBlendModeReplace()
{
  m_Image.SetBlendMode(CImage32::REPLACE);
}

afx_msg void
CImageView::OnBlendModeRGBOnly()
{
  m_Image.SetBlendMode(CImage32::RGB_ONLY);
}

afx_msg void
CImageView::OnBlendModeAlphaOnly()
{
  m_Image.SetBlendMode(CImage32::ALPHA_ONLY);
}

afx_msg void
CImageView::OnBlendModeAdditive()
{
  m_Image.SetBlendMode(CImage32::ADD);
}

afx_msg void
CImageView::OnBlendModeSubtractive()
{
  m_Image.SetBlendMode(CImage32::SUBTRACT);
}

afx_msg void
CImageView::OnBlendModeMultiplicative()
{
  m_Image.SetBlendMode(CImage32::MULTIPLY);
}

afx_msg void
CImageView::OnBlendModeAverage()
{
  m_Image.SetBlendMode(CImage32::AVERAGE);
}

afx_msg void
CImageView::OnBlendModeInvert()
{
  m_Image.SetBlendMode(CImage32::INVERT);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnRotateCW()
{
  int sx = GetSelectionLeftX();
  int sy = GetSelectionTopY();
  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();
  RGBA* pixels = GetSelectionPixels();

  //if (sw != sh) // can only be used on sqaure selections/images
  //  return;

  AddUndoState();

  if (RotateCW(sw, sh, pixels)) {
    std::swap(sw, sh);
    if (m_SelectionWidth > 0 && m_SelectionHeight > 0) {
      m_SelectionWidth = sw;
      m_SelectionHeight = sh;
    }
  }

  UpdateSelectionPixels(pixels, sx, sy, sw, sh);
  FreeSelectionPixels(pixels);

  // things have changed
  Invalidate();
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnRotateCCW()
{
  int sx = GetSelectionLeftX();
  int sy = GetSelectionTopY();
  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();
  RGBA* pixels = GetSelectionPixels();

  AddUndoState();

  if (RotateCCW(sw, sh, pixels))
  {
    std::swap(sw, sh);
    if (m_SelectionWidth > 0 && m_SelectionHeight > 0) {
      m_SelectionWidth = sw;
      m_SelectionHeight = sh;
    }
  }

  UpdateSelectionPixels(pixels, sx, sy, sw, sh);
  FreeSelectionPixels(pixels);

  // things have changed
  Invalidate();
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnSlideUp()
{
  int sx = GetSelectionLeftX();
  int sy = GetSelectionTopY();
  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();
  RGBA* pixels = GetSelectionPixels();

  AddUndoState();

  Translate(sw, sh, pixels, 0, -1);
  UpdateSelectionPixels(pixels, sx, sy, sw, sh);

  FreeSelectionPixels(pixels);

  // things have changed
  Invalidate();
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnSlideRight()
{
  int sx = GetSelectionLeftX();
  int sy = GetSelectionTopY();
  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();
  RGBA* pixels = GetSelectionPixels();

  AddUndoState();

  Translate(sw, sh, pixels, 1, 0);
  UpdateSelectionPixels(pixels, sx, sy, sw, sh);
  FreeSelectionPixels(pixels);

  // things have changed
  Invalidate();
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnSlideDown()
{
  int sx = GetSelectionLeftX();
  int sy = GetSelectionTopY();
  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();
  RGBA* pixels = GetSelectionPixels();

  AddUndoState();

  Translate(sw, sh, pixels, 0, 1);
  UpdateSelectionPixels(pixels, sx, sy, sw, sh);
  FreeSelectionPixels(pixels);

  // things have changed
  Invalidate();
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnSlideLeft()
{
  int sx = GetSelectionLeftX();
  int sy = GetSelectionTopY();
  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();
  RGBA* pixels = GetSelectionPixels();

  AddUndoState();

  Translate(sw, sh, pixels, -1, 0);
  UpdateSelectionPixels(pixels, sx, sy, sw, sh);
  FreeSelectionPixels(pixels);

  // things have changed
  Invalidate();
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnSlideOther()
{
  char horizontal_title[1024] = {0};
  char vertical_title[1024] = {0};
  sprintf (horizontal_title, "Slide Horizontally [%d - %d]", -GetSelectionWidth(), GetSelectionWidth());
  sprintf (vertical_title,   "Slide Vertically [%d - %d]", -GetSelectionHeight(), GetSelectionHeight());
  CNumberDialog dx(horizontal_title, "Value", 0, -GetSelectionWidth(), GetSelectionWidth()); 

  if (dx.DoModal() == IDOK)
  {
    CNumberDialog dy(vertical_title, "Value", 0, -GetSelectionHeight(), GetSelectionHeight()); 
    if (dy.DoModal() == IDOK)
    {
      if (dx.GetValue() != 0 || dy.GetValue() != 0)
      {
        int sx = GetSelectionLeftX();
        int sy = GetSelectionTopY();
        int sw = GetSelectionWidth();
        int sh = GetSelectionHeight();
        RGBA* pixels = GetSelectionPixels();

        AddUndoState();

        Translate(sw, sh, pixels, dx.GetValue(), dy.GetValue());
        UpdateSelectionPixels(pixels, sx, sy, sw, sh);
        FreeSelectionPixels(pixels);

        // things have changed
        Invalidate();
        m_Handler->IV_ImageChanged();
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnFlipHorizontally()
{
  int sx = GetSelectionLeftX();
  int sy = GetSelectionTopY();
  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();
  RGBA* pixels = GetSelectionPixels();

  AddUndoState();

  FlipHorizontally(sw, sh, pixels);
  UpdateSelectionPixels(pixels, sx, sy, sw, sh);
  FreeSelectionPixels(pixels);

  // things have changed
  Invalidate();
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnFlipVertically()
{
  int sx = GetSelectionLeftX();
  int sy = GetSelectionTopY();
  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();
  RGBA* pixels = GetSelectionPixels();

  AddUndoState();

  FlipVertically(sw, sh, pixels);
  UpdateSelectionPixels(pixels, sx, sy, sw, sh);
  FreeSelectionPixels(pixels);

  // things have changed
  Invalidate();
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnFillRGB()
{
  int sx = GetSelectionLeftX();
  int sy = GetSelectionTopY();
  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();
  int width = m_Image.GetWidth();
  RGBA* pImage = m_Image.GetPixels();

  AddUndoState();

  for (int dx = sx; dx < (sx + sw); dx++) {
    for (int dy = sy; dy < sy + sh; dy++) {
      pImage[dy * width + dx].red   = m_Colors[m_CurrentTool].red;
      pImage[dy * width + dx].green = m_Colors[m_CurrentTool].green;
      pImage[dy * width + dx].blue  = m_Colors[m_CurrentTool].blue;
    } 
  }
  Invalidate();
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnFillAlpha()
{
  AddUndoState();
  int sx = GetSelectionLeftX();
  int sy = GetSelectionTopY();
  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();
  int width = m_Image.GetWidth();
  RGBA* pImage = m_Image.GetPixels();
  for (int dx = sx; dx < (sx + sw); dx++) {
    for (int dy = sy; dy < sy + sh; dy++) {
      pImage[dy * width + dx].alpha = m_Colors[m_CurrentTool].alpha;
    } 
  }
  Invalidate();
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnFillBoth()
{
  AddUndoState();
  int sx = GetSelectionLeftX();
  int sy = GetSelectionTopY();
  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();
  int width = m_Image.GetWidth();
  RGBA* pImage = m_Image.GetPixels();
  for (int dx = sx; dx < (sx + sw); dx++)
    for (int dy = sy; dy < (sy + sh); dy++)
      pImage[dy * width + dx] = m_Colors[m_CurrentTool];
  Invalidate();
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnReplaceRGBA()
{
  POINT p = ConvertToPixel(m_CurPoint);
  if (!InImage(p)) {
    return;
  }
  AddUndoState();
  RGBA color = m_Image.GetPixel(p.x, p.y);;
  int sx = GetSelectionLeftX();
  int sy = GetSelectionTopY();
  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();
  int width = m_Image.GetWidth();
  RGBA* pImage = m_Image.GetPixels();
  for (int dx = sx; dx < (sx + sw); dx++)
    for (int dy = sy; dy < (sy + sh); dy++)
      if (pImage[dy * width + dx] == color)
        pImage[dy * width + dx] = GetColor();

  Invalidate();
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnReplaceRGB()
{
  POINT p = ConvertToPixel(m_CurPoint);
  if (!InImage(p)) {
    return;
  }
  AddUndoState();
  RGBA c = m_Image.GetPixel(p.x, p.y);
  int sx = GetSelectionLeftX();
  int sy = GetSelectionTopY();
  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();
  int width = m_Image.GetWidth();
  RGBA* pImage = m_Image.GetPixels();
  for (int dx = sx; dx < (sx + sw); dx++) {
    for (int dy = sy; dy < (sy + sh); dy++) {
      if (pImage[dy * width + dx].red   == c.red &&
          pImage[dy * width + dx].green == c.green &&
          pImage[dy * width + dx].blue  == c.blue)
      {
        pImage[dy * width + dx].red   = GetColor().red;
        pImage[dy * width + dx].green = GetColor().green;
        pImage[dy * width + dx].blue  = GetColor().blue;
      }
    }
  }
  Invalidate();
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnReplaceAlpha()
{
  POINT p = ConvertToPixel(m_CurPoint);
  if (!InImage(p))
    return;
  
  AddUndoState();
  RGBA c = m_Image.GetPixel(p.x, p.y);
  int sx = GetSelectionLeftX();
  int sy = GetSelectionTopY();
  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();
  int width = m_Image.GetWidth();
  RGBA* pImage = m_Image.GetPixels();
  for (int dx = sx; dx < (sx + sw); ++dx)
    for (int dy = sy; dy < (sy + sh); ++dy)
      if (pImage[dy * width + dx].alpha == c.alpha)
        pImage[dy * width + dx].alpha = GetColor().alpha;

  Invalidate();
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnFilterGrayscale()
{
  AddUndoState();
  int sx = GetSelectionLeftX();
  int sy = GetSelectionTopY();
  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();
  int width = m_Image.GetWidth();
  RGBA* pImage = m_Image.GetPixels();
  for (int dx = sx; dx < (sx + sw); ++dx) {
    for (int dy = sy; dy < (sy + sh); ++dy) {
      int c = pImage[dy * width + dx].red + pImage[dy * width + dx].green + pImage[dy * width + dx].blue;
      pImage[dy * width + dx].red = pImage[dy * width + dx].green = pImage[dy * width + dx].blue = c / 3;
    }
  }
  Invalidate();
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnFilterSaturate()
{
  AddUndoState();
  int sx = GetSelectionLeftX();
  int sy = GetSelectionTopY();
  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();
  int width = m_Image.GetWidth();
  RGBA* pImage = m_Image.GetPixels();
  for (int dx = sx; dx < (sx + sw); ++dx) {
    for (int dy = sy; dy < (sy + sh); ++dy) {
      double r = pImage[dy * width + dx].red   / 255.0;
      double g = pImage[dy * width + dx].green / 255.0;
      double b = pImage[dy * width + dx].blue  / 255.0;
      double h, s, i;
      RGBtoHSI(r, g, b, &h, &s, &i);
      s = 0;
      HSItoRGB(h, s, i, &r, &g, &b);
      pImage[dy * width + dx].red   = (byte) (r * 255);
      pImage[dy * width + dx].green = (byte) (g * 255);
      pImage[dy * width + dx].blue  = (byte) (b * 255);
    }
  }
  Invalidate();
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnFilterColorAdjuster()
{
  CColorAdjustDialog dialog(m_Image.GetWidth(), m_Image.GetHeight(), m_Image.GetPixels());
  if (dialog.DoModal() != IDOK)
    return;
  int sx = GetSelectionLeftX();
  int sy = GetSelectionTopY();
  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();
  const int width  = m_Image.GetWidth();
  const int height = m_Image.GetHeight();
  RGBA* pixels = m_Image.GetPixels();
  int red_value   = dialog.GetRedValue();
  int green_value = dialog.GetGreenValue();
  int blue_value  = dialog.GetBlueValue();
  int alpha_value = dialog.GetAlphaValue();
  int use_red   = dialog.ShouldUseRedChannel();
  int use_green = dialog.ShouldUseGreenChannel();
  int use_blue  = dialog.ShouldUseBlueChannel();
  int use_alpha = dialog.ShouldUseAlphaChannel();
  if ((use_red || use_green || use_blue || use_alpha)
      && (red_value || green_value || blue_value && alpha_value)) {
    AddUndoState();
    int method = 1;
    if (method == 0) {
      for (int iy = 0; iy < height; iy++) {
        for (int ix = 0; ix < width; ix++) {
          if (use_red)   pixels[iy * width + ix].red   += red_value;
          if (use_green) pixels[iy * width + ix].green += green_value;
          if (use_blue)  pixels[iy * width + ix].blue  += blue_value;
          if (use_alpha) pixels[iy * width + ix].alpha += alpha_value;
        }
      } 
    } else {
      double h_value = ((double)red_value   / (double)255.0) * ((double)2.0 * 3.14);
      double s_value = ((double)green_value / (double)255.0);
      double i_value = ((double)blue_value  / (double)255.0);
      for (int iy = 0; iy < height; iy++) {
        for (int ix = 0; ix < width; ix++) {
          double r = pixels[iy * width + ix].red   / 255.0;
          double g = pixels[iy * width + ix].green / 255.0;
          double b = pixels[iy * width + ix].blue  / 255.0;
          double h, s, i;
          RGBtoHSI(r, g, b, &h, &s, &i);
          if (use_red)   h += h_value;
          if (use_green) s += s_value;
          if (use_blue)  i += i_value;
          if (use_alpha) pixels[iy * width + ix].alpha += alpha_value;
          HSItoRGB(h, s, i, &r, &g, &b);
          pixels[iy * width + ix].red   = (byte) (r * 255);
          pixels[iy * width + ix].green = (byte) (g * 255);
          pixels[iy * width + ix].blue  = (byte) (b * 255);
        }
      } 
    }
    Invalidate();
    m_Handler->IV_ImageChanged();
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnFilterCustom()
{
  CConvolveListDialog dialog(m_Image.GetWidth(), m_Image.GetHeight(), m_Image.GetPixels());
  if (dialog.DoModal() == IDOK) {
    const double* double_mask = dialog.GetMask();
    if (double_mask == NULL)
      return;
    int mask_width = dialog.GetMaskWidth();
    int mask_height = dialog.GetMaskHeight();
    if (mask_width <= 0 || mask_height <= 0)
      return;
    int offset = dialog.GetOffset();
    int divisor = (int) dialog.GetDivisor();
    int clamp = dialog.ShouldClamp();
    int clamp_low = dialog.GetClampLow();
    int clamp_high = dialog.GetClampHigh();
    int wrap = dialog.ShouldWrap();
    int infinite = 0;
    int use_red = dialog.ShouldUseRedChannel();
    int use_green = dialog.ShouldUseGreenChannel();
    int use_blue = dialog.ShouldUseBlueChannel();
    int use_alpha = dialog.ShouldUseAlphaChannel();
    const char* mask_type = dialog.GetConvolveType();
    AddUndoState();
    int sx = GetSelectionLeftX();
    int sy = GetSelectionTopY();
    int sw = GetSelectionWidth();
    int sh = GetSelectionHeight();
    RGBA* pixels = GetSelectionPixels();
    if (strcmp(mask_type, "double") == 0) {
      double_convolve_rgba(0, 0, sw, sh, sw, sh, pixels, mask_width, mask_height,
          mask_width/2, mask_height/2, double_mask,
          divisor, offset, wrap,
          clamp, clamp_low, clamp_high, infinite,
          use_red, use_green, use_blue, use_alpha);
    }
    if (strcmp(mask_type, "int") == 0) {
      int* int_mask = new int[mask_width * mask_height];
      if (int_mask) {
        for (int y = 0; y < mask_height; y++)
          for (int x = 0; x < mask_width; x++)
            int_mask[y * mask_width + x] = (int) double_mask[y * mask_width + x];
        int_convolve_rgba(0, 0, sw, sh, sw, sh, pixels, mask_width, mask_height,
            mask_width/2, mask_height/2, int_mask,
            divisor, offset, wrap,
            clamp, clamp_low, clamp_high, infinite,
            use_red, use_green, use_blue, use_alpha);
        delete[] int_mask;
        int_mask = NULL;
      }
    }
    UpdateSelectionPixels(pixels, sx, sy, sw, sh);
    FreeSelectionPixels(pixels);
    InvalidateSelection(sx, sy, sw, sh);
    m_Handler->IV_ImageChanged();
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnFilterBlur()
{
  AddUndoState();
  int sx = GetSelectionLeftX();
  int sy = GetSelectionTopY();
  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();
  RGBA* pixels = GetSelectionPixels();
  // technically this is wrong, since the wrap around stuff
  // in blur doesn't know about the pixels outside the selection
  Blur(sw, sh, pixels);
  UpdateSelectionPixels(pixels, sx, sy, sw, sh);
  FreeSelectionPixels(pixels);
  InvalidateSelection(sx, sy, sw, sh);
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnFilterNoise()
{
  AddUndoState();
  int sx = GetSelectionLeftX();
  int sy = GetSelectionTopY();
  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();
  RGBA* pixels = GetSelectionPixels();
  Noise(sw, sh, pixels);
  UpdateSelectionPixels(pixels, sx, sy, sw, sh);
  FreeSelectionPixels(pixels);
  InvalidateSelection(sx, sy, sw, sh);
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnFilterNegativeImage(bool red, bool green, bool blue, bool alpha)
{
  AddUndoState();
  int sx = GetSelectionLeftX();
  int sy = GetSelectionTopY();
  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();
  RGBA* pixels = GetSelectionPixels();
  NegativeImage(sw, sh, red, green, blue, alpha, pixels);
  UpdateSelectionPixels(pixels, sx, sy, sw, sh);
  FreeSelectionPixels(pixels);
  InvalidateSelection(sx, sy, sw, sh);
  m_Handler->IV_ImageChanged();
}

///////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnFilterNegativeImageRGB()
{
  OnFilterNegativeImage(true, true, true, false);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnFilterNegativeImageAlpha()
{
  OnFilterNegativeImage(false, false, false, true);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnFilterNegativeImageRGBA()
{
  OnFilterNegativeImage(true, true, true, true);
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnFilterSolarize()
{
  CNumberDialog dialog("Solarize Value", "Value", 128, 0, 255);
  if (dialog.DoModal() == IDOK) {
    int value = dialog.GetValue();
    AddUndoState();
    int sx = GetSelectionLeftX();
    int sy = GetSelectionTopY();
    int sw = GetSelectionWidth();
    int sh = GetSelectionHeight();
    RGBA* pixels = GetSelectionPixels();
    Solarize(sw, sh, value, pixels);
    UpdateSelectionPixels(pixels, sx, sy, sw, sh);
    FreeSelectionPixels(pixels);
    InvalidateSelection(sx, sy, sw, sh);
    m_Handler->IV_ImageChanged();
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnFilterAdjustBrightness()
{
  CNumberDialog dialog("Adjustment Value", "Value", 0, -255, 255);
  if (dialog.DoModal() == IDOK) {
    int value = dialog.GetValue();
    if (value != 0) {
      AddUndoState();
      int sx = GetSelectionLeftX();
      int sy = GetSelectionTopY();
      int sw = GetSelectionWidth();
      int sh = GetSelectionHeight();
      RGBA* pixels = GetSelectionPixels();
      AdjustBrightness(sw, sh, pixels, value, value, value);
      UpdateSelectionPixels(pixels, sx, sy, sw, sh);
      FreeSelectionPixels(pixels);
      InvalidateSelection(sx, sy, sw, sh);
      m_Handler->IV_ImageChanged();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnFilterAdjustGamma()
{
  CNumberDialog dialog("Adjustment Value", "Value", 0.0, -10.0, 10.0);
  if (dialog.DoModal() == IDOK) {
    double value = dialog.GetDoubleValue();
    if (value != 1.0) {
      AddUndoState();
      int sx = GetSelectionLeftX();
      int sy = GetSelectionTopY();
      int sw = GetSelectionWidth();
      int sh = GetSelectionHeight();
      RGBA* pixels = GetSelectionPixels();
      AdjustGamma(sw, sh, pixels, value, value, value);
      UpdateSelectionPixels(pixels, sx, sy, sw, sh);
      FreeSelectionPixels(pixels);
      InvalidateSelection(sx, sy, sw, sh);
      m_Handler->IV_ImageChanged();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnSetColorAlpha()
{
  AddUndoState();
  RGB c = { m_Colors[m_CurrentTool].red, m_Colors[m_CurrentTool].green, m_Colors[m_CurrentTool].blue };
  
  int sx = GetSelectionLeftX();
  int sy = GetSelectionTopY();
  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();
  
  // m_Image.SetColorAlpha(c, m_Color.alpha);
  m_Image.SetColorAlpha(sx, sy, sw, sh, c, m_Colors[m_CurrentTool].alpha);
  
  InvalidateSelection(sx, sy, sw, sh);
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnScaleAlpha()
{
  AddUndoState();
  RGBA* pixels = m_Image.GetPixels();
  int width = m_Image.GetWidth();
  int sx = GetSelectionLeftX();
  int sy = GetSelectionTopY();
  int sw = GetSelectionWidth();
  int sh = GetSelectionHeight();

  for (int dx = sx; dx < (sx + sw); dx++)
    for (int dy = sy; dy < (sy + sh); dy++)
      pixels[dy * width + dx].alpha = (int) pixels[dy * width + dx].alpha * m_Colors[m_CurrentTool].alpha / 255;
  
  InvalidateSelection(sx, sy, sw, sh);
  m_Handler->IV_ImageChanged();
}

////////////////////////////////////////////////////////////////////////////////

afx_msg LRESULT 
CImageView::OnGetAccelerator(WPARAM wParam, LPARAM lParam)
{
	// If you want to provide a custom accelerator, copy from CImageView::OnGetAccelerator
	// Also a message map entry is needed like: 	
	//		ON_MESSAGE(WM_GETACCELERATOR, OnGetAccelerator)
	HACCEL* ret = ((HACCEL*)wParam);
	*ret = LoadAccelerators(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_IMAGEVIEW));
	return 1;
}

////////////////////////////////////////////////////////////////////////////////

afx_msg void
CImageView::OnToolChanged(UINT id, int tool_index)
{
  switch (id) {
    case IDI_IMAGETOOL_PENCIL:        m_SelectedTools[tool_index] = Tool_Pencil;         break;
    case IDI_IMAGETOOL_LINE:          m_SelectedTools[tool_index] = Tool_Line;           break;
    case IDI_IMAGETOOL_RECTANGLE:     m_SelectedTools[tool_index] = Tool_Rectangle;      break;
    case IDI_IMAGETOOL_CIRCLE:        m_SelectedTools[tool_index] = Tool_Circle;         break;
    case IDI_IMAGETOOL_ELLIPSE:       m_SelectedTools[tool_index] = Tool_Ellipse;        break;
    case IDI_IMAGETOOL_FILL:          m_SelectedTools[tool_index] = Tool_Fill;           break;
    case IDI_IMAGETOOL_SELECTION:     m_SelectedTools[tool_index] = Tool_Selection;      break;
    case IDI_IMAGETOOL_FREESELECTION: m_SelectedTools[tool_index] = Tool_FreeSelection;  break;
  }
  TP_ToolSelected(m_SelectedTools[tool_index], tool_index);
}

////////////////////////////////////////////////////////////////////////////////
 
BOOL
CImageView::IsToolAvailable(UINT id)
{
  BOOL available = FALSE;
  switch (id) {
    case IDI_IMAGETOOL_PENCIL:        available = TRUE; break;
    case IDI_IMAGETOOL_LINE:          available = TRUE; break;
    case IDI_IMAGETOOL_RECTANGLE:     available = TRUE; break;
    case IDI_IMAGETOOL_CIRCLE:        available = TRUE; break;
    case IDI_IMAGETOOL_ELLIPSE:       available = TRUE; break;
    case IDI_IMAGETOOL_FILL_SHAPE:    available = TRUE; break;
    case IDI_IMAGETOOL_ANTIALIAS:     available = TRUE; break;
    case IDI_IMAGETOOL_FILL:          available = TRUE; break;
    case IDI_IMAGETOOL_SELECTION:     available = TRUE; break;
    case IDI_IMAGETOOL_FREESELECTION: available = TRUE; break;
    case ID_FILE_COPY:  available = TRUE; break;
    case ID_FILE_PASTE: if (IsClipboardFormatAvailable(CF_BITMAP)) available = TRUE; break;
    case ID_FILE_UNDO: if (CanUndo()) available = TRUE; break;
    case ID_FILE_REDO: if (CanRedo()) available = TRUE; break;
  }
  return available;
}

////////////////////////////////////////////////////////////////////////////////
 