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

#ifndef _NII_ParticleCtrl_H_
#define _NII_ParticleCtrl_H_

#include "NiiPreInclude.h"
#include "NiiSpaceObj.h"
#include "NiiGeometryObj.h"

namespace NII
{
    /** 粒子控制器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ParticleCtrl : public SpaceObj, public GeometryObj
    {
    public:
        /** 旋转模式
        @version NIIEngine 3.0.0
        */
        enum RotationMode
        {
            RM_VertexPos,
            RM_TexCoord
        };

        /** 控制模式
        @version NIIEngine 3.0.0
        */
        enum CtrlMode
        {
            CM_Parallel_View,
            CM_Parallel_Ctrl,
            CM_Parallel_Self,
            CM_Vertical_Ctrl,
            CM_Vertical_Self
        };

        /** 排序模式
        @param[in]
        */
        enum SortMode
        {
            SM_DIRECTION,       ///<通过视口方向排序
            SM_DISTANCE         ///<通过视口距离排序
        };

        /** 位置中心
        @version NIIEngine 3.0.0
        */
        enum OriginType
        {
            OT_Center_Left,
            OT_Center,
            OT_Center_Right,
            OT_Top_Left,
            OT_Top_Center,
            OT_Top_Right,
            OT_Bottom_Left,
            OT_Bottom_Center,
            OT_Bottom_Right
        };

        typedef list<SyncParticle *>::type ParticleList;
    public:
        ParticleCtrl(SpaceID sid, NCount count = 16, bool linkdata = false);

        virtual ~ParticleCtrl();

        using SpaceObj::query;

        /** 设置原点类型
        @version NIIEngine 3.0.0
        */
        virtual void setOriginType(OriginType type);

        /** 获取原点类型
        @version NIIEngine 3.0.0
        */
        virtual OriginType getOriginType() const;

        /** 设置旋转模式
        @version NIIEngine 3.0.0
        */
        virtual void setRotationMode(RotationMode rm);

        /** 获取旋转模式
        @version NIIEngine 3.0.0
        */
        virtual RotationMode getRotationMode() const;

        /** 设置方向模式
        @version NIIEngine 3.0.0
        */
        virtual void setCtrlMode(CtrlMode bt);

        /** 获取方向模式
        @version NIIEngine 3.0.0
        */
        virtual CtrlMode getCtrlMode() const;

        /** 设置粒子宽
        @version NIIEngine 3.0.0
        */
        virtual void setParticleWidth(NIIf w);

        /** 获取粒子宽
        @version NIIEngine 3.0.0
        */
        virtual NIIf getParticleWidth() const;

        /** 设置粒子高
        @version NIIEngine 3.0.0
        */
        virtual void setParticleHeight(NIIf h);

        /** 获取粒子高
        @version NIIEngine 3.0.0
        */
        virtual NIIf getParticleHeight() const;

        /** 设置纹理
        @version NIIEngine 3.0.0
        */
        virtual void setMaterial(ResourceID rid, GroupID gid = GroupUnknow);

        /** 设置纹理
        @version NIIEngine 3.0.0
        */
        virtual void setMaterial(Material * material);

        /** 获取纹理资源ID
        @version NIIEngine 3.0.0
        */
        virtual ResourceID getMaterialID() const;

        /** 是否每帧都更新
        @version NIIEngine 3.0.0
        */
        void setUpdatePerFrame(bool b);

        /** 是否每帧都更新
        @version NIIEngine 3.0.0
        */
        bool isUpdatePerFrame() const;

        /** 独立拣选每个粒子
        @note 会增加计算量
        @version NIIEngnien 3.0.0
        */
        virtual void setCullItself(bool b);

        /** 独立拣选每个粒子
        @note 会增加计算量
        @version NIIEngine 3.0.0
        */
        virtual bool isCullItself() const;

        /** 设置控制器方向
        @version NIIEngine 3.0.0
        */
        void setCtrlDirection(const Vector3f & vec);

        /** 获取控制器方向
        @version NIIEngine 3.0.0
        */
        const Vector3f & getCtrlDirection() const;

        /** 设置控制器垂直方向
        @version NIIEngine 3.0.0
        */
        void setCtrlUp(const Vector3f & vec);

        /** 获取控制器垂直方向
        @version NIIEngine 3.0.0
        */
        const Vector3f & getCtrlUp() const;

        /** 设置是否平行于视口
        @version NIIEngine 3.0.0
        */
        virtual void setParallelView(bool b);

        /** 获取是否平行于视口
        @version NIIEngine 3.0.0
        */
        virtual bool isParallelView() const;

        /** 使用点渲染模式
        @note 指的是硬件像素点模式,需要硬件支持,由 ShaderCh::setPoint 控制
        @version NIIEngine 3.0.0
        */
        virtual void setPointSprites(bool b);

        /** 是否使用点渲染模式
        @note 指的是硬件像素点模式,需要硬件支持,由 ShaderCh::setPoint 控制
        @version NIIEngine 3.0.0
        */
        virtual bool isPointSprites() const;

        /** 创建同步粒子
        @version NIIEngine 3.0.0
        */
        SyncParticle * create(const Vector3f & pos, const Colour & c = Colour::White);

        /** 获取同步粒子
        @version NIIEngine 3.0.0
        */
        virtual SyncParticle * get(Nidx index) const;

        /** 移去同步粒子
        @version NIIEngine 3.0.0
        */
        virtual void remove(Nidx index);

        /** 移去同步粒子
        @version NIIEngine 3.0.0
        */
        virtual void remove(SyncParticle * obj);

        /** 获取同步粒子数量
        @version NIIEngine 3.0.0
        */
        virtual NCount getCount() const;

        /** 移去所有同步粒子
        @version NIIEngine 3.0.0
        */
        virtual void clear();

        /** 是否剔除
        @version NIIEngine 3.0.0
        */
        bool isCulling(Camera * cam, const SyncParticle & bill);

        /** 设置是否扩展粒子
        @version NIIEngine 3.0.0
        */
        void setExpandParticle(bool b);

        /** 获取是否扩展粒子
        @version NIIEngine 3.0.0
        */
        bool isExpandParticle() const;

        /** 设置粒子与摄象机是否距离排序
        @remark 如果是透明粒子是需要排序的
        @version NIIEngine 3.0.0
        */
        void setSortEnable(bool b);

        /** 获取粒子与摄象机是否距离排序
        @remark 如果是透明粒子是需要排序的
        @version NIIEngine 3.0.0
        */
        bool isSortEnable() const;

        /** 获取排序模式
        @version NIIEngine 3.0.0
        */
        virtual SortMode getSortMode() const;
        
        /** 设置容器大小
        @version NIIEngine 3.0.0
        */
        void setPoolSize(NCount size);

        /** 获取容器大小
        @version NIIEngine 3.0.0
        */
        NCount getPoolSize() const;

        /** 设置是否本地空间
        @version NIIEngine 3.0.0
        */
        void setLocalSpace(bool b);

        /** 获取是否本地空间
        @version NIIEngine 3.0.0
        */
        bool isLocalSpace() const;

        /** 定义纹理坐标
        @param[in] coords 纹理坐标数组
        @param[in] count 数量
        @version NIIEngine 3.0.0
        */
        void setTextureCoord(Rectf * coords, NCount count);

        /** 定义纹理坐标
        @param[in] idx 索引
        @param[in] coord 纹理坐标
        @version NIIEngine 3.0.0
        */
        void setTextureCoord(Nidx idx, const Rectf * coords);

        /** 设置纹理坐标网格布局
        @param[in] x 水平块数
        @param[in] y 垂直块数
        @version NIIEngine 3.0.0
        */
        void setTexCoordGrid(NCount x, NCount y);

        /** 获取纹理坐标数量
        @version NIIEngine 3.0.0
        */
        NCount getTexCoordCount();

        /** 设置空间边界(总)
        @note 一般粒子是有自己的位置边界,这个概念意义所有粒子的总空间边界
        @version NIIEngine 3.0.0 高级api
        */
        void setBounds(const AABox & box, NIIf radius);

        /** 更新空间边界(总)
        @note 一般粒子是有自己的位置边界,这个概念意义所有粒子的总空间边界
        @version NIIEngine 3.0.0 高级api
        */
        void updateBounds();

        /// @copydetails SpaceObj::_notify
        virtual void _notify(Camera * cam);

        /// @copydetails SpaceObj::getGeometry
        virtual void getGeometry(GeometryRaw & op) const;

        /// @copydetails SpaceObj::getMatrix
        virtual void getMatrix(Matrix4f * out, NCount & count) const;

        /// @copydetails SpaceObj::getCenterRange
        virtual NIIf getCenterRange() const;

        /// @copydetails SpaceObj::getAABB
        virtual const AABox & getAABB() const;

        /// @copydetails SpaceObj::queue
        virtual void queue(RenderQueue * queue);

        /// @copydetails SpaceObj::getMaterial
        virtual const Material * getMaterial() const;

        /// @copydetails SpaceObj::getTypeMark
        Nui32 getTypeMark() const;

        /// @copydetails SpaceObj::getFactoryID
        virtual FactoryID getFactoryID() const;

        /// @copydetails SpaceObj::query
        void query(GeometryQuery * query);

        /// @copydetails GeometryObj::distanceSquared
        NIIf distanceSquared(const Camera * cam) const;

        /// @copydetails GeometryObj::getLight
        void getLight(LightList & ll) const;
    public:
        /** 粒子大小改变时
        @version NIIEngine 3.0.0
        */
        virtual void onParticleSize();

        /** 粒子发生旋转时
        @version NIIEngine 3.0.0
        */
        virtual void onParticleRotate();

        /** 粒子位置改变时
        @version NIIEngine 3.0.0
        */
        virtual void onParticleChange();

        /** 渲染开始时
        @param[in] count 将要渲染多少个粒子
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onBeginRender(NCount count = 0);

        /** 渲染粒子时
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onRender(const SyncParticle & bb);

        /** 渲染结束后
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onEndRender();
    protected:
        ParticleCtrl();
    private:
        void createGeoBuffer();
        void destroyGeoBuffer();
        void addParticle(const SyncParticle & obj, const Vector3f * oft);
        void calcOffset(NIIf width, NIIf height, Vector3f * pDestVec);
    protected:
        typedef vector<SyncParticle *>::type ParticlePool;
        typedef vector<Rectf>::type TexCoordList;
    protected:
        ParticlePool mParticlePool;
        ParticleList mActiveList;
        ParticleList mFreeList;
        TexCoordList mTexCoordList;
        OriginType mOriginType;
        RotationMode mRotationType;
        CtrlMode mCtrlMode;
        ResourceID mMaterialID;
        Material * mMaterial;
        Vector3f mTempLocalX;
        Vector3f mTempLocalY;
        NIIf mOriginLeft;
        NIIf mOriginRight;
        NIIf mOriginTop;
        NIIf mOriginBottom;
        NIIf mParticleWidth;
        NIIf mParticleHeight;
        AABox mAABB;
        NIIf mBoundRange;
        NCount mPoolSize;
        NCount mVisableCount;
        Camera * mCamera;
        Vector3f mCameraLocalDir;
        Quaternion mCameraLocalOri;
        Vector3f mCameraLocalPos;
        Vector3f mCtrlDir;
        Vector3f mCtrlUp;
        Vector3f mVOffset[4];
        IndexData * mIndexData;
        VertexData * mVertexData;
        VertexBuffer * mGeoBuffer;
        NIIf * mGeoBufferPtr;
        bool mLocalSpace;
        bool mPointSprites;
        bool mCullItself;
        bool mSameSize;
        bool mAutoExtendPool;
        bool mSortEnable;
        bool mParallelView;
        bool mSameRotation;
        bool mGeoBufCreate;
        bool mExternalData;
        bool mUpdatePerFrame;
        bool mParticleChange;
    };
}
#endif