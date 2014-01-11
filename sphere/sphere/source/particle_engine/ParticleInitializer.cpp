#include "ParticleInitializer.hpp"



////////////////////////////////////////////////////////////////////////////////
ParticleInitializer::ParticleInitializer()
                    : m_PositionMode(ParticleInitializer::NULL_SHAPE)
                    , m_VelocityMode(ParticleInitializer::EXPLICIT_ORIENTATION)
                    , m_PosInit(&ParticleInitializer::NullPosInit)
                    , m_VelInit(&ParticleInitializer::ExplicitVelInit)
{
}

////////////////////////////////////////////////////////////////////////////////
ParticleInitializer::ParticleInitializer(const ParticleInitializer& initializer)
                    : m_PositionMode(initializer.m_PositionMode)
                    , m_VelocityMode(initializer.m_VelocityMode)
                    , m_PosInit(initializer.m_PosInit)
                    , m_VelInit(initializer.m_VelInit)
                    , m_Aging(initializer.m_Aging)
                    , m_PosRectangle(initializer.m_PosRectangle)
                    , m_PosEllipse(initializer.m_PosEllipse)
                    , m_VelEllipse(initializer.m_VelEllipse)
{
}

////////////////////////////////////////////////////////////////////////////////
void
ParticleInitializer::operator()(const Particle& parent, Particle& p)
{
    p.Life  = 1;
    p.Aging = m_Aging.Random();

    (this->*m_PosInit)(parent, p);
    (this->*m_VelInit)(parent, p);
}

////////////////////////////////////////////////////////////////////////////////
void
ParticleInitializer::NullPosInit(const Particle& parent, Particle& p)
{
    p.Pos = parent.Pos;
}

////////////////////////////////////////////////////////////////////////////////
void
ParticleInitializer::RectangularPosInit(const Particle& parent, Particle& p)
{
    p.Pos = parent.Pos + m_PosRectangle.Random();
}

////////////////////////////////////////////////////////////////////////////////
void
ParticleInitializer::EllipticalPosInit(const Particle& parent, Particle& p)
{
    p.Pos = parent.Pos + m_PosEllipse.Random();
}

////////////////////////////////////////////////////////////////////////////////
void
ParticleInitializer::ExplicitVelInit(const Particle& parent, Particle& p)
{
    p.Vel = m_VelEllipse.Random();
}

////////////////////////////////////////////////////////////////////////////////
void
ParticleInitializer::ImplicitVelInit(const Particle& parent, Particle& p)
{
    p.Vel = m_VelEllipse.Random(p.Pos - parent.Pos);
}

////////////////////////////////////////////////////////////////////////////////
void
ParticleInitializer::SetPositionMode(ParticleInitializer::PositionMode mode)
{
    switch (mode)
    {
        case ParticleInitializer::NULL_SHAPE:
            m_PosInit = &ParticleInitializer::NullPosInit;        break;
        case ParticleInitializer::RECTANGULAR_SHAPE:
            m_PosInit = &ParticleInitializer::RectangularPosInit; break;
        case ParticleInitializer::ELLIPTICAL_SHAPE:
            m_PosInit = &ParticleInitializer::EllipticalPosInit;  break;
    }

    m_PositionMode = mode;

}

////////////////////////////////////////////////////////////////////////////////
void
ParticleInitializer::SetPositionMode(int mode)
{
    switch (mode)
    {
        case ParticleInitializer::NULL_SHAPE:
            SetPositionMode(ParticleInitializer::NULL_SHAPE);        break;
        case ParticleInitializer::RECTANGULAR_SHAPE:
            SetPositionMode(ParticleInitializer::RECTANGULAR_SHAPE); break;
        case ParticleInitializer::ELLIPTICAL_SHAPE:
            SetPositionMode(ParticleInitializer::ELLIPTICAL_SHAPE);  break;
    }

}

////////////////////////////////////////////////////////////////////////////////
void
ParticleInitializer::SetVelocityMode(ParticleInitializer::VelocityMode mode)
{
    switch (mode)
    {
        case ParticleInitializer::EXPLICIT_ORIENTATION:
            m_VelInit = &ParticleInitializer::ExplicitVelInit; break;
        case ParticleInitializer::IMPLICIT_ORIENTATION:
            m_VelInit = &ParticleInitializer::ImplicitVelInit; break;
    }

    m_VelocityMode = mode;

}

////////////////////////////////////////////////////////////////////////////////
void
ParticleInitializer::SetVelocityMode(int mode)
{
    switch (mode)
    {
        case ParticleInitializer::EXPLICIT_ORIENTATION:
            SetVelocityMode(ParticleInitializer::EXPLICIT_ORIENTATION); break;
        case ParticleInitializer::IMPLICIT_ORIENTATION:
            SetVelocityMode(ParticleInitializer::IMPLICIT_ORIENTATION); break;
    }

}






