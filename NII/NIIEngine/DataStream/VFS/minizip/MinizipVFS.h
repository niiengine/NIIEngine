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

#ifndef _NII_MINIZIP_VFS_H_
#define _NII_MINIZIP_VFS_H_

#include "NiiPreInclude.h"
#include "minizip/unzip.h"

namespace NII
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MinizipVFS : public VFS
    {
    public:
        MinizipVFS(const String & name, VFSType type);
        ~MinizipVFS();

        /// @copydetails VFS::isExist
        bool isExist(const String & file);

        /// @copydetails VFS::peek
        void peek();

        /// @copydetails VFS::open
        DataStream * open(const String & file, bool r = true) const;

        /// @copydetails VFS::list
        void find(StringList & out, const String & pattern, bool r = true, bool dirs = false);
    protected:
        unzFile d_zfile;
        String d_archive;
    };

    class _EngineAPI MinizipVFSFactory : public VFSFactory
    {
    public:
        MinizipVFSFactory();
        virtual ~MinizipVFSFactory();

        /// @copydetails VFSFactory::getType
        VFSType getType() const;

        /// @copydetails VFSFactory::create
        VFS * create(const String & prl);

        /// @copydetails VFSFactory::destroy
        void destroy(VFS * obj);
    };
}
#endif