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

#include "ASAPCodec.h"
#include "utils/URIUtils.h"
#include "filesystem/File.h"

namespace NII
{
namespace NII_MEDIA
{
    //------------------------------------------------------------------------
    ASAPCodec::ASAPCodec()
    {
        m_CodecName = "asap";
    }
    //------------------------------------------------------------------------
    ASAPCodec::~ASAPCodec()
    {
    }
    //------------------------------------------------------------------------
    bool ASAPCodec::Init(const CStdString &strFile, Nui32 filecache)
    {
        if (!m_dll.Load())
            return false;

        CStdString strFileToLoad = strFile;
        int song = -1;
        CStdString strExtension;
        URIUtils::GetExtension(strFile, strExtension);
        strExtension.MakeLower();
        if (strExtension == ".asapstream")
        {
            CStdString strFileName = URIUtils::GetFileName(strFile);
            int iStart = strFileName.ReverseFind('-') + 1;
            song = atoi(strFileName.substr(iStart, strFileName.size() - iStart - 11).c_str()) - 1;
            CStdString strPath = strFile;
            URIUtils::GetDirectory(strPath, strFileToLoad);
            URIUtils::RemoveSlashAtEnd(strFileToLoad);
        }

        int duration;
        if (!m_dll.asapLoad(strFileToLoad.c_str(), song, &m_Channels, &duration))
            return false;
        m_TotalTime = duration;
        m_SampleRate = 44100;
        m_BitsPerSample = 16;
        m_DataFormat = SF_S16;
        return true;
    }
    //------------------------------------------------------------------------
    void ASAPCodec::DeInit()
    {
    }
    //------------------------------------------------------------------------
    int64_t ASAPCodec::Seek(int64_t iSeekTime)
    {
        m_dll.asapSeek((int) iSeekTime);
        return iSeekTime;
    }
    //------------------------------------------------------------------------
    int ASAPCodec::ReadPCM(BYTE *pBuffer, int size, int *actualsize)
    {
        *actualsize = m_dll.asapGenerate(pBuffer, size);
        if (*actualsize < size)
            return READ_EOF;
        return READ_SUCCESS;
    }
    //------------------------------------------------------------------------
    bool ASAPCodec::CanInit()
    {
        return m_dll.CanLoad();
    }
    //------------------------------------------------------------------------
    bool ASAPCodec::IsSupportedFormat(const CStdString & strExt)
    {
        if(strExt.empty())
            return false;
        CStdString ext = strExt;
        if (ext[0] == '.')
            ext.erase(0, 1);
        return ext == "sap"
            || ext == "cmc" || ext == "cmr" || ext == "dmc"
            || ext == "mpt" || ext == "mpd" || ext == "rmt"
            || ext == "tmc" || ext == "tm8" || ext == "tm2"
            || ext == "cms" || ext == "cm3" || ext == "dlt";
    }
    //------------------------------------------------------------------------
}
}