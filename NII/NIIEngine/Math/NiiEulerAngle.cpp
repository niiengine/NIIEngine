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

#include "NiiPreProcess.h"
#include "NiiEulerAngle.h"

namespace NII
{
    //-------------------------------------------------------------------
    bool eulerXYZ(const Matrix3f & src, Radian & rfYAngle, Radian & rfPAngle, Radian & rfRAngle)
    {
        // rot =  cy*cz          -cy*sz           sy
        //        cz*sx*sy+cx*sz  cx*cz-sx*sy*sz -cy*sx
        //       -cx*cz*sy+sx*sz  cz*sx+cx*sy*sz  cx*cy

        rfPAngle = Radian(Math::ASin(src.m[0][2]));
        if (rfPAngle < Radian(Math::PI_2))
        {
            if (rfPAngle > Radian(-Math::PI_2))
            {
                rfYAngle = Math::ATan2(-src.m[1][2], src.m[2][2]);
                rfRAngle = Math::ATan2(-src.m[0][1], src.m[0][0]);
                return true;
            }
            else
            {
                // WARNING.  Not a unique solution.
                Radian fRmY = Math::ATan2(src.m[1][0], src.m[1][1]);
                rfRAngle = Radian(0.0);  // any angle works
                rfYAngle = rfRAngle - fRmY;
                return false;
            }
        }
        else
        {
            // WARNING.  Not a unique solution.
            Radian fRpY = Math::ATan2(src.m[1][0], src.m[1][1]);
            rfRAngle = Radian(0.0);  // any angle works
            rfYAngle = fRpY - rfRAngle;
            return false;
        }
    }
    //-------------------------------------------------------------------
    bool eulerXZY(const Matrix3f & src, Radian & rfYAngle, Radian & rfPAngle, Radian & rfRAngle)
    {
        // rot =  cy*cz          -sz              cz*sy
        //        sx*sy+cx*cy*sz  cx*cz          -cy*sx+cx*sy*sz
        //       -cx*sy+cy*sx*sz  cz*sx           cx*cy+sx*sy*sz

        rfPAngle = Math::ASin(-src.m[0][1]);
        if (rfPAngle < Radian(Math::PI_2))
        {
            if (rfPAngle > Radian(-Math::PI_2))
            {
                rfYAngle = Math::ATan2(src.m[2][1], src.m[1][1]);
                rfRAngle = Math::ATan2(src.m[0][2], src.m[0][0]);
                return true;
            }
            else
            {
                // WARNING.  Not a unique solution.
                Radian fRmY = Math::ATan2(-src.m[2][0], src.m[2][2]);
                rfRAngle = Radian(0.0);  // any angle works
                rfYAngle = rfRAngle - fRmY;
                return false;
            }
        }
        else
        {
            // WARNING.  Not a unique solution.
            Radian fRpY = Math::ATan2(-src.m[2][0], src.m[2][2]);
            rfRAngle = Radian(0.0);  // any angle works
            rfYAngle = fRpY - rfRAngle;
            return false;
        }
    }
    //-------------------------------------------------------------------
    bool eulerYXZ(const Matrix3f & src, Radian & rfYAngle, Radian & rfPAngle, Radian & rfRAngle)
    {
        // rot =  cy*cz+sx*sy*sz  cz*sx*sy-cy*sz  cx*sy
        //        cx*sz           cx*cz          -sx
        //       -cz*sy+cy*sx*sz  cy*cz*sx+sy*sz  cx*cy

        rfPAngle = Math::ASin(-src.m[1][2]);
        if (rfPAngle < Radian(Math::PI_2))
        {
            if (rfPAngle > Radian(-Math::PI_2))
            {
                rfYAngle = Math::ATan2(src.m[0][2], src.m[2][2]);
                rfRAngle = Math::ATan2(src.m[1][0], src.m[1][1]);
                return true;
            }
            else
            {
                // WARNING.  Not a unique solution.
                Radian fRmY = Math::ATan2(-src.m[0][1], src.m[0][0]);
                rfRAngle = Radian(0.0);  // any angle works
                rfYAngle = rfRAngle - fRmY;
                return false;
            }
        }
        else
        {
            // WARNING.  Not a unique solution.
            Radian fRpY = Math::ATan2(-src.m[0][1], src.m[0][0]);
            rfRAngle = Radian(0.0);  // any angle works
            rfYAngle = fRpY - rfRAngle;
            return false;
        }
    }
    //-------------------------------------------------------------------
    bool eulerYZX(const Matrix3f & src, Radian & rfYAngle, Radian & rfPAngle, Radian & rfRAngle)
    {
        // rot =  cy*cz           sx*sy-cx*cy*sz  cx*sy+cy*sx*sz
        //        sz              cx*cz          -cz*sx
        //       -cz*sy           cy*sx+cx*sy*sz  cx*cy-sx*sy*sz

        rfPAngle = Math::ASin(src.m[1][0]);
        if (rfPAngle < Radian(Math::PI_2))
        {
            if (rfPAngle > Radian(-Math::PI_2))
            {
                rfYAngle = Math::ATan2(-src.m[2][0], src.m[0][0]);
                rfRAngle = Math::ATan2(-src.m[1][2], src.m[1][1]);
                return true;
            }
            else
            {
                // WARNING.  Not a unique solution.
                Radian fRmY = Math::ATan2(src.m[2][1], src.m[2][2]);
                rfRAngle = Radian(0.0);  // any angle works
                rfYAngle = rfRAngle - fRmY;
                return false;
            }
        }
        else
        {
            // WARNING.  Not a unique solution.
            Radian fRpY = Math::ATan2(src.m[2][1], src.m[2][2]);
            rfRAngle = Radian(0.0);  // any angle works
            rfYAngle = fRpY - rfRAngle;
            return false;
        }
    }
    //-------------------------------------------------------------------
    bool eulerZXY(const Matrix3f & src, Radian & rfYAngle, Radian & rfPAngle, Radian & rfRAngle)
    {
        // rot =  cy*cz-sx*sy*sz -cx*sz           cz*sy+cy*sx*sz
        //        cz*sx*sy+cy*sz  cx*cz          -cy*cz*sx+sy*sz
        //       -cx*sy           sx              cx*cy

        rfPAngle = Math::ASin(src.m[2][1]);
        if (rfPAngle < Radian(Math::PI_2))
        {
            if (rfPAngle > Radian(-Math::PI_2))
            {
                rfYAngle = Math::ATan2(-src.m[0][1], src.m[1][1]);
                rfRAngle = Math::ATan2(-src.m[2][0], src.m[2][2]);
                return true;
            }
            else
            {
                // WARNING.  Not a unique solution.
                Radian fRmY = Math::ATan2(src.m[0][2], src.m[0][0]);
                rfRAngle = Radian(0.0);  // any angle works
                rfYAngle = rfRAngle - fRmY;
                return false;
            }
        }
        else
        {
            // WARNING.  Not a unique solution.
            Radian fRpY = Math::ATan2(src.m[0][2], src.m[0][0]);
            rfRAngle = Radian(0.0);  // any angle works
            rfYAngle = fRpY - rfRAngle;
            return false;
        }
    }
    //-------------------------------------------------------------------
    bool eulerZYX(const Matrix3f & src, Radian & rfYAngle, Radian & rfPAngle, Radian & rfRAngle)
    {
        // rot =  cy*cz           cz*sx*sy-cx*sz  cx*cz*sy+sx*sz
        //        cy*sz           cx*cz+sx*sy*sz -cz*sx+cx*sy*sz
        //       -sy              cy*sx           cx*cy

        rfPAngle = Math::ASin(-src.m[2][0]);
        if (rfPAngle < Radian(Math::PI_2))
        {
            if (rfPAngle > Radian(-Math::PI_2))
            {
                rfYAngle = Math::ATan2(src.m[1][0], src.m[0][0]);
                rfRAngle = Math::ATan2(src.m[2][1], src.m[2][2]);
                return true;
            }
            else
            {
                // WARNING.  Not a unique solution.
                Radian fRmY = Math::ATan2(-src.m[0][1], src.m[0][2]);
                rfRAngle = Radian(0.0);  // any angle works
                rfYAngle = rfRAngle - fRmY;
                return false;
            }
        }
        else
        {
            // WARNING.  Not a unique solution.
            Radian fRpY = Math::ATan2(-src.m[0][1], src.m[0][2]);
            rfRAngle = Radian(0.0);  // any angle works
            rfYAngle = fRpY - rfRAngle;
            return false;
        }
    }
    //-------------------------------------------------------------------
    void eulerXYZ(Matrix3f * dst, const Radian & fYAngle, const Radian & fPAngle, const Radian & fRAngle)
    {
        float fCos, fSin;

        fCos = Math::Cos(fYAngle);
        fSin = Math::Sin(fYAngle);
        Matrix3f kXMat(1.0, 0.0, 0.0, 0.0, fCos, -fSin, 0.0, fSin, fCos);

        fCos = Math::Cos(fPAngle);
        fSin = Math::Sin(fPAngle);
        Matrix3f kYMat(fCos, 0.0, fSin, 0.0, 1.0, 0.0, -fSin, 0.0, fCos);

        fCos = Math::Cos(fRAngle);
        fSin = Math::Sin(fRAngle);
        Matrix3f kZMat(fCos, -fSin, 0.0, fSin, fCos, 0.0, 0.0, 0.0, 1.0);

        *dst = kXMat*(kYMat*kZMat);
    }
    //-------------------------------------------------------------------
    void eulerXZY(Matrix3f * dst, const Radian & fYAngle, const Radian & fPAngle, const Radian & fRAngle)
    {
        float fCos, fSin;

        fCos = Math::Cos(fYAngle);
        fSin = Math::Sin(fYAngle);
        Matrix3f kXMat(1.0, 0.0, 0.0, 0.0, fCos, -fSin, 0.0, fSin, fCos);

        fCos = Math::Cos(fPAngle);
        fSin = Math::Sin(fPAngle);
        Matrix3f kZMat(fCos, -fSin, 0.0, fSin, fCos, 0.0, 0.0, 0.0, 1.0);

        fCos = Math::Cos(fRAngle);
        fSin = Math::Sin(fRAngle);
        Matrix3f kYMat(fCos, 0.0, fSin, 0.0, 1.0, 0.0, -fSin, 0.0, fCos);

        *dst = kXMat*(kZMat*kYMat);
    }
    //-------------------------------------------------------------------
    void eulerYXZ(Matrix3f * dst, const Radian & fYAngle, const Radian & fPAngle, const Radian & fRAngle)
    {
        float fCos, fSin;

        fCos = Math::Cos(fYAngle);
        fSin = Math::Sin(fYAngle);
        Matrix3f kYMat(fCos, 0.0, fSin, 0.0, 1.0, 0.0, -fSin, 0.0, fCos);

        fCos = Math::Cos(fPAngle);
        fSin = Math::Sin(fPAngle);
        Matrix3f kXMat(1.0, 0.0, 0.0, 0.0, fCos, -fSin, 0.0, fSin, fCos);

        fCos = Math::Cos(fRAngle);
        fSin = Math::Sin(fRAngle);
        Matrix3f kZMat(fCos, -fSin, 0.0, fSin, fCos, 0.0, 0.0, 0.0, 1.0);

        *dst = kYMat*(kXMat*kZMat);
    }
    //-------------------------------------------------------------------
    void eulerYZX(Matrix3f * dst, const Radian & fYAngle, const Radian & fPAngle, const Radian & fRAngle)
    {
        float fCos, fSin;

        fCos = Math::Cos(fYAngle);
        fSin = Math::Sin(fYAngle);
        Matrix3f kYMat(fCos, 0.0, fSin, 0.0, 1.0, 0.0, -fSin, 0.0, fCos);

        fCos = Math::Cos(fPAngle);
        fSin = Math::Sin(fPAngle);
        Matrix3f kZMat(fCos, -fSin, 0.0, fSin, fCos, 0.0, 0.0, 0.0, 1.0);

        fCos = Math::Cos(fRAngle);
        fSin = Math::Sin(fRAngle);
        Matrix3f kXMat(1.0, 0.0, 0.0, 0.0, fCos, -fSin, 0.0, fSin, fCos);

        *dst = kYMat*(kZMat*kXMat);
    }
    //-------------------------------------------------------------------
    void eulerZXY(Matrix3f * dst, const Radian & fYAngle, const Radian & fPAngle, const Radian & fRAngle)
    {
        float fCos, fSin;

        fCos = Math::Cos(fYAngle);
        fSin = Math::Sin(fYAngle);
        Matrix3f kZMat(fCos, -fSin, 0.0, fSin, fCos, 0.0, 0.0, 0.0, 1.0);

        fCos = Math::Cos(fPAngle);
        fSin = Math::Sin(fPAngle);
        Matrix3f kXMat(1.0, 0.0, 0.0, 0.0, fCos, -fSin, 0.0, fSin, fCos);

        fCos = Math::Cos(fRAngle);
        fSin = Math::Sin(fRAngle);
        Matrix3f kYMat(fCos, 0.0, fSin, 0.0, 1.0, 0.0, -fSin, 0.0, fCos);

        *dst = kZMat*(kXMat*kYMat);
    }
    //-------------------------------------------------------------------
    void eulerZYX(Matrix3f * dst, const Radian & fYAngle, const Radian & fPAngle, const Radian & fRAngle)
    {
        float fCos, fSin;

        fCos = Math::Cos(fYAngle);
        fSin = Math::Sin(fYAngle);
        Matrix3f kZMat(fCos, -fSin, 0.0, fSin, fCos, 0.0, 0.0, 0.0, 1.0);

        fCos = Math::Cos(fPAngle);
        fSin = Math::Sin(fPAngle);
        Matrix3f kYMat(fCos, 0.0, fSin, 0.0, 1.0, 0.0, -fSin, 0.0, fCos);

        fCos = Math::Cos(fRAngle);
        fSin = Math::Sin(fRAngle);
        Matrix3f kXMat(1.0, 0.0, 0.0, 0.0, fCos, -fSin, 0.0, fSin, fCos);

        *dst = kZMat*(kYMat*kXMat);
    }
    //-------------------------------------------------------------------
}