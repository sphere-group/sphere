#ifndef SIMAGE_HPP
#define SIMAGE_HPP

#include "video.hpp"
#include "../common/IFileSystem.hpp"

extern IMAGE SLoadImage(const char* filename, IFileSystem& fs);
extern IMAGE SLoadImageScale(const char* filename, IFileSystem& fs, int width, int height);

#endif
