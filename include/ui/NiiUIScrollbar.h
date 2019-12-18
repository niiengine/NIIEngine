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
#ifndef _NII_UI_SCROLLBAR_H_
#define _NII_UI_SCROLLBAR_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"

namespace NII
{
namespace UI
{
    /** 滚动条
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Scrollbar : public Container
    {
    public:
        Scrollbar(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
			LangID lid = N_PrimaryLang);
        ~Scrollbar();

        /** 设置总页大小
        @version NIIEngine 3.0.0
        */
        void setDocSize(NIIf size);

        /** 获取总页大小
        @version NIIEngine 3.0.0
        */
        NIIf getDocSize() const;

        /** 设置按键步大小
        @version NIIEngine 3.0.0
        */
        void setStepSize(NIIf size);
        
        /** 获取按键步大小
        @version NIIEngine 3.0.0
        */
        NIIf getStepSize() const;

        /** 设置页大小
        @version NIIEngine 3.0.0
        */
        void setPageSize(NIIf size);

        /** 获取页大小
        @version NIIEngine 3.0.0
        */
        NIIf getPageSize() const;
        
        /** 设置页余大小
        @version NIIEngine 3.0.0
        */
        void setOverlapSize(NIIf size);
        
        /** 获取页余大小
        @version NIIEngine 3.0.0
        */
        NIIf getOverlapSize() const;

        /** 设置当前位置
        @param[in] pos [0 <= pos <= doc - page]
        @version NIIEngine 3.0.0
        */
        void setCurrent(NIIf pos);
        
        /** 获取当前位置
        @param[in] pos [0 <= pos <= doc - page]
        @version NIIEngine 3.0.0
        */
        NIIf getCurrent() const;

        /** 设置单位化当前位置
        @param[in] upos [0, 1]
        @version NIIEngine 3.0.0
        */
        void setCurrentUnit(NIIf upos);        
        
        /** 获取单位化当前位置
        @param[in] upos [0, 1]
        @version NIIEngine 3.0.0
        */
        NIIf getCurrentUnit() const;

        /** 减向步进
        @version NIIEngine 3.0.0
        */
        void negStep();
        
        /** 加向步进
        @version NIIEngine 3.0.0
        */
        void posStep();

        /** 减向页进
        @version NIIEngine 3.0.0
        */
        void negPage();
        
        /** 加向页进
        @version NIIEngine 3.0.0
        */
        void posPage();
        
        /** 获取位置指标
        @version NIIEngine 3.0.0
        */
        PosButton * getThumb() const;

        /** 获取减向按纽
        @version NIIEngine 3.0.0
        */
        PushButton * getNegative() const;
        
        /** 获取加向按纽
        @version NIIEngine 3.0.0
        */
        PushButton * getPositive() const;

        /** 设置是否自动末尾
        @version NIIEngine 3.0.0
        */
        void setAutoEndPos(bool b);

        /** 获取是否自动末尾
        @version NIIEngine 3.0.0
        */
        bool isAutoEndPos() const;
        
        /** 综合设置
        @version NIIEngine 3.0.0
        */
        void setConfig(NIIf doc, NIIf page, NIIf step, NIIf pageremain, NIIf pos);
    protected:
        /// @copydetails Container::init
        void init();

        /// @copydetails PropertyCmdObj::init
        bool init(PropertyCmdSet * dest);

        /** 设置当前位置
        @version NIIEngine 3.0.0
         */
        bool setCurrentImpl(NIIf pos);

        /** 获取最大位置
        @version NIIEngine 3.0.0
        */
        NIIf getCurrentMax() const;
        
        /** 是否处于末尾位置
        @version NIIEngine 3.0.0
        */
        bool isCurrentEnd() const;

        /** 位置指标函数
        @version NIIEngine 3.0.0 高级api
        */
        void ThumbMoveMFunc(const EventArgs * e);

        /** 位置指标函数
        @version NIIEngine 3.0.0 高级api
        */
        void PositiveMFunc(const EventArgs * e);

        /** 位置指标函数
        @version NIIEngine 3.0.0 高级api
        */
        void NegativeMFunc(const EventArgs * e);

        /** 位置指标函数
        @version NIIEngine 3.0.0 高级api
        */
        void ThumbBeginMFunc(const EventArgs * e);

        /** 位置指标函数
        @version NIIEngine 3.0.0 高级api
        */
        void ThumbEndMFunc(const EventArgs * e);

        /// @copydetail Widget::check
        virtual bool check(const WidgetModel * style) const;

        /** 当前位置变化时触发
        @version NIIEngine 3.0.0
        */
        virtual void onCurrentChange(const WidgetEventArgs * arg);

        /** 位置指标开始移动时触发
        @version NIIEngine 3.0.0
        */
        virtual void onTumbMoveBegin(const WidgetEventArgs * arg);

        /** 位置指标结束移动时触发
        @version NIIEngine 3.0.0
        */
        virtual void onThumbMoveEnd(const WidgetEventArgs * arg);

        /** 配置发生变化时触发
        @version NIIEngine 3.0.0
        */
        virtual void onConfigChange(const WidgetEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorDrag
        virtual void onCursorDrag(const CursorEventArgs * arg);

        /// @copydetails Widget::disableWrite
        void disableWrite();
    public:
		/** 
		@version NIIEngine 3.0.0
		*/
		static const PropertyID DocSizeProperty;

		/**
		@version NIIEngine 3.0.0
		*/
		static const PropertyID PageSizeProperty;

		/**
		@version NIIEngine 3.0.0
		*/
		static const PropertyID StepSizeProperty;

		/**
		@version NIIEngine 3.0.0
		*/
		static const PropertyID OverlapSizeProperty;

		/**
		@version NIIEngine 3.0.0
		*/
		static const PropertyID CurrentProperty;

		/**
		@version NIIEngine 3.0.0
		*/
		static const PropertyID PropertyCount;

        /** 当前位置变化时
        @version NIIEngine 3.0.0
        */
        static const EventID CurrentChageEvent;

        /** 位置指标开始移动时
        @version NIIEngine 3.0.0
        */
        static const EventID ThumbMoveBeginEvent;

        /** 位置指标结束移动时
        @version NIIEngine 3.0.0
        */
        static const EventID ThumbMoveEndEvent;

        /** 配置发生变化时
        @version NIIEngine 3.0.0
        */
        static const EventID ConfigChangeEvent;

        /** 事件总数
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        PosButton * mThumb;
        PushButton * mNegative;
        PushButton * mPositive;
        NIIf mDocSize;
        NIIf mPageSize;
        NIIf mStepSize;
        NIIf mPageRemian;
        NIIf mCurrentPos;
        bool mAutoEnd;
    };
}
}
#endif