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

#ifndef _NII_LINEARSPLINE_H_
#define _NII_LINEARSPLINE_H_

#include "NiiPreInclude.h"
#include "NiiVector3.h"
#include "NiiMatrix4.h"

namespace NII
{
    /** 实现线性样条曲线
    @remark Catmull-Rom
    @version NIIEngine 3.0.0
    */
    class _EngineAPI LinearSpline
    {
    public:
        LinearSpline();
        ~LinearSpline();

        /** 添加控制点
        @param ctl 控制点
        @version NIIEngine 3.0.0
        */
        void add(const Vector3f & ctl);

        /** 获取控制点
        @param index 索引
        @version NIIEngine 3.0.0
        */
        const Vector3f & get(Nidx index) const;

        /** 更新控制点
        @param[in] index 索引
        @param[in] ctl 控制点
        @remark 这个点必须存在于样条中
        */
        void set(Nidx index, const Vector3f & ctl);

        /** 控制点数量
        @version NIIEngine 3.0.0
        */
        NCount getCount() const;

        /** 构建切线
        @remark 控制点变化后要调用
        @version NIIEngine 3.0.0
        */
        void rebuild();

        /** 清除所有控制点
        @version NIIEngine 3.0.0
        */
        void clear();

        /** 插值
        @param[in] t 插值系数
        */
        Vector3f interpolate(NIIf t) const;

        /** 插值
        @param[in] first 第一值
        @param[in] t 第一值与下一值插值系数
        */
        Vector3f interpolate(Nidx first, NIIf t) const;
    protected:
        vector<Vector3f >::type mPoints;
        vector<Vector3f >::type mTangents;
        Matrix4f mHermite;
    };
}
#endif