#ifndef VECTOR_STRUCTS_HPP
#define VECTOR_STRUCTS_HPP

#define STRUCT_NAME VECTOR_INT
#define STRUCT_BODY  \
    int x;             \
    int y;
#include "packed_struct.h"

#define STRUCT_NAME VECTOR_FLO
#define STRUCT_BODY   \
    float x;            \
    float y;
#include "packed_struct.h"

#endif

