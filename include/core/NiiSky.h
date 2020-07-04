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

#ifndef _NII_SKY_H_
#define _NII_SKY_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 天空
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Sky : public SpaceAlloc
    {
    public:
        enum PlaneType
        {
            PT_Front = 0,
            PT_Back = 1,
            PT_Left = 2,
            PT_Right = 3,
            PT_Up = 4,
            PT_Down = 5
        };
    public:
        Sky();
        Sky(const SpaceManager * sm);
        virtual ~Sky();

        /** 把天空渲染到设想机中
        @param[in] obj 摄象机
        @version NIIEngine 3.0.0
        */
        virtual void render(Camera * obj);

        /** 设置空间域
        @version NIIEngine 3.0.0
        */
        void setSpace(SpaceManager * sm)                { mParent = sm; }
        
        /** 获取空间域
        @version NIIEngine 3.0.0
        */
        SpaceManager * getSpace() const                 { return mParent; }
        
        /** 设置渲染组
        @version NIIEngine 3.0.0
        */
        inline void setRenderGroup(GroupID gid)         { mRenderGroup = gid; }

        /** 获取渲染组
        @version NIIEngine 3.0.0
        */
        inline GroupID getRenderGroup()const            { return mRenderGroup; }

        /** 设置资源组
        @version NIIEngine 3.0.0
        */
        inline void setResourceGroup(GroupID gid)       { mResourceGroup = gid; }

        /** 获取资源组
        @version NIIEngine 3.0.0
        */
        inline GroupID getResourceGroup() const         { return mResourceGroup; }

        /** 设置是否启用
        @version NIIEngine 3.0.0
        */
        inline void setEnable(bool b)                   { mEnable = b; }

        /** 获取是否启用
        @version NIIEngine 3.0.0
        */
        inline bool isEnable() const                    { return mEnable; }
    protected:
        /** 渲染到队列中
        @version NIIEngine 3.0.0 高级api
        */
        virtual void renderImpl(RenderQueue * rq) = 0;
    protected:
        SpaceManager * mParent;
        GroupID mRenderGroup;
        GroupID mResourceGroup;
        SpaceNode * mNode;
        bool mEnable;
    };
    
    /** 盒天空
    @version NIIEngine 3.0.0
    */
    class _EngineAPI BoxSky : public Sky
    {
    public:
        BoxSky();
        BoxSky(const SpaceManager * gm);
        virtual ~BoxSky();
        
        /** 设置材质
        @param[in] rid 资源ID
        @version NIIEngine 3.0.0
        */
        void setMaterial(ResourceID rid);
        
        /** 设置方向偏量
        @version NIIEngine 3.0.0
        */
        inline void setOriOft(const Quaternion & o)     { mOriOft = o;  }
        
        /** 获取方向偏量
        @version NIIEngine 3.0.0
        */
        inline const Quaternion & getOriOft() const     { return mOriOft;  }
        
        /** 构建几何
        @version NIIEngine 3.0.0
        */
        void buildGeo(NIIf distance, const Quaternion & ori_oft);
    
        /** 获取与视口的距离
        @version NIIEngine 3.0.0
        */
        inline NIIf getRange() const                    { return mRange; }
    protected:
        /// @copydetails BoxSky::renderImpl
        virtual void renderImpl(RenderQueue * rq);
    protected:
        NIIf mRange;
        CustomSpaceObj * mGeo;
        Quaternion mOriOft;
        ResourceID mMaterial[7];
    };
    
    /** 穹天空
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DomeSky : public Sky
    {
    public:
        DomeSky();
        DomeSky(const SpaceManager * gm);
        virtual ~DomeSky();

        /** 设置材质
        @version NIIEngine 3.0.0
        */
        void setMaterial(ResourceID rid);

        /** 构建几何
        @version NIIEngine 3.0.0
        */
        void buildGeo(NIIf cuvfactor = 10, NIIf range = 1000, NIIf xtiling = 20,
            NIIf ytiling = 20, NIIi xsegments = 20, NIIi ysegments = 20,
                const Quaternion & ori_oft = Quaternion::IDENTITY);
    protected:
        /// @copydetails Sky::renderImpl
        virtual void renderImpl(RenderQueue * rq);

        /** 创建一个面
        @version NIIEngine 3.0.0
        */
        Mesh * createPlaneImpl(ResourceID & rid, GroupID gid, PlaneType bp,
            NIIf cuvfactor, NIIf range, const Quaternion & ori_oft,
                NIIf xtiling, NIIf ytiling, NIIi xsegments, NIIi ysegments) const;
    protected:
        SpaceNode * mNode;
        GeometrySpace * mGeo[5];
        ResourceID mResource[5];
        ResourceID mMaterial;
        NIIf mRange;
        NIIf mCurveFactor;
        NIIf mXTiling;
        NIIf mYTiling;
        NIIi mXSegment;
        NIIi mYSegment;
    };
    
    /** 面天空
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PlaneSky : public Sky
    {
    public:
        PlaneSky();
        PlaneSky(const SpaceManager * gm);
        virtual ~PlaneSky();

        /** 设置材质
        @param[in] rid 资源ID
        @version NIIEngine 3.0.0
        */
        void setMaterial(ResourceID rid);

        /** 构建几何
        @version NIIEngine 3.0.0
        */
        void buildGeo(const Plane & plane, NIIf scale = 1000, NIIf xtiling = 10,
            NIIf ytiling = 10, NIIi xsegments = 10, NIIi ysegments = 10, NIIf bow = 0);

        /** 天空面的大小因子
        @return 正比
        @version NIIEngine 3.0.0
        */
        inline NIIf getScale() const                { return mScale;  }

        /** 天空面的弯曲因子
        @return 整数存在
        @version NIIEngine 3.0.0
        */
        inline NIIf getBow() const                  { return mBow;  }

        /** 获取天空平面的定义
        @version NIIEngine 3.0.0
        */
        inline const Plane & getPlane() const       { return mPlane; }
    protected:
        /// @copydetails Sky::renderImpl
        virtual void renderImpl(RenderQueue * rq);
    protected:
        Plane mPlane;
        NIIf mScale;
        NIIf mXTiling;
        NIIf mYTiling;
        NIIi mXSegment;
        NIIi mYSegment;
        NIIf mBow;
        ResourceID mMaterial;
        ResourceID mMesh;
        GeometrySpace * mGeo;
    };
    
    /** 一般模拟极限天空
    @remark 一般用于粒子系统无法表达,或效率很差的粒子系统的代替方式
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VideoSky : public Sky
    {
    public:
        VideoSky();
        VideoSky(const SpaceManager * gm);
        virtual ~VideoSky();

        /** 设置视频资源
        @version NIIEngine 3.0.0
        */
        inline void setVideo(ResourceID rid)        { mVideoRID = rid; }

        /** 获取视频资源
        @version NIIEngine 3.0.0
        */
        inline ResourceID getVideo() const          { return mVideoRID; }

        /** 获取平面的定义
        @version NIIEngine 3.0.0
        */
        inline const Plane & getPlane() const       { return mPlane; }

        /** 设置时间因子
        @version NIIEngine 3.0.0
        */
        inline void setTimeFactor(NIIf f)           { mTimeFactor = f; }

        /** 获取时间因子
        @version NIIEngine 3.0.0
        */
        inline NIIf getTimeFactor() const           { return mTimeFactor; }

        /** 构建几何
        @version NIIEngine 3.0.0
        */
        void buildGeo(const Plane & plane, NIIf scale = 1000, NIIf width = 1000, NIIf height = 1000);
    protected:
        /// @copydetails Sky::renderImpl
        virtual void renderImpl(RenderQueue * rq);
    protected:
        ResourceID mVideoRID;
        Plane mPlane;
        NIIf mTimeFactor;
        Texture * mGeo;
    };
}

#endif