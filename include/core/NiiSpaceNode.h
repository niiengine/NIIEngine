/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-7

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

#ifndef _NII_SceneNode_H_
#define _NII_SceneNode_H_

#include "NiiPreInclude.h"
#include "NiiPosNode.h"
#include "NiiAABox.h"

namespace NII
{
    /** 空间节点
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SpaceNode : public PosNode
    {
    public:
        typedef map<SpaceID, SpaceObj *>::type AttachList;
    public:
        SpaceNode(SpaceManager * gm);
        SpaceNode(SpaceManager * gm, SpaceID id);
        ~SpaceNode();

        /** 更新自动锁定
        @version NIIEngine 3.0.0 高级api
        */
        void updateFocus();

        /** 附加空间对象
        @version NIIEngine 3.0.0
        */
        virtual void attach(SpaceObj * obj);

        /** 分离空间对象
        @version NIIEngine 3.0.0
        */
        virtual SpaceObj * detach(Nidx index);

        /** 分离空间对象
        @version NIIEngine 3.0.0
        */
        virtual SpaceObj * detach(SpaceID sid);

        /** 分离空间对象
        @version NIIEngine 3.0.0
        */
        virtual void detach(SpaceObj * obj);

        /** 获取空间对象
        @version NIIEngine 3.0.0
        */
        virtual SpaceObj * getAttach(Nidx index);

        /** 获取空间对象
        @version NIIEngien 3.0.0
        */
        virtual SpaceObj * getAttach(SpaceID sid);

        /** 分离空间对象
        @version NIIEngine 3.0.0
        */
        virtual void detachAll();

        /** 获取附加对象的数量
        @version NIIEngine 3.0.0
        */
        NCount getAttachCount() const;

        /** 获取附加列表
        @version NIIEngine 3.0.0 高级api
        */
        const AttachList & getAttachList() const;

        /** 设置节点是否有效
        @version NIIEngine 3.0.0 高级api
        */
        virtual void setEnable(bool b);

        /** 是否有效空间节点
        @version NIIEngine 3.0.0 高级api
        */
        virtual bool isEnable() const;

        /** 设置是否显示节点
        @version NIIEngine 3.0.0
        */
        virtual void setNodeView(bool b);

        /** 获取是否显示节点
        @version NIIEngine 3.0.0
        */
        virtual bool isNodeView() const;

        /** 设置是否显示边界
        @version NIIEngine 3.0.0
        */
        virtual void setAABBView(bool b);

        /** 获取是否显示边界
        @version NIIEngine 3.0.0
        */
        virtual bool isAABBView() const;

        /** 更新边界
        @note 全局型
        @version NIIEngine 3.0.0 高级api
        */
        virtual void updateSpaceAABB();

        /** 获取边界
        @note 全局型
        @version NIIEngine 3.0.0 高级api
        */
        virtual const AABox & getSpaceAABB() const;

        /** 设置空间对象是否可见
        @param[in] d 遍历其子节点对象
        @version NIIEngine 3.0.0
        */
        virtual void setVisible(bool b, bool r = true);

        /** 设置空间对象是否调试状态
        @param[in] d 遍历其子节点对象
        @version NIIEngine 3.0.0
        */
        virtual void setDebugMode(bool b, bool r = true);

        /** 添加空间对象到渲染队列
        @param[in] r 是否把子节点也这么做
        @param[in] onlycast 是否仅投影阴影
        @version NIIEngine 3.0.0
        */
        virtual void queue(RenderQueue * queue, Camera * cam, bool r = true, bool onlycast = false);

        /*** 添加节点边界到渲染队列
        @version NIIEngine 3.0.0
        */
        virtual void queueAABB(RenderQueue * queue);

        /** 寻找影响这个空间节点的灯光
        @version NIIEngine 3.0.0
        */
        virtual void findLights(LightList & dest, NIIf radius, Nmark mark = 0xFFFFFFFF) const;

        /** 所属场景
        @version NIIEngine 3.0.0
        */
        SpaceManager * getCreator() const { return mCreator; }

        /** 水平方向旋转控制
        @param[in] o 原点
        @version NIIEngine 3.0.0
        */
        virtual void setCustomYaw(bool enable, const Vector3f & o = Vector3f::Y);

        /** 对焦到点
        @param[in] point 点
		@param[in] mode 空间类型
		@param[in] dir 方向
        @version NIIEngine 3.0.0
        */
        virtual void lookAt(const Vector3f & point, SpaceMode mode,
            const Vector3f & dir = Vector3f::INV_Z);

        /** 设置节点方向
        @version NIIEngine 3.0.0
        */
        virtual void setDirection(NIIf x, NIIf y, NIIf z, SpaceMode mode = SM_Local,
            const Vector3f & dir = Vector3f::INV_Z);

        /** 设置节点方向
        @param[in] vec 方向
        @version NIIEngine 3.0.0
        */
        virtual void setDirection(const Vector3f & vec, SpaceMode mode = SM_Local,
            const Vector3f & dir = Vector3f::INV_Z);

        /** 自动对焦
        @version NIIEngine 3.0.0
        */
        virtual void setAutoFocus(bool enable, const SpaceNode * point = 0,
            const Vector3f & pos = Vector3f::ZERO, const Vector3f & dir = Vector3f::INV_Z);

        /** 获取对焦
        @version NIIEngine 3.0.0
        */
        const SpaceNode * getAutoFocus() const;

        /** 获取对焦的局部位置
        @version NIIEngine 3.0.0
        */
        const Vector3f & getAutoFocusPos();

        /** 获取对焦的局部方向
        @version NIIEngine 3.0.0
        */
        const Vector3f & getgetAutoFocusDir();

        /// @copydetails PosNode::update
        virtual void update(bool own, bool child);

        /// @copydetails PosNode::yaw
        virtual void yaw(const Radian & angle, SpaceMode mode = SM_Local);

        /// @copydetails PosNode::getGeometry
        virtual GeometryObj * getGeometry();
    protected:
        /// @copydetails PosNode::updateImpl.
        void updateImpl() const;

        /// @copydetails PosNode::createImpl
        PosNode * createImpl(SpaceID id);

        /// @copydetails PosNode::destroyImpl
        void destroyImpl(SpaceID id);

        /// @copydetails PosNode::setParent
        void setParent(PosNode * parent);
    protected:
        SpaceManager * mCreator;
        AttachList mAttachList;
        const SpaceNode * mAutoFocus;
        GeometryObj * mWorldAABBGeo;
        AABox mWorldAABB;
        Vector3f mAutoFocusPos;
        Vector3f mAutoFocusDir;
        Vector3f mCustomYawAxis;
        bool mCustomYaw;
        bool mEnable;
        bool mNodeView;
        bool mAABBView;
    };
}
#endif