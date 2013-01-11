#ifndef RANGE_HPP
#define RANGE_HPP


#include <stdlib.h>



template<typename T>
class Range
{
    public:

        T Min;
        T Max;

        Range();
        Range(const Range<T>& range);
        Range(T min, T max);
        ~Range();

        Range<T>& operator=(const Range<T>& rh);

        T Random() const;

};

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline
Range<T>::Range()
         : Min(0)
         , Max(0)
{
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline
Range<T>::Range(const Range<T>& range)
         : Min(range.Min)
         , Max(range.Max)
{
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline
Range<T>::Range(T min, T max)
         : Min(min)
         , Max(max)
{
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline
Range<T>::~Range()
{
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline Range<T>&
Range<T>::operator=(const Range<T>& rh)
{
    Min = rh.Min;
    Max = rh.Max;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline T
Range<T>::Random() const
{
    return Min + ((Max - Min) * (rand() * (1.0f / RAND_MAX)));
}




#endif




