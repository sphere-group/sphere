#include <stddef.h>
#include "smap.hpp"

void CalculateRotateBlitPoints(int tx[], int ty[], double x, double y, double w, double h, double radians);

////////////////////////////////////////////////////////////////////////////////

SMAP::SMAP()
{
    m_MaxLayerWidth = 0;
    m_MaxLayerHeight = 0;
}

////////////////////////////////////////////////////////////////////////////////

SMAP::~SMAP()
{
    unsigned int i;

    for (i = 0; i < m_Tiles.size(); i++)
        DestroyImage(m_Tiles[i]);
    for (i = 0; i < m_SolidTiles.size(); i++)
        DestroyImage(m_SolidTiles[i]);

    m_LayerInfo.clear();
}

////////////////////////////////////////////////////////////////////////////////

bool
SMAP::UpdateTile(int i)
{
    if (i < 0 || i >= int(m_Tiles.size()))
        return false;

    sTile& tile = m_Map.GetTileset().GetTile(i);
    if (m_Tiles[i])
        DestroyImage(m_Tiles[i]);
    m_Tiles[i] = CreateImage(tile.GetWidth(), tile.GetHeight(), tile.GetPixels());

    if (m_Tiles[i] == NULL)
        return false;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
SMAP::UpdateSolidTile(int i)
{
    if (i < 0 || i >= int(m_Tiles.size()))
        return false;

    sTile tile = m_Map.GetTileset().GetTile(i);
    // make the tile completely opaque
    for (int j = 0; j < tile.GetWidth() * tile.GetHeight(); j++)
        tile.GetPixels()[j].alpha = 0; // why not 255? o_0

    if (m_SolidTiles[i])
        DestroyImage(m_SolidTiles[i]);
    m_SolidTiles[i] = CreateImage(tile.GetWidth(), tile.GetHeight(), tile.GetPixels());

    if (m_SolidTiles[i] == NULL)
        return false;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool
SMAP::Load(const char* filename, IFileSystem& fs)
{
    unsigned int i;

    // load map
    if (m_Map.Load(filename, fs) == false)
        return false;

    // load tileset
    if (m_Map.GetTileset().GetNumTiles() == 0)
        return false;

    // initialize layer times array
    m_LayerInfo.resize(m_Map.GetNumLayers());
    if (m_LayerInfo.size()  != m_Map.GetNumLayers())
        return false;

    for (i = 0; i < m_LayerInfo.size(); i++)
    {
        m_LayerInfo[i].time = 0;
        m_LayerInfo[i].angle = 0;
        m_LayerInfo[i].zoomFactorX = 1;
        m_LayerInfo[i].zoomFactorY = 1;
        m_LayerInfo[i].mask = CreateRGBA(255, 255, 255, 255);
    }
    InitializeAnimation();
    for (i = 0; i < m_Tiles.size(); i++)
        DestroyImage(m_Tiles[i]);
    for (i = 0; i < m_SolidTiles.size(); i++)
        DestroyImage(m_SolidTiles[i]);

    // create the image array
    m_Tiles.resize(m_Map.GetTileset().GetNumTiles());
    if (m_Tiles.size() != m_Map.GetTileset().GetNumTiles())
        return false;

    std::fill(m_Tiles.begin(), m_Tiles.end(), IMAGE(0));
    for (i = 0; i < (unsigned int)m_Map.GetTileset().GetNumTiles(); i++)
        UpdateTile(i);

    // create the solid image array
    m_SolidTiles.resize(m_Map.GetTileset().GetNumTiles());
    if (m_SolidTiles.size() != m_Map.GetTileset().GetNumTiles())
        return false;

    std::fill(m_SolidTiles.begin(), m_SolidTiles.end(), IMAGE(0));
    for (i = 0; i < (unsigned int)m_Map.GetTileset().GetNumTiles(); i++)
        UpdateSolidTile(i);

    // calculate maximum non-parallax layer dimensions
    m_MaxLayerWidth = 0;
    m_MaxLayerHeight = 0;
    for (i = 0; i < (unsigned int)m_Map.GetNumLayers(); i++)
    {
        const sLayer& layer = m_Map.GetLayer(i);
        if (layer.HasParallax() == false)
        {
            if (layer.GetWidth() > m_MaxLayerWidth)
                m_MaxLayerWidth = layer.GetWidth();
            if (layer.GetHeight() > m_MaxLayerHeight)
                m_MaxLayerHeight = layer.GetHeight();
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

void
SMAP::UpdateMap()
{
    int i;

    // update layer times for autoscrolling
    for (i = m_LayerInfo.size() -1; i >= 0; --i)
		m_LayerInfo[i].time++;



    // update animations
    sTileset& tileset = m_Map.GetTileset();
    for (i = tileset.GetNumTiles() -1; i>=0; --i)
    //for (i = 0; i < (unsigned int)tileset.GetNumTiles(); i++)
    {
        sTile& tile = tileset.GetTile(i);
        if (tile.IsAnimated() && tileset.GetTile(m_AnimationMap[i].current).IsAnimated())
        {
            if (--m_AnimationMap[i].delay < 0)
            {
                int next = m_AnimationMap[i].next;
                // !!!! HACK - make sure the next tile is in bounds
                if (next >= 0 && next < tileset.GetNumTiles())
                {
                    m_AnimationMap[i].current = next;
                    m_AnimationMap[i].delay   = tileset.GetTile(next).GetDelay();
                    m_AnimationMap[i].next    = tileset.GetTile(next).GetNextTile();
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

#include <math.h>

void
SMAP::RenderLayer(int i, bool solid, int camera_x, int camera_y, int& offset_x, int& offset_y)
{
    sLayer& layer = m_Map.GetLayer(i);
    if (layer.IsVisible() == false)
        return;

    const int tile_width = m_Map.GetTileset().GetTileWidth();
    const int tile_height = m_Map.GetTileset().GetTileHeight();
    int blit_width  = (int)(tile_width  * m_LayerInfo[i].zoomFactorX);
    int blit_height = (int)(tile_height * m_LayerInfo[i].zoomFactorY);
    //m_LayerInfo[i].angle = 3.14;
    const int cx = GetScreenWidth()  / 2;
    const int cy = GetScreenHeight() / 2;

    // calculate camera offsets
    offset_x = 0;
    offset_y = 0;

    // if map is wider than the screen...
    if (blit_width * m_MaxLayerWidth > GetScreenWidth())
    {
        if (m_Map.IsRepeating() == false)
        {
            if (camera_x < cx)
                offset_x = cx - camera_x;
            else if (camera_x > (m_MaxLayerWidth * blit_width) - cx)
                offset_x = (m_MaxLayerWidth * blit_width) - camera_x - cx;
        }
    }
    else
    {
        offset_x = cx - camera_x;
    }

    // if map is higher than the screen...
    if (blit_height * m_MaxLayerHeight > GetScreenHeight())
    {
        if (m_Map.IsRepeating() == false)
        {
            if (camera_y < cy)
                offset_y = cy - camera_y;
            else if (camera_y > (m_MaxLayerHeight * blit_height) - cy)
                offset_y = (m_MaxLayerHeight * blit_height) - camera_y - cy;
        }
    }
    else
    {
        offset_y = cy - camera_y;
    }

    // calculate parallax/autoscrolling
    int parallax_x = 0;
    int parallax_y = 0;
    if (layer.HasParallax())
    {
        // autoscrolling
        parallax_x = int(m_LayerInfo[i].time * layer.GetXScrolling());
        parallax_y = int(m_LayerInfo[i].time * layer.GetYScrolling());
        // parallax
        parallax_x -= int((camera_x + offset_x - cx) * (layer.GetXParallax() - 1));
        parallax_y -= int((camera_y + offset_y - cy) * (layer.GetYParallax() - 1));
    }

    // calculate the tile to start with on the upper-left side
    int numerator_x = camera_x - cx + offset_x - parallax_x;
    int numerator_y = camera_y - cy + offset_y - parallax_y;
    while (numerator_x < 0)
        numerator_x += (blit_width * layer.GetWidth());
    while (numerator_y < 0)
        numerator_y += (blit_height * layer.GetHeight());

    // t[x,y] = indices into the layer
    // o[x,y] = rendering offsets
    // i[x,y] = number of rows/columns to render
    int ty = numerator_y / blit_height;
    int oy = -(numerator_y % blit_height);
    std::vector<IMAGE>& tiles = (solid ? m_SolidTiles : m_Tiles);

    int num_rows_to_blit = GetScreenWidth() / blit_width + 2;
    if (!m_Map.IsRepeating() && !layer.HasParallax() && num_rows_to_blit > layer.GetWidth())
        num_rows_to_blit = layer.GetWidth();

    int num_cols_to_blit = GetScreenHeight() / blit_height + 2;
    if (!m_Map.IsRepeating() && !layer.HasParallax() && num_cols_to_blit > layer.GetHeight())
        num_cols_to_blit = layer.GetHeight();

    // !!!! Warning!  Repeated code!  Please fix!
    if (m_LayerInfo[i].mask == CreateRGBA(255, 255, 255, 255))
    {
        // how many rows/columns to blit
        int iy = num_cols_to_blit;
        while (iy--)
        {
            //int tx = numerator_x / tile_width;
            //int ox = -(numerator_x % tile_width);
            int tx = numerator_x / blit_width;
            int ox = -(numerator_x % blit_width);
            int ix = num_rows_to_blit;
            while (ix--)
            {
                tx %= layer.GetWidth();
                ty %= layer.GetHeight();
                IMAGE image = tiles[m_AnimationMap[layer.GetTile(tx, ty)].current];
                if (m_LayerInfo[i].zoomFactorX != 1.0 || m_LayerInfo[i].zoomFactorY != 1.0 || m_LayerInfo[i].angle != 0.0)
                {
                    int __tx__[4];
                    int __ty__[4];
                    if (m_LayerInfo[i].angle == 0.0)
                    {
                        __tx__[0] = ox;
                        __ty__[0] = oy;
                        __tx__[1] = ox + blit_width;
                        __ty__[1] = oy;
                        __tx__[2] = ox + blit_width;
                        __ty__[2] = oy + blit_height;
                        __tx__[3] = ox;
                        __ty__[3] = oy + blit_height;
                    }
                    else
                    {
                        double angle = m_LayerInfo[i].angle - 3.14;
                        double x = ((double)ix - (double)((double)(num_rows_to_blit) / (double)2.0)) * (double)blit_width;
                        double y = ((double)iy - (double)((double)(num_cols_to_blit) / (double)2.0)) * (double)blit_height;
                        double w = ((double)y * sin(-angle)) + ((double)x * cos(-angle)) + (double)cx;
                        double h = ((double)y * cos(-angle)) - ((double)x * sin(-angle)) + (double)cy;
                        CalculateRotateBlitPoints(__tx__, __ty__, w, h, blit_width, blit_height, angle);
                    }
                    TransformBlitImage(image, __tx__, __ty__, CImage32::BLEND);
                }
                else
                {
                    BlitImage(image, ox, oy, CImage32::BLEND);
                }
                tx++;
                ox += blit_width;
            }
            ty++;
            oy += blit_height;
        }
    }
    else if (m_LayerInfo[i].mask.alpha != 0)
    {
        RGBA mask = m_LayerInfo[i].mask;
        // how many rows/columns to blit
        int iy = num_cols_to_blit;
        while (iy--)
        {
            int tx = numerator_x / blit_width;
            int ox = -(numerator_x % blit_width);
            int ix = num_rows_to_blit;
            while (ix--)
            {
                tx %= layer.GetWidth();
                ty %= layer.GetHeight();
                IMAGE image = tiles[m_AnimationMap[layer.GetTile(tx, ty)].current];
                if (m_LayerInfo[i].zoomFactorX != 1 || m_LayerInfo[i].zoomFactorY != 1 || m_LayerInfo[i].angle != 0.0)
                {
                    int tx[4];
                    int ty[4];
                    tx[0] = ox;
                    ty[0] = oy;
                    tx[1] = ox + blit_width;
                    ty[1] = oy;
                    tx[2] = ox + blit_width;
                    ty[2] = oy + blit_height;
                    tx[3] = ox;
                    ty[3] = oy + blit_height;
                    if (m_LayerInfo[i].angle != 0)
                    {
                        double x1 = (ix - (num_cols_to_blit / 2)) * blit_width;
                        double y1 = (iy - (num_rows_to_blit / 2)) * blit_height;
                        double tempx = (y1 * sin(m_LayerInfo[i].angle)) + (x1 * cos(m_LayerInfo[i].angle)) + cx;
                        double tempy = (y1 * cos(m_LayerInfo[i].angle)) - (x1 * sin(m_LayerInfo[i].angle)) + cy;
                        CalculateRotateBlitPoints(tx, ty, tempx, tempy, blit_width, blit_height, -m_LayerInfo[i].angle);
                    }
                    TransformBlitImageMask(image, tx, ty, CImage32::BLEND, mask, CImage32::MULTIPLY);
                }
                else
                {
                    BlitImageMask(image, ox, oy, CImage32::BLEND, mask, CImage32::MULTIPLY);
                }
                tx++;
                ox += blit_width;
            }
            ty++;
            oy += blit_height;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

int
SMAP::MapToScreenX(int /*layer*/, int camera_x, int mx)
{
    const int tile_width = m_Map.GetTileset().GetTileWidth();
    const int cx = GetScreenWidth() / 2;

    // calculate camera offsets
    int offset_x;

    // if map is wider than the screen...
    if (tile_width * m_MaxLayerWidth > GetScreenWidth())
    {
        if (camera_x < cx)
            offset_x = 0;
        else if (camera_x > m_MaxLayerWidth * tile_width - cx)
            offset_x = m_MaxLayerWidth * tile_width - GetScreenWidth();
        else
            offset_x = camera_x - cx;
    }
    else
    {
        offset_x = 0;
    }

    return mx - offset_x;
}

////////////////////////////////////////////////////////////////////////////////

int
SMAP::MapToScreenY(int /*layer*/, int camera_y, int my)
{
    const int tile_height = m_Map.GetTileset().GetTileHeight();
    const int cy = GetScreenHeight() / 2;

    // calculate camera offsets
    int offset_y;

    // if map is higher than the screen...
    if (tile_height * m_MaxLayerHeight > GetScreenHeight())
    {
        if (camera_y < cy)
            offset_y = 0;
        else if (camera_y > m_MaxLayerHeight * tile_height - cy)
            offset_y = m_MaxLayerHeight * tile_height - GetScreenHeight();
        else
            offset_y = camera_y - cy;
    }
    else
    {
        offset_y = 0;
    }

    return my - offset_y;
}

////////////////////////////////////////////////////////////////////////////////

int
SMAP::ScreenToMapX(int /*layer*/, int camera_x, int sx)
{
    const int tile_width = m_Map.GetTileset().GetTileWidth();
    const int cx = GetScreenWidth() / 2;

    // calculate camera offsets
    int offset_x;

    // if map is wider than the screen...
    if (tile_width * m_MaxLayerWidth > GetScreenWidth())
    {
        if (camera_x < cx)
            offset_x = 0;
        else if (camera_x > m_MaxLayerWidth * tile_width - cx)
            offset_x = m_MaxLayerWidth * tile_width - GetScreenWidth();
        else
            offset_x = camera_x - cx;
    }
    else
    {
        offset_x = 0;
    }

    return offset_x + sx;
}

////////////////////////////////////////////////////////////////////////////////

int
SMAP::ScreenToMapY(int /*layer*/, int camera_y, int sy)
{
    const int tile_height = m_Map.GetTileset().GetTileHeight();
    const int cy = GetScreenHeight() / 2;

    // calculate camera offsets
    int offset_y = 0;

    // if map is higher than the screen...
    if (tile_height * m_MaxLayerHeight > GetScreenHeight())
    {
        if (camera_y < cy)
            offset_y = 0;
        else if (camera_y > m_MaxLayerHeight * tile_height - cy)
            offset_y = m_MaxLayerHeight * tile_height - GetScreenHeight();
        else
            offset_y = camera_y - cy;
    }
    else
    {
        offset_y = 0;
    }

    return offset_y + sy;
}

////////////////////////////////////////////////////////////////////////////////

void
SMAP::SetLayerMask(int layer, RGBA color)
{
    m_LayerInfo[layer].mask = color;
}

////////////////////////////////////////////////////////////////////////////////

RGBA
SMAP::GetLayerMask(int layer)
{
    return m_LayerInfo[layer].mask;
}

////////////////////////////////////////////////////////////////////////////////

void
SMAP::InitializeAnimation()
{
    sTileset& tileset = m_Map.GetTileset();
    m_AnimationMap.resize(tileset.GetNumTiles());
    for (unsigned int i = 0; (i < (unsigned int)tileset.GetNumTiles() && i < m_AnimationMap.size()); i++)
    {

        m_AnimationMap[i].current = i;
        m_AnimationMap[i].delay = tileset.GetTile(i).GetDelay();
        m_AnimationMap[i].next = tileset.GetTile(i).GetNextTile();
    }
}

////////////////////////////////////////////////////////////////////////////////
