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

#ifndef _NII_PHYSICAL_GROUP_H_
#define _NII_PHYSICAL_GROUP_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 物理效应组
    @version NIIEngine 4.0.0
    */
    class _EngineAPI PhysicalGroup
    {
    public:
        PhysicalGroup(SpaceManager * space);
        ~PhysicalGroup();
        
        /** 更新
        @version NIIEngine 4.0.0
        */
        void update(TimeDurMS cost);
        
        /**
        @version NIIEngine 4.0.0
        */
        void add(PhysicalObj * obj);
        
        /**
        @version NIIEngine 4.0.0
        */
        void remove(PhysicalObj * obj);
        
        /**
        @version NIIEngine 4.0.0
        */
        inline const PhysicalObjList & getList() const      { return mPhysicalObjList; }
        
        /** 更新子步数量
        @version NIIEngine 4.0.0
        */
        inline void setStep(NCount step)                    { mStep = step;}
        
        /** 更新子步数量
        @version NIIEngine 4.0.0
        */
        inline NCount getStep() const                       { return mStep;}

        /** 重力
        @version NIIEngine 4.0.0
        */
        inline void setGravity(const Vector3f & dir)        { mGravity = dir; }

        /** 重力
        @version NIIEngine 4.0.0
        */
        inline const Vector3f & getGravity() const          { return mGravity; }
    private:
        SpaceManager * mSpaceManager;
        PhysicalObjList mPhysicalObjList;
        Vector3f mGravity;
        NCount mStep;
    };
}

#endif