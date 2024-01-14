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
#ifndef SSHACCOUNTINFO_H
#define SSHACCOUNTINFO_H

#include "gsgsPreInclude.h"
#include "gsgsConfig.h"
#include "gsgsStringUtil.h"
#include <wx/clntdata.h>

#if USE_SFTP
namespace gsgs
{
    class _gsgsAPI SSHAccountInfo : public ObjectData
    {
    public:
        typedef std::vector<SSHAccountInfo> Vect_t;

    public:
        SSHAccountInfo();

        virtual ~SSHAccountInfo();
        SSHAccountInfo& operator=(const SSHAccountInfo& other);

        void AddBookmark(const wxString& location);

        inline void SetBookmarks(const wxArrayString& bookmarks)    { this->m_bookmarks = bookmarks; }
        inline const wxArrayString& GetBookmarks() const            { return m_bookmarks; }
        inline void SetAccountName(const wxString& accountName)     { this->m_accountName = accountName; }
        inline const wxString& GetAccountName() const               { return m_accountName; }
        inline void SetHost(const wxString& host)                   { this->m_host = host; }
        inline void SetPassword(const wxString& password)           { this->m_password = password; }
        inline void SetPort(int port)                               { this->m_port = port; }
        inline void SetUsername(const wxString& username)           { this->m_username = username; }
        inline const wxString& GetHost() const                      { return m_host; }
        inline const wxString& GetPassword() const                  { return m_password; }
        inline int GetPort() const                                  { return m_port; }
        inline const wxString& GetUsername() const                  { return m_username; }
        inline void SetDefaultFolder(const wxString& defaultFolder) { this->m_defaultFolder = defaultFolder; }
        inline const wxString& GetDefaultFolder() const             { return m_defaultFolder; }

        virtual void FromJSON(const JSONObject& json);
        virtual JSONObject ToJSON() const;
    private:
        wxString m_accountName;
        wxString m_username;
        wxString m_password;
        wxString m_host;
        wxString m_defaultFolder;
        wxArrayString m_bookmarks;
        int m_port;
    };

    class _gsgsAPI SFTPBookmark
    {
    public:
        SFTPBookmark();
        ~SFTPBookmark();

        void SetAccount(const SSHAccountInfo& account)      { this->m_account = account; }
        const SSHAccountInfo& GetAccount() const            { return m_account; }
        void SetFolder(const wxString& folder)              { this->m_folder = folder; }
        const wxString& GetFolder() const                   { return m_folder; }

        void SetName(const wxString& name)                  { this->m_name = name; }
        const wxString& GetName() const                     { return m_name; }

        //-----------------------
        // Seerialization API
        //-----------------------
        JSONObject ToJSON() const;
        void FromJSON(const JSONObject &json);
    private:
        wxString        m_name;
        SSHAccountInfo  m_account;
        wxString        m_folder;
    };

    class _gsgsAPI RemoteFileInfo : public wxClientData
    {
    public:
        typedef std::unordered_map<wxString, RemoteFileInfo> Map_t;
    public:
        RemoteFileInfo();
        virtual ~RemoteFileInfo();

        inline void SetAccount(const SSHAccountInfo& account) { this->m_account = account; }
        inline const SSHAccountInfo& GetAccount() const { return m_account; }
        inline const wxString& GetLocalFile() const { return m_localFile; }
        void SetRemoteFile(const wxString& remoteFile);
        inline const wxString& GetRemoteFile() const { return m_remoteFile; }
        inline void SetPremissions(size_t premissions) { this->m_premissions = premissions; }
        inline size_t GetPremissions() const { return m_premissions; }

        inline void SetLineNumber(int lineNumber) { this->m_lineNumber = lineNumber; }
        inline int GetLineNumber() const { return m_lineNumber; }

        static wxString GetTempFolder();
    private:
        SSHAccountInfo m_account; // the account for which this file belongs to
        wxString m_localFile;
        wxString m_remoteFile;
        size_t m_premissions;
        int m_lineNumber = 0;
    };
}
#endif // USE_SFTP
#endif // SSHACCOUNTINFO_H
