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
#include "NiiAudioFormat.h"

#define AUD_U8_0    0x80
#define AUD_S16_MAX 0x7FFF
#define AUD_S16_MIN -32768
#define AUD_S16_FLT 32767.0f
#define AUD_S32_MAX 0x7FFFFFFF
#define AUD_S32_MIN 0x80000000
#define AUD_S32_FLT 2147483647.0f
#define AUD_FLT_MAX 1.0f
#define AUD_FLT_MIN -1.0f

namespace NII
{
    //-----------------------------------------------------------------------
    NCount AudioUtil::toByte(SampleFormat sf)
    {
        NCount a = 0;
        switch(sf)
        {
            case SF_U8:
                a = 1;break;
            case SF_S16:
                a = 2;break;
            case SF_S24W3:
            case SF_S24W3L:
                a = 3;break;
            case SF_S32:
            case SF_Float:
                a = 4;break;
            case SF_Double:
                a = 8;break;
            default: a = 0;
        }
        return a;
    }
    //-----------------------------------------------------------------------
    NCount AudioUtil::toNum(SampleRate sr)
    {
        switch(sr)
        {
        case SR_8000Hz: return 8000;
        case SR_11025Hz: return 11025;
        case SR_12000Hz: return 12000;
        case SR_16000Hz: return 16000;
        case SR_22050Hz: return 22050;
        case SR_24000Hz: return 24000;
        case SR_32000Hz: return 32000;
        case SR_44100Hz: return 44100;
        case SR_48000Hz: return 48000;
        default: return 0;
        }
        return 0;
    }
    //-----------------------------------------------------------------------
    SampleRate AudioUtil::toSampleRate(Nui32 num)
    {
        switch(num)
        {
            case 8000: return SR_8000Hz;
            case 11025: return SR_11025Hz;
            case 12000: return SR_12000Hz;
            case 16000: return SR_16000Hz;
            case 22050: return SR_22050Hz;
            case 24000: return SR_24000Hz;
            case 32000: return SR_32000Hz;
            case 44100: return SR_44100Hz;
            case 48000: return SR_48000Hz;
            default: return SR_Unknow;
        }
        return SR_Unknow;
    }
    //-----------------------------------------------------------------------
    NCount AudioUtil::calcSize(Nui32 bits, Nui32 sr, Nui8 channel)
    {
        //数据量(字节/秒)=  采样频率(Hz)′采样位数(bit)′声道数
        return bits * sr * channel /8;
    }
    //-----------------------------------------------------------------------
    NCount AudioUtil::calcSize(TimeDurMS dur, SampleFormat sf, SampleRate sr, Nui8 channel)
    {
        return dur * toByte(sf) * toNum(sr)* channel;
    }
    //-----------------------------------------------------------------------
    NCount AudioUtil::calcSize(SampleFormat sf, Nui8 channel)
    {
        return toByte(sf) * channel;
    }
    //-----------------------------------------------------------------------
    AudioUtil::convert AudioUtil::Nui8toNui8(SampleFormat sf)
    {
        switch(sf)
        {
        case SF_U8 :
            break;
        case SF_S16 :
            break;
        case SF_S32 :
            break;
        };
        return Nui8toNi16;
    }
    //-----------------------------------------------------------------------
    AudioUtil::convert AudioUtil::Nui8toNIIf(SampleFormat sf)
    {
        switch(sf)
        {
        case SF_U8 :
            break;
        case SF_S16 :
            break;
        case SF_S32 :
            break;
        };
        return Nui8toNi16;
    }
    //-----------------------------------------------------------------------
    AudioUtil::convert AudioUtil::NIIftoNui8(SampleFormat sf)
    {
        switch(sf)
        {
        case SF_U8 :
            break;
        case SF_S16 :
            break;

        case SF_S32 :
            break;
        };
        return Nui8toNi16;
    }
    //-----------------------------------------------------------------------
    NCount AudioUtil::calcSize(SampleFormat sf, SampleRate sr, Nui8 channel)
    {
        return toByte(sf) * toNum(sr)* channel;
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Nui8toNi16(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni16 * t = (Ni16 *)out;
        for(NCount i = count - 1; i >= 0; --i)
            t[i] = (((Ni16)in[i]) - AUD_U8_0) << 8;
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Nui8toNi24be(Nui8 * in, Nui8 * out, NCount count)
    {
        for(NCount i = count - 1; i >= 0; --i)
        {
            out[i*3] = in[i] - AUD_U8_0;
            out[i*3+1] = 0;
            out[i*3+2] = 0;
        }
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Nui8toNi24le(Nui8 * in, Nui8 * out, NCount count)
    {
        for(NCount i = count - 1; i >= 0; --i)
        {
            out[i*3+2] = in[i] - AUD_U8_0;
            out[i*3+1] = 0;
            out[i*3] = 0;
        }
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Nui8toNi32(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni32 * t = (Ni32 *) out;
        for(NCount i = count - 1; i >= 0; --i)
            t[i] = (((Ni32)in[i]) - AUD_U8_0) << 24;
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Nui8toNIIf(Nui8 * in, Nui8 * out, NCount count)
    {
        NIIf * t = (NIIf *) out;
        for(NCount i = count - 1; i >= 0; --i)
            t[i] = (((Ni32)in[i]) - AUD_U8_0) / ((NIIf)AUD_U8_0);
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Nui8toNIId(Nui8 * in, Nui8 * out, NCount count)
    {
        NIId * t = (NIId *) out;
        for(NCount i = count - 1; i >= 0; --i)
            t[i] = (((Ni32)in[i]) - AUD_U8_0) / ((NIId)AUD_U8_0);
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Ni16toNui8(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni16 * s = (Ni16 *) in;
        for(NCount i = 0; i < count; ++i)
            out[i] = (Nui8)((s[i] >> 8) + AUD_U8_0);
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Ni16toNi24be(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni16 * s = (Ni16 *)in;
        Ni16 t;
        for(NCount i = count - 1; i >= 0; --i)
        {
            t = s[i];
            out[i*3] = t >> 8 & 0xFF;
            out[i*3+1] = t & 0xFF;
            out[i*3+2] = 0;
        }
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Ni16toNi24le(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni16 * s = (Ni16 *)in;
        Ni16 t;
        for(NCount i = count - 1; i >= 0; --i)
        {
            t = s[i];
            out[i*3+2] = t >> 8 & 0xFF;
            out[i*3+1] = t & 0xFF;
            out[i*3] = 0;
        }
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Ni16toNi32(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni16 * s = (Ni16 *)in;
        Ni32 * t = (Ni32 *)out;
        for(NCount i = count - 1; i >= 0; --i)
            t[i] = ((Ni32)s[i]) << 16;
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Ni16toNIIf(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni16 * s = (Ni16 *)in;
        NIIf * t = (NIIf *)out;
        for(NCount i = count - 1; i >= 0; --i)
            t[i] = s[i] / AUD_S16_FLT;
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Ni16toNIId(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni16 * s = (Ni16 *)in;
        NIId * t = (NIId *)out;
        for(NCount i = count - 1; i >= 0; --i)
            t[i] = s[i] / AUD_S16_FLT;
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Ni24toNui8be(Nui8 * in, Nui8 * out, NCount count)
    {
        for(NCount i = 0; i < count; ++i)
            out[i] = in[i * 3] ^ AUD_U8_0;
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Ni24toNui8le(Nui8 * in, Nui8 * out, NCount count)
    {
        for(NCount i = 0; i < count; ++i)
            out[i] = in[i * 3 + 2] ^ AUD_U8_0;
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Ni24toNi16be(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni16 * t = (Ni16 *)out;
        for(NCount i = 0; i < count; ++i)
            t[i] = in[i * 3] << 8 | in[i * 3 + 1];
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Ni24toNi16le(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni16 * t = (Ni16 *)out;
        for(NCount i = 0; i < count; ++i)
            t[i] = in[i * 3 + 2] << 8 | in[i * 3 + 1];
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Ni24toNi24(Nui8 * in, Nui8 * out, NCount count)
    {
        memcpy(out, in, count * 3);
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Ni24toNi32be(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni32 * t = (Ni32 *)out;
        for(NCount i = count - 1; i >= 0; --i)
            t[i] = in[i*3] << 24 | in[i*3+1] << 16 | in[i*3+2] << 8;
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Ni24toNi32le(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni32 * t = (Ni32 *)out;
        for(NCount i = count - 1; i >= 0; --i)
            t[i] = in[i*3+2] << 24 | in[i*3+1] << 16 | in[i*3] << 8;
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Ni24toNIIfbe(Nui8 * in, Nui8 * out, NCount count)
    {
        NIIf * t = (NIIf *)out;
        Ni32 s;
        for(NCount i = count - 1; i >= 0; --i)
        {
            s = in[i*3] << 24 | in[i*3+1] << 16 | in[i*3+2] << 8;
            t[i] = s / AUD_S32_FLT;
        }
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Ni24toNIIfle(Nui8 * out, Nui8 * in, NCount count)
    {
        NIIf * t = (NIIf *)out;
        Ni32 s;
        for(NCount i = count - 1; i >= 0; --i)
        {
            s = in[i*3+2] << 24 | in[i*3+1] << 16 | in[i*3] << 8;
            t[i] = s / AUD_S32_FLT;
        }
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Ni24toNIIdbe(Nui8 * in, Nui8 * out, NCount count)
    {
        NIId * t = (NIId *)out;
        Ni32 s;
        for(NCount i = count - 1; i >= 0; --i)
        {
            s = in[i*3] << 24 | in[i*3+1] << 16 | in[i*3+2] << 8;
            t[i] = s / AUD_S32_FLT;
        }
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Ni24toNIIdle(Nui8 * in, Nui8 * out, NCount count)
    {
        NIId * t = (NIId *)out;
        Ni32 s;
        for(NCount i = count - 1; i >= 0; --i)
        {
            s = in[i*3+2] << 24 | in[i*3+1] << 16 | in[i*3] << 8;
            t[i] = s / AUD_S32_FLT;
        }
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Ni32toNui8(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni16 * s = (Ni16 *)in;
        for(NCount i = 0; i < count; ++i)
            out[i] = (Nui8)((s[i] >> 24) + AUD_U8_0);
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Ni32toNi16(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni16 * t = (Ni16 *) out;
        Ni32 * s = (Ni32 *) in;
        for(NCount i = 0; i < count; ++i)
            t[i] = s[i] >> 16;
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Ni32toNi24be(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni32 * s = (Ni32 *)in;
        Ni32 t;
        for(NCount i = 0; i < count; ++i)
        {
            t = s[i];
            out[i*3] = t >> 24 & 0xFF;
            out[i*3+1] = t >> 16 & 0xFF;
            out[i*3+2] = t >> 8 & 0xFF;
        }
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Ni32toNi24le(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni32 * s = (Ni32 *)in;
        Ni32 t;
        for(NCount i = 0; i < count; ++i)
        {
            t = s[i];
            out[i*3+2] = t >> 24 & 0xFF;
            out[i*3+1] = t >> 16 & 0xFF;
            out[i*3] = t >> 8 & 0xFF;
        }
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Ni32toNIIf(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni32 * s = (Ni32 *)in;
        NIIf * t = (NIIf *)out;
        for(NCount i = 0; i < count; ++i)
            t[i] = s[i] / AUD_S32_FLT;
    }
    //-----------------------------------------------------------------------
    void AudioUtil::Ni32toNIId(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni32 * s = (Ni32 *)in;
        NIId * t = (NIId *)out;
        for(NCount i = count - 1; i >= 0; --i)
            t[i] = s[i] / AUD_S32_FLT;
    }
    //-----------------------------------------------------------------------
    void AudioUtil::NIIftoNui8(Nui8 * in, Nui8 * out, NCount count)
    {
        NIIf * s = (NIIf *)in;
        NIIf t;
        for(NCount i = 0; i < count; ++i)
        {
            t = s[i] + AUD_FLT_MAX;
            if(t <= 0.0f)
                out[i] = 0;
            else if(t >= 2.0f)
                out[i] = 255;
            else
                out[i] = (Nui8)(t*127);
        }
    }
    //-----------------------------------------------------------------------
    void AudioUtil::NIIftoNi16(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni16 * t = (Ni16 *)out;
        NIIf * s = (NIIf *)in;
        for(NCount i = 0; i < count; ++i)
        {
            if(s[i] <= AUD_FLT_MIN)
                t[i] = (Ni16)AUD_S16_MIN;
            else if(s[i] >= AUD_FLT_MAX)
                t[i] = AUD_S16_MAX;
            else
                t[i] = (Ni16)(s[i] * AUD_S16_MAX);
        }
    }
    //-----------------------------------------------------------------------
    void AudioUtil::NIIftoNi24be(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni32 t;
        NIIf * s = (NIIf *)in;
        for(NCount i = 0; i < count; ++i)
        {
            if(s[i] <= AUD_FLT_MIN)
                t = AUD_S32_MIN;
            else if(s[i] >= AUD_FLT_MAX)
                t = AUD_S32_MAX;
            else
                t = (Ni32)(s[i]*AUD_S32_MAX);
            out[i*3] = t >> 24 & 0xFF;
            out[i*3+1] = t >> 16 & 0xFF;
            out[i*3+2] = t >> 8 & 0xFF;
        }
    }
    //-----------------------------------------------------------------------
    void AudioUtil::NIIftoNi24le(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni32 t;
        NIIf * s = (NIIf *)in;
        for(NCount i = 0; i < count; ++i)
        {
            if(s[i] <= AUD_FLT_MIN)
                t = AUD_S32_MIN;
            else if(s[i] >= AUD_FLT_MAX)
                t = AUD_S32_MAX;
            else
                t = (Ni32)(s[i] * AUD_S32_MAX);
            out[i*3+2] = t >> 24 & 0xFF;
            out[i*3+1] = t >> 16 & 0xFF;
            out[i*3] = t >> 8 & 0xFF;
        }
    }
    //-----------------------------------------------------------------------
    void AudioUtil::NIIftoNi32(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni32 * t = (Ni32 *)out;
        NIIf * s = (NIIf *)in;
        for(NCount i = 0; i < count; ++i)
        {
            if(s[i] <= AUD_FLT_MIN)
                t[i] = AUD_S32_MIN;
            else if(s[i] >= AUD_FLT_MAX)
                t[i] = AUD_S32_MAX;
            else
                t[i] = (Ni32)(s[i] * AUD_S32_MAX);
        }
    }
    //-----------------------------------------------------------------------
    void AudioUtil::NIIftoNIId(Nui8 * in, Nui8 * out, NCount count)
    {
        NIIf * s = (NIIf *)in;
        NIId * t = (NIId *)out;
        for(NCount i = count - 1; i >= 0; --i)
            t[i] = s[i];
    }
    //-----------------------------------------------------------------------
    void AudioUtil::NIIdtoNui8(Nui8 * in, Nui8 * out, NCount count)
    {
        NIId * s = (NIId *)in;
        NIId t;
        for(NCount i = 0; i < count; ++i)
        {
            t = s[i] + AUD_FLT_MAX;
            if(t <= 0.0)
                out[i] = 0;
            else if(t >= 2.0)
                out[i] = 255;
            else
                out[i] = (Nui8)(t * 127);
        }
    }
    //-----------------------------------------------------------------------
    void AudioUtil::NIIdtoNi16(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni16 * t = (Ni16 *)out;
        NIId * s = (NIId *)in;
        for(NCount i = 0; i < count; ++i)
        {
            if(s[i] <= AUD_FLT_MIN)
                t[i] = AUD_S16_MIN;
            else if(s[i] >= AUD_FLT_MAX)
                t[i] = AUD_S16_MAX;
            else
                t[i] = (Ni16)(s[i] * AUD_S16_MAX);
        }
    }
    //-----------------------------------------------------------------------
    void AudioUtil::NIIdtoNi24be(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni32 t;
        NIId * s = (NIId *)in;
        for(NCount i = 0; i < count; ++i)
        {
            if(s[i] <= AUD_FLT_MIN)
                t = AUD_S32_MIN;
            else if(s[i] >= AUD_FLT_MAX)
                t = AUD_S32_MAX;
            else
                t = (Ni32)(s[i]*AUD_S32_MAX);
            out[i*3] = t >> 24 & 0xFF;
            out[i*3+1] = t >> 16 & 0xFF;
            out[i*3+2] = t >> 8 & 0xFF;
        }
    }
    //-----------------------------------------------------------------------
    void AudioUtil::NIIdtoNi24le(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni32 t;
        NIId * s = (NIId *)in;
        for(NCount i = 0; i < count; ++i)
        {
            if(s[i] <= AUD_FLT_MIN)
                t = AUD_S32_MIN;
            else if(s[i] >= AUD_FLT_MAX)
                t = AUD_S32_MAX;
            else
                t = (Ni32)(s[i] * AUD_S32_MAX);
            out[i*3+2] = t >> 24 & 0xFF;
            out[i*3+1] = t >> 16 & 0xFF;
            out[i*3] = t >> 8 & 0xFF;
        }
    }
    //-----------------------------------------------------------------------
    void AudioUtil::NIIdtoNi32(Nui8 * in, Nui8 * out, NCount count)
    {
        Ni32 * t = (Ni32 *)out;
        NIId * s = (NIId *)in;
        for(NCount i = 0; i < count; ++i)
        {
            if(s[i] <= AUD_FLT_MIN)
                t[i] = AUD_S32_MIN;
            else if(s[i] >= AUD_FLT_MAX)
                t[i] = AUD_S32_MAX;
            else
                t[i] = (Ni32)(s[i] * AUD_S32_MAX);
        }
    }
    //-----------------------------------------------------------------------
    void AudioUtil::NIIdtoNIIf(Nui8 * in, Nui8 * out, NCount count)
    {
        NIId * s = (NIId *)in;
        NIIf * t = (NIIf *)out;
        for(NCount i = 0; i < count; ++i)
            t[i] = (NIIf)s[i];
    }
    //-----------------------------------------------------------------------
}