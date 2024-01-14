#include "SFTPSessionInfo.h"

#if USE_SFTP
#include "gsgsLogManager.h"

SFTPSessionInfo::SFTPSessionInfo() {}

SFTPSessionInfo::~SFTPSessionInfo() {}

void SFTPSessionInfo::FromJSON(const JSONObject& json)
{
    m_files.clear();
    json.getObject("name").getValue(m_account);
    json.getObject("rootFolder").getValue(m_rootFolder);
    wxArrayString files;
    json.getObject("files").getValue(files);
    std::for_each(files.begin(), files.end(), [&](const wxString& file) { m_files.push_back(file); });
}

JSONObject SFTPSessionInfo::ToJSON() const
{
    JSONObject json = JSONObject::createObject();
    wxArrayString files;
    std::for_each(m_files.begin(), m_files.end(), [&](const wxString& file) { files.Add(file); });
    json.addProperty("name", m_account);
    json.addProperty("rootFolder", m_rootFolder);
    json.addProperty("files", files);
    return json;
}

void SFTPSessionInfo::Clear()
{
    m_files.clear();
    m_account.Clear();
    m_rootFolder.Clear();
}

bool SFTPSessionInfo::IsOk() const { return !m_account.IsEmpty(); }

void SFTPSessionInfo::RemoveFile(const wxString& path)
{
    std::vector<wxString>::iterator iter =
        std::find_if(m_files.begin(), m_files.end(), [&](const wxString& file) { return path == file; });
    if(iter != m_files.end()) {
        gsgsDEBUG() << "SFTP session: removing file:" << path << "from session:" << GetAccount();
        m_files.erase(iter);
    }
}

void SFTPSessionInfo::AddFile(const wxString& path)
{
    std::vector<wxString>::iterator iter =
        std::find_if(m_files.begin(), m_files.end(), [&](const wxString& file) { return path == file; });
    if(iter == m_files.end()) {
        gsgsDEBUG() << "SFTP session: adding file:" << path << "to session:" << GetAccount();
        m_files.push_back(path);
    }
}

//-------------------------------------------------------------
// SFTPSessionInfoList
//-------------------------------------------------------------
SFTPSessionInfoList::SFTPSessionInfoList()
    : ObjectData("sessions")
{
}
SFTPSessionInfoList::~SFTPSessionInfoList() {}

void SFTPSessionInfoList::FromJSON(const JSONObject& json)
{
    m_sessions.clear();
    int arrSize = json.getArraySize();
    for(int i = 0; i < arrSize; ++i) {
        JSONObject o = json.getArray(i);
        SFTPSessionInfo sess;
        sess.FromJSON(o);
        m_sessions[sess.GetAccount()] = sess;
    }
}

JSONObject SFTPSessionInfoList::ToJSON() const
{
    JSONObject json = JSONObject::createArray(GetName());
    std::for_each(m_sessions.begin(), m_sessions.end(), [&](const SFTPSessionInfo::Map_t::value_type& vt) {
        const SFTPSessionInfo& sess = vt.second;
        json.arrayAppend(sess.ToJSON());
    });
    return json;
}

SFTPSessionInfoList& SFTPSessionInfoList::Load()
{
    gsgs::Config conf("sftp-sessions.conf");
    conf.ReadItem(this);
    return *this;
}

SFTPSessionInfoList& SFTPSessionInfoList::Save()
{
    gsgs::Config conf("sftp-sessions.conf");
    conf.WriteItem(this);
    return *this;
}

SFTPSessionInfo& SFTPSessionInfoList::GetSession(const wxString& accountName)
{
    SFTPSessionInfo::Map_t::iterator iter = m_sessions.find(accountName);
    if(iter == m_sessions.end()) {
        static SFTPSessionInfo NullSession;
        return NullSession;
    }
    return iter->second;
}

SFTPSessionInfoList& SFTPSessionInfoList::SetSession(const SFTPSessionInfo& sess)
{
    if(sess.GetAccount().IsEmpty()) { return *this; }
    if(m_sessions.count(sess.GetAccount())) { m_sessions.erase(sess.GetAccount()); }
    m_sessions[sess.GetAccount()] = sess;
    return *this;
}

#endif
