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

#include "MdfMessage.h"
#include "MdfLogManager.h"

namespace Mdf
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Message
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Message::Message()
    {
        mData = NULL;
        mAllocSize = 0;
        mWriteSize = 0;
        mCopyData = true;
    }
    //-----------------------------------------------------------------------
    Message::Message(Mui8 * buf, Mui32 len, bool copydata)
    {
        mWriteSize = len;

        if (buf && copydata)
        {
            mAllocSize = 128;
            if (mAllocSize < len)
                mAllocSize = len;
            mData = N_alloc_t(Mui8, mAllocSize);
            memcpy(mData, buf, len);
            mCopyData = true;
        }
        else
        {
            mAllocSize = 0;
            mData = buf;
            mCopyData = false;
        }
    }
    //-----------------------------------------------------------------------
    Message::~Message()
    {
        mAllocSize = 0;
        mWriteSize = 0;
        if (mData && mCopyData)
        {
            N_free(mData);
            mData = NULL;
        }
    }
    //-----------------------------------------------------------------------
    Mui32 Message::write(const void * in, MCount size)
    {
        if (mWriteSize + size > mAllocSize)
        {
            enlarge(size);
        }

        if (in)
        {
            memcpy(mData + mWriteSize, in, size);
        }

        mWriteSize += size;

        return size;
    }
    //-----------------------------------------------------------------------
    Mui32 Message::read(void * out, MCount cnt) const
    {
        if (cnt > mWriteSize)
            cnt = mWriteSize;

        if (out)
            memcpy(out, mData, cnt);

        mWriteSize -= cnt;
        memmove(mData, mData + cnt, mWriteSize);
        return cnt;
    }
    //-----------------------------------------------------------------------
    void Message::readSkip(MCount cnt)
    {
        if (cnt > mWriteSize)
            cnt = mWriteSize;

        mWriteSize -= cnt;
        memmove(mData, mData + cnt, mWriteSize);
    }
    //-----------------------------------------------------------------------
    void Message::writeSkip(MCount cnt)
    {
        mWriteSize += cnt;
    }
    //-----------------------------------------------------------------------
    Message * Message::create(Mui8 * buf, Mui32 len) const
    {
        Message * msg = new Message(buf, len, true);
        return msg;
    }
    //-----------------------------------------------------------------------
    void Message::enlarge(MCount cnt)
    {
        mAllocSize = mWriteSize + cnt;
        mAllocSize += mAllocSize >> 2;
        if (mCopyData)
        {
            Mui8 * nbuf = (Mui8*)realloc(mData, mAllocSize);
            mData = nbuf;
        }
        else
        {
            if (mWriteSize > 0)
            {
                Mui8 * nbuf = (Mui8*)malloc(mAllocSize);
                memcpy(nbuf, mData, mWriteSize);
                mData = nbuf;
            }
            else
            {
                mData = (Mui8*)malloc(mAllocSize);
            }
            mCopyData = true;
        }

    }
    //-----------------------------------------------------------------------
    void Message::setProto(const google::protobuf::MessageLite * in)
    {
        Mui32 msgsize = in->ByteSize();

        if (mData && mCopyData)
        {
            if (msgsize > mAllocSize)
            {
                mAllocSize = msgsize;

                Mui8 * new_buf = (Mui8 *)realloc(mData, mAllocSize);
                mData = new_buf;
            }
        }
        else
        {
            mCopyData = true;
            mAllocSize = msgsize;
            mData = (Mui8 *)malloc(mAllocSize);
        }

        if (!in->SerializeToArray(mData, msgsize))
        {
            Mlog("pb msg miss required fields.");
        }
    }
    //-----------------------------------------------------------------------
    bool Message::toProto(google::protobuf::MessageLite * out) const
    {
        return out->ParseFromArray(mData, mWriteSize);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // MdfMessage
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    MdfMessage::MdfMessage()
    {
        mLength = 0;
        mMsgType = 0;
        mMsgID = 0;
        mSeqIdx = 0;
        mCmdID = 0;
    }
    //-----------------------------------------------------------------------
    MdfMessage::MdfMessage(Mui8 * buf, Mui32 len, bool copydata, bool readhead) :
        Message(buf, len, copydata)
    {
        if (buf && readhead)
        {
            MemStream is(buf, sizeof(MdfMsgHeader));

            is.read(&mLength);
            is.read(&mMsgType);
            is.read(&mMsgID);
            is.read(&mSeqIdx);
            mCmdID = (mMsgType << 8) | mMsgID;
        }
    }
    //-----------------------------------------------------------------------
    MdfMessage::~MdfMessage()
    {
    }
    //-----------------------------------------------------------------------
    Message * MdfMessage::create(Mui8 * buf, Mui32 len) const
    {
        if(len < sizeof(MdfMsgHeader))
            return 0;

        Mui32 msglen;
        MemStream::read(buf, msglen);
        if (msglen > len)
        {
            return 0;
        }

        MdfMessage * msg = new MdfMessage(buf, msglen, true, true);

        return msg;
    }
    //-----------------------------------------------------------------------
    void MdfMessage::setProto(const google::protobuf::MessageLite * msg)
    {
        Mui32 msgsize = msg->ByteSize();
        mWriteSize = msgsize + sizeof(MdfMsgHeader);

        if (mData && mCopyData)
        {
            if (mWriteSize > mAllocSize)
            {
                mAllocSize = mWriteSize;

                Mui8 * new_buf = (Mui8 *)realloc(mData, mAllocSize);
                mData = new_buf;
            }
        }
        else
        {
			mCopyData = true;
            mAllocSize = mWriteSize;
            mData = (Mui8 *)malloc(mAllocSize);
        }

        if (!msg->SerializeToArray(mData + sizeof(MdfMsgHeader), msgsize))
        {
            Mlog("pb msg miss required fields.");
        }

        MemStream::write(mData, mWriteSize);
        MemStream::write(mData + 4, mMsgType);
        MemStream::write(mData + 5, mMsgID);
        MemStream::write(mData + 6, mSeqIdx);
    }
    //-----------------------------------------------------------------------
    bool MdfMessage::toProto(google::protobuf::MessageLite * out) const
    {
        return out->ParseFromArray(mData + sizeof(MdfMsgHeader), mWriteSize - sizeof(MdfMsgHeader));
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // SocketMessage
    //-----------------------------------------------------------------------
    //------------------------------------------------------------------------
    SocketMessage::SocketMessage(NCount size, bool autoDestroy, bool inner):
        mID(UniqueID::INVALID),
        mAddress(0),
        mAutoDestroy(autoDestroy),
        mLocal(inner)
    {
        mWriteSize = 0;
        mAllocSize = size;
        if(size)
        {
            mData = N_alloc_t(Nui8, size);
            mBitCount = N_b_TO_B(size);
        }
        else
        {
            mData = 0;
            mBitCount = 0;
        }
    }
    //------------------------------------------------------------------------
    SocketMessage::SocketMessage(Nui8 * data, NCount size, bool copydata, bool autoDestroy, bool inner):
        Message(data, size, copydata),
        mID(UniqueID::INVALID),
        mAddress(0),
        mAutoDestroy(autoDestroy),
        mLocal(inner)
    {
        mBitCount = N_b_TO_B(size);
    }
    //------------------------------------------------------------------------
    SocketMessage::~SocketMessage()
    {
        if(mAutoDestroy)
        {
            N_free(mData);
			mData = 0;
        }
    }
    //------------------------------------------------------------------------
    void SocketMessage::setHeadSize(Nui32 size)
    {
        Nui32 * temp = (Nui32 *)mData;
        *temp = size;
    }
    //------------------------------------------------------------------------
    Nui32 SocketMessage::getHeadSize() const
    {
        Nui32 * temp = (Nui32 *)mData;
        return *temp;
    }
    //------------------------------------------------------------------------
    Nui8 * SocketMessage::getBodyData(NCount maxsize) const
    {
        if(mAllocSize >= maxsize)
            return mData + sizeof(Nui32);
        return 0;
    }
    //------------------------------------------------------------------------
}