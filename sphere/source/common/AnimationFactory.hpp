#ifndef ANIMATION_FACTORY_HPP
#define ANIMATION_FACTORY_HPP
#include "DefaultFileSystem.hpp"
#include "IAnimation.hpp"
IAnimation* LoadAnimation(const char* filename, IFileSystem& fs = g_DefaultFileSystem);
#endif
