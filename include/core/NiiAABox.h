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

        inline AABox & operator=(const AABox & o) { mMinimum = o.mMinimum; mMaximum = o.mMaximum; return *this; }
        inline bool operator== (const AABox & o) const { return mMinimum == o.mMinimum && mMaximum == o.mMaximum; }
        inline bool operator!= (const AABox & o) const { return mMinimum != o.mMinimum || mMaximum != o.mMaximum; }

        /** 设置为空盒子
        @version NIIEngine 3.0.0
        */
        inline void setNull() { mMinimum = 0.0f; mMaximum = 0.0f;}

        /** 是否为空盒子
        @version NIIEngine 3.0.0
        */
        inline bool isNull() const { return mMaximum - mMinimum < 0.000001f; }

        /** 获取最小角落
        @version NIIEngine 3.0.0
        */
        inline const Vector3f & getMin() const { return mMinimum; }

        /** 获取最大角落
        @version NIIEngine 3.0.0
        */
        inline const Vector3f & getMax() const { return mMaximum; }

        /** 设置最小角落
        @version NIIEngine 3.0.0
        */
        inline void setMin(const Vector3f & vec) { mMinimum = vec; }

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
        inline void setMax(const Vector3f & vec) { mMaximum = vec; }

        /** 设置最大角落
        @version NIIEngine 3.0.0
        */
        inline void setMax(NIIf x, NIIf y, NIIf z){ mMaximum.x = x; mMaximum.y = y; mMaximum.z = z; }

        /** 合并输入盒子到现在盒子,这个结果包含全部
        @version NIIEngine 3.0.0
        */
        inline void merge(const AABox & o) { mMinimum = mMinimum.floor(o.mMinimum); mMaximum = mMaximum.ceil(o.mMaximum); }

        /** 扩展盒子,包含指定点(如果需要的话).
        @version NIIEngine 3.0.0
        */
        inline void merge(const Vector3f & o) { mMaximum = mMaximum.ceil(o); mMinimum = mMinimum.floor(o);  }

        /** 计算这个盒子的体积
        @version NIIEngine 3.0.0
        */
        inline NIIf volume() const { return (mMaximum - mMinimum).volume(); }

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
        inline void scale(const Vector3f & s) { mMinimum = mMinimum * s; mMaximum = mMaximum * s; }

        /** 距离
        @version NIIEngine 3.0.0
        */
        NIIf distance(const Vector3f & v) const;

        /** 测试指定盒子是否包含在这个盒子内
        @version NIIEngine 3.0.0
        */
        inline bool contains(const AABox & o) const { return mMinimum <= o.mMinimum && o.mMaximum <= mMaximum; }

        /** 返回这个盒子是否与指定的交叉
        @version NIIEngine 3.0.0
        */
        inline bool intersects(const AABox & o) const { return (mMaximum > o.mMinimum && mMinimum < mMaximum) ? true : false; }

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
        inline bool intersects(const Vector3f & o) const { return o >= mMinimum && o <= mMaximum; }

        /** 获取盒子的中心
        @version NIIEngine 3.0.0
        */
        inline Vector3f getCenter() const { return Vector3f((mMaximum + mMinimum) * 0.5f); }

        /** 获取盒子的大小
        @version NIIEngine 3.0.0
        */
        inline Vector3f getSize() const { return mMaximum - mMinimum; }

        /** 计算边界半径值
        @version NIIEngine 3.0.0
        */
        NIIf calcRadius();

        static const AABox Null;
    public:
        Vector3f mMinimum;
        Vector3f mMaximum;
    };

    inline _EngineAPI Nostream & operator << (Nostream & o, const AABox & aab)
    {
        o << "AABox(min=" << aab.mMinimum << ", max=" << aab.mMaximum << ")";
        return o;
    }
}
#endif