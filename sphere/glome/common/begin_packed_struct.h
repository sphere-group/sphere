#ifdef _MSC_VER  // VC++

  // change packing across header includes
  #pragma warning(disable : 4103)

  #pragma pack(push, 1)
  struct STRUCT_NAME {

#else            // gcc

  struct STRUCT_NAME {

#endif
