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

#ifndef _NII_DECODE_JOB_H_
#define _NII_DECODE_JOB_H_

#include "NiiPreInclude.h"
#include "NiiCodecJob.h"

namespace NII
{
    /** 加码工作
    @remark 
        高级接口,继承时必须和相应的对象和流结合在一起,否则会内存泄露,或非法指针
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DecodeJob : public CodecJob
    {
        friend class CodecJob;
    public:
        DecodeJob(NCount total, DataType type, const Any & any);
        virtual ~DecodeJob();

        /// 获取解码后的数据
        MemDataStream * getOut() const;
    protected:
        DecodeJob();

        /// @copydetails CodecJob::onRequest
        void onRequest(NCount count);
    protected:
        MemDataStream * mOut; ///< 输出流
    };
}

#endif