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

#ifndef _NII_ShadowSetupFocused_H_
#define _NII_ShadowSetupFocused_H_

#include "NiiPreInclude.h"

#include "NiiCamera.h"
#include "NiiPolygon.h"
#include "NiiConvexBody.h"

namespace NII
{
    class ConvexBody;
    
    /** 实现平面最佳阴影摄象机算法
    @remark
        Given a plane of interest, it is possible to set up the shadow camera
        matrix such that the mapping between screen and shadow map is the identity
        (when restricted to pixels that view the plane of interest).  Therefore,
        if the shadow map resolution matches the screen space resolution (of the
        seen planar receiver), we can get pixel perfect shadowing on the plane.
        Off the plane, the shadowing is not guaranteed to be perfect and will
        likely exhibit the usual sampling artifacts associated with shadow mapping.
    @note
        Important: this routine requires NIId-precision calculations. When you
        are running under Direct3D, you must ensure that you set the floating
        point mode to 'Consistent' rather than 'Fastest' to ensure this precision.
        This does allegedly come with some performance cost but when measuring
        it appears to be negligible in modern systems for normal usage.
    @note
        Second important note: this projection also only works for lights with
        a finite position. Therefore you cannot use it for directional lights
        at this time.
    */
    class _EngineAPI PlaneOptimalShadowSetup : public CameraSetup
    {
    public:
        PlaneOptimalShadowSetup(PlaneSpaceObj * plane);
        virtual ~PlaneOptimalShadowSetup();

        /// 返回当限制平面中时,在屏幕和阴影映射间配置1-1单应矩阵的阴影摄象机
        virtual void setupShadow(Camera * texCam, const SpaceManager * sm, const Camera * cam, Light * light, NCount level) const;
    private:
        PlaneOptimalShadowSetup(){}

        /** 前方项目平截头射线寻找前方交叉平面.
        @remark
            Forward projection may lead to intersections at infinity.
        */
        virtual void forwardIntersect(const Camera * src, const Plane & worldPlane,
            vector<Vector4f >::type * intersect3d) const;

        /// Helper function for forwardIntersect that intersects rays with canonical plane
        virtual vector<Vector4f >::type getRayForwardIntersect(
            const Vector3f & anchor, const Vector3f * dir, NIIf planeOffset) const;

        /// 辅助函数,给定的约束计算投影矩阵
        Matrix4f computeConstrainedProjection(const Vector4f & pinhole,
            const vector<Vector4f >::type & fpoint, const vector<Vector2f >::type & constraint) const;
    private:
        PlaneSpaceObj * mPlane;              ///< pointer to plane of interest
    };

    /** 实现集中模式统一摄象机描图算法
    @remark
        Differs from the default shadow mapping projection in that it focuses the
        shadow map on the visible areas of the scene. This results in better
        shadow map texel usage, at the expense of some 'swimming' of the shadow
        texture on receivers as the basis is constantly being reevaluated.
    @note
        Original implementation by Matthias Fink <matthias.fink@web.de>, 2006.
    */
    class _EngineAPI FocusedShadowSetup : public CameraSetup
    {
    protected:
        /// 内部类保存描述一个凸体的点列表.
        class _EngineAPI PointListBody
        {
        public:
            PointListBody();
            PointListBody(const ConvexBody & body);

            /** 合并第二PointListBody到这个对象
            */
            void merge(const PointListBody & plb);

            /** 从一个`真`身,建立一个点列表阀体.
            @remark Inserts all vertices from a body into the point list with or without adding duplicate vertices.
            */
            void build(const ConvexBody & body, bool filterDuplicates = true);

            /** 从一个阀体构建一个PointListBody ,还有包含所有给定方向的空间
            @remark
                Intersects the bounding box with a ray from each available point of the body with the given
                direction. Base and intersection points are stored in a PointListBody structure.
            @note
                Duplicate vertices are not filtered.
            @note
                Body is not checked for correctness.
            */
            void buildAndIncludeDirection(const ConvexBody & body, NIIf extrudeDist, const Vector3f & dir);

            /** 返回轴对齐盒.
            @version NIIEngine 3.0.0
            */
            inline const AABox & getAAB() const                 { return mAAB; }

            /** 添加一个特定的点到阀体列表.
            @param[in] point
            @version NIIEngine 3.0.0
            */
            void addPoint(const Vector3f & point);

            /** 添加一个轴对齐盒到所有点
            @param[in] aab
            @version NIIEngine 3.0.0
            */
            void addAAB(const AABox & aab);

            /** 返回一个点
            @param[in] cnt
            @version NIIEngine 3.0.0
            */
            inline const Vector3f & getPoint(NCount cnt) const  { N_assert1(cnt < mBodyPoints.size()); return mBodyPoints[cnt]; }

            /** 返回点的总数
            @version NIIEngine 3.0.0
            */
            inline NCount getPointCount() const                 { return mBodyPoints.size(); }

            /// 重置阀体
            void reset();
        private:
            Polygon::VertexList mBodyPoints;
            AABox mAAB;
        };
    public:
        FocusedShadowSetup();
        virtual ~FocusedShadowSetup();

        /// 返回一个以聚焦视图的统一阴影摄象机.
        virtual void setupShadow(Camera * texCam, const SpaceManager * sm, const Camera * cam, const Light * light, NCount level) const;

        /** 设置是否使用采取更激进的方式决定聚焦地区
        @note
            There are 2 approaches that can  be used to define the focus region,
            the more aggressive way introduced by Wimmer et al, or the original
            way as described in Stamminger et al. Wimmer et al's way tends to 
            come up with a tighter focus region but in rare cases (mostly highly
            glancing angles) can cause some shadow casters to be clipped 
            incorrectly. By default the more aggressive approach is used since it
            leads to significantly better results in most cases, but if you experience
            clipping issues, you can use the less aggressive version.
        @param[in] b True to use the more aggressive approach, false otherwise.
        */
        inline void setUseAggressiveFocusRegion(bool b)         { mUseAggressiveRegion = b; }

        inline bool getUseAggressiveFocusRegion() const         { return mUseAggressiveRegion; }
    protected:
        /** 计算标准的阴影描图矩阵.
        @remark
            Provides the view and projection matrix for standard shadow mapping.
        @note
            You can choose which things you want to have: view matrix and/or projection 
            matrix and/or shadow camera. Passing a NULL value as parameter ignores the
            generation of this specific value.
        @param[in] sm: scene manager
        @param[in] cam: currently active camera
        @param[in] light: currently active light
        @param[in] out_view: calculated uniform view shadow mapping matrix (may be NULL)
        @param[in] out_proj: calculated uniform projection shadow mapping matrix (may be NULL)
        @param[in] out_cam: calculated uniform shadow camera (may be NULL)
        */
        void calculateShadowMappingMatrix(const SpaceManager & sm, const Camera & cam, 
            const Light & light, Matrix4f * out_view, Matrix4f * out_proj, Camera * out_cam) const;

        /** 计算交叉bodyB.
        @remark
            The intersection bodyB consists of the concatenation the cam frustum clipped 
            by the scene bounding box followed by a convex hullification with the light's 
            position and the clipping with the scene bounding box and the light frustum:
            ((V \cap S) + l) \cap S \cap L (\cap: convex intersection, +: convex hull 
            operation).
            For directional lights the bodyB is assembled out of the camera frustum 
            clipped by the scene bounding box followed by the extrusion of all available 
            bodyB points towards the negative light direction. The rays are intersected 
            by a maximum bounding box and added to the bodyB points to form the final 
            intersection bodyB point list.
        @param[in] sm: scene manager
        @param[in] cam: currently active camera
        @param[in] light: currently active light
        @param[in] sceneBB: scene bounding box for clipping operations
        @param[in] receiverAABB: bounding information for just the receivers
        @param[in] out_bodyB: final intersection bodyB point list
        */
        void calculateB(const SpaceManager & sm, const Camera & cam, const Light & light, 
            const AABox & sceneBB, const AABox & receiverBB, PointListBody * out_bodyB) const;

        /** 计算 bodyLVS.
        @remark
            Calculates the bodyLVS which consists of the convex intersection operation 
            affecting the light frustum, the view frustum, and the current scene bounding
            box is used to find suitable positions in the viewer's frustum to build the 
            rotation matrix L_r. This matrix is applied after the projection matrix L_p to 
            avoid an accidental flip of the frustum orientation for views tilted with 
            respect to the shadow map.
        @param[in] scene: holds all potential occluders / receivers as one single bounding box
            of the currently active scene node
        @param[in] cam: current viewer camera
        @param[in] light: current light
        @param[in] out_LVS: intersection body LVS (world coordinates)
        */
        void calculateLVS(const SpaceManager & sm, const Camera & cam, const Light & light,
            const AABox & sceneBB, PointListBody * out_LVS) const;

        /** 返回投影视图方向.
        @remark
            After the matrix L_p is applied the orientation of the light space may tilt for
            non-identity projections. To prevent a false shadow cast the real view direction
            is evaluated and applied to the light matrix L.
        @param[in] lightSpace: matrix of the light space transformation
        @param[in] cam: current viewer camera
        @param[in] bodyLVS: intersection body LVS (relevant space in front of the camera)
        */
        Vector3f getLSProjViewDir(const Matrix4f & lightSpace, const Camera & cam, 
            const PointListBody & bodyLVS) const;

        /** 返回一个摄象机观看的有效近-点.
        @remark
            Returns a point that is situated near the camera by analyzing the bodyLVS that
            contains all the relevant scene space in front of the light and the camera in
            a point list array. The view matrix is relevant because the nearest point in
        front of the camera should be determined.
        @param[in] viewMatrix: view matrix of the current camera
        @param[in] bodyLVS: intersection body LVS (relevant space in front of the camera) 
        */
        Vector3f getNearCameraPoint_ws(const Matrix4f & viewMatrix, const PointListBody & bodyLVS) const;

        /** 变换一个给定阀体到单位立方体(-1,-1,-1)/(1,+1,+1)和启用一个指定的阴影矩阵
        @remark
            Transforms a given point list body object with the matrix m and then maps its
            extends to a (-1,-1,-1) / (+1,+1,+1) unit cube
        @param[in] m: transformation matrix applied on the point list body
        @param[in] body: contains the points of the extends of all valid scene elements which 
        are mapped to the unit cube
        */
        Matrix4f transformToUnitCube(const Matrix4f & m, const PointListBody & body) const;

        /** 建立视图矩阵
        @remark
            Builds a standard view matrix out of a given position, direction and up vector.
        */
        Matrix4f buildViewMatrix(const Vector3f & pos, const Vector3f & dir, const Vector3f & up) const;        
    protected:
        /** 变换到 或 来自Wimmer定义灯光空间等
        @remark
            Point and spot lights need to be converted to directional lights to enable a 1:1 
            light mapping. Otherwise a directional light may become a point light or a point 
            sink (opposite of a light source) or point/spot lights may become directional lights
            or light sinks. The light direction is always -y.
        */
        static const Matrix4f msNormalToLightSpace;
        static const Matrix4f msLightSpaceToNormal;

        /// 临时预分配平截头到::calculateShadowMappingMatrix()建立的投影矩阵
        Frustum * mTempFrustum;

        /// Temporary preallocated camera to set up a light frustum for clipping in ::calculateB.
        Camera * mLightFrustumCamera;
        mutable bool mLightFrustumCameraCalculated;

        /// 使用更严格的对焦区域?
        bool mUseAggressiveRegion;

        // Persistent calculations to prevent reallocation
        mutable ConvexBody mBodyB;
        mutable PointListBody mPointListBodyB;
        mutable PointListBody mPointListBodyLVS;
    };
    
    /** 实现灯光空间透视阴影描图算法
    @remark
        Implements the LiSPSM algorithm for an advanced shadow map generation. LiSPSM was
        developed by Michael Wimmer, Daniel Scherzer and Werner Purgathofer of the TU Wien.
        The algorithm was presented on the Eurographics Symposium on Rendering 2004.
    @note
        Shadow mapping was introduced by Williams in 1978. First a depth image is rendered
        from the light's view and compared in a second pass with depth values of the normal 
        camera view. In case the depth camera's depth value is greater than the depth seen
        by the light the fragment lies in the shadow.
        The concept has a major draw back named perspective aliasing. The shadow map distri-
        butes the samples uniformly meaning the position of the viewer is ignored. For the 
        viewer however the perspective projection affects near objects to be displayed 
        bigger than further away objects. The same thing happens with the shadow map texels:
        Near shadows appear very coarse and far away shadows are perfectly sampled.
        In 2002 Stamminger et al. presented an algorithm called Perspective Shadow Maps 
        (PSM). PSM battles the perspective aliasing by distributing 50% of the shadow map 
        texels for objects in the range of <near clipping plane> to <near clipping plane * 2>
        which inverts the problem: The shadows near the viewer are perfectly sampled, 
        however far away shadow may contain aliasing artefacts. A near clipping plane may be
        a problem. But this is not the only one. In the post-perspective space the light 
        sources are non-intuitively mapped: Directional lights may become point light and 
        point lights may become directional lights. Also light sinks (opposite of a light 
        source) may appear.    Another problem are shadow casters located behind the viewer. 
        In post-projective space objects behind the viewer are mapped in front of him with 
        a flipped up-vector.
        LiSPSM battles the light source problem of the post-projective space by rearranging
        the light space before transformation in such a way that no special cases appear. 
        This is done by converting point/spot lights into directional lights. The light 
        space is arranged in such a way that the light direction equals the inverse Y.
        In this combination    the directional light will neither change its type nor its 
        direction. Furthermore all visible objects and shadow casters affecting the user's 
        visible area lie in front of the shadow camera: After building the intersection body
        that contains all these objects (body intersection building was introduced with PSM; 
        have a look at the description for the method "calculateB" for further info) a 
        frustum around the body's light space bounding box is created. A parameter (called 
        'n') automatically adjusts the shadow map sample distribution by specifying the 
        frustum's view point - near plane which affects the perspective warp. In case the 
        distance is small the perspecive warp will be strong. As a consequence near objects 
        will gain quality.
        However there are still problems. PSM as well as LiSPSM only devote to minimize
        perspective aliasing. Projection aliasing is still a problem, also 'swimming 
        artefacts' still occur. The LiSPSM quality distribution is very good but not the 
        best available: Some sources say logarithmic shadow mapping is the non plus ultra, 
        however others reject this thought. There is a research project on logarithmic shadow 
        maps. The web page url is http://gamma.cs.unc.edu/logsm/. However there is no techical 
        report available yet (Oct 23rd, 2006).
    @note
        More information can be found on the webpage of the TU Wien: 
        http://www.cg.tuwien.ac.at/research/vr/lispsm/
    @note
        Original implementation by Matthias Fink <matthias.fink@web.de>, 2006.
    */
    class _EngineAPI LiSPSMShadowSetup : public FocusedShadowSetup
    {
    public:
        LiSPSMShadowSetup();
        virtual ~LiSPSMShadowSetup();

        /** 返回一个LiSPSM阴影摄象机.
        @remark
            Builds and returns a LiSPSM shadow camera. 
            More information can be found on the webpage of the TU Wien: 
            http://www.cg.tuwien.ac.at/research/vr/lispsm/
        */
        virtual void setupShadow(Camera * texCam, const SpaceManager * sm, const Camera * cam, const Light * light, NCount level) const;

        /** 调整参数,产生最佳的阴影.
        @remark
            The smaller the parameter n, the stronger the perspective warping effect.
            The consequence of a stronger warping is that the near shadows will gain 
            quality while the far ones will lose it. Depending on your scene and light
            types you may want to tweak this value - for example directional lights
            tend to benefit from higher values of n than other types of light, 
            especially if you expect to see more distant shadows (say if the viewpoint is
            higher above the ground plane). Remember that you can supply separate
            CameraSetup instances configured differently per light if you wish.
        @param[in] n The adjustment factor - default is 0.1f. 
        */
        virtual void setOptimalAdjustFactor(NIIf n)     { mOptAdjustFactor = n; }
        
        /** 获取用于生产最佳的阴影的参数
        @see setOptimalAdjustFactor
        */
        virtual NIIf getOptimalAdjustFactor() const     { return mOptAdjustFactor; }
        
        /// 设置是否使用摄像机近点推导的稍微简单版本(默认真)
        virtual void setUseSimpleOptimalAdjust(bool s)  { mUseSimpleNOpt = s; }
        
        /// 获取是否使用摄像机近点推导的稍微简单版本(默认真)
        virtual bool getUseSimpleOptimalAdjust() const  { return mUseSimpleNOpt; }

        /** 设置在摄象机和灯光方向下的临界值.其中LiSPSM投影'扁平化',因为在使
            用透视倾斜下灯光和摄象机投影产生问题
        @remark
            For example, setting this to 20 degrees will mean that as the difference 
            between the light and camera direction reduces from 20 degrees to 0
            degrees, the perspective skew will be proportionately removed.
        */
        virtual void setCameraLightDirectionThreshold(Angle angle);

        /** 获取在摄象机和灯光方向下的临界值.其中LiSPSM投影'扁平化',因为在使
            用透视倾斜下灯光和摄象机投影产生问题
        */
        virtual Angle getCameraLightDirectionThreshold() const;
    protected:
        /** 计算LiSPSM投影矩阵P.
        @remark
            The LiSPSM projection matrix will be built around the axis aligned bounding box 
            of the intersection body B in light space. The distance between the near plane 
            and the projection center is chosen in such a way (distance is set by the para-
            meter n) that the perspective error is the same on the near and far    plane. In 
            case P equals the identity matrix the algorithm falls back to a uniform shadow
            mapping matrix.
        @param[in] lightSpace: matrix of the light space transformation
        @param[in] bodyB: intersection body B
        @param[in] bodyLVS: intersection body LVS (relevant space in front of the camera)
        @param[in] sm: scene manager
        @param[in] cam: currently active camera
        @param[in] light: currently active light
        */
        Matrix4f calculateLiSPSM(const Matrix4f & lightSpace, const PointListBody & bodyB, 
            const PointListBody & bodyLVS, const SpaceManager & sm, const Camera & cam,
                const Light & light) const;

        /** 计算摄象机位置和近裁剪平面间的距离
        @remark
            n_opt determines the distance between light space origin (shadow camera position)
            and    the near clipping plane to achieve an optimal perspective foreshortening effect.
            In this    way the texel distribution over the shadow map is controlled.

            公式:
                           d
            n_opt = ---------------
                    sqrt(z1/z0) - 1

            Parameters:
            d: distance between the near and the far clipping plane
            z0: located on the near clipping plane of the intersection body b
            z1: located on the far clipping plane with the same x/y values as z0        
        @note
            A positive value is applied as the distance between viewer and near clipping plane.
            In case null is returned uniform shadow mapping will be applied.
        @param[in] lightSpace: matrix of the light space transformation
        @param[in] bodyBABB_ls: bounding box of the tranformed (light space) bodyB
        @param[in] bodyLVS: point list of the bodyLVS which describes the scene space which is in 
            front of the light and the camera
        @param[in] cam: currently active camera
        */
        NIIf calculateNOpt(const Matrix4f & lightSpace, const AABox & bodyBABB_ls, 
            const PointListBody & bodyLVS, const Camera & cam) const;

        /// 计算一个比以上更简单版本.
        NIIf calculateNOptSimple(const PointListBody & bodyLVS, const Camera & cam) const;

        /** 为n_opt计算,计算在近面的视点
        @remark
            z0 lies on the parallel plane to the near plane through e and on the near plane of 
        the frustum C (plane z = bodyB_zMax_ls) and on the line x = e.x.
        @param[in] lightSpace: matrix of the light space transformation
        @param[in] e: the LiSPSM parameter e is located near or on the near clipping plane of the 
            LiSPSM frustum C
        @param[in] bodyB_zMax_ls: maximum z-value of the light space bodyB bounding box
        @param[in] cam: currently active camera
        */
        Vector3f calculateZ0_ls(const Matrix4f & lightSpace, const Vector3f & e, 
            NIIf bodyB_zMax_ls, const Camera & cam) const;

        /** 建立一个平截头矩阵
        @remark
            Builds a standard frustum matrix out of the distance infos of the six frustum 
            clipping planes.
        */
        Matrix4f buildFrustumProjection(NIIf left, NIIf right, NIIf bottom, NIIf top,
            NIIf near, NIIf far) const;    
    protected:
        /// Warp factor adjustment
        NIIf mOptAdjustFactor;
        /// Use simple nopt derivation?
        bool mUseSimpleNOpt;
        /// Extra calculated warp factor
        mutable NIIf mOptAdjustFactorTweak;
        /// Threshold (cos angle) within which to start increasing the opt adjust as camera direction approaches light direction
        NIIf mCosCamLightDirThreshold;
    };
    
    /** 平行分割阴影描图(PSSM)阴影摄象机设置. 
    @remark
        A PSSM shadow system uses multiple shadow maps per light and maps each
        texture into a region of space, prniissing away from the camera. As such
        it is most appropriate for directional light setups. This particular version
        also uses LiSPSM projection for each split to maximise the quality. 
    @note
        Because PSSM uses multiple shadow maps per light, you will need to increase
        the number of shadow textures available (via SpaceManager) to match the 
        number of shadow maps required (default is 3 per light). 
    */
    class _EngineAPI PSSMShadowSetup : public LiSPSMShadowSetup
    {
    public:
        typedef vector<NIIf>::type SplitPointList;
        typedef vector<NIIf>::type OptimalAdjustFactorList;
    public:
        PSSMShadowSetup();
        ~PSSMShadowSetup();
        
        /// 返回一个基于PSSM分割的迭代的LiSPSM阴影摄象机
        virtual void setupShadow(Camera * dst, const SpaceManager * sm, const Camera * cam, const Light * light, NCount level) const;

        /** 计算一个新的分割方案
        @param[in] splitCount The number of splits to use
        @param[in] nearDist The near plane to use for the first split
        @param[in] farDist The far plane to use for the last split
        @param[in] lambda Factor to use to reduce the split size 
        */
        void calculateSplitPoints(NCount splitCount, NIIf nearDist, NIIf farDist, NIIf lambda = 0.95);

        /** 手到配置一个新分割方案
        @param[in] newSplitPoints A list which is splitCount + 1 entries NIIl, containing the
            split points. The first value is the near point, the last value is the
            far point, and each value in between is both a far point of the previous
            split, and a near point for the next one.
        */
        void setSplitPoints(const SplitPointList & newSplitPoints);

        /// 为给定分割设置 LiSPSM优化调整因子 (配置分割后调用).
        void setOptimalAdjustFactor(NCount splitIndex, NIIf factor);

        /// 设置填充因子,适用于 近&远距离当配对分割到另一个时候,避免`裂痕`.
        inline void setSplitPadding(NIIf pad)                       { mSplitPadding = pad; }

        /// 获取填充因子,适用于 近&远距离当配对分割到另一个时候,避免`裂痕`.
        inline NIIf getSplitPadding() const                         { return mSplitPadding; }
        
        /// 获取分割数目
        inline NCount getSplitCount() const                         { return mSplitCount; }

        /// 返回计算分割点.
        inline const SplitPointList & getSplitPoints() const        { return mSplitPoints; }
        
        /// 
        inline void setRenderIteration(NCount iteration)            { mCurrentIteration = iteration; }
        
        ///
        inline NCount getRenderIteration() const                    { return mCurrentIteration; }

        /// 为给定分割返回优化调整因子
        inline NIIf getOptimalAdjustFactor(NCount splitIndex) const { return mOptimalAdjustFactors[splitIndex]; }

        /// 重写建议仅供内部使用,因为取决于当前迭代
        inline NIIf getOptimalAdjustFactor() const                  { return mOptimalAdjustFactors[mCurrentIteration]; }
    protected:
        NCount mSplitCount;
        SplitPointList mSplitPoints;
        OptimalAdjustFactorList mOptimalAdjustFactors;
        NIIf mSplitPadding;

        mutable NCount mCurrentIteration;
    };
}
#endif