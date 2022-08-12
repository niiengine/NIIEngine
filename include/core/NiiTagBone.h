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

#ifndef _NII_TAG_BONE_H_
#define _NII_TAG_BONE_H_

#include "NiiPreInclude.h"
#include "NiiSkeleton.h"
#include "NiiMatrix4.h"

namespace NII
{
    /** 骨骼中可附加物体的骨头
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TagBone : public Bone
    {
    public:
        TagBone(SpaceID id, Skeleton * target);
        virtual ~TagBone();

        /** 设置拥有者
        @version NIIEngine 3.0.0
        */
        void setOwner(SpaceObj * obj);

        /** 获取拥有者
        @version NIIEngine 3.0.0
        */
        SpaceObj * getOwner() const;

        /** 设置使用使用拥有者位置
        @version NIIEngine 3.0.0
        */
        void setUseOwnerSpacePos(bool b);
        
        /** 是否使用使用拥有者位置
        @version NIIEngine 3.0.0
        */
        bool isUseOwnerSpacePos() const;
        
        /** 设置使用拥有者空间缩放
        @version NIIEngine 3.0.0
        */
        void setUseOwnerSpaceScale(bool b);

        /** 是否使用拥有者空间缩放
        @version NIIEngine 3.0.0
        */
        bool isUseOwnerSpaceScale() const;
        
        /** 设置使用拥有者空间方向
        @version NIIEngine 3.0.0
        */
        void setUseOwnerSpaceOri(bool b);

        /** 是否使用拥有者空间方向
        @version NIIEngine 3.0.0
        */
        bool isUseOwnerSpaceOri() const;

        /** 设置附加对象
        @version NIIEngine 3.0.0
        */
        void setAttach(SpaceObj * obj);

        /** 获取附加对象
        @version NIIEngine 3.0.0
        */
        SpaceObj * getAttach() const;
        
        /// @see GeometryObj::getLight
        void getLight(LightList & ll) const;

        /// @copydetails PosNode::notify
        void notify(bool force = false);

        /// @copydetails PosNode::updateImpl
        void updateImpl() const;
    private:
        SpaceObj * mOwner;
        SpaceObj * mAttach;
        bool mUseOwnerSpacePos;
        bool mUseOwnerSpaceScale;
        bool mUseOwnerSpaceOri;
        
    };
}
#endif