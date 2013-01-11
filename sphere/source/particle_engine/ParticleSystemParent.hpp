#ifndef PARTICLE_SYSTEM_PARENT_HPP
#define PARTICLE_SYSTEM_PARENT_HPP


#include <list>
#include <vector>

#include "ParticleSystemBase.hpp"



class ParticleSystemParent : public ParticleSystemBase
{
    private:

        enum DescendantType
        {
            ADOPTED = 0,
            HOSTED
        };

        class Descendant
        {
            public:

                ParticleSystemBase* System;
                DescendantType      Type;

                explicit Descendant(ParticleSystemBase* s, DescendantType t) : System(s)
                                                                             , Type(t)
                {
                }

                Descendant(const Descendant& d) : System(d.System)
                                                , Type(d.Type)
                {
                }

                ~Descendant()
                {
                }

                Descendant& operator=(const Descendant& d)
                {
                    System = d.System;
                    Type   = d.Type;
                    return *this;
                }

        };

        std::list<Descendant> m_Descendants;

    public:

        ParticleSystemParent();

        virtual ~ParticleSystemParent();

        virtual void Update();
        virtual void Render();

        void Adopt(ParticleSystemBase*);
        void Host(ParticleSystemBase*);

        dword Size() const;
        void  Unique();
        void  Apply(ScriptInterface::Applicator appl);
        void  Sort(ScriptInterface::Comparator comp);

        bool ContainsDescendant(dword id);
        bool ContainsDescendantGroup(int group);

        ParticleSystemBase*              GetDescendant(dword id);
        std::vector<ParticleSystemBase*> GetDescendantGroup(int group);

        ParticleSystemBase*              ExtractDescendant(dword id);
        std::vector<ParticleSystemBase*> ExtractDescendantGroup(int group);

        void  RemoveDescendant(dword id);
        void  RemoveDescendantGroup(int group);

        void  Clear();

};

////////////////////////////////////////////////////////////////////////////////
inline
ParticleSystemParent::ParticleSystemParent()
                     : ParticleSystemBase(ParticleSystemBase::PARENT)
{
}

////////////////////////////////////////////////////////////////////////////////
inline
ParticleSystemParent::~ParticleSystemParent()
{
    Clear();
}

////////////////////////////////////////////////////////////////////////////////
inline dword
ParticleSystemParent::Size() const
{
    return m_Descendants.size();
}





#endif








