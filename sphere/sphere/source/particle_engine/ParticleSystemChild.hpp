#ifndef PARTICLE_SYSTEM_HPP
#define PARTICLE_SYSTEM_HPP


#include "ParticleSystemBase.hpp"
#include "Particle.hpp"
#include "ParticleContainer.hpp"



class ParticleSystemChild : public ParticleSystemBase
{
    private:

        ParticleContainer<Particle> m_Swarm;
        ParticleRenderer            m_SwarmRenderer;

    public:

        ParticleSystemChild();
        ParticleSystemChild(dword size);
        ParticleSystemChild(const ParticleSystemChild& system);

        virtual ~ParticleSystemChild();

        virtual void Update();
        virtual void Render();

        dword Size() const;
        dword Capacity() const;

        void Grow();
        void Shrink();

        void Resize(dword n);
        void Reserve(dword n);

        void Clear();

              ParticleRenderer& GetSwarmRenderer();
        const ParticleRenderer& GetSwarmRenderer() const;

};

////////////////////////////////////////////////////////////////////////////////
inline
ParticleSystemChild::ParticleSystemChild()
                    : ParticleSystemBase(ParticleSystemBase::CHILD)
{
}

////////////////////////////////////////////////////////////////////////////////
inline
ParticleSystemChild::ParticleSystemChild(dword size)
                    : ParticleSystemBase(ParticleSystemBase::CHILD)
                    , m_Swarm(size)
{
}

////////////////////////////////////////////////////////////////////////////////
inline
ParticleSystemChild::~ParticleSystemChild()
{
    m_Swarm.clear();
}

////////////////////////////////////////////////////////////////////////////////
inline dword
ParticleSystemChild::Size() const
{
    return m_Swarm.size();
}

////////////////////////////////////////////////////////////////////////////////
inline dword
ParticleSystemChild::Capacity() const
{
    return m_Swarm.capacity();
}

////////////////////////////////////////////////////////////////////////////////
inline void
ParticleSystemChild::Reserve(dword n)
{
    m_Swarm.reserve(n);

}

////////////////////////////////////////////////////////////////////////////////
inline void
ParticleSystemChild::Clear()
{
    m_Swarm.clear();

}

////////////////////////////////////////////////////////////////////////////////
inline ParticleRenderer&
ParticleSystemChild::GetSwarmRenderer()
{
    return m_SwarmRenderer;
}

////////////////////////////////////////////////////////////////////////////////
inline const ParticleRenderer&
ParticleSystemChild::GetSwarmRenderer() const
{
    return m_SwarmRenderer;
}




#endif






