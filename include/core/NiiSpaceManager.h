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

#ifndef _NII_SCENEMANAGER_H_
#define _NII_SCENEMANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSpaceNode.h"
#include "NiiRenderSys.h"
#include "NiiCamera.h"
#include "NiiColour.h"
#include "NiiSpaceQuery.h"
#include "NiiLodListener.h"

namespace NII
{
    /** 空间几何管理
    @remark 管理空间几何之间的事物
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SpaceManager : public SceneAlloc
    {
        friend class RenderPattern;
        friend class Engine;
    public:
        /** 监听器
        @version NIIEngine 3.0.0
        */
        class _EngineAPI Listener
        {
        public:
            Listener() {}
            virtual ~Listener() {}

            /** 寻找有效灯光
            @version NIIEngine 3.0.0
            */
            virtual void onFindLight(SpaceManager *){}

            /** 寻找可见几何
            @version NIIEngine 3.0.0
            */
            virtual void onFindGeometry(SpaceManager *){}

            /** 创建时触发
            @version NIIEngine 3.0.0
            */
            virtual void onCreate(SpaceManager *){}

            /** 删除时触发
            @version NIIEngine 3.0.0
            */
            virtual void onDestroy(SpaceManager *){}
        };

        /** 渲染上下文
        @version NIIEngine 3.0.0 高级 api
        */
        struct RenderBase
        {
            Viewport * mView;
            Camera * mCamera;
            RenderQueue * mQueue;
            FusionInstance * mFusion;
            FrameObj * mFrame;
        };

        /** 裁剪类型
        @version NIIIEngine 3.0.0
        */
        enum ClipType
        {
            CT_None = 0,        ///< 没有裁剪
            CT_Some = 1,        ///< 部分裁剪
            CT_All = 2          ///< 所有裁剪
        };

        typedef map<SpaceID, SpaceObj *>::type SpaceMap;
        typedef map<FactoryID, SpaceMap>::type SpaceMapList;
    public:
        SpaceManager(const String & name, TypeID tid);
        virtual ~SpaceManager();

        /** 获取名称
        @version NIIEngine 3.0.0
        */
        inline const String & getName() const              { return mName; }

        /** 获取类型名称.
        @version NIIEngine 3.0.0
        */
        inline TypeID getTypeID() const                    { return mType; }

        /** 设置使用优化材质
        @version NIIEngine 3.0.0
        */
        inline void setOptMaterial(bool b)                 { mOptCh = b; }

        /** 是否使用优化材质
        @version NIIEngine 3.0.0
        */
        inline bool isOptMaterial() const                  { return mOptCh; }

        /** 设置是否自动可见空间
        @version NIIEngine 3.0.0
        */
        inline void setAutoVisibleSpace(bool b)            { mAutoVisibleSpace = b; }

        /** 获取是否自动可见空间
        @version NIIEngine 3.0.0
        */
        inline bool isAutoVisibleSpace() const             { return mAutoVisibleSpace; }

        /** 设置可见掩码
        @version NIIEngine 3.0.0
        */
        inline void setVisibleMask(Nmark mark)             { mVisibleMark = mark; }

        /** 获取可见掩码
        @version NIIEngine 3.0.0
        */
        inline Nmark getVisibleMask()                      { return mVisibleMark; }

        /** 获得环境可见掩码
        @version NIIEngine 3.0.0
        */
        Nmark getEnvVisibleMask() const;

        /** 设置摄象机原点模式
        @version NIIEngine 3.0.0 高级api
        */
        inline void setCameraOrigin(bool b)                { mCameraOrigin = b; }

        /** 获取摄象机原点模式
        @version NIIEngine 3.0.0 高级api
        */
        inline bool isCameraOrigin() const                 { return mCameraOrigin; }

        /** 设置自动单位化法线.
        @version NIIEngine 3.0.0
        */
        inline void setUnitNormals(bool b)                 { mUnitNormals = b; }

        /** 获取自动单位化法线.
        @version NIIEngine 3.0.0
        */
        inline bool isUnitNormals() const                  { return mUnitNormals; }

        /** 设置反缩放翻转拣选
        @version NIIEngine 3.0.0
        */
        inline void setNegScaleFlipCull(bool b)            { mNegScaleFlipCull = b; }

        /** 获得反缩放翻转拣选
        @version NIIEngine 3.0.0
        */
        inline bool isNegScaleFlipCull() const             { return mNegScaleFlipCull; }

        /** 设置锁定渲染通路
        @version NIIEngine 3.0.0 高级api
        */
        inline void setLockRenderCh(bool b)                { mLockRenderCh = b; }

        /** 是否锁定渲染通路
        @version NIIEngine 3.0.0 高级api
        */
        inline bool isLockRenderCh() const                 { return mLockRenderCh; }

        /** 设置是否显示边界
        @version NIIEngine 3.0.0
        */
        virtual void setAABBView(bool b);

        /** 获取是否显示边界
        @version NIIEngine 3.0.0
        */
        virtual bool isAABBView() const;

        /** 设置是否显示节点
        @version NIIEngine 3.0.0
        */
        virtual void setNodeView(bool b);

        /** 获取是否显示节点
        @version NIIEngine 3.0.0
        */
        virtual bool isNodeView() const;

        /** 设置环境光
        @note 是全局状态,不应叠加
        @version NIIEngine 3.0.0
        */
        inline void setAmbient(const Colour & c)            { mAmbient = c; }

        /** 获取环境光
        @note 是全局状态,不应叠加
        @version NIIEngine 3.0.0
        */
        inline const Colour & getAmbient() const            { return mAmbient; }

        /** 设置应用的雾
        @note 是全局状态,不应叠加
        @version NIIEngine 3.0.0
        */
        inline void setFog(const ShaderChFog & f)           { mFog = f; }

        /** 获取应用的雾
        @note 是全局状态,不应叠加
        @version NIIEngine 3.0.0
        */
        inline const ShaderChFog & getFog() const           { return mFog; }

        /** 更新空间体
        @version NIIEngine 3.0.0 高级api
        */
        virtual void updateSpace(Camera * cam);

        /** 找到空间体
        @version NIIEngine 3.0.0 高级api
        */
        virtual void findSpace(Camera * cam, bool onlycast);

        /** 渲染空间体
        @version NIIEngine 3.0.0 高级api
        */
        virtual void renderSpace(Camera * camera, Viewport * vp);

        /** 创建摄像机
        @version NIIEngine 3.0.0
        */
        virtual Camera * createCamera(SpaceID sid);

        /** 摄象机从场景中移去.
        @version NIIEngine 3.0.0
        */
        virtual void destroyCamera(SpaceID sid);

        /** 获取摄象机
        @version NIIEngine 3.0.0
        */
        virtual Camera * getCamera(SpaceID sid) const;

        /** 是否存在摄象机
        @version NIIEngine 3.0.0
        */
        virtual bool isCameraExist(SpaceID sid) const;

        /** 删除所有摄象机
        @version NIIEngine 3.0.0
        */
        virtual void destroyAllCamera();

        /** 创建灯光
        @version NIIEngine 3.0.0
        */
        virtual Light * createLight(SpaceID sid);

        /** 获取灯光
        @version NIIEngine 3.0.0
        */
        virtual Light * getLight(SpaceID sid) const;

        /** 是否存在灯光
        @version NIIEngine 3.0.0
        */
        virtual bool isLightExist(SpaceID sid) const;

        /** 删除灯光
        @version NIIEngine 3.0.0
        */
        virtual void destroyLight(SpaceID sid);

        /** 删除所有灯光
        @version NIIEngine 3.0.0
        */
        virtual void destroyAllLight();

        /** 通知灯光变化
        @version NIIEngine 3.0.0 高级 api
        */
        void notifyLight();

        /** 获取灯光变化掩码
        @version NIIEngine 3.0.0 api
        */
        inline Nmark getLightDirtyMark() const              { return mLightDirtyMark; }

        /** 获取当前影响渲染的灯光
        @version NIIEngine 3.0.0
        */
        inline const LightList & getLightList() const       { return mValidLights; }

        /** 获取灯光的容积区域
        @version NIIEngine 3.0.0
        */
        virtual const PlaneList & getClipPlane(Light * l);

        /** 获取灯光的平面区域
        @param[in] proj 投影视区
        @version NIIEngine 3.0.0
        */
        virtual const Rectf & getClipRect(Light * l, const Camera * proj);

        /** 从当前有效的灯光中寻找指定的
        @param[in] out 寻找出来的
        @param[in] pos 位置
        @param[in] radius 辐射范围
        @param[in] mark 掩码
        @version NIIEngine 3.0.0 高级api
        */
        virtual void findLight(LightList & out, const Vector3f & pos, NIIf radius, Nmark mark);

        /** 创建几何空间
        @param[in] rid 网格资源
        @version NIIEngine 3.0.0
        */
        GeometrySpace * createGeo(SpaceID sid, ResourceID rid, GroupID gid = GroupUnknow);

        /** 创建几何空间
        @param[in] gsp 预制网格
        @version NIIEngine 3.0.0
        */
        GeometrySpace * createGeo(SpaceID sid, GeometryPrefab gsp, GroupID gid = GroupUnknow);

        /** 获取几何空间
        @version NIIEngine 3.0.0
        */
        GeometrySpace * getGeo(SpaceID sid) const;

        /** 是否存在几何空间
        @version NIIEngine 3.0.0
        */
        inline bool isGeoExist(SpaceID sid) const       { return isSpaceExist(sid, N_FACTORY_GeoSpace);}

        /** 删除几何空间
        @version NIIEngine 3.0.0
        */
        inline void destroyGeo(SpaceID sid)             { destroySpace(sid, N_FACTORY_GeoSpace);}

        /** 删除所有几何空间
        @version NIIEngine 3.0.0
        */
        inline void destroyAllGeo()                     { destroyAllSpace(N_FACTORY_GeoSpace); }

        /** 自绘制几何
        @param[in] sid
        @version NIIEngine 3.0.0
        */
        CustomSpaceObj * createCustomGeo(SpaceID sid);

        /** 获取自绘制几何.
        @version NIIEngine 3.0.0
        */
        CustomSpaceObj * getCustomGeo(SpaceID sid) const;

        /** 删除绘制几何
        @version NIIEngine 3.0.0
        */
        inline void destroyCustomGeo(SpaceID sid)       { destroySpace(sid, N_FACTORY_CustomSpace); }

        /** 自绘制几何是否存在
        @version NIIEngine 3.0.0
        */
        inline bool isCustomGeoExist(SpaceID sid) const { return isSpaceExist(sid, N_FACTORY_CustomSpace); }

        /** 删除所有绘制几何
        @version NIIEngine 3.0.0
        */
        inline void destroyAllCustomGeo()               { destroyAllSpace(N_FACTORY_CustomSpace); }

        /** 创建空间节点
        @version NIIEngine 3.0.0
        */
        SpaceNode * createSpaceNode(SpaceID id);

        /** 删除空间节点
        @version NIIEngine 3.0.0
        */
        void destroySpaceNode(SpaceID id);

        /** 获取空间节点
        @version NIIEngine 3.0.0
        */
        SpaceNode * getSpaceNode(SpaceID id) const;

        /** 空间节点是否存在
        @version NIIEngine 3.0.0
        */
        bool isSpaceNodeExist(SpaceID id) const;

        /** 设置自动对焦
        @version NIIEngine 3.0.0
        */
        void setAutoFocus(SpaceNode * node, bool B);

        /** 添加天空
        @note 不负责内存释放工作
        @version NIIEngine 3.0.0
        */
        void addSky(Sky * obj);
        
        /** 移去天空
        @note 不负责内存释放工作
        @version NIIEngine 3.0.0
        */
        void removeSky(Sky * obj);
        
        /** 创建像素轨迹
        @version NIIEngine 3.0.0
        */
        PixelOrbit * createOrbit(SpaceID sid);

        /** 获取像素轨迹
        @version NIIEngine 3.0.0
        */
        PixelOrbit * getOrbit(SpaceID sid) const;

        /** 是否存在像素轨迹
        @version NIIEngine 3.0.0
        */
        inline bool isOrbitExist(SpaceID sid) const     { return isSpaceExist(sid, N_FACTORY_PixelOrbit); }

        /** 删除像素轨迹
        @version NIIEngine 3.0.0
        */
        inline void destroyOrbit(SpaceID sid)           { destroySpace(sid, N_FACTORY_PixelOrbit); }

        /** 删除所有轨迹
        @version NIIEngine 3.0.0
        */
        inline void destroyAllOrbit()                   { destroyAllSpace(N_FACTORY_PixelOrbit); }

        /** 创建同态粒子
        @version NIIEngine 3.0.0
        */
        ParticleCtrl * createParticleCtrl(SpaceID sid, NCount count);

        /** 获取同态粒子
        @version NIIEngine 3.0.0
        */
        ParticleCtrl * getParticleCtrl(SpaceID sid) const;

        /** 同态粒子是否存在
        @version NIIEngine 3.0.0
        */
        inline bool isParticleCtrlExist(SpaceID sid) const{ return isSpaceExist(sid, N_FACTORY_ParticleCtrl); }

        /** 删除同态粒子
        @version NIIEngine 3.0.0
        */
        inline void destroyParticleCtrl(SpaceID sid)    { destroySpace(sid, N_FACTORY_ParticleCtrl); }

        /** 删除所有同态粒子
        @version NIIEngine 3.0.0
        */
        inline void destroyAllParticleCtrl()            { destroyAllSpace(N_FACTORY_ParticleCtrl); }

        /** 创建粒子空间
        @version NIIEngine 3.0.0
        */
        ParticleSpace * createParticleSpace(SpaceID sid, SpaceID predefine);

        /** 创建创建
        @version NIIEngine 3.0.0
        */
        ParticleSpace * createParticleSpace(SpaceID sid, GroupID gid = GroupDefault, NCount max = 300);

        /** 获取粒子空间
        @version NIIEngine 3.0.0
        */
        ParticleSpace * getParticleSpace(SpaceID sid) const;

        /** 是否存在粒子空间
        @version NIIEngine 3.0.0
        */
        inline bool isParticleSpaceExist(SpaceID sid) const { return isSpaceExist(sid, N_FACTORY_ParticleSpace); }

        /** 删除粒子空间
        @version NIIEngine 3.0.0
        */
        inline void destroyParticleSpace(SpaceID sid)       { destroySpace(sid, N_FACTORY_ParticleSpace); }

        /** 删除所有粒子空间
        @version NIIEngine 3.0.0
        */
        inline void destroyAllParticleSpace()               { destroyAllSpace(N_FACTORY_ParticleSpace); }

        /** 创建静态空间体
        @version NIIEngine 3.0.0
        */
        StaticSpace * createStaticSpace(SpaceID sid);

        /** 获取静态空间体
        @version NIIEngine 3.0.0
        */
        StaticSpace * getStaticSpace(SpaceID sid) const;

        /** 静态空间体是否存在
        @version NIIEngine 3.0.0
        */
        bool isStaticSpaceExist(SpaceID sid) const;

        /** 删除静态空间体
        @version NIIEngine 3.0.0
        */
        void destroyStaticSpace(SpaceID sid);

        /** 删除所有静态空间体
        @version NIIEngine 3.0.0
        */
        void destroyAllStaticSpace();

        /** 创建复用空间体
        @version NIIEngine 3.0.0
        */
        InstanceSpace * createInstanceSpace(SpaceID sid);

        /** 获取复用空间体
        @version NIIEngine 3.0.0
        */
        InstanceSpace * getInstanceSpace(SpaceID sid) const;

        /** 删除复用空间体
        @version NIIEngine 3.0.0
        */
        void destroyInstanceSpace(SpaceID sid);

        /** 删除所有复用空间体
        @version NIIEngine 3.0.0
        */
        void destroyAllInstanceSpace();

        /** 创建空间对象
        @version NIIEngine 3.0.0
        */
        virtual SpaceObj * createSpace(SpaceID sid, FactoryID fid, const PropertyData * params = 0);

        /** 添加空间对象
        @version NIIEngine 3.0.0
        */
        virtual void addSpace(SpaceObj * m, FactoryID fid = 0);

        /** 移去空间对象
        @version NIIEngine 3.0.0
        */
        virtual void removeSpace(SpaceID sid, FactoryID fid);

        /** 移去指定类型空间对象
        @version NIIEngine 3.0.0
        */
        virtual void removeSpace(FactoryID fid);

        /** 获取空间对象
        @version NIIEngine 3.0.0
        */
        virtual SpaceObj * getSpace(SpaceID sid, FactoryID fid) const;

        /** 获取指定类型空间对象
        @version NIIEngine 3.0.0 高级api
        */
        virtual SpaceMap & getSpace(FactoryID fid);

        /** 获取指定类型空间对象
        @version NIIEngine 3.0.0
        */
        virtual const SpaceMap & getSpace(FactoryID fid) const;

        /** 空间对象是否存在
        @version NIIEngine 3.0.0
        */
        virtual bool isSpaceExist(SpaceID sid, FactoryID fid) const;

        /** 删除空间对象
        @version NIIEngine 3.0.0
        */
        virtual void destroySpace(SpaceID sid, FactoryID fid);

        /** 删除指定类型空间对象
        @version NIIEngine 3.0.0
        */
        virtual void destroyAllSpace(FactoryID fid);

        /** 删除所有空间对象
        @version NIIEngine 3.0.0
        */
        virtual void destroyAllSpace();

        /** 删除所有创建的对象
        @version 3.0.0
        */
        virtual void destroyAllCreateObj();

        /** 设置场景几何
        @version NIIEngine 3.0.0
        */
        virtual void setSceneGeo(const String & filename);

        /** 设置场景几何
        @version NIIEngine 3.0.0
        */
        virtual void setSceneGeo(DataStream * stream, const String & type);

        /** 设置场景几何队列
        @version NIIEngine 3.0.0
        */
        virtual void setSceneGeoRenderGroup(GroupID qid);

        /** 或设置场景几何队列
        @version NIIEngine 3.0.0
        */
        virtual GroupID getSceneGeoRenderGroup();

        /** 设置专有属性值
        @version NIIEngine 3.0.0
        */
        virtual bool setProperty(PropertyID pid, const void * value);

        /** 获取专有属性值
        @version NIIEngine 3.0.0
        */
        virtual bool getProperty(PropertyID pid, void * value) const;

        /** 是否存在专有属性
        @version NIIEngine 3.0.0
        */
        virtual bool isPropertyExist(PropertyID pid) const;

        /** 获取专有属性列表
        @version NIIEngine 3.0.0
        */
        virtual bool getPropertyList(PropertyArray * pids) const;

        /** 设置渲染处理器
        @version NIIEngine 3.0.0 高级api
        */
        void setRenderPattern(RenderPattern * obj);

        /** 获取渲染处理器
        @version NIIEngine 3.0.0 高级api
        */
        inline RenderPattern * getRenderPattern() const     { return mRenderPattern; }

        /** 设置渲染系统
        @version NIIEngine 3.0.0 高级api
        */
        void setGPUSys(RenderSys * sys);

        /** 获取渲染系统
        @version NIIEngine 3.0.0 高级api
        */
        inline RenderSys * getGPUSys() const                { return mGPUSys; }

        /** 设置帧合成
        @version NIIEngine 3.0.0 高级api
        */
        inline void setFrameResult(FusionInstance * chain)  { mFrameResult = chain; }

        /** 获取帧合成
        @version NIIEngine 3.0.0 高级api
        */
        inline FusionInstance * getFrameResult() const      { return mFrameResult; }

        /** 获取原点节点
        @version NIIEngine 3.0.0 高级api
        */
        virtual SpaceNode * getOriginNode() const;
        
        /** 设置队列是否使用阴影体积
        @version NIIEngine 3.0.0 高级api
        */
        void setShadowVolume(RenderQueue * rq, bool b);
        
        /** 获取队列是否使用阴影体积
        @version NIIEngine 3.0.0
        */
        bool isShadowVolume(RenderQueue * rq) const;
        
        /** 获取主渲染队列
        @version NIIEngine 3.0.0 高级api
        */
        RenderQueue * getRenderQueue() const            { return mMainQueue; }

        /** 获取渲染视口
        @version NIIEngine 3.0.0 高级api
        */
        Viewport * getViewport() const                  { return mView; }

        /** 应用渲染通路
        @param[in] ch 渲染通路
        @param[in] force 强制应用
        @param[in] shadow 用于阴影
        @return 实际应用的渲染通路
        @version NIIEngine 3.0.0 高级api
        */
        const ShaderCh * applyCh(const ShaderCh * ch, bool force, bool shadow);

        /** 应用GPU程序
        @version NIIEngine 3.0.0 高级api
        */
        void applyGpuProgram(GpuProgram * obj);

        /** 应用GPU参数
        @version NIIEngine 3.0.0 高级api
        */
        void applyGpuParam(const ShaderChProgram * pg);

        /** 在灯光群中建立矩形裁剪
        @version NIIEngine 3.0.0 高级api
        */
        ClipType applyClip(const LightList & ll, const Camera * cam);

        /** 在灯光群中建立区域裁剪
        @version NIIEngine 3.0.0
        */
        ClipType applyPlaneClip(const LightList & ll);

        /** 重置矩形裁剪
        @version NIIEngine 3.0.0
        */
        void resetClip();

        /** 重置区域裁剪
        @version NIIEngine 3.0.0
        */
        void resetPlaneClip();

        /** 渲染对象
        @param[in] obj 渲染对象
        @param[in] ch 通路
        @param[in] lightClip 使用设置中的灯光,是否灯光裁剪
        @version NIIEngine 3.0.0 高级api
        */
        virtual void render(GeometryObj * geo, const ShaderCh * ch, bool lightClip);

        /** 指定灯光去渲染对象
        @param[in] obj 渲染对象
        @param[in] ch 通路
        @param[in] lights 指定灯光列表
        @param[in] lightClip 灯光裁剪
        @version NIIEngine 3.0.0 高级api
        */
        virtual void render(GeometryObj * geo, const ShaderCh * ch, const LightList & lights, bool lightClip);

        /** 渲染单个对象
        @param[in] aloneframe 单独渲染一帧
        @version NIIEngine 3.0.0 高级api
        */
        virtual void render(Viewport * vp, const Matrix4f & proj, const Matrix4f & view,
            const Matrix4f & model, GeometryRaw * geo, ShaderCh * ch, bool aloneframe) ;

        /** 渲染单个对象
        @param[in] aloneframe 单独渲染一帧
        @version NIIEngine 3.0.0 高级api
        */
        virtual void render(Viewport * vp, const Matrix4f & proj,  const Matrix4f & view,
            GeometryObj * geo, const ShaderCh * ch, bool lightClip, bool aloneframe);

        /** 渲染单个对象
        @version NIIEngine 3.0.0 高级api
        */
        virtual void render(Viewport * vp, const Matrix4f & proj,  const Matrix4f & view,
            GeometryObj * geo, const ShaderCh * ch,  const LightList & lights, bool lightClip,
                bool aloneframe);

        /** 暂停渲染帧
        @version NIIEngine 3.0.0
        */
        virtual void pauseRender(RenderBase * out);

        /** 恢复帧渲染
        @version NIIEngine 3.0.0
        */
        virtual void resumeRender(RenderBase * in);

        /** 添加监听
        @version NIIEngine 3.0.0
        */
        virtual void add(RenderListener * obj);

        /** 移除监听
        @version NIIEngine 3.0.0
        */
        virtual void remove(RenderListener * obj);

        /** 添加监听
        @version NIIEngine 3.0.0
        */
        virtual void add(LodListener * obj);

        /** 移除监听
        @version NIIEngine 3.0.0
        */
        virtual void remove(LodListener * obj);

        /** 添加监听
        @version NIIEngine 3.0.0
        */
        virtual void add(GeometryObjListener * obj);

        /** 移除监听
        @version NIIEngine 3.0.0
        */
        virtual void remove(GeometryObjListener * obj);

        /** 添加排除渲染组
        @version NIIEngine 3.0.0
        */
        virtual void addExcludeRender(GroupID qid);

        /** 移去排除渲染组
        @version NIIEngine 3.0.0
        */
        virtual void removeExcludeRender(GroupID qid);

        /** 清除所有排除渲染组
        @version NIIEngine 3.0.0
        */
        virtual void removeAllExcludeRender();

        /** 是否排除除渲染
        @version NIIEngine 3.0.0
        */
        virtual bool isExcludeRenderExist(GroupID qid);

        /** 创建射线查询
        @version NIIEngine 3.0.0
        */
        virtual RaySpaceQuery * createRayQuery(const Ray & ray, Nmark query = 0xFFFFFFFF);

        /** 创建射线查询
        @version NIIEngine 3.0.0
        */
        virtual RaySpaceQuery * createRayQuery(Nmark query = 0xFFFFFFFF);

        /** 创建盒查询
        @version NIIEngine 3.0.0
        */
        virtual AABSpaceQuery * createAABBQuery(const AABox & box, Nmark query = 0xFFFFFFFF);

        /** 创建盒查询
        @version NIIEngine 3.0.0
        */
        virtual AABSpaceQuery * createAABBQuery(Nmark query = 0xFFFFFFFF);

        /** 创建球查询
        @version NIIEngine 3.0.0
        */
        virtual SphereSpaceQuery * createSphereQuery(const Sphere & sphere, Nmark query = 0xFFFFFFFF);

        /** 创建球查询
        @version NIIEngine 3.0.0
        */
        virtual SphereSpaceQuery * createSphereQuery(Nmark query = 0xFFFFFFFF);

        /** 创建容积查询
        @version NIIEngine 3.0.0
        */
        virtual PlaneSpaceQuery * createPlaneSpaceQuery(const PlaneSpaceList & volumes, Nmark query = 0xFFFFFFFF);

        /** 创建容积查询
        @version NIIEngine 3.0.0
        */
        virtual PlaneSpaceQuery * createPlaneSpaceQuery(Nmark query = 0xFFFFFFFF);

        /** 创建香交查询
        @version NIIEngine 3.0.0
        */
        virtual AreaSpaceQuery * createIntersectQuery(Nmark query = 0xFFFFFFFF);

        /** 删除查询
        @version NIIEngine 3.0.0
        */
        virtual void destroyQuery(SpaceQuery * query);

        /** 设置自定义渲染队列
        @version NIIEngine 3.0.0
        */
        static RenderQueueFusion * createQueueFusion(Nid id);

        /** 自定义渲染队列
        @version NIIEngine 3.0.0
        */
        static RenderQueueFusion * getQueueFusion(Nid id);

        /** 删除自定义渲染队列
        @version NIIEngine 3.0.0
        */
        static void destroyQueueFusion(Nid id);

        /** 删除所有自定义渲染队列
        @version NIIEngine 3.0.0
        */
        static void destroyAllQueueFusion();
    public:
        /** 资源事件
        @version NIIEngine 3.0.0
        */
        void onResEvent(const ResResultEvent & arg);

        /** 资源事件
        @version NIIEngine
        */
        void onResEvent(const ResLoadEvent & arg);

        /** lod事件
        @version NIIEngine 3.0.0
        */
        void onLodEvent(const SpaceLodEvent & arg);

        /** lod事件
        @version NIIEngine 3.0.0
        */
        void onLodEvent(const MeshLodEvent & arg);

        /** lod事件
        @version NIIEngine 3.0.0
        */
        void onLodEvent(const MaterialLodEvent & arg);
    protected:
        /** 创建节点
        @version NIIEngine 3.0.0 高级api
        */
        virtual SpaceNode * createNodeImpl(SpaceID id);

        /** 准备队列
        @version NIIEngine 3.0.0
        */
        virtual void prepareQueue();

        /** 渲染队列
        @version NIIEngine 3.0.0
        */
        virtual void renderQueue();

        /** 渲染队列
        @version NIIEngine 3.0.0
        */
        virtual void renderQueue(RenderQueueFusion * s);

        /** 渲染队列前
        @version NIIEngine 3.0.0
        */
        virtual bool onPreRenderQueue(GroupID qid);

        /** 渲染对象时
        @version NIIEngine 3.0.0
        */
        virtual void onRender(GeometryObj * geo, const ShaderCh * ch, const LightList & lights);

        /** 渲染队列后
        @version NIIEngine 3.0.0
        */
        virtual bool onEndRenderQueue(GroupID qid);

        /** 处理资源事件
        @version NIIEngine 3.0.0
        */
        void prcResEvent();

        /** 处理细节层次事件
        @version NIIEngine 3.0.0
        */
        void prcLodEvent();
    protected:
        void onDestroy();
    public:
        static Nmark ViewSpaceMark;
        static Nmark CameraSpaceMark;
        static Nmark SelfGeoMark;
        static Nmark StaticGeoMark;
        static Nmark InstanceGeoMark;
        static Nmark ParticleSpaceMark;
        static Nmark LightSpaceMark;
        static Nmark CustomSpaceMark;
        static Nmark SpaceGeoMark;
    protected:
        typedef map<SpaceID, Camera *>::type CameraMap;
        typedef map<SpaceID, SpaceNode *>::type SceneNodeList;
        typedef map<SpaceID, StaticSpace *>::type StaticSpaceList;
        typedef map<SpaceID, InstanceSpace *>::type InstanceSpaceList;
        typedef map<Nid, RenderQueueFusion *>::type QueueFusionList;
        typedef map<Light *, Rectf>::type LightClipList;
        typedef map<Light *, PlaneList>::type LightPlaneClipList;
        typedef set<SpaceNode *>::type AutoFocusList;
        typedef set<LodListener *>::type LodListenerSet;
        typedef set<GroupID>::type ExcludeQueueList;
        typedef vector<Sky *>::type SkyList; 
        typedef vector<SpaceLodEvent>::type SpaceLodEventList;
        typedef vector<MeshLodEvent>::type MeshLodEventList;
        typedef vector<MaterialLodEvent>::type MaterialLodEventList;
        typedef vector<RenderListener *>::type RenderListenerList;
        typedef vector<GeometryObjListener *>::type GeoRenderListenerList;
        static RenderPattern * mNoShadowPattern;
        static QueueFusionList mQueueFusionList;
    protected:
        String mName;
        TypeID mType;
        SkyList mSkyList;
        CameraMap mCameraList;
        SceneNodeList mSceneNodes;
        LightList mValidLights;
        LightClipList mLightClipList;
        LightPlaneClipList mLightPlaneClipList;
        RenderPattern * mRenderPattern;
        RenderQueue * mMainQueue;
        FusionInstance * mFrameResult;
        SpaceNode * mOriginNode;
        RenderSys * mGPUSys; //
        Camera * mCamera;
        Viewport * mView;
        Colour mAmbient;
        ShaderChFog mFog;
        CullingMode mCullingMode;
        AutoFocusList mAutoFocusList;
        ExcludeQueueList mExcludeList;
        GroupID mSceneGeoRenderGroup;
        Matrix4f mTempSpace[256];
        Matrix4f mTempViewMatrix;
        Vector3f mCameraRelPos;
        SpaceMapList mSpaceMapList;
        StaticSpaceList mStatics;
        InstanceSpaceList mInstances;
        NCount mLastLightLimit;
        Nmark mLightDirtyMark;
        Nmark mLastFrame;

        RenderListenerList mRenderListenerList;
        GeoRenderListenerList mGeoRenderLisList;
        SysSyncParam * mSyncParams;

        Nmark mVisibleMark;
        Nmark mLightClipFrame;
        Nui32 mLastLightHash;

        Nmark mGpuParamMark;
        Nmark mLastProgramLightMark;
        LodListenerSet mLodListeners;
        SpaceLodEventList mSpaceLodEventList;
        MeshLodEventList mMeshLodEventList;
        MaterialLodEventList mMatLodEventList;
        N_mutable_mutex(mSpaceMutex)
        bool mShadowVolume;
        bool mOptCh;
        bool mLockRenderCh;
        bool mUnitNormals;
        bool mAutoVisibleSpace;
        bool mNegScaleFlipCull;
        bool mLastCustomRender;
        bool mCameraOrigin;
        bool mIdentityViewMode;
        bool mIdentityProjMode;
        bool mAABBView;
        bool mNodeView;
    };
}
#endif