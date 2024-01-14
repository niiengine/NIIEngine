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

#include "NiiNetBufferReadBase.h"
#include <stdio.h>

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    Nui32 BufferReadBase::GetFilePart(const String & filename, 
        Nui32 startReadBytes, Nui32 numBytesToRead, void * preallocatedDestination,
            FileListNodeContext context)
    {
        FILE * fp = fopen(filename.c_str(), "rb");
        if(fp == 0)
            return 0;
        fseek(fp, startReadBytes, SEEK_SET);
        Nui32 numRead = (Nui32) fread(preallocatedDestination, 1, numBytesToRead, fp);
        fclose(fp);
        return numRead;
    }
    //------------------------------------------------------------------------
}
}