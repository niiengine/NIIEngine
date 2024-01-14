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

#include "ssh_account_info.h"
#include "gsgsPathManager.h"
#include "gsgsSFTP.h"

#if USE_SFTP

namespace gsgs
{
    SSHAccountInfo::SSHAccountInfo()
        : ObjectData("ssh-account")
        , m_port(22)
    {
    }

    SSHAccountInfo::~SSHAccountInfo() {}

    SSHAccountInfo& SSHAccountInfo::operator=(const SSHAccountInfo& other)
    {
        if (&other == this) return *this;

        m_accountName = other.m_accountName;
        m_username = other.m_username;
        m_password = other.m_password;
        m_port = other.m_port;
        m_host = other.m_host;
        m_bookmarks = other.m_bookmarks;
        m_defaultFolder = other.m_defaultFolder;
        return *this;
    }

    void SSHAccountInfo::FromJSON(const JSONObject& json)
    {
        json.getObject("m_accountName").getValue(m_accountName);
        json.getObject("m_username").getValue(m_username);
        wxString pass;
        json.getObject("m_password").getValue(pass);
        XORString x(pass);
        m_password = x.Decrypt();
        json.getObject("m_port").getValue(m_port, 22);
        json.getObject("m_host").getValue(m_host);
        json.getObject("m_bookmarks").getValue(m_bookmarks);
        json.getObject("m_defaultFolder").getValue(m_defaultFolder);
    }

    JSONObject SSHAccountInfo::ToJSON() const
    {
        JSONObject element = JSONObject::createObject(GetName());
        element.addProperty("m_accountName", m_accountName);
        element.addProperty("m_username", m_username);
        element.addProperty("m_port", m_port);
        element.addProperty("m_host", m_host);
        element.addProperty("m_bookmarks", m_bookmarks);
        element.addProperty("m_defaultFolder", m_defaultFolder);
        XORString x(m_password);
        element.addProperty("m_password", x.Encrypt());
        return element;
    }

    void SSHAccountInfo::AddBookmark(const wxString& location)
    {
        if (m_bookmarks.Index(location) == wxNOT_FOUND) {
            m_bookmarks.Add(location);
        }
    }

    SFTPBookmark::SFTPBookmark()
    {
    }

    SFTPBookmark::~SFTPBookmark()
    {
    }

    void SFTPBookmark::FromJSON(const JSONObject& json)
    {
        m_account.FromJSON(json.getObject("m_account"));
        m_name = json.getObject("m_name").getString();
        m_folder = json.getObject("m_folder").getString();
    }

    JSONObject SFTPBookmark::ToJSON() const
    {
        JSONObject json = JSONObject::createObject();
        json.addProperty("m_account", m_account.ToJSON());
        json.addProperty("m_name", m_name);
        json.addProperty("m_folder", m_folder);
        return json;
    }

    RemoteFileInfo::RemoteFileInfo()
        : m_premissions(0)
    {
    }

    RemoteFileInfo::~RemoteFileInfo() {}

    void RemoteFileInfo::SetRemoteFile(const wxString& remoteFile)
    {
        this->m_remoteFile = remoteFile;

        // Generate a temporary file location
        wxFileName fnRemoteFile(m_remoteFile);
        wxFileName localFile(SFTP::GetDefaultDownloadFolder(), fnRemoteFile.GetFullName());
        const wxArrayString& dirs = fnRemoteFile.GetDirs();
        for (size_t i = 0; i < dirs.GetCount(); ++i) {
            localFile.AppendDir(dirs.Item(i));
        }

        // Ensure that the path to the sftp temp folder exists
        localFile.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        m_localFile = localFile.GetFullPath();
    }

    wxString RemoteFileInfo::GetTempFolder() { return SFTP::GetDefaultDownloadFolder(); }
}
#endif // USE_SFTP
