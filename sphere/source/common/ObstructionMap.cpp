#include <math.h>
#include "ObstructionMap.hpp"

////////////////////////////////////////////////////////////////////////////////
sObstructionMap::sObstructionMap()
{}

////////////////////////////////////////////////////////////////////////////////
sObstructionMap::~sObstructionMap()
{}

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
sObstructionMap::Segment&
sObstructionMap::GetSegmentRef(int i)
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

    for (unsigned int i = 0; i < m_Segments.size(); i++)
    {
        if (TestSegments(s, m_Segments[i]))
        {
            return true;
        }
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
#include <assert.h>
bool
sObstructionMap::TestSegments(const Segment& s1, const Segment& s2)
{
    struct Local
    {
        static inline int square(int a)
        {
            return (a * a);
        }

        static inline int min(int a, int b)
        {
            return (a < b ? a : b);
        }

        static inline int max(int a, int b)
        {
            return (a < b ? b : a);
        }
    };

    // special case check for segments with no length
    int s1_length_squared = Local::square(s1.x2 - s1.x1) + Local::square(s1.y2 - s1.y1);
    int s2_length_squared = Local::square(s2.x2 - s2.x1) + Local::square(s2.y2 - s2.y1);
    if (s1_length_squared == 0 || s2_length_squared == 0)
    {

        if ((s1.x1 == s1.x2) && (s1.y1 == s1.y2))
        {
            // test horizontal line
            if (s2.y1 == s2.y2)
            {

                int cx = s1.x1;
                int cy = s1.y1;
                // test (s2.x1 >= s1.x1) && (s2.x2 <= s1.x1)
                if ( (cx >= Local::min(s2.x1, s2.x2)) && (cx <= Local::max(s2.x1, s2.x2))
                        &&  (cy >= Local::min(s2.y1, s2.y2)) && (cy <= Local::max(s2.y1, s2.y2)) )
                {

                    return true;
                }
            }
            // test vertical line
            if (s2.x1 == s2.x2)
            {

                // test for pixel within segment
                int cx = s1.x1;
                int cy = s1.y1;
                // test (s2.x1 >= s1.x1) && (s2.x2 <= s1.x1)
                if ( (cx >= Local::min(s2.x1, s2.x2)) && (cx <= Local::max(s2.x1, s2.x2))
                        &&  (cy >= Local::min(s2.y1, s2.y2)) && (cy <= Local::max(s2.y1, s2.y2)) )
                {

                    return true;
                }
            }
        }
        return false;
    }

    // algorithm from http://www.gamedev.net/columns/features/diariesandsites/javaextreme/2d.htm
    // probably moved to http://www.gamedev.net/hosted/javaextreme/2d.htm
    struct Point
    {
        int x;
        int y;
    };

    const Point A =
        {
            s1.x1, s1.y1
        };
    const Point B =
        {
            s1.x2, s1.y2
        };
    const Point C =
        {
            s2.x1, s2.y1
        };
    const Point D =
        {
            s2.x2, s2.y2
        };

    const double num_r = ((A.y - C.y) * (D.x - C.x)) - ((A.x - C.x) * (D.y - C.y));
    const double num_s = ((A.y - C.y) * (B.x - A.x)) - ((A.x - C.x) * (B.y - A.y));
    const double den   = ((B.x - A.x) * (D.y - C.y)) - ((B.y - A.y) * (D.x - C.x));

    // lines are coincident
    if (den == 0 && num_r == 0)
    {

        if (A.x == B.x)
        {   // if line is vertical, do test based on y values

            const int min_AB_y = Local::min(A.y, B.y);
            const int max_AB_y = Local::max(A.y, B.y);

            return (
                       (C.y >= min_AB_y && C.y <= max_AB_y) ||
                       (D.y >= min_AB_y && D.y <= max_AB_y)
                   );

        }
        else
        {            // otherwise, do general test based on x values

            const int min_AB_x = Local::min(A.x, B.x);
            const int max_AB_x = Local::max(A.x, B.x);

            return (
                       (C.x >= min_AB_x && C.x <= max_AB_x) ||
                       (D.x >= min_AB_x && D.x <= max_AB_x)
                   );

        }
    }
    // I'm assuming that if den ==  0 and A.x != B.x then there is no obstruction
    if (den == 0) return false;
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
    int point = FindSegmentByPoint(x, y);
    if (point != -1)
    {
        RemoveSegment(point);
    }
}

////////////////////////////////////////////////////////////////////////////////
int
sObstructionMap::FindSegmentByPoint(int x, int y)
{
    // if there aren't any segments, don't do anything
    if (m_Segments.size() == 0)
    {
        return -1;
    }

    // find minimum distance
    double distance = DistanceToSegment(x, y, 0);
    int point = 0;
    for (unsigned int i = 1; i < m_Segments.size(); i++)
    {
        double new_distance = DistanceToSegment(x, y, i);
        if (new_distance < distance)
        {
            distance = new_distance;
            point = i;
        }
    }

    return point;
}

////////////////////////////////////////////////////////////////////////////////
template<typename T>
T square(T t)
{
    return t * t;
}

////////////////////////////////////////////////////////////////////////////////
double
sObstructionMap::DistanceToSegment(int x, int y, int i)
{
    Segment& s = m_Segments[i];

    double n = (x - s.x1) * (s.x2 - s.x1) + (y - s.y1) * (s.y2 - s.y1);
    double d = square(s.x2 - s.x1) + square(s.y2 - s.y1);
    if (d == 0)
    {  // line segment with 0 length
        return DistanceToPoint(x, y, s.x1, s.y1);
    }
    double u = n / d;

    if (u < 0)
    {
        return DistanceToPoint(x, y, s.x1, s.y1);
    }
    else if (u > 1)
    {
        return DistanceToPoint(x, y, s.x2, s.y2);
    }
    else
    {
        int x3 = int(s.x1 + u * (s.x2 - s.x1));
        int y3 = int(s.y1 + u * (s.y2 - s.y1));
        return DistanceToPoint(x, y, x3, y3);
    }
}

////////////////////////////////////////////////////////////////////////////////
double
sObstructionMap::DistanceToPoint(int x1, int y1, int x2, int y2)
{
    return (double)sqrt((double)(square(x2 - x1) + square(y2 - y1)));
}

////////////////////////////////////////////////////////////////////////////////
void
sObstructionMap::Simplify()
{
    if (m_Segments.size() == 0)
        return;

    unsigned int i = 0;
    do
    {
        sObstructionMap::Segment& A = m_Segments[i];

        int A_vertical=(A.x2-A.x1==0);
        int mA,bA;
        if (!A_vertical)
        {
            mA=(A.y2 - A.y1) / (A.x2 - A.x1);
            bA=A.y1 - (mA * A.x1);
        }

        for (unsigned int j = i+1; j < m_Segments.size(); j++)
        {
            sObstructionMap::Segment& B = m_Segments[j];

            int B_vertical=(B.x2-B.x1==0);
            if (B_vertical && A_vertical)
            {
                if (B.x1==A.x1)
                {
                    // vertical lines are at same x
                    // so test for y overlap
                    if ( (A.y1 <= B.y1 && A.y2 >= B.y1) ||
                            (A.y1 <= B.y2 && A.y2 >= B.y2) )
                    {
                        // coordinates overlap
                        // determine endpoints of unified line
                        int new_y1=(A.y1 < B.y1) ? (A.y1):(B.y1);
                        int new_y2=(A.y2 < B.y2) ? (B.y2):(A.y2);

                        // change the A line to the new line
                        A.y1=new_y1;
                        A.y2=new_y2;

                        // swap B line to end and delete
                        int lastSegment=m_Segments.size()-1;
                        sObstructionMap::Segment& L=m_Segments[lastSegment];
                        std::swap(B,L);
                        RemoveSegment(lastSegment);

                        // restart j loop
                        j=i;

                        continue;
                    }
                }
            }
            else if (!A_vertical && !B_vertical)
            {
                // determine m and b of segment B
                int mB=(B.y2 - B.y1) / (B.x2 - B.x1);
                int bB=B.y1 - (mB * B.x1);

                // is A and B the same line
                if (mA==mB && bA==bB)
                {
                    // if so check for overlapping X coords
                    // (along the same line you may check
                    // either coordinate for incidence)
                    if ( (A.x1 <= B.x1 && A.x2 >= B.x1) ||
                            (A.x1 <= B.x2 && A.x2 >= B.x2) )
                    {
                        // coordinates overlap
                        // determine endpoints of unified line
                        int new_x1,new_x2;// master
                        int new_y1,new_y2;// slave (y=mx+b)

                        new_x1=(A.x1 < B.x1) ? (A.x1):(B.x1);
                        new_x2=(A.x2 < B.x2) ? (B.x2):(A.x2);
                        new_y1=(mA*new_x1)+bA;
                        new_y2=(mA*new_x2)+bA;

                        // A line becomes the new line
                        A.x1=new_x1;
                        A.x2=new_x2;
                        A.y1=new_y1;
                        A.y2=new_y2;

                        // swap B line to end and delete
                        int lastSegment=m_Segments.size()-1;
                        sObstructionMap::Segment& L=m_Segments[lastSegment];
                        std::swap(B,L);
                        RemoveSegment(lastSegment);

                        // restart j loop
                        j=i;

                        continue;
                    }
                }
            }
        }
        ++i;
    }
    while ((i+1)<m_Segments.size());
}

////////////////////////////////////////////////////////////////////////////////
