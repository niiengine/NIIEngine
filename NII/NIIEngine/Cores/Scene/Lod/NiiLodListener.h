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

#ifndef _N_LodListener_H_
#define _N_LodListener_H_

#include "NiiPreInclude.h"

namespace NII
{
    struct ResResultEvent
    {
    
    };
    struct ResLoadEvent
    {
    
    };
    /**
    @version NIIEngine 3.0.0
    */
    struct SpaceLodEvent
    {
        SpaceObj * mSpaceObj;
        Camera * mCamera;
    };

    /** 网格LOD改变事件的结构体
    @version NIIEngine 3.0.0
    */
    struct MeshLodEvent
    {
        GeometrySpace * mGeometrySpace;
        Camera * mCamera;
        Nui32 mValue;
        Nui16 mPreIndex;
        Nui16 mIndex;
    };

    /** 材质LOD改变事件的结构体
    @version NIIEngine 3.0.0
    */
    struct MaterialLodEvent
    {
        GeometryCom * mGeometryCom;
        Camera * mCamera;
        Nui32 mValue;
        Nui16 mPreIndex;
        Nui16 mIndex;
    };

    /** LOD事件的鉴听
    @version NIIEngine 3.0.0
    */
    class _EngineAPI LodListener
    {
    public:
        virtual ~LodListener() {}

        /** 一个可移动物体lod改变前调用
        @version NIIEngine 3.0.0
        */
        virtual bool preLodChange(const SpaceLodEvent &){ return false; }

        /** 一个可移动物体lod改变后调用
        @version NIIEngine 3.0.0
        */
        virtual void postLodChange(const SpaceLodEvent &) { }

        /** 一个实体网格lod改变前调用
        @version NIIEngine 3.0.0
        */
        virtual bool preLodChange(const MeshLodEvent & evt) {return false; }

        /** 一个实体网格lod改变后调用
        @version NIIEngine 3.0.0
        */
        virtual void postLodChange(const MeshLodEvent & evt) { }

        /** 一个实体材质lod改变前调用.
        @version NIIEngine 3.0.0
        */
        virtual bool preLodChange(const MaterialLodEvent & evt) { return false; }

        /** 一个实体材质lod改变后调用.
        @version NIIEngine 3.0.0
        */
        virtual void postLodChange(const MaterialLodEvent & evt) { }
    };
}
#endif