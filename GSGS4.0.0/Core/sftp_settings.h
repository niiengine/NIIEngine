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
#ifndef SFTPSETTINGS_H
#define SFTPSETTINGS_H

#include "gsgsConfig.h"
#if USE_SFTP
#include "ssh_account_info.h"
using namespace gsgs;
class _gsgsAPI SFTPSettings : public ObjectData
{
public:
    SFTPSettings();
    virtual ~SFTPSettings();

    inline void SetAccounts(const SSHAccountInfo::Vect_t& accounts)    { this->m_accounts = accounts; }
    
    inline const SSHAccountInfo::Vect_t& GetAccounts() const           { return m_accounts; }

    /**
     * @brief update account details
     */
    bool UpdateAccount(const SSHAccountInfo& account);

    inline void SetSshClient(const wxString& sshClient)                { this->m_sshClient = sshClient; }
    
    inline const wxString& GetSshClient() const                        { return m_sshClient; }
    
    bool GetAccount(const wxString& name, SSHAccountInfo& account) const;
    
    SFTPSettings& Load();
    SFTPSettings& Save();

    virtual void FromJSON(const JSONObject& json);
    virtual JSONObject ToJSON() const;
private:
    void MSWImportPuTTYAccounts();
private:
    SSHAccountInfo::Vect_t m_accounts;
    wxString m_sshClient;
};
#endif

#endif // SFTPSETTINGS_H
