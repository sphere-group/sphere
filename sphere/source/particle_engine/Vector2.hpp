#ifndef VECTOR2_HPP
#define VECTOR2_HPP


#include <math.h>

#include "../common/types.h"



template<typename T>
class Vector2
{
    public:

        T X;
        T Y;

        Vector2();
        Vector2(const Vector2<T>& v);
        Vector2(T x, T y);
        ~Vector2();

        Vector2<T>& operator=(const Vector2<T>& rh);

        Vector2<T> operator-() const;

        Vector2<T> operator+(const Vector2<T>& rh) const;
        Vector2<T> operator-(const Vector2<T>& rh) const;
        Vector2<T> operator*(const T rh) const;
        Vector2<T> operator/(const T rh) const;

        Vector2<T>& operator+=(const Vector2<T>& rh);
        Vector2<T>& operator-=(const Vector2<T>& rh);
        Vector2<T>& operator*=(const T rh);
        Vector2<T>& operator/=(const T rh);

};

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline
Vector2<T>::Vector2()
           : X(0)
           , Y(0)
{
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline
Vector2<T>::Vector2(const Vector2& v)
           : X(v.X)
           , Y(v.Y)
{
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline
Vector2<T>::Vector2(T x, T y)
           : X(x)
           , Y(y)
{
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline
Vector2<T>::~Vector2()
{
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline Vector2<T>&
Vector2<T>::operator=(const Vector2<T>& rh)
{
    X = rh.X;
    Y = rh.Y;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline Vector2<T>
Vector2<T>::operator-() const
{
    return Vector2<T>(-X, -Y);
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline Vector2<T>
Vector2<T>::operator+(const Vector2<T>& rh) const
{
    return Vector2<T>(X + rh.X, Y + rh.Y);
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline Vector2<T>
Vector2<T>::operator-(const Vector2<T>& rh) const
{
    return Vector2<T>(X - rh.X, Y - rh.Y);
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline Vector2<T>
Vector2<T>::operator*(const T rh) const
{
    return Vector2<T>(X * rh, Y * rh);
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline Vector2<T>
Vector2<T>::operator/(const T rh) const
{
    return Vector2<T>(X / rh, Y / rh);
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline Vector2<T>&
Vector2<T>::operator+=(const Vector2<T>& rh)
{
    X += rh.X;
    Y += rh.Y;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline Vector2<T>&
Vector2<T>::operator-=(const Vector2<T>& rh)
{
    X -= rh.X;
    Y -= rh.Y;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline Vector2<T>&
Vector2<T>::operator*=(const T rh)
{
    X *= rh;
    Y *= rh;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline Vector2<T>&
Vector2<T>::operator/=(const T rh)
{
    X /= rh;
    Y /= rh;
    return *this;
}





#endif





