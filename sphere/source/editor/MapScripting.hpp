#ifndef MAP_SCRIPTING
#define MAP_SCRIPTING
#include "Scripting.hpp"
class sMapScripting : public sScripting {
private:
  #define SS_FUNCTION(name, numargs) static JSBool ss##name(JSContext* cx, JSObject* obj, uintN argc, jsval* argv, jsval* rval);
  #include "ss_functions.table"
  #undef SS_FUNCTION
public:
  void InitializeSphereConstants();
  void InitializeSphereFunctions();
};
#endif
