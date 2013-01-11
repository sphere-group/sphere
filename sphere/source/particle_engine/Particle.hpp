#ifndef PARTICLE_HPP
#define PARTICLE_HPP


#include "Vector2.hpp"



class Particle
{
    public:

        Vector2<float> Pos;
        Vector2<float> Vel;
        float          Life;
        float          Aging;

        Particle();
        Particle(const Particle& p);
        ~Particle();

        Particle& operator=(const Particle& par);

};

////////////////////////////////////////////////////////////////////////////////
inline
Particle::Particle()
         : Life(1)
         , Aging(0)
{
}

////////////////////////////////////////////////////////////////////////////////
inline
Particle::Particle(const Particle& p)
         : Pos(p.Pos)
         , Vel(p.Vel)
         , Life(p.Life)
         , Aging(p.Aging)
{
}

////////////////////////////////////////////////////////////////////////////////
inline
Particle::~Particle()
{
}

////////////////////////////////////////////////////////////////////////////////
inline Particle&
Particle::operator=(const Particle& p)
{
    Pos   = p.Pos;
    Vel   = p.Vel;
    Life  = p.Life;
    Aging = p.Aging;
    return *this;

}





#endif





