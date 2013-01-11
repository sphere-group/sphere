#ifndef SPRITESET_SERVER_HPP
#define SPRITESET_SERVER_HPP

#include <string>
#include <vector>
#include "sspriteset.hpp"

class CSpritesetServer
{
public:
    CSpritesetServer();
    ~CSpritesetServer();

    SSPRITESET* Load(const char* filename, IFileSystem& fs);
    void Free(SSPRITESET* spriteset);

private:
    struct Spriteset
    {
        std::string name;
        int         refcount;
        SSPRITESET* spriteset;
    };

private:
    std::vector<Spriteset> m_Spritesets;
};

#endif
