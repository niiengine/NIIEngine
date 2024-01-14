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

#ifndef _NII_ROTATIONSPLINE_H_
#define _NII_ROTATIONSPLINE_H_

#include "NiiPreInclude.h"
#include "NiiQuaternion.h"

namespace NII
{
    /** 方向专用样条
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RotationSpline
    {
    public:
        RotationSpline();
        ~RotationSpline();

        /** 添加控制点
        @param[in] ctl 控制点
        @version NIIEngine 3.0.0
        */
        void add(const Quaternion & ctl);

        /** 获取控制点
        @param[in] index 索引
        @version NIIEngine 3.0.0
        */
        const Quaternion & get(Nidx index) const;

        /** 设置获取控制点
        @param[in] index 索引
        @param[in] ctl 控制点
        @verison NIIEngine 3.0.0
        */
        void set(Nidx index, const Quaternion & ctl);

        /** 获取控制点数量
        @version NIIEngine 3.0.0
        */
        NCount getCount() const;

        /** 清除这个样条所有点
        @version NIIEngine 3.0.0
        */
        void clear();

        /** 构建切线
        @remark 控制点变化后要调用
        @version NIIEngine 3.0.0
        */
        void rebuild();

        /** 插值
        @param[in] t 插值系数
        @param[in] shortpath 是否最短距离
        @version NIIEngine 3.0.0
        */
        Quaternion interpolate(NIIf t, bool shortpath = true);

        /** 插值
        @param[in] first 第一值
        @param[in] t 第一值与下一值插值系数
        @param[in] shortpath 是否最短距离
        @version NIIEngine 3.0.0
        */
        Quaternion interpolate(Nidx first, NIIf t, bool shortpath = true);
    protected:
        vector<Quaternion>::type mPoints;
        vector<Quaternion>::type mTangents;
    };
}
#endif