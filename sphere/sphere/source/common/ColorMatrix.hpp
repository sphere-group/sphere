#ifndef COLORMATRIX_HPP
#define COLORMATRIX_HPP

class CColorMatrix
{
public:
    int rn, rr, rg, rb;
    int gn, gr, gg, gb;
    int bn, br, bg, bb;
    CColorMatrix();
    CColorMatrix(int rn, int rr, int rg, int rb, int gn, int gr, int gg, int gb, int bn, int br, int bg, int bb);
};
inline
CColorMatrix::CColorMatrix()
{
}

inline
CColorMatrix::CColorMatrix(int vrn, int vrr, int vrg, int vrb, int vgn, int vgr, int vgg, int vgb, int vbn, int vbr, int vbg, int vbb)
        : rn(vrn), rr(vrr), rg(vrg), rb(vrb)
        , gn(vgn), gr(vgr), gg(vgg), gb(vgb)
        , bn(vbn), br(vbr), bg(vbg), bb(vbb)
{
}

#endif
