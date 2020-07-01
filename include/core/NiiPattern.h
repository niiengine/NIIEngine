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

#ifndef _NII_PATTERN_H_
#define _NII_PATTERN_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 这个处理器是属于协调式插件级别
    @remark
        这样做可以减少运行时库的大小,同时而不影响其扩展,这个类仅提供部分主方法,要依
        赖自类的具体嵌入式操作的实现,而且子类必须实现这个方法的所有纯虚方法
    @par
        这个处理器是和渲染帧相对的,也就是说每更新一帧这个处理器才更新一次,这样可以有
        效的协调渲染内容和用户交互,如果你需要的是另外一个独立的工作器,查看NiiJob.h
    @note 在设计中所有成员变量都应当是描述一个临时状态
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Pattern : public PatternAlloc
    {
        friend class Engine;
    public:
        typedef vector<PatternID>::type PatternType;
    public:
        virtual ~Pattern();

        /** 获取这个处理器的处理效果(质量)
        @remark 数字越大处理效果越好
        @return [0-255]
        @version NIIEngine 3.0.0
        */
        virtual Nui8 getQuality() const;

        /** 获取这个处理器的处理速度(性能)
        @remark 数字越大处理速度越快
        @return [0-255]
        @version NIIEngine 3.0.0
        */
        virtual Nui8 getPerformance() const;
    protected:
        /** 初始化处理器
        @remark 这个方法可能会执行不成功
        @version NIIEngine 3.0.0 高级api
        */
        virtual bool _init() = 0;

        /** 结束处理器
        @remark 这个方法可能会执行不成功
        @version NIIEngine 3.0.0 高级api
        */
        virtual bool _end() = 0;
        
        /** 更新这个处理器类的内部信息
        @remark 这个方法是在渲染每帧前调用的
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _update() = 0;

        /** 启动子处理器
        @remark 启动部分N in 1效果(默认全启用)
        @par 内部有规定
        @return 是否启动成功
        @version NIIEngine 3.0.0 高级api
        */
        virtual bool on(PatternID proc);

        /** 关闭子处理器
        @remark 关闭部分N in 1效果
        @par 内部有规定
        @return 是否关闭成功
        @version NIIEngine 3.0.0 高级api
        */
        virtual bool off(PatternID proc);

        /** 获取这个处理器的ID
        @remark 唯一的,和版本无关系
        @par 内部有规定
        @version NIIEngine 3.0.0 高级api
        */
        virtual PatternID getPID() const;
    protected:
        PatternID mMark;                        ///< 处理掩码
    };
}
#endif