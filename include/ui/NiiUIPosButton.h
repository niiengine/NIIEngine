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

#ifndef _NII_UI_POSBUTTON_H_
#define _NII_UI_POSBUTTON_H_

#include "NiiUIPreInclude.h"
#include "NiiUIPushButton.h"

namespace NII
{
namespace UI
{
    /** 记录按纽
    @remark 记录位置状态的按纽
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PosButton : public PushButton
    {
    public:
        PosButton(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
			LangID lid = N_PrimaryLang);

        virtual ~PosButton();

        /** 设置是否同步位置事件
        @remark 
        @version NIIEngine 3.0.0
        */
        void setPosEvent(bool b);

        /** 获取是否同步位置事件
        @version NIIEngine 3.0.0
        */
        bool isPosEvent() const;

        /** 设置y方向移动
        @version NIIEngine 3.0.0
        */
        void setYMove(bool b);

        /** 获取y方向移动
        @version NIIEngine 3.0.0
        */
        bool isYMove() const;

        /** 设置x方向移动
        @version NIIEngine 3.0.0
        */
        void setXMove(bool b);

        /** 获取x方向移动
        @version NIIEngine 3.0.0
        */
        bool isXMove() const;

        /** 设置y方向范围
        @version NIIEngine 3.0.0
        */
        void setYRange(NIIf min, NIIf max);

        /** 获取y方向范围
        @version NIIEngine 3.0.0
        */
        void getYRange(NIIf & min, NIIf & max) const;

        /** 设置x方向范围
        @version NIIEngine 3.0.0
        */
        void setXRange(NIIf min, NIIf max);

        /** 获取x方向范围
        @version NIIEngine 3.0.0
        */
        void getXRange(NIIf & min, NIIf & max) const;
    protected:
        /** 这个UI对象开始拖拽时触发
        @remark
            达成拖拽条件一般是指游标在这个UI对象的位置区域里,而且按下触发拖拽的按键
		@version NIIEngine 3.0.0
        */
        virtual void onMoveBegin(const WidgetEventArgs * arg);

        /** 这个UI对象结束拖拽时触发
        @remark
            达成拖拽条件后,又达成了拖拽结束的条件后触发,一般是指释放了游标的按键
		@version NIIEngine 3.0.0
        */
        virtual void onMoveEnd(const WidgetEventArgs * arg);

        /** 这个UI对象的位置发生变化时触发
        @remark
            依赖相应的是否同步位置变化事件的设计,如果同步则实时触发(存在多次),否则
            是以在每次开始拖拽事件到结束拖拽事件后触发一次.
		@version NIIEngine 3.0.0
        */
        virtual void onPosChange(const WidgetEventArgs * arg);

        /// @copydetails Widget::onCursorMove
        virtual void onCursorMove(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onLost
        virtual void onLost(const WidgetEventArgs * arg);
    protected:
        /// @copydetails PropertyCmdObj::init
        virtual bool init(PropertyCmdSet * dest);

        /// @copydetails Widget::disableWrite
        void disableWrite();
    public:
        /** 移动开始事件
        @version NIIEngine 3.0.0
        */
        static const EventID MoveBeginEvent;

        /** 移动结束事件
        @version NIIEngine 3.0.0
        */
        static const EventID MoveEndEvent;

        /** 位置改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID PosChangeEvent;
        
        /** 事件总数
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;

		/**
		@version NIIEngine 3.0.0
		*/
		static const PropertyID PosEventProperty;

		/**
		@version NIIEngine 3.0.0
		*/
		static const PropertyID YRangeProperty;

		/**
		@version NIIEngine 3.0.0
		*/
		static const PropertyID XRangeProperty;

		/**
		@version NIIEngine 3.0.0
		*/
		static const PropertyID YMoveProperty;

		/**
		@version NIIEngine 3.0.0
		*/
		static const PropertyID XMoveProperty;

		/**
		@version NIIEngine 3.0.0
		*/
		static const PropertyID PropertyCount;
    protected:
        Vector2f mDragPos;  ///<
        NIIf mVMin;         ///<
        NIIf mVMax;         ///<
        NIIf mHMin;         ///<
        NIIf mHMax;	        ///<
        bool mSynEvent;     ///< 是否同步位置变化事件
        bool mDrag;         ///< 
        bool mYMove;	    ///< 
        bool mXMove;        ///<
    };
}
}
#endif