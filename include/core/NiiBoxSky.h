/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-3

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

#ifndef _NII_BOX_SKY_H_
#define _NII_BOX_SKY_H_

#include "NiiPreInclude.h"
#include "NiiSky.h"

namespace NII
{
    /** 盒天空
    @version NIIEngine 3.0.0
    */
    class _EngineAPI BoxSky : public Sky
    {
    public:
        BoxSky();
        BoxSky(const SpaceManager * gm);
        virtual ~BoxSky();
        
        /** 设置材质
        @param[in] rid 资源ID
        @version NIIEngine 3.0.0
        */
        void setMaterial(ResourceID rid);
        
        /** 设置方向偏量
        @version NIIEngine 3.0.0
        */
        void setOriOft(const Quaternion & o);
        
        /** 获取方向偏量
        @version NIIEngine 3.0.0
        */
        const Quaternion & getOriOft() const;
        
        /** 构建几何
        @version NIIEngine 3.0.0
        */
        void buildGeo(NIIf distance, const Quaternion & ori_oft);
    
        /** 获取与视口的距离
        @version NIIEngine 3.0.0
        */
        NIIf getRange() const;
    protected:
        /// @copydetails BoxSky::renderImpl
        virtual void renderImpl(RenderQueue * rq);
    protected:
        NIIf mRange;
        CustomSpaceObj * mGeo;
        Quaternion mOriOft;
        ResourceID mMaterial[7];
    };
}
#endif