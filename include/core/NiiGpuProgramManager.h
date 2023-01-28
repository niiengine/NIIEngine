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

#ifndef _NII_GpuProgram_Manager_H_
#define _NII_GpuProgram_Manager_H_

#include "NiiPreInclude.h"
#include "NiiResourceManager.h"
#include "NiiGpuProgram.h"

namespace NII
{
    /** 着色程序管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuProgramManager : public ResourceManager, public Singleton<GpuProgramManager>
    {
        friend class Engine;
        friend class RenderSys;
    public:
        typedef map<Nui32, MemStream *>::type CacheList;
        typedef map<String, GpuParamValue *>::type ShareCustomList;
    public:
        GpuProgramManager();
        virtual ~GpuProgramManager();

        /** 是否支持指定语法
        @version NIIEngine 3.0.0
        */
        virtual bool isSupport(ShaderLanguage sl) const;

        /** 是否支持指定语法码
        @version NIIEngine 3.0.0
        */
        ShaderLanguageMark getSupportMark() const;

        /** 创建着色程序
        @note 不自动加载
        @param[in] rid 资源的ID
        @param[in] gid 资源组的ID
        @param[in] file 加载的文件
        @param[in] type 程序类型
        @param[in] sl 语法名字
        */
        virtual GpuProgram * create(ResourceID rid, GroupID gid, const String & file, GpuProgram::ShaderType type, ShaderLanguage sl);

        /** 创建着色程序
        @note 不自动加载
        @param[in] rid 资源标识
        @param[in] gid 资源组
        @param[in] type 程序类型
        @param[in] sl 语法名字
        @param[in] code 着色程序编码
        */
        virtual GpuProgram * create(ResourceID rid, GroupID gid, GpuProgram::ShaderType type, ShaderLanguage sl, const VString & code);

        /** 创建着色程序
        @note 不自动加载
        @version NIIEngine 3.0.0
        */
        virtual GpuProgram * create(ResourceID rid, GroupID gid, GpuProgram::ShaderType type,
            ShaderLanguage sl, ResLoadScheme * ls = 0, ResResultScheme * rs = 0);

        /** 加载着色程序
        @param[in] rid 资源ID
        @param[in] gid 资源组ID
        @param[in] file 加载的文件
        @param[in] type 程序类型
        @param[in] sl 语法名字
        */
        virtual GpuProgram * load(ResourceID rid, GroupID gid, const String & file,
            GpuProgram::ShaderType type, ShaderLanguage sl);

        /** 加载着色程序
        @param[in] sl 语法名字
        @param[in] code 着色程序编码
        */
        virtual GpuProgram * load(ResourceID rid, GroupID gid, GpuProgram::ShaderType type,
            ShaderLanguage sl, const VString & code);

        ///@copydetails GpuProgramManager::get
        virtual Resource * get(ResourceID rid, GroupID gid = GroupUnknow);

        /** 创建公共参数
        @version NIIEngine 3.0.0
        */
        GpuParamValue * createShare(const String & name);

        /** 移去公共参数
        @version NIIEngine 3.0.0
        */
        void destroyShare(const String & name);

        /** 获取公共列表
        @version NIIEngine 3.0.0
        */
        GpuParamValue * getShare(const String & name) const;

        /** 获取公共列表
        @version NIIEngine 3.0.0
        */
        inline const ShareCustomList & getShareList() const{ return mShareCustomList; }

        /** 设置是否自动缓存着色程序的二进制
        @remark 下次再用的时候会快点
        @version NIIEngine 3.0.0
        */
        void setCacheBinary(bool b);

        /** 获取是否自动缓存着色程序的二进制
        @remark 下次再用的时候会快点
        @version NIIEngine 3.0.0
        */
        bool isCacheBinary() const;

        /** 设置是否自动缓存着色程序的二进制到文件中
        @remark 下次再用的时候会快点
        @version NIIEngine 4.0.0
        */
        void setCacheBinaryFile(bool b);

        /** 获取是否自动缓存着色程序的二进制到文件中
        @remark 下次再用的时候会快点
        @version NIIEngine 4.0.0
        */
        bool isCacheBinaryFile() const;

        /** 设置是否自动缓存着色程序的二进制的文件目录
        @version NIIEngine 4.0.0
        */
        inline void setCachePath(const String & path)   { mCachePath = path; }

        /** 获取是否自动缓存着色程序的二进制的文件目录
        @version NIIEngine 3.0.0
        */
        inline const String & getCachePath() const      { return mCachePath; }

        /** 导出到流
        @version NIIEngine 3.0.0
        */
        void exportCache(DataStream * out) const;

        /** 从流导入
        @version NIIEngine 3.0.0
        */
        void inportCache(const DataStream * in);

        /** 创建着色程序的二进制缓存
        @version NIIEngine 3.0.0
        */
        MemStream * createCache(Nui32 cid, NCount size);

        /** 删除着色程序的二进制缓存
        @version NIIEngine 3.0.0
        */
        void destroyCache(Nui32 cid);

        /** 着色存在缓存是否存在
        @version NIIEngine 3.0.0
        */
        bool isCacheExist(Nui32 cid) const;

        /** 获取着色程序的二进制缓存
        @version NIIEngine 3.0.0
        */
        MemStream * getCache(Nui32 cid) const;
        
        /** 调试着色程序
        @version NIIEngine 5.0.0
        */
        void setDebugShaders(bool set)                  { mDebug = set;}
        
        /** 调试着色程序
        @version NIIEngine 5.0.0
        */
        bool getDebugShaders() const                    { return mDebug; }
        
        /** 获取语法标记
        @version NIIEngine 6.0.0
        */
        virtual const char * getToken(PixelFormat pf) const;

        /** 获取语法标记
        @version NIIEngine 6.0.0
        */
        virtual const char * getToken(PixelFormat pf, Texture::Type type, bool msaa, Buffer::OpType access) const;

        /** 获取自动同步参数
        @param[in] name 同步参数名字
        @version NIIEngine 3.0.0
        */
        static const GpuEnvParamUnit * getGpuSyncParam(const String & name);

        /** 获取自动同步参数
        @param[in] idx 同步参数下标
        @version NIIEngine 3.0.0
        */
        static const GpuEnvParamUnit * getGpuSyncParam(Nidx idx);
    protected:
        /// @copydetails ResourceManager::createImpl
        Resource * createImpl(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs,
            const PropertyData * params);

        /// 指定参数的专用创建方法
        virtual Resource * createImpl(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs,
            GpuProgram::ShaderType type, ShaderLanguage sl);
    protected:
        static GpuEnvParamUnit mGpuSyncParamList[];
        ShareCustomList mShareCustomList;
        CacheList mCacheList;
        String mCachePath;
        Nmark mCacheMark;
        bool mDebug;
    };
    
    /** 高层着色程序管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI HighLevelGpuProgramManager : public Singleton<HighLevelGpuProgramManager>, 
        public ResourceManager
    {
    public:
        HighLevelGpuProgramManager();
        ~HighLevelGpuProgramManager();

        /** 是否支持指定语法
        @version NIIEngine 3.0.0
        */
        bool isSupport(ShaderLanguage lang);
        
        /** 添加工厂类
        @version NIIEngine 3.0.0
        */
        void add(HighLevelGpuProgramFactory * factory);

        /** 移去工厂类
        @version NIIEngine 3.0.0
        */
        void remove(HighLevelGpuProgramFactory * factory);

        /** 创建高层着色程序
        @note 不加载
        @param[in] rid 标识着色程序的名字
        @param[in] gid 着色程序的资源组名字
        @param[in] lang 代码语言
        @param[in] type 创建的着色程序的类型
        @version NIIEngine 3.0.0
        */
        HighLevelGpuProgram * create(ResourceID rid, GroupID gid, ShaderLanguage lang, GpuProgram::ShaderType type);
        
        virtual const char* getToken( PixelFormat pf ) const = 0;
        
        virtual const char* getToken( PixelFormat pf, Texture::Type textureType, bool isMsaa, GpuParamBufferUnit::OpType access ) const = 0;
    protected:
        /// @copydetails ResourceManager::init
        void init();

        /** 获取
        @version NIIEngine 3.0.0 内部api
        */
        HighLevelGpuProgramFactory * getFactory(ShaderLanguage lang);

        /// @copydetails ResourceManager::createImpl
        Resource * createImpl(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs, const PropertyData * params);
    protected:
        typedef map<ShaderLanguage, HighLevelGpuProgramFactory *>::type FactoryMap;
    protected:
        FactoryMap mFactList;
    };
}
#endif