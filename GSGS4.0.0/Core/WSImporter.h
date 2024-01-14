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

#ifndef WSIMPORTER_H
#define WSIMPORTER_H

#include "gsgsPreInclude.h"
#include <memory>
#include "GenericImporter.h"

class _gsgsAPI WSImporter
{
public:
    WSImporter();
    ~WSImporter();

    void Load(const wxString& filename, const wxString& defaultCompiler);
    bool Import(wxString& errMsg);

protected:
    void AddImporter(std::shared_ptr<GenericImporter> importer);

private:
    bool ContainsEnvVar(std::vector<wxString> elems);
    std::set<wxString> GetListEnvVarName(std::vector<wxString> elems);
    wxString GetVPath(const wxString& filename, const wxString& virtualPath);

    wxString filename, defaultCompiler;
    std::vector<std::shared_ptr<GenericImporter> > importers;
};

#endif // WSIMPORTER_H
