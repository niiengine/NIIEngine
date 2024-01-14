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

#ifndef SFTPITEMCOMPARATOR_H
#define SFTPITEMCOMPARATOR_H

#include <vector>
#include <wx/treectrl.h>

class MyClientData : public wxTreeItemData
{
    wxString m_path;
    bool     m_initialized;
    bool     m_isFolder;

public:
    typedef std::vector<MyClientData*> Vector_t;

public:
    MyClientData(const wxString &path)
        : m_path(path)
        , m_initialized(false)
        , m_isFolder(false) {
        while (m_path.Replace("//", "/")) {}
        while (m_path.Replace("\\\\", "\\")) {}
    }

    virtual ~MyClientData() {}
    
    wxString GetBasename() const {
        return GetFullPath().BeforeLast('/');
    }
    wxString GetFullName() const {
        return GetFullPath().AfterLast('/');
    }
    void SetFullName( const wxString &fullname ) {
        wxString base = GetBasename();
        base << "/" << fullname;
        m_path.swap( base );
    }
    
    void SetInitialized(bool initialized) {
        this->m_initialized = initialized;
    }
    bool IsInitialized() const {
        return m_initialized;
    }
    void SetPath(const wxString& path) {
        this->m_path = path;
    }
    const wxString& GetFullPath() const {
        return m_path;
    }
    void SetIsFolder(bool isFolder) {
        this->m_isFolder = isFolder;
    }
    bool IsFolder() const {
        return m_isFolder;
    }
};

#endif // SFTPITEMCOMPARATOR_H
