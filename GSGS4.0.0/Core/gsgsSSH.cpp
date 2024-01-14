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

#include "gsgsSSH.h"

#if USE_SFTP
#include <wx/textdlg.h>
#include <wx/translation.h>
#ifdef __WXMSW__
#include "wx/msw/winundef.h"
#endif

#include <libssh/libssh.h>
#include "gsgsThread.h"
namespace gsgs
{
    //-----------------------------------------------------------------------------------
    wxDEFINE_EVENT(wxEVT_SSH_COMMAND_OUTPUT, gsgs::Event);
    wxDEFINE_EVENT(wxEVT_SSH_COMMAND_COMPLETED, gsgs::Event);
    wxDEFINE_EVENT(wxEVT_SSH_COMMAND_ERROR, gsgs::Event);
    wxDEFINE_EVENT(wxEVT_SSH_CONNECTED, gsgs::Event);
    wxDEFINE_EVENT(wxEVT_SSH_CHANNEL_READ_ERROR, gsgs::Event);
    wxDEFINE_EVENT(wxEVT_SSH_CHANNEL_READ_OUTPUT, gsgs::Event);
    wxDEFINE_EVENT(wxEVT_SSH_CHANNEL_CLOSED, gsgs::Event);
    //-----------------------------------------------------------------------------------
    #if 0
    class SSHConnectThread : public wxThread
    {
        size_t m_seconds;
        SSHSession_t m_session;
        SSH* m_handler;

    public:
        SSHConnectThread(gsgs::SSH* handler, SSHSession_t session, size_t seconds)
            : wxThread(wxTHREAD_JOINABLE)
            , m_handler(handler)
            , m_session(session)
            , m_seconds(seconds)
        {
        }
        virtual ~SSHConnectThread() {}

        void* Entry()
        {
            size_t retries = m_seconds * 100;
            while(retries) {
                int rc = ssh_connect(m_session);
                if(rc == SSH_AGAIN) {
                    wxThread::Sleep(10);
                    --retries;
                    continue;
                }
                if(rc == SSH_OK) {
                    m_handler->CallAfter(&gsgs::SSH::OnConnectSuccess);
                    return NULL;
                } else {
                    break;
                }
            }
            m_handler->CallAfter(&gsgs::SSH::OnConnectError);
            return NULL;
        }
    };
    #endif
    //-----------------------------------------------------------------------------------
    class SSHChannelReader : public gsgs::Thread
    {
        wxEvtHandler* m_handler;
        SSHChannel_t m_channel;

    public:
        SSHChannelReader(wxEvtHandler* handler, SSHChannel_t channel)
            : m_handler(handler)
            , m_channel(channel)
        {
        }
        virtual ~SSHChannelReader() {}

        void* Entry()
        {
            while(!TestDestroy()) {
                // First, poll the channel
                int bytes = ssh_channel_poll_timeout(m_channel, 500, 0);
                if(bytes == SSH_ERROR) {
                    // an error
                    gsgs::Event event(wxEVT_SSH_CHANNEL_READ_ERROR);
                    m_handler->AddPendingEvent(event);
                    break;
                } else if(bytes == SSH_EOF) {
                    gsgs::Event event(wxEVT_SSH_CHANNEL_CLOSED);
                    m_handler->AddPendingEvent(event);
                    break;
                } else if(bytes == 0) {
                    continue;
                } else {
                    // there is something to read
                    char* buffer = new char[bytes + 1];
                    if(ssh_channel_read(m_channel, buffer, bytes, 0) != bytes) {
                        gsgs::Event event(wxEVT_SSH_CHANNEL_READ_ERROR);
                        m_handler->AddPendingEvent(event);
                        wxDELETEA(buffer);
                        break;
                    } else {
                        buffer[bytes] = 0;
                        gsgs::Event event(wxEVT_SSH_CHANNEL_READ_OUTPUT);
                        event.SetString(wxString(buffer, wxConvUTF8));
                        m_handler->AddPendingEvent(event);
                        wxDELETEA(buffer);
                    }
                }
            }
            return NULL;
        }
    };
    //-----------------------------------------------------------------------------------
    SSHChannel::SSHChannel(SSH::Ptr_t ssh)
        : m_ssh(ssh)
        , m_channel(nullptr)
        , m_readerThread(nullptr)
    {
    }
    //-----------------------------------------------------------------------------------
    SSHChannel::~SSHChannel() { Close(); }
    //-----------------------------------------------------------------------------------
    void SSHChannel::Open()
    {
        if(IsOpen()) { return; }
        if(!m_ssh) { throw gsgs::Exception("ssh session is not opened"); }
        m_channel = ssh_channel_new(m_ssh->GetSession());
        if(!m_channel) { throw gsgs::Exception(BuildError("Failed to allocte ssh channel")); }

        int rc = ssh_channel_open_session(m_channel);
        if(rc != SSH_OK) {
            ssh_channel_free(m_channel);
            m_channel = NULL;
            throw gsgs::Exception(BuildError("Failed to open ssh channel"));
        }
    }
    //-----------------------------------------------------------------------------------
    void SSHChannel::Close()
    {
        // Stop the worker thread
        wxDELETE(m_readerThread);
        
        if(IsOpen()) {
            ssh_channel_close(m_channel);
            ssh_channel_free(m_channel);
            m_channel = NULL;
        }
    }
    //-----------------------------------------------------------------------------------
    void SSHChannel::Execute(const wxString& command, wxEvtHandler* sink)
    {
        // Sanity
        if(m_readerThread) { throw gsgs::Exception("Channel is busy"); }
        if(!IsOpen()) { throw gsgs::Exception("Channel is not opened"); }
        int rc = ssh_channel_request_exec(m_channel, command.mb_str(wxConvUTF8).data());
        if(rc != SSH_OK) {
            Close();
            throw gsgs::Exception(BuildError("Execute failed"));
        }
        m_readerThread = new SSHChannelReader(sink, m_channel);
        m_readerThread->Start();
    }
    //-----------------------------------------------------------------------------------
    wxString SSHChannel::BuildError(const wxString& prefix) const
    {
        if(!m_ssh) { return prefix; }
        wxString errmsg = ssh_get_error(m_ssh->GetSession());
        return wxString() << prefix << ". " << errmsg;
    }
    //-----------------------------------------------------------------------------------
    SSH::SSH(const wxString& host, const wxString& user, const wxString& pass, int port)
        : m_host(host)
        , m_username(user)
        , m_password(pass)
        , m_port(port)
        , m_connected(false)
        , m_session(NULL)
        , m_channel(NULL)
        , m_timer(NULL)
        , m_owner(NULL)
    {
        m_timer = new wxTimer(this);
        Bind(wxEVT_TIMER, &SSH::OnCheckRemoteOutut, this, m_timer->GetId());
    }
    //-----------------------------------------------------------------------------------
    SSH::SSH()
        : m_port(22)
        , m_connected(false)
        , m_session(NULL)
        , m_channel(NULL)
        , m_timer(NULL)
        , m_owner(NULL)
    {
    }
    //-----------------------------------------------------------------------------------
    SSH::~SSH() { Close(); }
    //-----------------------------------------------------------------------------------
    void SSH::Connect(int seconds)
    {
        m_session = ssh_new();
        if(!m_session) { throw gsgs::Exception("ssh_new failed!"); }

        ssh_set_blocking(m_session, 0);
        int verbosity = SSH_LOG_NOLOG;
        ssh_options_set(m_session, SSH_OPTIONS_HOST, m_host.mb_str(wxConvUTF8).data());
        ssh_options_set(m_session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
        ssh_options_set(m_session, SSH_OPTIONS_PORT, &m_port);
        ssh_options_set(m_session, SSH_OPTIONS_USER, GetUsername().mb_str().data());

        // Connect the session
        int retries = seconds * 100;
        if(retries < 0) { retries = 1; }
        DoConnectWithRetries(retries);
        ssh_set_blocking(m_session, 1);
    }
    //-----------------------------------------------------------------------------------
    bool SSH::AuthenticateServer(wxString& message)
    {
        int state;
        unsigned char* hash = NULL;
        char* hexa = NULL;

        message.Clear();
        state = ssh_is_server_known(m_session);

    #if LIBSSH_VERSION_INT < SSH_VERSION_INT(0, 6, 1)
        int hlen = 0;
        hlen = ssh_get_pubkey_hash(m_session, &hash);
        if(hlen < 0) { throw gsgs::Exception("Unable to obtain server public key!"); }
    #else
        size_t hlen = 0;
        ssh_key key = NULL;
        ssh_get_publickey(m_session, &key);
        ssh_get_publickey_hash(key, SSH_PUBLICKEY_HASH_SHA1, &hash, &hlen);
        if(hlen == 0) { throw gsgs::Exception("Unable to obtain server public key!"); }
    #endif

        switch(state) {
        case SSH_SERVER_KNOWN_OK:
            free(hash);
            return true;

        case SSH_SERVER_KNOWN_CHANGED:
            hexa = ssh_get_hexa(hash, hlen);
            message << _("Host key for server changed: it is now:\n") << hexa << "\n" << _("Accept server authentication?");
            free(hexa);
            free(hash);
            return false;

        case SSH_SERVER_FOUND_OTHER:
            message << _("The host key for this server was not found but another type of key exists.\n")
                    << _("An attacker might change the default server key to confuse your client into thinking the key "
                         "does not exist\n")
                    << _("Accept server authentication?");
            free(hash);
            return false;

        case SSH_SERVER_FILE_NOT_FOUND:
            message << _("Could not find known host file.\n")
                    << _("If you accept the host key here, the file will be automatically created.\n");
        /* fallback to SSH_SERVER_NOT_KNOWN behavior */
        case SSH_SERVER_NOT_KNOWN:
            hexa = ssh_get_hexa(hash, hlen);
            message << _("The server is unknown. Do you trust the host key?\n") << _("Public key hash: ") << hexa << "\n"
                    << _("Accept server authentication?");
            free(hexa);
            free(hash);
            return false;

        default:
        case SSH_SERVER_ERROR:
            throw gsgs::Exception(wxString() << "An error occurred: " << ssh_get_error(m_session));
        }
        return false;
    }
    //-----------------------------------------------------------------------------------
    void SSH::AcceptServerAuthentication()
    {
        if(!m_session) { throw gsgs::Exception("NULL SSH session"); }
        ssh_write_knownhost(m_session);
    }
    //-----------------------------------------------------------------------------------
    #define THROW_OR_FALSE(msg)                  \
        if(throwExc) { throw gsgs::Exception(msg); } \
        return false;

    bool SSH::LoginPassword(bool throwExc)
    {
        if(!m_session) { THROW_OR_FALSE("NULL SSH session"); }

        int rc;
        // interactive keyboard method failed, try another method
        rc = ssh_userauth_password(m_session, NULL, GetPassword().mb_str().data());
        if(rc == SSH_AUTH_SUCCESS) {
            return true;

        } else if(rc == SSH_AUTH_DENIED) {
            THROW_OR_FALSE("Login failed: invalid username/password");

        } else {
            THROW_OR_FALSE(wxString() << _("Authentication error: ") << ssh_get_error(m_session));
        }
        return false;
    }
    //-----------------------------------------------------------------------------------
    bool SSH::LoginInteractiveKBD(bool throwExc)
    {
        if(!m_session) { THROW_OR_FALSE("NULL SSH session"); }

        int rc;
        rc = ssh_userauth_kbdint(m_session, NULL, NULL);
        if(rc == SSH_AUTH_INFO) {
            while(rc == SSH_AUTH_INFO) {
                const char *name, *instruction;
                int nprompts, iprompt;
                name = ssh_userauth_kbdint_getname(m_session);
                instruction = ssh_userauth_kbdint_getinstruction(m_session);
                nprompts = ssh_userauth_kbdint_getnprompts(m_session);
                wxUnusedVar(name);
                wxUnusedVar(instruction);
                for(iprompt = 0; iprompt < nprompts; iprompt++) {
                    const char* prompt;
                    char echo;
                    prompt = ssh_userauth_kbdint_getprompt(m_session, iprompt, &echo);
                    if(echo) {
                        wxString answer = ::wxGetTextFromUser(prompt, "SSH");
                        if(answer.IsEmpty()) { THROW_OR_FALSE(wxString() << "Login error: " << ssh_get_error(m_session)); }
                        if(ssh_userauth_kbdint_setanswer(m_session, iprompt, answer.mb_str(wxConvUTF8).data()) < 0) {
                            THROW_OR_FALSE(wxString() << "Login error: " << ssh_get_error(m_session));
                        }
                    } else {
                        if(ssh_userauth_kbdint_setanswer(m_session, iprompt, GetPassword().mb_str(wxConvUTF8).data()) < 0) {
                            THROW_OR_FALSE(wxString() << "Login error: " << ssh_get_error(m_session));
                        }
                    }
                }
                rc = ssh_userauth_kbdint(m_session, NULL, NULL);
            }
            return true; // success
        }
        THROW_OR_FALSE("Interactive Keyboard is not enabled for this server");
        return false;
    }
    //-----------------------------------------------------------------------------------
    bool SSH::LoginPublicKey(bool throwExc)
    {
        if(!m_session) { THROW_OR_FALSE("NULL SSH session"); }

        int rc;
        rc = ssh_userauth_autopubkey(m_session, NULL);
        if(rc != SSH_AUTH_SUCCESS) { THROW_OR_FALSE(wxString() << _("Public Key error: ") << ssh_get_error(m_session)); }
        return true;
    }
    //-----------------------------------------------------------------------------------
    void SSH::Close()
    {
        m_timer->Stop();
        Unbind(wxEVT_TIMER, &SSH::OnCheckRemoteOutut, this, m_timer->GetId());
        wxDELETE(m_timer);

        DoCloseChannel();

        if(m_session && m_connected) { ssh_disconnect(m_session); }

        if(m_session) { ssh_free(m_session); }

        m_connected = false;
        m_session = NULL;
        m_channel = NULL;
    }
    //-----------------------------------------------------------------------------------
    void SSH::Login()
    {
        int rc;

        rc = ssh_userauth_none(m_session, NULL);
        if(rc == SSH_AUTH_SUCCESS) { return; }

        // Try the following 3 methods in order
        // if non succeeded, this function will throw an exception

        try {
            LoginPublicKey();

        } catch(gsgs::Exception& e) {
            try {
                LoginPassword();

            } catch(gsgs::Exception& e2) {
                LoginInteractiveKBD();
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void SSH::ExecuteShellCommand(wxEvtHandler* owner, const wxString& command)
    {
        DoOpenChannel();

        m_owner = owner;
        if(!m_owner) { throw gsgs::Exception(wxString() << "No owner specified for output"); }

        wxCharBuffer buffer = command.mb_str(wxConvUTF8);
        int rc = ssh_channel_write(m_channel, buffer.data(), buffer.length());
        if(rc != (int)buffer.length()) { throw gsgs::Exception("SSH Socket error"); }

        // Start a timer to check for the output on 10ms intervals
        if(!m_timer->IsRunning()) { m_timer->Start(50); }
    }
    //-----------------------------------------------------------------------------------
    void SSH::OnCheckRemoteOutut(wxTimerEvent& event)
    {
        if(!m_channel) return;

        char buffer[1024];
        int nbytes = ssh_channel_read_nonblocking(m_channel, buffer, sizeof(buffer), 0);
        if(nbytes > 0) {
            wxString strOutput = wxString::FromUTF8((const char*)buffer, nbytes);
            gsgs::Event sshEvent(wxEVT_SSH_COMMAND_OUTPUT);
            sshEvent.SetString(strOutput);
            m_owner->AddPendingEvent(sshEvent);

        } else if(nbytes == SSH_ERROR) {
            m_timer->Stop();
            DoCloseChannel();
            gsgs::Event sshEvent(wxEVT_SSH_COMMAND_ERROR);
            sshEvent.SetString(ssh_get_error(m_session));
            m_owner->AddPendingEvent(sshEvent);

        } else {
            // nbytes == 0
            if(ssh_channel_is_eof(m_channel)) {
                m_timer->Stop();
                DoCloseChannel();
                // EOF was sent, nothing more to read
                gsgs::Event sshEvent(wxEVT_SSH_COMMAND_COMPLETED);
                m_owner->AddPendingEvent(sshEvent);
            } else {
                // Nothing to read, no error
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void SSH::DoCloseChannel()
    {
        // Properly close the channel
        if(m_channel) {
            ssh_channel_close(m_channel);
            ssh_channel_send_eof(m_channel);
            ssh_channel_free(m_channel);
        }
        m_channel = NULL;
    }
    //-----------------------------------------------------------------------------------
    void SSH::DoOpenChannel()
    {
        if(m_channel) return;

        m_channel = ssh_channel_new(m_session);
        if(!m_channel) { throw gsgs::Exception(ssh_get_error(m_session)); }

        int rc = ssh_channel_open_session(m_channel);
        if(rc != SSH_OK) { throw gsgs::Exception(ssh_get_error(m_session)); }

        rc = ssh_channel_request_pty(m_channel);
        if(rc != SSH_OK) { throw gsgs::Exception(ssh_get_error(m_session)); }

        rc = ssh_channel_change_pty_size(m_channel, 80, 24);
        if(rc != SSH_OK) { throw gsgs::Exception(ssh_get_error(m_session)); }

        rc = ssh_channel_request_shell(m_channel);
        if(rc != SSH_OK) { throw gsgs::Exception(ssh_get_error(m_session)); }
    }
    //-----------------------------------------------------------------------------------
    void SSH::DoConnectWithRetries(int retries)
    {
        while(retries) {
            int rc = ssh_connect(m_session);
            if(rc == SSH_AGAIN) {
                if(wxThread::IsMain()) { ::wxSafeYield(); }
                wxThread::Sleep(10);
                --retries;
                continue;
            }
            if(rc == SSH_OK) {
                m_connected = true;
                return;
            } else {
                throw gsgs::Exception(ssh_get_error(m_session));
            }
        }
        throw gsgs::Exception("Connect timeout");
    }
    //-----------------------------------------------------------------------------------
}
#endif