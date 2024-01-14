
#ifndef WEBTOOLSCONFIG_H
#define WEBTOOLSCONFIG_H

#include "gsgsConfig.h"

class WebToolsConfig : public gsgs::ObjectData
{
public:
    virtual void FromJSON(const gsgs::JSONObject& json);
    virtual gsgs::JSONObject ToJSON() const;

    enum eJSFlags {
        kJSEnableCC = (1 << 0),
        kJSEnableVerboseLogging = (1 << 1),
        kJSLibraryJQuery = (1 << 2),
        kJSLibraryBrowser = (1 << 3),
        kJSLibraryEcma5 = (1 << 4),
        kJSLibraryEcma6 = (1 << 5),
        kJSLibraryChai = (1 << 6),
        kJSLibraryUnderscore = (1 << 7),
        kJSPluginNode = (1 << 8),
        kJSPluginStrings = (1 << 9),
        kJSPluginAngular = (1 << 10),
        kJSPluginQML = (1 << 11),
        kJSPluginRequireJS = (1 << 12),
        kJSWebPack = (1 << 13),
        kJSNodeExpress = (1 << 14),
    };

    enum eNodeJSFlags {
        kLintOnSave = (1 << 0),
    };

    enum eHtmlFlags {
        kHtmlEnableCC = (1 << 0),
    };

    enum eXmlFlags {
        kXmlEnableCC = (1 << 0),
    };
public:
    virtual ~WebToolsConfig();

    void SetPortNumber(int portNumber) { this->m_portNumber = portNumber; }
    int GetPortNumber() const { return m_portNumber; }
    
    static WebToolsConfig& Get();
    bool HasJavaScriptFlag(eJSFlags flag) const { return HasFlag(m_jsFlags, flag); }
    void EnableJavaScriptFlag(eJSFlags flag, bool b) { EnableFlag(m_jsFlags, flag, b); }

    bool HasXmlFlag(eXmlFlags flag) const { return HasFlag(m_xmlFlags, flag); }
    void EnableXmlFlag(eXmlFlags flag, bool b) { EnableFlag(m_xmlFlags, flag, b); }

    bool HasHtmlFlag(eHtmlFlags flag) const { return HasFlag(m_htmlFlags, flag); }
    void EnableHtmlFlag(eHtmlFlags flag, bool b) { EnableFlag(m_htmlFlags, flag, b); }

    void SetNodejs(const wxString& nodejs) { this->m_nodejs = nodejs; }
    void SetNpm(const wxString& npm) { this->m_npm = npm; }
    const wxString& GetNodejs() const { return m_nodejs; }
    const wxString& GetNpm() const { return m_npm; }

    bool IsLintOnSave() const { return HasFlag(m_nodeOptions, kLintOnSave); }
    void SetLintOnSave(bool b) { EnableFlag(m_nodeOptions, kLintOnSave, b); }
    
    bool IsNodeInstalled() const;
    bool IsNpmInstalled() const;
    
    wxFileName GetTernScript() const;
    bool IsTernInstalled() const;
    wxString GetTempFolder(bool create = false) const;
    
    /**
     * @brief create tern project file content based on the settings
     */
    wxString GetTernProjectFile() const;
    WebToolsConfig& Load();
    WebToolsConfig& SaveConfig();
protected:
    void EnableFlag(size_t& flags, int flag, bool b)
    {
        if(b) {
            flags |= flag;
        } else {
            flags &= ~flag;
        }
    }
    bool HasFlag(const size_t& flags, int flag) const { return flags & flag; }
    WebToolsConfig();
private:
    size_t m_jsFlags;
    size_t m_xmlFlags;
    size_t m_htmlFlags;
    wxString m_nodejs;
    wxString m_npm;
    size_t m_nodeOptions;
    int m_portNumber = 12089;
};

#endif // WEBTOOLSCONFIG_H
