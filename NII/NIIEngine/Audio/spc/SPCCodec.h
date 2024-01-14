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
#include "snesapu/Types.h"
#include "../DllLoader/LibraryLoader.h"

namespace NII
{
namespace NII_MEDIA
{
    class SPCCodec : public ICodec
    {
    public:
        SPCCodec();
        virtual ~SPCCodec();

        virtual bool Init(const CStdString &strFile, unsigned int filecache);
        virtual void DeInit();
        virtual int64_t Seek(int64_t iSeekTime);
        virtual int ReadPCM(BYTE *pBuffer, int size, int *actualsize);
        virtual bool CanInit();
    private:
#ifdef _LINUX
        typedef void  (__cdecl *LoadMethod) ( const void* p1);
        typedef void* (__cdecl *EmuMethod) ( void *p1, u32 p2, u32 p3);
        typedef void  (__cdecl *SeekMethod) ( u32 p1, b8 p2 );
        typedef u32 (__cdecl *InitMethod)(void);
        typedef void (__cdecl *DeInitMethod)(void);
#else
        typedef void  (__stdcall* LoadMethod) ( const void* p1);
        typedef void* (__stdcall * EmuMethod) ( void *p1, u32 p2, u32 p3);
        typedef void  (__stdcall * SeekMethod) ( u32 p1, b8 p2 );
#endif
        struct
        {
            LoadMethod LoadSPCFile;
            EmuMethod EmuAPU;
            SeekMethod SeekAPU;
#ifdef _LINUX
            InitMethod InitAPU;
            DeInitMethod ResetAPU;
#endif
        } m_dll;

        LibraryLoader * m_loader;
        CStdString m_loader_name;

        char * m_szBuffer;
        u8 * m_pApuRAM;
        int64_t m_iDataPos;
    };
}
}
#endif
