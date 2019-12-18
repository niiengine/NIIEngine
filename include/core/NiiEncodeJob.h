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

#ifndef _NII_ENCODE_JOB_H_
#define _NII_ENCODE_JOB_H_

#include "NiiPreInclude.h"
#include "NiiCodecJob.h"

namespace NII
{
    /** 解码工作
    @remark 
        高级接口,继承时必须和相应的对象和流结合在一起,否则会内存泄露,或非法指针
    @version NIIEngine 3.0.0
    */
    class _EngineAPI EncodeJob : public CodecJob
    {
        friend class CodecJob;
    public:
        /** 从内存中加码数据到目标位置
        @param[in] in 还没加码的数据
        @param[in] type 目标类型
        @param[in] any 目标位置
        */
        EncodeJob(NCount total, DataType type, const Any & any);
        virtual ~EncodeJob();
        
        /// 获取数据源
        MemDataStream * getIn();
    protected:
        EncodeJob();

        /// @copydetails CodecJob::onRequest
        void onRequest(NCount count);
    protected:
        MemDataStream * mIn;  ///< 输入流
    };
}
#endif