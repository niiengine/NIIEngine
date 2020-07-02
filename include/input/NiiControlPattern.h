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
#include "NiiControlDevType.h"

namespace NII
{
namespace NII_MEDIA
{
    /** 控制器
    @remark 子类必须依赖具体的控制单元实现虚方法
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ControlPattern : public Pattern
    {
        friend class ControlPatternManager;
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