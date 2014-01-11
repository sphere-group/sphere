#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "video.hpp"
#include "sfont.hpp"
#include "swindowstyle.hpp"
#include <string>
#include <cstdlib>

#include "../common/strcmp_ci.hpp"
struct Game
{
    std::string name;
    std::string directory;

    std::string author;
    std::string description;
    bool operator<(const Game& rhs) const
    {
        return (strcmp_ci(name.c_str(), rhs.name.c_str()) < 0);
    }
};

struct SSystemObjects
{
    sFont        font;
    sWindowStyle window_style;
    CImage32     arrow;
    CImage32     up_arrow;
    CImage32     down_arrow;
};

extern void RunSphere(int argc, const char** argv);
#endif
