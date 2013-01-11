// Configuration server
// It will store and retrieve aggregate data and C++ std::strings
// I broke a lot of coding conventions here, because VC++ doesn't deal with templates well
// Quite frankly, this code is awful.  However, it works properly since I spent so much time
//   working around bugs in VC++'s implementation of template functions
//
// Summary of use:
//   Okay, the Configuration namespace is a smart state server which allows you to access
//   data via objects known as keys.  Each key has an identification string, a data type,
//   and a default value associated with it.  A key is an instance of any struct that has
//   a typedef in it named "type" and a static const tchar* named "keyname" along with a
//   default_value field.
//
// If you have a key named "KEY_MY_DATA" which represents a boolean value, you could read
// from the configuration with code like this:
// bool b = Configuration::Get(KEY_MY_DATA);
//
// If you want to store a new value in the configuration database, do this:
// Configuration::Set(KEY_MY_DATA, true);

#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <windows.h>
#include <string>

// begin namespace
// I had to put these in a non-class namespace so VC++ would accept these template functions...
// I can't be a good programmer and put this in an object in case there needs to be more than one configuration.  ;_;
// Down with VC++!  Long live gcc!
namespace Configuration {
    
	extern std::string ConfigurationFile;

    inline unsigned FromHex(char c) {
      if (c >= 'A' && c <= 'F') {
        return (c - 'A' + 10);
      }

      if (c >= 'a' && c <= 'f') {
        return (c - 'a' + 10);
      }

      if (c >= '0' && c <= '9') {
        return (c - '0');
      }

      return 0;
    }

    // Retrieves a configuration setting
    // Usage:  var = Configuration::Get(KEY);
    template<typename T>
    typename T::type Get(T /*key*/)
    {
      char str[8193];
      GetPrivateProfileString("editor", T::keyname, "", str, 8193, ConfigurationFile.c_str());

      int size = GetTypeSize<T::type>(T::type());
      if (int(strlen(str)) < size * 2) {
        return T::default_value;
      }

      BYTE* data = new BYTE[size];
      if (!data)
        return T::default_value;

      for (int i = 0; i < size; i++) {
        data[i] = (FromHex(str[i * 2]) * 16) + FromHex(str[(i * 2) + 1]);
      }

      T::type t = FromRaw<T::type>(data, T::type());
      delete[] data;

      return t;
    }

    
    inline char ToHex(unsigned i) {
      return (i >= 10 ? (char)('A' + i - 10) : (char)('0' + i));
    }

    // Sets a configuration setting
    // Usage:  Configuration::Set(KEY, value);
    template<typename T>
    void Set(T /*key*/, typename T::type val)
    {
      std::string hex;
      BYTE* data = (BYTE*)ToRaw(val);
      if (!data) return;

      for (int i = 0; i < GetTypeSize<T::type>(T::type()); i++) {
        hex += ToHex(data[i] >> 4);
        hex += ToHex(data[i] & 0x0F);
      }
      delete[] data;

      if (WritePrivateProfileString("editor", T::keyname, hex.c_str(), ConfigurationFile.c_str()) == 0) {
      }
    }
    

    // WARNING! These functions/variables should not be used outside of this namespace
    // if I have a default implementation and a specialized implementation, the default implementation is never called
    // therefore, there are specialized implementations for everything...
    // Down with VC++!  Long live gcc!

    // *** GetTypeSize() ***
    template<typename T> int GetTypeSize(T t);
    // GRAH, how many of these template problems will I find in one day?
    // these functions now have a worthless parameter like FromRaw does
    template<> inline int GetTypeSize<bool>(bool /*b = bool()*/) { return sizeof(bool); }
    template<> inline int GetTypeSize<unsigned char>(unsigned char /*uc = unsigned char()*/) { return sizeof(unsigned char); }
    template<> inline int GetTypeSize<int>(int /*i = int()*/) { return sizeof(int); }
    template<> inline int GetTypeSize<double>(double /*d = double()*/) { return sizeof(double); }
	template<> inline int GetTypeSize<std::string>(std::string /*s = std::string()*/) { return 4096; } // only supports up to 4096-1 characters
    template<> inline int GetTypeSize<WINDOWPLACEMENT>(WINDOWPLACEMENT /*wp = WINDOWPLACEMENT()*/) { return sizeof(WINDOWPLACEMENT); }
    template<> inline int GetTypeSize<RECT>(RECT /*w = RECT()*/) { return sizeof(RECT); }
    
    // *** ToRaw() ***

    template<typename T> void* ToRaw(T val);
    template<>
    inline void* ToRaw<bool>(bool val)
    {
        BYTE* b = new BYTE[sizeof(bool)];
        if (b) memcpy(b, &val, sizeof(bool));
        return b;
    }

    template<>
    inline void* ToRaw<unsigned char>(unsigned char val)
    {
        BYTE* b = new BYTE[sizeof(unsigned char)];
        if (b) memcpy(b, &val, sizeof(unsigned char));
        return b;
    }

    template<>
    inline void* ToRaw<int>(int val)
    {
        BYTE* b = new BYTE[sizeof(int)];
        if (b) memcpy(b, &val, sizeof(int));
        return b;
    }

    template<>
    inline void* ToRaw<double>(double val)
    {
        BYTE* b = new BYTE[sizeof(double)];
        if (b) memcpy(b, &val, sizeof(double));
        return b;
    }

    template<>
    inline void* ToRaw<std::string>(std::string val)
    {

        int size = GetTypeSize<std::string>(std::string());
        char* str = new char[size];
        if (str) {

          memset(str, 0, size);
          strcpy(str, val.c_str());
        }
        return str;
    }

    template<>
    inline void* ToRaw<WINDOWPLACEMENT>(WINDOWPLACEMENT wp)
    {
      BYTE* b = new BYTE[sizeof(WINDOWPLACEMENT)];
      if (b) memcpy(b, &wp, sizeof(WINDOWPLACEMENT));
      return b;
    }

    template<>
    inline void* ToRaw<RECT>(RECT r)
    {
      BYTE* b = new BYTE[sizeof(RECT)];
      if (b) memcpy(b, &r, sizeof(RECT));
      return b;
    }

    // *** FromRaw() ***
    template<typename T> T FromRaw(void* raw, T t = T());
    // due to yet *another* bug in VC++, I added an extra parameter to these functions so they can be looked up properly
    // Down with VC++!  Long live gcc!

    template<>    
    inline bool FromRaw<bool>(void* raw, bool /*b*/)
    {
        bool t;
        memcpy(&t, raw, sizeof(bool));
        return t;
    }

    template<>
    inline unsigned char FromRaw<unsigned char>(void* raw, unsigned char /*i*/)
    {
        unsigned char t;
        memcpy(&t, raw, sizeof(unsigned char));
        return t;
    }

    template<>
    inline int FromRaw<int>(void* raw, int /*i*/)
    {
        int t;
        memcpy(&t, raw, sizeof(int));
        return t;
    }

    template<>    
    inline double FromRaw<double>(void* raw, double /*d*/)
    {
        double t;
        memcpy(&t, raw, sizeof(double));
        return t;
    }

    template<>    
    inline std::string FromRaw<std::string>(void* raw, std::string /*s*/)
    {
        return std::string((char*)raw);
    }

    template<>
    inline WINDOWPLACEMENT FromRaw<WINDOWPLACEMENT>(void* raw, WINDOWPLACEMENT /*wp*/)
    {
      WINDOWPLACEMENT wp;
      memcpy(&wp, raw, sizeof(WINDOWPLACEMENT));
      return wp;
    }

    template<>
    inline RECT FromRaw<RECT>(void* raw, RECT /*r*/)
    {
      RECT r;
      memcpy(&r, raw, sizeof(RECT));
      return r;
    }
}

// end namespace
#endif
