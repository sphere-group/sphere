#ifndef SMAP_HPP
#define SMAP_HPP

#include <vector>
#include "video.hpp"
#include "../common/Map.hpp"
#include "../common/Layer.hpp"
#include "../common/Tileset.hpp"
#include "../common/IFileSystem.hpp"

class SMAP
{
public:
    struct TileDelay
    {

        int current; ///< the current tile that is displayed
        int delay;   ///< delay until next switch
        int next;    ///< next tile to be displayed
    };
    SMAP();
    ~SMAP();

    bool Load(const char* filename, IFileSystem& fs);
    void UpdateMap();

    bool UpdateSolidTile(int tile_index);
    bool UpdateTile(int tile_index);
    sMap& GetMap();
    const sMap& GetMap() const;
    void RenderLayer(int i, bool solid, int camera_x, int camera_y, int& offset_x, int& offset_y);
    int MapToScreenX(int layer, int camera_x, int mx);
    int MapToScreenY(int layer, int camera_y, int my);
    int ScreenToMapX(int layer, int camera_x, int sx);
    int ScreenToMapY(int layer, int camera_y, int sy);

    void SetLayerMask(int layer, RGBA mask);
    RGBA  GetLayerMask(int layer);

    const std::vector<TileDelay>& GetAnimationMap() const
    {

        return m_AnimationMap;
    }
    void SetLayerScaleFactorX(int layer, double factor_x);
    void SetLayerScaleFactorY(int layer, double factor_y);

    double GetLayerScaleFactorX(int layer);
    double GetLayerScaleFactorY(int layer);
    void SetLayerAngle(int layer, double angle);
    double GetLayerAngle(int layer);
private:
    void InitializeAnimation();

private:
    struct LayerInfo
    {

        int    time;
        RGBA   mask;
        double angle;
        double zoomFactorX;
        double zoomFactorY;
    };
    sMap m_Map;
    std::vector<TileDelay> m_AnimationMap;

    std::vector<IMAGE> m_Tiles;
    std::vector<IMAGE> m_SolidTiles; // used for reflectivity
    std::vector<LayerInfo> m_LayerInfo;

    // max dimensions of non-parallax layers
    int m_MaxLayerWidth;
    int m_MaxLayerHeight;
};

////////////////////////////////////////////////////////////////////////////////
inline sMap&
SMAP::GetMap()
{
    return m_Map;
}

////////////////////////////////////////////////////////////////////////////////
const inline sMap&
SMAP::GetMap() const
{
    return m_Map;
}
////////////////////////////////////////////////////////////////////////////////
inline void
SMAP::SetLayerScaleFactorX(int layer, double factor_x)
{
    m_LayerInfo[layer].zoomFactorX = factor_x;
}
inline double
SMAP::GetLayerScaleFactorX(int layer)
{
    return m_LayerInfo[layer].zoomFactorX;
}
////////////////////////////////////////////////////////////////////////////////
inline void
SMAP::SetLayerScaleFactorY(int layer, double factor_y)
{
    m_LayerInfo[layer].zoomFactorY = factor_y;
}
inline double
SMAP::GetLayerScaleFactorY(int layer)
{
    return m_LayerInfo[layer].zoomFactorY;
}
////////////////////////////////////////////////////////////////////////////////
inline void
SMAP::SetLayerAngle(int layer, double angle)
{
    m_LayerInfo[layer].angle = angle;
}
inline double
SMAP::GetLayerAngle(int layer)
{
    return m_LayerInfo[layer].angle;
}
////////////////////////////////////////////////////////////////////////////////
#endif
