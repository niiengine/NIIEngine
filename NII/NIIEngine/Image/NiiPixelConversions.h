/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/

/* 内部包含文件 -- 不使用外部 */
#ifndef PIXELCONVERSIONS_H_
#define PIXELCONVERSIONS_H_

#if N_COMPILER != N_CPP_MSVC || N_COMPILER_VER >= 1300

#define FMTCONVERTERID(from, to) (((from)<<8)|(to))
/**

 * 自动选择一个格式盒子像素到另一种格式的代码
 * 如果我们需要C + +模板和设计模式方案则生成
 *
 * @param[in] DST Policy class to facilitate pixel-to-pixel conversion. This class
 *    has at least two typedefs: SrcType and DstType. SrcType is the source element type,
 *    dstType is the destination element type. It also has a static method, pixelConvert, that
 *    converts a srcType into a dstType.
 */

template <class U> struct PixelBoxConverter
{
    static const NII::NIIi ID = U::ID;
    static void conversion(const NII::PixelBlock & src, const NII::PixelBlock & dst)
    {
        typename U::SrcType * srcptr = (typename U::SrcType *)(src.mData
            + src.mLeft * mUnitSize + src.mTop * src.mRowSize + src.mFront * src.mSliceSize);
        typename U::DstType * dstptr = (typename U::DstType *)(dst.mData
            + dst.mLeft * mUnitSize + dst.mTop * dst.mRowSize + dst.mFront * dst.mSliceSize);
        NII::NCount srcSliceSkip = src.getSliceOft() / src.mUnitSize;
        NII::NCount dstSliceSkip = dst.getSliceOft() / dst.mUnitSize;
        NII::NCount srcRowSkip = src.mRowSize / src.mUnitSize;
        NII::NCount dstRowSkip = dst.mRowSize / dst.mUnitSize;
        const NII::NCount k = src.mRight - src.mLeft;
        for(NII::NCount z = src.mFront; z < src.mBack; ++z)
        {
            for(NII::NCount y = src.mTop; y < src.mBottom; ++y)
            {
                for(NII::NCount x = 0; x < k; ++x)
                {
                    dstptr[x] = U::pixelConvert(srcptr[x]);
                }
                srcptr += srcRowSkip;
                dstptr += dstRowSkip;
            }
            srcptr += srcSliceSkip;
            dstptr += dstSliceSkip;
        }
    }
};

template <typename SRC, typename DST, NII::NIIi id> struct PixelConverter
{
    static const NII::NIIi ID = id;
    typedef SRC SrcType;
    typedef DST DstType;
    
    //inline static DstType pixelConvert(const SrcType &inp);
};

/** Type for PF_R8G8B8UN/PF_B8G8R8UN
*/
struct Col3b
{
    Col3b(NII::Nui32 a, NII::Nui32 b, NII::Nui32 c) : x((NII::Nui8)a), y((NII::Nui8)b), z((NII::Nui8)c) {}
    NII::Nui8 x,y,z;
};

/** Type for PF_R32G32B32
*/
struct Col3f
{
    Col3f(NII::NIIf r, NII::NIIf g, NII::NIIf b) : r(r), g(g), b(b) {}
    NII::NIIf r,g,b;
};

/** Type for PF_R32G32B32A32
*/
struct Col4f {
    Col4f(NII::NIIf r, NII::NIIf g, NII::NIIf b, NII::NIIf a):
        r(r), g(g), b(b), a(a) { }
    NII::NIIf r,g,b,a;
};

struct A8R8G8B8toA8B8G8R8: public PixelConverter <NII::Nui32, NII::Nui32, FMTCONVERTERID(NII::PF_A8R8G8B8, NII::PF_A8B8G8R8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<16)|(inp&0xFF00FF00)|((inp&0x00FF0000)>>16);
    }
};

struct A8R8G8B8toB8G8R8A8: public PixelConverter <NII::Nui32, NII::Nui32, FMTCONVERTERID(NII::PF_A8R8G8B8, NII::PF_B8G8R8A8UN)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<24)|((inp&0x0000FF00)<<8)|((inp&0x00FF0000)>>8)|((inp&0xFF000000)>>24);
    }
};

struct A8R8G8B8toR8G8B8A8: public PixelConverter <NII::Nui32, NII::Nui32, FMTCONVERTERID(NII::PF_A8R8G8B8, NII::PF_R8G8B8A8ui)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x00FFFFFF)<<8)|((inp&0xFF000000)>>24);
    }
};

struct A8B8G8R8toA8R8G8B8: public PixelConverter <NII::Nui32, NII::Nui32, FMTCONVERTERID(NII::PF_A8B8G8R8, NII::PF_A8R8G8B8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<16)|(inp&0xFF00FF00)|((inp&0x00FF0000)>>16);
    }
};

struct A8B8G8R8toB8G8R8A8: public PixelConverter <NII::Nui32, NII::Nui32, FMTCONVERTERID(NII::PF_A8B8G8R8, NII::PF_B8G8R8A8UN)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x00FFFFFF)<<8)|((inp&0xFF000000)>>24);
    }
};

struct A8B8G8R8toR8G8B8A8: public PixelConverter <NII::Nui32, NII::Nui32, FMTCONVERTERID(NII::PF_A8B8G8R8, NII::PF_R8G8B8A8ui)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<24)|((inp&0x0000FF00)<<8)|((inp&0x00FF0000)>>8)|((inp&0xFF000000)>>24);
    }
};

struct B8G8R8A8toA8R8G8B8: public PixelConverter <NII::Nui32, NII::Nui32, FMTCONVERTERID(NII::PF_B8G8R8A8UN, NII::PF_A8R8G8B8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<24)|((inp&0x0000FF00)<<8)|((inp&0x00FF0000)>>8)|((inp&0xFF000000)>>24);
    }
};

struct B8G8R8A8toA8B8G8R8: public PixelConverter <NII::Nui32, NII::Nui32, FMTCONVERTERID(NII::PF_B8G8R8A8UN, NII::PF_A8B8G8R8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<24)|((inp&0xFFFFFF00)>>8);
    }
};

struct B8G8R8A8toR8G8B8A8: public PixelConverter <NII::Nui32, NII::Nui32, FMTCONVERTERID(NII::PF_B8G8R8A8UN, NII::PF_R8G8B8A8ui)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x0000FF00)<<16)|(inp&0x00FF00FF)|((inp&0xFF000000)>>16);
    }
};

struct R8G8B8A8toA8R8G8B8: public PixelConverter <NII::Nui32, NII::Nui32, FMTCONVERTERID(NII::PF_R8G8B8A8ui, NII::PF_A8R8G8B8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<24)|((inp&0xFFFFFF00)>>8);
    }
};

struct R8G8B8A8toA8B8G8R8: public PixelConverter <NII::Nui32, NII::Nui32, FMTCONVERTERID(NII::PF_R8G8B8A8ui, NII::PF_A8B8G8R8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<24)|((inp&0x0000FF00)<<8)|((inp&0x00FF0000)>>8)|((inp&0xFF000000)>>24);
    }
};

struct R8G8B8A8toB8G8R8A8: public PixelConverter <NII::Nui32, NII::Nui32, FMTCONVERTERID(NII::PF_R8G8B8A8ui, NII::PF_B8G8R8A8UN)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x0000FF00)<<16)|(inp&0x00FF00FF)|((inp&0xFF000000)>>16);
    }
};

struct A8B8G8R8toR8: public PixelConverter <NII::Nui32, NII::Nui8, FMTCONVERTERID(NII::PF_A8B8G8R8, NII::PF_R8UN)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return (NII::Nui8)(inp&0x000000FF);
    }
};

struct R8toA8B8G8R8: public PixelConverter <NII::Nui8, NII::Nui32, FMTCONVERTERID(NII::PF_R8UN, NII::PF_A8B8G8R8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return 0xFF000000|((unsigned int)inp);
    }
};

struct A8R8G8B8toR8: public PixelConverter <NII::Nui32, NII::Nui8, FMTCONVERTERID(NII::PF_A8R8G8B8, NII::PF_R8UN)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return (NII::Nui8)((inp&0x00FF0000)>>16);
    }
};

struct R8toA8R8G8B8: public PixelConverter <NII::Nui8, NII::Nui32, FMTCONVERTERID(NII::PF_R8UN, NII::PF_A8R8G8B8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return 0xFF000000|(((unsigned int)inp)<<16);
    }
};

struct B8G8R8A8toR8: public PixelConverter <NII::Nui32, NII::Nui8, FMTCONVERTERID(NII::PF_B8G8R8A8UN, NII::PF_R8UN)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return (NII::Nui8)((inp&0x0000FF00)>>8);
    }
};

struct R8toB8G8R8A8: public PixelConverter <NII::Nui8, NII::Nui32, FMTCONVERTERID(NII::PF_R8UN, NII::PF_B8G8R8A8UN)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return 0x000000FF|(((unsigned int)inp)<<8);
    }
};

struct A8B8G8R8toL8: public PixelConverter <NII::Nui32, NII::Nui8, FMTCONVERTERID(NII::PF_A8B8G8R8, NII::PF_L8ui)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return (NII::Nui8)(inp&0x000000FF);
    }
};

struct L8toA8B8G8R8: public PixelConverter <NII::Nui8, NII::Nui32, FMTCONVERTERID(NII::PF_L8ui, NII::PF_A8B8G8R8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return 0xFF000000|(((NII::Nui32)inp)<<0)|(((NII::Nui32)inp)<<8)|(((NII::Nui32)inp)<<16);
    }
};

struct A8R8G8B8toL8: public PixelConverter <NII::Nui32, NII::Nui8, FMTCONVERTERID(NII::PF_A8R8G8B8, NII::PF_L8ui)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return (NII::Nui8)((inp&0x00FF0000)>>16);
    }
};

struct L8toA8R8G8B8: public PixelConverter <NII::Nui8, NII::Nui32, FMTCONVERTERID(NII::PF_L8ui, NII::PF_A8R8G8B8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return 0xFF000000|(((NII::Nui32)inp)<<0)|(((NII::Nui32)inp)<<8)|(((NII::Nui32)inp)<<16);
    }
};

struct B8G8R8A8toL8: public PixelConverter <NII::Nui32, NII::Nui8, FMTCONVERTERID(NII::PF_B8G8R8A8UN, NII::PF_L8ui)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return (NII::Nui8)((inp&0x0000FF00)>>8);
    }
};

struct L8toB8G8R8A8: public PixelConverter <NII::Nui8, NII::Nui32, FMTCONVERTERID(NII::PF_L8ui, NII::PF_B8G8R8A8UN)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return 0x000000FF|(((NII::Nui32)inp)<<8)|(((NII::Nui32)inp)<<16)|(((NII::Nui32)inp)<<24);
    }
};

struct L8toL16: public PixelConverter <NII::Nui8, NII::Nui16, FMTCONVERTERID(NII::PF_L8ui, NII::PF_L16ui)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return (NII::Nui16)((((NII::Nui32)inp)<<8)|(((NII::Nui32)inp)));
    }
};

struct L16toL8: public PixelConverter <NII::Nui16, NII::Nui8, FMTCONVERTERID(NII::PF_L16ui, NII::PF_L8ui)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return (NII::Nui8)(inp>>8);
    }
};

struct R8G8B8toB8G8R8: public PixelConverter <Col3b, Col3b, FMTCONVERTERID(NII::PF_R8G8B8UN, NII::PF_B8G8R8UN)>
{
    inline static DstType pixelConvert(const SrcType &inp)
    {
        return Col3b(inp.z, inp.y, inp.x);
    }
};

struct B8G8R8toR8G8B8: public PixelConverter <Col3b, Col3b, FMTCONVERTERID(NII::PF_B8G8R8UN, NII::PF_R8G8B8UN)>
{
    inline static DstType pixelConvert(const SrcType &inp)
    {
        return Col3b(inp.z, inp.y, inp.x);
    }
};

// X8Y8Z8 ->  X8<<xshift Y8<<yshift Z8<<zshift A8<<ashift
template <NII::NIIi id, NII::Nui32 xshift, NII::Nui32 yshift, NII::Nui32 zshift, NII::Nui32 ashift>
    struct Col3btoUint32swizzler : public PixelConverter <Col3b, NII::Nui32, id>
{
    inline static NII::Nui32 pixelConvert(const Col3b & inp)
    {
#if N_ENDIAN == N_ENDIAN_B
        return (0xFF<<ashift) | (((NII::Nui32)inp.x)<<xshift) | (((NII::Nui32)inp.y)<<yshift) | (((NII::Nui32)inp.z)<<zshift);
#else
        return (0xFF<<ashift) | (((NII::Nui32)inp.x)<<zshift) | (((NII::Nui32)inp.y)<<yshift) | (((NII::Nui32)inp.z)<<xshift);
#endif
    }
};

struct R8G8B8toA8R8G8B8: public Col3btoUint32swizzler<FMTCONVERTERID(NII::PF_R8G8B8UN, NII::PF_A8R8G8B8), 16, 8, 0, 24> { };
struct B8G8R8toA8R8G8B8: public Col3btoUint32swizzler<FMTCONVERTERID(NII::PF_B8G8R8UN, NII::PF_A8R8G8B8), 0, 8, 16, 24> { };
struct R8G8B8toA8B8G8R8: public Col3btoUint32swizzler<FMTCONVERTERID(NII::PF_R8G8B8UN, NII::PF_A8B8G8R8), 0, 8, 16, 24> { };
struct B8G8R8toA8B8G8R8: public Col3btoUint32swizzler<FMTCONVERTERID(NII::PF_B8G8R8UN, NII::PF_A8B8G8R8), 16, 8, 0, 24> { };
struct R8G8B8toB8G8R8A8: public Col3btoUint32swizzler<FMTCONVERTERID(NII::PF_R8G8B8UN, NII::PF_B8G8R8A8UN), 8, 16, 24, 0> { };
struct B8G8R8toB8G8R8A8: public Col3btoUint32swizzler<FMTCONVERTERID(NII::PF_B8G8R8UN, NII::PF_B8G8R8A8UN), 24, 16, 8, 0> { };

struct A8R8G8B8toR8G8B8: public PixelConverter <NII::Nui32, Col3b, FMTCONVERTERID(NII::PF_A8R8G8B8, NII::PF_BYTE_RGB)>
{
    inline static DstType pixelConvert(NII::Nui32 inp)
    {
        return Col3b((NII::Nui8)((inp>>16)&0xFF), (NII::Nui8)((inp>>8)&0xFF), (NII::Nui8)((inp>>0)&0xFF));
    }
};

struct A8R8G8B8toB8G8R8: public PixelConverter <NII::Nui32, Col3b, FMTCONVERTERID(NII::PF_A8R8G8B8, NII::PF_BYTE_BGR)>
{
    inline static DstType pixelConvert(NII::Nui32 inp)
    {
        return Col3b((NII::Nui8)((inp>>0)&0xFF), (NII::Nui8)((inp>>8)&0xFF), (NII::Nui8)((inp>>16)&0xFF));
    }
};

// Only conversions from X8R8G8B8 to formats with alpha need to be defined, the rest is implicitly the same
// as A8R8G8B8
struct X8R8G8B8toA8R8G8B8: public PixelConverter <NII::Nui32, NII::Nui32, FMTCONVERTERID(NII::PF_X8R8G8B8, NII::PF_A8R8G8B8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return inp | 0xFF000000;
    }
};

struct X8R8G8B8toA8B8G8R8: public PixelConverter <NII::Nui32, NII::Nui32, FMTCONVERTERID(NII::PF_X8R8G8B8, NII::PF_A8B8G8R8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x0000FF)<<16)|((inp&0xFF0000)>>16)|(inp&0x00FF00)|0xFF000000;
    }
};

struct X8R8G8B8toB8G8R8A8: public PixelConverter <NII::Nui32, NII::Nui32, FMTCONVERTERID(NII::PF_X8R8G8B8, NII::PF_B8G8R8A8UN)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x0000FF)<<24)|((inp&0xFF0000)>>8)|((inp&0x00FF00)<<8)|0x000000FF;
    }
};

struct X8R8G8B8toR8G8B8A8: public PixelConverter <NII::Nui32, NII::Nui32, FMTCONVERTERID(NII::PF_X8R8G8B8, NII::PF_R8G8B8A8ui)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0xFFFFFF)<<8)|0x000000FF;
    }
};

// X8B8G8R8
struct X8B8G8R8toA8R8G8B8: public PixelConverter <NII::Nui32, NII::Nui32, FMTCONVERTERID(NII::PF_X8B8G8R8, NII::PF_A8R8G8B8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x0000FF)<<16)|((inp&0xFF0000)>>16)|(inp&0x00FF00)|0xFF000000;
    }
};

struct X8B8G8R8toA8B8G8R8: public PixelConverter <NII::Nui32, NII::Nui32, FMTCONVERTERID(NII::PF_X8B8G8R8, NII::PF_A8B8G8R8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return inp | 0xFF000000;
    }
};

struct X8B8G8R8toB8G8R8A8: public PixelConverter <NII::Nui32, NII::Nui32, FMTCONVERTERID(NII::PF_X8B8G8R8, NII::PF_B8G8R8A8UN)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0xFFFFFF)<<8)|0x000000FF;
    }
};

struct X8B8G8R8toR8G8B8A8: public PixelConverter <NII::Nui32, NII::Nui32, FMTCONVERTERID(NII::PF_X8B8G8R8, NII::PF_R8G8B8A8ui)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x0000FF)<<24)|((inp&0xFF0000)>>8)|((inp&0x00FF00)<<8)|0x000000FF;
    }
};

#define CASECONVERTER(type) case type::ID : PixelBoxConverter<type>::conversion(src, dst); return 1;

inline NII::NIIi doOptimizedConversion(const NII::PixelBlock & src, const NII::PixelBlock & dst)
{
    switch(FMTCONVERTERID(src.mFormat, dst.mFormat))
    {
        // 在这里注册转换器
        CASECONVERTER(A8R8G8B8toA8B8G8R8);
        CASECONVERTER(A8R8G8B8toB8G8R8A8);
        CASECONVERTER(A8R8G8B8toR8G8B8A8);
        CASECONVERTER(A8B8G8R8toA8R8G8B8);
        CASECONVERTER(A8B8G8R8toB8G8R8A8);
        CASECONVERTER(A8B8G8R8toR8G8B8A8);
        CASECONVERTER(B8G8R8A8toA8R8G8B8);
        CASECONVERTER(B8G8R8A8toA8B8G8R8);
        CASECONVERTER(B8G8R8A8toR8G8B8A8);
        CASECONVERTER(R8G8B8A8toA8R8G8B8);
        CASECONVERTER(R8G8B8A8toA8B8G8R8);
        CASECONVERTER(R8G8B8A8toB8G8R8A8);
        CASECONVERTER(A8B8G8R8toR8);
        CASECONVERTER(R8toA8B8G8R8);
        CASECONVERTER(A8R8G8B8toR8);
        CASECONVERTER(R8toA8R8G8B8);
        CASECONVERTER(B8G8R8A8toR8);
        CASECONVERTER(R8toB8G8R8A8);
        CASECONVERTER(A8B8G8R8toL8);
        CASECONVERTER(L8toA8B8G8R8);
        CASECONVERTER(A8R8G8B8toL8);
        CASECONVERTER(L8toA8R8G8B8);
        CASECONVERTER(B8G8R8A8toL8);
        CASECONVERTER(L8toB8G8R8A8);
        CASECONVERTER(L8toL16);
        CASECONVERTER(L16toL8);
        CASECONVERTER(B8G8R8toR8G8B8);
        CASECONVERTER(R8G8B8toB8G8R8);
        CASECONVERTER(R8G8B8toA8R8G8B8);
        CASECONVERTER(B8G8R8toA8R8G8B8);
        CASECONVERTER(R8G8B8toA8B8G8R8);
        CASECONVERTER(B8G8R8toA8B8G8R8);
        CASECONVERTER(R8G8B8toB8G8R8A8);
        CASECONVERTER(B8G8R8toB8G8R8A8);
        CASECONVERTER(A8R8G8B8toR8G8B8);
        CASECONVERTER(A8R8G8B8toB8G8R8);
        CASECONVERTER(X8R8G8B8toA8R8G8B8);
        CASECONVERTER(X8R8G8B8toA8B8G8R8);
        CASECONVERTER(X8R8G8B8toB8G8R8A8);
        CASECONVERTER(X8R8G8B8toR8G8B8A8);
        CASECONVERTER(X8B8G8R8toA8R8G8B8);
        CASECONVERTER(X8B8G8R8toA8B8G8R8);
        CASECONVERTER(X8B8G8R8toB8G8R8A8);
        CASECONVERTER(X8B8G8R8toR8G8B8A8);
    default:
        return 0;
    }
}
#undef CASECONVERTER
#endif
#endif
