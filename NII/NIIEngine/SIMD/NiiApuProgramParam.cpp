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
#include "NiiApuProgramParam.h"
#include "NiiSimdManager.h"
#include "NiiStrConv.h"
#include "NiiLogManager.h"

/*
int main()
{
    int a[5] = {1, 3, 0, 4, 6};
    int *p = (int*)(&a + 1);
    cout<<*(a + 1)<<' ';
    cout<<*(p - 1)<<endl;
    return 0;
}

下面说说a和&a的区别，说完了，再看该题的结果。

C/C++里面的数组名字会退化为指针，所以数组名a实际指的是数组的第一个元素的地址。而数组名作为指针来讲有特殊性，
它正在它所指向的内存区域中，&a的值和a的数值是相同的（可以输出观察一下），但是类型和意义不同。而指针的加法操
作和指向的数据类型密切相关。

所以，int a[10]; a就相当于int *，如果是对它加1（a + 1）是相当于a + 1 * sizeof(int)。但是&a的类型则相当于int **，
是所谓指向数组的指针，是数组元素类型的二级指针，对它加1是相当于 &a + 1 * sizeof(a)的，所以会偏移一整个数组长度。

现在来看程序，&a + 1其实偏移了一个数组的长度即就是5，其实在VS中通过调试可以查看&a + 1的类型，其类型为int[5] *，
所以(int*)(&a + 1)将其强制转成int *类型。这里应该很明显了a + 1指向的是数组a的第二个元素，p指向的是数组后面的元
素（这里我们不知道它是多少），p - 1则指向数组的最后一个元素！

所以程序运行的结果为3 6.
*/
namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ApuParamMap
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ApuParamMap::ApuParamMap(const ApuContext * ctx) :
        mCtx(ctx)
    {
    }
    //------------------------------------------------------------------------
    ApuParamMap::~ApuParamMap()
    {
    }
    //------------------------------------------------------------------------
    bool ApuParamMap::bind(Nidx idx, Nmark typemark, void * src, NCount unitSize, NCount unitCnt)
    {
        UnitList::iterator i = mUnitList.find(idx);
        if (i != mUnitList.end())
        {
            if (i->second.mDst)
            {
                N_Only(Simd).destroy(i->second.mDst);
                i->second.mDst = 0;
            }
            mUnitList.erase(i);
        }

        {
            if (N_MarkOnlyTrue(typemark, M_USAGEMARK, M_OnlyREAD))
            {
                mUnitList.insert(Npair(idx, Unit(src, src, typemark, unitSize, unitCnt)));
            }
            else
            {
                ApuBuffer * buf;
                N_Only(Simd).create(buf, mCtx, src, unitSize, unitCnt, typemark);
                mUnitList.insert(Npair(idx, Unit(src, buf, typemark, unitSize, unitCnt)));
            }
        }
        return true;
    }
    //------------------------------------------------------------------------
    void * ApuParamMap::lock(Nidx idx, Nmark opmark, NCount oft, NCount size)
    {
        UnitList::iterator i = mUnitList.find(idx);
        if (i != mUnitList.end())
        {
            i->second.mMutexMark = opmark;
            if (N_MarkOnlyTrue(i->second.mTypeMark, M_USAGEMARK, M_OnlyREAD))
            {
                N_assert(oft + size < N_ApuMaxConstSize, "");
                return i->second.mRawData + oft;
            }
            else
            {
                return i->second.mDst->lock(opmark, oft, size);
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void ApuParamMap::unlock(Nidx idx)
    {
        UnitList::iterator i = mUnitList.find(idx);
        if (i != mUnitList.end())
        {
            i->second.mMutexMark = 0;
            if (!(N_MarkOnlyTrue(i->second.mTypeMark, M_USAGEMARK, M_OnlyREAD)))
            {
                i->second.mDst->unlock();
            }
        }
    }
    //------------------------------------------------------------------------
    void ApuParamMap::read(Nidx idx, void * out, NCount oft, NCount size) const
    {
        UnitList::const_iterator i = mUnitList.find(idx);
        if (i != mUnitList.end())
        {
            if (N_MarkOnlyTrue(i->second.mTypeMark, M_USAGEMARK, M_OnlyREAD) )
            {
                memcpy(out, i->second.mRawData + oft, size);
            }
            else
            {
                i->second.mDst->read(out, oft, size);
            }
        }
    }
    //------------------------------------------------------------------------
    void ApuParamMap::write(Nidx idx, const void * in, NCount oft, NCount size)
    {
        UnitList::iterator i = mUnitList.find(idx);
        if (i != mUnitList.end())
        {
            if (N_MarkOnlyTrue(i->second.mTypeMark, M_USAGEMARK, M_OnlyREAD))
            {
                memcpy(i->second.mRawData + oft, in, size);
            }
            else
            {
                i->second.mDst->write(in, oft, size);
            }
        }
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    //      ApuProgramParam 方法
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    ApuProgramParam::ApuProgramParam():
        mParamCount(0)
    {
    }
    //-----------------------------------------------------------------------------
    ApuProgramParam::~ApuProgramParam()
    {
        removeAllDefault();
    }
    //-----------------------------------------------------------------------------
    ApuProgramParam::ApuProgramParam(const ApuProgramParam & o)
    {
        *this = o;
    }
    //-----------------------------------------------------------------------------
    bool ApuProgramParam::isValid() const
    {
        vector<bool>::type vaildtest;
        vaildtest.resize(mParamCount, false);

        if (mParamMap)
        {
            N_assert(mParamCount < mParamMap->getCount(), "");
            ApuParamMap::UnitList::const_iterator j, jend = mParamMap->getUnitList().end();
            for (j = mParamMap->getUnitList().begin(); j != jend; ++j)
            {
                if (j->first < mParamCount)
                    vaildtest[j->first] = true;
            }
        }
        ApuParamMap::UnitList::const_iterator s, send = mShareSyncList.end();
        for (s = mShareSyncList.begin(); s != send; ++s)
        {
            if (s->first < mParamCount)
                vaildtest[s->first] = true;
        }
        vector<bool>::type::iterator c, cend = vaildtest.end();
        for (c = vaildtest.begin(); c != cend; ++c)
        {
            if(*c == false)
                return false;
        }
        return true;
    }
    //-----------------------------------------------------------------------------
    ApuProgramParam & ApuProgramParam::operator=(const ApuProgramParam & o)
    {
        copyDefault(o);
        return *this;
    }
    //---------------------------------------------------------------------------
    void ApuProgramParam::copyDefault(const ApuProgramParam & src)
    {
        ApuParamMap::UnitList::const_iterator j, jend = src.mShareSyncList.end();
        for (j = src.mShareSyncList.begin(); j != jend; ++j)
        {
            ApuParamMap::UnitList::iterator i = mShareSyncList.find(j->first);
            if (i != mShareSyncList.end())
            {
                if (i->second.mDst)
                {
                    N_Only(Simd).destroy(i->second.mDst);
                    i->second.mDst = 0;
                }
                mShareSyncList.erase(i);
            }
            const ApuParamMap::Unit & srcunit = j->second;
            if (N_MarkOnlyTrue(srcunit.mTypeMark, M_USAGEMARK, M_OnlyREAD))
            {
                mShareSyncList.insert(Npair(j->first, ApuParamMap::Unit(srcunit.mSrc, srcunit.mRawData, srcunit.mTypeMark, srcunit.mUnitSize, srcunit.mUnitCount)));
            }
            else
            {
                ApuBuffer * buf;
                N_Only(Simd).create(buf, mParamMap->getContext(), srcunit.mSrc, srcunit.mUnitSize, srcunit.mUnitCount, srcunit.mTypeMark);
                mShareSyncList.insert(Npair(j->first, ApuParamMap::Unit(srcunit.mSrc, buf, srcunit.mTypeMark, srcunit.mUnitSize, srcunit.mUnitCount)));
            }
        }
    }
    //-----------------------------------------------------------------------
    bool ApuProgramParam::addDefault(Nidx idx, Nmark typemark, void * src, NCount unitSize, NCount unitCnt)
    {
        ApuParamMap::UnitList::iterator i = mShareSyncList.find(idx);
        if (i != mShareSyncList.end())
        {
            if (i->second.mDst)
            {
                N_Only(Simd).destroy(i->second.mDst);
                i->second.mDst = 0;
            }
            mShareSyncList.erase(i);
        }

        {
            if (N_MarkOnlyTrue(typemark, M_USAGEMARK, M_OnlyREAD))
            {
                mShareSyncList.insert(Npair(idx, ApuParamMap::Unit(src, src, typemark, unitSize, unitCnt)));
            }
            else
            {
                ApuBuffer * buf;
                N_Only(Simd).create(buf, mParamMap->getContext(), src, unitSize, unitCnt, typemark);
                mShareSyncList.insert(Npair(idx, ApuParamMap::Unit(src, buf, typemark, unitSize, unitCnt)));
            }
        }
        return true;
    }
    //-----------------------------------------------------------------------
    void ApuProgramParam::removeDefault(Nidx idx)
    {
        ApuParamMap::UnitList::iterator i = mShareSyncList.find(idx);
        if (i != mShareSyncList.end())
        {
            if(i->second.mDst)
                N_Only(Simd).destroy(i->second.mDst);
            mShareSyncList.erase(i);
        }
    }
    //-----------------------------------------------------------------------
    void ApuProgramParam::removeAllDefault()
    {
        ApuParamMap::UnitList::iterator i, iend = mShareSyncList.end();
        for (i = mShareSyncList.begin(); i != iend; ++i)
        {
            if (i->second.mDst)
                N_Only(Simd).destroy(i->second.mDst);
        }
        mShareSyncList.clear();
    }
    //---------------------------------------------------------------------
    void * ApuProgramParam::lockDefault(Nidx idx, Nmark opmark, NCount oft, NCount size)
    {
        ApuParamMap::UnitList::iterator i = mShareSyncList.find(idx);
        if (i != mShareSyncList.end())
        {
            i->second.mMutexMark = opmark;
            if (N_MarkOnlyTrue(i->second.mTypeMark, M_USAGEMARK, M_OnlyREAD))
            {
                N_assert(oft + size < N_ApuMaxConstSize, "");
                return i->second.mRawData + oft;
            }
            else
            {
                return i->second.mDst->lock(opmark, oft, size);
            }
        }
        return 0;
    }
    //---------------------------------------------------------------------
    void ApuProgramParam::unlockDefault(Nidx idx)
    {
        ApuParamMap::UnitList::iterator i = mShareSyncList.find(idx);
        if (i != mShareSyncList.end())
        {
            i->second.mMutexMark = 0;
            if (!(N_MarkOnlyTrue(i->second.mTypeMark, M_USAGEMARK, M_OnlyREAD)))
            {
                i->second.mDst->unlock();
            }
        }
    }
    //---------------------------------------------------------------------
    void ApuProgramParam::readDefault(Nidx idx, void * out, NCount oft, NCount size) const
    {
        ApuParamMap::UnitList::const_iterator i = mShareSyncList.find(idx);
        if (i != mShareSyncList.end())
        {
            if (N_MarkOnlyTrue(i->second.mTypeMark, M_USAGEMARK, M_OnlyREAD))
            {
                memcpy(out, i->second.mRawData + oft, size);
            }
            else
            {
                i->second.mDst->read(out, oft, size);
            }
        }
    }
    //---------------------------------------------------------------------
    void ApuProgramParam::writeDefault(Nidx idx, const void * in, NCount oft, NCount size)
    {
        ApuParamMap::UnitList::iterator i = mShareSyncList.find(idx);
        if (i != mShareSyncList.end())
        {
            if (N_MarkOnlyTrue(i->second.mTypeMark, M_USAGEMARK, M_OnlyREAD))
            {
                memcpy(i->second.mRawData + oft, in, size);
            }
            else
            {
                i->second.mDst->write(in, oft, size);
            }
        }
    }
    //---------------------------------------------------------------------
}