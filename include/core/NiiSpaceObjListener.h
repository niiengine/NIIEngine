/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-10-28

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

#ifndef _NII_SPACEOBJ_LISTENER_H_
#define _NII_SPACEOBJ_LISTENER_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 事件监听
    @remark 可用于物理引擎/阴影渲染器控制
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI SpaceObjListener
    {
    public:
        SpaceObjListener();
        virtual ~SpaceObjListener();

        /** 创建后时
        @version NIIEngine 3.0.0
        */
        virtual void create(SpaceObj * obj);
        
        /** 删除时
        @version NIIEngine 3.0.0
        */
        virtual void destroy(SpaceObj * obj);
        
        /** 位置发生改变
        @version NIIEngine 3.0.0
        */
        virtual void notify(SpaceObj * obj);

        /** 附加到节点时
        @note 仅在参数2非0时触发
        @version NIIEngine 3.0.0
        */
        virtual void attach(SpaceObj * obj, const PosNode * np);

        /** 从节点中分离时
        @note 仅在参数2非0时触发
        @version NIIEngine 3.0.0
        */
        virtual void detach(SpaceObj * obj, const PosNode * op);

        /** 使用指定摄像机绘制空间
        @return 是否有实际绘制效果,没有效果则过滤
        @version NIIEngine 3.0.0
        */
        virtual bool render(SpaceObj * obj, const Camera * view);

        /** 查询实际影响空间对象的灯光
        @version NIIEngine 3.0.0
        */
        virtual void query(const SpaceObj * obj, LightList & out);
    };
}

#endif