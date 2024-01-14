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

#ifndef _NII_SIMDUtil_H_
#define _NII_SIMDUtil_H_

#include "NiiPreInclude.h"
#include "NiiGeoEdget.h"

namespace NII
{
    /** SIMD优化计算
    */
    class _EngineAPI SIMDUtil
    {
    public:
        virtual ~SIMDUtil() {}

        static void startup();

        static void shutdown();

        static SIMDUtil * getImplementation(void) { return mImpl; }

        /** 执行软件顶点贴皮.
        @param[in] srcPosPtr 指向资源缓存的位置
        @param[in] destPosPtr 指向目的缓存位置
        @param[in] srcNormPtr 指向资源法线缓存,如果为空,意味着仅混合位置
        @param[in] destNormPtr 指向目的法线缓存,如果srcNormPtr为空,忽略它
        @param[in] blendWeightPtr 指向混合权重缓存
        @param[in] blendIndexPtr 指向混合索引缓存
        @param[in] blendMatrices
            一个混合矩阵的指针针数,矩阵必须对齐为SIMD对齐方式,但并非数组本身必要的.
        @param[in] srcPosStride 资源位置的步距 单位字节
        @param[in] destPosStride 目的位置的步距 单位字节
        @param[in] srcNormStride 资源法线的步距 单位字节.
        @param[in] destNormStride 目的法线的步距 单位字节,如果srcNormPtr为空,忽视它.
        @param[in] blendWeightStride 混合权重缓存的步距 单位字节.
        @param[in] blendIndexStride 混合索引缓存的步距 单位字节.
        @param[in] numWeightsPerVertex 每顶点混合权重的数量,像混合指数
        @param[in] numVertices 混合的顶点数量.
        */
        virtual void softwareVertexSkinning(
            const NIIf * src_pos, NIIf * dst_pos,
            const NIIf * src_nrl, NIIf * dst_nrl,
            const NIIf * blendWeightPtr, const Nui8 * blendIndexPtr,
            const Matrix4f* const* blendMatrices,
            NCount srcPosStride, NCount destPosStride,
            NCount srcNormStride, NCount destNormStride,
            NCount blendWeightStride, NCount blendIndexStride,
            NCount numWeightsPerVertex,
            NCount numVertices) = 0;


        /** 执行一个软件顶点变形,容易使用到变形动画,它可以有的其它用途.
        @remark 这个函数,将在两个缓存间线性插补`位置`到第三个缓存中
        @param[in] t 开始到结束`位置`间的距离
        @param[in] f1 指向在缓存中的开始`位置`
        @param[in] f2 指向在缓存中的结束`位置`
        @param[in] dst 指向目的`位置`的缓存
        @param[in] pos1VSize, pos2VSize, dstVSize
            Vertex sizes in bytes of each of the 3 buffers referenced
        @param[in] numVertices
            变形的顶点数量,与开始,结束的数量和目的缓存一致.牢记每个顶点为3个浮点值
        */
        virtual void softwareVertexMorph(NIIf t, const NIIf * f1, const NIIf * f2,
            NIIf * dst, NCount pos1VSize, NCount pos2VSize,
            NCount dstVSize, NCount numVertices, bool morphNormals) = 0;

        /** 仿射矩阵
        @note 一个第3行等于(0, 0, 0, 1)的 4 * 4 仿射矩阵,例如 没有投射的系数
        @param[out] dst 仿射结果(数组)
        @param[in] base 仿射原点
        @param[in] src 仿射原(数组)
        @param[in] cnt 数组数量.
        */
        virtual void affine(Matrix4f * dst, const Matrix4f & base, const Matrix4f * src,  NCount cnt) = 0;

        /** 基于三角形位置信息计算出法面.
        @param[in] pos
            指向`位置`信息,包装为(x, y, z)格式,由三角形顶点索引的索引,无对齐请求
        @param[in] triangles
            三角形需要计算面法线,顶点位置由顶点索引 索引为"位置"信息
        @param[out] faceNormals
            Vector4的数组,用于存储三角形面法线,必须对齐为SIMD对齐格式
        @param[in] numTriangles 三角形数量,去计算面法线
        */
        virtual void calculateFaceNormals(const NIIf * pos, const GeoEdget::Triangle * triangles,
            Vector4f * faceNormals, NCount numTriangles) = 0;

        /** 根据三角形法面计算灯光照射情况
        @remark 这通常是计算一个轮廓的第一阶段.即建立面向灯光的三角形,和背向的
        @param[in] lightPos
            灯光在物体空间的四维位置,注意方向灯光(没有位置),w成分是0和x/y/z位置是方向
        @param[in] faceNormals
            对于三角形的一个面法线数组,面法线是一个单位向量正交到三角形,从原点加上距离.
            这个数组必须对齐为SIMD对齐方式.
        @param[out] lightFacings
            存储灯光面向状态结果的标记数组.相应的面法线面向灯光,结果标记为真,否则为假.
            该数组没有对齐要求
        @param[in] numFaces 去计算面法线的数量
        */
        virtual void calculateLightFacing(const Vector4f & lightPos, const Vector4f * faceNormals,
            NIIb * lightFacings, NCount numFaces) = 0;

        /** 一个根据灯光位置的固定距离,积压顶点.
        @param[in] pos
            四维灯光位置.当w = 0.0f描述的是一个方向灯光.否则,w必须等于1.0f描述的是一个点
            灯光
        @param[in] extrudeDist 挤出的距离
        @param[in] src
            指向资源顶点"位置"的缓存,"位置"是一个三维向量,包装到(x,y,z)格式,没有SIMD对
            齐的要求,但没有对齐数据会有性能的丢失
        @param[in] dest
            指向目的顶点"位置"的缓存,"位置"是一个三维向量,包装到(x,y,z)格式,没有SIMD对
            齐的要求,但没有对齐数据会有性能的丢失
        @param[in] numVertices 需要挤压的顶点数量,与资源和目的缓存一致
        */
        virtual void extrude(const Vector4f & pos, NIIf extrudeDist, const NIIf * src, NIIf * dest, NCount cnt) = 0;
       /*
        typedef void (*memcpy)(void *, void *, NCount);

        typedef void (*memset)(void *, NIIi, NCount);*/
    protected:
        SIMDUtil() {}
        SIMDUtil & operator=(const SIMDUtil & o) { return *this; }
    protected:
        static SIMDUtil * mImpl;
    };

    template <class T> static N_FORCEINLINE const T* rawOffsetPointer(const T* ptr, ptrdiff_t offset)
    {
        return (const T*)((const char*)(ptr)+offset);
    }

    template <class T> static N_FORCEINLINE T * rawOffsetPointer(T * ptr, ptrdiff_t offset)
    {
        return (T*)((NIIb*)(ptr) + offset);
    }

    template <class T> static N_FORCEINLINE void advanceRawPointer(const T*& ptr, ptrdiff_t offset)
    {
        ptr = rawOffsetPointer(ptr, offset);
    }

    template <class T> static N_FORCEINLINE void advanceRawPointer(T *& ptr, ptrdiff_t offset)
    {
        ptr = rawOffsetPointer(ptr, offset);
    }
}
#endif