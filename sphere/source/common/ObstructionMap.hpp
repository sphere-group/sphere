#ifndef OBSTRUCTION_MAP_HPP
#define OBSTRUCTION_MAP_HPP

#include <vector>
class sObstructionMap
{
public:
    struct Segment
    {
        int x1;
        int y1;
        int x2;
        int y2;
    };

    sObstructionMap();
    ~sObstructionMap();

    // accessors
    int GetNumSegments() const;
    const Segment& GetSegment(int i) const;

    Segment& GetSegmentRef(int i);
    // test
    bool TestRectangle(int x1, int y1, int x2, int y2) const;
    bool TestSegment(int x1, int y1, int x2, int y2) const;
    static bool TestSegments(const Segment& s1, const Segment& s2);

    // mutators
    void AddSegment(int x1, int y1, int x2, int y2);
    void RemoveSegment(int i);
    void RemoveSegmentByPoint(int x, int y);

    void Simplify();
    int FindSegmentByPoint(int x, int y);
private:
    double DistanceToSegment(int x, int y, int i);
    double DistanceToPoint(int x1, int y1, int x2, int y2);

private:
    std::vector<Segment> m_Segments;
};

#endif
