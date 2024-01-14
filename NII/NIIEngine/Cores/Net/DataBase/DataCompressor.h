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

#ifndef __DATA_COMPRESSOR_H
#define __DATA_COMPRESSOR_H

#include "NiiNetPreInclude.h"
#include "DS_HuffmanEncodingTree.h"

namespace NII
{
namespace NII_NET
{
    /// \brief Does compression on a block of data.  Not very good compression, but it's small and fast so is something you can compute at runtime.
    class _EngineAPI DataCompressor
    {
    public:
        static void Compress(Nui8 * userData, Nui32 sizeInBytes, NetSerializer * output);
        static Nui32 DecompressAndAllocate(NetSerializer * input, Nui8 ** output);
    };

}
}
#endif
