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
#include "NiiSocketIO.h"
#include "NiiEngine.h"
#include "NiiTimer.h"

namespace NII
{
    //------------------------------------------------------------------------
    class SocketIOThreadMain : public ThreadMain
    {
    public:
        SocketIOThreadMain(SocketIO * io) : mSocketIO(io)
        {
            mTimerTick = N_Engine().getTimer()->getMS();
        }

        void run(void * arg)
        {
            current = N_Engine().getTimer()->getMS();
            mSocketIO->onTimer(current - mTimerTick);
            mTimerTick = current;
        }
    protected:
        SocketIO * mSocketIO;
        Nul mTimerTick;
    };
    //------------------------------------------------------------------------
    SocketIO::SocketIO() :
        mTimer(0),
        mSocketPrc(0),
        mBusy(false),
        mMessageSplit(false)
    {
        mLastSend = mLastReceive = N_Engine().getTimer()->getMS();
    }
    //------------------------------------------------------------------------
    SocketIO::~SocketIO()
    {
        if(mTimer)
        {
            N_delete mTimer;
            mTimer = 0;
        }
    }
    //------------------------------------------------------------------------
    NIIi SocketIO::send(void * data, NCount size)
    {
        RingBuffer * sbuff = N_new RingBuffer();
        sbuff->write(data, size);
        {
            ScopeLock tmutex(mOutMutex);
            mOutList.push_back(sbuff);
        }
        return size;
    }
    //------------------------------------------------------------------------
    NIIi SocketIO::sendHead(void * data, NCount size)
    {
        RingBuffer * sbuff = N_new RingBuffer();
        sbuff->write(data, size);
        {
            ScopeLock tmutex(mOutMutex);
            if(mMessageSplit)
            {
                BufferList::iterator i = mOutList.begin();
                ++i;
                mOutList.insert(i, sbuff);
            }
            else
            {
                mOutList.push_front(sbuff);
            }
        }
        return size;
    }
    //------------------------------------------------------------------------
    void SocketIO::setTimer(bool set, TimeDurMS delay, TimeDurMS interval, bool sync)
    {
        if(mTimer)
        {
            N_Only(Socket)->removeTimer(mTimer, 0);
            N_delete mTimer;
            mTimer = 0;
        }
        if(set)
        {
            mTimer = N_new SocketIOThreadMain(this);
            N_Only(Socket)->addTimer(mTimer, 0, interval, sync);
        }
    }
    //------------------------------------------------------------------------
    void SocketIO::setMessageTimer(bool sync)
    {
        
    }
    //------------------------------------------------------------------------
    NCount SocketIO::getSendSize() const
    {
        NCount cnt = 0;
        ScopeLock tmutex(mOutMutex);
        BufferList::iterator i, iend = mOutList.end();
        for(i = mOutList.begin(); i != iend; ++i)
        {
            cnt += (*i)->getWriteSize();
        }
        return cnt;
    }
    //------------------------------------------------------------------------
    void SocketIO::onRead()
    {
        mLastReceive = N_Engine().getTimer()->getMS();

        while (1)
        {
            uint32_t freesize = mInBuffer.getAllocSize() - mInBuffer.getWriteSize();
            if (freesize < N_Socket_ReadBufferSize)
                mInBuffer.enlarge(N_Socket_ReadBufferSize);

            int resize = mSocketPrc->receive(mInBuffer.getBuffer() + mInBuffer.getWriteSize(), N_Socket_ReadBufferSize);
            if (resize <= 0)
                break;

            mInBuffer.writeSkip(resize);
        }

        Message * msg = 0;
        try
        {
            while((msg = create(mInBuffer.getBuffer(), mInBuffer.getWriteSize())))
            {
                NCount msgsize = msg->getWriteSize();
                
                //onMessage(msg);
                mInMutex.lock();
                mInList.push_back(msg);
                mInMutex.unlock();
                //msg = 0;
                mInBuffer.readSkip(msgsize);
            }
        } 
        catch(Exception & ex) 
        {
            if (msg) 
            {
                N_delete msg;
                msg = 0;
            }
            mSocketPrc->onClose();
        }
    }
    //------------------------------------------------------------------------
    void SocketIO::onWrite()
    {
        mLastSend = N_Engine().getTimer()->getMS();

        mOutMutex.lock();
        while (mOutList.size() > 0) 
        {
            mMessageSplit = true;
            RingBuffer * sbuff = mOutList.front();
            mOutMutex.unlock();
            
            while(sbuff->getWriteSize() > 0)
            {
                NCount sesize = sbuff->getWriteSize();
                if (sesize > N_Socket_MaxBufferSize) 
                {
                    sesize = N_Socket_MaxBufferSize;
                }

                NIIi lsesize = mSocketPrc->send(sbuff->getBuffer(), sesize);
                if (lsesize <= 0) 
                {
                    lsesize = 0;
                    goto exitsend;
                }

                sbuff->readSkip(lsesize);
            }
            N_delete sbuff;
            mOutMutex.lock();
            mOutList.pop_front();
            mMessageSplit = false;
        }
        mOutMutex.unlock();
        
    exitsend:
    }
    //------------------------------------------------------------------------
    virtual Message * SocketIO::create(Nui8 * buf, Nui32 size) const
    {
        if (size < sizeof(MdfMsgHeader))
            return 0;

        Mui32 msglen;
        MemStream::read(buf, msglen);
        if (msglen > size)
        {
            return 0;
        }

        MdfMessage * msg = N_new MdfMessage(buf, msglen, false, true);

        return msg;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ServerSocketIO
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    SocketIO * ServerSocketIO::createInstance() const
    {
        return N_new ServerSocketIO();
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ClientSocketIO
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    SocketIO * ClientSocketIO::createInstance() const
    {
        return N_new ClientSocketIO();
    }
    //------------------------------------------------------------------------
}
