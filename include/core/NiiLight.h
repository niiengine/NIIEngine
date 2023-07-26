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

#ifndef _NII_LIGHT_H_
#define _NII_LIGHT_H_

#include "NiiPreInclude.h"
#include "NiiSpaceObj.h"
#include "NiiColour.h"
#include "NiiPlane.h"
#include "NiiShaderCh.h"
#include "NiiCamera.h"
#include "NiiGpuProgramParam.h"
#include "NiiFusionDataCollector.h"

namespace NII
{
    /** 用于创建灯光的工厂类对象
    @version NIIEngine 3.0.0
    */
    class _EngineAPI LightFactory : public SpaceObjFactory
    {
    public:
        LightFactory();
        ~LightFactory();

        /// @copydetails SpaceObjFactory::getID
        FactoryID getID() const;

        /// @copydetails SpaceObjFactory::create
        SpaceObj * create(SpaceID sid, SpaceManager * mag, const PropertyData * params = 0);

        /// @copydetails SpaceObjFactory::destroy
        void destroy(void * obj);
    };

    /** 灯光
    @remark 有很多操作被渲染系统局限
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Light : public SpaceObj, public CustomGpuParam, public FusionDataCollector
    {
    public:
        Light();
        Light(SpaceID sid);

        ~Light();

        /** 设置灯光类型
        @param[in] type 灯光类型
        @version NIIEngine 3.0.0
        */
        inline void setType(LightType type)                 { mLightType = type; }

        /** 返回灯光类型
        @version NIIEngine 3.0.0
        */
        inline LightType getType() const                    { return mLightType; }

        /** 设置灯光的位置.
        @remark 点光源专有概念
        @version NIIEngine 3.0.0
        */
        inline void setPos(const Vector3f & o)              { mPosition = o; mMove = true; }

        /** 返回灯光的位置.
        @note 点光源专有概念
        */
        inline const Vector3f & getPos() const              { return mPosition; }

        /** 设置灯光点的方向.
        @remark 点光源和方向光源.
        @version NIIEngine 3.0.0
        */
        inline void setDirection(const Vector3f & vec)      { mDirection = vec; mMove = true; }

        /** 获取灯光点的方向.
        @remark 点光源和方向光源.
        @version NIIEngine 3.0.0
        */
        inline const Vector3f & getDirection() const        { return mDirection; }

        /** 获取4元空间
        @remark 用于着色程序/渲染系统,着色程序/渲染系统可能需要四元值
        @version NIIEngine 3.0.0
        */
        Vector4f getSpace4() const;

        /** 获取4元空间
        @remark 用于着色程序/渲染系统,着色程序/渲染系统可能需要四元值
        @version NIIEngine 3.0.0
        */
        Vector4f getRelSpace4() const;

        /** 设置反射(漫)颜色.
        @version NIIEngine 3.0.0
        */
        inline void setDiffuse(NIIf r, NIIf g, NIIf b)      { mDiffuse.r = r; mDiffuse.b = g; mDiffuse.g = b; }

        /** 设置反射(漫)颜色.
        @version NIIEngine 3.0.0
        */
        inline void setDiffuse(const Colour & c)            { mDiffuse = c; }

        /** 获取反射(漫)颜色
        @version NIIEngine 3.0.0
        */
        inline const Colour & getDiffuse() const            { return mDiffuse; }

        /** 设置反射(镜面)颜色.
        @remark 三色元
        @version NIIEngine 3.0.0
        */
        inline void setSpecular(NIIf r, NIIf g, NIIf b)     { mSpecular.r = r; mSpecular.b = g; mSpecular.g = b; }

        /** 设置反射(镜面)颜色.
        @remark 三色元
        @version NIIEngine 3.0.0
        */
        inline void setSpecular(const Colour & c)           { mSpecular = c; }

        /** 返回反射(镜面)颜色.
        @version NIIEngine 3.0.0
        */
        inline const Colour & getSpecular() const           { return mSpecular; }

        /** 设置光亮因子
        @version NIIEngine 3.0.0
        */
        inline void setBrightFactor(NIIf f)                 { mBrightFactor = f; }

        /** 获取光亮因子
        @version NIIEngine 3.0.0
        */
        inline NIIf getBrightFactor() const                 { return mBrightFactor; }

        /** 设置阴影范围
        @version NIIEngine 3.0.0
        */
        inline void setShadowExtent(NIIf f)                 { mShadowExtent = f; }

        /** 获取阴影范围
        @version NIIEngine 3.0.0
        */
        NIIf getShadowExtent() const;

        /** 设置阴影近剪切距离
        @remark 使用摆位摄想机, 一般情况下近剪切距离是由摄想机设置的
        @version NIIEngine 3.0.0
        */
        inline void setShadowNearClip(NIIf extent)          { mShadowNearClip = extent; }

        /** 获取阴影近剪切距离
        @remark 使用摆位摄想机, 一般情况下近剪切距离是由摄想机设置的
        @param[in] ext 扩展
        @version NIIEngine 3.0.0
        */
        inline NIIf getShadowNearClip(NIIf ext) const       { return mShadowNearClip > 0 ? mShadowNearClip : ext; }

        /** 设置阴影近剪切距离
        @remark 使用摆位摄想机, 一般情况下近远切距离是由摄想机设置的
        @version NIIEngine 3.0.0
        */
        inline void setShadowFarClip(NIIf farClip)          { mShadowFarClip = farClip; }

        /** 获取阴影近剪切距离
        @remark 使用摆位摄想机, 一般情况下近远切距离是由摄想机设置的
        @param[in] ext 扩展
        @version NIIEngine 3.0.0
        */
        NIIf getShadowFarClip(NIIf ext) const;

        /** 设置生成阴影时使用的摄像机
        @version NIIEngine 3.0.0
        */
        void setShadowCamera(ShadowSetup * obj);

        /** 设置生成阴影时使用的摄像机
        @version NIIEngine 3.0.0
        */
        inline const ShadowSetup * getShadowCamera() const { return mShadowCamera; }

        /** 设置灯光排序相对位置
        @version NIIEngine 3.0.0
        */
        void setSortBase(const Vector3f & o);

        /** 获取灯光排序比对值
        @version NIIEngine 3.0.0
        */
        inline NIIf getSortValue() const                    { return mSortValue; }

        /** 设置索引
        @remark 一般是指应用到渲染器中的索引
        @version NIIEngine 3.0.0
        */
        inline void setIndex(Nidx i)                        { mIndexInFrame = i; }

        /** 获取索引
        @remark 一般是指应用到渲染器中的索引
        @version NIIEngine 3.0.0
        */
        inline Nidx getIndex() const                        { return mIndexInFrame; }

        /** 设置光照开始
        @remark 对方向光无效果
        @remark fn(y) = quadratic(y - (constant + x * linear)) and x < range x > start
        @version NIIEngine 3.0.0
        */
        inline void setStart(NIIf f)                        { mStart = f; }

        /** 获取光照开始
        @remark 对方向光无效果
        @remark fn(y) = quadratic(y - (constant + x * linear)) and x < range x > start
        @version NIIEngine 3.0.0
        */
        inline NIIf getStart() const                        { return mStart; }

        /** 设置光照范围.
        @remark 对方向光无效果
        @note fn(y) = quadratic(y - (constant + x * linear)) and x < range x > start
        @version NIIEngine 3.0.0
        */
        inline void setRange(NIIf f)                        { mEnd = f; }

        /** 获取光照范围.
        @remark 对方向光无效果
        @note fn(y) = quadratic(y - (constant + x * linear)) and x < range x > start
        @version NIIEngine 3.0.0
        */
        inline NIIf getRange() const                        { return mEnd; }

        /** 设置聚光灯内角
        @note 聚光灯专用
        @version NIIEngine 3.0.0
        */
        inline void setSpotlightInner(const Radian & val)   { mSpotInner = val; }

        /** 获取聚光灯内角
        @note 聚光灯专用
        @version NIIEngine 3.0.0
        */
        inline const Radian & getSpotlightInner() const     { return mSpotInner; }

        /** 设置聚光灯外角
        @note 聚光灯专用
        @version NIIEngine 3.0.0
        */
        inline void setSpotlightOuter(const Radian & val)   { mSpotOuter = val; }

        /** 获取聚光灯外角
        @note 聚光灯专用
        @version NIIEngine 3.0.0
        */
        inline const Radian & getSpotlightOuter() const     { return mSpotOuter; }

        /** 设置内外角衰减率.
        @note 聚光灯专用
        @version NIIEngine 3.0.0
        */
        inline void setSpotlightFalloff(NIIf val)           { mSpotFalloff = val; }

        /** 获取内外角衰减率.
        @note 聚光灯专用
        @version NIIEngine 3.0.0
        */
        inline NIIf getSpotlightFalloff() const             { return mSpotFalloff; }

        /** 设置衰减常数因子.
        @remark 对方向光无效果
        @note fn(y) = quadratic(y - (constant + x * linear)) and x < range x > start
        @version NIIEngine 3.0.0
        */
        inline void setAttenuationConstant(NIIf f)          { mAttenuationConst = f; }

        /** 获取衰减常数因子.
        @remark 对方向光无效果
        @note fn(y) = quadratic(y - (constant + x * linear)) and x < range x > start
        @version NIIEngine 3.0.0
        */
        inline NIIf getAttenuationConstant() const          { return mAttenuationConst; }

        /** 设置衰减线性因子.
        @remark 对方向光无效果
        @note fn(y) = quadratic(y - (constant + x * linear)) and x < range x > start
        @version NIIEngine 3.0.0
        */
        inline void setAttenuationLinear(NIIf f)            { mAttenuationLinear = f; }

        /** 获取衰减线性因子.
        @remark 对方向光无效果
        @note fn(y) = quadratic(y - (constant + x * linear)) and x < range x > start
        @version NIIEngine 3.0.0
        */
        inline NIIf getAttenuationLinear() const            { return mAttenuationLinear; }

        /** 设置衰减二次参子.
        @remark 对方向光无效果
        @note fn(y) = quadratic(y - (constant + x * linear)) and x < range x > start
        @version NIIEngine 3.0.0
        */
        inline void setAttenuationQuadric(NIIf f)           { mAttenuationQuad = f; }

        /** 获取衰减二次参子.
        @remark 对方向光无效果
        @note fn(y) = quadratic(y - (constant + x * linear)) and x < range x > start
        @version NIIEngine 3.0.0
        */
        inline NIIf getAttenuationQuadric() const           { return mAttenuationQuad; }

        /** 获取有效空间域
        @version NIIEngine 3.0.0
        */
        void getClip(PlaneList & planes) const;

        /// @copydetails FusionDataCollector::getFusionCollectorType
        TypeID getFusionCollectorType() const;
        
        /// @copydetails FusionDataCollector::createFusionData
        FusionData * createFusionData(Nid id);

        /// @copydetails SpaceObj::_notify
        void _notify();

        /// @copydetails SpaceObj::query
        void query(GeometryQuery * query);

        /// @copydetails SpaceObj::queue
        void queue(RenderQueue * queue);

        /// @copydetails SpaceObj::attach
        void attach(PosNode * node, bool tag = false);

        /// @copydetails SpaceObj::getCenterRange
        NIIf getCenterRange() const;

        /// @copydetails SpaceObj::getAABB
        const AABox & getAABB() const;

        /// @copydetails SpaceObj::getSpacePos
        virtual const Vector3f & getSpacePos() const;

        /// @copydetails SpaceObj::getRelSpacePos
        virtual const Vector3f & getRelSpacePos() const;

        /// @copydetails SpaceObj::getSpaceDirection
        virtual const Vector3f & getSpaceDirection() const;

        /// @copydetails SpaceObj::getTypeMark
        Nui32 getTypeMark() const;

        /// @copydetails SpaceObj::getFactoryID
        FactoryID getFactoryID() const;
    protected:
        virtual void update();

        /// @copydetails FusionDataCollector::initFusionCollector
        void initFusionCollector(IDList * ids) const;
    public:
        /** 灯光到摄像机的距离排序
        @version NIIEngine 3.0.0
        */
        struct LightDistanceLess
        {
            _EngineAPI bool operator()(const Light * a, const Light * b) const;
        };
        /** 灯光到摄像机的距离排序
        @version NIIEngine 3.0.0
        */
        struct ValueDistanceLess
        {
            _EngineAPI bool operator()(const Light * a, NIIf b) const;
        };
    protected:
        Nidx mIndexInFrame;
        LightType mLightType;
        NIIf mSortValue;
        NIIf mStart;
        NIIf mEnd;
        Vector3f mPosition;
        Vector3f mDirection;
        Vector3f mSpacePos;
        Vector3f mRelSpacePos;
        Vector3f mSpaceDirection;
        NIIf mBrightFactor;
        NIIf mAttenuationConst;
        NIIf mAttenuationLinear;
        NIIf mAttenuationQuad;
        Colour mDiffuse;
        Colour mSpecular;
        Radian mSpotOuter;
        Radian mSpotInner;
        NIIf mSpotFalloff;
        ShadowSetup * mShadowCamera;
        NIIf mShadowNearClip;
        NIIf mShadowFarClip;
        NIIf mShadowExtent;
    };
}
#endif