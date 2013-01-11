#ifndef PACKED_HPP
#define PACKED_HPP


#include <assert.h>
#include <stdlib.h>


#ifdef NDEBUG

  #define ASSERT_STRUCT_SIZE(name, size) ;

#else

  #define ASSERT_STRUCT_SIZE(name, size) \
  static class name##_AssertStructSize__ \
  {                                      \
  public:                                \
    name##_AssertStructSize__()          \
    {                                    \
      assert(sizeof(name) == size);      \
    }                                    \
  } name##_AssertStructSize___;

#endif // NDEBUG


#endif // PACKED_HPP
