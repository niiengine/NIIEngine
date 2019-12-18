/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_MemoryTracker_H_
#define _NII_MemoryTracker_H_

#if NII_MEMORY_TRACKER

#if N_COMPILER == N_CPP_GNUC && NII_COMP_VER >= 310 && !defined(STLPORT)
    // For gcc 4.3 see http://gcc.gnu.org/gcc-4.3/changes.html
    #if NII_COMP_VER >= 430
        #include <tr1/unordered_map>
    #else
        #include <ext/hash_map>
namespace __gnu_cxx
{
    template <> struct hash<void *>
    {
        size_t operator()(void * const & ptr) const
        {
            return (size_t)ptr;
        }
    };
}
    #endif
#endif

namespace NII
{
	class TrackLog;
    /** 并能统计报告内存使用状态
	@version NIIEngine 3.0.0
    */
    class _EngineAPI MemoryTracker
    {
    protected:
        MemoryTracker();
    public:
        ~MemoryTracker();

		/** 设置是否记录
		@version NIIEngine 3.0.0
		*/
		void setEnable(bool set);

		/** 是否开启记录
		@version NIIEngine 3.0.0
		*/
		bool getEnable() const;

        /** 记录分配
        @param[in] ptr 内存指针
        @param[in] sz 内存大小 单位字节
        @param[in] pool 这个内存池分配发生于
		@version NIIEngine 3.0
        */
        void _recordAlloc(void * ptr, size_t sz, Nui32 pool = 0,
            const char * file = 0, int ln = 0, const char * func = 0);

        /** 记录重新分配
        @param[in] ptr 需要重新分配的内存指针
        @param[in] sz 内存大小 单位字节
        @param[in] pool 所在内存内存池
		@version NIIEngine 3.0
        */
        void _recordRealloc(void * ptr, size_t sz, Nui32 pool = 0,
            const char * file = 0, int ln = 0, const char * func = 0);

        /** 记录内存释放
		@version NIIEngine 3.0
		*/
        void _recordDealloc(void * ptr);

        /** 设置报告文件名字
		@version NIIEngine 3.0
		*/
        void setFile(const String & name);

        /** 获取报告的文件名字
		@version NIIEngine 3.0
		*/
        const String & getFile() const;

        /* 设置是否输出到 std::out
		@version NIIEngine 3.0.0
		*/
        void setStdOut(bool set);

        /** 获取是否输出到 std::out
		@version NIIEngine 3.0.0
		*/
        bool getStdOut() const;

        /** 获取当前分配内存的总数量
		@version NIIEngine 3.0.0
		*/
        NCount getTotal() const;

        /** 获取一个给定池的分配的内存总数
        @version NIIEngine 3.0.0
		*/
		NCount getTotal(Nui32 pool) const;

        static MemoryTracker & getOnly();
    protected:
        typedef HashMap<void *, TrackLog *> AllocList;
        typedef std::vector<size_t> PoolCountList;
	protected:
		N_mutex1
        String mOutFile;
        AllocList mAllocList;
        PoolCountList mAllocPoolList;
		size_t mAllocCount;
        bool mRecordEnable;
		bool mStdOutEnable;
    };
}
#endif
#endif