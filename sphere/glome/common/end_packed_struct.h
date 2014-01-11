#ifdef _MSC_VER  // VC++

  };
  #pragma pack(pop)

// changing the warning back to default makes the warning come back  :(
//  #pragma warning(default : 4103)

#else            // gcc

  } __attribute__((packed));

#endif

#undef STRUCT_NAME
