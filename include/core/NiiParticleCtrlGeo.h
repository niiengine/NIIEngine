/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-5-7

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

#ifndef _NII_ParticleCtrlGeo_H__
#define _NII_ParticleCtrlGeo_H__

#include "NiiPreInclude.h"
#include "NiiParticleGeo.h"
#include "NiiParticleGeoFactory.h"

namespace NII
{
    /** 渲染
    @par NO 0x08
    */
    class _EngineAPI ParticleCtrlGeo : public ParticleGeo
    {
    public:
        ParticleCtrlGeo(LangID lid = N_PrimaryLang);
        ~ParticleCtrlGeo();

        /** 设置是否点动画
        @version NIIEngine 3.0.0
        */
        void setPointSprites(bool b);

        /** 获取是否点动画
        @version NIIEngine 3.0.0
        */
        bool isPointSprites() const;

        /** 设置是否平行视觉
        @version NIIEngine 3.0.0
        */
        void setParallelView(bool b);

        /** 获取是否平行视觉
        @version NIIEngine 3.0.0
        */
        bool isParallelView() const;

        /** 设置控制模式
        @version NIIEngine 3.0.0
        */
        void setCtrlMode(ParticleCtrl::CtrlMode cm);

        /** 获取控制模式
        @version NIIEngine 3.0.0
        */
        ParticleCtrl::CtrlMode getCtrlMode() const;

        /** 获取作为这个集合所有粒子的原点.
        @version NIIEngine 3.0.0
        */
        void setOriginType(ParticleCtrl::OriginType origin);

        /** 获取作为这个集合所有粒子的原点.
        @version NIIEngine 3.0.0
        */
        ParticleCtrl::OriginType getOriginType() const;

        /** 设置旋转类型.
        @version NIIEngine 3.0.0
        */
        void setRotationMode(ParticleCtrl::RotationMode rm);

        /** 获取旋转类型.
        @version NIIEngine 3.0.0
        */
        ParticleCtrl::RotationMode getRotationMode() const;

        /** 设置控制器
        @version NIIEngine 3.0.0
        */
        void setCtrlDirection(const Vector3f & dir);

        /** 设置控制器
        @version NIIEngine 3.0.0
        */
        const Vector3f & getCtrlDirection() const;

        /** 设置所有粒子向上向量
        @version NIIEngine 3.0.0
        */
        void setCtrlUp(const Vector3f & up);

        /** 获取所有粒子向上向量CM_Vertical_Self
		@version NIIEngine 3.0.0
        */
        const Vector3f & getCtrlUp() const;

        /// @copydetails ParticleGeo::getType
        FactoryID getType() const;

        /// @copydetails ParticleGeo::queue
        void queue(RenderQueue * queue, ParticleList & list);

        /// @copydetails ParticleGeo::onCtrlResize
        void onCtrlResize(NIIf width, NIIf height);

        /// @copydetails ParticleGeo::setRenderGroup
        void setRenderGroup(Nui16 rgid);

        /// @copydetails ParticleGeo::setLocalSpace
        void setLocalSpace(bool b);

        /// @copydetails ParticleGeo::setCullItself
        void setCullItself(bool b);

        /// @copydetails ParticleGeo::getSortMode
		ParticleCtrl::SortMode getSortMode() const;

        /// @copydetails ParticleGeo::_notify
        void _notify(Camera * cam);
        
        /// @copydetails ParticleGeo::query
        void query(GeometryQuery * query);

        /// @copydetails ParticleGeo::setMaterial
        void setMaterial(ResourceID rid, GroupID gid);

        /// @copydetails ParticleGeo::attach
        void attach(PosNode * node, bool tag);

        /// @copydetails ParticleGeo::onParticleRotate
        void onParticleRotate();

        /// @copydetails ParticleGeo::onParticleResize
        void onParticleResize();

        /// @copydetails ParticleGeo::onPoolSizeChange
        void onPoolSizeChange(NCount count);
	protected:
		bool init(PropertyCmdSet * dest);
    protected:
        ParticleCtrl * mCtrl;
    };

    /// ParticleCtrlGeo的工厂类
    class _EngineAPI ParticleCtrlGeoFactory : public ParticleGeoFactory
    {
    public:
        ParticleCtrlGeoFactory();
        ~ParticleCtrlGeoFactory();

        /// @copydetails FactoryObj::getVFSType
        Nmark getVFSType() const;
        
        /// @copydetails FactoryObj::getID
        FactoryID getID() const;

        /// @copydetails FactoryObj::create
		void * create(const String & file);

        /// @copydetails FactoryObj::destroy
		void destroy(void * obj);
    };
}
#endif