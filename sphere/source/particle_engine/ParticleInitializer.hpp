#ifndef PARTICLE_INITIALIZER_HPP
#define PARTICLE_INITIALIZER_HPP


#include "RectangularRange.hpp"
#include "EllipticalRange.hpp"
#include "Particle.hpp"



class ParticleInitializer
{
    public:

        enum PositionMode
        {
            NULL_SHAPE = 0,
            RECTANGULAR_SHAPE,
            ELLIPTICAL_SHAPE
        };

        enum VelocityMode
        {
            EXPLICIT_ORIENTATION = 0,
            IMPLICIT_ORIENTATION,
        };

    private:

        PositionMode m_PositionMode;
        VelocityMode m_VelocityMode;

        void (ParticleInitializer::*m_PosInit)(const Particle& parent, Particle& p);
        void (ParticleInitializer::*m_VelInit)(const Particle& parent, Particle& p);

        Range<float>            m_Aging;
        RectangularRange<float> m_PosRectangle;
        EllipticalRange<float>  m_PosEllipse;
        EllipticalRange<float>  m_VelEllipse;

    public:

        ParticleInitializer();
        ParticleInitializer(const ParticleInitializer& initializer);
        ~ParticleInitializer();

        void operator()(const Particle& parent, Particle& p);

        void NullPosInit(const Particle& parent, Particle& p);
        void RectangularPosInit(const Particle& parent, Particle& p);
        void EllipticalPosInit(const Particle& parent, Particle& p);

        void ImplicitVelInit(const Particle& parent, Particle& p);
        void ExplicitVelInit(const Particle& parent, Particle& p);

        PositionMode GetPositionMode() const;
        void         SetPositionMode(PositionMode mode);
        void         SetPositionMode(int mode);

        VelocityMode GetVelocityMode() const;
        void         SetVelocityMode(VelocityMode mode);
        void         SetVelocityMode(int mode);

              Range<float>& GetAging();
        const Range<float>& GetAging() const;

              RectangularRange<float>& GetPosRectangle();
        const RectangularRange<float>& GetPosRectangle() const;

              EllipticalRange<float>&  GetPosEllipse();
        const EllipticalRange<float>&  GetPosEllipse() const;

              EllipticalRange<float>&  GetVelEllipse();
        const EllipticalRange<float>&  GetVelEllipse() const;

};

////////////////////////////////////////////////////////////////////////////////
inline
ParticleInitializer::~ParticleInitializer()
{
}

////////////////////////////////////////////////////////////////////////////////
inline ParticleInitializer::PositionMode
ParticleInitializer::GetPositionMode() const
{
    return m_PositionMode;
}

////////////////////////////////////////////////////////////////////////////////
inline ParticleInitializer::VelocityMode
ParticleInitializer::GetVelocityMode() const
{
    return m_VelocityMode;
}

////////////////////////////////////////////////////////////////////////////////
inline Range<float>&
ParticleInitializer::GetAging()
{
    return m_Aging;
}

////////////////////////////////////////////////////////////////////////////////
inline const Range<float>&
ParticleInitializer::GetAging() const
{
    return m_Aging;
}

////////////////////////////////////////////////////////////////////////////////
inline RectangularRange<float>&
ParticleInitializer::GetPosRectangle()
{
    return m_PosRectangle;
}

////////////////////////////////////////////////////////////////////////////////
inline const RectangularRange<float>&
ParticleInitializer::GetPosRectangle() const
{
    return m_PosRectangle;
}

////////////////////////////////////////////////////////////////////////////////
inline EllipticalRange<float>&
ParticleInitializer::GetPosEllipse()
{
    return m_PosEllipse;
}

////////////////////////////////////////////////////////////////////////////////
inline const EllipticalRange<float>&
ParticleInitializer::GetPosEllipse() const
{
    return m_PosEllipse;
}

////////////////////////////////////////////////////////////////////////////////
inline EllipticalRange<float>&
ParticleInitializer::GetVelEllipse()
{
    return m_VelEllipse;
}

////////////////////////////////////////////////////////////////////////////////
inline const EllipticalRange<float>&
ParticleInitializer::GetVelEllipse() const
{
    return m_VelEllipse;
}





#endif





