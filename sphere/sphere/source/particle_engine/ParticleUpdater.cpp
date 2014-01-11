#include "ParticleUpdater.hpp"



////////////////////////////////////////////////////////////////////////////////
ParticleUpdater::ParticleUpdater(const ParticleUpdater& updater)
                : m_GlobalForce(updater.m_GlobalForce)
                , m_DampingConstant(updater.m_DampingConstant)
{
}

////////////////////////////////////////////////////////////////////////////////
void
ParticleUpdater::operator()(Particle& p)
{
    p.Life -= p.Aging;
    p.Vel  += m_GlobalForce;
    p.Vel  -= p.Vel * m_DampingConstant;
    p.Pos  += p.Vel;

}




