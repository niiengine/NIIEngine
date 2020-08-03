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

#ifndef _NII_APU_PARAMMap_H_
#define _NII_APU_PARAMMap_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 加速参数内存绑定
    @version NIIEngine 4.0.0
    */
    class _EngineAPI ApuParamMap : public GpuParamsAlloc
    {
    public:
        enum Type
        {
            T_Read,
            T_Write,
            T_Copy,
            T_Ref,
            T_Host
        };
    public:
        ApuParamMap(){}

        /**
        @version NIIEngine 4.0.0
        */
        void map(Nidex idx, Nmark tmark, void * src);

        /**
        @version NIIEngine 4.0.0
        */
        void read(Nidex idx, void * out) const;

        /**
        @version NIIEngine 4.0.0
        */
        void write(Nidex idx, const void * in);
    protected:

    };
}
#endif