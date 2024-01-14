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
#include "NiiExtFrameBuffer.h"
#include "NiiFrameObj.h"

/**
    Behavior is consistent across all render systems, if, and only if, the same RSC flags are set
    RSC flags that affect this class are:
    * GF_RenderTarget_SelfDepth:
        The RTT can create a custom depth buffer different from the main depth buffer. This means,
        an RTT is able to not share it's depth buffer with the main window if it wants to.
    * GF_RenderTarget_AttachMainDepth:
        When GF_RenderTarget_SelfDepth is set, some APIs (ie. OpenGL w/ FBO) don't allow using
        the main depth buffer for offscreen RTTs. When this flag is set, the depth buffer can be
        shared between the main window and an RTT.
    * GF_RenderTarget_LessDepth:
        When this flag isn't set, the depth buffer can only be shared across RTTs who have the EXACT
        same resolution. When it's set, it can be shared with RTTs as NIIl as they have a
        resolution less or equal than the depth buffer's.
*/
namespace NII
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ExtFrameBuffer
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    ExtFrameBuffer::ExtFrameBuffer(GroupID gid, NCount w, NCount h, NCount bit,
        Nui32 fsaa, const String & hint, bool manual) :
            mPoolId(gid),
            mWidth(w),
            mHeight(h),            
            mBitDepth(bit),
            mFsaa(fsaa),
            mFsaaHint(hint),
            mManual(manual)
    {
    }
    //-----------------------------------------------------------------------
    ExtFrameBuffer::~ExtFrameBuffer()
    {
        UserList temp;
        std::swap(temp, mUserList);
        UserList::const_iterator i, iend = temp.end();
        for(i = temp.begin(); i != iend; ++i)
        {
            (*i)->detachExt();
        }
    }
    //-----------------------------------------------------------------------
    void ExtFrameBuffer::setGroup(GroupID gid)
    {
        mPoolId = gid;

        UserList temp;
        std::swap(temp, mUserList);
        UserList::const_iterator i, iend = temp.end();
        for(i = temp.begin(); i != iend; ++i)
        {
            (*i)->detachExt();
        }
    }
    //-----------------------------------------------------------------------
    bool ExtFrameBuffer::isSuitable(FrameObj * fo) const
    {
        if(getWidth() >= fo->getWidth() && getHeight() >= fo->getHeight() &&
            getFsaa() == fo->getFSAA())
        {
            return true;
        }

        return false;
    }
    //-----------------------------------------------------------------------
    void ExtFrameBuffer::attach(FrameObj * obj)
    {
        if (mUserList.find(obj) == mUserList.end())
        {
            mUserList.insert(obj);
        }
    }
    //-----------------------------------------------------------------------
    void ExtFrameBuffer::detach(FrameObj * obj)
    {
        UserList::iterator itor = mUserList.find(obj);
        if (itor != mUserList.end())
        {
            mUserList.erase(itor);
        }
    }
    //-----------------------------------------------------------------------
}