#ifndef LAYER_HPP
#define LAYER_HPP
#include <string>
#include "types.h"
#include "ObstructionMap.hpp"
class sLayer
{
public:
    sLayer();
    sLayer(int width, int height);
    sLayer(const sLayer& layer);
    ~sLayer();
    const sLayer& operator=(const sLayer& layer);
    void Resize(int width, int height);
    int GetWidth() const;
    int GetHeight() const;
    // replace one tile with another
    void Replace(int old_index, int new_index);
    sObstructionMap& GetObstructionMap();
    const sObstructionMap& GetObstructionMap() const;
    void SetName(const char* name);
    const char* GetName() const;
    void SetTile(int x, int y, int tile);
    int GetTile(int x, int y) const;
    int GetTileTerraFlags(int x, int y) const;
    void SetVisible(bool visible);
    bool IsVisible() const;
    void SetReflective(bool reflective);
    bool IsReflective() const;
    bool HasParallax() const;
    float32 GetXParallax() const;
    float32 GetYParallax() const;
    float32 GetXScrolling() const;
    float32 GetYScrolling() const;
    void EnableParallax(bool enable);
    void SetXParallax(float32 parallax);
    void SetYParallax(float32 parallax);
    void SetXScrolling(float32 scrolling);
    void SetYScrolling(float32 scrolling);
    void Translate(int dx, int dy);
private:
    std::string m_Name;
    int  m_Width;
    int  m_Height;
    int* m_Tiles;
    bool m_HasParallax;
    float32 m_XParallax;
    float32 m_YParallax;
    float32 m_XScrolling;
    float32 m_YScrolling;
    bool m_Visible;
    bool m_Reflective;
    sObstructionMap m_ObstructionMap;
    void Translate(int width, int height, int* m_Tiles, int dx, int dy);
};
inline int
sLayer::GetWidth() const
{
    return m_Width;
}
inline int
sLayer::GetHeight() const
{
    return m_Height;
}
inline sObstructionMap&
sLayer::GetObstructionMap()
{
    return m_ObstructionMap;
}
inline const sObstructionMap&
sLayer::GetObstructionMap() const
{
    return m_ObstructionMap;
}
inline void
sLayer::SetTile(int x, int y, int tile)
{
    m_Tiles[y * m_Width + x] = tile;
}
inline int
sLayer::GetTile(int x, int y) const
{
    return m_Tiles[y * m_Width + x];
}
inline int
sLayer::GetTileTerraFlags(int x, int y) const
{
    return (m_Tiles[y * m_Width + x] << 28) & 15;
}
inline bool
sLayer::IsVisible() const
{
    return m_Visible;
}
inline bool
sLayer::IsReflective() const
{
    return m_Reflective;
}
inline bool
sLayer::HasParallax() const
{
    return m_HasParallax;
}
inline float32
sLayer::GetXParallax() const
{
    return m_XParallax;
}
inline float32
sLayer::GetYParallax() const
{
    return m_YParallax;
}
inline float32
sLayer::GetXScrolling() const
{
    return m_XScrolling;
}
inline float32
sLayer::GetYScrolling() const
{
    return m_YScrolling;
}
inline void
sLayer::EnableParallax(bool enable)
{
    m_HasParallax = enable;
}
inline void
sLayer::SetXParallax(float32 parallax)
{
    m_XParallax = parallax;
}
inline void
sLayer::SetYParallax(float32 parallax)
{
    m_YParallax = parallax;
}
inline void
sLayer::SetXScrolling(float32 scrolling)
{
    m_XScrolling = scrolling;
}
inline void
sLayer::SetYScrolling(float32 scrolling)
{
    m_YScrolling = scrolling;
}
#endif
