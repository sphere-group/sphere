#include "EditRange.hpp"
#include "NumberDialog.hpp"
#include "FontGradientDialog.hpp"
#include "../common/Spriteset.hpp"
#include "../common/Font.hpp"
#include "../common/Tileset.hpp"
#include "resource.h"
///////////////////////////////////////////////////////////
static unsigned int
GetEditRangeIDFromMenuID(const std::string type, unsigned int id)
{
  switch (id)
  {
    case ID_SPRITESETVIEWFRAMES_ER_SLIDE_OTHER:
    case ID_TILESETVIEW_ER_SLIDE_OTHER:
    case ID_FONT_ER_SLIDE_OTHER:
      id = ID_SPRITESETVIEWFRAMES_ER_SLIDE_OTHER;
    break;
    case ID_TILESETVIEW_ER_SLIDE_UP:
    case ID_SPRITESETVIEWFRAMES_ER_SLIDE_UP:
    case ID_FONT_ER_SLIDE_UP:
      id = ID_SPRITESETVIEWFRAMES_ER_SLIDE_UP;
    break;
    case ID_TILESETVIEW_ER_SLIDE_RIGHT:
    case ID_SPRITESETVIEWFRAMES_ER_SLIDE_RIGHT:
    case ID_FONT_ER_SLIDE_RIGHT:
      id = ID_SPRITESETVIEWFRAMES_ER_SLIDE_RIGHT;
    break;
  
    case ID_TILESETVIEW_ER_SLIDE_DOWN:
    case ID_SPRITESETVIEWFRAMES_ER_SLIDE_DOWN:
    case ID_FONT_ER_SLIDE_DOWN:
      id = ID_SPRITESETVIEWFRAMES_ER_SLIDE_DOWN;
    break;
    case ID_TILESETVIEW_ER_SLIDE_LEFT:
    case ID_SPRITESETVIEWFRAMES_ER_SLIDE_LEFT:
    case ID_FONT_ER_SLIDE_LEFT:
      id = ID_SPRITESETVIEWFRAMES_ER_SLIDE_LEFT;
    break;
  
    case ID_TILESETVIEW_ER_FLIP_HORIZONTALLY:
    case ID_SPRITESETVIEWFRAMES_ER_FLIP_HORIZONTALLY:
    case ID_FONT_ER_FLIP_HORIZONTALLY:
      id = ID_SPRITESETVIEWFRAMES_ER_FLIP_HORIZONTALLY;
    break;
    case ID_TILESETVIEW_ER_FLIP_VERTICALLY:
    case ID_SPRITESETVIEWFRAMES_ER_FLIP_VERTICALLY:
    case ID_FONT_ER_FLIP_VERTICALLY:
      id = ID_SPRITESETVIEWFRAMES_ER_FLIP_VERTICALLY;
    break;
    case ID_TILESETVIEW_ER_REPLACE_RGBA:
    case ID_SPRITESETVIEWFRAMES_ER_REPLACE_RGBA:
    case ID_FONT_ER_REPLACE_RGBA:
      id = ID_SPRITESETVIEWFRAMES_ER_REPLACE_RGBA;
    break;
  }
  return id;
}
///////////////////////////////////////////////////////////
static int
GetEditRangeStartIndex(int min_value, int max_value, int default_value)
{
  CNumberDialog dialog("Start frame index", "Value", default_value, min_value, max_value);
  if (dialog.DoModal() == IDOK) {
    return dialog.GetValue();
  }
  return -1;
}
///////////////////////////////////////////////////////////
static int
GetEditRangeEndIndex(int min_value, int max_value, int default_value)
{
  CNumberDialog dialog("End frame index", "Value", default_value, min_value, max_value);
  if (dialog.DoModal() == IDOK) {
    return dialog.GetValue();
  }
  return -1;
}
///////////////////////////////////////////////////////////
static CImage32*
GetEditRangeImage(const std::string type, void* data, int frame)
{
  if (type == "spriteset") {
    sSpriteset* m_Spriteset = (sSpriteset*) data;
    return &m_Spriteset->GetImage(frame);
  }
  else
  if (type == "font") {
    sFont* m_Font = (sFont*) data;
    return &m_Font->GetCharacter(frame);
  }
  else
  if (type == "tileset") {
    sTileset* m_Tileset = (sTileset*) data;
    return &m_Tileset->GetTile(frame);
  }
  return NULL;
}
///////////////////////////////////////////////////////////
static std::vector<int>
GetEditRangeIndexes(const std::string type, const bool allow_duplicates, void* data, const int pos)
{
  std::vector<int> frames;
  if (type == "spriteset")
  {
    const sSpriteset* m_Spriteset = (sSpriteset*) data;
    const int m_CurrentDirection = pos;
    int start_frame = GetEditRangeStartIndex(0, m_Spriteset->GetNumFrames(m_CurrentDirection) - 1, 0);
    if (start_frame > -1) {
      int end_frame = GetEditRangeEndIndex(start_frame, m_Spriteset->GetNumFrames(m_CurrentDirection) - 1, start_frame);
      if (end_frame > -1)
      {
        for (int i = start_frame; i <= end_frame; i++)
        {
          int frame_index = m_Spriteset->GetFrameIndex(m_CurrentDirection, i);
          bool already_added = false;
          if (!allow_duplicates) { 
            for (int j = 0; j < int(frames.size()); j++) {
              if (frames[j] == frame_index) {
                already_added = true;
                break;
              }
            }
          }
          if (!already_added) {
            frames.push_back(frame_index);
          }
        }
      }
    }
  
  }
  if (type == "tileset") {
    const int m_SelectedTile = pos;
    const sTileset* m_Tileset = (sTileset*) data;
    int start_frame = GetEditRangeStartIndex(0, m_Tileset->GetNumTiles() - 1, m_SelectedTile);
    if (start_frame > -1) {
      int end_frame = GetEditRangeEndIndex(start_frame, m_Tileset->GetNumTiles() - 1, start_frame);
      if (end_frame > -1)
      {
        for (int i = start_frame; i <= end_frame; i++)
        {
          frames.push_back(i);
        }
      }
    }    
  }
  if (type == "font") {
    const int m_CurrentCharacter = pos;
    const sFont* m_Font = (sFont*) data;
    int start_frame = GetEditRangeStartIndex(0, m_Font->GetNumCharacters() - 1, m_CurrentCharacter);
    if (start_frame > -1) {
      int end_frame = GetEditRangeEndIndex(start_frame, m_Font->GetNumCharacters() - 1, start_frame);
      if (end_frame > -1)
      {
        for (int i = start_frame; i <= end_frame; i++)
        {
          frames.push_back(i);
        }
      }
    }    
  }
  return frames;
}
///////////////////////////////////////////////////////////
bool
EditRange::OnEditRange(const std::string type, unsigned int id, const bool allow_duplicates, void* data, const int pos)
{
  //const unsigned int id = GetCurrentMessage()->wParam;
  std::vector<int> frames = GetEditRangeIndexes(type, allow_duplicates, data, pos);
  bool changed = false;
  if (frames.size() == 0)
    return false;
  int frame_width = 0;
  int frame_height = 0;
  id = GetEditRangeIDFromMenuID(type, id);
  if (type == "spriteset") {
    const sSpriteset* m_Spriteset = (sSpriteset*) data;
    frame_width = m_Spriteset->GetFrameWidth();
    frame_height = m_Spriteset->GetFrameHeight();
  }
  if (type == "tileset") {
    const sTileset* m_Tileset = (sTileset*) data;
    frame_width = -m_Tileset->GetTileWidth();
    frame_height = -m_Tileset->GetTileHeight();
  }
  ////////////////////////////////////////////////////////////////////////////////
  if (id == ID_SPRITESETVIEWFRAMES_ER_SLIDE_OTHER
   || id == ID_SPRITESETVIEWFRAMES_ER_SLIDE_UP
   || id == ID_SPRITESETVIEWFRAMES_ER_SLIDE_RIGHT
   || id == ID_SPRITESETVIEWFRAMES_ER_SLIDE_DOWN
   || id == ID_SPRITESETVIEWFRAMES_ER_SLIDE_LEFT) {
    int dx = 0;
    int dy = 0;
    if (id == ID_SPRITESETVIEWFRAMES_ER_SLIDE_OTHER) {
      char horizontal_title[1024] = {0};
      char vertical_title[1024] = {0};
      sprintf (horizontal_title, "Slide Horizontally [%d - %d]", -frame_width, frame_width);
      sprintf (vertical_title,   "Slide Vertically [%d - %d]", -frame_height, frame_height);
      CNumberDialog dxd(horizontal_title, "Value", 0, -frame_width, frame_width); 
      if (dxd.DoModal() == IDOK) {
        CNumberDialog dyd(vertical_title, "Value", 0, -frame_height, frame_height);
        if (dyd.DoModal() == IDOK) {
          dx = dxd.GetValue();
          dy = dyd.GetValue();
        }
      }
    }
    switch (id) {
      case ID_SPRITESETVIEWFRAMES_ER_SLIDE_UP:    dy = -1; break;
      case ID_SPRITESETVIEWFRAMES_ER_SLIDE_RIGHT: dx = 1;  break;
      case ID_SPRITESETVIEWFRAMES_ER_SLIDE_DOWN:  dy = 1;  break;
      case ID_SPRITESETVIEWFRAMES_ER_SLIDE_LEFT:  dx = -1; break;
    }
    if (dx != 0 || dy != 0) {
      for (int i = 0; i < int(frames.size()); i++) {
        CImage32* image = GetEditRangeImage(type, data, frames[i]);
        image->Translate(dx, dy);
      }
      changed = true;
    }
  }
  ////////////////////////////////////////////////////////////////////////////////
  if (id == ID_SPRITESETVIEWFRAMES_ER_FLIP_HORIZONTALLY) {
    for (int i = 0; i < int(frames.size()); i++) {
      CImage32* image = GetEditRangeImage(type, data, frames[i]);
      image->FlipHorizontal();
    }
    changed = true;
  }
  ////////////////////////////////////////////////////////////////////////////////
  if (id == ID_SPRITESETVIEWFRAMES_ER_FLIP_VERTICALLY) {
    for (int i = 0; i < int(frames.size()); i++) {
      CImage32* image = GetEditRangeImage(type, data, frames[i]);
      image->FlipVertical();
    }
    changed = true;
  }
  ////////////////////////////////////////////////////////////////////////////////
  if (id == ID_SPRITESETVIEWFRAMES_ER_REPLACE_RGBA) {
    CFontGradientDialog colorChoiceDialog("Replace Color", "In", "Out");
    if (colorChoiceDialog.DoModal() == IDOK) {
      RGBA old_color = colorChoiceDialog.GetTopColor();
      RGBA replacement_color = colorChoiceDialog.GetBottomColor();
      for (int i = 0; i < int(frames.size()); i++) {
        CImage32* image = GetEditRangeImage(type, data, frames[i]);
        image->ReplaceColor(old_color, replacement_color);
      }
      
      changed = true;
    }
  }
  ////////////////////////////////////////////////////////////////////////////////
  return changed;
}
