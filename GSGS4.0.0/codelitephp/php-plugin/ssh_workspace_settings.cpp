#include "ssh_workspace_settings.h"
#include "php_workspace.h"

SSHWorkspaceSettings::SSHWorkspaceSettings()
    : ObjectData("sftp")
    , m_remoteUploadEnabled(true)
{
}

SSHWorkspaceSettings::~SSHWorkspaceSettings()
{
}

void SSHWorkspaceSettings::FromJSON(const gsgs::JSONObject& json)
{
    json.getObject("m_account").getValue(m_account);
    json.getObject("m_remoteFolder").getValue(m_remoteFolder);
    json.getObject("m_remoteUploadEnabled").getValue(m_remoteUploadEnabled);
}

gsgs::JSONObject SSHWorkspaceSettings::ToJSON() const
{
    gsgs::JSONObject json = JSONObject::createObject(GetName());
    json.addProperty("m_account", m_account);
    json.addProperty("m_remoteFolder", m_remoteFolder);
    json.addProperty("m_remoteUploadEnabled", m_remoteUploadEnabled);
    return json;
}

void SSHWorkspaceSettings::Load()
{
    wxFileName fn(PHPWorkspace::Get()->GetPrivateFolder(), "php-sftp.conf" );
    gsgs::Config conf(fn.GetFullPath());
    conf.ReadItem( this );
}

void SSHWorkspaceSettings::Save()
{
    wxFileName fn(PHPWorkspace::Get()->GetPrivateFolder(), "php-sftp.conf" );
    gsgs::Config conf(fn.GetFullPath());
    conf.WriteItem( this );
}

bool SSHWorkspaceSettings::IsRemoteUploadSet() const
{
    return !m_remoteFolder.IsEmpty() && !m_account.IsEmpty();
}

void SSHWorkspaceSettings::Reset()
{
    m_remoteFolder.clear();
    m_account.clear();
    m_remoteUploadEnabled = false;
}
