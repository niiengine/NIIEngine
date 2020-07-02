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

#ifndef _NII_NET_SERIALIZER_H_
#define _NII_NET_SERIALIZER_H_

#include "NiiNetPreInclude.h"
#include "NiiSerializer.h"

namespace NII
{
    /** 网络序列器
    @remark
        和普通文件序列器不同的是,这个类支持位写入和输出操作,正因为网络资源宝贵,所以
        是使用以位作为单位去描述布尔和不规则非8倍位数数据.还有一个方法就是收集8个位
        级别的操作放到同一个字节中,这样保证效率的同时,也节省了很多网络资源.而且还有
        一点就是翻转字节序并不是一个效率高的东西
    @par
        有点需要注意的是,序列是依照写入顺序的,所以写入的顺序是怎么样,读取的顺序就是
        怎么样
    @par
        由于memcpy等c函数操作是以byte为基数(可能还能并行计算),所以这个网络序列相对
        与其他普通序列来讲会消耗更多的性能
    @version NIIEngine 3.0.0
    */
    class _EngineAPI NetSerializer : public Serializer
    {
    public:
        NetSerializer();
        NetSerializer(NCount size);
        NetSerializer(const Nui8 * data, NCount size, bool copy);
        ~NetSerializer();

        /**
        @param[in] in
        @vesion NIIEngine 3.0.0
        */
        void write(const NetSerializer * in);

        /**
        @param[in] in
        @param[in] count
        @vesion NIIEngine 3.0.0
        */
        void write(const NetSerializer * in, NBitCount count);

        /** 写入一个位
        @remark 仅占一个位,要用read(bool &)
        @version NIIEngine 3.0.0
        */
        void write0();

        /** 写入一个位
        @remark 仅占一个位,要用read(bool &)
        @version NIIEngine 3.0.0
        */
        void write1();

        /** 写入一个字节
        @remark 速度比基类的写入快一点点
        @version NIIEngine 3.0.0
        */
        void write(Nui8 in);

        /** 写入二个字节
        @remark 速度比基类的写入快一点点
        @param[in] in
        */
        void write(Nui16 in);

        /** 写入四个字节
        @remark 速度比基类的写入快一点点
        @param[in] in
        @version NIIEngine 3.0.0
        */
        void write(Nui32 in);

        /** 写入一个浮点
        @remark 精度削一半
        @version NIIEngine 3.0.0
        */
        void writeCompress(NIIf in);

        /**
        @remark 精度削一半
        @version NIIEngine 3.0.0
        */
        void writeCompress(NIId in);

        /**
        @version NIIEngine 3.0.0
        */
        void writeAlign();

        /**
        @version NIIEngine 3.0.0
        */
        void writeAlign(NCount count);

        /**
        @param[in] in
        @param[in] count
        @param[in] Ralign
        @version NIIEngine 3.0.0
        */
        void writeBit(const Nui8 * in, NBitCount count, bool Ralign = true);

        /**
        @param[in] in 比特位数量
        @param[in] count
        @param[in] flip
        @version NIIEngine 3.0.0
        */
        void writeCompress(const Nui8 * in, NBitCount count, bool flip = true);

        /// @copydetails Serializer::write
        NCount write(const void * in, NCount size, NCount count, bool flip = true);

        /**
        @vesion NIIEngine 3.0.0
        */
        NBitCount read(NetSerializer * out) const;

        /**
        @vesion NIIEngine 3.0.0
        */
        NBitCount read(NetSerializer * out, NBitCount count) const;

        /**
        @remark 速度比基类的读取快一点点
        @version NIIEngine 3.0.0
        */
        NBitCount read(bool & out) const;

        /**
        @remark 速度比基类的读取快一点点
        @version NIIEngine 3.0.0
        */
        NCount read(Nui8 & out) const;

        /**
        @remark 速度比基类的读取快一点点
        @version NIIEngine 3.0.0
        */
        NCount read(Nui16 & out) const;

        /**
        @remark 速度比基类的读取快一点点
        @param[in] in
        @version NIIEngine 3.0.0
        */
        NCount read(Nui32 & out) const;

        /**
        @remark 精度削一半
        @version NIIEngine 3.0.0
        */
        NCount readCompress(NIIf & out) const;

        /**
        @remark 精度削一半
        @version NIIEngine 3.0.0
        */
        NCount readCompress(NIId & out) const;

        /**
        @version NIIEngine 3.0.0
        */
        NBitCount readAlign() const;

        /**
        @version NIIEngine 3.0.0
        */
        NBitCount readAlign(NCount count) const;

        /**
        @param[out] out
        @param[in] count
        @param[in] Ralign
        @version NIIEngine 3.0.0
        */
        NBitCount readBit(Nui8 * out, NBitCount count, bool Ralign = true) const;

        /** 写入一个压缩
        @param[in] out
        @param[in] count 比特位数量
        @param[in] flip
        @version NIIEngine 3.0.0
        */
        NBitCount readCompress(Nui8 * out, NBitCount count, bool flip = true) const;

        /// @copydetails Serializer::read
        NCount read(void * out, NCount size, NCount count, bool flip = true) const;

        /**
        @param[in] count
        @version NIIEngine 3.0.0
        */
        void seekWrite(NBitCount count);

        /**
        @param[in] count
        @version NIIEngine 3.0.0
        */
        void skipWrite(NBitCount count);

        /**
        @param[in] count
        @version NIIEngine 3.0.0
        */
        void seekRead(NBitCount count);

        /**
        @param[in] count
        @version NIIEngine 3.0.0
        */
        void skipRead(NBitCount count);

        /**
        @param[in] count
        @version NIIEngine 3.0.0
        */

        void enlargeByte(NCount count);

        /**
        @param[in] count
        @version NIIEngine 3.0.0
        */
        void enlargeBit(NBitCount count);

        /** 获取高位的第一字节
        @remark 最右边开始
        @param[in] data
        @param[in] byteskip
        @version NIIEngine 3.0.0
        */
        bool get1Hbit(const Nui8 * data, NCount byteskip = 0);

        /** 获取高位的第二字节
        @remark 最右边开始
        @param[in] data
        @param[in] byteskip
        @version NIIEngine 3.0.0
        */
        bool get2Hbit(const Nui8 * data, NCount byteskip = 0);

        /** 获取高位的第三字节
        @remark 最右边开始
        @param[in] data
        @param[in] byteskip
        @version NIIEngine 3.0.0
        */
        bool get3Hbit(const Nui8 * data, NCount byteskip = 0);

        /** 获取高位的第四字节
        @remark 最右边开始
        @param[in] data
        @param[in] byteskip
        @version NIIEngine 3.0.0
        */
        bool get4Hbit(const Nui8 * data, NCount byteskip = 0);

        /** 获取低位的第1字节
        @remark 最左边开始
        @param[in] data
        @param[in] byteskip
        */
        bool get1Lbit(const Nui8 * data, NCount byteskip = 0);

        /** 获取低位的第2字节
        @remark 最左边开始
        @param[in] data
        @param[in] byteskip
        */
        bool get2Lbit(const Nui8 * data, NCount byteskip = 0);

        /** 获取低位的第3字节
        @remark 最左边开始
        @param[in] data
        @param[in] byteskip
        */
        bool get3Lbit(const Nui8 * data, NCount byteskip = 0);

        /** 获取低位的第4字节
        @remark 最左边开始
        @param[in] data
        @param[in] byteskip
        */
        bool get4Lbit(const Nui8 * data, NCount byteskip = 0);

        /** 数据有效的数据位
        @version NIIEngine 3.0.0
        */
        NBitCount getWriteOffset();

        /** 当前读取的数据位
        @version NIIEngine 3.0.0
        */
        NBitCount getReadOffset();

        /** 实际占用的字节数
        @version NIIEngine 3.0.0
        */
        NCount getSize() const;

        /**
        @version NIIEngine 3.0.0
        */
        NBitCount getRemainCount() const;

        /**
        @version NIIEngine 3.0.0
        */
        const Nui8 * getData() const;

        /** 是否翻转数据(数据的字节序)
        @remark 网络设备存在一个定性,所以在同个机器上可能存在同样的结果
        @version NIIEngine 3.0.0
        */
        static bool isFlipData();

        /** 是否是网络字节序(数据的字节序)
        @remark 网络设备存在一个定性,所以在同个机器上可能存在同样的结果
        @version NIIEngine 3.0.0
        */
        static bool isNetEndian();
    protected:
        ///
        static const Nui8 ZeroOMark[8];

        ///
        static const Nui8 ZeroIMark[8];

        ///
        static const Nui8 HMark[8];

        ///
        static const Nui8 LMark[8];
    private:
        NBitCount mBitWrite;
        NBitCount mBitAlloc;
        mutable NBitCount mBitRead;
        Nui8 * mData;
        bool mAutoData;
    };
}
#endif