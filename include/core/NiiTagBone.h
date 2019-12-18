/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-7

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

#ifndef _NII_TAG_BONE_H_
#define _NII_TAG_BONE_H_

#include "NiiPreInclude.h"
#include "NiiBone.h"
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