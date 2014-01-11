#ifndef WRITE_MNG_H
#define WRITE_MNG_H

#ifndef NO_WRITE_MNG
#include <libmng.h>
class CImage32;

const char* mng_get_error_message(mng_retcode code);
typedef mng_bool   (MNG_DECL *WriteMNG_GetImage)(int image_index, CImage32& image, void* data);
typedef mng_uint32 (MNG_DECL *WriteMNG_GetDelay)(int index, void* data);
typedef mng_bool   (MNG_DECL *WriteMNG_ContinueProcessingImages)(int image_index, int total);

mng_retcode
SaveMNGAnimationFromImages(const char* filename,
                           WriteMNG_GetImage get_image,
                           WriteMNG_GetDelay get_delay,
                           WriteMNG_ContinueProcessingImages should_continue,
                           void* data);

#endif // NO_WRITE_MNG
#endif // WRITE_MNG_H
