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

#ifndef __INCREMENTAL_READ_INTERFACE_H
#define __INCREMENTAL_READ_INTERFACE_H

#include "NiiNetPreInclude.h"
#include "FileListNodeContext.h"

namespace NII
{
namespace NII_NET
{
    class _EngineAPI BufferReadBase
    {
    public:
        BufferReadBase() {}
        virtual ~BufferReadBase() {}

        /// Read part of a file into \a destination
        /// Return the number of bytes written. Return 0 when file is done.
        /// \param[in] filename Filename to read
        /// \param[in] startReadBytes What offset from the start of the file to read from
        /// \param[in] numBytesToRead How many bytes to read. This is also how many bytes have been allocated to preallocatedDestination
        /// \param[out] preallocatedDestination Write your data here
        /// \return The number of bytes read, or 0 if none
        virtual Nui32 GetFilePart(const String & filename, Nui32 startReadBytes,
            Nui32 numBytesToRead, void * preallocatedDestination, FileListNodeContext context);
    };
}
}
#endif