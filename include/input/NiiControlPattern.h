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

#ifndef _NII_CONTROL_PATTERN_H_
#define _NII_CONTROL_PATTERN_H_

#include "NiiPreInclude.h"
#include "NiiPattern.h"
#include "NiiSerialItem.h"

namespace NII
{
    /** 控制器类型
    @version NIIEngine 3.0.0
    */
        enum ControlDevType
        {
            CDT_Keyboard = 1,
            CDT_Mouse = 2,
            CDT_JoyPad = 3,
            CDT_JoyHandle = 4,
            CDT_JoyWheel = 5,
            CDT_Touch = 6
        };
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
        friend class InputManager;
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

    /** 控制器
    @remark 子类必须依赖具体的控制单元实现虚方法
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ControlPattern : public Pattern
    {
        friend class InputManager;
    public:
        typedef vector<ControlItem *>::type ItemList;
    public:
        /// 返回输入设备器的类型
        virtual ControlDevType getType() const = 0;
    protected:
        ControlPattern();

        /** 使用ControlPatternManger,而不直接使用这个函数
        @param[in] id 同类型设备中必须是唯一的
        @param[in] own 所附属在的对象
        @param[in] type 输入设备器的类型
        */
        ControlPattern(Nid id, const ViewWindow * own);

        virtual ~ControlPattern();

        /** 设置设备的供应商
        @param[in] v 供应商的名字
        @version NIIEngine 3.0.0
        */
        void setVendor(const String & v);

        /** 绑定控制窗口
        @remark 内部函数
        @param[in] win 需要绑定的控制窗口
        @version NIIEngine 3.0.0
        */
        virtual void bind(const ViewWindow * win);

        /** 应用控制单元
        @remark 内部函数
        @param[in] item 需要应用的控制单元
        @version NIIEngine 3.0.0
        */
        virtual void apply(ControlItem * item) = 0;

        /** 解除当前控制单元的绑定
        @remark 内部函数
        @version NIIEngine 3.0.0
        */
        virtual void detach() = 0;

        /** 重至控制器
        @remark 主线程程中的
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _reset() = 0;

        /** 开始运行控制器
        @remark 主线程程中的
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _start() = 0;

        /** 暂停运行控制器
        @remark 主线程程中的
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _pause() = 0;

        /** 停止运行控制器
        @remark 主线程程中的
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _stop() = 0;
    protected:
        const ViewWindow * mOwn;///< 所附属在的对象
        Nid mID;                ///< 有多个同类型的输入设备器时很有用
        String mVendor;         ///< 供应商(设备生产商)信息
        bool mActive;           ///< 设备器是否活动
    };
    
    class _EngineAPI QueueControlPattern : public ControlPattern
    {
    protected:
        QueueControlPattern();
        ~QueueControlPattern();
    };
}
}
#endif