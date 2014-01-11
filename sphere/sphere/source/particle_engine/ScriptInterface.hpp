#ifndef SCRIPT_INTERFACE_HPP
#define SCRIPT_INTERFACE_HPP


#include <jsapi.h>


// forward declaration
class ParticleSystemBase;


class ScriptInterface
{
    protected:

        JSContext* m_Context; // javascript context
        JSObject*  m_Object;  // javascript object, which owns the particle system

        bool       m_Protected; // object protected from being garbage-collected

        // callback functions
        jsval    m_OnUpdateFunc;
        jsval    m_OnRenderFunc;
        jsval    m_OnBirthFunc;
        jsval    m_OnDeathFunc;

        JSObject* m_TextureObject;

    public:

        class Comparator
        {
            private:

                JSContext* m_Context;
                JSObject*  m_Global;
                jsval      m_CompareFunc;

            public:

                explicit Comparator(JSContext* context,
                                    JSObject* object,
                                    jsval comp_func) : m_Context(context)
                                                     , m_Global(object)
                                                     , m_CompareFunc(comp_func)
                {
                    JS_AddRoot(m_Context, &m_CompareFunc);
                }

                Comparator(const Comparator& comp) : m_Context(comp.m_Context)
                                                   , m_Global(comp.m_Global)
                                                   , m_CompareFunc(comp.m_CompareFunc)
                {
                    JS_AddRoot(m_Context, &m_CompareFunc);
                }

                ~Comparator()
                {
                    // balancing call to JS_RemoveRoot
                    JS_RemoveRoot(m_Context, &m_CompareFunc);
                }

                Comparator& operator=(const Comparator& comp)
                {
                    m_Context     = comp.m_Context;
                    m_Global      = comp.m_Global;
                    m_CompareFunc = comp.m_CompareFunc;
                    return *this;
                }

                bool operator()(JSObject* a, JSObject* b, bool& out_result)
                {
                    jsval succeeded, result;
                    jsval arguments[2] = {OBJECT_TO_JSVAL(a), OBJECT_TO_JSVAL(b)};

                    succeeded = JS_CallFunctionValue(m_Context,
                                                     m_Global,
                                                     m_CompareFunc,
                                                     2, arguments,
                                                     &result);

                    if (JSVAL_IS_BOOLEAN(result))
                        out_result = (JSVAL_TRUE == result);
                    else
                        out_result = true;

                    return succeeded == JS_TRUE;
                }

        };

        class Applicator
        {
            private:

                JSContext* m_Context;
                jsval      m_ApplyFunc;

            public:

                explicit Applicator(JSContext* context,
                                    jsval apply_func) : m_Context(context)
                                                      , m_ApplyFunc(apply_func)
                {
                    JS_AddRoot(m_Context, &m_ApplyFunc);
                }

                Applicator(const Applicator& appl) : m_Context(appl.m_Context)
                                                   , m_ApplyFunc(appl.m_ApplyFunc)
                {
                    JS_AddRoot(m_Context, &m_ApplyFunc);
                }

                ~Applicator()
                {
                    // balancing call to JS_RemoveRoot
                    JS_RemoveRoot(m_Context, &m_ApplyFunc);
                }

                Applicator& operator=(const Applicator& appl)
                {
                    m_Context   = appl.m_Context;
                    m_ApplyFunc = appl.m_ApplyFunc;
                    return *this;
                }

                bool operator()(JSObject* object)
                {
                    jsval rval;
                    return JS_CallFunctionValue(m_Context,
                                                object,
                                                m_ApplyFunc,
                                                0, NULL,
                                                &rval) == JS_TRUE;
                }

        };

        ScriptInterface();
        ScriptInterface(const ScriptInterface& interface);

        ~ScriptInterface();

        bool Init(JSContext* context, JSObject* object);
        JSObject* GetObject() const;
        bool IsProtected() const;
        bool StartProtection();
        void EndProtection();

        bool  HasOnUpdate() const;
        jsval GetOnUpdate() const;
        void  SetOnUpdate(jsval on_update);

        bool  HasOnRender() const;
        jsval GetOnRender() const;
        void  SetOnRender(jsval on_render);

        bool  HasOnBirth() const;
        jsval GetOnBirth() const;
        void  SetOnBirth(jsval on_birth);

        bool  HasOnDeath() const;
        jsval GetOnDeath() const;
        void  SetOnDeath(jsval on_death);

        void OnUpdate() const;
        void OnRender() const;
        void OnBirth(ParticleSystemBase* parent) const;
        void OnDeath(ParticleSystemBase* parent) const;

        JSObject* GetTextureObject() const;
        void      SetTextureObject(JSObject* tex_obj);

};

////////////////////////////////////////////////////////////////////////////////
inline JSObject*
ScriptInterface::GetObject() const
{
    return m_Object;
}

////////////////////////////////////////////////////////////////////////////////
inline bool
ScriptInterface::IsProtected() const
{
    return m_Protected;
}

////////////////////////////////////////////////////////////////////////////////
inline bool
ScriptInterface::HasOnUpdate() const
{
    return m_OnUpdateFunc != JSVAL_NULL;

}

////////////////////////////////////////////////////////////////////////////////
inline jsval
ScriptInterface::GetOnUpdate() const
{
    return m_OnUpdateFunc;
}

////////////////////////////////////////////////////////////////////////////////
inline void
ScriptInterface::SetOnUpdate(jsval on_update)
{
    m_OnUpdateFunc = on_update;
}

////////////////////////////////////////////////////////////////////////////////
inline bool
ScriptInterface::HasOnRender() const
{
    return m_OnRenderFunc != JSVAL_NULL;

}

////////////////////////////////////////////////////////////////////////////////
inline jsval
ScriptInterface::GetOnRender() const
{
    return m_OnRenderFunc;
}

////////////////////////////////////////////////////////////////////////////////
inline void
ScriptInterface::SetOnRender(jsval on_render)
{
    m_OnRenderFunc = on_render;
}

////////////////////////////////////////////////////////////////////////////////
inline bool
ScriptInterface::HasOnBirth() const
{
    return m_OnBirthFunc != JSVAL_NULL;

}

////////////////////////////////////////////////////////////////////////////////
inline jsval
ScriptInterface::GetOnBirth() const
{
    return m_OnBirthFunc;
}

////////////////////////////////////////////////////////////////////////////////
inline void
ScriptInterface::SetOnBirth(jsval on_birth)
{
    m_OnBirthFunc = on_birth;
}

////////////////////////////////////////////////////////////////////////////////
inline bool
ScriptInterface::HasOnDeath() const
{
    return m_OnDeathFunc != JSVAL_NULL;

}

////////////////////////////////////////////////////////////////////////////////
inline jsval
ScriptInterface::GetOnDeath() const
{
    return m_OnDeathFunc;
}

////////////////////////////////////////////////////////////////////////////////
inline void
ScriptInterface::SetOnDeath(jsval on_death)
{
    m_OnDeathFunc = on_death;
}

////////////////////////////////////////////////////////////////////////////////
inline JSObject*
ScriptInterface::GetTextureObject() const
{
    return m_TextureObject;
}

////////////////////////////////////////////////////////////////////////////////
inline void
ScriptInterface::SetTextureObject(JSObject* tex_obj)
{
    m_TextureObject = tex_obj;
}




#endif





