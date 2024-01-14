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

#ifndef _NII_ApuBuffer_H_
#define _NII_ApuBuffer_H_

#include "NiiPreInclude.h"
#include "NiiBuffer.h"

namespace NII
{
    /** 加速程序语法标准
    @version NIIEngine 4.0.0
    */
    enum ApuLanguage
    {
        ApuL_Unknow = 0,
        ApuL_OPENCL = 0x1,
        ApuL_CUDA   = 0x2
    };
    typedef Nui32 ApuLanguageMark;

    /** Apu缓存
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ApuBuffer : public Buffer
    {
    public:
        /** 缓存存储应用类型
        @version NIIEngine 4.0.0
        */
        enum ExtMode
        {
            EM_AllocHost    = Buffer::M_EXT1,
            EM_CopyHost     = Buffer::M_EXT2,
            EM_RefHost      = Buffer::M_EXT3
        };
    public:
        ApuBuffer(BufferManager * mag, NCount unitSize, NCount unitCnt, Nmark mode, ApuLanguage type);
        virtual ~ApuBuffer();
    protected:
        ApuLanguage mType;
    };
}
#endif