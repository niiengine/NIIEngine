/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-7

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

#ifndef _NII_UI_ProgressBar_H_
#define _NII_UI_ProgressBar_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidget.h"

namespace NII
{
namespace UI
{
    /** 处理进度
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ProgressBar : public Widget
    {
    public:
        ProgressBar(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
			LangID lid = N_PrimaryLang);
        virtual ~ProgressBar();

        /** 按步进自动更新
        @version NIIEngine 3.0.0
        */
        void step();

        /** 调整当前进度
        @version NIIEngine 3.0.0
        */
        void adjust(NIIf delta);

        /** 设置当前进度
        @version NIIEngine 3.0.0
        */
        void setProgress(NIIf p);

        /** 获取当前进度
        @version NIIEngine 3.0.0
        */
        NIIf getProgress() const;

        /** 设置步进
        @version NIIEngine 3.0.0
        */
        void setStep(NIIf p);

        /** 设置步进
        @version NIIEngine 3.0.0
        */
        NIIf getStep() const;
    protected:
        /// @copydetials PropertyCmdObj::init
        bool init(PropertyCmdSet * dest);

        /** 进度变化时触发
        @version NIIEngine 3.0.0
        */
        virtual	void onPrcChange(const WidgetEventArgs * arg);

        /** 进度变化时触发
        @version NIIEngine 3.0.0
        */
        virtual void onPrcDone(const WidgetEventArgs * arg);
    public:
        /** 进度开始事件
        @version NIIEngine 3.0.0
        */
        static const EventID PrcStartEvent;

        /** 进度变化事件
        @version NIIEngine 3.0.0
        */
        static const EventID PrcChangeEvent;

        /** 进度完成事件
        @version NIIEngine 3.0.0
        */
        static const EventID PrcDoneEvent;

        /// 事件数量
		static const EventID EventCount;
    protected:
        NIIf mProgress;
        NIIf mStep;
    };
}
}
#endif