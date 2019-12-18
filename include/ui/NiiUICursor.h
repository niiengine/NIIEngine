/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_UI_CURSOR_H_
#define _NII_UI_CURSOR_H_

#include "NiiUIPreInclude.h"
#include "NiiCommandObj.h"
#include "NiiUICursorEventArgs.h"
#include "NiiRelPos.h"

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
        Cursor(const Rectf & area);
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
        void hide();

        /** 显示
        @version NIIEngine 3.0.0
        */
        void show();

        /** 设置是否可见
        @version NIIEngine 3.0.0
        */
        void setVisible(bool b);

        /** 获取是否可见
        @version NIIEngine 3.0.0
        */
        bool isVisible() const;

        /** 设置大小
        @version NIIEngine 3.0.0
        */
        void setRenderSize(const PlaneSizef & size);

        /** 获取大小
        @version NIIEngine 3.0.0
        */
        const PlaneSizef & getRenderSize() const;

        /** 设置位置
        @version NIIEngine 3.0.0
        */
        void setPos(const Vector2f & pos);

        /** 获取位置
        @version NIIEngine 3.0.0
        */
        const Vector2f & getPos() const;

        /** 设置当前图案
        @version NIIEngine 3.0.0
        */
        void setImage(PixelBuffer * pb);

        /** 获取当前图案
        @version NIIEngine 3.0.0
        */
        PixelBuffer * getImage() const;

        /** 设置基础图案
        @version NIIEngine 3.0.0
        */
        void setDefaultImage(PixelBuffer * pb);

        /** 获取基础图案
        @version NIIEngine 3.0.0
        */
        PixelBuffer * getDefaultImage() const;

        /** 设置移动区域
        @version NIIEngine 3.0.0
        */
        void setMoveArea(const Rectf * area);

        /** 获取移动区域
        @version NIIEngine 3.0.0
        */
        Rectf getMoveArea() const;

        /** 设置移动区域
        @version NIIEngine 3.0.0
        */
        void setEvnMoveArea(const RelRect * area);

        /** 获取移动区域
        @version NIIEngine 3.0.0
        */
        const RelRect & getEvnMoveArea() const;

        /** 通知画盘大小
        @version NIIEngine 3.0.0
        */
        void notifyView(const PlaneSizef & size);
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
        Rectf mArea;
        RelRect mMoveArea;
        bool mVisible;
        bool mBufferValid;
    };
}
}
#endif