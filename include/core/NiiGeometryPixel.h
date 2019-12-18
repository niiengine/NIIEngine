/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-7-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
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

namespace NII
{
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
        void setBlendMode(PixelBlendMode m);

        /** 获取像素混合模式
        @version NIIEngine 3.0.0
        */
        PixelBlendMode getBlendMode() const;

        /** 设置是否裁剪
        @param[in] b
        @version NIIEngine 3.0.0
        */
        void setClip(bool b);

        /** 设置裁剪区域
        @param[in] area 矩形裁剪
        @version NIIEngine 3.0.0
        */
        void setClip(const Rectf & area);

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
        NCount getVertexCount() const;

        /** 批次数量
        @version NIIEngine 3.0.0
        */
        NCount getBatchCount() const;

        /** 设置当前纹理
        @param obj 纹理
        @note 不管理纹理内存
        @version NIIEngine 3.0.0
        */
        void apply(Texture * obj);

        /** 获取当前纹理
        @note 不管理纹理内存
        @version NIIEngine 3.0.0
        */
		Texture * getTexture() const;

        /** 设置渲染效果
        @version NIIEngine 3.0.0
        */
        void apply(RenderEffect * obj);

        /** 获取渲染效果
        @version NIIEngine 3.0.0
        */
		RenderEffect * getEffect() const;
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
        RenderSys * mSys;       ///< 渲染系统
        Texture * mTexture;     ///<
        VertexBuffer * mBuffer; ///<
        RenderEffect * mEffect; ///<
        GeometryRaw mRenderOp;  ///<
        PixelBlendMode mMode;   ///<
        BatchList mBatchList;   ///<
        VertexList mVertexList; ///<

        Rectf mClipArea;        ///<
        Vector3f mOffset;       ///<
        Vector3f mOrigin;       ///< 原点
        Vector2f mSkinOffset;   ///<
        Quaternion mRot;        ///<

        Matrix4f mModelMatrix;   ///< 模型矩阵
        Nmark mMark;
    };
}
#endif