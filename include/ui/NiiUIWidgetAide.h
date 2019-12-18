/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-8

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

#ifndef _NII_UI_WIDGET_AIDE_H_
#define _NII_UI_WIDGET_AIDE_H_

#include "NiiUIPreInclude.h"

namespace NII
{
namespace UI
{
    /** UI单元辅助
    @version NIIEngine 3.0.0
    */
    class _EngineAPI WidgetAide : public PropertyCmdObj
    {
    public:
        WidgetAide(Widget * dst, ScriptTypeID stid, LangID lid = N_PrimaryLang) :
			PropertyCmdObj(stid, lid),
			mCurrent(dst){}
        virtual ~WidgetAide() {}
    protected:
        /** 当游标进入时触发
        @remark 仅在状态交换时触发
        @param[in] arg
        @version NIIEngine 3.0.0
        */
		virtual void onCursorEnter(const CursorEventArgs * arg) {}
        
        /** 当游标离开时触发
        @remark 仅在状态交换时触发
        @remark 仅在
        @param[in] arg
        @version NIIEngine 3.0.0
        */
		virtual void onCursorLeave(const CursorEventArgs * arg) {}
        
        /** 当游标移动时触发(进入状态下)
        @param[in] arg
        @version NIIEngine 3.0.0
        */
		virtual void onCursorMove(const CursorEventArgs * arg) {}
        
        /** 当游标拖拽时触发(按下状态)
        @param[in] arg
        @version NIIEngine 3.0.0
        */
		virtual void onCursorDrag(const CursorEventArgs * arg) {}
        
        /** 当游标按键释放时触发(进入状态下)
        @remark 仅在状态交换时触发(另一状态则为按键按下)
        @param[in] arg
        @version NIIEngine 3.0.0
        */
		virtual void onButtonDown(const CursorEventArgs * arg) {}
        
        /** 当游标按键按下时触发(进入状态下)
        @remark 仅在状态交换时触发(另一状态则为按键释放)
        @param[in] arg
        @version NIIEngine 3.0.0
        */
		virtual void onButtonUp(const CursorEventArgs * arg) {}
    protected:
        WidgetAide() : PropertyCmdObj(5001, N_PrimaryLang){}
    protected:
        UISheet * mSheet;
		Widget * mCurrent;
    };
}
}
#endif