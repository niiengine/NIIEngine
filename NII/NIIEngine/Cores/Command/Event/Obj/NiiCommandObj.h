/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
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
        @version NIIEngine 3.0.0
        */
        virtual void signal(EventID id, const EventArgs * arg);

        /** 添加事件集
        @param[in] id 事件
        @version NIIEngine 3.0.0
        */
        virtual Event * bind(EventID eid);

        /** 移去事件集
        @version NIIEngine 3.0.0
        */
        virtual void unbind(EventID eid);

        /** 添加事件集处理函子
        @version NIIEngine 3.0.0
        */
        virtual SignalPtr bind(EventID id, EventFunctor * func);

        /** 移去事件集处理函子
        @version NIIEngine 3.0.0
        */
        virtual void unbind(EventID id, EventFunctor * func);

		/** 添加事件集处理函子
		@version NIIEngine 3.0.0
		*/
		inline SignalPtr bind(EventID id, Event * event, EventMethod em)
		{
			EventFunctor * ef = N_new EventFunctor(event, em);
			return bind(id, ef);
		}

		/** 移去事件集处理函子
		@version NIIEngine 3.0.0
		*/
		inline void unbind(EventID id, Event * event, EventMethod em)
		{
			EventFunctor ef(event, em);
			unbind(id, &ef);
		}

		/** 添加事件集处理函子
		@version NIIEngine 3.0.0
		*/
		inline SignalPtr bind(EventID id, CommandObj * obj, CommandObjFunctor mf)
		{
			EventFunctor * ef = N_new EventFunctor(obj, mf);
			return bind(id, ef);
		}

		/** 移去事件集处理函子
		@version NIIEngine 3.0.0
		*/
		inline void unbind(EventID id, CommandObj * obj, CommandObjFunctor mf)
		{
			EventFunctor ef(obj, mf);
			unbind(id, &ef);
		}

        /** 添加事件集处理函子
        @version NIIEngine 3.0.0
        */
		template <typename _class, typename _earg> 
			inline SignalPtr bind(EventID id, _class * obj, void (_class::*method)(const _earg *))
		{
			ObjFunctor<_class, _earg> * of = N_new ObjFunctor<_class, _earg>(obj, method);
			EventFunctor * ef = N_new EventFunctor(of);
			return bind(id, ef);
		}

        /** 移去事件集处理函子
        @version NIIEngine 3.0.0
        */
		template <typename _class, typename _earg> 
			inline void unbind(EventID id, _class * obj, void (_class::*method)(const _earg *))
		{
			ObjFunctor<_class, _earg> * of = N_new ObjFunctor<_class, _earg>(obj, method);
			EventFunctor ef(of);
			unbind(id, &ef);
		}

        /** 获取事件集
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