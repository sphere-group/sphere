#include "ParticleSystemBase.hpp"



////////////////////////////////////////////////////////////////////////////////
dword
ParticleSystemBase::s_NextUniqueID = 0;

////////////////////////////////////////////////////////////////////////////////
ParticleSystemBase::ParticleSystemBase(const ParticleSystemBase& system)
                   : m_Type(system.m_Type)
                   , m_ID(s_NextUniqueID++)
                   , m_Group(system.m_Group)
                   , m_Halted(system.m_Halted)
                   , m_Hidden(system.m_Hidden)
                   , m_Cursed(system.m_Cursed)
                   , m_Dead(system.m_Dead)
                   , m_Extinct(system.m_Extinct)
                   , m_Body(system.m_Body)
                   , m_Initializer(system.m_Initializer)
                   , m_Updater(system.m_Updater)
                   , m_Renderer(system.m_Renderer)
                   , m_ScriptInterface(system.m_ScriptInterface)

{
}

////////////////////////////////////////////////////////////////////////////////
bool
ParticleSystemBase::Borrow()
{
    if (!m_ScriptInterface.IsProtected() && !m_ScriptInterface.StartProtection())
        return false;

    IncRef();

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
ParticleSystemBase::Release()
{
    DecRef();

    if (m_ScriptInterface.IsProtected() && RefCount() == 0)
        m_ScriptInterface.EndProtection();

}

////////////////////////////////////////////////////////////////////////////////
void
ParticleSystemBase::Kill(ParticleSystemBase* parent)
{
    m_Dead = true;

    if (m_ScriptInterface.HasOnDeath())
        m_ScriptInterface.OnDeath(parent);

}

////////////////////////////////////////////////////////////////////////////////
void
ParticleSystemBase::Revive(ParticleSystemBase* parent)
{
    m_Dead    = false;
    m_Extinct = false;

    if (m_ScriptInterface.HasOnBirth())
        m_ScriptInterface.OnBirth(parent);

}




