/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-5-7

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

#ifndef NII_AUDIOBUFFER_H
#define NII_AUDIOBUFFER_H

#include "NiiPreInclude.h"

namespace NII
{
    /**
    @version NIIEngine 4.0.0
    */
    class AudioBuffer : public Buffer
    {
        friend class 
    public:
        enum ExtMode
        {
            EM_STREAM       = M_EXT1,
            EM_ADPCM        = M_EXT2,
            EM_STEREO       = M_EXT3,
            EM_MONO         = M_EXT4,
            EM_8BIT         = M_EXT5,
            EM_16BIT        = M_EXT6
        };
        AudioBuffer(BufferManager * mag, Nmark mode);
        virtual ~AudioBuffer() {}

        /** 
        @version NIIEngine 4.0.0 adv
        */
        inline void setID(BufferID bid){ mID = bid; }
        
        /** 
        @version NIIEngine 4.0.0
        */
        inline BufferID getID() const{ return mID; }
        
        /**
        @version NIIEngine 4.0.0
        */
        inline bool isStreamMode() const;

        /** 
        @version NIIEngine 4.0.0
        */
        virtual bool isStereo() const = 0;
        
        /** 
        @version NIIEngine 4.0.0
        */
        virtual void setFormat(SampleFormat format) = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual void setRate(NCount rate) = 0;
        
        /** 
        @version NIIEngine 4.0.0
        */
        virtual SampleFormat getFormat() const = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual NCount getRate() const = 0;
        
        /**
        @version NIIEngine 4.0.0
        */
        virtual TimeDurMS getDuration() const = 0;
        
        /** 
        @version NIIEngine 4.0.0
        */
        virtual bool isValid() = 0;

        /**
        @version NIIEngine 4.0.0
        */
        static void encodeADPCM(Ni16 * in, Nui8 * out, NCount count);

        /**
        @version NIIEngine 4.0.0
        */
        static void decodeADPCM(Nui8 * in, Ni16 * out, NCount count);
    protected:
        AudioBuffer(){}

        /// @copydetails Buffer::lockImpl
        void * lockImpl(Nmark m, NCount oft, NCount size);
    private:
        BufferID mID;
        Nmark mOpMark;
    };
}
#endif