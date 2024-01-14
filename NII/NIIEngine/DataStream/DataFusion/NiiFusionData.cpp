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
#include "NiiFusionData.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // FusionNum
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    FusionNum::FusionNum()
    {
    }
    //------------------------------------------------------------------------
    FusionNum::~FusionNum()
    {
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // FusionData
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    FusionData::FusionData() :
        mObj(0),
        mAutoDestroy(true)
    {
    }
    //------------------------------------------------------------------------
    FusionData::FusionData(const FusionData & o)
    {
        if(o.mObj)
        {
            mObj = o.mObj->clone();
            mAutoDestroy = true;
        }
        else
        {
            mObj = 0;
            mAutoDestroy = false;
        }
    }
    //------------------------------------------------------------------------
    FusionData::FusionData(FusionNum * obj, bool copy, bool autodestory) :
        mAutoDestroy(autodestory)
    {
        if(copy && obj)
        {
            mObj = obj->clone();
        }
        else
        {
            mObj = obj;
        }
    }
    //------------------------------------------------------------------------
    FusionData::~FusionData()
    {
        if(mObj != 0 && mAutoDestroy)
        {
            N_delete mObj;
        }
        mObj = 0;
    }
    //------------------------------------------------------------------------
    void FusionData::setBaseValue(FusionNum * data, bool copy, bool autodestroy)
    {
        if(mObj != 0 && mAutoDestroy)
        {
            N_delete mObj;
        }

        if(copy)
        {
            if(data)
            {
                mObj = data->clone();
                mAutoDestroy = autodestroy;
            }
            else
            {
                mObj = 0;
                mAutoDestroy = true;
            }
        }
        else
        {
            mObj = data;
            mAutoDestroy = autodestroy;
        }
    }
    //--------------------------------------------------------------------------
    void FusionData::setBaseAsCurrent()
    {
        N_assert(mObj, "error");
        setValue(*mObj);
    }
    //--------------------------------------------------------------------------
    void FusionData::setCurrentAsBase()
    {
    }
    //--------------------------------------------------------------------------
    void FusionData::setValue(const FusionNum &)
    {
    }
    //--------------------------------------------------------------------------
    void FusionData::addValue(const FusionNum &)
    {
    }
    //--------------------------------------------------------------------------
    void FusionData::subValue(const FusionNum &)
    {
    }
    //--------------------------------------------------------------------------
    void FusionData::mulValue(const FusionNum &)
    {
    }
    //--------------------------------------------------------------------------
    void FusionData::divValue(const FusionNum &)
    {
    }
    //--------------------------------------------------------------------------
    void FusionData::FusionValue(const FusionData & k1, const FusionData & k2, NIIf t)
    {
        if(mObj == 0)
        {
            mObj = k1.mObj->cloneType();
            mAutoDestroy = true;
        }
        mObj->Fusion(k1.mObj, k2.mObj, t);
    }
    //--------------------------------------------------------------------------
    void FusionData::RelFusionValue(const FusionData & bas, const FusionData & k1, const FusionData & k2, NIIf t)
    {
        if(mObj == 0)
        {
            mObj = k1.mObj->cloneType();
            mAutoDestroy = true;
        }
        mObj->RelFusion(bas.mObj, k1.mObj, k2.mObj, t);
    }
    //--------------------------------------------------------------------------
    void FusionData::RelMulFusionValue(const FusionData & bas, const FusionData & k1, const FusionData & k2, NIIf t)
    {
        if(mObj == 0)
        {
            mObj = k1.mObj->cloneType();
            mAutoDestroy = true;
        }
        mObj->RelMulFusion(bas.mObj, k1.mObj, k2.mObj, t);
    }
    //--------------------------------------------------------------------------
    FusionData & FusionData::operator =(const FusionData & o)
    {
        if(mObj != 0 && mAutoDestroy)
        {
            N_delete mObj;
        }

        if(o.mObj)
        {
            mObj = o.mObj->clone();
            mAutoDestroy = true;
        }
        else
        {
            mObj = 0;
            mAutoDestroy = false;
        }
        return *this;
    }
    //------------------------------------------------------------------------
    FusionData FusionData::operator +(const FusionData & o) const
    {
        N_assert(mObj, "error");
        FusionNum * temp = mObj->cloneType();
        mObj->Add(o.mObj, temp);
        return FusionData(temp, false);
    }
    //------------------------------------------------------------------------
    FusionData FusionData::operator -(const FusionData & o) const
    {
        N_assert(mObj, "error");
        FusionNum * temp = mObj->cloneType();
        mObj->Sub(o.mObj, temp);
        return FusionData(temp, false);
    }
    //------------------------------------------------------------------------
    FusionData FusionData::operator *(const FusionData & o) const
    {
        N_assert(mObj, "error");
        FusionNum * temp = mObj->cloneType();
        mObj->Mul(o.mObj, temp);
        return FusionData(temp, false);
    }
    //------------------------------------------------------------------------
    FusionData FusionData::operator /(const FusionData & o) const
    {
        N_assert(mObj, "error");
        FusionNum * temp = mObj->cloneType();
        mObj->Div(o.mObj, temp);
        return FusionData(temp, false);
    }
    //------------------------------------------------------------------------
    FusionData FusionData::operator +(NIIf o) const
    {
        N_assert(mObj, "error");
        FusionNum * temp = mObj->cloneType();
        mObj->Add(o, temp);
        return FusionData(temp, false);
    }
    //------------------------------------------------------------------------
    FusionData FusionData::operator -(NIIf o) const
    {
        N_assert(mObj, "error");
        FusionNum * temp = mObj->cloneType();
        mObj->Sub(o, temp);
        return FusionData(temp, false);
    }
    //------------------------------------------------------------------------
    FusionData FusionData::operator *(NIIf o) const
    {
        N_assert(mObj, "error");
        FusionNum * temp = mObj->cloneType();
        mObj->Mul(o, temp);
        return FusionData(temp, false);
    }
    //------------------------------------------------------------------------
    FusionData FusionData::operator /(NIIf o) const
    {
        N_assert(mObj, "error");
        FusionNum * temp = mObj->cloneType();
        mObj->Div(o, temp);
        return FusionData(temp, false);
    }
    //------------------------------------------------------------------------
    FusionData operator +(NIIf left, const FusionData & right)
    {
        FusionNum * temp = right.mObj->cloneType();
        right.mObj->Add(left, temp);
        return FusionData(temp, false);
    }
    //------------------------------------------------------------------------
    FusionData operator -(NIIf left, const FusionData & right)
    {
        FusionNum * temp = right.mObj->cloneType();
        right.mObj->Sub(left, temp);
        return FusionData(temp, false);
    }
    //------------------------------------------------------------------------
    FusionData operator *(NIIf left, const FusionData & right)
    {
        FusionNum * temp = right.mObj->cloneType();
        right.mObj->Mul(left, temp);
        return FusionData(temp, false);
    }
    //------------------------------------------------------------------------
    FusionData operator /(NIIf left, const FusionData & right)
    {
        FusionNum * temp = right.mObj->cloneType();
        right.mObj->Div(left, temp);
        return FusionData(temp, false);
    }
    //------------------------------------------------------------------------
}