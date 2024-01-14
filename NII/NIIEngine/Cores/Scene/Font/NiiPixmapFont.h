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

#ifndef _NII_PixmapFont_H_
#define _NII_PixmapFont_H_

#include "NiiPreInclude.h"
#include "NiiFont.h"

namespace NII
{
    /** 像素级字体
    @version NIIEngine 3.0.0
    */
    class PixmapFont : public Font
    {
    public:
        PixmapFont(FontID fid, ResourceID rid, GroupID gid = GroupUnknow,
            ResLoadScheme * ls = 0, ResResultScheme * rs = 0, LangID lid = N_PrimaryLang);
        PixmapFont(FontID fid, const String & src, ResourceID rid, GroupID gid = GroupUnknow,
            ResLoadScheme * ls = 0, ResResultScheme * rs = 0, ScaleMode mode = SM_Unknow,
                const PlaneSizef & nativeRes = PlaneSizef(640.0f, 480.0f), LangID lid = N_PrimaryLang);
        ~PixmapFont();

        /** 定义字符
        @param[in] texidx 纹理索引,对应着所设置的纹理数组
        @param[in] value 字符码值
        @param[in] coord 纹理坐标
        @param[in] oft 像素位置偏移
        @param[in] extent 宽度
        @version NIIEngine 3.0.0
        */
        void defineCodePoint(Nidx texidx, CodePoint value, const Rectf & coord,
            const Vector2f & oft, NIIf extent);

        /** 设置纹理ID
        @note 将会重新设置当前纹理
        @version NIIEngine 3.0.0 高级api
        */
        void setTexture(ResourceID * rid, NCount count);

        /** 获取纹理ID
        @version NIIEngine 3.0.0 高级api
        */
        void getTexture(TextureList & out) const;
    protected:
        /// @copydetails Resource::loadImpl
        void loadImpl();

        /// @copydetails Resource::unloadImpl
        void unloadImpl();

        /// @copydetails Resource::calcSize
        NCount calcSize() const;
    protected:
        /// @copydetails PropertyCmdObj::init
        bool initCmd(PropertyCmdSet * dest);

        /// @copydetails Font::rasterCodePointImpl
        void rasterCodePointImpl(CodePoint start, CodePoint end);

        /// @copydetails Font::reflushCodePointImpl
        void reflushCodePointImpl();

        /// @copydetails Font::getCodePointWidth
        NIIf getCodePointWidth(const FontGlyph * value) const;

        /// @copydetails Font::getCodePointHeight
        NIIf getCodePointHeight(const FontGlyph * value) const;

        /// @copydetails Font::getCodePointExtentImpl
        NIIf getCodePointExtentImpl(const FontGlyph * value) const;
    protected:
        NIIf mLastXScale;
        NIIf mLastYScale;
    };
}
#endif