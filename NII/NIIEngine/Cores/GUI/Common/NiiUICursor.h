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

#ifndef _NII_UI_CURSOR_H_
#define _NII_UI_CURSOR_H_

#include "NiiUIPreInclude.h"
#include "NiiCommandObj.h"
#include "NiiUICursorEventArgs.h"
#include "NiiPlaneSize.h"

using namespace NII::NII_COMMAND;

namespace NII
{
namespace UI
{
    /** 光标对象
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Cursor : public CommandObj, public UIAlloc
    {
    public:
        Cursor(const Recti & area);
        ~Cursor();

        /** 绘制
        @version NIIEngine 3.0.0
        */
        void draw();

        /** 刷新
        @note 一般光标图案/位置变化了要刷新
        @version NIIEngine 3.0.0
        */
        void refresh();

        /** 隐藏
        @version NIIEngine 3.0.0
        */
        void hide()                         { mVisible = false; }

        /** 显示
        @version NIIEngine 3.0.0
        */
        void show()                         { mVisible = true; }

        /** 设置是否可见
        @version NIIEngine 3.0.0
        */
        void setVisible(bool b)             { mVisible = b; }

        /** 获取是否可见
        @version NIIEngine 3.0.0
        */
        bool isVisible() const              { return mVisible; }

        /** 设置大小
        @version NIIEngine 3.0.0
        */
        void setRenderSize(const PlaneSizef & size);

        /** 获取大小
        @version NIIEngine 3.0.0
        */
        const PlaneSizef & getRenderSize() const { return mSize; }

        /** 设置位置
        @version NIIEngine 3.0.0
        */
        void setPos(const Vector2f & pos);

        /** 获取位置
        @version NIIEngine 3.0.0
        */
        const Vector2f & getPos() const         { return mPos; }

        /** 设置当前图案
        @version NIIEngine 3.0.0
        */
        void setImage(PixelBuffer * pb);

        /** 获取当前图案
        @version NIIEngine 3.0.0
        */
        PixelBuffer * getImage() const          { return mView; }

        /** 设置基础图案
        @version NIIEngine 3.0.0
        */
        void setDefaultImage(PixelBuffer * pb);

        /** 获取基础图案
        @version NIIEngine 3.0.0
        */
        PixelBuffer * getDefaultImage() const   { return mNormalView; }

        /** 设置移动区域
        @version NIIEngine 3.0.0
        */
        void setMoveArea(const Recti * area);

        /** 获取移动区域
        @version NIIEngine 3.0.0
        */
        Recti getMoveArea() const;

        /** 设置移动区域
        @version NIIEngine 3.0.0
        */
        void setEvnMoveArea(const RelRecti * area);

        /** 获取移动区域
        @version NIIEngine 3.0.0
        */
        const RelRecti & getEvnMoveArea() const { return mMoveArea; }

        /** 通知画盘大小
        @version NIIEngine 3.0.0
        */
        void notifyView(const PlaneSizei & size);
    protected:
        Cursor();

        /** 当前图案改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onImageChange(const CursorViewArgs * arg);

        /** 基础图案改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onDefaultImageChange(const CursorViewArgs * arg);

        void clampMoveArea();
    public:
        /** 当前图案改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID ImageChangeEvent;

        /** 基础图案改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID DefaultImageChangeEvent;

        /// 事件总数
        static const EventID EventCount;
    private:
        GeometryPixel * mBuffer;
        PixelBuffer * mView;
        PixelBuffer * mNormalView;
        Vector2f mPos;
        PlaneSizef mSize;
        Recti mArea;
        RelRecti mMoveArea;
        bool mVisible;
        bool mBufferValid;
    };
}
}
#endif