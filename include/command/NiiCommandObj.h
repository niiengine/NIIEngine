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

#ifndef _NII_COMMANDOBJECT_H_
#define _NII_COMMANDOBJECT_H_

#include "NiiPreInclude.h"
#include "NiiEvent.h"

namespace NII
{
namespace NII_COMMAND
{
    /** 可以发出信号的对象
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CommandObj
    {
    public:
        typedef vector<Event *>::type Events;
        typedef map<EventID, Event *>::type ExtEvents;
    public:
        CommandObj();
        virtual ~CommandObj();

        /** 触发事件信号
        @param[in] id
        @param[in] arg
        @version NIIEngine 3.0.0
        */
        virtual void signal(EventID id, const EventArgs * arg);

        /** 添加事件集
        @param[in] id 事件
        @version NIIEngine 3.0.0
        */
        virtual Event * addEvent(EventID eid);

        /** 移去事件集
        @param[in] id 事件
        @version NIIEngine 3.0.0
        */
        virtual void removeEvent(EventID eid);

        /** 添加事件集处理函子
        @remark
        @param[in] id 事件
        @param[in] slot 处理函子
        @version NIIEngine 3.0.0
        */
        virtual SignalPtr addEvent(EventID id, EventFunctor * slot);

        /** 移去事件集处理函子
        @remark
        @param[in] id 事件
        @param[in] slot 事件函子
        @version NIIEngine 3.0.0
        */
        virtual void removeEvent(EventID id, EventFunctor * slot);

        /** 获取事件集
        @param[in]
        @param[in]
        @version NIIEngine 3.0.0
        */
        Event * getEvent(EventID id, bool add = false);

        /** 添加脚本
        @param smid 脚本模块ID
        @param id 事件ID
        @param fc 脚本函数
        @version NIIEngine 3.0.0
        */
        virtual SignalPtr addScript(ScriptModuleID smid, EventID id, const String & fc);

        /** 设置是否激发信号
        @remark 第二参数将使状态不可变
        @param[in] s 是否
        @param[in] r 移去当前所有事件
        @version NIIEngine 3.0.0
        */
        void setSignalEnable(bool s, bool r = false);

        /** 获取是否激发信号
        @version NIIEngine 3.0.0
        */
        bool isSignalEnable() const;
        
        /** 返回这个对象是否还有效
        @remark 当这个对象中没有任何参子的时候,就会无效
        @version NIIEngine 3.0.0
        */
        bool isInvalid() const;
    public:
        /** 对象通告有数据更新了
        @remark 对象的任何数据改变了都会触发这个事件
        @version NIIEngine 3.0.0
        */
        static const EventID NotifyEvent;

        /** 父对象状态改变的时候触发
        @remark 强调的是状态
        @version NIIEngine 3.0.0
        */
        static const EventID ParentNotifyEvent;

        /** 子对象状态改变的时候触发
        @remark 强调的是状态
        @version NIIEngine 3.0.0
        */
        static const EventID ChildNotifyEvent;

        /** 对象存在关联
        @remark 也就是说这个对象属于任意一个管理器所管理中
        @version NIIEngine 3.0.0
        */
        static const EventID MapEvent;

        /** 对象无任何关联
        @remark 也就是说这不是通过任何一个管理器生产出来的,而是一个临时实例,或直接 N_new 出来后没有把它加到任何管理器类中
        @version NIIEngine 3.0.0
        */
        static const EventID UnMapEvent;

        /// 事件总数
        static const EventID EventCount;
    protected:
        /** 对象通告有数据更新了
        @remark 对象的任何数据改变了都会触发这个事件
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onNotify(const EventArgs * arg);

        /** 父对象状态改变的时候触发
        @remark 强调的是状态
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onParentNotify(const EventArgs * arg);

        /** 子对象状态改变的时候触发
        @remark 强调的是状态
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onChildNotify(const EventArgs * arg);

        /** 对象存在关联
        @remark 也就是说这个对象属于任意一个管理器所管理中
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onMap(const EventArgs * arg);

        /** 对象无任何关联
        @remark
            也就是说这不是通过任何一个管理器生产出来的,而是一个临时实例,或直接 N_new 出来后没有把它加到任何管理器类中
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onUnMap(const EventArgs * arg);
    protected:
        /** 设置事件信号总个数
        @remark
            设置事件信号总个数,一般是所有继承级别的基类的EvenCount和自身的EventCount相加
        @param[in] count 事件数量
        @version NIIEngine 3.0.0
        */
        void notifySignal(NCount count);
    protected:
        Events mEvents;         ///< 事件集
        BitSet * mEventMarks;   ///< 事件集掩码
        bool mEnable;           ///< 是否启用
    };
}
}
#endif