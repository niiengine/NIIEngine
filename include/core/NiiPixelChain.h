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

#ifndef _NII_PixelChain_H_
#define _NII_PixelChain_H_

#include "NiiPreInclude.h"
#include "NiiGeometryObj.h"
#include "NiiSpaceObj.h"
#include "NiiVertex.h"
#include "NiiSpaceObjFactory.h"

namespace NII
{
    /** 相素链
    @remark 粒子的一种
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelChain : public SpaceObj, public GeometryObj
    {
    public:
        PixelChain(SpaceID sid, NCount maxpixel = 18, NCount count = 1,
            bool texcoord = true, bool colour = true);
        virtual ~PixelChain();

        /** 设置材质
        @version NIIEngine 3.0.0
        */
        virtual void setMaterial(ResourceID rid, GroupID gid = GroupUnknow);

        /** 获取资源id
        @version NIIEngine 3.0.0
        */
        virtual ResourceID getMaterialID() const;

        /** 添加像素
        @version NIIEngine 3.0.0
        */
        virtual void add(Nidx index, const Pixel & e);

        /** 移去链的所有像素
        @version NIIEngine 3.0.0
        */
        virtual void removeAll(Nidx index);

        /** 修改链的像素
        @param[in] index
        @version NIIEngine 3.0.0
        */
        virtual void modify(Nidx index, Nidx eindex, const Pixel & e);

        /** 获取链的像素
        @param[in] index
        @version NIIEngine 3.0.0
        */
        virtual const Pixel & getPixel(Nidx index, NCount eindex) const;

        /** 获取链的像素数量
        @version NIIEngine 3.0.0
        */
        virtual NCount getCount(Nidx index) const;

        /** 移去所有链所有像素
        @version NIIEngine 3.0.0
        */
        virtual void removeAll();

        /** 设置每链像素最大数量
        @version NIIEngine 3.0.0
        */
        virtual void setMaxPixelCount(NCount max);

        /** 获取每链像素最大数量
        @version NIIEngine 3.0.0
        */
        virtual NCount getMaxPixelCount() const;

        /** 设置链数量
        @version NIIEngine 3.0.0
        */
        virtual void setChainCount(NCount count);

        /** 获取链数量
        @version NIIEngine 3.0.0
        */
        virtual NCount getChainCount() const;

        /** 设置是否平行与视图
        @version NIIEngine 3.0.0
        */
        inline void setParallelView(bool b)         { mFaceCamera = b; }

        /** 获取是否平行与视图
        @version NIIEngine 3.0.0
        */
        inline bool isParallelView() const          { return mFaceCamera; }

        /** 设置是否使用顶点颜色
        @version NIIEngine 3.0.0
        */
        void setVertexColour(bool b);

        /** 获取是否使用顶点颜色
        @version NIIEngine 3.0.0
        */
        inline bool isVertexColour() const          { return mTexColour; }

        /** 设置是否使用纹理坐标
        @version NIIEngine 3.0.0
        */
        void setTextureCoord(bool b, NIIf start = 0.0f, NIIf end = 1.0f);

        /** 获取是否使用纹理坐标
        @version NIIEngine 3.0.0
        */
        inline bool isTextureCoord() const          { return mTexCoord; }

        /** 纹理坐标最小值
        @version NIIEngine 3.0.0
        */
        inline NIIf getTextureCoordMin() const      { return mTexCoordMin; }

        /** 纹理坐标最大值
        @version NIIEngine 3.0.0
        */
        inline NIIf getTextureCoordMax() const      { return mTexCoordMax; }

        /** 设置是否纹理x方向寻址
        @version NIIEngine 3.0.0
        */
        inline void setTexCoordU(bool b)            { mTexCoordU = b; }

        /** 获取是否纹理x方向寻址
        @version NIIEngine 3.0.0
        */
        inline bool getTexCoordU()                  { return mTexCoordU; }

        /** 视图面基础
        @version NIIEngine 3.0.0
        */
        inline void setViewBase(const Vector3f & v) { v.normalise(mOriBase); }

        /** 视图面基础
        @version NIIEngine 3.0.0
        */
        inline const Vector3f & getViewBase() const { return mOriBase; }

        using SpaceObj::query;

        /// @copydetails SpaceObj::_notify
        void _notify(Camera * cam);

        /// @copydetails SpaceObj::query
        void query(GeometryQuery * query);

        /// @copydetails GeometryObj::distanceSquared
        NIIf distanceSquared(const Camera * cam) const;

        /// @copydetails SpaceObj::getCenterRange
        NIIf getCenterRange() const;

        /// @copydetails SpaceObj::getAABB
        const AABox & getAABB() const;

        ///@copydetails GeometryObj::getMaterial
        const Material * getMaterial() const;

        ///@copydetails SpaceObj::getFactoryID
        FactoryID getFactoryID() const;

        ///@copydetails SpaceObj::queue
        void queue(RenderQueue * queue);

        ///@copydetails GeometryObj::getGeometry
        void getGeometry(GeometryRaw & obj) const;

        ///@copydetails GeometryObj::getMatrix
        void getMatrix(Matrix4f * out, NCount & count) const;

        ///@copydetails GeometryObj::getLight
        void getLight(LightList & l) const;
    protected:
        virtual void resizeGeoCount();
        virtual void genGeoBuffer();
    protected:
        struct Chain
        {
            Nidx mBegin;
            NCount mCount;
            NCount mStart;
            NCount mEnd;
        };
        typedef vector<Chain>::type ChainList;
    protected:
        PixelList mPixelList;
        ChainList mChainList;
        VertexData * mVertexData;
        IndexData * mIndexData;
        ResourceID mMaterialID;
        Material * mMaterial;
        NCount mChainCount;
        NCount mPixelCount;
        Vector3f mOriBase;
        NIIf mTexCoordMin;
        NIIf mTexCoordMax;
        mutable NIIf mRadius;
        mutable AABox mAABB;
        mutable bool mAABBValid;
        bool mReGenGeoBuffer;
        bool mTexColour;
        bool mTexCoord;
        bool mTexCoordU;
        bool mUnitChange;
        bool mIndexValid;
        bool mFaceCamera;
    };
    
    /** 创建PixelChain实例的工厂类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelChainFactory : public SpaceObjFactory
    {
    public:
        PixelChainFactory() {}
        ~PixelChainFactory() {}

        /// @copydetails SpaceObjFactory::getID
        FactoryID getID() const;

        /// @copydetails SpaceObjFactory::create
        SpaceObj * create(SpaceID sid, SpaceManager * mag, const PropertyData * params = 0);

        /// @copydetails SpaceObjFactory::destroy
        void destroy(void * obj);
    };
}
#endif