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
#ifndef _NII_GeometryPixel_H_
#define _NII_GeometryPixel_H_

#include "NiiPreInclude.h"
#include "NiiRect.h"
#include "NiiTexture.h"
#include "NiiMatrix4.h"
#include "NiiGeometryRaw.h"
#include "NiiVertex.h"
#include "NiiColourArea.h"
#include "NiiEventArgs.h"

namespace NII
{
    /** 几何图层
    @version NIIEngine 3.0.0
    */
    enum GeometryLevel
    {
        GL_Base = 0,        ///< NIIEngine 渲染机制
        GL_Core = 1,        ///< NIIEngine 渲染机制基础物品
        GL_Overlay = 2,     ///< NIIEngine UI机制
        GL_Tip = 3,         ///< NIIEngine UI机制扩展
        GL_Ext = 4,         ///< NIIEngine 扩展
        GL_Unknow = 5
    };

    /** 混合模式
    @version NIIEngine 3.0.0
    */
    enum PixelBlendMode
    {
        PBM_Normal,
        PBM_Average,
        PBM_Unknow,
    };

    class RenderEffect;
    struct GeometryPixelBatch;

    /** 一个复合的复合缓存
    @remark
        相比引擎中的 VertexBuffer / IndexBuffer / FrameBuffer 这个类就更形象些,通
        过名字就可以发现这个缓存有多种缓存组成,最终体现出了很形象的事物.和它对应的则
        是 GeometrySpace, GeometrySpace 包含了阴影,动画概念,而这个类则不包含.
    @note
        这个类适合零散的三角形/四边形,不合适大规模有规律的三角形/四边形,所以使用这个
        类会产生更多的开销
    @par NIIEngine 3.0.0 有3个版本的Geometry
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GeometryPixel : public BufferAlloc
    {
    public:
        GeometryPixel(RenderSys * rs);

        ~GeometryPixel();

        /** 添加
        @param in
        @param count
        @version NIIEngine 3.0.0
        */
        void add(const Vertex * in, NCount count = 1);

        /** 添加
        @param[in] tex
        @param[in] texarea
        @param[in] posarea
        @param[in] posclip
        @param[in] ca
        @version NIIEngine 3.0.0
        */
        void add(Texture * tex, const Rectf & texarea, const Rectf & posarea,
            const Rectf * posclip, const ColourArea & ca);

        /** 添加
        @param[in] in
        @param[in] area
        @param[in] clip
        @param[in] ca
        @version NIIEngine 3.0.0
        */
        void add(const PixelBuffer & in, const Rectf & area, const Rectf * clip,
            const ColourArea & ca);

        /** 执行渲染
        @remark 和 Opengl 里的 glFlush 函数概念差不多
        @version NIIEngine 3.0.0
        */
        void flush();

        /** 清理渲染
        @remark 和 Opengl 里的 glClear 函数概念差不多
        @version NIIEngine 3.0.0
        */
        void clear();

        /** 设置像素混合模式
        @param m 混合模式
        @version NIIEngine 3.0.0
        */
        inline void setBlendMode(PixelBlendMode set) { mMode = set; }

        /** 获取像素混合模式
        @version NIIEngine 3.0.0
        */
        inline PixelBlendMode getBlendMode() const { return mMode;  }

        /** 设置是否裁剪
        @param[in] b
        @version NIIEngine 3.0.0
        */
        void setClip(bool b);

        /** 设置裁剪区域
        @param[in] area 矩形裁剪
        @version NIIEngine 3.0.0
        */
        void setClip(const Recti & area);

        /** 是否裁剪
        @return
        @version NIIEngine 3.0.0
        */
        bool isClip() const;

        /** 位置偏量
        @param v
        */
        void setOffset(const Vector3f & oft);

        /** 位置旋转
        @param r
        */
        void setRotation(const Quaternion & r);

        /** 原点
        @remark 旋转依赖
        @param p 原点
        @version NIIEngine 3.0.0
        */
        void setOrigin(const Vector3f & pos);

        /** 获取最终距阵
        @version NIIEngine 3.0.0
        */
        const Matrix4f & getMatrix();

        /** 顶点数量
        @version NIIEngine 3.0.0
        */
        inline NCount getVertexCount() const { return mVertexList.size();  }

        /** 批次数量
        @version NIIEngine 3.0.0
        */
        inline NCount getBatchCount() const { return mBatchList.size();  }

        /** 设置当前纹理
        @param obj 纹理
        @note 不管理纹理内存
        @version NIIEngine 3.0.0
        */
        inline void setTexture(Texture * obj) { mTexture = obj;  }

        /** 获取当前纹理
        @note 不管理纹理内存
        @version NIIEngine 3.0.0
        */
        inline Texture * getTexture() const { return mTexture;  }

        /** 设置渲染效果
        @version NIIEngine 3.0.0
        */
        inline void setEffect(RenderEffect * obj) { mEffect = obj;  }

        /** 获取渲染效果
        @version NIIEngine 3.0.0
        */
        inline RenderEffect * getEffect() const { return mEffect;  }
    protected:
        GeometryPixel();
    protected:
        struct VertexData
        {
            NIIf mPosX, mPosY, mPosZ;
            RGBA mDiffuse;          ///< opengl
            NIIf mCoordU, mCoordV;
        };
        typedef vector<VertexData>::type VertexList;
        typedef vector<GeometryPixelBatch *>::type BatchList;
    private:
        RenderSys * mSys;
        Texture * mTexture;
        VertexBuffer * mBuffer;
        RenderEffect * mEffect;
        GeometryRaw mRenderOp;
        PixelBlendMode mMode;
        BatchList mBatchList;
        VertexList mVertexList;

        Recti mClipArea;
        Vector3f mOffset;
        Vector3f mOrigin;       ///< 原点
        Vector2f mSkinOffset;
        Quaternion mRot;

        Matrix4f mModelMatrix;   ///< 模型矩阵
        Nmark mMark;
    };

    /** 几何像素队列
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GeometryQueue : public UIAlloc
    {
    public:
        /** 把几何像素冲刷到渲染器中
        @version NIIEngine 3.0.0
        */
        void flush() const;

        /** 添加
        @note 本类不管理参数一的内存
        @param[in] obj 几何像素
        @version NIIEngine 3.0.0
        */
        void add(GeometryPixel * obj);

        /** 移去
        @note 本类不管理参数一的内存
        @param[in] obj 几何像素
        @version NIIEngine 3.0.0
        */
        void remove(GeometryPixel * obj);

        /** 移去所有
        @note 不管几何像素懂得内存
        @version NIIEngine 3.0.0
        */
        void clear();
    private:
        typedef vector<GeometryPixel *>::type Buffers;
    private:
        Buffers mObjs;
    };
}
#endif