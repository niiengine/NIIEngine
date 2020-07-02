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

#ifndef _NII_MemoryTracker_H_
#define _NII_MemoryTracker_H_

#if N_MEMTRACK

namespace NII
{
    class TrackLog;
    /** 并能统计报告内存使用状态
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MemoryTracker
    {
    public:
        ~MemoryTracker();

        /** 记录分配
        @param[in] ptr 内存指针
        @param[in] sz 内存大小 单位字节
        @param[in] pool 所在内存池
        @version NIIEngine 3.0
        */
        void * allocT(void * ptr, size_t sz, Nui32 pool = 0,
            const Ntchar * file = 0, int ln = 0, const Ntchar * func = 0);

        /** 记录重新分配
        @param[in] ptr 需要重新分配的内存指针
        @param[in] sz 内存大小 单位字节
        @param[in] pool 所在内存池
        @version NIIEngine 3.0
        */
        void * reallocT(void * ptr, void * nptr, size_t sz, Nui32 pool = 0,
            const Ntchar * file = 0, int ln = 0, const Ntchar * func = 0);

        /** 记录内存释放
        @version NIIEngine 3.0
        */
        void freeT(void * ptr);

        /** 设置是否记录
        @version NIIEngine 3.0.0
        */
        inline void setEnable(bool set) { mEnable = set;  }

        /** 是否开启记录
        @version NIIEngine 3.0.0
        */
        inline bool isEnable() const { return mEnable;  }

        /** 设置报告文件名字
        @version NIIEngine 3.0
        */
        inline void setFile(const String & name) { mOutFile = name;  }

        /** 获取报告的文件名字
        @version NIIEngine 3.0
        */
        inline const String & getFile() const { return mOutFile;  }

        /** 设置是否输出到 std::out
        @version NIIEngine 3.0.0
        */
        inline void setStdOut(bool set) { mStdOut = set; }

        /** 获取是否输出到 std::out
        @version NIIEngine 3.0.0
        */
        inline bool isStdOut() const { return mStdOut;  }

        /** 获取分配内存的总数量
        @version NIIEngine 3.0.0
        */
        inline NCount getTotal() const { return mAllocCount; }

        /** 获取指定池的分配的内存总数
        @version NIIEngine 3.0.0
        */
        inline NCount getTotal(Nui32 pool) const { return mPoolList[pool];  }

        static MemoryTracker & getOnly();
    protected:
        MemoryTracker();
        typedef HashMap<void *, TrackLog *> AllocList;
        typedef std::vector<size_t> PoolCountList;
    protected:
        N_mutex1
        String mOutFile;
        AllocList mAllocList;
        PoolCountList mPoolList;
        size_t mAllocCount;
        bool mEnable;
        bool mStdOut;
    };
}
#endif
#endif