
#include "strcmp_ci.hpp"
#include "ends_with.hpp"
#include <string.h>

int ends_with(const char* filename, const char* str)
{
  if (strlen(filename) >= strlen(str)) {
    return (memcmp(filename + strlen(filename) - strlen(str), str, strlen(str)) == 0 ? 1 : 0);
  }
  return 0;
}

int ends_with_ci(const char* filename, const char* str)
{
  if (strlen(filename) >= strlen(str)) {
    return (strcmp_ci(filename + strlen(filename) - strlen(str), str) == 0 ? 1 : 0);
  }
  return 0;
}

