#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP

#include <jsapi.h>

// Addition to jsapi.h
#define JSVAL_IS_OBJECTNOTNULL(v)      ( (JSVAL_TAG(v) == JSVAL_OBJECT) && ((v) != JSVAL_NULL) && ((v) != JSVAL_VOID))


/*
 * float numbers will be converted (rounded) automatically to the nearest integer, and so are strings
 * and objects. Objects need the method .valueOf() to be converted to number.
 */
inline int argInt(JSContext* cx, jsval arg)
{
    // If the value is an integer, dispatch as soon as possible (8% performance boost)
    if (JSVAL_IS_INT(arg))
        return JSVAL_TO_INT(arg);

    // The value is something else, convert it to integer
    int32 i;
    if (JS_ValueToECMAInt32(cx, arg, &i)) //used to be JS_ValueToInt32, which rounded negatives differently.
		return i;

    JS_ReportError(cx, "Invalid integer.");
    return 0; // invalid integer
}

int argStrictInt(JSContext* cx, jsval arg);
const char* argStr(JSContext* cx, jsval arg);
bool argBool(JSContext* cx, jsval arg);
double argDouble(JSContext* cx, jsval arg);
JSObject* argObject(JSContext* cx, jsval arg);
JSObject* argArray(JSContext* cx, jsval arg);
JSObject* argFunctionObject(JSContext* cx, jsval arg);

#endif
