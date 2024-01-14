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
#include "NiiCommon.h"
#include "NiiEventObj.h"
#include "NiiMemberFunctor.h"
#include "NiiCondition.h"
#include "NiiBitSet.h"

namespace NII
{
namespace NII_COMMAND
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // EventObj
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    EventObj::EventObj() :
        mEnable(true)
    {
    }
    //------------------------------------------------------------------------
    EventObj::~EventObj()
    {
        Conditions::iterator it, itend = mConditions.end();
        for(it = mConditions.begin(); it != itend; ++it)
            remove(*it);
    }
    //------------------------------------------------------------------------
    bool EventObj::add(Condition * con)
    {
        Conditions::iterator i, iend = mConditions.end();
        for(i = mConditions.begin(); i != iend; ++i)
        {
            if(con == *i)
                return false;
        }
        // ����¼�¼
        mConditions.push_back(con);

        NCount iter;
        NCount itermark;
        EventID eventid;
        Condition::ConditionParams::iterator j, jend = con->mParams.end();
        for(j = con->mParams.begin(); j != jend; ++j)
        {
            eventid = j->first;
            iter = eventid >> IOMark;
            itermark = eventid % IOValid;

            if(mEventMarks->isTrue(iter, itermark))
            {   // �����������ݸ���
                mMids[mEventMarks->getCount(iter, itermark)]->add(j->second);
            }
            else
            {
                // ����ʵ��
                MemberFunctor * mf = create(eventid);
                // ����ʵ��
                mf->add(j->second);
                // �����������ݸ���
                mMids.insert(mMids.begin() + mEventMarks->getCount(iter, itermark), mf);
                mEventMarks->setTrue(iter, itermark);
            }
        }
        return true;
    }
    //------------------------------------------------------------------------
    bool EventObj::remove(Condition * con)
    {
        NCount iter;
        NCount itermark;
        EventID eventid;
        Conditions::iterator i, iend = mConditions.end();
        for(i = mConditions.begin(); i != iend; ++i)
        {
            if(con == *i)
            {
                Condition::ConditionParams::iterator j, jend = con->mParams.end();
                for(j = con->mParams.begin(); j != jend; ++j)
                {
                    eventid = j->first;
                    iter = eventid >> IOMark;
                    itermark = eventid % IOValid;
                    if(mEventMarks->isTrue(iter, itermark))
                    {
                        Mids::iterator it = mMids.begin() + mEventMarks->getCount(iter, itermark);
                        // �ҵ��¼�����
                        MemberFunctor * mf = *it;
                        // ��ȥ�¼���������
                        mf->remove(j->second);
                        // �¼��������Ƿ��в���
                        if(mf->isInvalid())
                        {
                            // ��ȥʵ��
                            mMids.erase(it);
                            mEventMarks->setFalse(iter, itermark);
                            // ɾ��ʵ��
                            destroy(mf);
                        }
                    }
                }
                mConditions.erase(i);
                return true;
            }
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool EventObj::connect(EventID id)
    {
        NCount iter = id >> IOMark;
        NCount itermark = id % IOValid;
        if (mEventMarks->isTrue(iter, itermark))
        {
            mMids[mEventMarks->getCount(iter, itermark)]->setEnable(true);
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool EventObj::disconnect(EventID id)
    {
        NCount iter = id >> IOMark;
        NCount itermark = id % IOValid;
        if (mEventMarks->isTrue(iter, itermark))
        {
            mMids[mEventMarks->getCount(iter, itermark)]->setEnable(false);
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    void EventObj::setReceiveEnable(bool s, bool forever)
    {
        mEnable = s;
        if(forever && !s)
        {
            Conditions::iterator it, itend = mConditions.end();
            for(it = mConditions.begin(); it != itend; ++it)
                remove(*it);
        }
    }
    //------------------------------------------------------------------------
    bool EventObj::isReceiveEnable() const
    {
        return mEnable;
    }
    //------------------------------------------------------------------------
    bool EventObj::isInvalid() const
    {
        return mMids.begin() == mMids.end();
    }
    //------------------------------------------------------------------------
    void EventObj::receive(EventID id, const EventArgs * arg)
    {
        if(mEnable)
        {
            NCount iter = id >> IOMark;
            NCount itermark = id % IOValid;
            if(mEventMarks->isTrue(iter, itermark))
                mMids[mEventMarks->getCount(iter, itermark)]->execute(arg);
        }
    }
    //------------------------------------------------------------------------
    void EventObj::notifyReceive(NCount count)
    {
        mEventMarks->resize(count);
    }
    //------------------------------------------------------------------------
    void EventObj::onReach(const ConditionReachArgs * arg)
    {

    }
    //------------------------------------------------------------------------
}
}