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

#ifndef _NII_ParticleGeo_H_
#define _NII_ParticleGeo_H_

#include "NiiPreInclude.h"
#include "NiiPropertyCmdObj.h"
#include "NiiParticleCtrl.h"
#include "NiiParticle.h"
#include "NiiFactoryObj.h"

namespace NII
{
    /** 粒子几何
    @note 仅控制活动粒子
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ParticleGeo : public PropertyCmdObj, public ParticleAlloc
    {
    public:
        ParticleGeo(ScriptTypeID stid = N_CmdObj_ParticleGeo, LangID lid = N_PrimaryLang) :
            PropertyCmdObj(stid, lid){}
        virtual ~ParticleGeo(){}

        /** 获取类型
        @version NIIEngine 3.0.0
        */
        virtual FactoryID getType() const = 0;

        /** 排列粒子
        @version NIIEngine 3.0.0
        */
        virtual void queue(RenderQueue * queue, ParticleList & list) = 0;

        /** 设置材质
        @version NIIEngine 3.0.0
        */
        virtual void setMaterial(ResourceID rid, GroupID gid) = 0;

        /** 设置渲染组
        @version NIIEngine 3.0.0
        */
        virtual void setRenderGroup(Nui16 rqid) = 0;

        /** 设置是否独立裁剪
        @version NIIEngine 3.0.0
        */
        virtual void setCullItself(bool b) = 0;
        
        /** 设置是否使用本地模式
        @version NIIEngine 3.0.0
        */
        virtual void setLocalSpace(bool b) = 0;

        /** 获取排序模式
        @version NIIEngine 3.0.0
        */
        virtual ParticleCtrl::SortMode getSortMode() const = 0;

        /** 通知摄象机机改变
        @version NIIEngine 3.0.0
        */
        virtual void _notify(Camera * cam) = 0;
        
        /** 执行几何查询
        @version NIIEngine 3.0.0
        */
        virtual void query(GeometryQuery * query) = 0;

        /** 附加到节点上
        @version NIIEngine 3.0.0
        */
        virtual void attach(PosNode * node, bool tag) = 0;
    public:
        /** 当粒子旋转时
        @version NIIEngine 3.0.0
        */
        virtual void onParticleRotate(){}

        /** 当粒子大小变化时
        @version NIIEngine 3.0.0
        */
        virtual void onParticleResize(){}

        /** 当控制器大小变化时
        @version NIIEngine 3.0.0
        */
        virtual void onCtrlResize(NIIf width, NIIf height){}
        
        /** 当粒子池变化时
        @version NIIEngine 3.0.0
        */
        virtual void onPoolSizeChange(NCount count){}
    };
    
    /// ParticleGeo工产类
    class _EngineAPI ParticleGeoFactory : public FactoryObj
    {
    public:
        ParticleGeoFactory(Nid id) : FactoryObj(), mID(id){}
        virtual ~ParticleGeoFactory(){}

        FactoryID getID() const { return mID; }
    private:
        Nid mID;
    };
}
#endif