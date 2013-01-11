#include <string.h>
#include "AnimationFactory.hpp"
#include "FLICAnimation.hpp"
#include "MNGAnimation.hpp"
////////////////////////////////////////////////////////////////////////////////
inline bool has_extension(const char* filename, const char* ext)
{

    int filename_length = strlen(filename);
    int ext_length = strlen(ext);
    return (filename_length > ext_length && strcmp(filename + filename_length - ext_length, ext) == 0);
}
IAnimation* LoadAnimation(const char* filename, IFileSystem& fs)
{
    if (has_extension(filename, ".mng") || has_extension(filename, ".jng"))
    {

        // MNG
        CMNGAnimation* animation = new CMNGAnimation;
        if (!animation->Load(filename, fs))
        {

            delete animation;
            return NULL;
        }
        return animation;
    }
    else if (

        has_extension(filename, ".flic") ||
        has_extension(filename, ".flc") ||
        has_extension(filename, ".fli")
    )
    {

        CFLICAnimation* animation = new CFLICAnimation;
        if (!animation->Load(filename, fs))
        {

            delete animation;
            return NULL;
        }
        return animation;
    }
    return NULL;
}
////////////////////////////////////////////////////////////////////////////////
