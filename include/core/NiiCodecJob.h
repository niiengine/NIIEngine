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

#ifndef _NII_CODEC_JOB_H_
#define _NII_CODEC_JOB_H_

#include "NiiPreInclude.h"
#include "NiiJob.h"
#include "NiiAny.h"
#include "NiiDataType.h"

namespace NII
{
    /** 解码/压码 工作
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CodecJob : public Job
    {
        friend Codec;
    public:
        enum DataType
        {
            /** 无操作,未初始化
            @par 没有数据格式
            @version NIIEngine 3.0.0
            */
            DT_None = 0,
            /** 写入到文件
            @par String数据格式
            @version NIIEngine 3.0.0
            */
            DT_File = 0x01,
            /** 写入到内存
            @par MemDataStream ** 数据格式
            @version NIIEngine 3.0.0
            */
            DT_Memory = 0x02,
            /** 写入到设备
            @version NIIEngine 3.0.0
            */
            DT_Dev = 0x08,
            /** 上传到网络中
            @par String数据格式
            @version NIIEngine 3.0.0
            */
            DT_Net = 0x10
        };
    public:
        CodecJob(NCount total, DataType type, const Any & any);
        virtual ~CodecJob();

        /** 设置加码的目标
        @remark 注意设置 setDestType 类型相关数据
        */
        void setData(const Any & data);

        /** 获取加码的目标
        @remark 注意设置 setDestType 类型相关数据
        */
        const Any & getData() const;

        /** 设置加码的目标类型
        @remark 注意设置 setDest 类型相关数据
        */
        void setDataType(DataType type);

        /** 获取加码的目标类型
        @remark 注意设置 setDest 类型相关数据
        */
        DataType getDataType() const;

        /** 执行当前请求
        @remark
        */
        virtual void go(Nui32 group) = 0;

        /// @copydetails Job::run
        bool run();
    protected:
        /** 请求事件发生时
        @param[in] count
        */
        void onRequest(NCount count);
    protected:
        NCount mTotal;                  ///< 任务总数量
        NCount mComplete;               ///< 已经完成度
        DataType mDataType;             ///< 目标类型        
        Any mData;                      ///< 目标位置
	};
}
#endif