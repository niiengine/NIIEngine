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
#ifndef _NII_WIDGET_STYLE_H_
#define _NII_WIDGET_STYLE_H_

#include "NiiUIPreInclude.h"
#include "NiiUICommon.h"

namespace NII
{
namespace UI
{
    /** UI渲染样式
    @version NIIEngine 3.0.0
    */
    class _EngineAPI WidgetModel : public UIAlloc
    {
        friend class Widget;
    public:
        WidgetModel(WidgetModelID wsid, WidgetModelID type);
        virtual ~WidgetModel();

        /** UI渲染形式ID
        @version NIIEngine 3.0.0
        */
        WidgetModelID getID() const;

        /** UI渲染形式类型
        @version NIIEngine 3.0.0
        */
        WidgetModelID getType() const;

        /** 更新
        @param[in] cost 上次调用和这次度过的事件
        @version NIIEngine 3.0.0
        */
        virtual void update(TimeDurMS cost);

        /** 把UI冲刷到缓存中
        @version NIIEngine 3.0.0
        */
        virtual void flush() = 0;

        /** 执行样式排版
        @version NIIEngine 3.0.0
        */
        virtual void layout();

        /** 获取排版区域
        @version NIIEngine 3.0.0
        */
        virtual Rectf getArea() const;

        /** 获取使用的风格
        @version NIIEngine 3.0.0
        */
		Style * getStyle() const;
        
        /** 获取实际画盘
        @version NIIEngine 3.0.0
        */
        virtual void getContext(SheetContext *& ctx) const;
    protected:
        /** 附加属性
        @param pcd 属性
        @param serl 是否可序列化
        */
        void add(PropertyCmd * pcd, bool serl = true);

        /** 附加到UI单元时触发
        @version NIIEngine 3.0.0
        */
        virtual void onAttach();

        /** 从UI单元中解除时触发
        @version NIIEngine 3.0.0
        */
        virtual void onDetach();

        /** 附加风格时触发
        @version NIIEngine 3.0.0
        */
        virtual void onStyleAttach();

        /** 解除风格时触发
        @version NIIEngine 3.0.0
        */
        virtual void onStyleDetach();

        /** 字体改变时触发
        @param[in] obj
        @verison NIIEngine 3.0.0
        */
        virtual bool notifyFont(const Font * obj);
    public:
        /** enable/启用 标签中的元素
        @version NIIEngine 3.0.0
        */
        static const StateID EnableState;

        /** disable/禁用 标签中的元素
        @version NIIEngine 3.0.0
        */
        static const StateID DisableState;

        /** 状态总数
        @version NIIEngine 3.0.0
        */
        static const StateID StateCount;
    protected:
        WidgetModel & operator=(const WidgetModel &);
    protected:
        typedef vector<std::pair<PropertyCmd *, bool> >::type PropertyCmdList;
    protected:
        Widget * mSrc;
        WidgetModelID mID;
        WidgetModelID mType;
        PropertyCmdList mCmdList;
    };
}
}
#endif