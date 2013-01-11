#ifndef SPHERE_MINMAX_HPP
#define SPHERE_MINMAX_HPP
#include <algorithm>
// if you have problems with this file in vc6
// make sure NOMINMAX is defined
#if defined(_MSC_VER) && _MSC_VER <= 1200
namespace std
{

    template<typename T>
    T min(T a, T b)
    {

        return (a < b ? a : b);
    }
    template<typename T>
    T max(T a, T b)
    {

        return (a > b ? a : b);
    }
}
#endif
#endif
