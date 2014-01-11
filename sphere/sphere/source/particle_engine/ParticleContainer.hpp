#ifndef PARTICLE_CONTAINER_HPP
#define PARTICLE_CONTAINER_HPP


#include <algorithm>

#include "../common/types.h"



template<typename T>
class ParticleContainer
{
    private:

        T*    m_Particles;
        dword m_Reserved;
        dword m_Size;

    public:

        ParticleContainer();
        ParticleContainer(const ParticleContainer& container);
        ParticleContainer(dword n);
        ~ParticleContainer();

              T& operator[](dword i);
        const T& operator[](dword i) const;

              T& front();
        const T& front() const;

              T& back();
        const T& back() const;

        bool  empty() const;
        dword size() const;
        dword capacity() const;

        void resize(dword n);
        void reserve(dword n);

        void clear();

};

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline
ParticleContainer<T>::ParticleContainer()
                     : m_Particles(NULL)
                     , m_Reserved(0)
                     , m_Size(0)
{
}

////////////////////////////////////////////////////////////////////////////////
template<typename T>
ParticleContainer<T>::ParticleContainer(const ParticleContainer& container)
                     : m_Particles(NULL)
                     , m_Reserved(container.m_Reserved)
                     , m_Size(container.m_Size)
{
    if (m_Reserved > 0)
    {
        m_Particles = new T[m_Reserved];

        if (m_Particles)
        {
            memcpy(m_Particles, container.m_Particles, sizeof(T) * m_Size);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
template<typename T>
ParticleContainer<T>::ParticleContainer(dword n)
                     : m_Particles(NULL)
                     , m_Reserved(0)
                     , m_Size(0)
{
    if (n > 0)
    {
        m_Particles = new T[n];

        if (m_Particles)
        {
            m_Reserved = n;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline
ParticleContainer<T>::~ParticleContainer()
{
    delete[] m_Particles;
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline T&
ParticleContainer<T>::operator[](dword i)
{
    return m_Particles[i];
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline const T&
ParticleContainer<T>::operator[](dword i) const
{
    return m_Particles[i];
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline T&
ParticleContainer<T>::front()
{
    return m_Particles[0];
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline const T&
ParticleContainer<T>::front() const
{
    return m_Particles[0];
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline T&
ParticleContainer<T>::back()
{
    return m_Particles[m_Size - 1];
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline const T&
ParticleContainer<T>::back() const
{
    return m_Particles[m_Size - 1];
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline bool
ParticleContainer<T>::empty() const
{
    return m_Size == 0;
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline dword
ParticleContainer<T>::size() const
{
    return m_Size;
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline dword
ParticleContainer<T>::capacity() const
{
    return m_Reserved;
}

////////////////////////////////////////////////////////////////////////////////
template<typename T> inline void
ParticleContainer<T>::resize(dword n)
{
    n = std::min(n, m_Reserved);
    m_Size = n;

}

////////////////////////////////////////////////////////////////////////////////
template<typename T> void
ParticleContainer<T>::reserve(dword n)
{
    if (n > m_Reserved)
    {
        T* temp = new T[n];

        if (!temp)
            return;

        memcpy(temp, m_Particles, sizeof(T) * m_Size);
        delete[] m_Particles;
        m_Particles = temp;
        m_Reserved  = n;
    }

}

////////////////////////////////////////////////////////////////////////////////
template<typename T> void
ParticleContainer<T>::clear()
{
    delete[] m_Particles;
    m_Particles = NULL;
    m_Reserved  = 0;
    m_Size      = 0;

}





#endif








