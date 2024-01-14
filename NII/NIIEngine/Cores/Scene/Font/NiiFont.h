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
#ifndef _NII_FONT_H_
#define _NII_FONT_H_

#include "NiiPreInclude.h"
#include "NiiFontGlyph.h"
#include "NiiColour.h"
#include "NiiEvent.h"

namespace NII
{
    /** 字体事件参数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FontEventArgs : public EventArgs
    {
    public:
        FontEventArgs(Font * font) :
            mFont(font) {}

        Font * mFont;
    };

    /** 字体类型
    @version NIIEngine 3.0.0
    */
    enum FontType
    {
        FT_TrueType = 1,    ///< ttf字体
        FT_Pixel = 2        ///< 像素
    };

    /** 字体
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Font : public Resource
    {
    public:
        typedef std::pair<CodePoint, CodePoint> CodePointRange;
        typedef vector<CodePointRange>::type CodePointRangeList;
        typedef map<CodePoint, FontGlyph>::type CodePointList;
        typedef vector<std::pair<Texture *, CodePointList *> >::type TextureGlyphList;
    public:
        virtual ~Font();
        
        /** 获取字体ID
        @note 和 ResourceID 概念不一样
        @version NIIEngine 3.0.0高级api
        */
        void setID(FontID fid);
        
        /** 获取字体ID
        @note 和 ResourceID 概念不一样
        @version NIIEngine 3.0.0
        */
        FontID getID() const;

        /** 获取字体类型
        @version NIIEngine 3.0.0
        */
        FontType getFontType() const;

        /** 设置资源来源
        @note 可以是字体文件或图片文件
        @version NIIEngine 3.0.0
        */
        void setSource(const String & src);
        
        /** 获取资源来源
        @note 可能字体或图片
        @version NIIEngine 3.0.0
        */
        const String & getSource() const;

        /** 定义字符范围
        @param[in] range
        @version NIIEngine 3.0.0
        */
        void addDefineRange(const CodePointRange & range);

        /** 清理所有定义字符
        @version NIIEngine 3.0.0
        */
        void clearAllDefineRange();

        /** 获取字符范围
        @version NIIEngine 3.0.0
        */
        const CodePointRangeList & getDefineRangeList() const;

        /** 获取加载的字符范围列表
        @note 和 getDefineRangeList 相比,这个属于页管理结构
        @version NIIEngine 3.0.0 高级api
        */
        const CodePointRangeList & getCodePointList() const;

        /** 获取资源原分辨率
        @note (像素级)
        @version NIIEngine 3.0.0
        */
        void setNativeRes(const PlaneSizef & size);

        /** 设置视图分辨率
        @note (像素级)
        @version NIIEngine 3.0.0
        */
        void setViewRes(const PlaneSizei & size);

        /** 设置资源原分辨率
        @note (像素级)
        @version NIIEngine 3.0.0
        */
        const PlaneSizef & getNativeRes() const;

        /** 设置缩放模式
        @version NIIEngine 3.0.0
        */
        void setScaleMode(ScaleMode sm);

        /** 获取缩放模式
        @version NIIEngine 3.0.0
        */
        ScaleMode getScaleMode() const;

        /** 渲染字体到像素几何缓像素中
        @param dest 几何像素
        @param text 文本
        @param pos
        @param xScale
        @param yScale
        @return
        */
        NIIf drawText(GeometryPixel * dest, const String & text,
            const Vector2f & pos, const Rectf * clipper, const ColourArea & colour,
                NIIf spaceExtent = 0.0f, NIIf xScale = 1.0f, NIIf yScale = 1.0f) const;

        /** 字符图形结构是否存在
        @version NIIEngine 3.0.0
        */
        bool isCodePointExist(CodePoint c) const;

        /** 获取字符图形结构
        @param[in] pageidx
        @version NIIEngine 3.0.0
        */
        const FontGlyph * getCodePoint(CodePoint c, Nidx & pageidx) const;

        /** 获取材质
        @param[in] pageidx
        @version NIIEngine 3.0.0
        */
        Material * getMaterial(Nidx pageidx) const;

        /** 获取材质
        @param[in] pageidx
        @version NIIEngine 3.0.0
        */
        Texture * getTexture(Nidx pageidx) const;
        
        /** 字体高度
        @version NIIEngine 3.0.0
        */
        NIIf getFontHeight() const;

        /** 基础行
        @version NIIEngine 3.0.0
        */
        NIIf getBaseline() const;

        /** 行距
        @version NIIEngine 3.0.0
        */
        NIIf getLineSpacing() const;

        /** 获取字符宽高比
        @version NIIEngine 3.0.0
        */
        NIIf getCharAspect(CodePoint c) const;
        
        /** 获取字符宽高比
        @version NIIEngine 3.0.0
        */
        NIIf getCharAspect(const FontGlyph * glyph) const;
        
        /** 获取字符串位偏长度
        @version NIIEngine 3.0.0
        */
        NIIf getTextExtent(const String & text) const;

        /** 获取字符串水平偏移
        @version NIIEngine 3.0.0
        */
        NIIf getTextAdvance(const String & text) const;

        /** 获取指定像素中的字符
        @version NIIEngine 3.0.0
        */
        NCount getCharAtPixel(const String & txt, NIIf pixel, NIIf xScale = 1.0f) const;

        /** 获取指定像素中的字符
        @version NIIEngine 3.0.0
        */
        NCount getCharAtPixel(const String & txt, NCount start, NIIf pixel, NIIf xScale = 1.0f) const;

        /** 获取视图X方向缩放
        @version NIIEngine 3.0.0 高级api
        */
        NIIf getViewXScale() const;

        /** 获取视图Y方向缩放
        @version NIIEngine 3.0.0 高级api
        */
        NIIf getViewYScale() const;

        /** 获取纹理文字列表
        @version NIIEngine 3.0.0 高级api
        */
        const TextureGlyphList & getGlyphList() const;
    public:
        static const ARGB DefaultColour;
        static const NIIf DefaultNativeHorzRes;
        static const NIIf DefaultNativeVertRes;
    protected:
        Font(ScriptTypeID stid, FontID id, FontType type, const String & src, ResourceID rid, GroupID gid,
            ResLoadScheme * ls = 0, ResResultScheme * rs = 0, ScaleMode mode = SM_Unknow,
                const PlaneSizef & nativeRes = PlaneSizef(640.0f, 480.0f), LangID lid = N_PrimaryLang);

        /// @copydetails PropertyCmdObj::init
        bool initCmd(PropertyCmdSet * dest);

        /** 获取字符实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual const FontGlyph * getCodePointImpl(CodePoint c, Nidx & page) const;

        /** 光栅化字符
        @note 一般是指编码连续的一段字体
        @param start 开始字符
        @param end 结束字符
        @version NIIEngine 3.0.0 高级api
        */
        virtual void rasterCodePointImpl(CodePoint start, CodePoint end) = 0;

        /** 字符形态变化,刷新已光栅字符
        @version NIIEngine 3.0.0 高级api
        */
        virtual void reflushCodePointImpl() = 0;

        /** 获取字符宽
        @version NIIEngine 3.0.0 高级api
        */
        virtual NIIf getCodePointWidth(const FontGlyph * glyph) const = 0;

        /** 获取字符高
        @version NIIEngine 3.0.0 高级api
        */
        virtual NIIf getCodePointHeight(const FontGlyph * glyph) const = 0;

        /** 获取字符位偏
        @version NIIEngine 3.0.0 高级api
        */
        virtual NIIf getCodePointExtentImpl(const FontGlyph * glyph) const = 0;
    protected:
        FontID mFontID;
        FontType mType;
        String mSource;
        TextureGlyphList mTextureGlyphList;
        CodePointRangeList mDefineRangeList;
        CodePointRangeList mLoadRangeList;
        MaterialList mMaterialList;
        CodePoint mMinCodePoint;
        CodePoint mMaxCodePoint;
        ScaleMode mScaleMode;
        NIIf mHeight;
        PlaneSizef mNativeRes;
        NIIf mXScale;
        NIIf mYScale;
        NIIf mAscLine;
        NIIf mDesLine;
    };
}
#endif