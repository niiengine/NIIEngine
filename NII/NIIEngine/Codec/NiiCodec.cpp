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
#include "NiiCodec.h"
#include "NiiException.h"
#include "NiiStrConv.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Codec
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    Codec::CodecList Codec::mCodecs;
    //------------------------------------------------------------------------
    Codec::Codec(CodecID cid, const String & type)
    {
        mCID = cid;
        mType = type;
    }
    //------------------------------------------------------------------------
    Codec::~Codec()
    {
        CodecList::iterator i, iend = mCodecs.end();
        for(i = mCodecs.end(); i != iend; ++i)
        {
            if(i->second == this)
            {
                mCodecs.erase(i);
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    void Codec::add(Codec * c)
    {
        CodecList::iterator i = mCodecs.find(c->getType());
        if(i != mCodecs.end())
            N_EXCEPT(UniqueRepeat, c->getType() + _I18n(" 编码已经注册."));

        mCodecs[c->getType()] = c;
    }
    //------------------------------------------------------------------------
    bool Codec::isExist(const String & type)
    {
        return mCodecs.find(type) != mCodecs.end();
    }
    //------------------------------------------------------------------------
    void Codec::remove(Codec * c)
    {
        mCodecs.erase(c->getType());
    }
    //------------------------------------------------------------------------
    String Codec::getType() const
    {
        return mType;
    }
    //------------------------------------------------------------------------
    CodecID Codec::getCodeID() const
    {
        return mCID;
    }
    //------------------------------------------------------------------------
    StringList Codec::getExtensions()
    {
        StringList result;
        result.reserve(mCodecs.size());
        CodecList::const_iterator i, iend = mCodecs.end();
        for (i = mCodecs.begin(); i != iend; ++i)
        {
            result.push_back(i->first);
        }
        return result;
    }
    //------------------------------------------------------------------------
    Codec * Codec::getCodec(const String & type)
    {
        String lwrcase = type;
        StrUtil::toLower(lwrcase);
        CodecList::const_iterator i = mCodecs.find(lwrcase);
        if (i == mCodecs.end())
        {
            String formats_str;
            if(mCodecs.empty())
                formats_str = _I18n("There are no formats supported (no codecs registered).");
            else
                formats_str = _I18n("Supported formats are: ") + N_conv(getExtensions()) + _T(".");

            N_EXCEPT(NotExist, _I18n("Can not find codec for '") + type +
                    _I18n("' image format.\n") + formats_str);
        }
        return i->second;
    }
    //------------------------------------------------------------------------
    Codec * Codec::getCodec(CodecID cid)
    {
        CodecList::const_iterator i, iend = mCodecs.end();
        for(i = mCodecs.begin(); i != iend; ++i)
        {
            if(i->second->getCodeID() == cid)
                return i->second;
        }
        N_EXCEPT(NotExist, _I18n("不能找到编码器 '") + cid);
    }
    //------------------------------------------------------------------------
    Codec * Codec::getCodec(DataStream * data)
    {
        // 从magic数字中获得,读取第一个32字节或文件大小,如果小于的话
        NCount magicLen = N_MAX(NCount, data->size(), 32);
        Nui8 magicBuf[32];
        data->read(magicBuf, magicLen);
        data->seek(0);

        Codec * code = Codec::getCodec(magicBuf, magicLen);

        if(code)
            return code;
        return 0;
    }
    //------------------------------------------------------------------------
    Codec * Codec::getCodec(Nui8 * magic, NCount valid)
    {
        CodecList::const_iterator i, iend = mCodecs.end();
        for (i = mCodecs. begin(); i != iend; ++i)
        {
            String ext = i->second->match(magic, valid);
            if (!ext.empty())
            {
                // check codec type matches
                // if we have a single codec class that can handle many types,
                // and register many instances of it against different types, we
                // can end up matching the wrong one here, so grab the right one
                if (ext == i->second->getType())
                    return i->second;
                else
                    return getCodec(ext);
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
}