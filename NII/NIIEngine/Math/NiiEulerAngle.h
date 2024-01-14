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

#include "NiiPreInclude.h"

namespace NII
{
    /* 正交矩阵,yaw * pitch * roll. yaw饶向上量旋转.pitch饶右轴旋转,roll饶方向轴旋转.
    @version NIIEngine 3.0.0
    */
    extern _EngineAPI bool eulerXYZ(const Matrix3f & src, Radian & Y, Radian & P, Radian & R);

    extern _EngineAPI bool eulerXZY(const Matrix3f & src, Radian & Y, Radian & P, Radian & R);

    extern _EngineAPI bool eulerYXZ(const Matrix3f & src, Radian & Y, Radian & P, Radian & R);

    extern _EngineAPI bool eulerYZX(const Matrix3f & src, Radian & Y, Radian & P, Radian & R);

    extern _EngineAPI bool eulerZXY(const Matrix3f & src, Radian & Y, Radian & P, Radian & R);

    extern _EngineAPI bool eulerZYX(const Matrix3f & src, Radian & Y, Radian & P, Radian & R);

    extern _EngineAPI void eulerXYZ(Matrix3f * dst, const Radian & Y, const Radian & P, const Radian & R);

    extern _EngineAPI void eulerXZY(Matrix3f * dst, const Radian & Y, const Radian & P, const Radian & R);

    extern _EngineAPI void eulerYXZ(Matrix3f * dst, const Radian & Y, const Radian & P, const Radian & R);

    extern _EngineAPI void eulerYZX(Matrix3f * dst, const Radian & Y, const Radian & P, const Radian & R);

    extern _EngineAPI void eulerZXY(Matrix3f * dst, const Radian & Y, const Radian & P, const Radian & R);

    extern _EngineAPI void eulerZYX(Matrix3f * dst, const Radian & Y, const Radian & P, const Radian & R);
}