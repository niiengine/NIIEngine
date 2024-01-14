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

#ifndef _NII_ILCodecs_H_
#define _NII_ILCodecs_H_

#include "NiiPreInclude.h"

#include <IL/il.h>
#include <IL/ilu.h>

namespace NII
{
    class ILImageCodec;

    class _EngineInner ILCodecs
    {
    protected:
       static std::list<ILImageCodec*> codeclist;
    public:
       ILenum getType(const String & ext);

       static void registerCodecs();

       static void deleteCodecs();
    };
}
#endif