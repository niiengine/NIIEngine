/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

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

#ifndef _NII_MEMBER_FUNCTOR_H_
#define _NII_MEMBER_FUNCTOR_H_

#include "NiiPreInclude.h"
#include "NiiFunctor.h"
#include "NiiCondition.h"

namespace NII
{
namespace NII_COMMAND
{
    /** 成员触发函数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MemberFunctor : public Functor
    {
    public:
        typedef vector<Condition::ConditionParam *>::type DestList;
    public:
        MemberFunctor(EventObj * own);
        virtual ~MemberFunctor();

        /** 添加一个参子
        @remark 这个参子和这个方法是同类型匹配的
        @param[in] factor
        @version NIIEngine 3.0.0
        */
        void add(Condition::ConditionParam * factor);

        /** 移去一个参子
        @remark 这个参子和这个方法是同类型匹配的
        @param[in] factor
        @version NIIEngine 3.0.0
        */
        void remove(Condition::ConditionParam * factor);

        /** 设置是否启动这个方法函数
        @remark 如果不启动的话,不会监听参子的状态
        @version NIIEngine 3.0.0
        */
        void setEnable(bool s);
        
        /** 设置是否启动这个方法函数
        @remark 如果不启动的话,不会监听参子的状态
        @version NIIEngine 3.0.0
        */
        bool getEnable() const;
        
        /** 返回这个对象是否还有效
        @remark 当这个对象中没有任何条件参子的时候,就会无效
        @return 返回一个布尔,标识是否无效
        @version NIIEngine 3.0.0
        */
        bool isInvalid() const;
        
        /// @copydetails Functor::execute
        bool execute(const EventArgs * args);
    protected:
        /** 这个方法处理比较,这个函数不会改变对象成员属性,但会改变条件参子达成状态
        @remark 参数mid所对应的函数必须和参数ex所配对
        @param[in] mid 参子数据入口ID
        @param[in] cp 比较类型
        @param[in] ex 需要去比较的数值
        @return 一个布尔,比较成立则返回true,否则返回false
        @version NIIEngine 3.0.0 高级api
        */
        virtual bool process(CmpMode cp, const EventArgs * args) = 0;

        /** 效验条件单元是否成立,这个函数将会改变目标的成员属性
        @remark 参数mid所对应的函数必须和参数arg所配对
        @param[in] mid 所引发的处理函数
        @param[in] arg 传输到处理函数的参数
        @return 一个布尔,表示内部的条件成立状态是否有改变
        @version NIIEngine 3.0.0 高级api
        */
        virtual bool check(const EventArgs * arg) = 0;
    protected:
        EventObj * mOwn;		///< 所属的可命令对象
        DestList mDestList;		///< 
        bool mEnable;			///< 
    };
}
}
#endif