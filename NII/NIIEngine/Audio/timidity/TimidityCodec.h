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

#include "ICodec.h"
#include "DllTimidity.h"

class TimidityCodec : public ICodec
{
public:
    TimidityCodec();
    virtual ~TimidityCodec();

    virtual bool Init(const CStdString & strFile, NCount filecache);
    virtual void DeInit();
    virtual int64_t Seek(int64_t iSeekTime);
    virtual int ReadPCM(BYTE * pBuffer, int size, int * actualsize);
    virtual bool CanInit();
    static bool IsSupportedFormat(const CStdString & strExt);

private:
    LibraryLoader * m_loader;
    CStdString m_loader_name;

    typedef int (__cdecl *InitMethod) ( const char * soundfont );
    typedef void* (__cdecl *LoadMethod) ( const char* p1);
    typedef int (__cdecl *FillMethod) ( void* p1, char* p2, int p3);
    typedef void  (__cdecl *CleanupMethod)();
    typedef void  (__cdecl *FreeMethod) ( void* p1);
    typedef const char* (__cdecl *ErrorMsgMethod) ();
    typedef unsigned long (__cdecl *LengthMethod) ( void* p1 );
    typedef unsigned long (__cdecl *SeekMethod) ( void* p1, unsigned long p2);

    struct
    {
        InitMethod Init;
        CleanupMethod Cleanup;
        ErrorMsgMethod ErrorMsg;
        LoadMethod LoadMID;
        FillMethod FillBuffer;
        FreeMethod FreeMID;
        LengthMethod GetLength;
        SeekMethod Seek;
    }     m_dll;

    void * m_mid;
    int m_iTrack;
    int64_t m_iDataPos;
};

#endif