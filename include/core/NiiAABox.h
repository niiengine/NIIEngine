/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-7

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

#ifndef _NII_AABOX_H_
#define _NII_AABOX_H_

#include "NiiPreInclude.h"
#include "NiiMatrix4.h"

namespace NII
{
    /** 轴对齐盒
    @version NIIEngine 3.0.0
    */
    class _EngineAPI AABox
    {
    public:
        AABox();

        AABox(const AABox & o);

        AABox(const Vector3f & min, const Vector3f & max);

        AABox(NIIf minx, NIIf miny, NIIf minz, NIIf maxx, NIIf maxy, NIIf maxz);

        ~AABox();

        AABox & operator=(const AABox & o);

        bool operator== (const AABox & o) const;
        bool operator!= (const AABox & o) const;

        /** 设置为空盒子
        @version NIIEngine 3.0.0
        */
        void setNull();

        /** 是否为空盒子
        @version NIIEngine 3.0.0
        */
        bool isNull() const;

        /** 获取最小角落
        @version NIIEngine 3.0.0
        */
        inline const Vector3f & getMin() const
        {
            return mMinimum;
        }

        /** 获取最大角落
        @version NIIEngine 3.0.0
        */
        inline const Vector3f & getMax() const
        {
            return mMaximum;
        }

        /** 设置最小角落
        @version NIIEngine 3.0.0
        */
        inline void setMin(const Vector3f & vec)
        {
            mMinimum = vec;
        }

        /** 设置最小角落
        @version NIIEngine 3.0.0
        */
        inline void setMin(NIIf x, NIIf y, NIIf z)
        {
            mMinimum.x = x;
            mMinimum.y = y;
            mMinimum.z = z;
        }

        /** 设置最大角落
        @version NIIEngine 3.0.0
        */
        inline void setMax(const Vector3f & vec)
        {
            mMaximum = vec;
        }

        /** 设置最大角落
        @version NIIEngine 3.0.0
        */
        inline void setMax(NIIf x, NIIf y, NIIf z)
        {
            mMaximum.x = x;
            mMaximum.y = y;
            mMaximum.z = z;
        }

        /** 合并输入盒子到现在盒子,这个结果包含全部
        @version NIIEngine 3.0.0
        */
        void merge(const AABox & o);

        /** 扩展盒子,包含指定点(如果需要的话).
        @version NIIEngine 3.0.0
        */
        void merge(const Vector3f & o);

        /** 计算这个盒子的体积
        @version NIIEngine 3.0.0
        */
        NIIf volume() const;

        /** 变换盒子
        @version NIIEngine 3.0.0
        */
        void transform(const Matrix4f & m);

        /** 仿射变换盒子
        @remark 意义和 transform 函数相同
        @note 必须是仿射距阵
        @version NIIEngine 3.0.0
        */
        void affine(const Matrix4f & m);

        /** 获取交集区域
        @version NIIEngine 3.0.0
        */
        AABox intersection(const AABox & o) const;

        /** 缩放
        @version NIIEngine 3.0.0
        */
        void scale(const Vector3f & s);

        /** 距离
        @version NIIEngine 3.0.0
        */
        NIIf distance(const Vector3f & v) const;

        /** 测试另一个盒子是否包含在这个盒子内
        @version NIIEngine 3.0.0
        */
        bool contains(const AABox & o) const;

        /** 返回这个盒子是否和另一个交叉
        @version NIIEngine 3.0.0
        */
        bool intersects(const AABox & o) const;

        /** 测试这个盒子是否交叉一个球
        @version NIIEngine 3.0.0
        */
        bool intersects(const Sphere & o) const;

        /** 测试这个盒子是否交叉一个面
        @version NIIEngine 3.0.0
        */
        bool intersects(const Plane & o) const;

        /** 测试向量点是否在这个盒子内
        @version NIIEngine 3.0.0
        */
        bool intersects(const Vector3f & o) const;

        /** 获取盒子的中心
        @version NIIEngine 3.0.0
        */
        Vector3f getCenter() const;

        /** 获取盒子的大小
        @version NIIEngine 3.0.0
        */
        Vector3f getSize() const;

        /** 计算边界半径值
        @version NIIEngine 3.0.0
        */
        NIIf calcRadius();

        static const AABox Null;
    public:
        Vector3f mMinimum;
        Vector3f mMaximum;
    };

    inline _EngineAPI std::ostream & operator << (std::ostream & o, const AABox & aab)
    {
        o << "AABox(min=" << aab.mMinimum << ", max=" << aab.mMaximum << ")";
        return o;
    }
}
#endif