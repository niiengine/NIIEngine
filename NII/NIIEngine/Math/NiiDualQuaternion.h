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

#ifndef _NII_DUALQuaternion_H_
#define _NII_DUALQuaternion_H_

#include "NiiPreInclude.h"
#include "NiiMath.h"

namespace NII
{
    /** 双四元数
    @note 用于骨骼蒙皮
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DualQuaternion
    {
    public:
        DualQuaternion();
        DualQuaternion(NIIf * data);
        DualQuaternion(NIIf w, NIIf x, NIIf y, NIIf z, NIIf dw, NIIf dx, NIIf dy, NIIf dz);
        DualQuaternion(const Quaternion & rot, const Vector3f & oft);
        DualQuaternion(const Matrix4f & rotspace);

        /** 获取值
        @version NIIEngine 3.0.0
        */
        inline NIIf operator [](NCount i) const
        {
            return *(&w+i);
        }

        /** 获取值
        @version NIIEngine 3.0.0
        */
        inline NIIf & operator [](NCount i)
        {
            return *(&w+i);
        }

        /** 赋值
        @version NIIEngine 3.0.0
        */
        DualQuaternion & operator= (const DualQuaternion & o);

        /** 是否相等
        @version NIIEngine 3.0.0
        */
        bool operator== (const DualQuaternion & o) const;

        /** 是否不等
        @version NIIEngine 3.0.0
        */
        bool operator!= (const DualQuaternion & o) const;

        /** 交换值
        @version NIIEngine 3.0.0
        */
        void swap(DualQuaternion & o);

        /** 从四元组和平移中构建
        @version NIIEngine 3.0.0
        */
        void from(const Quaternion & q, const Vector3f & oft);

        /** 解析出四元组和平移
        @version NIIEngine 3.0.0
        */
        void to(Quaternion & q, Vector3f & oft) const;

        /** 通过空间矩阵构建
        @version NIIEngine 3.0.0
        */
        void from(const Matrix4f & rotspace);

        /** 解析出空间矩阵
        @version NIIEngine 3.0.0
        */
        void to(Matrix4f & rotspace) const;

        /** 是否有效
        @version NIIEngine 3.0.0
        */
        bool isNaN() const;
    public:
        NIIf w, x, y, z, dw, dx, dy, dz;
    };

    inline _EngineAPI Nostream & operator << (Nostream & o, const DualQuaternion & q)
    {
        o << "DualQuaternion(" << q.w << "," << q.x << "," << q.y << "," << q.z << ","
            << q.dw << "," << q.dx << "," << q.dy << "," << q.dz << ")";
        return o;
    }
}
#endif
