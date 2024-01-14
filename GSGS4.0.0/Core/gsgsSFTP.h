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

#ifndef gsgsSCP_H
#define gsgsSCP_H

#if USE_SFTP

#include "gsgsPreInclude.h"
#include "gsgsSSH.h"
#include "gsgsSharedPtr.h"
#include <wx/buffer.h>
#include <wx/clntdata.h>

// We do it this way to avoid exposing the include to <libssh/sftp.h> to files including this header
struct sftp_attributes_struct;
typedef struct sftp_attributes_struct* SFTPAttribute_t;
struct sftp_session_struct;
typedef struct sftp_session_struct* SFTPSession_t;

namespace gsgs
{
    class _gsgsAPI SFTPAttribute : public wxClientData
    {
    public:
        typedef SharedPtr<SFTPAttribute> Ptr_t;
        typedef std::list<SFTPAttribute::Ptr_t> List_t;

        enum {
            TYPE_FOLDER         = 0x00000001,
            TYPE_SYMBLINK       = 0x00000002,
            TYPE_REGULAR_FILE   = 0x00000004,
            TYPE_SEPCIAL        = 0x00000008,
            TYPE_UNKNOWN        = 0x00000010,
        };
    public:
        SFTPAttribute(SFTPAttribute_t attr);
        virtual ~SFTPAttribute();

        static bool Compare(SFTPAttribute::Ptr_t one, SFTPAttribute::Ptr_t two);
        /**
         * @brief assign this object with attributes.
         * This object takes ownership of the attributes and it will free it when done
         * @param attr
         */
        void Assign(SFTPAttribute_t attr);

        inline size_t GetSize() const                   { return m_size; }
        wxString GetTypeAsString() const;
        inline const wxString& GetName() const          { return m_name; }

        inline bool IsFolder() const                    { return m_flags & TYPE_FOLDER; }

        inline bool IsFile() const                      { return m_flags & TYPE_REGULAR_FILE; }

        inline bool IsSymlink() const                   { return m_flags & TYPE_SYMBLINK; }

        inline bool IsSpecial() const                   { return m_flags & TYPE_SEPCIAL; }
        inline void SetPermissions(size_t permissions)  { this->m_permissions = permissions; }
        inline size_t GetPermissions() const            { return m_permissions; }
    protected:
        void DoClear();
        void DoConstruct();
    private:
        wxString m_name;
        SFTPAttribute_t m_attributes;
        size_t m_flags;
        size_t m_size;
        size_t m_permissions;
    };

    class _gsgsAPI SFTPEvent : public Event
    {
    public:
        SFTPEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
        SFTPEvent(const SFTPEvent& event);
        SFTPEvent& operator=(const SFTPEvent& src);

        virtual ~SFTPEvent();
        virtual wxEvent* Clone() const                              { return new SFTPEvent(*this); }

        inline void SetAccount(const wxString& account)             { this->m_account = account; }
        inline void SetLocalFile(const wxString& localFile)         { this->m_localFile = localFile; }
        inline void SetRemoteFile(const wxString& remoteFile)       { this->m_remoteFile = remoteFile; }
        inline const wxString& GetAccount() const                   { return m_account; }
        inline const wxString& GetLocalFile() const                 { return m_localFile; }
        inline const wxString& GetRemoteFile() const                { return m_remoteFile; }
        inline void SetNewRemoteFile(const wxString& newRemoteFile) { this->m_newRemoteFile = newRemoteFile; }
        inline const wxString& GetNewRemoteFile() const             { return m_newRemoteFile; }
    protected:
        wxString m_account;
        wxString m_localFile;
        wxString m_remoteFile;
        wxString m_newRemoteFile;
    };

    class _gsgsAPI SFTP
    {
    public:
        typedef wxSharedPtr<SFTP> Ptr_t;
        enum 
        {
            SFTP_BROWSE_FILES = 0x00000001,
            SFTP_BROWSE_FOLDERS = 0x00000002,
        };
    public:
        SFTP(SSH::Ptr_t ssh);
        virtual ~SFTP();

        /**
         * @brief return the default download folder
         */
        static wxString GetDefaultDownloadFolder();
        
        /**
         * @brief return the underlying ssh session
         */
        inline SSH::Ptr_t GetSsh() const                { return m_ssh; }

        inline bool IsConnected() const                 { return m_connected; }

        inline void SetAccount(const wxString& account) { this->m_account = account; }
        inline const wxString& GetAccount() const       { return m_account; }
        /**
         * @brief intialize the scp over ssh
         */
        void Initialize() ;

        /**
         * @brief close the scp channel
         */
        void Close();

        /**
         * @brief write the content of local file into a remote file
         * @param localFile the local file
         * @param remotePath the remote path (abs path)
         */
        void Write(const wxFileName& localFile, const wxString& remotePath, SFTPAttribute::Ptr_t attributes = SFTPAttribute::Ptr_t(NULL)) ;

        /**
         * @brief write the content of 'fileContent' into the remote file represented by remotePath
         */
        void Write(const wxMemoryBuffer& fileContent, const wxString& remotePath, SFTPAttribute::Ptr_t attributes = SFTPAttribute::Ptr_t(NULL)) ;

        /**
         * @brief read remote file and return its content
         * @return the file content + the file attributes
         */
        SFTPAttribute::Ptr_t Read(const wxString& remotePath, wxMemoryBuffer& buffer) ;

        /**
         * @brief list the content of a folder
         * @param folder
         * @param foldersOnly
         * @param filter filter out files that do not match the filter
         * @throw Exception incase an error occurred
         */
        SFTPAttribute::List_t List(const wxString& folder, size_t flags, const wxString& filter = "") ;

        /**
         * @brief create a directory
         * @param dirname
         */
        void CreateDir(const wxString& dirname) ;

        /**
         * @brief create a file. This function also creates the path to the file (by calling internally to Mkpath)
         */
        void CreateRemoteFile(const wxString& remoteFullPath, const wxString& content, SFTPAttribute::Ptr_t attr) ;

        /**
         * @brief this version create a copy of the local file on the remote server. Similar to the previous
         * version, it also creates the path to the file if needed
         */
        void CreateRemoteFile(const wxString& remoteFullPath, const wxFileName& localFile, SFTPAttribute::Ptr_t attr) ;

        /**
         * @brief create path . If the directory does not exist, create it (all sub paths if needed)
         */
        void Mkpath(const wxString& remoteDirFullpath) ;

        /**
         * @brief Remove a directoy.
         * @param dirname
         */
        void RemoveDir(const wxString& dirname) ;

        /**
         * @brief Unlink (delete) a file.
         * @param dirname
         */
        void UnlinkFile(const wxString& path) ;

        /**
         * @brief Rename or move a file or directory
         * @param oldpath
         * @param newpath
         */
        void Rename(const wxString& oldpath, const wxString& newpath) ;

        /**
         * @brief cd up and list the content of the directory
         * @return
         */
        SFTPAttribute::List_t CdUp(size_t flags, const wxString& filter) ;

        /**
         * @brief stat the path
         */
        SFTPAttribute::Ptr_t Stat(const wxString& path) ;

        /**
         * @brief change remote file permissions
         */
        void Chmod(const wxString& remotePath, size_t permissions) ;

        /**
         * @brief return the current folder
         */
        inline const wxString& GetCurrentFolder() const { return m_currentFolder; }
    private:
        SSH::Ptr_t m_ssh;
        SFTPSession_t m_sftp;
        wxString m_currentFolder;
        wxString m_account;
        bool m_connected;
    };

    typedef void (wxEvtHandler::*clSFTPEventFunction)(SFTPEvent &);
    #define clSFTPEventHandler(func) wxEVENT_HANDLER_CAST(clSFTPEventFunction, func)

    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_SFTP_SAVE_FILE, SFTPEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_SFTP_RENAME_FILE, SFTPEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_SFTP_DELETE_FILE, SFTPEvent);
}
#endif // USE_SFTP
#endif // CLSCP_H
