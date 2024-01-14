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

#ifndef _NII_GROUP_LOAD_SCHEME_H_
#define _NII_GROUP_LOAD_SCHEME_H_

#include "NiiPreInclude.h"
#include "NiiLoadScheme.h"

namespace NII
{
    /** 资源组加载策略
    @remark
        使用资源组加载策略可以更有效的控制硬件系统实际性能,协调资源在硬件系统中占用
        的内存和程序加载时间
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI GroupLoadScheme : public LoadScheme
    {
    public:
        /** 构造函数
        @remark
        @param[in] wl 总工作量
        @version NIIEngine 3.0.0 高级api
        */
        GroupLoadScheme(SchemeID sid, GroupID gid, NCount wl);
        virtual ~GroupLoadScheme();

        virtual bool request(LoadingState ls);

        /** 这个事件当一个资源组开始解析脚本时引发
        @remark
        @param[in] gid 资源组ID
        @param[in] count 工作数量
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onPreScriptParse(GroupID gid, NCount count);

        /** 当解析属于此资源组的脚本前触发
        @remark 此文件不一定来自本地文件中
        @param[in] file 文件名字
        @return 是否成功,不成功或许要找备用方案
        @version NIIEngine 3.0.0 高级api
        */
        virtual bool onPreScriptParse(const String & file);

        /// 这个事件当一个资源组结束解析脚本时引发
        virtual void onScriptParseEnd(GroupID gid);

        /** 当解析属于此资源组的脚本后触发
        @remark 此文件不一定来自本地文件中
        @param[in] file 文件名字
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onScriptParseEnd(const String & file);

        /** 这个事件当一个资源组开始准备时引发
        @param[in] gid 资源组ID
        @param[in] count
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onPrePlan(GroupID gid, NCount count);

        /** 这个事件当一个资源组即将准备完时引发
        @param[in] r
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onPrePlan(const Resource * r);

        /** 这个事件当一个资源组结束加载时引发
        @param[in] gid
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onPlanEnd(GroupID gid);

        /** 这个事件当一个资源组已经准备完时引发
        @param[in] r
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onPlanEnd(const Resource * r);

        /** 这个事件当一个资源组开始加载时引发
        @param[in] gid 资源组ID
        @param[in] count
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onPreLoad(GroupID gid, NCount count);

        /** 这个事件当一个声明资源即将完成加载时引发
        @param[in] r
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onPreLoad(const Resource * r);

        /** 这个事件当一个声明资源已经完成加载时引发
        @param[in] r
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onLoadEnd(const Resource * r);

        /** 这个事件当一个资源组完成加载时引发
        @param[in] gid
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onLoadEnd(GroupID gid);
    protected:
        GroupID mGID;           ///< 对应的资源组
        NCount mWorkloadFactor; ///< 总工作量因子
    };
}
#endif