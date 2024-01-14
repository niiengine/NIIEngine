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

#ifndef _NII_StreamSerialiser_H_
#define _NII_StreamSerialiser_H_

#include "NiiPreInclude.h"
#include "NiiFileSerializer.h"

namespace NII
{
    /** 读/写辅助文件序列
    @remark
        该结构的文件读/写的这个类是一系列的'块'.一大块的格式对于数据扩展有优势。
    @par
        一个块的数据格式如下:
        1.块编号(Nui32).内容里是唯一的.
        2.块版本(Nui16).块可以随版本号的变化而随时间改变
        3.长度(Nui32).该块数据的长度,包括嵌套块.不包括这个本块头.
        4.校验(Nui32).校验值产生上述基本资料- 让我们检查这是一个有效的块.
        5 块数据，在块数据的部分将包含块的数据,其中还可能包括其他嵌套块.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FileStreamSerializer : public FileSerializer
    {
    public:
        /** 块数据定义
        @version NIIEngine 3.0.0
        */
        struct DataChunk : public StreamAlloc
        {
            Nui32 id;       ///< 块的标识符
            Nui16 version;  ///< 块的版本
            Nui32 length;   ///< 块数据的长度不包括此块头(单位字节)
            Nui32 offset;   ///< 从流开始到块(头)的位置(单位字节)

            DataChunk() : id(0), version(1), length(0), offset(0) {}
        };

        using Serializer::read;

        using Serializer::write;

        /** 构造函数
        @param[in] data 数据的流
        @param[in] emode 字节序
        @param[in] autoHeader
        @version NIIEngine 3.0.0
        */
        FileStreamSerializer(const DataStream * data, Endian emode = E_Native, bool checkHeader = true);
        virtual ~FileStreamSerializer();

        /** 读取AABox
        @version NIIEngine 3.0.0
        */
        virtual void write(const AABox * in, NCount cnt = 1);

        /** 读取Sphere
        @version NIIEngine 3.0.0
        */
        virtual void write(const Sphere * in, NCount cnt = 1);

        /** 读取Plane
        @version NIIEngine 3.0.0
        */
        virtual void write(const Plane * in, NCount cnt = 1);

        /** 读取Ray
        @version NIIEngine 3.0.0
        */
        virtual void write(const Ray * in, NCount cnt = 1);

        /** 读取Radian
        @version NIIEngine 3.0.0
        */
        virtual void write(const Radian * in, NCount cnt = 1);

        /** 读取PosNode
        @version NIIEngine 3.0.0
        */
        virtual void write(const PosNode * in, NCount cnt = 1);

        /** 写入任意数据到一个流中
        @param[in] in 数据
        @param[in] size 元素大小;
        @param[in] cnt 元素的数量
        @version NIIEngine 3.0.0
        */
        virtual NCount write(const void * in, NCount size, NCount cnt, bool flip = true);

        /** 读取AABox
        @version NIIEngine 3.0.0
        */
        virtual void read(AABox * out, NCount cnt = 1) const;

        /** 读取Sphere
        @version NIIEngine 3.0.0
        */
        virtual void read(Sphere * out, NCount cnt = 1) const;

        /** 读取Plane
        @version NIIEngine 3.0.0
        */
        virtual void read(Plane * out, NCount cnt = 1) const;

        /** 读取Ray
        @version NIIEngine 3.0.0
        */
        virtual void read(Ray * out, NCount cnt = 1) const;

        /** 读取Radian
        @version NIIEngine 3.0.0
        */
        virtual void read(Radian * out, NCount cnt = 1);

        /** 读取PosNode
        @version NIIEngine 3.0.0
        */
        virtual void read(PosNode * out, NCount cnt = 1);

        /** 从一个流中读取任意数据
        @param[in] buf 数据
        @param[in] size 元素大小
        @param[in] cnt 元素的数量
        @version NIIEngine 3.0.0
        */
        virtual NCount read(void * out, NCount size, NCount cnt, bool flip = true) const;

        /** 写入
        @version NIIEngine 3.0.0
        */
        virtual void writeNIIfAsNIId(const NIIf * in, NCount cnt = 1);

        /** 写入
        @version NIIEngine 3.0.0
        */
        virtual void writeNIIdAsNIIf(const NIId * in, NCount cnt = 1);

        /** 读取
        @version NIIEngine 3.0.0
        */
        virtual void readNIIfAsNIId(NIId * out, NCount cnt = 1);

        /** 读取
        @version NIIEngine 3.0.0
        */
        virtual void readNIIdAsNIIf(NIIf * out, NCount cnt = 1);

        /** 获取字节序模式
        @version NIIEngine 3.0.0
        */
        virtual Endian getEndian() const;

        /** 获取当前读取/写入块的ID
        @version NIIEngine 3.0.0
        */
        Nui32 getCurrentChunkID() const;

        /** 获取当前与读取/写入块数据开始位置的距离
        @version NIIEngine 3.0.0
        */
        NCount getChunkDataOffset() const;

        /** 获取当前正在读取块的定义
        @version NIIEngine 3.0.0
        */
        virtual const DataChunk * getCurrentChunk() const;

        /** 读取文件块
        @note 使用完后调用readChunkEnd.
        @version NIIEngine 3.0.0
        */
        virtual const DataChunk * readChunkBegin();

        /** 读取指定文件块
        @param[in] id 块编号ID(用于验证)
        @param[in] version 能处理的最大版本
        @param[in] msg 版本不兼容日志说明文字
        @return 通过验证后的块
        @version NIIEngine 3.0.0
        */
        virtual const DataChunk * readChunkBegin(Nui32 id, Nui16 version, const String & msg = StrUtil::WBLANK);

        /** 退回到上数据块的结束位置
        @param[in] id 你需要读取块的ID
        @version NIIEngine 3.0.0
        */
        virtual void seekLastChunk(Nui32 id);

        /** 查看下一数据块的ID
        @version NIIEngine 3.0.0
        */
        virtual Nui32 peekNextChunkID();

        /** 结束读取一个块
        @remark 将跳到下一个数据块
        @param[in] id 块编号ID(用于验证)
        @version NIIEngine 3.0.0
        */
        virtual void readChunkEnd(Nui32 id);

        /** 是否在当前块的末尾
        @param[in] id 块编号(用于验证)
        @version NIIEngine 3.0.0
        */
        virtual bool isEndOfChunk(Nui32 id);

        /** 报告是否在流文件结尾
        @version NIIEngine 3.0.0
        */
        virtual bool eof() const;

        /** 开始写入一个新块
        @param[in] id 块编号(用于验证)
        @param[in] version 你写入的块的版本
        @version NIIEngine 3.0.0
        */
        virtual void writeChunkBegin(Nui32 id, Nui16 version = 1);

        /** 结束写入一个块.
        @param[in] id 块编号(用于验证)
        @version NIIEngine 3.0.0
        */
        virtual void writeChunkEnd(Nui32 id);

        /// @copydetails FileSerializer::readFileHeader
        virtual void readFileHeader();

        /// @copydetails FileSerializer::writeFileHeader
        virtual void writeFileHeader();
    protected:
        virtual DataChunk * readChunkImpl();
        virtual void writeChunkImpl(Nui32 id, Nui16 version);
        virtual DataChunk * popChunk(Nui32 id);

        virtual Nui32 calcChecksum(DataChunk * c);

        template <typename Tsrc, typename Tdst>
        void writeConverted(const Tsrc * src, NCount cnt)
        {
            Tdst* tmp = N_alloc_t(Tdst, cnt);
            Tdst* pDst = tmp;
            const Tsrc* pSrc = src;
            for (NCount i = 0; i < cnt; ++i)
                *pDst++ = static_cast<Tdst>(*pSrc++);

            write(tmp, sizeof(Tdst), cnt);

            N_free(tmp);
        }
        template <typename Tsrc, typename Tdst>
        void readConverted(Tsrc * dst, NCount cnt)
        {
            Tdst* tmp = N_alloc_t(Tdst, cnt);
            read(tmp, sizeof(Tdst), cnt);

            Tsrc* pDst = dst;
            const Tdst* pSrc = tmp;
            for (NCount i = 0; i < cnt; ++i)
                *pDst++ = static_cast<Tsrc>(*pSrc++);

            N_free(tmp);
        }
    protected:
        typedef deque<DataChunk *>::type ChunkStack;
    protected:
        ChunkStack mChunkStack;
        bool mCheckHeader;
        static Nui32 HEADER_ID;
        static Nui32 REVERSE_HEADER_ID;
        static Nui32 CHUNK_HEADER_SIZE;
    };
}

#endif