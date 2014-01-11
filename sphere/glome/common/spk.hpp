#ifndef SPK_HPP
#define SPK_HPP


// Sphere Package Headers
#include "types.h"


#define STRUCT_NAME SPK_HEADER
#include "begin_packed_struct.h"
  byte  signature[4];
  word  version;
  dword num_files;
  dword index_offset;
  byte  reserved[2];
#include "end_packed_struct.h"


#define STRUCT_NAME SPK_ENTRY
#include "begin_packed_struct.h"
  word  entry_version;
  word  file_name_length;
  dword file_offset;
  dword file_size;
  dword compressed_size;
#include "end_packed_struct.h"


#endif
