

#include "gsgsFileUtil.h"
#include "sftp_settings.h"
#ifdef __WXMSW__
#include <wx/msw/registry.h>
#include <wx/volume.h>
#endif

#if USE_SFTP

SFTPSettings::SFTPSettings()
    : ObjectData("sftp-settings")
#ifdef __WXMSW__
    , m_sshClient("putty")
#else
    , m_sshClient("ssh")
#endif
{
}

SFTPSettings::~SFTPSettings() {}

void SFTPSettings::FromJSON(const JSONObject& json)
{
    m_accounts.clear();
    json.getObject("sshClient").getValue(m_sshClient, m_sshClient);
    JSONObject arrAccounts = json.getObject("accounts");
    int size = arrAccounts.getArraySize();
    for(int i = 0; i < size; ++i) {
        SSHAccountInfo account;
        account.FromJSON(arrAccounts.getArray(i));
        m_accounts.push_back(account);
    }
}

JSONObject SFTPSettings::ToJSON() const
{
    JSONObject element = JSONObject::createObject(GetName());
    element.addProperty("sshClient", m_sshClient);
    JSONObject arrAccounts = JSONObject::createArray("accounts");
    element.addProperty(arrAccounts);
    for(size_t i = 0; i < m_accounts.size(); ++i) {
        arrAccounts.addProperty(m_accounts.at(i).ToJSON());
    }
    return element;
}

SFTPSettings& SFTPSettings::Load()
{
    gsgs::Config config("sftp-settings.conf");
    config.ReadItem(this);
    MSWImportPuTTYAccounts();
    return *this;
}

SFTPSettings& SFTPSettings::Save()
{
    gsgs::Config config("sftp-settings.conf");
    config.WriteItem(this);
    return *this;
}

bool SFTPSettings::GetAccount(const wxString& name, SSHAccountInfo& account) const
{
    for(size_t i = 0; i < m_accounts.size(); ++i) {
        const SSHAccountInfo& currentAccount = m_accounts.at(i);
        if(name == currentAccount.GetAccountName()) {
            account = currentAccount;
            return true;
        }
    }
    return false;
}

bool SFTPSettings::UpdateAccount(const SSHAccountInfo& account)
{
    for(size_t i = 0; i < m_accounts.size(); ++i) {
        SSHAccountInfo& currentAccount = m_accounts.at(i);
        if(account.GetAccountName() == currentAccount.GetAccountName()) {
            currentAccount = account;
            return true;
        }
    }
    return false;
}

void SFTPSettings::MSWImportPuTTYAccounts()
{
#ifdef __WXMSW__
    SSHAccountInfo::Vect_t puttyAccounts;
    {
        // 64 bit version
        // HKEY_CURRENT_USER\Software\SimonTatham\\PuTTY\Sessions
        wxRegKey regPutty(wxRegKey::HKCU, "Software\\SimonTatham\\PuTTY\\Sessions");
        if(regPutty.Exists()) {
            long index = wxNOT_FOUND;
            wxString strKeyName;
            bool res = regPutty.GetFirstKey(strKeyName, index);
            while(res) {
                wxString hostname;
                wxString username;
                long port = wxNOT_FOUND;
                wxRegKey rk(wxRegKey::HKCU, "Software\\SimonTatham\\PuTTY\\Sessions\\" + strKeyName);
                if(rk.Exists()) {
                    rk.QueryValue("HostName", hostname);
                    int where = hostname.Find('@');
                    if(where != wxNOT_FOUND) {
                        hostname = hostname.Mid(where + 1);
                        username = hostname.Mid(where);
                    }
                    rk.QueryValue("PortNumber", &port);
                    if(!hostname.IsEmpty() && port != wxNOT_FOUND) {
                        SSHAccountInfo acc;
                        acc.SetAccountName(FileUtil::DecodeURI(strKeyName));
                        acc.SetHost(hostname);
                        acc.SetPort(port);
                        acc.SetUsername(username);
                        puttyAccounts.push_back(acc);
                    }
                }
                res = regPutty.GetNextKey(strKeyName, index);
            }
        }
    }

    {
        // 32 bit version
        // HKEY_CURRENT_USER\Software\SimonTatham\\PuTTY\Sessions
        wxRegKey regPutty(wxRegKey::HKCU, "Software\\Wow6432Node\\SimonTatham\\PuTTY\\Sessions");
        if(regPutty.Exists()) {
            long index = wxNOT_FOUND;
            wxString strKeyName;
            bool res = regPutty.GetFirstKey(strKeyName, index);
            while(res) {
                wxString hostname;
                wxString username;
                long port = wxNOT_FOUND;
                wxRegKey rk(wxRegKey::HKCU, "Software\\Wow6432Node\\SimonTatham\\PuTTY\\Sessions\\" + strKeyName);
                if(rk.Exists()) {
                    rk.QueryValue("HostName", hostname);
                    int where = hostname.Find('@');
                    if(where != wxNOT_FOUND) {
                        hostname = hostname.Mid(where + 1);
                        username = hostname.Mid(where);
                    }
                    rk.QueryValue("PortNumber", &port);
                    if(!hostname.IsEmpty() && port != wxNOT_FOUND) {
                        SSHAccountInfo acc;
                        acc.SetAccountName(FileUtil::DecodeURI(strKeyName));
                        acc.SetHost(hostname);
                        acc.SetPort(port);
                        acc.SetUsername(username);
                        puttyAccounts.push_back(acc);
                    }
                }
                res = regPutty.GetNextKey(strKeyName, index);
            }
        }
    }

    std::for_each(puttyAccounts.begin(), puttyAccounts.end(), [&](const SSHAccountInfo& acc) {
        SSHAccountInfo::Vect_t::iterator iter =
            std::find_if(m_accounts.begin(), m_accounts.end(),
                         [&](const SSHAccountInfo& a) { return a.GetAccountName() == acc.GetAccountName(); });
        if(iter == m_accounts.end()) {
            // No such account
            // add it
            m_accounts.push_back(acc);
        }
    });
#endif
}
#endif // USE_SFTP
