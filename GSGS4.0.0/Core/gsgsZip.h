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
#ifndef GSGSZIP_H
#define GSGSZIP_H

#include "gsgsPreInclude.h"
#include <wx/zipstrm.h>
#include <wx/wfstream.h>
#include <wx/stream.h>

namespace gsgs
{
    class _gsgsAPI ZipReader
    {
    public:
        ZipReader(const wxFileName& zipfile);
        ~ZipReader();
        
        /**
         * @brief extract filename into 'path' (directory)
         * @param filename file name to extract. Wildcards ('*'/'?') can be used here
         * @param directory the target directory
         */
        void Extract(const wxString & filename, const wxString & directory);
        
        /**
         * @brief close the zip archive
         */
        void Close();
    private:
        wxFileInputStream *m_file;
        wxZipInputStream *m_zip;
    };

    class _gsgsAPI ZipWriter
    {
    public:
        ZipWriter(const wxFileName& zipfile);
        ~ZipWriter();

        /**
         * @brief add file to the zip
         */
        void Add(const wxFileName& file);
        
        /**
         * @brief add a directory to the archive
         * @param path directory path
         * @param pattern files to add mask
         */
        void AddDirectory(const wxString & path, const wxString & pattern);
        
        /**
         * @brief return the zip filename
         */
        inline const wxFileName & GetFilename() const { return m_filename; }
        
        /**
         * @brief close the zip
         */
        void Close();
    private:
        wxFileName m_filename;
        wxZipOutputStream* m_zip;
        wxFileOutputStream* m_file;
    };
}
#endif // CLZIP_H
