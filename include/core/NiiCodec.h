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
#ifndef _NII_CODEC_H_
#define _NII_CODEC_H_

#include "NiiPreInclude.h"
#include "NiiString.h"
#include "NiiCodecList.h"

namespace NII
{
    #define CodecResult_Success     0   ///< 成功
    #define CodecResult_Error       1   ///< 无法解决的错误
    #define CodecResult_NoImpl      2   ///< 并没有实现这个函数
    #define CodecResult_Suspend     3   ///< 被系统中断
    #define CodecResult_NotMatch    4   ///< 格式并不匹配
    #define CodecResult_Count       5

    /** 编码基类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Codec : public CodecAlloc
    {
    public:
        typedef map<String, Codec *>::type CodecList;
    public:
        /** 构造函数
        @param[in] 统一格式标识,统一匹配
        @param[in] 扩展/文件后缀/magic提示(|符号隔开每项),辅助寻找
        @version NIIEngine 3.0.0
        */
        Codec(CodecID cid, const String & hint);
        virtual ~Codec();

        /** 是否能通过这个编码器解码
        @param[in] hint
            暗示需要的编码类型,或者编码代号,或者文件后缀,或者编码名字
        @version NIIEngine 3.0.0
        */
        virtual bool match(const String & hint) const = 0;

        /** 映射一个标志数字头到一个文件扩展名,如果这个编解码器识别它的话
        @param[in] magic
            指向一个字节流,标识文件.注意,这个相当需要-每个编解码器寻找一个不同大小
            的"特殊数字"
        @param[in] valid 输入的字节数量
        @return 一个空字符串如果一个魔术数字不详,或为文件扩展名.
        @version NIIEngine 3.0.0
        */
        virtual String match(const Nui8 * magic, NCount valid) const = 0;

        /** 返回编解码类型字符串
        @version NIIEngine 3.0.0
        */
        virtual String getType() const;

        /** 获取编码器唯一ID
        @version NIIEngine 3.0.0
        */
        virtual CodecID getCodeID() const;

        /** 从编码ID,获取编解码器.
        @version NIIEngine 3.0.0
        */
        static Codec * getCodec(CodecID cid);

        /** 从文件扩展名,获取编解码器.
        @version NIIEngine 3.0.0
        */
        static Codec * getCodec(const String & type);

        /** 从文件留中,获取编码解码器.
        @version NIIEngine 3.0.0
        */
        static Codec * getCodec(DataStream * data);

        /** 从一个"标签"标识符获得编解码器
        @param[in] magic 可能在文件头部,也可能在文件尾部,也可能存在歧义
        @param[in] count 字节数
        @version NIIEngine 3.0.0
        */
        static Codec * getCodec(Nui8 * magic, NCount count);

        /** 在数据库中注册一个新的编解码器.
        @version NIIEngine 3.0.0
        */
        static void add(Codec * c);

        /** 从数据库注销一个编解码器.
        @version NIIEngine 3.0.0
        */
        static void remove(Codec * c);

        /** 返回一个编解码器是否已经注册.
        @version NIIEngine 3.0.0
        */
        static bool isExist(const String & type);

        /** 获取编解码器的注册文件扩展名列表.
        @version NIIEngine 3.0.0
        */
        static StringList getExtensions();
    protected:
        CodecID mCID;                   ///< 统一标识
        String mType;                   ///< 提示标识
        Nui32 mFactor;                  ///< 线程因子(辅助多线程基数)
        static CodecList mCodecs;       ///< 已注册编解码器
    };
}
#endif