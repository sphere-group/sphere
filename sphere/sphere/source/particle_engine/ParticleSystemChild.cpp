#include "ParticleSystemChild.hpp"



////////////////////////////////////////////////////////////////////////////////
/*
 * - Copy-constructor.
 */
ParticleSystemChild::ParticleSystemChild(const ParticleSystemChild& system)
                    : ParticleSystemBase(system)
                    , m_Swarm(system.m_Swarm)
                    , m_SwarmRenderer(system.m_SwarmRenderer)
{
}

////////////////////////////////////////////////////////////////////////////////
/*
 * - Increases the swarm size by one (if possible).
 */
void
ParticleSystemChild::Grow()
{
    m_Swarm.resize(m_Swarm.size() + 1);
    m_Initializer(m_Body, m_Swarm.back());
}

////////////////////////////////////////////////////////////////////////////////
/*
 * - Decreases the swarm size by one (if possible).
 */
void
ParticleSystemChild::Shrink()
{
    m_Swarm.resize(m_Swarm.size() - 1);
}

////////////////////////////////////////////////////////////////////////////////
/*
 * - Calls the on_update callback and updates the swarm particles.
 */
void
ParticleSystemChild::Update()
{
    if (IsExtinct() || IsHalted())
        return;

    if (IsDead() && m_Swarm.size() == 0)
    {
        m_Extinct = true;
        return;
    }

    // callback
    if (m_ScriptInterface.HasOnUpdate())
        m_ScriptInterface.OnUpdate();

    // update particles
    for (dword i = 0; i < m_Swarm.size(); ++i)
    {
        // update particle
        m_Updater(m_Swarm[i]);

        // handle death
        if (m_Swarm[i].Life <= 0)
        {
            if (IsCursed() || IsDead())
            {
                m_Swarm[i] = m_Swarm.back();
                m_Swarm.resize(m_Swarm.size() - 1);
            }
            else
            {
                m_Initializer(m_Body, m_Swarm[i]);
            }
        }
    } // end update particles
}

////////////////////////////////////////////////////////////////////////////////
/*
 * - Calls the on_render callback, renders the swarm particles and finally renders itself.
 */
void
ParticleSystemChild::Render()
{
    if (IsExtinct() || IsHidden())
        return;

    // render swarm
    for (dword i = 0; i < m_Swarm.size(); ++i)
        m_SwarmRenderer(m_Swarm[i]);

    // render itself, if alive
    if (!IsDead())
        m_Renderer(m_Body);

    // callback
    if (m_ScriptInterface.HasOnRender())
        m_ScriptInterface.OnRender();

}

////////////////////////////////////////////////////////////////////////////////
/*
 * - Resizes the swarm by the integer n.
 * - If n is negative, the new swarm size will be zero.
 * - If n is bigger than the swarm capacity, the new swarm size will be equal
 *   to the swarm capacity.
 */
void
ParticleSystemChild::Resize(dword n)
{
    dword old_size = m_Swarm.size();
    m_Swarm.resize(n);

    if (m_Swarm.size() > old_size)
        for (dword i = old_size; i < m_Swarm.size(); ++i)
            m_Initializer(m_Body, m_Swarm[i]);

}





