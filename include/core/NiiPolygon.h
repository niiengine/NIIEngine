/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_POLYGON_H_
#define _NII_POLYGON_H_

#include "NiiPreInclude.h"
#include "NiiVector3.h"

namespace NII
{
    /** 一个在三维空间描述多边形的类
    @remark 多个点描述的逆时针顺序多边形,并且所有点在某个平面上
	@version NIIEngine 3.0.0
    */
    class _EngineAPI Polygon
    {
    public:
        typedef vector<Vector3f>::type VertexList;
        typedef std::pair<Vector3f, Vector3f> Edge;
		typedef list<Edge>::type EdgeMap;
    public:
        Polygon();
        ~Polygon();

        Polygon(const Polygon & o);

		/** 确定当前对象和比较的相等
		@version NIIEngine 3.0.0
		*/
		bool operator == (const Polygon & o) const;

		/** 确定当前对象和比较的不相等
		@version NIIEngine 3.0.0
		*/
		bool operator != (const Polygon & o) const
		{
			return !(*this == o);
		}

        /** 插入一个顶点到一个指定位置
        @note 顶点必须共面
		@version NIIEngine 3.0.0
        */
        void insertVertex(const Vector3f & vertex, NCount idx);

        /** 插入一个顶点到多边形末尾
        @note 顶点必须共面
		@version NIIEngine 3.0.0
        */
        void insertVertex(const Vector3f & vertex);

		/** 删除特定的顶点
		@version NIIEngine 3.0.0
		*/
		void deleteVertex(NCount vertex);

        /** 返回指定顶点
        @version NIIEngine 3.0.0
		*/
		const Vector3f & getVertex(NCount idx) const;

        /** 设置一个多边形的一个指定顶点
        @note 顶点必须共面
		@version NIIEngine 3.0.0
        */
        void setVertex(const Vector3f & vertex, NCount idx);

        /** 顶点数量
        @version NIIEngine 3.0.0
		*/
		NCount getVertexCount() const;

        /** 返回多边形法线
        @version NIIEngine 3.0.0
		*/
		const Vector3f & getNormal() const;

        /** 确定点是否在多边形里面
        @remark 
            凸面的和平面的多边形，存在一个点在多边形的平面上,且在多边形的边界里
		@version NIIEngine 3.0.0
        */
        bool isPointInside(const Vector3f & point) const;

        /** 获取多边形边缘
		@par 1-2 2-3 3-4 4-1,逆时针顺序ccw
		@version NIIEngine 3.0.0
		*/
        void getEdge(EdgeMap * list) const;

		/** 获取多边形边缘
		@par 1-4 4-3 3-2 2-1,顺时针顺序cw
		@version NIIEngine 3.0.0
		*/
		void getClockwiseEdge(EdgeMap * list) const;

		/** 从多边形中移去重复顶点
		@version NIIEngine 3.0.0
		*/
		void opt();

        /** 重新设置对象
        @version NIIEngine 3.0.0
		*/
		void cleanAll();
    protected:
        /**更新法线
		@version NIIEngine 3.0.0
		*/
        void updateNormal() const;
    protected:
        VertexList          mVertexList;
        mutable Vector3f	mNormal;
        mutable bool        mIsNormalSet;
    };

	/** 输出顶点数据
	@version NIIEngine 3.0.0
	*/
	_EngineAPI std::ostream & operator<< (std::ostream & strm, const Polygon & poly);
}
#endif