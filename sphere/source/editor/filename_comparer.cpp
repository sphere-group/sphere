#include <string.h>

////////////////////////////////////////////////////////////////////////////////

bool extension_compare(const char* path, const char* extension)
{
  int path_length = strlen(path);
  int ext_length  = strlen(extension);
  return (
   path_length >= ext_length &&
   strcmp(path + path_length - ext_length, extension) == 0
  );
}

////////////////////////////////////////////////////////////////////////////////

bool pathname_compare(const char* path, const char* folder)
{
  int path_length = strlen(path);
  int folder_length = strlen(folder);
  return (
    path_length >= folder_length &&
    memcmp(path, folder, folder_length) == 0
  );
}

////////////////////////////////////////////////////////////////////////////////
