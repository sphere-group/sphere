#include "platform.h"

#ifndef STRUCT_NAME
#  error You must define STRUCT_NAME before including packed_struct.h
#endif

#ifndef STRUCT_BODY
#  error You must define STRUCT_BODY before including packed_struct.h
#endif

#ifdef SPHERE_MVC  // VC++
#  pragma pack(push, 1)
struct STRUCT_NAME
{
    STRUCT_BODY
};
#  pragma pack(pop)

#else            // assume gcc
struct __attribute__((packed)) STRUCT_NAME
{
    STRUCT_BODY
};

#endif
#undef STRUCT_NAME
#undef STRUCT_BODY
