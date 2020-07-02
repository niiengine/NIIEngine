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

#ifndef _NII_CONTROLITEM_H_
#define _NII_CONTROLITEM_H_

#include "NiiPreInclude.h"
#include "NiiControlDevType.h"
#include "NiiSerialItem.h"

namespace NII
{
namespace NII_MEDIA
{
    /** 序列参数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ControlSerialArgs : public EventArgs
    {
    public:
        ControlSerialArgs(Nid id);
        virtual ~ControlSerialArgs();

        Nid mSerialID;      ///< 当前激活的序列ID
    };
    
    /** 输入设备的控制单元类
    @remark 添加序列会影响运行性能
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ControlItem
    {
        friend class ControlItemManager;
        friend class ControlPatternManager;
        friend class ControlPattern;
    public:
        /// 序列状态映射
        typedef map<Nui32, SerialItem *>::type SerialStatusMap;
    public:
        ControlItem(bool includeRelease = false);

        virtual ~ControlItem();

        /** 设置是否启用序列监听
        @param[in] b 一个布尔
        @version NIIEngine 3.0.0
        */
        inline void setSerial(bool b) { mSerial = b; }

        /** 是否启用序列监听
        @return 一个布尔
        @version NIIEngine 3.0.0
        */
        inline bool getSerial() const { return mSerial; }

        /** 设置控制器
        @version NIIEngine 3.0.0
        */
        inline void setInfluence(ControlPattern * cp) { mInfluence = cp; }
        
        /** 获取控制器
        @version NIIEngine 3.0.0
        */
        inline ControlPattern * getInfluence() const { return mInfluence; }

        /** 添加一个条件序列
        @remark
            序列中的按键是分按下和释放,但由于在分析和添加序列的时候比较复杂,所以
            序列是不应该和不能包含释放按键的序列单元
        @param[in] id 条件序列ID
        @param[in] item 序列
        @param[in] func 执行的函子槽
        */
        void add(Nui32 id, SerialItem * item);

        /** 移去一个按键序列
        @param[in] id 条件序列ID
        */
        void remove(Nui32 id);

        /// 返回当前的控制单元类型
        virtual ControlDevType getType() const = 0;
    protected:
        /** 引发键记录
        @param[in] arg 所引发按键事件参数
        @return 返回false则中断余下的缓存处理
        */
        void raise(const EventArgs * arg);

        /** 当按键序列成立的时候触发
        @remark 可以重写这个函数去实现输入捕捉
        @param[in] arg
        */
        virtual void onSerial(const ControlSerialArgs * arg);
    protected:
        Nid mID;                            ///< 这个控制单元可以共享
        SerialStatusMap mSerialStatuses;    ///< 序列状态
        ControlPattern * mInfluence;        ///< 当前影响对这个控制单元的控制器
        bool mSerial;                       ///< 是否启用序列
    };
    
    class _EngineAPI QueueControlItem : public ControlItem
    {
    public:
        QueueControlItem(bool includeRelease = false);
        ~QueueControlItem();
    protected:
        /** 引发键记录
        @param[in] arg 所引发按键事件参数
        @return 返回false则中断余下的缓存处理
        */
        void raise(const EventArgs * arg);

        /** 当按键序列成立的时候触发
        @remark 可以重写这个函数去实现输入捕捉
        @param[in] arg
        */
        virtual void onSerial(const ControlSerialArgs * arg);
    };
}
}
#endif