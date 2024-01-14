
#ifndef PHPCONFIGURATIONDATA_H
#define PHPCONFIGURATIONDATA_H

#include "gsgsConfig.h"

class PHPConfigurationData : public gsgs::ObjectData
{
public:
    enum {
        kDontPromptForMissingFileMapping = (1 << 0),
    };

public:
    virtual void FromJSON(const gsgs::JSONObject& json);
    virtual gsgs::JSONObject ToJSON() const;
    PHPConfigurationData();

    PHPConfigurationData& Load();
    void Save();

    virtual ~PHPConfigurationData();

    PHPConfigurationData& SetWorkspaceType(int workspaceType)
    {
        this->m_workspaceType = workspaceType;
        return *this;
    }

    int GetWorkspaceType() const { return m_workspaceType; }
    PHPConfigurationData& EnableFlag(size_t flag, bool b)
    {
        if(b) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
        return *this;
    }

    bool HasFlag(size_t flag) const { return m_flags & flag; }

    PHPConfigurationData& SetFindInFilesMask(const wxString& findInFilesMask)
    {
        this->m_findInFilesMask = findInFilesMask;
        return *this;
    }

    const wxString& GetFindInFilesMask() const { return m_findInFilesMask; }
    // ----------------------------------------------------
    // Setters
    // ----------------------------------------------------
    PHPConfigurationData& SetCcIncludePath(const wxArrayString& ccIncludePath)
    {
        this->m_ccIncludePath = ccIncludePath;
        return *this;
    }
    PHPConfigurationData& SetErrorReporting(const wxString& errorReporting)
    {
        m_phpOptions.SetErrorReporting(errorReporting);
        return *this;
    }
    PHPConfigurationData& SetIncludePaths(const wxArrayString& includePaths)
    {
        m_phpOptions.SetIncludePaths(includePaths);
        return *this;
    }
    PHPConfigurationData& SetPhpExe(const wxString& phpExe)
    {
        m_phpOptions.SetPhpExe(phpExe);
        return *this;
    }
    PHPConfigurationData& SetXdebugPort(size_t xdebugPort)
    {
        this->m_xdebugPort = xdebugPort;
        return *this;
    }
    PHPConfigurationData& SetFlags(size_t flags)
    {
        this->m_flags = flags;
        return *this;
    }
    PHPConfigurationData& SetXdebugIdeKey(const wxString& xdebugIdeKey)
    {
        this->m_xdebugIdeKey = xdebugIdeKey;
        return *this;
    }

    // ----------------------------------------------------
    // Getters
    // ----------------------------------------------------

    const wxArrayString& GetCcIncludePath() const { return m_ccIncludePath; }
    wxArrayString& GetCcIncludePath() { return m_ccIncludePath; }

    const wxString& GetErrorReporting() const { return m_phpOptions.GetErrorReporting(); }
    const wxArrayString& GetIncludePaths() const { return m_phpOptions.GetIncludePaths(); }
    wxString GetIncludePathsAsString() const;
    wxString GetCCIncludePathsAsString() const;
    const wxString& GetPhpExe() const { return m_phpOptions.GetPhpExe(); }
    size_t GetXdebugPort() const { return m_xdebugPort; }

    size_t GetFlags() const { return m_flags; }
    const wxString& GetXdebugIdeKey() const { return m_xdebugIdeKey; }
    PHPConfigurationData& SetXdebugHost(const wxString& xdebugHost)
    {
        this->m_xdebugHost = xdebugHost;
        return *this;
    }
    const wxString& GetXdebugHost() const { return m_xdebugHost; }
    PHPConfigurationData& SetSettersGettersFlags(size_t settersGettersFlags)
    {
        this->m_settersGettersFlags = settersGettersFlags;
        return *this;
    }
    size_t GetSettersGettersFlags() const { return m_settersGettersFlags; }
protected:
    size_t m_xdebugPort;
    wxArrayString m_ccIncludePath;
    size_t m_flags;
    wxString m_xdebugIdeKey;
    wxString m_xdebugHost;
    wxString m_findInFilesMask;
    int m_workspaceType;
    size_t m_settersGettersFlags;
    gsgs::PhpOptions m_phpOptions;
};

#endif // PHPCONFIGURATIONDATA_H
