
#include "gsgsSerializer.h"
#include <wx/colour.h>
#include <wx/xml/xml.h>
#include <memory>
#include "xmlutils.h"
#include "gsgsCommon.h"
#include "gsgsConfig.h"
#include "globals.h"

//-----------------------------------------------------------------------------
namespace
{
    const wxChar breakpointName[] = wxT("Breakpoint");
    const wxChar breakpointArrayName[] = wxT("BreakpointArray");
}
//-----------------------------------------------------------------------------
static wxXmlNode* FindNodeByName(const wxXmlNode* parent, const wxString& tagName, const wxString& name)
{
    if(!parent) {
        return NULL;
    }

    wxXmlNode* child = parent->GetChildren();
    while(child) {
        if(child->GetName() == tagName) {
            if(child->XmlGetPropVal(wxT("Name"), wxEmptyString) == name) {
                return child;
            }
        }
        child = child->GetNext();
    }
    return NULL;
}
//-----------------------------------------------------------------------------
static void SetNodeContent(wxXmlNode* node, const wxString& text)
{
    wxXmlNode* n = node->GetChildren();
    wxXmlNode* contentNode = NULL;
    while(n) {
        if(n->GetType() == wxXML_TEXT_NODE || n->GetType() == wxXML_CDATA_SECTION_NODE) {
            contentNode = n;
            break;
        }
        n = n->GetNext();
    }

    if(contentNode) {
        node->RemoveChild(contentNode);
        delete contentNode;
    }

    if(!text.IsEmpty()) {
        contentNode = new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, text);
        node->AddChild(contentNode);
    }
}
//-----------------------------------------------------------------------------
static void SetCDATANodeContent(wxXmlNode* node, const wxString& text)
{
    wxXmlNode* n = node->GetChildren();
    wxXmlNode* contentNode = NULL;
    while(n) {
        if(n->GetType() == wxXML_TEXT_NODE || n->GetType() == wxXML_CDATA_SECTION_NODE) {
            contentNode = n;
            break;
        }
        n = n->GetNext();
    }

    if(contentNode) {
        node->RemoveChild(contentNode);
        delete contentNode;
    }

    if(!text.IsEmpty()) {
        contentNode = new wxXmlNode(wxXML_CDATA_SECTION_NODE, wxEmptyString, text);
        node->AddChild(contentNode);
    }
}
namespace gsgs
{
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // TabInfoData
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    TabInfoData::TabInfoData() :
        m_firstVisibleLine(0),
        m_currentLine(0) {}
    //-----------------------------------------------------------------------------
    TabInfoData::~TabInfoData() {}
    //-----------------------------------------------------------------------------
    void TabInfoData::DeSerialize(Serializer & arch)
    {
        arch.Read(wxT("FileName"), mFileName);
        arch.Read(wxT("FirstVisibleLine"), m_firstVisibleLine);
        arch.Read(wxT("CurrentLine"), m_currentLine);
        arch.Read(wxT("Bookmarks"), m_bookmarks);
        arch.Read(wxT("CollapsedFolds"), m_folds);
    }
    //-----------------------------------------------------------------------------
    void TabInfoData::Serialize(Serializer & arch)
    {
        arch.Write(wxT("FileName"), mFileName);
        arch.Write(wxT("FirstVisibleLine"), m_firstVisibleLine);
        arch.Write(wxT("CurrentLine"), m_currentLine);
        arch.Write(wxT("Bookmarks"), m_bookmarks);
        arch.Write(wxT("CollapsedFolds"), m_folds);
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // Serializer
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    Serializer::Serializer()
        : m_root(NULL)
    {
    }
    //-----------------------------------------------------------------------------
    Serializer::~Serializer() {}
    //-----------------------------------------------------------------------------
    bool Serializer::Write(const wxString & name, ObjectData * obj)
    {
        Serializer arch;
        wxXmlNode* node = FindNodeByName(m_root, wxT("ObjectData"), name);
        if (node) {
            m_root->RemoveChild(node);
            delete node;
        }

        node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("ObjectData"));
        m_root->AddChild(node);
        node->XmlAddProperty(wxT("Name"), name);

        arch.SetXmlNode(node);
        obj->Serialize(arch);
        return true;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Read(const wxString & name, ObjectData * obj)
    {
        Serializer arch;
        wxXmlNode* node = FindNodeByName(m_root, wxT("ObjectData"), name);
        if (node)
        {
            arch.SetXmlNode(node);
            obj->DeSerialize(arch);
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Write(const wxString& name, const wxArrayString& arr)
    {
        if (!m_root) {
            return false;
        }
        wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("wxArrayString"));
        m_root->AddChild(node);
        node->XmlAddProperty(wxT("Name"), name);

        // add an entry for each wxString in the array
        for (size_t i = 0; i < arr.GetCount(); i++) {
            wxXmlNode* child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("wxString"));
            node->AddChild(child);
            child->XmlAddProperty(wxT("Value"), arr.Item(i));
        }
        return true;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Write(const wxString& name, std::vector<TabInfoData> & _vTabInfoArr)
    {
        if (!m_root) {
            return false;
        }
        wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("TabInfoArray"));
        m_root->AddChild(node);
        node->XmlAddProperty(wxT("Name"), name);

        // add an entry for each wxString in the array
        for (size_t i = 0; i < _vTabInfoArr.size(); i++) {
            wxXmlNode* child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("TabInfoData"));
            Serializer arch;
            arch.SetXmlNode(child);
            _vTabInfoArr[i].Serialize(arch);
            node->AddChild(child);
        }
        return true;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Write(const wxString& name, std::vector<int>& _vInt)
    {
        if (!m_root) {
            return false;
        }
        wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("IntVector"));
        m_root->AddChild(node);
        node->XmlAddProperty(wxT("Name"), name);

        // add an entry for each int in the vector
        for (size_t i = 0; i < _vInt.size(); ++i) {
            wxXmlNode* child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("IntVectorItem"));
            node->AddChild(child);
            child->XmlAddProperty(wxT("Value"), wxString::Format(wxT("%i"), _vInt.at(i)));
        }
        return true;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Write(const wxString& name, const StringMap& str_map)
    {
        if (!m_root) {
            return false;
        }

        wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("StringMap"));
        m_root->AddChild(node);
        node->XmlAddProperty(wxT("Name"), name);

        // add an entry for each wxString in the array
        StringMap::const_iterator iter = str_map.begin();
        for (; iter != str_map.end(); iter++) {
            wxXmlNode* child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("MapEntry"));
            node->AddChild(child);
            child->XmlAddProperty(wxT("Key"), iter->first);
            child->XmlAddProperty(wxT("Value"), iter->second);
        }
        return true;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Read(const wxString& name, wxArrayString& arr)
    {
        if (!m_root) {
            return false;
        }

        wxXmlNode* node = FindNodeByName(m_root, wxT("wxArrayString"), name);
        if (node) {
            // fill the output array with the values
            arr.Clear();
            wxXmlNode* child = node->GetChildren();
            while (child) {
                if (child->GetName() == wxT("wxString")) {
                    wxString value;
                    value = child->XmlGetPropVal(wxT("Value"), wxEmptyString);
                    arr.Add(value);
                }
                child = child->GetNext();
            }
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Read(const wxString& name, std::vector<TabInfoData>& _vTabInfoArr)
    {
        if (!m_root) {
            return false;
        }

        Serializer arch;
        wxXmlNode* node = FindNodeByName(m_root, wxT("TabInfoArray"), name);
        if (node) {
            // fill the output array with the values
            _vTabInfoArr.clear();
            wxXmlNode* child = node->GetChildren();
            while (child) {
                if (child->GetName() == wxT("TabInfoData")) {
                    arch.SetXmlNode(child);
                    TabInfoData oTabInfo;
                    oTabInfo.DeSerialize(arch);
                    _vTabInfoArr.push_back(oTabInfo);
                }
                child = child->GetNext();
            }
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Read(const wxString& name, std::vector<int>& _vInt)
    {
        if (!m_root) {
            return false;
        }

        wxXmlNode* node = FindNodeByName(m_root, wxT("IntVector"), name);
        if (node) {
            // fill the output array with the values
            _vInt.clear();
            wxXmlNode* child = node->GetChildren();
            while (child) {
                if (child->GetName() == wxT("IntVectorItem")) {
                    long value;
                    wxString stringvalue = child->XmlGetPropVal(wxT("Value"), wxEmptyString);
                    if (stringvalue.ToLong(&value)) {
                        _vInt.push_back(value);
                    }
                }
                child = child->GetNext();
            }
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Read(const wxString& name, StringMap& str_map)
    {
        if (!m_root) {
            return false;
        }

        wxXmlNode* node = FindNodeByName(m_root, wxT("StringMap"), name);
        if (node) {
            // fill the output array with the values
            str_map.clear();
            wxXmlNode* child = node->GetChildren();
            while (child) {
                if (child->GetName() == wxT("MapEntry")) {
                    wxString value;
                    wxString key;
                    key = child->XmlGetPropVal(wxT("Key"), wxEmptyString);
                    value = child->XmlGetPropVal(wxT("Value"), wxEmptyString);
                    str_map[key] = value;
                }
                child = child->GetNext();
            }
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------
#if wxUSE_GUI
    bool Serializer::Read(const wxString& name, wxSize& size)
    {
        if (!m_root) {
            return false;
        }

        wxXmlNode* node = FindNodeByName(m_root, wxT("wxSize"), name);
        if (node) {
            long v;
            wxString value;
            value = node->XmlGetPropVal(wxT("x"), wxEmptyString);
            value.ToLong(&v);
            size.x = v;

            value = node->XmlGetPropVal(wxT("y"), wxEmptyString);
            value.ToLong(&v);
            size.y = v;
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Read(const wxString& name, wxPoint& pt)
    {
        if (!m_root) {
            return false;
        }

        wxXmlNode* node = FindNodeByName(m_root, wxT("wxPoint"), name);
        if (node) {
            long v;
            wxString value;
            value = node->XmlGetPropVal(wxT("x"), wxEmptyString);
            value.ToLong(&v);
            pt.x = v;

            value = node->XmlGetPropVal(wxT("y"), wxEmptyString);
            value.ToLong(&v);
            pt.y = v;
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Read(const wxString& name, wxFont& font, const wxFont& defaultFont)
    {
        wxString strFont;
        bool res = Read(name, strFont);
        if (!res || strFont.IsEmpty()) {
            font = defaultFont;
            return false;
        }
        font = gsgs::FontHelper::FromString(strFont);
        return true;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Write(const wxString& name, const wxFont& font) 
    { 
        return Write(name, gsgs::FontHelper::ToString(font)); 
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Read(const wxString& name, wxColour& colour)
    {
        if (!m_root) {
            return false;
        }

        wxXmlNode* node = FindNodeByName(m_root, wxT("wxColour"), name);
        wxString value;
        if (node) {
            value = node->XmlGetPropVal(wxT("Value"), wxEmptyString);
        }

        if (value.IsEmpty()) {
            return false;
        }

        colour = wxColour(value);
        return true;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Write(const wxString& name, const wxColour& colour)
    {
        if (!m_root) {
            return false;
        }
        wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("wxColour"));
        m_root->AddChild(node);
        node->XmlAddProperty(wxT("Name"), name);
        node->XmlAddProperty(wxT("Value"), colour.GetAsString());
        return true;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Write(const wxString& name, wxSize size)
    {
        if (!m_root) {
            return false;
        }

        wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("wxSize"));
        m_root->AddChild(node);
        node->XmlAddProperty(wxT("Name"), name);

        wxString xstr, ystr;
        xstr << size.x;
        ystr << size.y;

        node->XmlAddProperty(wxT("x"), xstr);
        node->XmlAddProperty(wxT("y"), ystr);
        return true;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Write(const wxString& name, wxPoint pt)
    {
        if (!m_root) {
            return false;
        }

        wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("wxPoint"));
        m_root->AddChild(node);
        node->XmlAddProperty(wxT("Name"), name);

        wxString xstr, ystr;
        xstr << pt.x;
        ystr << pt.y;

        node->XmlAddProperty(wxT("x"), xstr);
        node->XmlAddProperty(wxT("y"), ystr);
        return true;
    }
#endif
    //-----------------------------------------------------------------------------
    bool Serializer::Write(const wxString& name, int value) 
    { 
        return WriteSimple(value, wxT("int"), name); 
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Read(const wxString& name, int& value)
    {
        if (!m_root) {
            return false;
        }

        long v;
        bool res = ReadSimple(v, wxT("int"), name);
        if (res) {
            value = v;
        }
        return res;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Write(const wxString& name, long value) 
    { 
        return WriteSimple(value, wxT("long"), name); 
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Read(const wxString& name, long& value) 
    { 
        return ReadSimple(value, wxT("long"), name); 
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Write(const wxString& name, bool value) 
    { 
        return WriteSimple(value ? 1 : 0, wxT("bool"), name); 
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Read(const wxString& name, bool& value)
    {
        long v;
        bool res = ReadSimple(v, wxT("bool"), name);
        if (res) {
            v == 0 ? value = false : value = true;
        }
        return res;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Write(const wxString& name, const wxString& str)
    {
        if (!m_root) {
            return false;
        }
        wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("wxString"));
        m_root->AddChild(node);
        node->XmlAddProperty(wxT("Name"), name);
        node->XmlAddProperty(wxT("Value"), str);
        return true;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::WriteCData(const wxString& name, const wxString& str)
    {
        if (!m_root) {
            return false;
        }

        wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("CData"));
        m_root->AddChild(node);

        SetCDATANodeContent(node, str);
        node->XmlAddProperty(wxT("Name"), name);
        return true;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::ReadCData(const wxString& name, wxString& value)
    {
        if (!m_root) {
            return false;
        }

        wxXmlNode* node = FindNodeByName(m_root, wxT("CData"), name);
        if (node) {
            // get the content node
            value = node->GetNodeContent();
            value.Trim().Trim(false);
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Read(const wxString& name, wxString& value)
    {
        if (!m_root) {
            return false;
        }
        wxXmlNode* node = FindNodeByName(m_root, wxT("wxString"), name);
        if (node) {
            value = node->XmlGetPropVal(wxT("Value"), value);
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Read(const wxString& name, size_t& value)
    {
        long v = 0;
        bool res = Read(name, v);
        if (res) {
            value = v;
        }
        return res;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Read(const wxString& name, wxFileName& fileName)
    {
        wxString value;
        bool res = Read(name, value);
        if (res) {
            fileName = wxFileName(value);
        }
        return res;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Write(const wxString& name, size_t value) 
    { 
        return Write(name, (long)value); 
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Write(const wxString& name, const wxFileName& fileName)
    { 
        return Write(name, fileName.GetFullPath()); 
    }
    //-----------------------------------------------------------------------------
    void Serializer::SetXmlNode(wxXmlNode* node) 
    { 
        m_root = node; 
    }
    //-----------------------------------------------------------------------------
    bool Serializer::WriteSimple(long in, const wxString & type, const wxString & name)
    {
        if (!m_root)
            return false;

        wxString propValue;
        propValue << in;

        wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, type);
        m_root->AddChild(node);
        node->XmlAddProperty(wxT("Name"), name);
        node->XmlAddProperty(wxT("Value"), propValue);
        return true;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::ReadSimple(long & out, const wxString & type, const wxString & name)
    {
        if (!m_root)
            return false;

        out = 0;
        wxXmlNode* node = FindNodeByName(m_root, type, name);
        if (node) {
            wxString val = node->XmlGetPropVal(wxT("Value"), wxEmptyString);
            val.ToLong(&out);
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Write(const wxString& name, const gsgsStringMap& strinMap)
    {
        if (!m_root) {
            return false;
        }

        wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("StringMap"));
        m_root->AddChild(node);
        node->XmlAddProperty(wxT("Name"), name);

        // add an entry for each wxString in the array
        gsgsStringMap::const_iterator iter = strinMap.begin();
        for (; iter != strinMap.end(); ++iter) {
            wxXmlNode* child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("MapEntry"));
            node->AddChild(child);
            child->XmlAddProperty(wxT("Key"), iter->first);
            SetNodeContent(child, iter->second);
        }
        return true;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Read(const wxString& name, gsgsStringMap& strinMap)
    {
        if (!m_root) {
            return false;
        }

        wxXmlNode* node = FindNodeByName(m_root, wxT("StringMap"), name);
        if (node) {
            // fill the output array with the values
            strinMap.clear();
            wxXmlNode* child = node->GetChildren();
            while (child) {
                if (child->GetName() == wxT("MapEntry")) {
                    wxString value;
                    wxString key;
                    key = child->XmlGetPropVal(wxT("Key"), wxEmptyString);
                    value = child->GetNodeContent();
                    strinMap[key] = value;
                }
                child = child->GetNext();
            }
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Read(const wxString& name, gsgsStringSet& s)
    {
        if (!m_root) {
            return false;
        }

        wxXmlNode* node = FindNodeByName(m_root, wxT("std_string_set"), name);
        if (node) {
            // fill the output array with the values
            s.clear();
            wxXmlNode* child = node->GetChildren();
            while (child) {
                if (child->GetName() == wxT("SetEntry")) {
                    wxString value;
                    value = child->GetNodeContent();
                    s.insert(value);
                }
                child = child->GetNext();
            }
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------
    bool Serializer::Write(const wxString& name, const gsgsStringSet & s)
    {
        if (!m_root) {
            return false;
        }

        wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("std_string_set"));
        m_root->AddChild(node);
        node->XmlAddProperty(wxT("Name"), name);

        // add an entry for each wxString in the array
        gsgsStringSet::const_iterator iter = s.begin();
        for (; iter != s.end(); ++iter) {
            wxXmlNode* child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("SetEntry"));
            node->AddChild(child);
            SetNodeContent(child, *iter);
        }
        return true;
    }
    //-----------------------------------------------------------------------------
    SettersGettersData::SettersGettersData()
    {
    }
    //-----------------------------------------------------------------------------
    SettersGettersData::~SettersGettersData()
    {
    }
    //-----------------------------------------------------------------------------
    void SettersGettersData::Serialize(Serializer &arch)
    {
        arch.Write(wxT("m_flags"), m_flags);
    }
    //-----------------------------------------------------------------------------
    void SettersGettersData::DeSerialize(Serializer &arch)
    {
        arch.Read(wxT("m_flags"), m_flags);
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // PhpOptions
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    PhpOptions::PhpOptions()
        : gsgs::ObjectData("PHPConfigurationData")
        , m_phpExe("")
        , m_errorReporting("E_ALL & ~E_NOTICE")
    {
        wxFileName newConfigFile = gsgs_Path().GetUserDataDir() + gsgsPathSep + "config" +
            gsgsPathSep + "php-general.conf";
        if (!newConfigFile.FileExists()) {
            wxFileName oldConfigFile = gsgs_Path().GetUserDataDir() + gsgsPathSep + "config" +
                gsgsPathSep + "php.conf";
            // first time, copy the values from the old settings
            JSON root(oldConfigFile);
            JSONObject oldJson = root.getObject().getObject("PHPConfigurationData");

            oldJson.getObject("m_phpExe").getValue(m_phpExe);
            oldJson.getObject("m_includePaths").getValue(m_includePaths);
            oldJson.getObject("m_errorReporting").getValue(m_errorReporting);

            // Save it

            JSON newRoot(newConfigFile);
            JSONObject e = JSONObject::createObject(GetName());
            e.addProperty("m_phpExe", m_phpExe);
            e.addProperty("m_includePaths", m_includePaths);
            e.addProperty("m_errorReporting", m_errorReporting);
            newRoot.getObject().addProperty(e);
            newRoot.save(newConfigFile);
        }
    }
    //------------------------------------------------------------------------------------------
    PhpOptions::~PhpOptions() {}
    //------------------------------------------------------------------------------------------
    void PhpOptions::FromJSON(const JSONObject& json)
    {
        json.getObject("m_phpExe").getValue(m_phpExe, m_phpExe);
        if (m_phpExe.IsEmpty()) {
            wxFileName phpExe;
            FindExec("php", phpExe);
            m_phpExe = phpExe.GetFullPath();
        }

        json.getObject("m_errorReporting").getValue(m_errorReporting, m_errorReporting);
        json.getObject("m_includePaths").getValue(m_includePaths);
    }
    //------------------------------------------------------------------------------------------
    JSONObject PhpOptions::ToJSON() const
    {
        gsgs::JSONObject element = gsgs::JSONObject::createObject(GetName());
        element.addProperty("m_phpExe", m_phpExe);
        element.addProperty("m_errorReporting", m_errorReporting);
        element.addProperty("m_includePaths", m_includePaths);
        return element;
    }
    //------------------------------------------------------------------------------------------
    PhpOptions& PhpOptions::Load()
    {
        Config config("php-general.conf");
        config.ReadItem(this);
        return *this;
    }
    //------------------------------------------------------------------------------------------
    PhpOptions& PhpOptions::Save()
    {
        Config config("php-general.conf");
        config.WriteItem(this);

        // Notify that the PHP settings were modified
        gsgs::Event event(wxEVT_PHP_SETTINGS_CHANGED);
        gsgs_Event().AddPendingEvent(event);

        return *this;
    }
    //------------------------------------------------------------------------------------------
}