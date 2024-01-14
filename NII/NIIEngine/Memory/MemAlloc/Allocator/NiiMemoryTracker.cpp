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
#include "NiiPreInclude.h"
#include "NiiString.h"
#include "NiiMemoryTracker.h"

#if N_PLAT == N_PLAT_WIN32
    #include <windows.h>
    #if _UNICODE
    #define Nii_OutputString(str) ::OutputDebugStringW(str)
    #else
    #define Nii_OutputString(str) ::OutputDebugStringA(str)
    #endif
#else
    #define Nii_OutputString(str) std::cerr << str
    #define Nii_OutputString(str) std::cerr << str
#endif

namespace NII
{
    class TrackLog
    {
    public:
        size_t bytes;
        Nui32 pool;
        int line;
        String filename;
        String function;

        TrackLog() : bytes(0), line(0) {}

        TrackLog(size_t sz, Nui32 p, const Ntchar * file, int ln, const Ntchar * func) :
            bytes(sz),
            pool(p),
            line(ln)
        {
            if (file)
                filename = file;
            if (func)
                function = func;
        }
    };
#if N_MEMTRACK
    //--------------------------------------------------------------------------
    MemoryTracker::MemoryTracker() :
        mOutFile(_T("NiiMemLeaks.log")),
        mStdOut(true),
        mEnable(true),
        mAllocCount(0){}
    //--------------------------------------------------------------------------
    MemoryTracker::~MemoryTracker()
    {
        if (mEnable)
        {
            StringStream os;

            if (mAllocList.empty())
            {
                os << "NII Memory: No memory leaks" << std::endl;
            }
            else
            {
                os << "NII Memory: Detected memory leaks !!! " << std::endl;
                os << "NII Memory: (" << mAllocList.size() << ") Allocation(s) with total " << mAllocCount << " bytes." << std::endl;
                os << "NII Memory: Dumping allocations -> " << std::endl;

                AllocList::const_iterator i, iend = mAllocList.end();
                for (i = mAllocList.begin(); i != iend; ++i)
                {
                    const TrackLog * alloc = i->second;
                    if (!alloc->filename.empty())
                        os << alloc->filename;
                    else
                        os << "(unknown source):";

                    os << "(" << alloc->line << ") : {" << alloc->bytes << " bytes}" << " function: " << alloc->function << std::endl;

                }
                os << std::endl;
            }

            if (mStdOut)
                Ncout << os.str();

            Ncout << os.str();
            Nofstream of;
            of.open(mOutFile.c_str());
            of << os.str();
            of.close();

            Nii_OutputString(os.str().c_str());
        }
    }
    //--------------------------------------------------------------------------
    MemoryTracker & MemoryTracker::getOnly()
    {
        static MemoryTracker tracker;
        return tracker;
    }
    //--------------------------------------------------------------------------
    void * MemoryTracker::allocT(void * ptr, size_t sz, Nui32 pool, const Ntchar * file, int ln, const Ntchar * func)
    {
        if (mEnable)
        {
            N_mutex1_lock

            N_assert(mAllocList.find(ptr) == mAllocList.end(), "Double allocation with same address - "
            "this probably means you have a mismatched allocation / deallocation style, "
            "check if you're are using N_alloc_t / N_free and N_new_t / N_delete_t consistently");

            mAllocList[ptr] = new TrackLog(sz, pool, file, ln, func);
            if(pool >= mPoolList.size())
                mPoolList.resize(pool+1, 0);
            mPoolList[pool] += sz;
            mAllocCount += sz;
        }
        return ptr;
    }
    //--------------------------------------------------------------------------
    void * MemoryTracker::reallocT(void * ptr, void * nptr, size_t sz, Nui32 pool, const Ntchar * file, int ln, const Ntchar * func)
    {
        if (mEnable)
        {
            N_mutex1_lock
            AllocList::iterator i = mAllocList.find(ptr);
            if(i != mAllocList.end())
            {
                mAllocCount -= i->second->bytes;
                mPoolList[pool] -= i->second->bytes;
                //i->second->bytes = sz;
                //i->second->filename = file;
                //i->second->line = ln;
                //i->second->function = func;
                //mPoolList[pool] += sz;
                //mAllocCount += sz;
                delete i->second;
                mAllocList.erase(i);
            }
            mAllocList[nptr] = new TrackLog(sz, pool, file, ln, func);
            if(pool >= mPoolList.size())
                mPoolList.resize(pool+1, 0);
            mPoolList[pool] += sz;
            mAllocCount += sz;
        }
        return nptr;
    }
    //--------------------------------------------------------------------------
    void MemoryTracker::freeT(void * ptr)
    {
        if (mEnable)
        {
            // deal cleanly with null pointers
            if (!ptr)
                return;

            N_mutex1_lock

            AllocList::iterator i = mAllocList.find(ptr);
            N_assert(i != mAllocList.end(), "Unable to locate allocation unit - "
                "this probably means you have a mismatched allocation / deallocation style, "
                "check if you're are using N_alloc_t / N_free and N_new_t / N_delete_t consistently");
            // update category stats
            mPoolList[i->second->pool] -= i->second->bytes;
            // global stats
            mAllocCount -= i->second->bytes;
            delete i->second;
            mAllocList.erase(i);
        }
    }
    //--------------------------------------------------------------------------
#endif
}