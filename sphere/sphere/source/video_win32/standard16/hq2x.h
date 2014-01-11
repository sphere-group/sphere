#ifndef __HQ2X_H
#define __HQ2X_H

#include <stdlib.h>

#include "../../common/types.h"

void hq2x(word* dst, int dst_pitch, word* src, int src_width, int src_height, int bpp);

#endif
