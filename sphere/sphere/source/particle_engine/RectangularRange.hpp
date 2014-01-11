#ifndef RECTANGULAR_RANGE_HPP
#define RECTANGULAR_RANGE_HPP


#include "Range.hpp"
#include "Vector2.hpp"



template<typename T>
class RectangularRange
{
    public:

        Range<T> X;
        Range<T> Y;

        RectangularRange();
        RectangularRange(const RectangularRange<T>& range);
        RectangularRange(const Range<T>& x, const Range<T>& y);
        ~RectangularRange();

        RectangularRange<T>& operator=(const RectangularRange<T>& rh);

        Vector2<T> Random() const;

};

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline
RectangularRange<T>::RectangularRange()
{
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline
RectangularRange<T>::RectangularRange(const RectangularRange<T>& range)
                    : X(range.X)
                    , Y(range.Y)
{
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline
RectangularRange<T>::RectangularRange(const Range<T>& x, const Range<T>& y)
                    : X(x)
                    , Y(y)
{
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline
RectangularRange<T>::~RectangularRange()
{
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline RectangularRange<T>&
RectangularRange<T>::operator=(const RectangularRange<T>& rh)
{
    X = rh.X;
    Y = rh.Y;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline Vector2<T>
RectangularRange<T>::Random() const
{
    return Vector2<T>(X.Random(), Y.Random());
}





#endif




