#include <math.h>
#include "ObstructionMap.hpp"


////////////////////////////////////////////////////////////////////////////////

sObstructionMap::sObstructionMap()
{
}

////////////////////////////////////////////////////////////////////////////////

sObstructionMap::~sObstructionMap()
{
}

////////////////////////////////////////////////////////////////////////////////

int
sObstructionMap::GetNumSegments() const
{
  return m_Segments.size();
}

////////////////////////////////////////////////////////////////////////////////

const sObstructionMap::Segment&
sObstructionMap::GetSegment(int i) const
{
  return m_Segments[i];
}

////////////////////////////////////////////////////////////////////////////////

bool
sObstructionMap::TestRectangle(int x1, int y1, int x2, int y2) const
{
  return (
    TestSegment(x1, y1, x2, y1) ||  // top
    TestSegment(x2, y1, x2, y2) ||  // right
    TestSegment(x1, y2, x2, y2) ||  // bottom
    TestSegment(x1, y1, x1, y2)     // left
  );
}

////////////////////////////////////////////////////////////////////////////////

bool
sObstructionMap::TestSegment(int x1, int y1, int x2, int y2) const
{
  Segment s = { x1, y1, x2, y2 };

  for (int i = 0; i < m_Segments.size(); i++) {
    if (TestSegments(s, m_Segments[i])) {
      return true;
    }    
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////

bool
sObstructionMap::TestSegments(const Segment& s1, const Segment& s2)
{
  struct Local {
    static inline int square(int a) {
      return (a * a);
    }

    static inline int min(int a, int b) {
      return (a < b ? a : b);
    }

    static inline int max(int a, int b) {
      return (a < b ? b : a);
    }
  };


  // special case check for segments with no length
  int s1_length_squared = Local::square(s1.x2 - s1.x1) + Local::square(s1.y2 - s1.y1);
  int s2_length_squared = Local::square(s2.x2 - s2.x1) + Local::square(s2.y2 - s2.y1);
  if (s1_length_squared == 0 || s2_length_squared == 0) {
    return false;
  }

  // algorithm from http://www.gamedev.net/columns/features/diariesandsites/javaextreme/2d.htm

  struct Point {
    int x;
    int y;
  };

  const Point A = { s1.x1, s1.y1 };
  const Point B = { s1.x2, s1.y2 };
  const Point C = { s2.x1, s2.y1 };
  const Point D = { s2.x2, s2.y2 };

  const double num_r = (A.y - C.y) * (D.x - C.x) - (A.x - C.x) * (D.y - C.y);
  const double num_s = (A.y - C.y) * (B.x - A.x) - (A.x - C.x) * (B.y - A.y);
  const double den   = (B.x - A.x) * (D.y - C.y) - (B.y - A.y) * (D.x - C.x);
  
  // lines are coincident
  if (den == 0 && num_r == 0) {
    
    if (A.x == B.x) {   // if line is vertical, do test based on y values

      const int min_AB_y = Local::min(A.y, B.y);
      const int max_AB_y = Local::max(A.y, B.y);


      return (
        (C.y >= min_AB_y && C.y <= max_AB_y) ||
        (D.y >= min_AB_y && D.y <= max_AB_y)
      );

    } else {            // otherwise, do general test based on x values

      const int min_AB_x = Local::min(A.x, B.x);
      const int max_AB_x = Local::max(A.x, B.x);

      return (
        (C.x >= min_AB_x && C.x <= max_AB_x) ||
        (D.x >= min_AB_x && D.x <= max_AB_x)
      );

    }

  }

  double r = num_r / den;
  double s = num_s / den;
  
  return (r >= 0 && r <= 1 && s >= 0 && s <= 1);
}

////////////////////////////////////////////////////////////////////////////////

void
sObstructionMap::AddSegment(int x1, int y1, int x2, int y2)
{
  Segment s = { x1, y1, x2, y2 };
  m_Segments.push_back(s);
}

////////////////////////////////////////////////////////////////////////////////

void
sObstructionMap::RemoveSegment(int i)
{
  m_Segments.erase(m_Segments.begin() + i);
}

////////////////////////////////////////////////////////////////////////////////

void
sObstructionMap::RemoveSegmentByPoint(int x, int y)
{
  // if there aren't any segments, don't do anything
  if (m_Segments.size() == 0) {
    return;
  }

  // find minimum distance
  double distance = DistanceToSegment(x, y, 0);
  int point = 0;
  for (int i = 1; i < m_Segments.size(); i++) {
    double new_distance = DistanceToSegment(x, y, i);
    if (new_distance < distance) {
      distance = new_distance;
      point = i;
    }
  }

  // remove that point
  RemoveSegment(point);
}

////////////////////////////////////////////////////////////////////////////////

template<typename T>
T square(T t) {
  return t * t;
}

////////////////////////////////////////////////////////////////////////////////

double
sObstructionMap::DistanceToSegment(int x, int y, int i)
{
  Segment& s = m_Segments[i];

  double n = (x - s.x1) * (s.x2 - s.x1) + (y - s.y1) * (s.y2 - s.y1);
  double d = square(s.x2 - s.x1) + square(s.y2 - s.y1);
  if (d == 0) {  // line segment with 0 length
    return DistanceToPoint(x, y, s.x1, s.y1);
  }
  double u = n / d;

  if (u < 0) {
    return DistanceToPoint(x, y, s.x1, s.y1);
  } else if (u > 1) {
    return DistanceToPoint(x, y, s.x2, s.y2);
  } else {
    int x3 = s.x1 + u * (s.x2 - s.x1);
    int y3 = s.y1 + u * (s.y2 - s.y1);
    return DistanceToPoint(x, y, x3, y3);
  }
}

////////////////////////////////////////////////////////////////////////////////

double
sObstructionMap::DistanceToPoint(int x1, int y1, int x2, int y2)
{
  return sqrt(square(x2 - x1) + square(y2 - y1));
}

////////////////////////////////////////////////////////////////////////////////
