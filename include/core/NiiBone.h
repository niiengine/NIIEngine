/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-5

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

#ifndef _NII_BONE_H_
#define _NII_BONE_H_

#include "NiiPreInclude.h"
#include "NiiPosNode.h"

namespace NII
{
    /** 骨骼顶点分配
    @version NIIEngine 3.0.0
    */
    typedef struct _SkeletonVertex
    {
        Nui32 mVertexIndex; ///< 顶点索引
        Nui16 mBoneIndex;   ///< 骨头索引
        NIIf mWeight;       ///< 权重

    } SkeletonVertex;
    typedef multimap<Nidx, SkeletonVertex>::type SkeletonVertexList;

    /** 骨骼里的骨头 .
    @remark 如果还在地球尝试使用三轴传感器完成骨头动画的录制
    @note id机制是局限在一个骨骼里的
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Bone : public PosNode
    {
        friend class Skeleton;
    public:
        Bone(SpaceID id, Skeleton * own);
        ~Bone();

        /** 设置辅助名字
        @version NIIEngine 3.0.0 高级api
        */
        void setName(const String & name);
        
        /** 获取辅助名字
        @version NIIEngine 3.0.0 高级api
        */
        const String & getName() const;
        
        /** 创建子骨头
        @param[in] id 同个体系(骨骼)中唯一
        @param[in] pos 相对位置
        @param[in] rot 相对方向
        @version NIIEngine 3.0.0
        */
        Bone * create(SpaceID id, const Vector3f & pos = Vector3f::ZERO,
            const Quaternion & rot = Quaternion::IDENTITY);

        /** 设立原始点
        @version NIIEngine 3.0.0
        */
        void origin();

        /** 重至位置
        @remark 重至到设立的原始点
        @version NIIEngine 3.0.0
        */
        void reset();

        /** 设置是否手动控制.
        @version NIIEngine 3.0.0
        */
        void setManual(bool b);

        /** 获取是否手动控制.
        @version NIIEngine 3.0.0
        */
        bool isManual() const;

        /** 获取初始化位置
        @version NIIEngine 3.0.0
        */
        virtual const Vector3f & getOriginPos() const;

        /** 获取初始化方向
        @version NIIEngine 3.0.0
        */
        virtual const Quaternion & getOriginOri() const;

        /** 获取初始化缩放
        @version NIIEngine 3.0.0
        */
        virtual const Vector3f & getOriginScale() const;

        /** 副本对象
        @remark 复制到指定的骨骼中
        @version NIIEngine 3.0.0 高级api
        */
        Bone * cloneTo(Skeleton * dst) const;
        
        /// @copydetails PosNode::notify
        void notify(bool force = false);
    protected:
        /// @copydetails PosNode::createImpl
        PosNode * createImpl(SpaceID id = 0);

        /// @copydetails PosNode::destroyImpl
        void destroyImpl(SpaceID id );
        
        /** 获取骨头当前空间的变换
        @param[out] m 空间的变换
        @version NIIEngine 3.0.0 高级api
        */
        void getFusion(Matrix4f * m) const;
    protected:
        Skeleton * mCreator;
        Vector3f mSrcPosOrg;
        Vector3f mSrcScaleOrg;
        Quaternion mSrcOriOrg;
        Vector3f mSrcPos;
        Vector3f mSrcScale;
        Quaternion mSrcOri;
        String mName;
        bool mManual;
    };
}
#endif