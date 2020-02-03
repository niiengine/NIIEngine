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
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_ResourceJob_H_
#define _NII_ResourceJob_H_

#include "NiiPreInclude.h"
#include "NiiResource.h"
#include "NiiJob.h"

namespace NII
{
    /// 枚举出请求的类型
    enum RequestType
    {
        RT_INITIALISE_GROUP = 0,        ///<
        RT_INITIALISE_ALL_GROUPS = 1,   ///<
        RT_PREPARE_GROUP = 2,           ///<
        RT_PREPARE_RESOURCE = 3,        ///<
        RT_LOAD_GROUP = 4,              ///<
        RT_LOAD_RESOURCE = 5,           ///<
        RT_UNLOAD_GROUP = 6,            ///<
        RT_UNLOAD_RESOURCE = 7          ///<
    };
    /** 资源任务
    @version NIIEngine 3.0.0
    */
    class _EngineInner ResourceJob : public Job
    {
        friend class ResourceLoadSchemeManager;
    public:
        ResourceJob(RequestType type, void * data, Nui8 retry = 0, RequestID id = 0) :
            Job(id, data, retry),
            mType(type){}

        ~ResourceJob()
        {
            if(mData != 0)
            {
                N_free(mData);
                mData = 0;
            }
        }

        /** 获取请求的类型
        @version NIIEngine 3.0.0
        */
        inline RequestType getType() const { return mType; }
    protected:
        RequestType mType;
    };
    
    class _EngineInner ResourceJobResult : public JobResult
    {
        friend class ResourceLoadSchemeManager;
    public:
        ResourceJobResult(const Job * src, void * data, bool complete,
            const String & msg = StrUtil::WBLANK) :
                JobResult(src, data, complete, msg){}
        ~ResourceJobResult()
        {
            if(mData)
            {
                ResourceResponse * temp = static_cast<ResourceResponse *>(mData);
                N_delete temp;
                temp = 0;
            }
        }
    };
}
#endif