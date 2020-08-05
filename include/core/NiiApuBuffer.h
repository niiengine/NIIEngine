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
#include "NiiApuProgram.h"

namespace NII
{
    /** Apu缓存
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ApuBuffer : public Buffer
    {
    public:
        ApuBuffer(BufferManager * mag, NCount size, NCount count, Nmark mode);
        ~ApuBuffer();

        /** 获取单元数量
        @version NIIEngine 3.0.0
        */
        inline NCount getUnitCount() const      { return mUnitCount; }

        /** 获取单元大小
        @version NIIEngine 3.0.0
        */
        inline NCount getUnitSize() const       { return mUnitSize; }

        /// @copydetails Buffer::clone()
        Buffer * clone(Nmark m = M_WRITE | M_WHOLE | M_CPU) const;
    protected:
        NCount mUnitCount;
        NCount mUnitSize;
    };
}
#endif