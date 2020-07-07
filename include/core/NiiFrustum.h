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

#ifndef _NII_FRUSTUM_H_
#define _NII_FRUSTUM_H_

#include "NiiPreInclude.h"
#include "NiiSpaceObj.h"
#include "NiiGeometryObj.h"
#include "NiiVertexData.h"
#include "NiiVector2.h"
#include "NiiPlane.h"
#include "NiiRect.h"

namespace NII
{
    /** 平截空间
    @note 空间矩阵 = 投影矩阵 * 视图矩阵, 有了空间矩阵可以确定平截面
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Frustum : public SpaceObj, public GeometryObj
    {
    public:
        /** 透视/正交投影
        @version NIIEngine 3.0.0
        */
        enum Type
        {
            T_Orthogonal,
            T_Perspective
        };

        /** 裁剪面类型
        @version NIIEngine 3.0.0
        */
        enum PlaneType
        {
            PT_Near   = 0,
            PT_Far    = 1,
            PT_Left   = 2,
            PT_Right  = 3,
            PT_Top    = 4,
            PT_Bottom = 5
        };
    public:
        Frustum();
        Frustum(SpaceID sid, SpaceManager * mag = 0);

        virtual ~Frustum();

        /** 设置投影类型(模型/透视).
        @version NIIEngine 3.0.0
        */
        inline void setProjType(Type t)         { mProjType = t; notifyProj(); }

        /** 获取投影类型(模型/透视)
        @version NIIEngine 3.0.0
        */
        inline Type getProjType() const         {return mProjType;}

        /** 设置近裁面距离.
        @version NIIEngine 3.0.0
        */
        void setNearClipDistance(NIIf extent);

        /** 获取近剪面距离.
        @version NIIEngine 3.0.0
        */
        virtual NIIf getNearClipDistance() const;

        /** 设置远裁面距离
        @version NIIEngine 3.0.0
        */
        void setFarClipDistance(NIIf extent);

        /** 设置远裁面距离.
        @version NIIEngine 3.0.0
        */
        virtual NIIf getFarClipDistance() const;

        /** 设置宽高比
        @remark aspect ratio = width / height
        @version NIIEngine 3.0.0
        */
        virtual void setAspectRatio(NIIf ratio);

        /** 获取宽高比
        @version NIIEngine 3.0.0
        */
        virtual NIIf getAspectRatio() const;

        /** 设置单位比
        @version NIIEngine 3.0.0
        */
        virtual void setAspectUnit(NIIf unit);

        /** 获取单位比
        @version NIIEngine 3.0.0
        */
        virtual NIIf getAspectUnit() const;

        /** 设置平截头偏量,用于立体的渲染.
        @param[in] oft
        @version NIIEngine 3.0.0
        */
        virtual void setAspectOffset(const Vector2f & oft);

        /** 获取平截头偏量,用于立体的渲染
        @version NIIEngine 3.0.0
        */
        virtual const Vector2f & getAspectOffset() const;

        /** 设置这个平截头视野范围.
        @note 正交轴
        @version NIIEngine 3.0.0
        */
        void setFOV(const Radian & angle);

        /** 获取这个平截头视野范围.
        @note 正交轴
        @version NIIEngine 3.0.0
        */
        const Radian & getFOV() const;

        /** 设置偏转
        @remark 正交方向
        @version NIIEngine 3.0.0
        */
        void setDirection(NIIf dir);

        /** 获取偏转
        @version NIIEngine 3.0.0
        */
        NIIf getDirection() const;

        /** 是否自定义区域
        @version NIIEngine 3.0.0
        */
        void setCustomAspect(bool b);

        /** 设置自定义区域
        @param[in]
        @version NIIEngine 3.0.0
        */
        void setCustomAspect(NIIf left, NIIf right, NIIf top, NIIf bottom);

        /** 设置正交投影区域
        @param[in] w 宽
        @param[in] h 高
        @version NIIEngine 3.0.0
        */
        void setOrthoAspect(NIIf w, NIIf h);

        /** 设置正交投影宽
        @version NIIEngine 3.0.0
        */
        inline NIIf getOrthoAspectWidth() const             { return mOrthoWidth; }

        /** 设置正交投影高
        @version NIIEngine 3.0.0
        */
        inline NIIf getOrthoAspectHeight() const            { return mOrthoHeight; }

        /** 获取裁剪面
        @note 世界空间
        @version NIIEngine 3.0.0
        */
        virtual const Plane & getClipPlane(PlaneType plane) const;

        /** 设置是否反射
        @version NIIEngine 3.0.0
        */
        void setReflection(bool b);

        /** 获取是否反射
        @version NIIEngine 3.0.0
        */
        inline bool isReflection() const            { return mReflect;  }

        /** 设置反射平面
        @version NIIEngine 3.0.0
        */
        void setReflection(const Plane & p);

        /** 获取反射平面
        @version NIIEngine 3.0.0
        */
        inline const Plane & getReflection() const  { return mReflectPlane; }

        /** 是否有交集
        @version NIIEngine 3.0.0
        */
        virtual bool isIntersect(const AABox & aabb) const;

        /** 是否有交集
        @version NIIEngine 3.0.0
        */
        virtual bool isIntersect(const Vector3f & v) const;

        /** 是否有交集
        @version NIIEngine 3.0.0
        */
        virtual bool isIntersect(const Sphere & s) const;

        /** 获取投射球到近裁剪面的边界矩形
        @param[in] in 球
        @param[in] out 边界矩形
        @return 如果投射球包含了整个近裁剪面则返回 false
        */
        virtual bool project(const Sphere & in, Rectf & out) const;

        /** 获取裁剪面的8个角
        @param[out]
            [0]前右上交点 [1]前左上交点 [2]前左下交点 [3]前右下交点
            [4]远右上交点 [5]远左上交点 [6]远左下交点 [7]远右下交点
        @version NIIEngine 3.0.0
        */
        virtual const Vector3f * getClipPlaneIntersect() const;

        /** 设置自定义近裁减面
        @remark 透视投影专用
        @param[in] plane
        @version NIIEngine 3.0.0
        */
        void setCustomNearClip(const Plane & plane);

        /** 设置是否使用自定义近裁减面
        @version NIIEngine 3.0.0
        */
        void setCustomNearClip(bool b);

        /** 自定义近裁剪面在使用?
        @version NIIEngine 3.0.0
        */
        inline bool isCustomNearClip() const        { return mOblique; }

        /** 设置是否使用自定义投影矩阵
        @version NIIEngine 3.0.0 高级api
        */
        inline void setCustomProjMatrix(bool b)     { mCustomProjMatrix = b; }

        /** 获取是否使用自定义投影矩阵
        @version NIIEngine 3.0.0 高级api
        */
        inline bool isCustomProjMatrix() const      { return mCustomProjMatrix; }

        /** 设置自定义投影矩阵
        @version NIIEngine 3.0.0
        */
        void setCustomProjMatrix(const Matrix4f & proj);

        /** 获取投影矩阵
        @note 标准数学/右手坐标系/z[-1,+1]
        @version NIIEngine 3.0.0 高级api
        */
        const Matrix4f & getProjMatrix() const;

        /** 设置是否使用自定义视图矩阵
        @version NIIEngine 3.0.0 高级api
        */
        inline void setCustomViewMatrix(bool b)             { mCustomViewMatrix = b; }

        /** 获取是否使用自定义视图矩阵
        @version NIIEngine 3.0.0 高级api
        */
        inline bool isCustomViewMatrix() const              { return mCustomViewMatrix; }

        /** 获取视图矩阵
        @version NIIEngine 3.0.0
        */
        void setCustomViewMatrix(const Matrix4f & view);

        /** 获取视图矩阵
        @version NIIEngine 3.0.0
        */
        virtual const Matrix4f & getViewMatrix() const;

        /** 获取投影矩阵
        @note 获取渲染系统投影距阵(opengl/dx坐标系不一样)/z[-1,+1]
        @version NIIEngine 3.0.0 高级api
        */
        const Matrix4f & getSysProjMatrix() const;

        /** 获取反射矩阵
        @version NIIEngine 3.0.0
        */
        inline const Matrix4f & getReflectionMatrix() const    { return mReflectMatrix; }

        /** 获取所在节点位置
        @version NIIEngine 3.0.0
        */
        virtual const Vector3f & getNodePos() const;

        /** 获取所在节点方向
        @version NIIEngine 3.0.0
        */
        virtual const Quaternion & getNodeOri() const;

        /// @copydetails SpaceObj::getFactoryID
        FactoryID getFactoryID() const;

        /// @copydetails SpaceObj::getAABB
        const AABox & getAABB() const;

        /// @copydetails SpaceObj::getCenterRange
        NIIf getCenterRange() const;

        /// @copydetails SpaceObj::queue
        void queue(RenderQueue * queue);

        /// @copydetails SpaceObj::query
        void query(GeometryQuery * query);

        /// @copydetails SpaceObj::_notify
        void _notify(Camera * cam);

        /// @copydetails GeometryObj::getMaterial
        const Material * getMaterial() const;

        /// @copydetails GeometryObj::getGeometry
        void getGeometry(GeometryRaw & op) const;

        /// @copydetails GeometryObj::getMatrix
        void getMatrix(Matrix4f * out, NCount & count) const;

        /// @copydetails GeometryObj::distanceSquared
        NIIf distanceSquared(const Camera * cam) const;

        /// @copydetails GeometryObj::getLight
        void getLight(LightList & ll) const;

        /// @copydetails SpaceObj::getTypeMark
        Nui32 getTypeMark() const;
    protected:
        /** 投影空间是否有效
        @version NIIEngine 3.0.0
        */
        virtual bool isProjValid() const;

        /** 视图空间是否有效
        @version NIIEngine 3.0.0
        */
        virtual bool isViewValid() const;

        /** 更新投影
        @version NIIEngine 3.0.0
        */
        virtual void updateProj();

        /** 更新视口
        @version NIIEngine 3.0.0
        */
        virtual void updateView();

        /** 更新平截头的面
        @version NIIEngine 3.0.0
        */
        virtual void updateFrustumPlanes();

        /** 更新视窗
        @version NIIEngine 3.0.0
        */
        virtual void updateAspect(NIIf & left, NIIf & right, NIIf & bottom, NIIf & top) const;

        /** 实现投影矩阵更新
        @version NIIEngine 3.0.0
        */
        virtual void updateProjImpl();

        /** 实现视图矩阵更新
        @version NIIEngine 3.0.0
        */
        virtual void updateViewImpl() ;

        /** 发出更新平截头信息信号
        @version NIIEngine 3.0.0
        */
        virtual void notifyProj() const;

        /** 发出更新视口信息信号
        @version NIIEngine 3.0.0
        */
        virtual void notifyView() const;
        
        /** 更新视图矩阵
        @version NIIEngine 3.0.0
        */
        void updateViewMatrix(const Vector3f & pos, const Quaternion & ori, const Matrix4f * reflect = 0);
    protected:
        NIIf mFarExtent;
        NIIf mNearExtent;
        NIIf mUnit;                 ///< 单元/距离
        NIIf mAspect;               ///< 视口宽/高比
        Radian mFOV;                ///< 视角
        Vector2f mOffset;
        NIIf mOrthoWidth;
        NIIf mOrthoHeight;
        Type mProjType;
        Material * mMaterial;
        NIIf mDirection;
        mutable NIIf mLeft;
        mutable NIIf mTop;
        mutable NIIf mRight;
        mutable NIIf mBottom;
        mutable AABox mBoundingBox;
        mutable Plane mFrustumPlanes[6];
        mutable Vector3f mPlaneIntersect[8];
        mutable Matrix4f mViewMatrix;
        mutable Matrix4f mProjMatrix;
        mutable Matrix4f mSysProjMatrix;
        mutable Matrix4f mReflectMatrix;
        mutable Vector3f mLastNodePos;
        mutable Quaternion mLastNodeDir;
        mutable Plane mReflectPlane;
        mutable Plane mObliqueProjPlane;
        mutable Plane mLastReflectPlane;
        mutable Plane mLastObliqueProjPlane;
        mutable VertexData mVertexData;
        mutable bool mProjValid;
        mutable bool mViewValid;
        mutable bool mPlaneValid;
        mutable bool mPlaneIntersectValid;
        mutable bool mDebugVertexValid;
        bool mReflect;
        bool mOblique;
        bool mCustomAspect;
        bool mCustomViewMatrix;
        bool mCustomProjMatrix;
    };
}
#endif