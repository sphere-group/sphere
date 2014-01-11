#ifndef PARTICLE_RENDERER_HPP
#define PARTICLE_RENDERER_HPP


#include "../common/Image32.hpp"
#include "../engine/video.hpp"

#include "Particle.hpp"



class ParticleRenderer
{
    private:

        IMAGE               m_Texture;
        CImage32::BlendMode m_BlendMode;
        int                 m_OffsetX;
        int                 m_OffsetY;
        bool                m_Disabled;

    public:

        ParticleRenderer();
        ParticleRenderer(const ParticleRenderer& renderer);
        ~ParticleRenderer();

        void operator()(const Particle& p) const;

        IMAGE GetTexture() const;
        void  SetTexture(IMAGE texture);

        CImage32::BlendMode GetBlendMode() const;
        void                SetBlendMode(CImage32::BlendMode blendmode);
        void                SetBlendMode(int blendmode);

        int  GetOffsetX() const;
        void SetOffsetX(int off_x);

        int  GetOffsetY() const;
        void SetOffsetY(int off_y);

        bool IsDisabled() const;
        void Disable(bool disable);

};

////////////////////////////////////////////////////////////////////////////////
inline
ParticleRenderer::ParticleRenderer()
                 : m_Texture(NULL)
                 , m_BlendMode(CImage32::BLEND)
                 , m_OffsetX(0)
                 , m_OffsetY(0)
                 , m_Disabled(false)
{
}

////////////////////////////////////////////////////////////////////////////////
inline
ParticleRenderer::~ParticleRenderer()
{
    // texture is owned by a script object, which will destroy it automatically
    m_Texture = NULL;
}

////////////////////////////////////////////////////////////////////////////////
inline IMAGE
ParticleRenderer::GetTexture() const
{
    return m_Texture;
}

////////////////////////////////////////////////////////////////////////////////
inline void
ParticleRenderer::SetTexture(IMAGE image)
{
    m_Texture = image;
}

////////////////////////////////////////////////////////////////////////////////
inline CImage32::BlendMode
ParticleRenderer::GetBlendMode() const
{
    return m_BlendMode;
}

////////////////////////////////////////////////////////////////////////////////
inline int
ParticleRenderer::GetOffsetX() const
{
    return m_OffsetX;
}

////////////////////////////////////////////////////////////////////////////////
inline void
ParticleRenderer::SetOffsetX(int off_x)
{
    m_OffsetX = off_x;
}

////////////////////////////////////////////////////////////////////////////////
inline int
ParticleRenderer::GetOffsetY() const
{
    return m_OffsetY;
}

////////////////////////////////////////////////////////////////////////////////
inline void
ParticleRenderer::SetOffsetY(int off_y)
{
    m_OffsetY = off_y;
}

////////////////////////////////////////////////////////////////////////////////
inline bool
ParticleRenderer::IsDisabled() const
{
    return m_Disabled;
}

////////////////////////////////////////////////////////////////////////////////
inline void
ParticleRenderer::Disable(bool disable)
{
    m_Disabled = disable;
}




#endif





