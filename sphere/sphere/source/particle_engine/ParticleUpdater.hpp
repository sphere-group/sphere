#ifndef PARTICLE_UPDATER_HPP
#define PARTICLE_UPDATER_HPP


#include "Particle.hpp"



class ParticleUpdater
{
    private:

        Vector2<float> m_GlobalForce;
        float          m_DampingConstant;

    public:

        ParticleUpdater();
        ParticleUpdater(const ParticleUpdater& updater);
        ~ParticleUpdater();

        void operator()(Particle& p);

              Vector2<float>& GetGlobalForce();
        const Vector2<float>& GetGlobalForce() const;

        float GetDampingConstant() const;
        void  SetDampingConstant(float damp);

};

////////////////////////////////////////////////////////////////////////////////
inline
ParticleUpdater::ParticleUpdater()
                : m_DampingConstant(0)
{
}

////////////////////////////////////////////////////////////////////////////////
inline
ParticleUpdater::~ParticleUpdater()
{
}

////////////////////////////////////////////////////////////////////////////////
inline Vector2<float>&
ParticleUpdater::GetGlobalForce()
{
    return m_GlobalForce;
}

////////////////////////////////////////////////////////////////////////////////
inline const Vector2<float>&
ParticleUpdater::GetGlobalForce() const
{
    return m_GlobalForce;
}

////////////////////////////////////////////////////////////////////////////////
inline float
ParticleUpdater::GetDampingConstant() const
{
    return m_DampingConstant;
}

////////////////////////////////////////////////////////////////////////////////
inline void
ParticleUpdater::SetDampingConstant(float damp)
{
    m_DampingConstant = damp;
}





#endif





