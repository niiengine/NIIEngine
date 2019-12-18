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

#ifndef _NII_UI_CURSOR_WIDGET_AIDE_H_
#define _NII_UI_CURSOR_WIDGET_AIDE_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidgetAide.h"

namespace NII
{
namespace UI
{
    /** 游标辅助对象
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CursorWidgetAide : public WidgetAide
    {
    public:
        CursorWidgetAide(Widget * dst, LangID lid = N_PrimaryLang);
        ~CursorWidgetAide();

        /** 鼠标图片像素
        @param[in] pb 对象
        @version NIIEngine 3.0.0
        */
        void setMouseCursor(PixelBuffer * pb);
        
        /** 获取实际的像素缓存
        @param _default
        @vesion NIIEngine 3.0.0
        */
		PixelBuffer * getMouseCursor(bool _default = true) const;
    protected:
        /// @copydetails PropertyCmdObj::init
        virtual bool init(PropertyCmdSet * dest);

        /// @copydetails WidgetAide::onCursorEnter
        void onCursorEnter(const CursorEventArgs * arg);
    protected:
		PixelBuffer * mImage;        ///< 鼠标指针图片
    };
}
}
#endif