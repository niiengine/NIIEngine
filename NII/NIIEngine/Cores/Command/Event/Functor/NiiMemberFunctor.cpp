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
#include "NiiMemberFunctor.h"
#include "NiiEventObj.h"

namespace NII
{
namespace NII_COMMAND
{
    //------------------------------------------------------------------------
    MemberFunctor::MemberFunctor(EventObj * own) :
        mOwn(own)
    {
    }
    //------------------------------------------------------------------------
    MemberFunctor::~MemberFunctor()
    {
        mDestList.clear();
    }
    //------------------------------------------------------------------------
    void MemberFunctor::add(Condition::ConditionParam * factor)
    {
        mDestList.push_back(factor);
    }
    //------------------------------------------------------------------------
    void MemberFunctor::remove(Condition::ConditionParam * factor)
    {
        DestList::iterator i, iend = mDestList.end();
        for(i = mDestList.begin(); i != iend; ++i)
        {
            if(*i == factor)
            {
                mDestList.erase(i);
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    void MemberFunctor::setEnable(bool s)
    {
        mEnable = s;
    }
    //------------------------------------------------------------------------
    bool MemberFunctor::getEnable() const
    {
        return mEnable;
    }
    //------------------------------------------------------------------------
    bool MemberFunctor::isInvalid() const
    {
        return mDestList.size() == 0;
    }
    //------------------------------------------------------------------------
    bool MemberFunctor::execute(const EventArgs * args)
    {
        Condition * cn;
        Condition::ConditionParam * cp;

        if(!mEnable)
            return false;
        DestList::iterator i, iend = mDestList.end();
        for(i = mDestList.begin(); i != iend; ++i)
        {
            cp = *i;
            cn = cp->mOwn;
            if(check(args))
            {
                if(process(cp->mOperator, cp->mExpect))
                {
                    // 参子达成
                    cp->mOwn->reach(cp);
                    // 条件达成
                    if(cn->reached())
                    {
                        ValueEventArgs<EventObj> eventarg(*mOwn);
                        // 执行命令
                        cn->exec(&eventarg);
                        // 激活完成参数
                        ConditionReachArgs reachArg(cn);
                        // 激活完成
                        mOwn->onReach(&reachArg);

                        if(cn->mAutoReset)
                        {
                            cn->fail();
                        }
                        else
                        {
                            mOwn->remove(cn);
                            N_delete cn;
                        }
                    }
                }
                else
                {
                    // 参子不达成
                    cn->fail(cp);
                }
            }
        }
        return true;
    }
    //------------------------------------------------------------------------
}
}