#ifndef PARTICLE_SYSTEM_BASE_HPP
#define PARTICLE_SYSTEM_BASE_HPP


#include "RefCounted.hpp"
#include "Particle.hpp"
#include "ParticleInitializer.hpp"
#include "ParticleUpdater.hpp"
#include "ParticleRenderer.hpp"
#include "ScriptInterface.hpp"



class ParticleSystemBase : public RefCounted
{
    public:

        enum SystemType
        {
            PARENT = 0,
            CHILD
        };

    private:

        static dword s_NextUniqueID; // defines the next unique id

    protected:

        SystemType m_Type;

        dword m_ID;      // system-defined unique id
        int   m_Group;   // user-defined group id

        bool  m_Halted;  // doesn't update
        bool  m_Hidden;  // doesn't render
        bool  m_Cursed;  // doesn't revive or accept descendants
        bool  m_Dead;    // doesn't exist
        bool  m_Extinct; // flag for parents that its ready to be removed or revived

        Particle            m_Body;
        ParticleInitializer m_Initializer;
        ParticleUpdater     m_Updater;
        ParticleRenderer    m_Renderer;

        ScriptInterface     m_ScriptInterface;

    public:

        explicit ParticleSystemBase(SystemType type);
        ParticleSystemBase(const ParticleSystemBase& system);

        virtual ~ParticleSystemBase();

        virtual void Update() = 0;
        virtual void Render() = 0;

        bool Borrow();
        void Release();

        SystemType GetType() const;

        dword GetID() const;

        int  GetGroup() const;
        void SetGroup(int group);

        bool IsHalted() const;
        void Halt(bool halt);

        bool IsHidden() const;
        void Hide(bool hide);

        bool IsCursed() const;
        void Curse(bool curse);

        bool IsDead() const;
        bool IsExtinct() const;
        void Kill(ParticleSystemBase* parent = NULL);
        void Revive(ParticleSystemBase* parent = NULL);

              Particle&            GetBody();
        const Particle&            GetBody() const;

              ParticleInitializer& GetInitializer();
        const ParticleInitializer& GetInitializer() const;

              ParticleUpdater&     GetUpdater();
        const ParticleUpdater&     GetUpdater() const;

              ParticleRenderer&    GetRenderer();
        const ParticleRenderer&    GetRenderer() const;

        ScriptInterface& GetScriptInterface();

};

////////////////////////////////////////////////////////////////////////////////
inline
ParticleSystemBase::ParticleSystemBase(SystemType type)
                   : m_Type(type)
                   , m_ID(s_NextUniqueID++)
                   , m_Group(0)
                   , m_Halted(false)
                   , m_Hidden(false)
                   , m_Cursed(false)
                   , m_Dead(false)
                   , m_Extinct(false)
{
}

////////////////////////////////////////////////////////////////////////////////
inline
ParticleSystemBase::~ParticleSystemBase()
{
}

////////////////////////////////////////////////////////////////////////////////
inline ParticleSystemBase::SystemType
ParticleSystemBase::GetType() const
{
    return m_Type;
}

////////////////////////////////////////////////////////////////////////////////
inline dword
ParticleSystemBase::GetID() const
{
    return m_ID;
}

////////////////////////////////////////////////////////////////////////////////
inline int
ParticleSystemBase::GetGroup() const
{
    return m_Group;
}

////////////////////////////////////////////////////////////////////////////////
inline void
ParticleSystemBase::SetGroup(int group)
{
    m_Group = group;
}

////////////////////////////////////////////////////////////////////////////////
inline bool
ParticleSystemBase::IsHalted() const
{
    return m_Halted;
}

////////////////////////////////////////////////////////////////////////////////
inline void
ParticleSystemBase::Halt(bool halt)
{
    m_Halted = halt;
}

////////////////////////////////////////////////////////////////////////////////
inline bool
ParticleSystemBase::IsHidden() const
{
    return m_Hidden;
}

////////////////////////////////////////////////////////////////////////////////
inline void
ParticleSystemBase::Hide(bool hide)
{
    m_Hidden = hide;
}

////////////////////////////////////////////////////////////////////////////////
inline bool
ParticleSystemBase::IsCursed() const
{
    return m_Cursed;
}

////////////////////////////////////////////////////////////////////////////////
inline void
ParticleSystemBase::Curse(bool curse)
{
    m_Cursed = curse;
}

////////////////////////////////////////////////////////////////////////////////
inline bool
ParticleSystemBase::IsDead() const
{
    return m_Dead;
}

////////////////////////////////////////////////////////////////////////////////
inline bool
ParticleSystemBase::IsExtinct() const
{
    return m_Extinct;
}

////////////////////////////////////////////////////////////////////////////////
inline Particle&
ParticleSystemBase::GetBody()
{
    return m_Body;
}

////////////////////////////////////////////////////////////////////////////////
inline const Particle&
ParticleSystemBase::GetBody() const
{
    return m_Body;
}

////////////////////////////////////////////////////////////////////////////////
inline ParticleInitializer&
ParticleSystemBase::GetInitializer()
{
    return m_Initializer;
}

////////////////////////////////////////////////////////////////////////////////
inline const ParticleInitializer&
ParticleSystemBase::GetInitializer() const
{
    return m_Initializer;
}

////////////////////////////////////////////////////////////////////////////////
inline ParticleUpdater&
ParticleSystemBase::GetUpdater()
{
    return m_Updater;
}

////////////////////////////////////////////////////////////////////////////////
inline const ParticleUpdater&
ParticleSystemBase::GetUpdater() const
{
    return m_Updater;
}

////////////////////////////////////////////////////////////////////////////////
inline ParticleRenderer&
ParticleSystemBase::GetRenderer()
{
    return m_Renderer;
}

////////////////////////////////////////////////////////////////////////////////
inline const ParticleRenderer&
ParticleSystemBase::GetRenderer() const
{
    return m_Renderer;
}

////////////////////////////////////////////////////////////////////////////////
inline ScriptInterface&
ParticleSystemBase::GetScriptInterface()
{
    return m_ScriptInterface;
}




#endif






