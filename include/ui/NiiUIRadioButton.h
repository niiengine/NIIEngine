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

#ifndef _NII_UI_RADIOBUTTON_H_
#define _NII_UI_RADIOBUTTON_H_

#include "NiiUIPreInclude.h"
#include "NiiUIButton.h"

namespace NII
{
namespace UI
{
    /** 单选按钮
    @remark 用于描述同一群组中的仅单一选中的按钮群
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RadioButton : public Button
    {
    public:
        RadioButton(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank, 
			LangID lid = N_PrimaryLang);

        /** 设置所属的群组
        @param[in] gid
        @version NIIEngine 3.0.0
        */
        void setGroup(GroupID gid);

        /** 获取所属的群组
        @version NIIEngine 3.0.0
        */
        GroupID getGroup() const;

        /** 获取所属的群组中正被选中的单选按钮
        @version NIIEngine 3.0.0
        */
        RadioButton * getGroupSelect() const;
    protected:
        /// @copydetails PropertyCmdObj::init
        virtual bool init(PropertyCmdSet * dest);

		// @copydetails Button::onSelectChange
		void onSelectChange(const WidgetEventArgs * arg);
    protected:
        void verify() const;
    protected:
        GroupID mGID;
    };
}
}
#endif