/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-9-8

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

#ifndef _NII_UI_TOOLTIP_WIDGET_AIDE_H_
#define _NII_UI_TOOLTIP_WIDGET_AIDE_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidgetAide.h"

namespace NII
{
namespace UI
{
    /** UI提示辅助
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TooltipWidgetAide : public WidgetAide
    {
    public:
        TooltipWidgetAide(Widget * dst, LangID lid = N_PrimaryLang);
        ~TooltipWidgetAide();

        /** 设置提示文本
        @param[in] txt 提示文本
        @version NIIEngine 3.0.0
        */
        void setTooltipText(const String & txt);

        /** 获取提示文本
        @version NIIEngine 3.0.0
        */
        const String & getTooltipText() const;

        /** 设置提示对象类型
        @param type 已经注册的Tooltip类类型
        @version NIIEngine 3.0.0
        */
        void setTooltipType(StyleID type);

        /** 获取提示对象类型
        @return 已经注册的Tooltip类类型
        @version NIIEngine 3.0.0
        */
        StyleID getTooltipType() const;

        /** 设置提示对象
        @param obj Tooltip对象
        @version NIIEngine 3.0.0
        */
        void setTooltip(Tooltip * obj);

        /** 获取提示对象
        @return Tooltip对象
        @version NIIEngine 3.0.0
        */
        Tooltip * getTooltip() const;

        /** 是否使用默认的提示对象
        @version NIIEngine 3.0.0
        */
        bool isDefault() const;

        /** 设置是否使用父对象提示文本
        @version NIIEngine 3.0.0
        */
        void setParentTooltipText(bool b);

        /** 获取是否使用父对象提示文本
        @return NIIEngine 3.0.0
        */
        bool isParentTooltipText() const;
    protected:
        /// @copydetails PropertyCmdObj::init
        virtual bool init(PropertyCmdSet * dst);
    protected:
        /// @copydetails WidgetAide::onCursorEnter
        void onCursorEnter(const CursorEventArgs * arg);

        /// @copydetails WidgetAide::onCursorLeave
        void onCursorLeave(const CursorEventArgs * arg);

        /// @copydetails WidgetAide::onCursorMove
        void onCursorMove(const CursorEventArgs * arg);

        /// @copydetails WidgetAide::onButtonDown
        void onButtonDown(const CursorEventArgs * arg);
    protected:
        String mTipText;        ///< 提示文本
        mutable StyleID mType;  ///< Tooltip类型
        Tooltip * mCustomTip;   ///< 自定义提示窗体对象
        bool mGenTip;           ///< 自定义对象
        bool mInheritTipText;   ///< 使用父对象文本
    };
}
}
#endif