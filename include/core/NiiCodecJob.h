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