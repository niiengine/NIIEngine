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

#ifndef NODEJSLOCATOR_H
#define NODEJSLOCATOR_H

#include "gsgsPreInclude.h"

class _gsgsAPI NodeJSLocator
{
public:
    NodeJSLocator();
    ~NodeJSLocator();

    /**
     * @brief attempt to auto locate nodejs installation
     */
    void Locate(const wxArrayString& hints = {});

    const wxString& GetNodejs() const { return m_nodejs; }
    const wxString& GetNpm() const { return m_npm; }
protected:
    bool TryPaths(const wxArrayString& paths, const wxString& fullname, wxFileName& fullpath);
private:
    wxString m_nodejs;
    wxString m_npm;
};

#endif // NODEJSLOCATOR_H
