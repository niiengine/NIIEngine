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
    /**
    @version NIIEngine 4.0.0
    */
    class _EngineAPI PhysicalCom
    {
    public:
        PhysicalCom();
        ~PhysicalCom();
        
        /**
        @version NIIEngine 4.0.0
        */
        inline void setGroupMark(Ni32 mark) const       { mGroupMark = mark;}
        
        /**
        @version NIIEngine 4.0.0
        */
        inline Ni32 getGroupMark() const                { return mGroupMark;}
        
        /** 设置是否启动
        @version NIIEngine 4.0.0
        */
        inline void setEnable(bool set)                 { mEnable = set;}
        
        /** 获取是否启动
        @version NIIEngine 4.0.0
        */
        inline bool isEnable() const                    { return mEnable;}

        /** 设置是否能断离
        @version NIIEngine 4.0.0
        */
        inline void setFracture(bool set)               { mFracture = set; }

        /** 获取是否能断离
        @version NIIEngine 4.0.0
        */
        inline bool isFracture() const                  { return mFracture; }
        
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
        inline setAttach(SpaceObj * obj)                { mAttach = obj; }

        /**
        @version NIIEngine 4.0.0
        */
        inline SpaceObj * getAttach() const             { return mAttach; }
    private:
        SpaceObj * mAttach;
        NIIf mFriction;
        Ni32 mDensity;
        Ni32 mGroupMark;
        bool mEnable;
        bool mFracture;
    };

    /**
    @version NIIEngine 4.0.0
    */
    class _EngineAPI Physical2DCom : public PhysicalCom
    {
    public:
        Physical2DCom();
        ~Physical2DCom();
    };

    /**
    @version NIIEngine 4.0.0
    */
    class _EngineAPI Physical3DCom : public PhysicalCom
    {
    public:
        Physical2DCom();
        ~Physical2DCom();
    };
}

#endif