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

#ifndef _NII_FontGlyph_H_
#define _NII_FontGlyph_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 字体字型
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FontGlyph : public UIAlloc
    {
    public:
        FontGlyph(){}
        FontGlyph(const Rectf & coord, const Vector2f & oft, NIIf extent) :
            mTexCoord(coord),
            mPixelOffset(oft),
            mHoriAdvance(extent){}

        /** 设置纹理坐标
        @version NIIEngine 3.0.0 高级 api
        */
        void setTextureCoord(const Rectf & coord)
        {
            mTexCoord = coord;
        }

        /** 获取纹理坐标
        @version NIIEngine 3.0.0 高级 api
        */
        const Rectf & getTextureCoord() const
        {
            return mTexCoord;
        }

        /** 设置像素显示位置偏移
        @version NIIEngine 3.0.0 高级 api
        */
        void setOffset(const Vector2f & oft)
        {
            mPixelOffset = oft;
        }

        /** 获取像素显示位置偏移
        @version NIIEngine 3.0.0 高级 api
        */
        const Vector2f & getOffset() const
        {
            return mPixelOffset;
        }

        /** 设置水平偏移
        @version NIIEngine 3.0.0 高级 api
        */
        void setHoriAdvance(NIIf adv)
        {
            mHoriAdvance = adv;
        }

        /** 获取水平偏移
        @version NIIEngine 3.0.0 高级 api
        */
        NIIf getHoriAdvance() const
        {
            return mHoriAdvance;
        }
    protected:
        Rectf mTexCoord;
        Vector2f mPixelOffset;
        NIIf mHoriAdvance;
    };
    typedef vector<FontGlyph *>::type FontGlyphList;
}
#endif