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

#ifndef _NII_PHYSICAL_OBJ_H_
#define _NII_PHYSICAL_OBJ_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 物理范围
    @version 4.0.0
    */
    class _EngineAPI PhysicalScope : public PhysicalAlloc
    {
    public:
        PhysicalScope();
        virtual ~PhysicalScope();
        
        /**
        @version NIIEngine 4.0.0
        */
        void add(PhysicalObj * obj, bool exclude = false);
        
        /**
        @version NIIEngine 4.0.0
        */
        void remove(PhysicalObj * obj);
    private:
        PhysicalObjList mList;
        PhysicalObjList mExcludeList;
    };

    /** 物理对象
    @version 4.0.0
    */
    class _EngineAPI PhysicalObj
    {
    public:
        PhysicalObj();
        virtual ~PhysicalObj();
        
        /**
        @version NIIEngine 4.0.0
        */
        void add(PhysicalCom * obj);
        
        /**
        @version NIIEngine 4.0.0
        */
        void remove(PhysicalCom * obj);
        
        /**
        @version NIIEngine 4.0.0
        */
        inline const PhysicalComList & getList() const  { return mPhysicalComList; }
        
        /** 设置是否启动
        @version NIIEngine 4.0.0
        */
        inline void setEnable(bool set)                 { mEnable = set;}
        
        /** 获取是否启动
        @version NIIEngine 4.0.0
        */
        inline bool isEnable() const                    { return mEnable;}

        /** 摩擦系数
        @version NIIEngine 4.0.0
        */
        inline void setFriction(NIIf factor)            { mFriction = factor; }

        /** 摩擦系数
        @version NIIEngine 4.0.0
        */
        inline NIIf getFriction() const                 { return mFriction; }

        /** 密度
        @version NIIEngine 4.0.0
        */
        inline void setDensity(Ni32 mass)               { mDensity = mass; }

        /** 密度
        @version NIIEngine 4.0.0
        */
        inline Ni32 getDensity() const                  { return mDensity; }
        
        /**
        @version NIIEngine 4.0.0
        */
        inline setOuterAABox(const AABox & aab)         { mOuterAABox = aab;} 
        
        /**
        @version NIIEngine 4.0.0
        */
        inline const AABox & getOuterAABB() const       { return mOuterAABox; }
        
        /** 主体
        @version NIIEngine 4.0.0
        */
        inline void setPrimary(SpaceObj * obj)          { mSpaceObj = obj; }
        
        /** 主体
        @version NIIEngine 4.0.0
        */
        inline SpaceObj * getPrimary() const            { return mSpaceObj; }
        
        /** 辅体(预示体)
        @version NIIEngine 4.0.0
        */
        inline void setSecond(SpaceObj * obj)           { mSecondObj = obj; }
        
        /** 辅体(预示体)
        @version NIIEngine 4.0.0
        */
        inline SpaceObj * getSecond() const             { return mSecondObj; }
    protected:
        /**
        @version NIIEngine 4.0.0
        */
        virtual void onOuterAABoxEnter(const EventArg * arg);
        
        /**
        @version NIIEngine 4.0.0
        */
        virtual void onOuterAABoxLeave(const EventArg * arg);
    private:
        PhysicalComList mPhysicalComList;
        NIIf mFriction;
        Ni32 mDensity;
        AABox mOuterAABox;
        SpaceObj * mSpaceObj;
        SpaceObj * mSecondObj;
        bool mEnable;
    };
}

#endif