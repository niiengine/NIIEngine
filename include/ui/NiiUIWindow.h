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

#ifndef _NII_UI_WINDOW_H_
#define _NII_UI_WINDOW_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"
#include "NiiUIInstance.h"

namespace NII
{
namespace UI
{
    /** 窗体级UI单元
    @remark 只有窗体级UI单元才能被Sheet绘制
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Window : public Container
    {
        friend class UISheet;
    public:
        Window(WidgetID wid, FactoryID fid, Container * own, UISheet * sheet = 0, const String & name = N_StrBlank,
			ScriptTypeID stid = N_CMD_Window, LangID lid = N_PrimaryLang);
        virtual ~Window();

        /** 设置旋转
        @version NIIEngine 3.0.0
        */
        void setRotation(const Quaternion & q);

        /** 获取旋转
        @version NIIEngine 3.0.0
        */
        const Quaternion & getRotation() const;

        /** 设置是否需要等待处理
        @remark
            等待处理指的是从UI窗体显示开始到UI窗体显示结束的这断时间
        @version NIIEngine 3.0.0
        */
        void setModal(bool b);

        /** 返回是否需要等待处理
        @remark
            等待处理指的是从UI窗体显示开始到UI窗体显示结束的这断时间
        @version NIIEngine 3.0.0
        */
        bool isModal() const;

        /** 设置鼠标事件是否穿过这个对象而触发其背后对象
        @param b 是或否
        */
        void setOvergo(bool b);

        /** 返回鼠标事件是否穿过这个对象而触发其背后对象
        @return 是或否
        @version NIIEngine 3.0.0
        */
        bool isOvergo() const;

        /** 设置自动生成渲染体
        @version NIIEngine 3.0.0
        */
        void setAutoRender(bool b);

        /** 是否自动生成渲染体
        @version NIIEngine 3.0.0 高级api
        */
        bool isAutoRender() const;

        /** 设置独立渲染体
        @version NIIEngine 3.0.0 高级api
        */
        void setRender(UIInstance * o);

        /** 获取独立渲染体
        @version NIIEngine 3.0.0 高级api
        */
        UIInstance * getRender() const;

        /** 设置UI画盘
        @version NIIEngine 3.0.0 高级api
        */
        void setSheet(UISheet * obj);

        /** 获取UI画盘
        @version NIIEngine 3.0.0 高级api
        */
        UISheet * getSheet() const;

        /** 获取一个投影后的位置在机制中的实际位置
        @param[in] pos 投影后(可见结果)的位置
        @return 内部机制实际的位置
        @version NIIEngine 3.0.0
        */
        Vector2f affine(const Vector2f & pos) const;

		/// @copydetails Widget::isPos
		bool isPos(const Vector2f & pos, bool disable) const;

        /// @copydetails Widget::activate
        void activate();
    protected:
        /** 旋转时触发
        @version NIIEngine 3.0.0
        */
        virtual void onRotate(const WidgetEventArgs * arg);

        /// @copydetails Widget::onSize
        virtual void onSize(const WidgetEventArgs * arg);

        /// @copydetails Widget::onDestroy
        virtual void onDestroy(const WidgetEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onRemove
		virtual void onRemove(const WidgetEventArgs * arg);

        /// @copydetails Container::onChildMove
        virtual void onChildMove(const WidgetEventArgs * arg);
    protected:
        /// @copydetails PropertyCmdObj::init
        virtual bool init(PropertyCmdSet * dest);

        /// @copydetails Widget::getClipAreaImpl
        virtual Rectf getClipAreaImpl() const;

        /// @copydetails Widget::getClipInnerAreaImpl
        virtual Rectf getClipInnerAreaImpl() const;

        /// @copydetails Widget::updateImpl
        virtual void updateImpl(TimeDurMS t);

        /// @copydetails Widget::updateImpl
        virtual void updateImpl(SheetContext * rc);

        /// @copydetails Widget::getContextImpl
        virtual void getContextImpl(SheetContext *& ctx) const;

        /// @copydetails Widget::refreshFaceImpl
        virtual void refreshFaceImpl();

        /// @copydetails Widget::queuePixelImpl
        virtual void queuePixelImpl();

        /** 分配渲染面
        @version NIIEngine 3.0.0
        */
        void allocRender();

        /** 删除渲染面
        @version NIIEngine 3.0.0
        */
        void freeRender();
    protected:
        UIInstance * mRender;   ///< 实际的渲染面
        bool mAutoGenRender;    ///< 自动创建渲染面
        bool mOvergo;           ///< 鼠标类事件穿透这个窗体
    };
}
}
#endif