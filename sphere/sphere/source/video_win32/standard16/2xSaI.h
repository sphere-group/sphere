#ifndef __2xSAIWIN_H
#define __2xSAIWIN_H

#include "../../common/types.h"

void _2xSaI(word* dst, int dst_pitch, word* src, int src_width, int src_height, int bpp);
void Super2xSaI(word* dst, int dst_pitch, word* src, int src_width, int src_height, int bpp);
void SuperEagle(word* dst, int dst_pitch, word* src, int src_width, int src_height, int bpp);

#endif
