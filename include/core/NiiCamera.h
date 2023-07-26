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

#ifndef _NII_CAMERA_H_
#define _NII_CAMERA_H_

#include "NiiPreInclude.h"
#include "NiiQuaternion.h"
#include "NiiFrustum.h"
#include "NiiCommon.h"
#include "NiiShaderCh.h"
#include "NiiRay.h"

namespace NII
{
    /** 摄象机监听
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CameraListener
    {
    public:
        virtual ~CameraListener();

        /** 加入监听时触发
        @version NIIEngine 3.0.0
        */
        virtual void onInit(Camera * obj);

        /** 在删除时调用
        @version NIIEngine 3.0.0
        */
        virtual void onDestroy(Camera * obj);

        /** 拣选后触发
        @version NIIEngine 3.0.0
        */
        virtual void onCull(Camera * obj);

        /** 渲染后触发
        @version NIIEngine 3.0.0
        */
        virtual void onRender(Camera * obj);
    };
    typedef vector<CameraListener *>::type CameraListenerList;
    
    /** 几何摄象口
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Camera : public Frustum
    {
    public:
        Camera(SpaceID sid, SpaceManager * sm = 0);
        virtual ~Camera();
        
        /** 拣选视口
        @param[in] vp 需要渲染的视口
        @version NIIEngine 3.0.0 高级api
        */
        void cull(Viewport * vp, const RenderQueueFusion * rqfusion);

        /** 渲染视口
        @param[in] vp 需要渲染的视口
        @version NIIEngine 3.0.0 高级api
        */
        void render(Viewport * vp, const RenderQueueFusion * rqfusion);

        /** 世界空间的视点.
        @param[in] v 点
        @version NIIEngine 3.0.0
        */
        void lookAt(const Vector3f & v);

        /** 移动摄象机
        @remark 世界坐标
        @version NIIEngine 3.0.0
        */
        inline void movePos(const Vector3f & v)     { mPosition += v; notifyView(); }

        /** 移动摄象机
        @remark 摄象机坐标标准
        @version NIIEngine 3.0.0
        */
        inline void moveRel(const Vector3f & v)     { mPosition += mOrientation * v; notifyView(); }

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
        @param[out] plans
        @param[in] viewleft
        @param[in] viewtop
        @param[in] viewright
        @param[in] viewbottom
        @version NIIEngine 3.0.0
        */
        void getVolume(PlaneSpace & plans, NIIf viewleft, NIIf viewtop, NIIf viewright, NIIf viewbottom);

        /** 是否启用扩展空间模式
        @remark 一般用于空间物体影响当前空间,但空间物体实际并不存在于视区
        @version NIIEngine 3.0.0
        */
        inline void setExpandMode(bool b)               { mExpandMode = b; }

        /** 是否启用扩展空间模式
        @remark 一般用于空间物体影响当前空间,但空间物体实际并不存在于视区
        @version NIIEngine 3.0.0
        */
        inline bool isExpandMode() const                { return mExpandMode; }

        /** 重至摄像区域
        @remark 一般用于空间物体影响当前空间,但空间物体实际并不存在于视区
        @version NIIEngine 3.0.0
        */
        void resetBound();

        /** 扩容摄像区域
        @remark 一般用于空间物体影响当前空间,但空间物体实际并不存在于视区
        @param[in] b1 边界盒
        @param[in] b2 范围边界
        @param[in] mark ST_Normal ST_Cast ST_Receive 复合参数
        */
        void expandBound(const AABox & b1, const Sphere & b2, Nmark mark = ShadowType::ST_Normal | ShadowType::ST_Receive);

        /** 获取摄像最近区域
        @version NIIEngine 3.0.0
        */
        inline NIIf getBoundMin(ShadowType type) const          { return mMin[type]; }

        /** 获取摄像最远区域
        @version NIIEngine 3.0.0
        */
        inline NIIf getBoundMax(ShadowType type) const          { return mMax[type]; }

        /** 获取摄像区域
        @version NIIEngine 3.0.0
        */
        inline const AABox & getBound(ShadowType type) const    { return mAABB[type]; }

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
        inline void setPos(const Vector3f & p)          { mPosition = p; notifyView(); }

        /** 获取摄象机位置
        @version NIIEngine 3.0.0
        */
        inline const Vector3f & getPos() const          { return mPosition; }

        /** 获取摄象机上方向向量.
        @version NIIEngine 3.0.0
        */
        inline Vector3f getUp() const                   { return mOrientation * Vector3f::Y;}

        /** 获取摄象机右方向向量.
        @version NIIEngine 3.0.0
        */
        inline Vector3f getRight() const                { return mOrientation * Vector3f::X; }

        /** 设置摄象机朝向
        @version NIIEngine 3.0.0
        */
        void setDirection(const Vector3f & v);

        /** 获取摄象机朝向
        @version NIIEngine 3.0.0
        */
        inline Vector3f getDirection() const            { return mOrientation * -Vector3f::Z; }

        /** 设置摄象机方向
        @version NIIEngine 3.0.0
        */
        void setOri(const Quaternion & q);

        /** 返回摄象机方向
        @version NIIEngine 3.0.0
        */
        inline const Quaternion & getOri() const        { return mOrientation; }

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
        inline void setTriangle(NCount count)       { mTriCount = count; }

        /** 获取三角形数量
        @version NIIEngine 3.0.0
        */
        inline Nui32 getTriangleCount() const       { return mTriCount; }

        /** 设置批次数量
        @version NIIEngine 3.0.0 高级api
        */
        inline void setBatch(NCount count)          { mBatchCount = count; }

        /** 获取批次数量
        @version NIIEngine 3.0.0
        */
        inline Nui32 getBatchCount() const          { return mBatchCount; }

        /** 设置使用的几何模式
        @version NIIEngine 3.0.0
        */
        inline void setPolygonMode(PolygonMode pm)  { mPolygonMode = pm; }

        /** 获取使用的几何模式.
        @version NIIEngine 3.0.0
        */
        inline PolygonMode getPolygonMode() const   { return mPolygonMode; }

        /** 设置子视口模式
        @version NIIEngine 3.0.0
        */
        inline void setSubMode(bool b)              { mSubMode = b; }

        /** 是否子视口模式
        @version NIIEngine 3.0.0
        */
        inline bool isSubMode() const               { return mSubMode; }

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
        inline void setVisiblePixelLimit(bool b)        { mPixelLimit = b; }

        /** 是否可见像素限制
        @version NIIEngine 3.0.0
        */
        inline bool isVisiblePixelLimit() const         {return mPixelLimit; }

        /** 设置可见距离限制
        @version NIIEngine 3.0.0
        */
        inline void setVisibleDistanceLimit(bool b)     { mDisLimit = b; }

        /** 是否可见距离限制
        @version NIIEngine 3.0.0
        */
        inline bool isVisibleDistanceLimit() const      { return mDisLimit; }

        /** 设置重新计算宽/高比
        @version NIIEngine 3.0.0
        */
        inline void setAutoAspectRatio(bool b)          { mAutoAspectRatio = b; }

        /** 获取重新计算宽/高比
        @version NIIEngine 3.0.0
        */
        inline bool isAutoAspectRatio() const           { return mAutoAspectRatio; }

        /** 获取渲染像素宽/高比
        @version NIIEngine 3.0.0
        */
        inline NIIf getPixelRatio() const               { return mPixelRatio; }

        /** 自动对焦
        @version NIIEngine 3.0.0
        */
        inline void setAutoFocus(bool b)                { mAutoFocus = b; }

        /** 设置对焦点
        @param[in] obj 对焦点
        @param[in] oft 对焦偏量
        @version NIIEngine 3.0.0
        */
        void setAutoFocus(PosNode * obj, const Vector3f & oft = Vector3f::ZERO);

        /** 获取对焦点
        @version NIIEngine 3.0.0
        */
        inline PosNode * getAutoFocus() const           { return mFocusNode; }

        /** 获取对焦偏量
        @version NIIEngine 3.0.0
        */
        inline const Vector3f & getFocusOffset() const  { return mFocusOffset; }

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
        inline Vector3f getSpaceUp() const              { const_cast<Camera *>(this)->updateView(); return mSpaceOrientation * Vector3f::Y; }

        /** x轴朝向
        @remark 含概所有变换
        @version NIIEngine 3.0.0
        */
        inline Vector3f getSpaceRight() const           { const_cast<Camera *>(this)->updateView(); return mSpaceOrientation * Vector3f::X; }

        /** 获取位置
        @version NIIEngine 3.0.0
        */
        inline const Vector3f & getLocalPos() const     { const_cast<Camera *>(this)->updateView(); return mLocalPos; }

        /** 获取方向
        @version NIIEngine 3.0.0
        */
        inline const Quaternion & getLocalOri() const   { const_cast<Camera *>(this)->updateView(); return mLocalOri; }

        /** y轴朝向
        @version NIIEngine 3.0.0
        */
        inline Vector3f getLocalUp() const              { const_cast<Camera *>(this)->updateView(); return mLocalOri * Vector3f::Y; }

        /** x轴朝向
        @version NIIEngine 3.0.0
        */
        inline Vector3f getLocalRight() const           { const_cast<Camera *>(this)->updateView(); return mLocalOri * Vector3f::X; }

        /** z轴朝向
        @version NIIEngine 3.0.0
        */
        inline Vector3f getLocalDirection() const       { const_cast<Camera *>(this)->updateView(); return mLocalOri * Vector3f::INV_Z; }

        /** 为这个摄象机设置lod因子.
        @version NIIEngine 3.0.0 高级api
        */
        inline void setLodBias(NIIf factor = 1.0)   { mLodFactor = factor; }

        /** 获取这个摄象机应用的lod因子.
        @version NIIEngine 3.0.0 高级api
        */
        inline NIIf getLodBias() const              { return mLodFactor; }

        /** 设置当前视口
        @version NIIEngine 3.0.0 高级api
        */
        inline void setViewport(Viewport * obj)     { mViewport = obj; }

        /** 获取当前视口
        @version NIIEngine 3.0.0 高级api
        */
        inline Viewport * getViewport() const       { return mViewport; }

        /** 使用扩展的LOD控制摄象机
        @version NIIEngine 3.0.0 高级api
        */
        inline void setExtLod(const Camera * obj)   { obj == this ? mLod = 0 : mLod = obj; }

        /** 获取扩展的LOD控制摄象机
        @version NIIEngine 3.0.0 高级api
        */
        inline const Camera * getExtLod() const     { return mLod ? mLod : this; } 

        /** 使用扩展的裁减区域
        @version NIIEngine 3.0.0 高级api
        */
        inline void setExtCulling(Frustum * obj)    { mCull = obj; }

        /** 返回扩展的裁减区域
        @version NIIEngine 3.0.0 高级api
        */
        inline Frustum * getExtCulling() const      { return mCull; }

        /** 获取视图距阵
        @param[in] 是否获取其他的裁减区域的视图距阵
        @version NIIEngine 3.0.0
        */
        inline const Matrix4f & getViewMatrix(bool cull) const  { return cull ? getViewMatrix() : Frustum::getViewMatrix(); }

        /// @copydetails SpaceObj::getFactoryID
        FactoryID getFactoryID() const;

        /// @copydetails SpaceObj::getCenterRange
        NIIf getCenterRange() const;

        /// @copydetails GeometryObj::getMatrix
        void getMatrix(Matrix4f * out, NCount & count) const;

        /// @copydetails Frustum::isIntersect
        bool isIntersect(const AABox & aabb) const;

        /// @copydetails Frustum::isIntersect
        bool isIntersect(const Sphere & s) const;

        /// @copydetails Frustum::isIntersect
        bool isIntersect(const Vector3f & v) const;

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
        AABox mAABB[ShadowType::ST_Count];  //0: normarl 1: cast 2: Receive
        NIIf mMin[ShadowType::ST_Count];
        NIIf mMax[ShadowType::ST_Count];
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
        Frustum * mCull;
        const Camera * mLod;
        PosNode * mFocusNode;
        Vector3f mFocusOffset;
        PlaneList mSubClips;

        bool mDisLimit;
        bool mExpandMode;
        bool mPixelLimit;
        bool mAutoFocus;
    };

    inline _EngineAPI Nostream & operator << (Nostream & s, const Camera & c)
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
    
    /** 阴影投射摄像机设置
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShadowSetup : public ShadowAlloc
    {
    public:
        ShadowSetup();
        virtual ~ShadowSetup();
        
        /// 设置阴影投射摄像机
        virtual void getShadowSetup(Camera * out, const RenderPattern * sm, const Camera * cam, 
            const Viewport * vp, const Light * light) const = 0;
    };
}
#endif