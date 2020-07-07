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
        /// The movable object whose level of detail has changed.
        SpaceObj * movableObject;

        /// The camera with respect to which the level of detail has changed.
        Camera * camera;
    };

    /** 包含关于实体的一个网格LOD改变事件的结构体
    @version NIIEngine 3.0.0
    */
    struct MeshLodEvent
    {
        /// The entity whose level of detail has changed.
        GeometrySpace * entity;

        /// The camera with respect to which the level of detail has changed.
        Camera * camera;

        /// Lod value as determined by lod strategy.
        Nui32 lodValue;

        /// Previous level of detail index.
        Nui16 previousLodIndex;

        /// New level of detail index.
        Nui16 newLodIndex;
    };

    /// 包含关于实体的一个材质LOD改变事件的结构体
    struct MaterialLodEvent
    {
        /// The sub-entity whose material's level of detail has changed.
        GeometryCom * subEntity;

        /// The camera with respect to which the level of detail has changed.
        Camera * camera;

        /// Lod value as determined by lod strategy.
        Nui32 lodValue;

        /// Previous level of detail index.
        Nui16 previousLodIndex;

        /// New level of detail index.
        Nui16 newLodIndex;
    };

    /** 一个口类定义,一个接收通知LOD事件的鉴听
    @remark
        A 'listener' is an interface designed to be called back when
        particular events are called. This class defines the
        interface relating to lod events. In order to receive
        notifications of lod events, you should create a subclass of
        LodListener and override the methods for which you would like
        to customise the resulting processing. You should then call
        SpaceManager::add passing an instance of this class.
        There is no limit to the number of lod listeners you can register,
        allowing you to register multiple listeners for different purposes.

        For some uses, it may be advantageous to also subclass
        RenderListener as this interface makes available information
        regarding render queue invocations.

        It is important not to modify the scene graph during rendering, so,
        for each event, there are two methods, a prequeue method and a
        postqueue method.  The prequeue method is invoked during rendering,
        and as such should not perform any changes, but if the event is
        relevant, it may return true indicating the postqueue method should
        also be called.  The postqueue method is invoked at an appropriate
        time after rendering and scene changes may be safely made there.
    */
    class _EngineAPI LodListener
    {
    public:
        virtual ~LodListener() {}

        /** 一个可移动物体lod改变前调用
        @remark
            Do not change the Nii state from this method,
            instead return true and perform changes in
            postqueueMovableObjectLodChanged.
        @return
            True to indicate the event should be queued and
            postqueueMovableObjectLodChanged called after
            rendering is complete.
        */
        virtual bool prequeueMovableObjectLodChanged(const SpaceLodEvent & evt)
        { (void)evt; return false; }

        /** 一个可移动物体lod改变后调用
        @remark
            May be called even if not requested from prequeueMovableObjectLodChanged
            as only one event queue is maintained per SceneManger instance.
        */
        virtual void postqueueMovableObjectLodChanged(const SpaceLodEvent & evt)
        { (void)evt; }

        /** 一个实体网格lod改变前调用
        @remark
            Do not change the Nii state from this method,
            instead return true and perform changes in
            postqueueEntityMeshLodChanged.

            It is possible to change the event notification
            and even alter the newLodIndex field (possibly to
            prevent the lod from changing, or to skip an
            index).
        @return
            True to indicate the event should be queued and
            postqueueEntityMeshLodChanged called after
            rendering is complete.
        */
        virtual bool prequeueEntityMeshLodChanged(const MeshLodEvent & evt)
        { (void)evt; return false; }

        /** 一个实体网格lod改变后调用
        @remark
            May be called even if not requested from prequeueEntityMeshLodChanged
            as only one event queue is maintained per SceneManger instance.
        */
        virtual void postqueueEntityMeshLodChanged(const MeshLodEvent & evt)
        { (void)evt; }

        /** 一个实体材质lod改变前调用.
        @remark
            Do not change the Nii state from this method,
            instead return true and perform changes in
            postqueueMaterialLodChanged.

            It is possible to change the event notification
            and even alter the newLodIndex field (possibly to
            prevent the lod from changing, or to skip an
            index).
        @return
            True to indicate the event should be queued and
            postqueueMaterialLodChanged called after
            rendering is complete.
        */
        virtual bool prequeueEntityMaterialLodChanged(const MaterialLodEvent & evt)
        { (void)evt; return false; }

        /** 一个实体材质lod改变后调用.
        @remark
            May be called even if not requested from prequeueEntityMaterialLodChanged
            as only one event queue is maintained per SceneManger instance.
        */
        virtual void postqueueEntityMaterialLodChanged(const MaterialLodEvent & evt)
        { (void)evt; }
    };
}
#endif