#include <string.h>
#include "Tile.hpp"
////////////////////////////////////////////////////////////////////////////////
sTile::sTile(int width, int height)
        : CImage32(width, height)
        , m_Animated(false)
        , m_NextTile(0)
        , m_Delay(0)
        , m_Terraform(false)
{
    Clear();
}
////////////////////////////////////////////////////////////////////////////////
sTile::sTile(const sTile& rhs)
{
    CImage32::operator=(rhs);
    m_Animated       = rhs.m_Animated;
    m_NextTile       = rhs.m_NextTile;
    m_Delay          = rhs.m_Delay;
    m_ObstructionMap = rhs.m_ObstructionMap;
    m_Name           = rhs.m_Name;
    m_Terraform      = rhs.m_Terraform;
}
////////////////////////////////////////////////////////////////////////////////
sTile&
sTile::operator=(const sTile& rhs)
{
    CImage32::operator=(rhs);
    m_Animated       = rhs.m_Animated;
    m_NextTile       = rhs.m_NextTile;
    m_Delay          = rhs.m_Delay;
    m_ObstructionMap = rhs.m_ObstructionMap;
    m_Name           = rhs.m_Name;
    m_Terraform      = rhs.m_Terraform;
    return *this;
}
////////////////////////////////////////////////////////////////////////////////
void
sTile::SetAnimated(bool animated)
{
    m_Animated = animated;
}
////////////////////////////////////////////////////////////////////////////////
void
sTile::SetNextTile(int next_tile)
{
    m_NextTile = next_tile;
}
////////////////////////////////////////////////////////////////////////////////
void
sTile::SetDelay(int delay)
{
    m_Delay = delay;
}
////////////////////////////////////////////////////////////////////////////////
void
sTile::SetName(std::string tile_name)
{
    m_Name = tile_name;
}
////////////////////////////////////////////////////////////////////////////////
void
sTile::SetTerraformable(bool terraform)
{
    m_Terraform = terraform;
}
////////////////////////////////////////////////////////////////////////////////
