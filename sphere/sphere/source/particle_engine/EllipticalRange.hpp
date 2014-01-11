#ifndef ELLIPTICAL_RANGE_HPP
#define ELLIPTICAL_RANGE_HPP


#include <math.h>

#include "Range.hpp"
#include "Vector2.hpp"



template<typename T>
class EllipticalRange
{
    public:

        Range<T> Angle;
        Range<T> A;
        Range<T> B;

        EllipticalRange();
        EllipticalRange(const EllipticalRange<T>& range);
        EllipticalRange(const Range<T>& ang, const Range<T>& a, const Range<T>& b);

        ~EllipticalRange();

        EllipticalRange<T>& operator=(const EllipticalRange<T>& rh);

        Vector2<T> Random() const;
        Vector2<T> Random(const Vector2<T>& direction) const;

};

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline
EllipticalRange<T>::EllipticalRange()
{
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline
EllipticalRange<T>::EllipticalRange(const EllipticalRange<T>& range)
                   : Angle(range.Angle)
                   , A(range.A)
                   , B(range.B)
{
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline
EllipticalRange<T>::EllipticalRange(const Range<T>& ang, const Range<T>& a, const Range<T>& b)
                   : Angle(ang)
                   , A(a)
                   , B(b)
{
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline
EllipticalRange<T>::~EllipticalRange()
{
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline EllipticalRange<T>&
EllipticalRange<T>::operator=(const EllipticalRange<T>& rh)
{
    Angle = rh.Angle;
    A     = rh.A;
    B     = rh.B;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline Vector2<T>
EllipticalRange<T>::Random() const
{
    T ang = Angle.Random();
    return Vector2<T>(A.Random() * cos(ang), B.Random() * sin(ang));
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline Vector2<T>
EllipticalRange<T>::Random(const Vector2<T>& direction) const
{
    T ang = atan2(direction.Y, direction.X);
    return Vector2<T>(A.Random() * cos(ang), B.Random() * sin(ang));
}




#endif




