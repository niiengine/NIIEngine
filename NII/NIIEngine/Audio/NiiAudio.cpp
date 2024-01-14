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
#include "NiiAudio.h"
#include "NiiLogManager.h"
#include "NiiAudioCodec.h"
#include "NiiResourceSchemeManager.h"

namespace NII
{
    //------------------------------------------------------------------------
    Audio::Audio() :
        mFormat(SF_Unknow),
        mSampleRate(SR_Unknow),
        mSize(0),
        mBuffer(0),
        mAutoDelete(true)
    {
    }
    //------------------------------------------------------------------------
    Audio::Audio(const Audio & audio) :
        mBuffer(0),
        mAutoDelete(true)
    {
        *this = audio;
    }
    //------------------------------------------------------------------------
    Audio::~Audio()
    {
        if(mBuffer && mAutoDelete)
        {
            N_free(mBuffer);
            mBuffer = NULL;
        }
    }
    //------------------------------------------------------------------------
    Audio & Audio::operator = (const Audio & o)
    {
        if(mBuffer && mAutoDelete)
        {
            N_free(mBuffer);
            mBuffer = NULL;
        }
        mFormat = o.mFormat;
        mSampleRate = o.mSampleRate;
        mSize = o.mSize;
        mAutoDelete = o.mAutoDelete;
        if(mAutoDelete)
        {
            mBuffer = N_alloc_t(Nui8, mSize);
            memcpy(mBuffer, o.mBuffer, mSize);
        }
        else
        {
            mBuffer = o.mBuffer;
        }
        return *this;
    }
    //------------------------------------------------------------------------
    Audio & Audio::load(Nui8 * data, TimeDurMS dur, SampleFormat sf, SampleRate sr,
        Nui8 ch, bool autoDelete)
    {
        if(mBuffer && mAutoDelete)
        {
            N_free(mBuffer);
            mBuffer = NULL;
        }
        mFormat = sf;
        mSampleRate = sr;
        mChannel = ch;
        mLength = dur;
        mSize = AudioUtil::toByte(sf) * AudioUtil::toNum(sr) * ch;
        mAutoDelete = autoDelete;
        mBuffer = data;
        return *this;
    }
    //------------------------------------------------------------------------
    Audio & Audio::load(DataStream * stream, TimeDurMS dur, SampleFormat sf,
        SampleRate sr, Nui8 ch)
    {
        mFormat = sf;
        mSampleRate = sr;
        mChannel = ch;
        mLength = dur;
        mSize = AudioUtil::toByte(sf) * AudioUtil::toNum(sr) * ch;
        // 分配实际大小
        Nui8 * buffer = N_alloc_t(Nui8, mSize);
        // 从流中读取实际大小的数据到缓存中
        stream->read(buffer , mSize);

        return load(buffer, dur, sf, sr, ch, true);
    }
    //------------------------------------------------------------------------
    Audio & Audio::load(DataStream * stream, TimeDurMS dur, const String & hint)
    {
        if(mBuffer && mAutoDelete)
        {
            N_free(mBuffer);
            mBuffer = NULL;
        }
        mAutoDelete = true;
        AudioCodec * codec = 0;
        if(!hint.empty())
        {
            // 使用命名的编解码器
            codec = static_cast<AudioCodec *>(Codec::getCodec(hint));
        }
        else // 使用ffmpeg,因为支持的格式多
        {
            codec = static_cast<AudioCodec *>(Codec::getCodec(_T("ffmpegA")));

            if(!codec)
                N_EXCEPT(InvalidParam, _I18n("Unable to load Audio: Audio format is unknown. Unable to identify codec.Check it or specify format explicitly."));
        }

        return *this;
    }
    //------------------------------------------------------------------------
    Audio & Audio::load(const String & file, GroupID gid, TimeDurMS dur)
    {
        // 分割出后缀名
        String strExt;
        NCount pos = file.find_last_of(_T("."));
        if(pos != String::npos && pos < (file.length() - 1))
        {
            strExt = file.substr(pos+1);
        }
        
        // 打开文件
        DataStream * encoded = N_Only(ResourceScheme).open(file, gid);
        // 调用实际加载
        return load(encoded, dur, strExt);
    }
    //------------------------------------------------------------------------
    DataStream * Audio::encode(const String & ext)
    {
        if(!mBuffer) // 内部数据不存在时抛出异常
        {
            N_EXCEPT(InvalidParam, _I18n("No audio data loaded"));
        }
        // 由后缀名字找到 编码器,在ffmpeg中是不起效的
        Codec * pCodec = Codec::getCodec(ext);
        if(!pCodec)
            N_EXCEPT(InvalidParam, _I18n("Unable to encode audio data as '") + ext + _I18n("' - invalid extension."));

        DataStream * dest = 0;

        static_cast<AudioCodec *>(pCodec)->encode(this, dest, ext);

        return dest;
    }
    //------------------------------------------------------------------------
    void Audio::save(const String & filename, CodecID cid)
    {
        if(!mBuffer) // 内部数据不存在时抛出异常
        {
            N_EXCEPT(InvalidParam, _I18n("No Vioce data loaded"));
        }
        // 找出后缀名
        String strExt;
        NCount pos = filename.find_last_of(_T("."));
        if(pos == String::npos)
            N_EXCEPT(InvalidParam, _I18n("Unable to save audio file '") + filename + _I18n("' - invalid extension."));

        while(pos != filename.length() - 1)
            strExt += filename[++pos];
        // 由文件后缀名获取 编码器
        Codec * pCodec = Codec::getCodec(strExt);
        if(!pCodec)
            N_EXCEPT(InvalidParam, _I18n("Unable to save audio file '") + filename + _I18n("' - invalid extension."));

    }
    //------------------------------------------------------------------------
}