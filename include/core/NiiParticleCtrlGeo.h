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

#ifndef _NII_ParticleCtrlGeo_H__
#define _NII_ParticleCtrlGeo_H__

#include "NiiPreInclude.h"
#include "NiiParticleGeo.h"

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
        inline void setPointSprites(bool b)         { mCtrl->setPointSprites(b); }

        /** 获取是否点动画
        @version NIIEngine 3.0.0
        */
        inline bool isPointSprites() const          { return mCtrl->isPointSprites(); }

        /** 设置是否平行视觉
        @version NIIEngine 3.0.0
        */
        inline void setParallelView(bool b)         { mCtrl->setParallelView(b); }

        /** 获取是否平行视觉
        @version NIIEngine 3.0.0
        */
        inline bool isParallelView() const          { return mCtrl->isParallelView(); }

        /** 设置控制模式
        @version NIIEngine 3.0.0
        */
        inline void setCtrlMode(ParticleCtrl::CtrlMode cm)  { mCtrl->setCtrlMode(cm); }

        /** 获取控制模式
        @version NIIEngine 3.0.0
        */
        inline ParticleCtrl::CtrlMode getCtrlMode() const   { return mCtrl->getCtrlMode(); }

        /** 获取作为这个集合所有粒子的原点.
        @version NIIEngine 3.0.0
        */
        inline void setOriginType(ParticleCtrl::OriginType origin)  { mCtrl->setOriginType(origin); }

        /** 获取作为这个集合所有粒子的原点.
        @version NIIEngine 3.0.0
        */
        inline ParticleCtrl::OriginType getOriginType() const       { return mCtrl->getOriginType(); }

        /** 设置旋转类型.
        @version NIIEngine 3.0.0
        */
        inline void setRotationMode(ParticleCtrl::RotationMode rm)  { mCtrl->setRotationMode(rm); }
        
        /** 获取旋转类型.
        @version NIIEngine 3.0.0
        */
        inline ParticleCtrl::RotationMode getRotationMode() const   { return mCtrl->getRotationMode(); }

        /** 设置控制器
        @version NIIEngine 3.0.0
        */
        inline void setCtrlDirection(const Vector3f & dir)          { mCtrl->setCtrlDirection(dir); }

        /** 设置控制器
        @version NIIEngine 3.0.0
        */
        inline const Vector3f & getCtrlDirection() const            { return mCtrl->getCtrlDirection(); }

        /** 设置所有粒子向上向量
        @version NIIEngine 3.0.0
        */
        inline void setCtrlUp(const Vector3f & up)                  { mCtrl->setCtrlUp(up); }

        /** 获取所有粒子向上向量CM_Vertical_Self
        @version NIIEngine 3.0.0
        */
        inline const Vector3f & getCtrlUp() const                   { return mCtrl->getCtrlUp(); }

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
        bool initCmd(PropertyCmdSet * dest);
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