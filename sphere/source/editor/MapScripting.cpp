
#include "MapScripting.hpp"
#include "../engine/time.hpp"
#include "../engine/parameters.cpp"
#include "MapView.hpp"
#define __arg_error_check__(arg_type)                                                                                          \
  if (This->m_ShouldExit) {                                                                                                    \
    JS_ReportError(cx, "%s - Argument %d, invalid %s...\n\"%s\"", script_name, arg - 1, arg_type, argStr(cx, argv[arg - 1])); \
    return JS_FALSE;                                                                                                           \
  }                                                                                                                            \

#define arg_int(name)        int name           = argInt(cx, argv[arg++]);                                            __arg_error_check__("integer")
#define arg_str(name)        const char* name   = argStr(cx, argv[arg++]);                                            __arg_error_check__("string")
#define arg_bool(name)       bool name          = argBool(cx, argv[arg++]);                                           __arg_error_check__("boolean")
#define arg_double(name)     double name        = argDouble(cx, argv[arg++]);                                         __arg_error_check__("double")
#define arg_object(name)     JSObject* name     = argObject(cx, argv[arg++]);                                         __arg_error_check__("Object")
#define arg_array(name)      JSObject* name     = argArray(cx, argv[arg++]);                                          __arg_error_check__("Array")

//#define arg_color(name)      RGBA name          = argColor(cx, argv[arg++]);                                          __arg_error_check__("Color")
//#define arg_surface(name)    CImage32* name     = argSurface(cx, argv[arg++]);     if (name == NULL) return JS_FALSE; __arg_error_check__("Surface")
//#define arg_colormatrix(name)CColorMatrix* name = argColorMatrix(cx, argv[arg++]); if (name == NULL) return JS_FALSE; __arg_error_check__("ColorMatrix")
//#define arg_byte_array(name) SS_BYTEARRAY* name = argByteArray(cx, argv[arg++]);   if (name == NULL) return JS_FALSE; __arg_error_check__("ByteArray")
//#define arg_image(name)      SS_IMAGE* name     = argImage(cx, argv[arg++]);       if (name == NULL) return JS_FALSE; __arg_error_check__("Image")
//#define arg_font(name)       SS_FONT* name      = argFont(cx, argv[arg++]);        if (name == NULL) return JS_FALSE; __arg_error_check__("Font")
//#define arg_spriteset(name)  sSpriteset* name   = argSpriteset(cx, argv[arg++]);   if (name == NULL) return JS_FALSE; __arg_error_check__("Spriteset")
// return values

#define return_int(expr)      *rval = INT_TO_JSVAL(expr)
#define return_bool(expr)     *rval = BOOLEAN_TO_JSVAL(expr)
#define return_object(expr)   *rval = OBJECT_TO_JSVAL(expr)
#define return_str(expr)      *rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, expr))
#define return_str_n(expr, n) *rval = STRING_TO_JSVAL(JS_NewStringCopyN(cx, expr, n))
#define return_double(expr)   *rval = DOUBLE_TO_JSVAL(JS_NewDouble(cx, expr))

///////////////////////////////////////////////////////////
template<typename T>
inline void USED(T /*t*/) { }
// system function definition macros and inline functions

#define begin_func(name, minargs)                                                                      \
  JSBool sMapScripting::ss##name(JSContext* cx, JSObject* obj, uintN argc, jsval* argv, jsval* rval) { \
    const char* script_name = #name;                                                                   \
    sMapScripting* This = (sMapScripting*)JS_GetContextPrivate(cx);                                    \
    if (argc < minargs) {                                                                              \
      JS_ReportError(cx, "%s called with less than %s parameters", script_name, #minargs);             \
      *rval = JSVAL_NULL;                                                                              \
      return JS_FALSE;                                                                                 \
    }                                                                                                  \
    int arg = 0;                                                                                       \
    USED(arg);

#define end_func()  \
    return (This->m_ShouldExit ? JS_FALSE : JS_TRUE); \
  }

///////////////////////////////////////////////////////////
#define MOUSE_LEFT     0
#define MOUSE_MIDDLE   1
#define MOUSE_RIGHT    2

void
sMapScripting::InitializeSphereConstants()
{
  static struct {
    const char* name;
    int         value;
  } constants[] = {

    // surface - setBlendMode constants
    { "REPLACE", CImage32::REPLACE },
    { "BLEND",   CImage32::BLEND   },
    // keyboard constants
#define KEY_CONSTANT(name) { #name, name },
    KEY_CONSTANT(MOUSE_LEFT)
    KEY_CONSTANT(MOUSE_MIDDLE)
    KEY_CONSTANT(MOUSE_RIGHT)
#undef KEY_CONSTANT
  };
  // define the constants
  for (unsigned int i = 0; i < sizeof(constants) / sizeof(*constants); i++) {
    JS_DefineProperty(
      cx,
      global,
      constants[i].name,
      INT_TO_JSVAL(constants[i].value),
      JS_PropertyStub,
      JS_PropertyStub,
      JSPROP_READONLY | JSPROP_PERMANENT
    );
  }
}
///
void
sMapScripting::InitializeSphereFunctions()
{
  static JSFunctionSpec functions[] = {
    #define SS_FUNCTION(name, numargs) { #name, ss##name, numargs, 0, 0 },
    #include "ss_functions.table"
    #undef SS_FUNCTION
    { 0, 0, 0, 0, 0 },
  };
  JS_DefineFunctions(cx, global, functions);
}
///////////////////////////////////////////////////////////
begin_func(GetTime, 0)
  return_int(GetTime());
end_func()
///////////////////////////////////////////////////////////
begin_func(Exit, 0)
  This->m_ShouldExit = true;
  This->m_Error = "";  // don't report an error (there is none)
end_func()
///////////////////////////////////////////////////////////
begin_func(Abort, 1)
  arg_str(message);
  This->m_ShouldExit = true;
  JS_ReportError(cx, "%s", message);
  return JS_FALSE;
end_func()
///////////////////////////////////////////////////////////
begin_func(SetTile, 4)
  arg_int(x);
  arg_int(y);
  arg_int(layer);
  arg_int(tile);
  CMapView* map_view = (CMapView*) This->GetPrivate();
  if (!map_view->SetTile(x, y, layer, tile)) {
    JS_ReportError(cx, "SetTile(%d, %d, %d, %d) failed...", x, y, layer, tile);
    return JS_FALSE;
  }
end_func()
////////////////////////////////////////////////////////////////////////////////
begin_func(GetTile, 3)
  arg_int(x);
  arg_int(y);
  arg_int(layer);
  CMapView* map_view = (CMapView*) This->GetPrivate();
  int tile;
  if (!map_view->GetTile(x, y, layer, tile)) {
    JS_ReportError(cx, "GetTile(%d, %d, %d) failed...", x, y, layer);
    return JS_FALSE;
  }
  return_int(tile);
end_func()
///////////////////////////////////////////////////////////
begin_func(GetMouseX, 0)
  int x = 0;
  POINT point;
  GetCursorPos(&point); 
  CMapView* map_view = (CMapView*) This->GetPrivate();
  ScreenToClient(map_view->m_hWnd, &point);
  
  x = point.x;
  return_int(x);
end_func()
///////////////////////////////////////////////////////////
begin_func(GetMouseY, 0)
  int y = 0;
  POINT point;
  GetCursorPos(&point); 
  CMapView* map_view = (CMapView*) This->GetPrivate();
  ScreenToClient(map_view->m_hWnd, &point);
  y = point.y;
  return_int(y);
end_func()
///////////////////////////////////////////////////////////
begin_func(GetLayerScaleFactorX, 1)
  arg_int(layer_index);
  double factor_x;
  CMapView* map_view = (CMapView*) This->GetPrivate();
  if (!map_view->GetLayerScaleFactorY(layer_index, factor_x)) {
    JS_ReportError(cx, "GetLayerScaleFactorX(%d) failed", layer_index);
    return JS_FALSE;
  }
  return_double(factor_x);
end_func()
///////////////////////////////////////////////////////////
begin_func(GetLayerScaleFactorY, 1)
  arg_int(layer_index);
  double factor_y;
  CMapView* map_view = (CMapView*) This->GetPrivate();
  if (!map_view->GetLayerScaleFactorY(layer_index, factor_y)) {
    JS_ReportError(cx, "GetLayerScaleFactorY(%d) failed", layer_index);
    return JS_FALSE;
  }
  return_double(factor_y);
end_func()
///////////////////////////////////////////////////////////
begin_func(GetTileWidth, 0)
  CMapView* map_view = (CMapView*) This->GetPrivate();
  int width;
  if (!map_view->GetTileWidth(width)) {
    JS_ReportError(cx, "GetTileWidth() failed...");
    return JS_FALSE;
  }
  return_int(width);
end_func()
///////////////////////////////////////////////////////////
begin_func(GetTileHeight, 0)
  CMapView* map_view = (CMapView*) This->GetPrivate();
  int height;
  if (!map_view->GetTileHeight(height)) {
    JS_ReportError(cx, "GetTileHeight() failed...");
    return JS_FALSE;
  }
  return_int(height);
end_func()
///////////////////////////////////////////////////////////
begin_func(GetNumTiles, 0)
  CMapView* map_view = (CMapView*) This->GetPrivate();
  int num_layers;
  if (!map_view->GetNumTiles(num_layers)) {
    JS_ReportError(cx, "GetNumTiles() failed...");
    return JS_FALSE;
  }
  return_int(num_layers);
end_func()
///////////////////////////////////////////////////////////
begin_func(GetLayerWidth, 1)
  arg_int(layer);
  CMapView* map_view = (CMapView*) This->GetPrivate();
  int width;
  if (!map_view->GetLayerWidth(layer, width)) {
    JS_ReportError(cx, "GetLayerWidth() failed...");
    return JS_FALSE;
  }
  return_int(width);
end_func()
///////////////////////////////////////////////////////////
begin_func(GetLayerHeight, 1)
  arg_int(layer);
  CMapView* map_view = (CMapView*) This->GetPrivate();
  int height;
  if (!map_view->GetLayerHeight(layer, height)) {
    JS_ReportError(cx, "GetLayerHeight() failed...");
    return JS_FALSE;
  }
  return_int(height);
end_func()
///////////////////////////////////////////////////////////
begin_func(GetNumLayers, 0)
  CMapView* map_view = (CMapView*) This->GetPrivate();
  int num_layers;
  if (!map_view->GetNumLayers(num_layers)) {
    JS_ReportError(cx, "GetNumLayers() failed...");
    return JS_FALSE;
  }
  return_int(num_layers);
end_func()
//////////////////////////////////////////////////////////
begin_func(MapToScreenX, 2)
  arg_int(layer);
  arg_int(mx);
  CMapView* map_view = (CMapView*) This->GetPrivate();
  int sx;
  if (!map_view->MapToScreenX(layer, mx, sx)) {
    JS_ReportError(cx, "MapToScreenX() failed");
    return JS_FALSE;
  }
  return_int(sx);
end_func()
////////////////////////////////////////////////////////////////////////////////
begin_func(MapToScreenY, 2)
  arg_int(layer);
  arg_int(my);
  CMapView* map_view = (CMapView*) This->GetPrivate();
  int sy;
  if (!map_view->MapToScreenY(layer, my, sy)) {
    JS_ReportError(cx, "MapToScreenY() failed");
    return JS_FALSE;
  }
  return_int(sy);
end_func()
////////////////////////////////////////////////////////////////////////////////
begin_func(ScreenToMapX, 2)
  arg_int(layer);
  arg_int(sx);
  CMapView* map_view = (CMapView*) This->GetPrivate();
  int mx;
  if (!map_view->ScreenToMapX(layer, sx, mx)) {
    JS_ReportError(cx, "ScreenToMapX() failed");
    return JS_FALSE;
  }
  return_int(mx);
end_func()
////////////////////////////////////////////////////////////////////////////////
begin_func(ScreenToMapY, 2)
  arg_int(layer);
  arg_int(sy);
  CMapView* map_view = (CMapView*) This->GetPrivate();
  int my;
  if (!map_view->ScreenToMapY(layer, sy, my)) {
    JS_ReportError(cx, "ScreenToMapY() failed");
    return JS_FALSE;
  }
  return_int(my);
end_func()
////////////////////////////////////////////////////////////////////////////////
begin_func(IsMouseButtonPressed, 1)
  arg_int(button);
  if ( !(button == MOUSE_LEFT || button == MOUSE_RIGHT || button == MOUSE_MIDDLE) ) {
    JS_ReportError(cx, "Invalid mouse button");
    return JS_FALSE;
  }
  CMapView* map_view = (CMapView*) This->GetPrivate();
  return_bool(map_view->IsMouseButtonPressed(button));
end_func()
////////////////////////////////////////////////////////////////////////////////
begin_func(GetScreenWidth, 0)
  CMapView* map_view = (CMapView*) This->GetPrivate();
  return_int(map_view->GetScreenWidth());
end_func()
////////////////////////////////////////////////////////////////////////////////
begin_func(GetScreenHeight, 0)
  CMapView* map_view = (CMapView*) This->GetPrivate();
  return_int(map_view->GetScreenHeight());
end_func()
////////////////////////////////////////////////////////////////////////////////
