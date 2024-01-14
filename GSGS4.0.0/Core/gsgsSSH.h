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

#ifndef gsgsSSH_H
#define gsgsSSH_H

#if USE_SFTP

#include "gsgsPreInclude.h"
#include "gsgsException.h"
#include "gsgsEvent.h"
#include <wx/timer.h>
#include <errno.h>

// We do it this way to avoid exposing the include to ssh/libssh.h to files including this header
struct ssh_session_struct;
struct ssh_channel_struct;
typedef struct ssh_session_struct* SSHSession_t;
typedef struct ssh_channel_struct* SSHChannel_t;

namespace gsgs
{
    // Sent when a remote command over ssh has an output
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_SSH_COMMAND_OUTPUT, gsgs::Event);
    // Sent when a remote command over ssh has completed
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_SSH_COMMAND_COMPLETED, gsgs::Event);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_SSH_COMMAND_ERROR, gsgs::Event);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_SSH_CONNECTED, gsgs::Event);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_SSH_CHANNEL_READ_ERROR, gsgs::Event);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_SSH_CHANNEL_READ_OUTPUT, gsgs::Event);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_SSH_CHANNEL_CLOSED, gsgs::Event);

    class _gsgsAPI SSH : public wxEvtHandler
    {
    public:
        typedef wxSharedPtr<SSH> Ptr_t;
    public:
        SSH(const wxString& host, const wxString & user, const wxString& pass, int port = 22);
        SSH();
        virtual ~SSH();

        inline bool IsConnected() const            { return m_connected; }
        inline bool IsCommandRunning() const       { return m_channel != NULL; }
        
        /**
         * @brief connect to the remote server
         */
        void Connect(int seconds = 10) ;

        /**
         * @brief authenticate the server
         * @param [output] message in case the authentication failed, prompt the user with the message
         * @return true if the server could be authenticated, otherwise return false.
         * In case an error occurs, throw a gsgs::Exception
         */
        bool AuthenticateServer(wxString& message) ;

        /**
         * @brief accepts the server authentication and add it to the "known_hosts"
         */
        void AcceptServerAuthentication() ;

        /**
         * @brief login to the server with the user credentials
         * @return true if we managed to login
         * @throw gsgs::Exception incase something really bad happened
         */
        bool LoginPassword(bool throwExc = true) ;

        /**
         * @brief login using public key
         * @return true if we managed to login
         * @throw gsgs::Exception incase something really bad happened
         */
        bool LoginPublicKey(bool throwExc = true) ;

        /**
         * @brief login using interactive-keyboard method
         * @return true if we managed to login
         * @throw gsgs::Exception incase something really bad happened
         */
        bool LoginInteractiveKBD(bool throwExc = true) ;

        /**
         * @brief try to login using all the methods we support (interactive-kbd, user/pass and public key)
         */
        void Login() ;

        /**
         * @brief close the SSH session
         * IMPORTANT: this will invalidate all other channels (like: scp)
         */
        void Close();

        /**
         * @brief execute a remote command and return the output. open the shell if no is opened
         */
        void ExecuteShellCommand(wxEvtHandler* owner, const wxString& command) ;

        SSHSession_t GetSession()                           { return m_session; }

        inline void SetPassword(const wxString& password)   { this->m_password = password; }
        inline void SetPort(int port)                       { this->m_port = port; }

        inline const wxString& GetPassword() const          { return m_password; }
        inline int GetPort() const                          { return m_port; }
        inline void SetHost(const wxString& host)           { this->m_host = host; }
        inline void SetUsername(const wxString& username)   { this->m_username = username; }
        inline const wxString& GetHost() const              { return m_host; }
        inline const wxString& GetUsername() const          { return m_username; }
    protected:
        void OnCheckRemoteOutut(wxTimerEvent& event);
        void DoCloseChannel();
        void DoOpenChannel()  ;
        void DoConnectWithRetries(int retries) ;
    protected:
        wxString m_host;
        wxString m_username;
        wxString m_password;
        int m_port;
        SSHSession_t m_session;
        SSHChannel_t m_channel;
        wxTimer* m_timer;
        wxEvtHandler* m_owner;
        bool m_connected;
    };

    class _gsgsAPI SSHChannel
    {
    public:
        typedef wxSharedPtr<SSHChannel> Ptr_t;
    public:
        SSHChannel(SSH::Ptr_t ssh);
        virtual ~SSHChannel();

        /**
         * @brief read all data from the channel
         * @param output
         * @param timeout_ms
         * @return true on success, false on timeout
         * @throw gsgs::Exception on error
         */
        bool ReadAll(wxString& output, int timeout_ms = -1);

        /**
         * @brief is the channel opened?
         */
        inline bool IsOpen() const                 { return m_channel != nullptr; }

        /**
         * @brief Open the channel
         * @throw gsgs::Exception
         */
        void Open();

        /**
         * @brief close the channel
         */
        void Close();

        /**
         * @brief execute remote command
         * The reply will be returned to 'sink' object in form of events
         */
        void Execute(const wxString& command, wxEvtHandler* sink);
    protected:
        wxString BuildError(const wxString& prefix) const;
        /**
         * @brief read data from the channel.
         * @return true on success, false on timeout
         * @throw gsgs::Exception on error
         */
        bool DoRead(wxString& output, int timeout_ms = -1);
    private:
        SSH::Ptr_t m_ssh;
        SSHChannel_t m_channel;
        Thread* m_readerThread;
    };
}
#endif
#endif