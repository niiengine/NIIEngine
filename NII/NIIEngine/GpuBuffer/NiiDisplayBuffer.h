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
#ifndef _NII_DISPLAY_BUFFER_H_
#define _NII_DISPLAY_BUFFER_H_

#include "NiiPreInclude.h"
#include "NiiGeometryRaw.h"

namespace NII
{
    /** 显示缓存
    @remark 可以多次重复调用渲染的几何对象
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DisplayBuffer: public GPUAlloc
    {
    public:
        DisplayBuffer();
        virtual ~DisplayBuffer();

        /** 重新冲刷顶点
        @verison NIIEngine 3.0.0
        */
        virtual void reflush();

        /** 冲刷顶点
        @version NIIEngine 3.0.0
        */
        virtual void flush(SpaceManager * mag) = 0;

        /** 设置用于渲染几何到顶点缓存的材质名字
        @version NIIEngine 3.0.0
        */
        void setMaterialID(ResourceID rid);

        /** 获取用于渲染几何到顶点缓存的材质
        @version NIIEngine 3.0.0
        */
        inline Material * getMaterial() const{ return mMaterial; }

        /** 设置几何类型
        @note 不同渲染系统,可能会被局限
        @version NIIEngine 3.0.0
        */
        inline void setGeoOp(GeometryRaw::OperationType type){ mGeoOp = type; }

        /** 获取几何类型
        @note 不同渲染系统,可能会被局限
        @version NIIEngine 3.0.0
        */
        inline GeometryRaw::OperationType getGeoOp() const{ return mGeoOp; }

        /** 获取顶点来源
        @version NIIEngine 3.0.0
        */
        inline void setGeo(GeometryObj * src){ mGeo = src;  }

        /** 获取顶点来源
        @version NIIEngine 3.0.0
        */
        inline GeometryObj * getGeo() const{ return mGeo; }

        /** 设置缓存顶点最大数量
        @version NIIEngine 3.0.0
        */
        inline void setMaxVertexCount(NCount cnt){ mMaxVertexCount = cnt;  }

        /** 获取缓存顶点最大数量
        @version NIIEngine 3.0.0
        */
        inline NCount getMaxVertexCount() const{ return mMaxVertexCount; }

        /** 设置是否自动刷新顶点
        @version NIIEngine 3.0.0
        */
        inline void setAutoFlush(bool b){ mAutoFlush = b; }

        /** 获取是否自动刷新顶点
        @version NIIEngine 3.0.0
        */
        inline bool isAutoFlush() const{ return mAutoFlush; }

        /** 获取数据
        @remark 获取目标数据
        @version NIIEngine 3.0.0
        */
        inline VertexData * getData(){ return mVertexData; }

        /** 获取渲染操作
        @version NIIEngine 3.0.0
        */
        virtual void getGeometry(GeometryRaw & op) const = 0;
    protected:
        GeometryRaw::OperationType mGeoOp;
        Material * mMaterial;
        GeometryObj * mGeo;
        VertexData * mVertexData;
        NCount mMaxVertexCount;
        bool mRequestFlush;
        bool mAutoFlush;
    };
}
#endif