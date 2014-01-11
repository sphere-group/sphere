#ifndef SSPRITESET_HPP
#define SSPRITESET_HPP

#include "video.hpp"
#include "../common/Spriteset.hpp"
#include "../common/IFileSystem.hpp"

class SSPRITESET
{
public:
    SSPRITESET();

    SSPRITESET(const sSpriteset& s);
protected:  // use Release()
    ~SSPRITESET();

public:
    void AddRef();
    void Release();

    bool Load(const char* filename, IFileSystem& fs, std::string pfilename);
    std::string GetFilename() const;
    const sSpriteset& GetSpriteset() const;

    void GetBase(int& x1, int& y1, int& x2, int& y2) const;
    void GetRealBase(int& x1, int& y1, int& x2, int& y2) const;
    void SetBase(int x1, int y1, int x2, int y2);
	void Base2Real() { m_Spriteset.Base2Real(); };


    int GetNumImages() const;
    IMAGE GetImage(int i) const;
    IMAGE GetFlipImage(int i) const;

private:
    bool Create();
    void Destroy();
private:
    IMAGE CreateFrameImage(int image);
    IMAGE CreateFlipFrameImage(int image);

private:
    sSpriteset m_Spriteset;
    IMAGE* m_Images;
    IMAGE* m_FlipImages;

    int m_RefCount;
    std::string m_Filename;
};
#endif
