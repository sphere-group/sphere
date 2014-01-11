#include "SpritesetServer.hpp"
////////////////////////////////////////////////////////////////////////////////
CSpritesetServer::CSpritesetServer()
{}

////////////////////////////////////////////////////////////////////////////////
CSpritesetServer::~CSpritesetServer()
{

    for (unsigned int i = 0; i < m_Spritesets.size(); i++)
    {

        m_Spritesets[i].spriteset->Release();
    }
}

////////////////////////////////////////////////////////////////////////////////
SSPRITESET*
CSpritesetServer::Load(const char* filename, IFileSystem& fs)
{
    // if a spriteset is already loaded...
    for (unsigned int i = 0; i < m_Spritesets.size(); i++)
        if (filename == m_Spritesets[i].name)
        {
            m_Spritesets[i].refcount++;
            return m_Spritesets[i].spriteset;
        }

    // we have to load a new one...
    Spriteset spriteset;
    spriteset.name      = filename;
    spriteset.refcount  = 1;
    spriteset.spriteset = new SSPRITESET;
    m_Spritesets.push_back(spriteset);

    if (!spriteset.spriteset->Load(filename, fs, std::string(filename)))
    {
        spriteset.spriteset->Release();
        m_Spritesets.pop_back();
        return NULL;
    }

    return spriteset.spriteset;
}

////////////////////////////////////////////////////////////////////////////////
void
CSpritesetServer::Free(SSPRITESET* spriteset)
{

    bool found = false;
    for (unsigned int i = 0; i < m_Spritesets.size(); i++)
    {
        if (spriteset == m_Spritesets[i].spriteset)
        {

            found = true;
            if (--m_Spritesets[i].refcount == 0)
            {
                // remove m_Spritesets[i]
                m_Spritesets[i].spriteset->Release();

                m_Spritesets.erase(m_Spritesets.begin() + i);
                return;
            }

        }
    }
    // the spriteset server does not own this spriteset
    if (!found)
    {

        spriteset->Release();
    }
}
////////////////////////////////////////////////////////////////////////////////
