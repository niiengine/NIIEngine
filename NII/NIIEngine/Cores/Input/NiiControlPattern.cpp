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
#include "NiiControlPattern.h"

namespace NII
{
namespace NII_MEDIA
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // SerialItem
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    SerialItem::SerialItem(NII_COMMAND::Functor * func) : mFunc(func)
    {
    }
    //------------------------------------------------------------------------
    void SerialItem::add(const SerialItemArgs * item)
    {
        mItemSerial.push_back(item);
    }
    //------------------------------------------------------------------------
    void SerialItem::add(NCount pos, const SerialItemArgs * item)
    {
        mItemSerial[pos] = item;
    }
    //------------------------------------------------------------------------
    void SerialItem::remove(NCount pos)
    {
        mItemSerial.erase(mItemSerial.begin() + pos);
    }
    //------------------------------------------------------------------------
    void SerialItem::compile()
    {
        mBegin = mItemSerial.begin();
        mCurrent = mBegin;
        mEnd = mItemSerial.end();
    }
    //------------------------------------------------------------------------
    bool SerialItem::check(const EventArgs * arg)
    {
        if(*mCurrent == static_cast<const SerialItemArgs *>(arg))
        {
            ++ mCurrent;
            if(mCurrent == mEnd)
            {
                mCurrent = mBegin;
                return true;
            }
        }
        else
        {
            mCurrent = mBegin;
        }
        return false;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ControlSerialArgs
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------    
    ControlSerialArgs::ControlSerialArgs(Nid id) : mSerialID(id) {}
    //------------------------------------------------------------------------
    ControlSerialArgs::~ControlSerialArgs() {}
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ControlItem
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ControlItem::ControlItem(bool includeRelease) :
        mInfluence(0),
        mSerial(includeRelease) {}
    //------------------------------------------------------------------------
    ControlItem::~ControlItem()
    {
        SerialStatusMap::iterator i, iend = mSerialStatuses.end();
        for (i = mSerialStatuses.begin(); i != iend; ++i)
        {
            N_delete i->second;
        }
        mSerialStatuses.clear();
    }
    //------------------------------------------------------------------------
    void ControlItem::add(Nui32 id, SerialItem * ser)
    {
        SerialStatusMap::iterator it = mSerialStatuses.find(id);
        if (it != mSerialStatuses.end())
        {
            remove(id);
        }
        mSerialStatuses.insert(Npair(id, ser));
    }
    //------------------------------------------------------------------------
    void ControlItem::remove(Nui32 id)
    {
        SerialStatusMap::iterator i = mSerialStatuses.find(id);
        if (i != mSerialStatuses.end())
        {
            N_delete i->second;
            mSerialStatuses.erase(i);
        }
    }
    //------------------------------------------------------------------------
    void ControlItem::raise(const EventArgs * arg)
    {
        if (mSerial)
        {
            SerialStatusMap::iterator it, itend = mSerialStatuses.end();
            for (it = mSerialStatuses.begin(); it != itend; it++)
            {
                if (it->second->check(arg))
                {   //´ï³É
                    ControlSerialArgs _arg(it->first);
                    onSerial(&_arg);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void ControlItem::onSerial(const ControlSerialArgs *)
    {
        // do nothing
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // QueueControlItem
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    QueueControlItem::QueueControlItem(bool includeRelease)
    {
    }
    //------------------------------------------------------------------------
    QueueControlItem::~QueueControlItem()
    {
    }
    //------------------------------------------------------------------------
    void QueueControlItem::raise(const EventArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    void QueueControlItem::onSerial(const ControlSerialArgs * arg)
    {

    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ControlPattern
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ControlPattern::ControlPattern() :
        mOwn(NULL),
        mID(0),
        mVendor(_T("Unknow Dev")){}
    //------------------------------------------------------------------------
    ControlPattern::ControlPattern(Nid id, const ViewWindow * own) :
        mID(id),
        mOwn(own),
        mVendor(_T("Unknow Dev")){}
    //-------------------------------------------------------------------------
    ControlPattern::~ControlPattern()
    {
    }
    //-------------------------------------------------------------------------
    void ControlPattern::bind(const ViewWindow * win)
    {
        mOwn = win;
    }
    //-------------------------------------------------------------------------
    void ControlPattern::setVendor(const String & v)
    {
        mVendor = v;
    }
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    QueueControlPattern::QueueControlPattern()
    {
    }
    //-------------------------------------------------------------------------
    QueueControlPattern::~QueueControlPattern()
    {
    }
    //-------------------------------------------------------------------------
}
}