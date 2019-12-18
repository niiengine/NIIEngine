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

#ifndef _NII_Billboard_Serializer_H_
#define _NII_Billboard_Serializer_H_

#include "NiiPreInclude.h"
#include "NiiFileSerializer.h"
#include "NiiString.h"

namespace NII
{
    class _EngineAPI ParticleCtrlSerializer : public FileSerializer
    {
    public:
        ParticleCtrlSerializer();
        virtual ~ParticleCtrlSerializer();

        /** 导出粒子控制到指定文件
        @param[in] in
        @param[in] file
        @param[in] emode
        */
        void write(const ParticleCtrl * in, const String & file, SerialVer ver,
            SerialFormat sf, Endian emode = E_Native);

        /** 导出粒子控制到指定文件
        @param[in] in
        @param[in] stream
        @param[in] emode
        */
        void write(const ParticleCtrl * in, DataStream * stream, SerialVer ver,
            SerialFormat sf, Endian emode = E_Native);

        /** 从一个.particlectrl文件DataStream,导近粒子控制数据
        @remark
        @param[in] stream
        @param[in] out
        */
        void read(DataStream * stream, ParticleCtrl * out);
    protected:
        /// @copydetails Serializer::read
        void readUnit(void * out);

        /// @copydetails Serializer::write
        void writeUnit(const void * in, SerialVer ver = 30000,
            SerialFormat sf = F_EC_None, Endian emode = E_Native);
    };
}
#endif