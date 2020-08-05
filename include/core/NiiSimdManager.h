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

#ifndef _NII_SIMD_MANAGER_H_
#define _NII_SIMD_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiSimdInfo.h"
#include "NiiResourceManager.h"
#include "NiiApuProgram.h"
#include "NiiApuBuffer.h"

namespace NII
{
    /** 并行处理管理器
    @version NIIEngine 4.0.0
    */
    class _EngineAPI SimdManager : public Singleton<SimdManager>, public ResourceManager, public BufferManager,
        public CPUAlloc
    {
    public:
        enum Type
        {
            T_CPU,
            T_GPU,
            T_APU,
            T_ALL
        };
        typedef map<Nui32, MemDataStream *>::type CacheList;
    public:
        SimdManager();
        ~SimdManager();

        /** 是否支持指定语法
        @version NIIEngine 4.0.0
        */
        virtual bool isSupport(ApuLanguage sl) const;

        /** 是否支持指定语法码
        @version NIIEngine 4.0.0
        */
        ApuLanguageMark getSupportMark() const;

        /** 创建加速缓存
        @param[in] size 
        @param[in] count 数量.
        @param[in] mode 一般情况使用 Buffer::M_GPU | M_WRITE, 如果写入则应包含 M_WRITE 或 M_CPU
        @version NIIEngine 3.0.0
        */
        virtual void create(ApuBuffer *& out, NCount size, NCount count, Nmark mode);

        /** 删除加速缓存
        @version NIIEngine 3.0.0
        */
        virtual void destroy(ApuBuffer * obj);

        /** 创建加速程序
        @note 不自动加载
        @param[in] rid 资源的ID
        @param[in] gid 资源组的ID
        @param[in] file 加载的文件
        @param[in] type 程序类型
        @param[in] sl 语法名字
        */
        virtual ApuProgram * create(ResourceID rid, GroupID gid, const String & file, const VString & kernel,
            ApuLanguage sl, Nidx platidx, Type type);

        /** 创建加速程序
        @note 不自动加载
        @param[in] rid 资源标识
        @param[in] gid 资源组
        @param[in] type 程序类型
        @param[in] sl 语法名字
        @param[in] code 加速程序编码
        */
        virtual ApuProgram * create(ResourceID rid, GroupID gid, const VString & kernel, ApuLanguage sl,
            Nidx platidx, Type type, const VString & code);

        /** 创建加速程序
        @note 不自动加载
        @version NIIEngine 4.0.0
        */
        virtual ApuProgram * create(ResourceID rid, GroupID gid, const VString & kernel, ApuLanguage sl,
            Nidx platidx, Type type, ResLoadScheme * ls = 0, ResResultScheme * rs = 0);

        /** 加载加速程序
        @param[in] rid 资源ID
        @param[in] gid 资源组ID
        @param[in] file 加载的文件
        @param[in] type 程序类型
        @param[in] sl 语法名字
        */
        virtual ApuProgram * load(ResourceID rid, GroupID gid, const String & file, const VString & kernel,
            ApuLanguage sl, Nidx platidx, Type type);

        /** 加载加速程序
        @param[in] sl 语法名字
        @param[in] code 加速程序编码
        */
        virtual ApuProgram * load(ResourceID rid, GroupID gid, const VString & kernel, ApuLanguage sl, 
            Nidx platidx, Type type, const VString & code);

        ///@copydetails ResourceManager::get
        virtual Resource * get(ResourceID rid, GroupID gid = GroupUnknow);

        /** 设置是否自动缓存加速程序的二进制
        @remark 下次再用的时候会快点
        @version NIIEngine 4.0.0
        */
        void setCacheBinary(bool b);

        /** 获取是否自动缓存加速程序的二进制
        @remark 下次再用的时候会快点
        @version NIIEngine 4.0.0
        */
        bool isCacheBinary();

        /** 导出到流
        @version NIIEngine 4.0.0
        */
        void exportCache(DataStream * out) const;

        /** 从流导入
        @version NIIEngine 4.0.0
        */
        void inportCache(const DataStream * in);

        /** 创建加速程序的二进制缓存
        @version NIIEngine 4.0.0
        */
        MemDataStream * createCache(Nui32 cid, NCount size);

        /** 删除加速程序的二进制缓存
        @version NIIEngine 4.0.0
        */
        void destroyCache(Nui32 cid);

        /** 加速存在缓存是否存在
        @version NIIEngine 4.0.0
        */
        bool isCacheExist(Nui32 cid) const;

        /** 获取加速程序的二进制缓存
        @version NIIEngine 4.0.0
        */
        MemDataStream * getCache(Nui32 cid) const;

        /** 获取平台列表
        @version NIIEngine 4.0.0
        */
        inline const ApuInfoList & getPlatformList() const { return mApuInfoList; }
    public:
        /** 对象已经被删除
        @version NIIEngine 3.0.0
        */
        void onDestroy(ApuBuffer * obj);
    protected:
        /// @copydetails ResourceManager::createImpl
        Resource * createImpl(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs,
            const PropertyData * params);

        virtual Resource * createImpl(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs, 
            const VString & kernel, ApuLanguage sl, Nidx platidx, Type type);
    protected:
        ApuInfoList mApuInfoList;
        CacheList mCacheList;
        Nmark mCacheMark;
    };
}
#endif