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

#include "NiiPreProcess.h"
#include "NiiSerializer.h"
#include "NiiDataStream.h"
#include "NiiException.h"

namespace NII
{
    // 流的头部 = 字节序标识 + 成分标识ID
    const NIIl Serializer::ChunkHeaderSize = sizeof(Nui16) + sizeof(Nui32);
    const Nui16 Serializer::NorEndian = 0x1000;
    const Nui16 Serializer::RevEndian = 0x0010;
    //------------------------------------------------------------------------
    Serializer::Serializer() :
        mFlipEndian(false),
        mStream(0)
    {
    }
    //------------------------------------------------------------------------
    Serializer::Serializer(const DataStream * org, bool flip) :
        mFlipEndian(flip),
        mStream(const_cast<DataStream *>(org))
    {
    }
    //------------------------------------------------------------------------
    Serializer::~Serializer()
    {
    }
    //------------------------------------------------------------------------
    void Serializer::write(bool in)
    {
        NIIb out = in;
        write(&out, sizeof(Nui8), 1, false);
    }
    //------------------------------------------------------------------------
    void Serializer::write(const bool * in, NCount count)
    {
        NIIb * out = (NIIb *)N_alloc(count);
        for(Nui32 i = 0; i < count; i++)
        {
            *(NIIb *)(out + i) = *(bool *)(in + i);
        }

        write(out, sizeof(Nui8), count, false);
        N_free(out);
    }
    //------------------------------------------------------------------------
    void Serializer::write(NIId in)
    {
        // Convert to NIIf, then write
        NIIf tmp = (NIIf)in;
        write(&tmp, sizeof(NIIf), 1);
    }
    //------------------------------------------------------------------------
    void Serializer::write(const NIId * in, NCount count)
    {
        // Convert to NIIf, then write
        NIIf * tmp = N_alloc_t(NIIf, count);
        for(Nui32 i = 0; i < count; ++i)
        {
            tmp[i] = static_cast<NIIf>(in[i]);
        }
        write(tmp, sizeof(NIIf), count);
        N_free(tmp);
    }
    //------------------------------------------------------------------------
    void Serializer::write(const WString & in)
    {
        Nui32 temp = in.length();
        write(temp);
        // Old, backwards compatible way - \n terminated
        write(in.c_str(), sizeof(wchar_t), in.length(), false);
        // Write terminating newline NIIb
        // NIIb terminator = '\n';
        // write(terminator, sizeof(Nui8), 1, false);
    }
    //------------------------------------------------------------------------
    void Serializer::write(const VString & in)
    {
        Nui32 temp = in.length();
        write(temp);
        // Old, backwards compatible way - \n terminated
        write(in.c_str(), sizeof(char), in.length(), false);
        // Write terminating newline NIIb
        // NIIb terminator = '\n';
        // write(terminator, sizeof(Nui8), 1, false);
    }
    //------------------------------------------------------------------------
    NCount Serializer::write(const void * in, NCount size, NCount count, bool _flip)
    {
        NCount amount = size * count;
        if(_flip && mFlipEndian)
        {
            Nui8 * out = N_alloc_t(Nui8, amount);

            flip((Nui8 *)in, out, size, count);
            amount = mStream->write(out, amount);

            N_free(out);
        }
        else
        {
            amount = mStream->write(in, amount);
        }
        return amount;
    }
    //------------------------------------------------------------------------
    NCount Serializer::read(bool * out, NCount count) const
    {
        // 由于PC机和移动设备的编译工具对于bool型描述有差异,为了能够保证从PC机(x86)
        // 上设计的模型能在移动设备上正常运行,都使用NIIb过度
        NIIb * temp = (NIIb *)N_alloc(count);
        // 不需要转换
        NCount c = read(temp, sizeof(Nui8), count, false);
        for(Nui32 i = 0; i < count; i++)
            *(bool *)(out + i) = *(NIIb *)(temp + i);
        N_free(temp);

        return c;
    }
    //------------------------------------------------------------------------
    NCount Serializer::read(NIId * out, NCount count) const
    {
        // Read from NIIf, convert to NIId
        NIIf * tmp = N_alloc_t(NIIf, count);
        NIIf * ptmp = tmp;

        NCount c = read(tmp, sizeof(NIIf), count);

        // Convert to doubles (no cast required)
        while(count--)
        {
            *out++ = *ptmp++;
        }
        N_free(tmp);

        return c;
    }
    //------------------------------------------------------------------------
    NCount Serializer::read(WString & out) const
    {
        Nui32 charcnt;
        read(&charcnt);

        wchar_t * str = N_alloc_t(wchar_t, charcnt);
        read(str, sizeof(wchar_t), charcnt, false);

        out = WString(str, charcnt);
        N_free(str);
        return charcnt + sizeof(4);
    }
    //------------------------------------------------------------------------
    NCount Serializer::read(VString & out) const
    {
        Nui32 charcnt;
        read(&charcnt);

        char * str = N_alloc_t(char, charcnt);
        read(str, sizeof(char), charcnt, false);

        out = VString(str, charcnt);
        N_free(str);
        return charcnt + sizeof(4);
    }
    //------------------------------------------------------------------------
    NCount Serializer::read(void * out, NCount size, NCount count, bool _flip) const
    {
        NCount amount = size * count;
        amount = mStream->read(out, amount);

        if(_flip && mFlipEndian)
        {
            flip((Nui8 *)out, size, count);
        }
        return amount;
    }
    //------------------------------------------------------------------------
    void Serializer::flip(Nui8 * data, NCount size, NCount count)
    {
        for(NCount i = 0; i < count; ++i)
        {
            mirror(data + i * size, size);
        }
    }
    //------------------------------------------------------------------------
    void Serializer::flip(const Nui8 * in, Nui8 * out, NCount size, NCount count)
    {
        NCount dest;
        for(NCount i = 0; i < count; ++i)
        {
            dest = i * size;
            mirror(in + dest, out + dest, size);
        }
    }
    //------------------------------------------------------------------------
    NCount Serializer::getSize(const WString & str)
    {
        return sizeof(Nui32) + str.length() * sizeof(wchar_t);
    }
    //------------------------------------------------------------------------
    NCount Serializer::getSize(const VString & str)
    {
        return sizeof(Nui32) + str.length();
    }
    //------------------------------------------------------------------------
    void Serializer::mirror(const Nui8 * in, Nui8 * out, NCount count)
    {
        for(NCount i = 0; i < count; ++i)
        {
            out[i] = in[count - i - 1];
        }
    }
    //------------------------------------------------------------------------
    void Serializer::mirror(Nui8 * data, NCount count)
    {
        Nui8 temp;
        NCount length = count / 2;

        for(NCount i = 0; i < length; ++i)
        {
            temp = data[i];
            data[i] = data[count - i - 1];
            data[count - i - 1] = temp;
        }
    }
    //------------------------------------------------------------------------
    void Serializer::flipL(Nui8 * data, NCount size, NCount count)
    {
#if N_ENDIAN == N_ENDIAN_B
        for(NCount i = 0; i < count; ++i)
        {
            mirror(data[i * size], size);
        }
#endif
    }
    //------------------------------------------------------------------------
    void Serializer::flipB(Nui8 * data, NCount size, NCount count)
    {
#if N_ENDIAN == N_ENDIAN_L
        for(NCount i = 0; i < count; ++i)
        {
            mirror(data + i * size, size);
        }
#endif
    }
    //------------------------------------------------------------------------
    void Serializer::check(DataStream * stream, NCount oft)
    {
        stream->skip(oft);

        Nui16 dest;
        // read header id manually (no conversion)
        NCount endian = stream->read(&dest, sizeof(Nui16));
        // skip back
        stream->skip(0 - (NIIl)endian - oft);
        if(endian != sizeof(Nui16))
        {
            // end of file?
            N_EXCEPT(InvalidParam, _I18n("不能读取字节序数据,数据流: ") + stream->getName());
        }

        if(dest == NorEndian)
        {
            mFlipEndian = false;
        }
        else if (dest == RevEndian)
        {
            mFlipEndian = true;
        }
        else
        {
            N_EXCEPT(InvalidParam, _I18n("读取字节序数据错误,数据流: ") + stream->getName());
        }
    }
    //------------------------------------------------------------------------
    void Serializer::check(Endian dest)
    {
        switch(dest)
        {
        case E_Native:
            mFlipEndian = false;
            break;
        case E_B:
#if N_ENDIAN == N_ENDIAN_B
            mFlipEndian = false;
#else
            mFlipEndian = true;
#endif
            break;
        case E_L:
#if N_ENDIAN == N_ENDIAN_B
            mFlipEndian = true;
#else
            mFlipEndian = false;
#endif
            break;
        }
    }
    //------------------------------------------------------------------------
}