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

#ifndef _NII_CONTROL_PATTERN_MANAGER_H_
#define _NII_CONTROL_PATTERN_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiControlPattern.h"

namespace NII
{
    /** 输入设备的控制器的管理器类
    @remark 所有的输入性设备控制器都应该由这个类所创建
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ControlPatternManager : public Singleton<ControlPatternManager>,
        public PatternAlloc
    {
    public:
        typedef vector<NII_MEDIA::ControlPattern *>::type ControlList;
        typedef map<const ViewWindow *, ControlList>::type WindowControlList;
    public:
        ControlPatternManager();
        virtual ~ControlPatternManager();

        /** 创建一个控制处理器
        @param[in] main 这个控制器的主控窗集体
        @return 一个键盘处理器
        @version NIIEngine 3.0.0
        */
        NII_MEDIA::ControlPattern * create(const ViewWindow * main, ControlDevType type);

        /** 获取一个有效的键盘控制器
        @param[in] main 哪个窗体
        @param[in] type 输入控制器类型
        @param[in] id 控制器的ID
        @version NIIEngine 3.0.0
        */
        NII_MEDIA::ControlPattern * get(const ViewWindow * main, ControlDevType type, Nid id) const;

        /** 回收一个控制处理器
        @param[in] main 哪个窗体
        @param[in] dest 回收的控制器
        @version NIIEngine 3.0.0
        */
        virtual void retrieve(const ViewWindow * main, NII_MEDIA::ControlPattern * dest);

        /** 挂起窗体控制设备
        @param[in] main 窗体
        @version NIIEngine 3.0.0
        */
        void suspend(const ViewWindow * main);

        /** 激活窗体控制设备
        @param[in] main 窗体
        @version NIIEngine 3.0.0
        */
        void active(const ViewWindow * main);

        /** 应用设备控制单元
        @remark 这个方法必须把握住类型的正确性,否则肯定会出现应用程序异常
        @param[in] main 需要应用的控制单元所属的窗体
        @param[in] item 需要应用的控制单元
        @version NIIEngine 3.0.0
        */
        void apply(const ViewWindow * main, NII_MEDIA::ControlItem * item);

        /** 获取一个窗体类型设备当前的总数量
        @param[in] main 指定的窗体
        @param[in] type 指定的设备类型
        @version NIIEngine 3.0.0
        */
        NCount getCurrent(const ViewWindow * main, ControlDevType type) const;

        /** 获取设备的最大数量
        @param 相对于设备而言
        @version NIIEngine 3.0.0
        */
        virtual NCount getCount(ControlDevType type) const = 0;

        /** 初始化控制系统
        @remark 具体的实现依赖于实际使用的操作系统
        @version NIIEngine 3.0.0
        */
        virtual void init() = 0;

        /// @copydetails Singleton::getOnly
        static ControlPatternManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static ControlPatternManager * getOnlyPtr();
    public:
        ControlPatternManager * DummyPatternManager;
    protected:
        /** 创建一个键盘控制处理器
        @param[in] main 这个控制器的主控窗集体
        @param[in] type 控制器类型
        @return 一个键盘处理器
        */
        virtual NII_MEDIA::ControlPattern * createImpl(const ViewWindow * main, ControlDevType type) = 0;
    protected:
        WindowControlList mControls;        ///< 窗体(1:N)相应的控制器
    };
}
#endif