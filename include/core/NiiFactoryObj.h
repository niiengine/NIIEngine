/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-2

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_FACTORYOBJ_H_
#define _NII_FACTORYOBJ_H_

#include "NiiPreInclude.h"
#include "NiiString.h"

namespace NII
{
    /** 工场类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FactoryObj : public FactoryAlloc
    {
    public:
        virtual ~FactoryObj();

        /** 获取ID
        @version NIIIEngine 3.0.0
        */
        virtual FactoryID getID() const = 0;
        
        /** 获取虚拟文件类型
        @return VFSType 组合
        @version NIIEngine 3.0.0 高级api
        */
        virtual Nmark getVFSType() const;

        /** 从(虚拟)文件系统创建
        @note 如果子类不实现,将返回0
        @version NIIEngine 3.0.0
        */
        virtual void * create(const String & file);

        /** 删除对象
        @version NIIEngine 3.0.0
        */
        virtual void destroy(void * obj) = 0;
    };
}
#endif