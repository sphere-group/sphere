#ifndef __SCALE_H
#define __SCALE_H

#include "../../common/types.h"

void DirectScale(word* dst, int dst_pitch, word* src, int src_width, int src_height);
void Scale2x(word* dst, int dst_pitch, word* src, int src_width, int src_height);
void Eagle(word* dst, int dst_pitch, word* src, int src_width, int src_height);

#endif
