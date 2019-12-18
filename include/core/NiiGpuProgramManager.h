/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-8

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

#ifndef _NII_GpuProgram_Manager_H_
#define _NII_GpuProgram_Manager_H_

#include "NiiPreInclude.h"
#include "NiiResourceManager.h"
#include "NiiGpuProgram.h"
#include "NiiGpuPreDefineValue.h"

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
        typedef map<Nui32, MemDataStream *>::type CacheList;
        typedef map<String, GpuPreDefineValue *>::type ShareCustomList;
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
        virtual GpuProgram * create(ResourceID rid, GroupID gid,
            const String & file, GpuProgram::ShaderType type, ShaderLanguage sl);

        /** 创建着色程序
        @note 不自动加载
        @param[in] rid 资源标识
        @param[in] gid 资源组
        @param[in] type 程序类型
        @param[in] sl 语法名字
        @param[in] code 着色程序编码
        */
        virtual GpuProgram * create(ResourceID rid, GroupID gid,
            GpuProgram::ShaderType type, ShaderLanguage sl, const VString & code);

        /** 创建着色程序
        @note 不自动加载
        @version NIIEngine 3.0.0
        */
        virtual GpuProgram * create(ResourceID rid, GroupID gid, GpuProgram::ShaderType type,
            ShaderLanguage sl, ResourceLoadScheme * ls = 0, ResourceResultScheme * rs = 0);

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
        GpuPreDefineValue * createShare(const String & name);

        /** 移去公共参数
        @version NIIEngine 3.0.0
        */
        void destroyShare(const String & name);

        /** 获取公共列表
        @version NIIEngine 3.0.0
        */
        GpuPreDefineValue * getShare(const String & name) const;

        /** 获取公共列表
        @version NIIEngine 3.0.0
        */
        const ShareCustomList & getShareList() const;

        /** 设置是否自动缓存着色程序的二进制
        @remark 下次再用的时候会快点
        @version NIIEngine 3.0.0
        */
        void setCacheBinary(bool b);

        /** 获取是否自动缓存着色程序的二进制
        @remark 下次再用的时候会快点
        @version NIIEngine 3.0.0
        */
        bool isCacheBinary();

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
        MemDataStream * createCache(Nui32 cid, NCount size);

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
        MemDataStream * getCache(Nui32 cid) const;

        /** 获取自动同步参数
        @param[in] name 同步参数名字
        @version NIIEngine 3.0.0
        */
        static const GpuSyncParamDef * getGpuSyncParam(const String & name);

        /** 获取自动同步参数
        @param[in] idx 同步参数下标
        @version NIIEngine 3.0.0
        */
        static const GpuSyncParamDef * getGpuSyncParam(Nidx idx);

        /// @copydetails Singleton::getOnly
        static GpuProgramManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static GpuProgramManager * getOnlyPtr();
    protected:
        /// @copydetails ResourceManager::createImpl
        Resource * createImpl(ResourceID rid, GroupID gid, ResourceLoadScheme * ls, ResourceResultScheme * rs,
            const PropertyData * params);

        /// 指定参数的专用创建方法
        virtual Resource * createImpl(ResourceID rid, GroupID gid, ResourceLoadScheme * ls, ResourceResultScheme * rs,
            GpuProgram::ShaderType type, ShaderLanguage sl);
    protected:
        static GpuSyncParamDef mGpuSyncParamList[];
        ShareCustomList mShareCustomList;
        CacheList mCacheList;
        Nmark mCacheMark;
    };
}
#endif