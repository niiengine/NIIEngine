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
#ifndef _NII_FreeTypeFont_H_
#define _NII_FreeTypeFont_H_

#include "NiiPreInclude.h"
#include "NiiFont.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace NII
{
    /** ttf 字体
    @version NIIEngine 3.0.0
    */
    class FreeTypeFont : public Font
    {
    public:
        FreeTypeFont(FontID fid, NIIf size, NCount pdi, bool antialias,
            const String & src, ResourceID rid, GroupID gid = GroupUnknow,
            ResLoadScheme * ls = 0, ResResultScheme * rs = 0, ScaleMode sm = SM_Unknow,
			const PlaneSizef & nativeRes = PlaneSizef(640.0f, 480.0f), NIIf lineSpace = 0.0f, 
				LangID lid = N_PrimaryLang);

        ~FreeTypeFont();

        /** 设置分辨率
        @version NIIEngine 3.0.0
        */
        void setDPI(NCount c);

        /** 获取分辨率
        @version NIIEngine 3.0.0
        */
        NCount getDPI() const;

        /** 设置点大小
        @version NIIEngine 3.0.0
        */
        void setPointSize(NIIf size);

        /** 获取点大小
        @version NIIEngine 3.0.0
        */
        NIIf getPointSize() const;

        /** 设置是否抗锯齿
        @version NIIEngine 3.0.0
        */
        void setAntialias(bool b);

        /** 获取是否抗锯齿
        @version NIIEngine 3.0.0
        */
        bool isAntialias() const;
        
        /** 设置行距离
        @version NIIEngine 3.0.0
        */
		void setLineSpace(NIIf space);

        /** 获取行距离
        @version NIIEngine 3.0.0
        */
        NIIf getLineSpace() const;
        
        /** 设置此抗锯齿字体的颜色.
        @version NIIEngine 3.0.0
        */
        void setAlphaAsColour(bool b);

        /** 获取此抗锯齿字体的颜色.
        @version NIIEngine 3.0.0
        */
        bool isAlphaAsColour() const;
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

        /** 生成字体
        @param[in] 纹理缓存
        @param[in] w 主纹理的宽
        @param[in] h 主文理的高
        @version NIIEngine 3.0.0 内部api
        */
        void genCodePointGlyph(Nui16 * out, NCount w, NCount h) const;

        /** 删除生成字体
        @version NIIEngine 3.0.0 内部api
        */
        void destroyCodePointGlyph();

        /** 获取纹理大小
        @param out 需要的纹理大小
        @param start 开始
        @param end 结束
        @return 实际加载字符数量
        @version NIIEngine 3.0.0 内部api
        */
        NCount getTextureSize(Vector2ui & out, CodePoint start, CodePoint end) const;
    protected:
        /// @copydetails Resource::planImpl
        void planImpl();

        /// @copydetails Resource::loadImpl
        void loadImpl();

        /// @copydetails Resource::unloadImpl
        void unloadImpl();

        /// @copydetails Resource::calcSize
        NCount calcSize() const;
    protected:
        NCount mDIP;
        NIIf mPointSize;
        NIIf mLineSpace;
        FT_Face mFontFace;
        DataStream * mFontStream;
        bool mAntialias;
        bool mAlphaAsColour;
    };
}
#endif