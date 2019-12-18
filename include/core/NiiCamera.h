/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-12-7

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

#ifndef _NII_CAMERA_H_
#define _NII_CAMERA_H_

#include "NiiPreInclude.h"
#include "NiiQuaternion.h"
#include "NiiFrustum.h"
#include "NiiCameraListener.h"
#include "NiiCommon.h"
#include "NiiShaderCh.h"
#include "NiiRay.h"

namespace NII
{
    /** 几何摄象口
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Camera : public Frustum
    {
    public:
        Camera(SpaceID sid, SpaceManager * sm = 0);
        virtual ~Camera();

        /** 渲染的视口
        @param[in] vp 需要渲染的视口
        @version NIIEngine 3.0.0 高级api
        */
        void render(Viewport * vp);

        /** 世界空间的视点.
        @param[in] v 点
        @version NIIEngine 3.0.0
        */
        void lookAt(const Vector3f & v);

        /** 移动摄象机
        @remark 世界坐标
        @version NIIEngine 3.0.0
        */
        void movePos(const Vector3f & v);

        /** 移动摄象机
        @remark 摄象机坐标标准
        @version NIIEngine 3.0.0
        */
        void moveRel(const Vector3f & v);

        /** 更新对焦
        @version NIIEngine 3.0.0
        */
        void updateFocus();

        /** 获取一条贯穿视窗的射线
        @remark 主要用于物体拾获.渲染系统里还有个像素拾获,获取更符合实际
        @param[in] viewx
        @param[in] viewy
        @version NIIEngine 3.0.0
        */
        Ray getRay(NIIf viewx, NIIf viewy) const;

        /** 获取一条贯穿视窗的区域
        @remark 包含远/近平截面.渲染系统里还有个像素拾获,获取更符合实际
        @param[in] viewleft
        @param[in] viewtop
        @param[in] viewright
        @param[in] viewbottom
        @version NIIEngine 3.0.0
        */
        PlaneSpace getVolume(NIIf viewleft, NIIf viewtop, NIIf viewright, NIIf viewbottom);

        /** 是否启用扩展空间模式
        @remark 一般用于空间物体影响当前空间,但空间物体实际并不存在于视区
        @version NIIEngine 3.0.0
        */
        void setExpandMode(bool b);

        /** 是否启用扩展空间模式
        @remark 一般用于空间物体影响当前空间,但空间物体实际并不存在于视区
        @version NIIEngine 3.0.0
        */
        bool isExpandMode() const;

        /** 重至摄像区域
        @remark 一般用于空间物体影响当前空间,但空间物体实际并不存在于视区
        @version NIIEngine 3.0.0
        */
        void resetBound();

        /** 扩容摄像区域
        @remark 一般用于空间物体影响当前空间,但空间物体实际并不存在于视区
        @param[in] b1 边界盒
        @param[in] b2 范围边界
        @param[in] receiver 合并物是否接收阴影
        */
        void expandBound(const AABox & b1, const Sphere & b2, bool receiver = true);

        /** 扩容摄像投影区域
        @remark 一般用于空间物体影响当前空间,但空间物体实际并不存在于视区
        @param[in] b1 边界盒
        @param[in] b2 范围边界
        @version NIIEngine 3.0.0
        */
        void expandCastBound(const AABox & b1, const Sphere & b2);

        /** 获取摄像最近区域
        @version NIIEngine 3.0.0
        */
        NIIf getBoundMin() const;

        /** 获取摄像最远区域
        @version NIIEngine 3.0.0
        */
        NIIf getBoundMax() const;

        /** 获取摄像投射最近区域
        @version NIIEngine 3.0.0
        */
        NIIf getCastBoundMin() const;

        /** 获取摄像投射最远区域
        @version NIIEngine 3.0.0
        */
        NIIf getCastBoundMax() const;

        /** 获取摄像区域
        @version NIIEngine 3.0.0
        */
        const AABox & getBound() const;

        /** 获取阴影接收摄像区域
        @version NIIEngine 3.0.0
        */
        const AABox & getReceiveBound() const;

        /** 添加摄像机监听器
        @version NIIEngine 3.0.0
        */
        void add(CameraListener * obj);

        /** 移去摄像机监听器
        @version NIIEngine 3.0.0
        */
        void remove(CameraListener * obj);

        /** 设置摄象机位置
        @version NIIEngine 3.0.0
        */
        void setPos(const Vector3f & p);

        /** 获取摄象机位置
        @version NIIEngine 3.0.0
        */
        const Vector3f & getPos() const;

        /** 获取摄象机上方向向量.
        @version NIIEngine 3.0.0
        */
        Vector3f getUp() const;

        /** 获取摄象机右方向向量.
        @version NIIEngine 3.0.0
        */
        Vector3f getRight() const;

        /** 设置摄象机朝向
        @version NIIEngine 3.0.0
        */
        void setDirection(const Vector3f & v);

        /** 获取摄象机朝向
        @version NIIEngine 3.0.0
        */
        Vector3f getDirection() const;

        /** 设置摄象机方向
        @version NIIEngine 3.0.0
        */
        void setOri(const Quaternion & q);

        /** 返回摄象机方向
        @version NIIEngine 3.0.0
        */
        const Quaternion & getOri() const;

        /** 饶y轴旋转摄象机
        @note 逆时针方向
        @version NIIEngine 3.0.0
        */
        void yaw(const Radian & angle);

        /** 饶x轴颠簸摄象机
        @note 逆时针方向
        @version NIIEngine 3.0.0
        */
        void pitch(const Radian & angle);

        /** 饶z轴滚动摄象机
        @note 逆时针方向
        @version NIIEngine 3.0.0
        */
        void roll(const Radian & angle);

        /** 围绕的任意轴旋转的摄像头.
        @version NIIEngine 3.0.0
        */
        void rotate(const Vector3f & axis, const Radian & angle);

        /** 使用四元组旋转摄像头.
        @version NIIEngine 3.0.0
        */
        void rotate(const Quaternion & q);

        /** 水平方向旋转控制
        @param[in] o 原点
        @version NIIEngine 3.0.0
        */
        void setCustomYaw(bool enable, const Vector3f & o = Vector3f::Y);

        /** 设置三角形数量
        @version NIIEngine 3.0.0 高级api
        */
        void setTriangle(NCount count);

        /** 获取三角形数量
        @version NIIEngine 3.0.0
        */
        Nui32 getTriangleCount() const;

        /** 设置批次数量
        @version NIIEngine 3.0.0 高级api
        */
        void setBatch(NCount count);

        /** 获取批次数量
        @version NIIEngine 3.0.0
        */
        Nui32 getBatchCount() const;

        /** 设置使用的几何模式
        @version NIIEngine 3.0.0
        */
        void setPolygonMode(PolygonMode pm);

        /** 获取使用的几何模式.
        @version NIIEngine 3.0.0
        */
        PolygonMode getPolygonMode() const;

        /** 设置子视口模式
        @version NIIEngine 3.0.0
        */
        void setSubMode(bool b);

        /** 是否子视口模式
        @version NIIEngine 3.0.0
        */
        bool isSubMode() const;

        /** 设置子视图区域
        @note setSubMode/isSubMode 函数必须为 true 才有效
        @version NIIEngine 3.0.0
        */
        void setSubView(NIIf left, NIIf top, NIIf right, NIIf bottom);

        /** 获取子视图面区域
        @note setSubMode/isSubMode 函数必须为 true 才有效
        @version NIIEngine 3.0.0
        */
        const PlaneList & getSubClip();

        /** 设置可见像素限制
        @version NIIEngine 3.0.0
        */
        void setVisiblePixelLimit(bool b);

        /** 是否可见像素限制
        @version NIIEngine 3.0.0
        */
        bool isVisiblePixelLimit() const;

        /** 设置可见距离限制
        @version NIIEngine 3.0.0
        */
        void setVisibleDistanceLimit(bool b);

        /** 是否可见距离限制
        @version NIIEngine 3.0.0
        */
        bool isVisibleDistanceLimit() const;

        /** 设置重新计算宽/高比
        @remark
        */
        void setAutoAspectRatio(bool b);

        /** 获取重新计算宽/高比
        @version NIIEngine 3.0.0
        */
        bool getAutoAspectRatio() const;

        /** 获取渲染像素宽/高比
        @version NIIEngine 3.0.0
        */
        NIIf getPixelRatio() const;

        /** 自动对焦
        @version NIIEngine 3.0.0
        */
        void setAutoFocus(bool b);

        /** 设置对焦点
        @param[in] obj 对焦点
        @param[in] oft 对焦偏量
        @version NIIEngine 3.0.0
        */
        void setAutoFocus(PosNode * obj, const Vector3f & oft = Vector3f::ZERO);

        /** 获取对焦点
        @version NIIEngine 3.0.0
        */
        PosNode * getAutoFocus() const;

        /** 获取对焦偏量
        @version NIIEngine 3.0.0
        */
        const Vector3f & getFocusOffset() const;

        /** 获取空间方向
        @remark 含概所有变换
        @version NIIEngine 3.0.0
        */
        const Quaternion & getSpaceOri() const;

        /// @copydetails SpaceObj::getSpacePos
        const Vector3f & getSpacePos() const;

        /// @copydetails SpaceObj::getSpaceDirection
        const Vector3f & getSpaceDirection() const;

        /** y轴朝向
        @remark 含概所有变换
        @version NIIEngine 3.0.0
        */
        Vector3f getSpaceUp() const;

        /** x轴朝向
        @remark 含概所有变换
        @version NIIEngine 3.0.0
        */
        Vector3f getSpaceRight() const;

        /** 获取位置
        @version NIIEngine 3.0.0
        */
        const Vector3f & getLocalPos() const;

        /** 获取方向
        @version NIIEngine 3.0.0
        */
        const Quaternion & getLocalOri() const;

        /** y轴朝向
        @version NIIEngine 3.0.0
        */
        Vector3f getLocalUp() const;

        /** x轴朝向
        @version NIIEngine 3.0.0
        */
        Vector3f getLocalRight() const;

        /** z轴朝向
        @version NIIEngine 3.0.0
        */
        Vector3f getLocalDirection() const;

        /** 为这个摄象机设置lod因子.
        @version NIIEngine 3.0.0 高级api
        */
        void setLodBias(NIIf factor = 1.0);

        /** 获取这个摄象机应用的lod因子.
        @version NIIEngine 3.0.0 高级api
        */
        NIIf getLodBias() const;

        /** 设置当前视口
        @version NIIEngine 3.0.0 高级api
        */
        void setViewport(Viewport * obj);

        /** 获取当前视口
        @version NIIEngine 3.0.0 高级api
        */
        Viewport * getViewport() const;

        /** 使用扩展的LOD控制摄象机
        @version NIIEngine 3.0.0 高级api
        */
        void setExtLod(const Camera * obj);

        /** 获取扩展的LOD控制摄象机
        @version NIIEngine 3.0.0 高级api
        */
        const Camera * getExtLod() const;

        /** 使用扩展的裁减区域
        @version NIIEngine 3.0.0 高级api
        */
        void setExtCulling(Frustum * obj);

        /** 返回扩展的裁减区域
        @version NIIEngine 3.0.0 高级api
        */
        Frustum * getExtCulling() const;

        /** 获取视图距阵
        @param[in] 是否获取其他的裁减区域的视图距阵
        @version NIIEngine 3.0.0
        */
        const Matrix4f & getViewMatrix(bool cull) const;

        /// @copydetails SpaceObj::getFactoryID
        FactoryID getFactoryID() const;

        /// @copydetails SpaceObj::getCenterRange
        NIIf getCenterRange() const;

        /// @copydetails GeometryObj::getMatrix
        void getMatrix(Matrix4f * out, NCount & count) const;

        /// @copydetails Frustum::isIntersect
        bool isIntersect(const AABox & aabb) const;

        /// @copydetails Frustum::isIntersect
        bool isIntersect(const Sphere & bs) const;

        /// @copydetails Frustum::isIntersect
        bool isIntersect(const Vector3f & vv) const;

        /// @copydetails Frustum::getClipPlaneIntersect
        const Vector3f * getClipPlaneIntersect() const;

        /// @copydetails Frustum::getClipPlane
        const Plane & getClipPlane(PlaneType plane) const;

        /// @copydetails Frustum::getNodePos
        const Vector3f & getNodePos() const;

        /// @copydetails Frustum::getNodeOri
        const Quaternion & getNodeOri() const;

        /// @copydetails Frustum::project
        bool project(const Sphere & in, Rectf & out) const;

        /// @copydetails Frustum::getNearClipDistance
        NIIf getNearClipDistance() const;

        /// @copydetails Frustum::getFarClipDistance
        NIIf getFarClipDistance() const;

        /// @copydetails Frustum::getViewMatrix
        const Matrix4f & getViewMatrix() const;
    protected:
        /// @copydetails Frustum::isViewValid
        bool isViewValid() const;

        /// @copydetails Frustum::notifyProj
        void notifyProj() const;

        /// @copydetails Frustum::notifyView
        void notifyView() const;
    protected:
        CameraListenerList mListeners;
        NIIf mPixelRatio;
        Vector3f mPosition;
        Quaternion mOrientation;
        NCount mTriCount;
        NCount mBatchCount;
        Vector3f mCustomYawAxis;
        PolygonMode mPolygonMode;
        NIIf mLodFactor;
        NIIf mSubLeft;
        NIIf mSubTop;
        NIIf mSubRight;
        NIIf mSubBottom;
        AABox mAABB;
        NIIf mMin;
        NIIf mMax;
        AABox mAABBReceive;
        NIIf mMinCast;
        NIIf mMaxCast;
        mutable Quaternion mLocalOri;
        mutable Vector3f mLocalPos;
        mutable Vector3f mSpacePos;
        mutable Vector3f mSpaceDirection;
        mutable Quaternion mSpaceOrientation;
        mutable bool mSubClipDirty;
        bool mSubMode;
        bool mCustomYaw;
        bool mAutoAspectRatio;

        Viewport * mViewport;
        Frustum * mExtCull;
        const Camera * mExtLod;
        PosNode * mFocusNode;
        Vector3f mFocusOffset;
        PlaneList mSubClips;

        bool mDisLimit;
        bool mExpandMode;
        bool mPixelLimit;
        bool mAutoFocus;
    };

    inline _EngineAPI std::ostream & operator << (std::ostream & s, const Camera & c)
    {
        s << "Camera(Name='" << c.getID() << "', pos=" << c.getPos();
        Vector3f dir(c.getOri() * Vector3f(0, 0, -1));
        s << ", direction=" << dir << ", near=" << c.getNearClipDistance();
        s << ", far=" << c.getFarClipDistance() << ", FOVy=" << c.getFOV().getAngle();
        s << ", aspect=" << c.getAspectRatio();
        s << ", xoffset=" << c.getAspectOffset().x;
        s << ", yoffset=" << c.getAspectOffset().y;
        s << ", focalLength=" << c.getAspectUnit();
        s << ", near=" << c.getClipPlane(Frustum::PT_Near);
        s << ", far=" << c.getClipPlane(Frustum::PT_Far);
        s << ", left=" << c.getClipPlane(Frustum::PT_Left);
        s << ", right=" << c.getClipPlane(Frustum::PT_Right);
        s << ", top=" << c.getClipPlane(Frustum::PT_Top);
        s << ", bottom=" << c.getClipPlane(Frustum::PT_Bottom);
        s << ")";

        return s;
    }
    typedef vector<Camera *>::type CameraList;
}
#endif