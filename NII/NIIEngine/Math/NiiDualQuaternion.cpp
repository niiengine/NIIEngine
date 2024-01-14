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

/* dqconv.c

  Conversion routines between (regular quaternion, translation) and dual quaternion.

  Version 1.0.0, February 7th, 2007

  Copyright (C) 2006-2007 University of Dublin, Trinity College, All Rights 
  Reserved

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the author(s) be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Author: Ladislav Kavan, kavanl@cs.tcd.ie

*/

#include "NiiPreProcess.h"
#include "NiiDualQuaternion.h"
#include "NiiMath.h"
#include "NiiMatrix4.h"

namespace NII 
{
    //Based on dqconv.c from http://isg.cs.tcd.ie/projects/DualQuaternions/
    //-----------------------------------------------------------------------
    DualQuaternion::DualQuaternion() : 
        w(1), 
        x(0), 
        y(0), 
        z(0), 
        dw(1), 
        dx(0), 
        dy(0), 
        dz(0)
    {
    }
    //-----------------------------------------------------------------------
    DualQuaternion::DualQuaternion(NIIf * data)
    {
        memcpy(&w, data, sizeof(NIIf) * 8);
    }
    //-----------------------------------------------------------------------
    DualQuaternion::DualQuaternion(NIIf _w, NIIf _x, NIIf _y, NIIf _z, NIIf _dw, NIIf _dx, NIIf _dy, NIIf _dz) :
        w(w),
        x(x),
        y(y),
        z(z),
        dw(dw),
        dx(dx),
        dy(dy),
        dz(dz)
    {
    }
    //-----------------------------------------------------------------------
    DualQuaternion::DualQuaternion(const Matrix4f & rot)
    {
        from(rot);
    }
    //-----------------------------------------------------------------------
    DualQuaternion::DualQuaternion(const Quaternion & q, const Vector3f & trans)
    {
        from(q, trans);
    }
    //-----------------------------------------------------------------------
    bool DualQuaternion::isNaN() const
    {
        return Math::isNaN(w) || Math::isNaN(x) || Math::isNaN(y) || Math::isNaN(z) ||
            Math::isNaN(dw) || Math::isNaN(dx) || Math::isNaN(dy) || Math::isNaN(dz);
    }
    //-----------------------------------------------------------------------
    DualQuaternion & DualQuaternion::operator= (const DualQuaternion & o)
    {
        w = o.w;
        x = o.x;
        y = o.y;
        z = o.z;
        dw = o.dw;
        dx = o.dx;
        dy = o.dy;
        dz = o.dz;

        return *this;
    }
    //-----------------------------------------------------------------------
    bool DualQuaternion::operator== (const DualQuaternion & o) const
    {
        return (o.w == w) && (o.x == x) && (o.y == y) && (o.z == z) &&
            (o.dw == dw) && (o.dx == dx) && (o.dy == dy) && (o.dz == dz);
    }
    //-----------------------------------------------------------------------
    bool DualQuaternion::operator!= (const DualQuaternion & o) const
    {
        return (o.w != w) || (o.x != x) || (o.y != y) || (o.z != z) ||
            (o.dw != dw) || (o.dx != dx) || (o.dy != dy) || (o.dz != dz);
    }
    //-----------------------------------------------------------------------
    void DualQuaternion::swap(DualQuaternion & o)
    {
        std::swap(w, o.w);
        std::swap(x, o.x);
        std::swap(y, o.y);
        std::swap(z, o.z);
        std::swap(dw, o.dw);
        std::swap(dx, o.dx);
        std::swap(dy, o.dy);
        std::swap(dz, o.dz);
    }
    //-----------------------------------------------------------------------
    void DualQuaternion::from(const Quaternion & q, const Vector3f & trans)
    {
        // non-dual part (just copy the quaternion):
        w = q.w;
        x = q.x;
        y = q.y;
        z = q.z;
        
        // dual part:
        NIIf half = 0.5;
        dw = -half *  (trans.x * x + trans.y * y + trans.z * z ); 
        dx =  half *  (trans.x * w + trans.y * z - trans.z * y ); 
        dy =  half * (-trans.x * z + trans.y * w + trans.z * x ); 
        dz =  half *  (trans.x * y - trans.y * x + trans.z * w ); 
    }
    //Based on dqconv.c from http://isg.cs.tcd.ie/projects/DualQuaternions/
    //-----------------------------------------------------------------------
    void DualQuaternion::to(Quaternion & q, Vector3f & oft) const
    {
        // regular quaternion (just copy the non-dual part):
        q.w = w;
        q.x = x;
        q.y = y;
        q.z = z;

        // translation vector:
        NIIf doub = 2.0;
        oft.x = doub * (-dw * x + dx * w - dy * z + dz * y);
        oft.y = doub * (-dw * y + dx * z + dy * w - dz * x);
        oft.z = doub * (-dw * z - dx * y + dy * x + dz * w);
    }
    //-----------------------------------------------------------------------
    void DualQuaternion::from(const Matrix4f & kTrans)
    {
        Vector3f pos;
        Vector3f scale;
        Quaternion rot;

        kTrans.get(pos, scale, rot);
        from(rot, pos);
    }
    //-----------------------------------------------------------------------
    void DualQuaternion::to(Matrix4f & kTrans) const
    {
        Vector3f pos;
        Quaternion rot;
        to(rot, pos);

        Vector3f scale = Vector3f::UNIT;
        kTrans.set(pos, scale, rot);
    }
    //------------------------------------------------------------------------
}