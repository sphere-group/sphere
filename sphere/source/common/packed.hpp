#ifndef PACKED_HPP
#define PACKED_HPP

#include <assert.h>
#include <stdlib.h>

#include "platform.h"

#ifdef NDEBUG
# define ASSERT_STRUCT_SIZE(name, size) ;
#else
# if __has_feature(cxx_static_assert)
#  define ASSERT_STRUCT_SIZE(name, size) static_assert(sizeof(name) == size, "sizeof(" # name ") == " # size);
# elif __has_feature(c_static_assert)
#  define ASSERT_STRUCT_SIZE(name, size) _Static_assert(sizeof(name) == size, "sizeof(" # name ") == " # size);
# else // SPHERE_CLANG
#  define ASSERT_STRUCT_SIZE(name, size)		\
	static class name##_AssertStructSize__	\
  {											\
  public:									\
  name##_AssertStructSize__()				\
	{										\
	assert(sizeof(name) == size);			\
	}										\
  } name##_AssertStructSize___;
# endif // !SPHERE_CLANG
#endif // NDEBUG

#endif // PACKED_HPP
