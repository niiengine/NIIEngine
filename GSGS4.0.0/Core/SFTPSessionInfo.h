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
#ifndef SFTPSESSIONINFO_H
#define SFTPSESSIONINFO_H

#if USE_SFTP

#include <gsgsPreInclude.h>
#include "gsgsConfig.h"
#include <gsgsStringUtil.h>

using namespace gsgs;
class _gsgsAPI SFTPSessionInfo
{
public:
    SFTPSessionInfo();
    ~SFTPSessionInfo();

    void FromJSON(const JSONObject& json);
    JSONObject ToJSON() const;

    void SetFiles(const std::vector<wxString> & files) { m_files = files; }
    void SetRootFolder(const wxString & rootFolder) { m_rootFolder = rootFolder; }
    const std::vector<wxString>& GetFiles() const { return m_files; }
    std::vector<wxString>& GetFiles() { return m_files; }
    const wxString& GetRootFolder() const { return m_rootFolder; }
    void SetAccount(const wxString& account) { this->m_account = account; }
    const wxString& GetAccount() const { return m_account; }
    void Clear();
    bool IsOk() const;
    void RemoveFile(const wxString& path);
    void AddFile(const wxString& path);
public:
    typedef std::unordered_map<wxString, SFTPSessionInfo> Map_t;
private:
    std::vector<wxString> m_files;
    wxString m_rootFolder;
    wxString m_account;
};

class _gsgsAPI SFTPSessionInfoList : public ObjectData
{
public:
    SFTPSessionInfoList();
    virtual ~SFTPSessionInfoList();
    void FromJSON(const JSONObject& json);
    JSONObject ToJSON() const;

    SFTPSessionInfoList & Load();
    SFTPSessionInfoList & Save();

    /**
     * @brief load session for the given account
     */
    SFTPSessionInfo& GetSession(const wxString & accountName);
    /**
     * @brief add session. If a session with this name already exists, overwrite it
     */
    SFTPSessionInfoList& SetSession(const SFTPSessionInfo & sess);
private:
    SFTPSessionInfo::Map_t m_sessions;
};
#endif

#endif // SFTPSESSIONINFO_H
