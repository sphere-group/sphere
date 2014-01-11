#ifndef TILE_HPP
#define TILE_HPP


#include "Image32.hpp"
#include "ObstructionMap.hpp"


class sTile : public CImage32
{
public:
  sTile(int width = 16, int height = 16);
  sTile(const sTile& rhs);

  sTile& operator=(const sTile& rhs);

  sObstructionMap& GetObstructionMap();
  const sObstructionMap& GetObstructionMap() const;

  bool IsAnimated() const;
  int  GetNextTile() const;
  int  GetDelay() const;

  void SetAnimated(bool animated);
  void SetNextTile(int next_tile);
  void SetDelay(int delay);

private:
  bool m_Animated;
  int  m_NextTile;
  int  m_Delay;

  sObstructionMap m_ObstructionMap;
};


inline bool
sTile::IsAnimated() const
{
  return m_Animated;
}


inline int
sTile::GetNextTile() const
{
  return m_NextTile;
}


inline int
sTile::GetDelay() const
{
  return m_Delay;
}


inline sObstructionMap&
sTile::GetObstructionMap()
{
  return m_ObstructionMap;
}


inline const sObstructionMap&
sTile::GetObstructionMap() const
{
  return m_ObstructionMap;
}


#endif
