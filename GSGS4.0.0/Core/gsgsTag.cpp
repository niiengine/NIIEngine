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

#include "gsgsPreProcess.h"
#include "gsgsTagManager.h"
#include "pptable.h"
#include "gsgsTag.h"
#include "gsgsLanguage.h"
#include "code_completion_api.h"
#include "gsgsComment.h"
#include "istorage.h"
#include <wx/regex.h>

namespace gsgs
{
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // TagsOptionsData
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    wxString TagsOptionsData::CLANG_CACHE_LAZY = "Lazy";
    wxString TagsOptionsData::CLANG_CACHE_ON_FILE_LOAD = "On File Load";
    //-----------------------------------------------------------------------------------
    size_t TagsOptionsData::CURRENT_VERSION = 7100;
    //-----------------------------------------------------------------------------------
    static bool _IsValidCppIndetifier(const wxString & id)
    {
        if(id.IsEmpty()) {
            return false;
        }
        // first char can be only _A-Za-z
        wxString first(id.Mid(0, 1));
        if(first.find_first_not_of(wxT("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")) != wxString::npos) {
            return false;
        }
        // make sure that rest of the id contains only a-zA-Z0-9_
        if(id.find_first_not_of(wxT("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789")) != wxString::npos) {
            return false;
        }
        return true;
    }
    //-----------------------------------------------------------------------------------
    static bool _IsCppKeyword(const wxString& word)
    {
        static gsgsStringSet words;
        if(words.empty()) {
            TagsManager::GetCXXKeywords(words);
        }
        return words.count(word);
    }

    //-----------------------------------------------------------------------------------

    TagsOptionsData::TagsOptionsData()
        : ObjectData("CodeCompletion")
        , m_ccFlags(CC_DISP_FUNC_CALLTIP | CC_CPP_KEYWORD_ASISST | CC_COLOUR_VARS | CC_ACCURATE_SCOPE_RESOLVING |
                    CC_DEEP_SCAN_USING_NAMESPACE_RESOLVING | CC_WORD_ASSIST)
        , m_ccColourFlags(CC_COLOUR_DEFAULT)
        , m_fileSpec(wxT("*.cpp;*.cc;*.cxx;*.h;*.hpp;*.c;*.c++;*.tcc;*.hxx;*.h++"))
        , m_minWordLen(3)
        , m_parserEnabled(true)
        , m_maxItemToColour(1000)
    #ifdef __WXMSW__
        , m_macrosFiles(wxT("_mingw.h bits/c++config.h"))
    #elif defined(__WXMAC__)
        , m_macrosFiles(wxT("sys/cdefs.h bits/c++config.h AvailabilityMacros.h"))
    #else
        , m_macrosFiles(wxT("sys/cdefs.h bits/c++config.h"))
    #endif
        , m_clangOptions(0)
        , m_clangBinary(wxT(""))
        , m_clangCachePolicy(TagsOptionsData::CLANG_CACHE_ON_FILE_LOAD)
        , m_ccNumberOfDisplayItems(500)
        , m_version(0)
    {
        // Initialize defaults
        m_languages.Add(wxT("C++"));
        m_tokens.Add(wxT("EXPORT"));
        m_tokens.Add(wxT("_GLIBCXX_BEGIN_NAMESPACE_VERSION"));
        m_tokens.Add(wxT("_GLIBCXX_END_NAMESPACE_VERSION"));
        m_tokens.Add(wxT("_GLIBCXX_NOEXCEPT"));
        m_tokens.Add(wxT("_NOEXCEPT"));
        m_tokens.Add(wxT("WXDLLIMPEXP_CORE"));
        m_tokens.Add(wxT("WXDLLIMPEXP_BASE"));
        m_tokens.Add(wxT("WXDLLIMPEXP_XML"));
        m_tokens.Add(wxT("WXDLLIMPEXP_XRC"));
        m_tokens.Add(wxT("WXDLLIMPEXP_ADV"));
        m_tokens.Add(wxT("WXDLLIMPEXP_AUI"));
        m_tokens.Add(wxT("WXDLLIMPEXP_FWD_ADV"));
        m_tokens.Add(wxT("_gsgsAPI"));
        m_tokens.Add(wxT("WXDLLIMPEXP_LE_SDK"));
        m_tokens.Add(wxT("WXDLLIMPEXP_SQLITE3"));
        m_tokens.Add(wxT("WXDLLIMPEXP_SCI"));
        m_tokens.Add(wxT("WXDLLIMPEXP_FWD_AUI"));
        m_tokens.Add(wxT("WXDLLIMPEXP_FWD_PROPGRID"));
        m_tokens.Add(wxT("WXDLLIMPEXP_FWD_XML"));
        m_tokens.Add(wxT("WXMAKINGDLL"));
        m_tokens.Add(wxT("WXUSINGDLL"));
        m_tokens.Add(wxT("_CRTIMP"));
        m_tokens.Add(wxT("__CRT_INLINE"));
        m_tokens.Add(wxT("__cdecl"));
        m_tokens.Add(wxT("__stdcall"));
        m_tokens.Add(wxT("WXDLLEXPORT"));
        m_tokens.Add(wxT("WXDLLIMPORT"));
        m_tokens.Add(wxT("__MINGW_ATTRIB_PURE"));
        m_tokens.Add(wxT("__MINGW_ATTRIB_MALLOC"));
        m_tokens.Add(wxT("__GOMP_NOTHROW"));
        m_tokens.Add(wxT("wxT"));
        m_tokens.Add(wxT("SCI_SCOPE(%0)=%0"));
        m_tokens.Add(wxT("WINBASEAPI"));
        m_tokens.Add(wxT("WINAPI"));
        m_tokens.Add(wxT("__nonnull"));
        m_tokens.Add(wxT("noexcept"));
        m_tokens.Add(wxT("wxOVERRIDE"));
        m_tokens.Add(wxT("override"));
        m_tokens.Add("final"); // ignore "final" keyword
    #if defined(__WXGTK__)
        m_tokens.Add(wxT("wxTopLevelWindowNative=wxTopLevelWindowGTK"));
        m_tokens.Add(wxT("wxWindow=wxWindowGTK"));

    #elif defined(__WXMSW__)
        m_tokens.Add(wxT("wxTopLevelWindowNative=wxTopLevelWindowMSW"));
        m_tokens.Add(wxT("wxWindow=wxWindowMSW"));
    #else
        m_tokens.Add(wxT("wxTopLevelWindowNative=wxTopLevelWindowMac"));
        m_tokens.Add(wxT("wxWindow=wxWindowMac"));
    #endif
        m_tokens.Add(wxT("wxWindowNative=wxWindowBase"));
        m_tokens.Add(wxT("wxStatusBar=wxStatusBarBase"));
        m_tokens.Add(wxT("BEGIN_DECLARE_EVENT_TYPES()=enum {"));
        m_tokens.Add(wxT("END_DECLARE_EVENT_TYPES()=};"));
        m_tokens.Add(wxT("DECLARE_EVENT_TYPE"));
        m_tokens.Add(wxT("DECLARE_EXPORTED_EVENT_TYPE"));
        m_tokens.Add(wxT("WXUNUSED(%0)=%0"));
        m_tokens.Add(wxT("wxDEPRECATED(%0)=%0"));
        m_tokens.Add(wxT("_T"));
        m_tokens.Add(wxT("ATTRIBUTE_PRINTF_1"));
        m_tokens.Add(wxT("ATTRIBUTE_PRINTF_2"));
        m_tokens.Add(wxT("WXDLLIMPEXP_FWD_BASE"));
        m_tokens.Add(wxT("WXDLLIMPEXP_FWD_CORE"));
        m_tokens.Add(wxT("DLLIMPORT"));
        m_tokens.Add(wxT("DECLARE_INSTANCE_TYPE"));
        m_tokens.Add(wxT("emit"));
        m_tokens.Add(wxT("Q_OBJECT"));
        m_tokens.Add(wxT("Q_PACKED"));
        m_tokens.Add(wxT("Q_GADGET"));
        m_tokens.Add(wxT("QT_BEGIN_NAMESPACE"));
        m_tokens.Add(wxT("QT_END_NAMESPACE"));
        m_tokens.Add(wxT("Q_GADGET"));
        m_tokens.Add(wxT("QT_BEGIN_HEADER"));
        m_tokens.Add(wxT("QT_END_HEADER"));
        m_tokens.Add(wxT("Q_REQUIRED_RESULT"));
        m_tokens.Add(wxT("Q_INLINE_TEMPLATE"));
        m_tokens.Add(wxT("Q_OUTOFLINE_TEMPLATE"));
        m_tokens.Add(wxT("_GLIBCXX_BEGIN_NAMESPACE(%0)=namespace %0{"));
        m_tokens.Add(wxT("_GLIBCXX_END_NAMESPACE=}"));
        m_tokens.Add(wxT("_GLIBCXX_BEGIN_NESTED_NAMESPACE(%0, %1)=namespace %0{"));
        m_tokens.Add(wxT("wxDECLARE_EXPORTED_EVENT(%0,%1,%2)=int %1;"));
        m_tokens.Add(wxT("wxDECLARE_EVENT(%0,%1)=int %0;"));
        m_tokens.Add(wxT("BOOST_FOREACH(%0, %1)=%0;"));
        m_tokens.Add(wxT("DECLARE_EVENT_TYPE(%0,%1)=int %0;"));
        m_tokens.Add(wxT("_GLIBCXX_END_NESTED_NAMESPACE=}"));
        m_tokens.Add(wxT("_GLIBCXX_VISIBILITY(%0)"));
        m_tokens.Add(wxT("_GLIBCXX_BEGIN_NAMESPACE_TR1=namespace tr1{"));
        m_tokens.Add(wxT("_GLIBCXX_END_NAMESPACE_TR1=}"));
        m_tokens.Add(wxT("_GLIBCXX_STD=std"));
        m_tokens.Add(wxT("_GLIBCXX_BEGIN_NAMESPACE_CONTAINER"));
        m_tokens.Add(wxT("__const=const"));
        m_tokens.Add(wxT("__restrict"));
        m_tokens.Add(wxT("__THROW"));
        m_tokens.Add(wxT("__wur"));
        m_tokens.Add(wxT("_STD_BEGIN=namespace std{"));
        m_tokens.Add(wxT("_STD_END=}"));
        m_tokens.Add(wxT("__CLRCALL_OR_CDECL"));
        m_tokens.Add(wxT("_CRTIMP2_PURE"));
        m_tokens.Add(wxT("_GLIBCXX_CONST"));
        m_tokens.Add(wxT("_GLIBCXX_CONSTEXPR"));
        m_tokens.Add(wxT("_GLIBCXX_NORETURN"));
        m_tokens.Add(wxT("_GLIBCXX_NOTHROW"));
        m_tokens.Add(wxT("_GLIBCXX_PURE"));
        m_tokens.Add(wxT("_GLIBCXX_THROW(%0)"));
        m_tokens.Add(wxT("_GLIBCXX_DEPRECATED"));
        m_tokens.Add("LLDB_API");
        m_tokens.Add("PYTHON_API");
        m_tokens.Add("__cpp_deduction_guides=0");
        m_tokens.Add("wxMSVC_FWD_MULTIPLE_BASES");
        
        // libcpp macros
        m_tokens.Add("_LIBCPP_TYPE_VIS_ONLY");
        m_tokens.Add("_LIBCPP_CONSTEXPR");
        m_tokens.Add("_LIBCPP_CONSTEXPR_AFTER_CXX11");
        m_tokens.Add("_LIBCPP_INLINE_VISIBILITY");
        m_tokens.Add("_LIBCPP_BEGIN_NAMESPACE_STD=namespace std{");
        m_tokens.Add("_LIBCPP_END_NAMESPACE_STD=}");
        m_types.Add(wxT("std::vector::reference=_Tp"));
        m_types.Add(wxT("std::vector::const_reference=_Tp"));
        m_types.Add(wxT("std::vector::iterator=_Tp"));
        m_types.Add(wxT("std::vector::const_iterator=_Tp"));
        m_types.Add(wxT("std::queue::reference=_Tp"));
        m_types.Add(wxT("std::queue::const_reference=_Tp"));
        m_types.Add(wxT("std::priority_queue::reference=_Tp"));
        m_types.Add(wxT("std::priority_queue::const_reference=_Tp"));
        m_types.Add(wxT("std::set::const_iterator=_Key"));
        m_types.Add(wxT("std::set::iterator=_Key"));
        m_types.Add(wxT("std::unordered_set::const_iterator=_Key"));
        m_types.Add(wxT("std::unordered_set::iterator=_Key"));
        m_types.Add(wxT("std::deque::reference=_Tp"));
        m_types.Add(wxT("std::deque::const_reference=_Tp"));
        m_types.Add(wxT("std::map::iterator=std::pair<_Key, _Tp>"));
        m_types.Add(wxT("std::map::const_iterator=std::pair<_Key,_Tp>"));
        m_types.Add(wxT("std::unordered_map::iterator=std::pair<_Key, _Tp>"));
        m_types.Add(wxT("std::unordered_map::mapped_type=_Tp"));
        m_types.Add(wxT("std::unordered_map::const_iterator=std::pair<_Key,_Tp>"));
        m_types.Add(wxT("std::unordered_map::value_type=std::pair<_Key,_Tp>"));
        m_types.Add(wxT("std::multimap::iterator=std::pair<_Key,_Tp>"));
        m_types.Add(wxT("std::multimap::const_iterator=std::pair<_Key,_Tp>"));
        m_types.Add(wxT("wxOrderedMap::iterator=std::pair<Key,Value>"));
        m_types.Add(wxT("wxOrderedMap::const_iterator=std::pair<Key,Value>"));
        m_types.Add(wxT("boost::shared_ptr::type=T"));
        m_types.Add(wxT("std::unique_ptr::pointer=_Tp"));

        SyncData();
    }
    //-----------------------------------------------------------------------------------
    TagsOptionsData::~TagsOptionsData() {}
    //-----------------------------------------------------------------------------------
    wxString TagsOptionsData::ToString() const
    {
        wxString options(wxEmptyString);

        static wxString file_name;
        wxString file_content;

        if(file_name.IsEmpty()) {
            char* ctagsReplacement = getenv("CTAGS_REPLACEMENTS");
            if(ctagsReplacement) {
                file_name = wxString(ctagsReplacement, wxConvUTF8).c_str();
            }
        }

        const gsgsStringMap& tokensMap = GetTokensWxMap();
        gsgsStringMap::const_iterator iter = tokensMap.begin();

        if(tokensMap.empty() == false) {
            for(; iter != tokensMap.end(); ++iter) {
                if(!iter->second.IsEmpty() || (iter->second.IsEmpty() && iter->first.Find(wxT("%0")) != wxNOT_FOUND)) {
                    // Key = Value pair. Place this one in the output file
                    file_content << iter->first << wxT("=") << iter->second << wxT("\n");
                } else {

                    if(options.IsEmpty()) options = wxT(" -I");

                    options << iter->first;
                    options << wxT(",");
                }
            }

            if(options.IsEmpty() == false) options.RemoveLast();

            options += wxT(" ");
        }

        // write the file content
        if(file_name.IsEmpty() == false) {
            wxFFile fp(file_name, wxT("w+b"));
            if(fp.IsOpened()) {
                fp.Write(file_content);
                fp.Close();
            }
        }

        if(GetLanguages().IsEmpty() == false) {
            options += wxT(" --language-force=");
            options += GetLanguages().Item(0);
            options += wxT(" ");
        }
        return options;
    }
    //-----------------------------------------------------------------------------------
    void TagsOptionsData::SetLanguageSelection(const wxString& lang)
    {
        int where = m_languages.Index(lang);
        if(where != wxNOT_FOUND) {
            m_languages.RemoveAt(where);
        }
        m_languages.Insert(lang, 0);
    }
    //-----------------------------------------------------------------------------------
    std::map<std::string, std::string> TagsOptionsData::GetTokensMap() const
    {
        std::map<std::string, std::string> tokens;
        for(size_t i = 0; i < m_tokens.GetCount(); i++) {
            // const wxCharBuffer bufKey = _C(
            wxString item = m_tokens.Item(i);
            item.Trim().Trim(false);
            wxString k = item.BeforeFirst(wxT('='));
            wxString v = item.AfterFirst(wxT('='));

            const wxCharBuffer bufKey = _C(k);
            std::string key = bufKey.data();
            std::string value;
            if(!v.empty()) {
                const wxCharBuffer bufValue = _C(v);
                value = bufValue.data();
            }
            tokens[key] = value;
        }
        return tokens;
    }
    //-----------------------------------------------------------------------------------
    gsgsStringMap TagsOptionsData::GetTypesMap() const
    {
        gsgsStringMap tokens;
        for(size_t i = 0; i < m_types.GetCount(); i++) {
            wxString item = m_types.Item(i);
            item.Trim().Trim(false);
            wxString k = item.BeforeFirst(wxT('='));
            wxString v = item.AfterFirst(wxT('='));
            tokens[k] = v;
        }
        return tokens;
    }
    //-----------------------------------------------------------------------------------
    void TagsOptionsData::SetTokens(const wxString& tokens)
    {
        this->m_tokens = ::wxStringTokenize(tokens, "\r\n", wxTOKEN_STRTOK);
        DoUpdateTokensWxMapReversed();
        DoUpdateTokensWxMap();
    }
    //-----------------------------------------------------------------------------------
    void TagsOptionsData::DoUpdateTokensWxMap()
    {
        m_tokensWxMap.clear();
        for(size_t i = 0; i < m_tokens.GetCount(); i++) {
            wxString item = m_tokens.Item(i).Trim().Trim(false);
            wxString k = item.BeforeFirst(wxT('='));
            wxString v = item.AfterFirst(wxT('='));
            m_tokensWxMap[k] = v;
        }
    }
    //-----------------------------------------------------------------------------------
    void TagsOptionsData::DoUpdateTokensWxMapReversed()
    {
        m_tokensWxMapReversed.clear();
        for(size_t i = 0; i < m_tokens.GetCount(); i++) {
            wxString item = m_tokens.Item(i).Trim().Trim(false);
            wxString k = item.AfterFirst(wxT('='));
            wxString v = item.BeforeFirst(wxT('='));
            if(_IsValidCppIndetifier(k) && !_IsCppKeyword(k)) {
                m_tokensWxMapReversed[k] = v;
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void TagsOptionsData::FromJSON(const gsgs::JSONObject& json)
    {
        json.getObject("version").getValue(m_version);
        json.getObject(wxT("m_ccFlags")).getValue(m_ccFlags, m_ccFlags);
        json.getObject(wxT("m_ccColourFlags")).getValue(m_ccColourFlags, m_ccColourFlags);
        json.getObject(wxT("m_tokens")).getValue(m_tokens, m_tokens);
        json.getObject(wxT("m_types")).getValue(m_types, m_types);
        json.getObject(wxT("m_fileSpec")).getValue(m_fileSpec, m_fileSpec);
        json.getObject(wxT("m_languages")).getValue(m_languages);
        json.getObject(wxT("m_minWordLen")).getValue(m_minWordLen, m_minWordLen);
        json.getObject(wxT("m_parserSearchPaths")).getValue(m_parserSearchPaths);
        json.getObject(wxT("m_parserEnabled")).getValue(m_parserEnabled);
        json.getObject(wxT("m_parserExcludePaths")).getValue(m_parserExcludePaths);
        json.getObject(wxT("m_maxItemToColour")).getValue(m_maxItemToColour);
        json.getObject(wxT("m_macrosFiles")).getValue(m_macrosFiles);
        json.getObject(wxT("m_clangOptions")).getValue(m_clangOptions);
        json.getObject(wxT("m_clangBinary")).getValue(m_clangBinary);
        json.getObject(wxT("m_clangCmpOptions")).getValue(m_clangCmpOptions);
        json.getObject(wxT("m_clangSearchPaths")).getValue(m_clangSearchPaths);
        json.getObject(wxT("m_clangMacros")).getValue(m_clangMacros);
        json.getObject(wxT("m_clangCachePolicy")).getValue(m_clangCachePolicy);
        json.getObject(wxT("m_ccNumberOfDisplayItems")).getValue(m_ccNumberOfDisplayItems, m_ccNumberOfDisplayItems);

        if(!m_fileSpec.Contains("*.hxx")) {
            m_fileSpec = "*.cpp;*.cc;*.cxx;*.h;*.hpp;*.c;*.c++;*.tcc;*.hxx;*.h++";
        }

        DoUpdateTokensWxMapReversed();
        DoUpdateTokensWxMap();
        m_ccFlags |= CC_ACCURATE_SCOPE_RESOLVING;
    }
    //-----------------------------------------------------------------------------------
    gsgs::JSONObject TagsOptionsData::ToJSON() const
    {
        gsgs::JSONObject json = gsgs::JSONObject::createObject(GetName());
        json.addProperty("version", m_version);
        json.addProperty("m_ccFlags", m_ccFlags);
        json.addProperty("m_ccColourFlags", m_ccColourFlags);
        json.addProperty("m_tokens", m_tokens);
        json.addProperty("m_types", m_types);
        json.addProperty("m_fileSpec", m_fileSpec);
        json.addProperty("m_languages", m_languages);
        json.addProperty("m_minWordLen", m_minWordLen);
        json.addProperty("m_parserSearchPaths", m_parserSearchPaths);
        json.addProperty("m_parserEnabled", m_parserEnabled);
        json.addProperty("m_parserExcludePaths", m_parserExcludePaths);
        json.addProperty("m_maxItemToColour", m_maxItemToColour);
        json.addProperty("m_macrosFiles", m_macrosFiles);
        json.addProperty("m_clangOptions", m_clangOptions);
        json.addProperty("m_clangBinary", m_clangBinary);
        json.addProperty("m_clangCmpOptions", m_clangCmpOptions);
        json.addProperty("m_clangSearchPaths", m_clangSearchPaths);
        json.addProperty("m_clangMacros", m_clangMacros);
        json.addProperty("m_clangCachePolicy", m_clangCachePolicy);
        json.addProperty("m_ccNumberOfDisplayItems", m_ccNumberOfDisplayItems);
        return json;
    }
    //-----------------------------------------------------------------------------------
    wxString TagsOptionsData::DoJoinArray(const wxArrayString& arr) const
    {
        wxString s;
        for(size_t i = 0; i < arr.GetCount(); ++i)
            s << arr.Item(i) << "\n";

        if(s.IsEmpty() == false) s.RemoveLast();

        return s;
    }
    //-----------------------------------------------------------------------------------
    void TagsOptionsData::Merge(const TagsOptionsData& tod)
    {
        wxArrayString temp1, temp2;
        StringUtils::MergeArrays(temp1, m_tokens, tod.m_tokens);
        StringUtils::MergeArrays(temp2, m_types, tod.m_types);
        m_tokens = temp1;
        m_types = temp2;
        DoUpdateTokensWxMapReversed();
        DoUpdateTokensWxMap();
        if(m_version != TagsOptionsData::CURRENT_VERSION) {
            m_ccFlags |= CC_WORD_ASSIST;
            m_ccNumberOfDisplayItems = tod.m_ccNumberOfDisplayItems;
        }
        m_version = TagsOptionsData::CURRENT_VERSION;
    }
    //-----------------------------------------------------------------------------------
    void TagsOptionsData::SyncData()
    {
        DoUpdateTokensWxMap();
        DoUpdateTokensWxMapReversed();
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // TagEntry
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    TagEntry::TagEntry(const tagEntry& entry)
        : m_isClangTag(false)
        , m_flags(0)
        , m_isCommentForamtted(false)
    {
        Create(entry);
    }
    //-----------------------------------------------------------------------------------
    TagEntry::TagEntry()
        : m_path(wxEmptyString)
        , m_file(wxEmptyString)
        , m_lineNumber(-1)
        , m_pattern(wxEmptyString)
        , m_kind(wxT("<unknown>"))
        , m_parent(wxEmptyString)
        , m_name(wxEmptyString)
        , m_id(wxNOT_FOUND)
        , m_scope(wxEmptyString)
        , m_differOnByLineNumber(false)
        , m_isClangTag(false)
        , m_flags(0)
        , m_isCommentForamtted(false)
    {
    }
    //-----------------------------------------------------------------------------------
    TagEntry::~TagEntry() {}
    //-----------------------------------------------------------------------------------
    TagEntry::TagEntry(const TagEntry& rhs) { *this = rhs; }
    //-----------------------------------------------------------------------------------
    TagEntry& TagEntry::operator=(const TagEntry& rhs)
    {
        m_id = rhs.m_id;
        m_file = rhs.m_file.c_str();
        m_kind = rhs.m_kind.c_str();
        m_parent = rhs.m_parent.c_str();
        m_pattern = rhs.m_pattern.c_str();
        m_lineNumber = rhs.m_lineNumber;
        m_name = rhs.m_name.c_str();
        m_path = rhs.m_path.c_str();
    #if wxUSE_GUI
        m_hti = rhs.m_hti;
    #endif
        m_scope = rhs.m_scope.c_str();
        m_isClangTag = rhs.m_isClangTag;
        m_differOnByLineNumber = rhs.m_differOnByLineNumber;
        m_flags = rhs.m_flags;
        m_formattedComment = rhs.m_formattedComment;
        m_isCommentForamtted = rhs.m_isCommentForamtted;

        // loop over the map and copy item by item
        // we use the c_str() method to force our own copy of the string and to avoid
        // ref counting which may cause crash when sharing wxString among threads
        m_extFields.clear();
        gsgsStringMap::const_iterator iter = rhs.m_extFields.begin();
        for(; iter != rhs.m_extFields.end(); iter++) {
            m_extFields[iter->first.c_str()] = iter->second.c_str();
        }
        m_comment = rhs.m_comment;
        return *this;
    }
    //-----------------------------------------------------------------------------------
    bool TagEntry::operator==(const TagEntry& rhs)
    {
        // Note: tree item id is not used in this function!
        bool res = m_scope == rhs.m_scope && m_file == rhs.m_file && m_kind == rhs.m_kind && m_parent == rhs.m_parent &&
                   m_pattern == rhs.m_pattern && m_name == rhs.m_name && m_path == rhs.m_path &&
                   m_lineNumber == rhs.m_lineNumber && GetInheritsAsString() == rhs.GetInheritsAsString() &&
                   GetAccess() == rhs.GetAccess() && GetSignature() == rhs.GetSignature() &&
                   GetTyperef() == rhs.GetTyperef();

        bool res2 = m_scope == rhs.m_scope && m_file == rhs.m_file && m_kind == rhs.m_kind && m_parent == rhs.m_parent &&
                    m_pattern == rhs.m_pattern && m_name == rhs.m_name && m_path == rhs.m_path &&
                    GetInheritsAsString() == rhs.GetInheritsAsString() && GetAccess() == rhs.GetAccess() &&
                    GetSignature() == rhs.GetSignature() && GetTyperef() == rhs.GetTyperef();

        if(res2 && !res) {
            // the entries are differs only in the line numbers
            m_differOnByLineNumber = true;
        }
        return res;
    }
    //-----------------------------------------------------------------------------------
    void TagEntry::Create(const wxString& fileName, const wxString& name, int lineNumber, const wxString& pattern,
                          const wxString& kind, gsgsStringMap& extFields)
    {
        m_isCommentForamtted = false;
        m_flags = 0;
        m_isClangTag = false;
        SetName(name);
        SetLine(lineNumber);
        SetKind(kind.IsEmpty() ? wxT("<unknown>") : kind);
        SetPattern(pattern);
        SetFile(fileName);
        SetId(-1);
        m_extFields = extFields;
        wxString path;

        // Check if we can get full name (including path)
        path = GetExtField(wxT("class"));
        if(!path.IsEmpty()) {
            UpdatePath(path);
        } else {
            path = GetExtField(wxT("struct"));
            if(!path.IsEmpty()) {
                UpdatePath(path);
            } else {
                path = GetExtField(wxT("namespace"));
                if(!path.IsEmpty()) {
                    UpdatePath(path);
                } else {
                    path = GetExtField(wxT("interface"));
                    if(!path.IsEmpty()) {
                        UpdatePath(path);
                    } else {
                        path = GetExtField(wxT("enum"));
                        if(!path.IsEmpty()) {
                            UpdatePath(path);
                        } else {
                            path = GetExtField(wxT("cenum"));
                            if(!path.IsEmpty()) {
                                UpdatePath(path);
                            } else {
                                path = GetExtField(wxT("union"));
                                wxString tmpname = path.AfterLast(wxT(':'));
                                if(!path.IsEmpty()) {
                                    if(!tmpname.StartsWith(wxT("__anon"))) {
                                        UpdatePath(path);
                                    } else {
                                        // anonymouse union, remove the anonymous part from its name
                                        path = path.BeforeLast(wxT(':'));
                                        path = path.BeforeLast(wxT(':'));
                                        UpdatePath(path);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if(!path.IsEmpty()) {
            SetScope(path);
        } else {
            SetScope(wxT("<global>"));
        }

        // If there is no path, path is set to name
        if(GetPath().IsEmpty()) SetPath(GetName());

        // Get the parent name
        StringTokenizer tok(GetPath(), wxT("::"));
        wxString parent;

        (tok.Count() < 2) ? parent = wxT("<global>") : parent = tok[tok.Count() - 2];
        SetParent(parent);
    }
    //-----------------------------------------------------------------------------------
    void TagEntry::Create(const tagEntry& entry)
    {
        m_isCommentForamtted = false;
        m_isClangTag = false;
        // Get other information from the string data and store it into map
        for(int i = 0; i < entry.fields.count; ++i) {
            wxString key = _U(entry.fields.list[i].key);
            wxString value = _U(entry.fields.list[i].value);
            m_extFields[key] = value;
        }
        Create(_U(entry.file), _U(entry.name), entry.address.lineNumber, _U(entry.address.pattern), _U(entry.kind),
               m_extFields);
    }
    //-----------------------------------------------------------------------------------
    void TagEntry::Print()
    {
        std::cout << "======================================" << std::endl;
        std::cout << "Name:\t\t" << GetName() << std::endl;
        std::cout << "File:\t\t" << GetFile() << std::endl;
        std::cout << "Line:\t\t" << GetLine() << std::endl;
        std::cout << "Pattern\t\t" << GetPattern() << std::endl;
        std::cout << "Kind:\t\t" << GetKind() << std::endl;
        std::cout << "Parent:\t\t" << GetParent() << std::endl;

        std::cout << " ---- Ext fields: ---- " << std::endl;
        gsgsStringMap::const_iterator iter = m_extFields.begin();
        for(; iter != m_extFields.end(); iter++)
            std::cout << iter->first << ":\t\t" << iter->second << std::endl;
        std::cout << "======================================" << std::endl;
    }
    //-----------------------------------------------------------------------------------
    wxString TagEntry::Key() const
    {
        wxString key;
        if(GetKind() == wxT("prototype") || GetKind() == wxT("macro")) {
            key << GetKind() << wxT(": ");
        }

        key << GetPath() << GetSignature();
        return key;
    }
    //-----------------------------------------------------------------------------------
    wxString TagEntry::GetDisplayName() const
    {
        wxString name;
        name << GetName() << GetSignature();
        return name;
    }
    //-----------------------------------------------------------------------------------
    wxString TagEntry::GetFullDisplayName() const
    {
        wxString name;

        if(GetParent() == wxT("<global>")) {
            name << GetDisplayName();
        } else {
            name << GetParent() << wxT("::") << GetName() << GetSignature();
        }

        return name;
    }
    //-----------------------------------------------------------------------------------
    wxString TagEntry::GetScopeName() const 
    { 
        return GetScope(); 
    }
    //-----------------------------------------------------------------------------------
    wxString TagEntry::GetKind() const
    {
        wxString kind(m_kind);
        kind.Trim();
        return kind;
    }
    //-----------------------------------------------------------------------------------
    const bool TagEntry::IsContainer() const
    {
        return GetKind() == wxT("class") || GetKind() == wxT("struct") || GetKind() == wxT("union") ||
               GetKind() == wxT("namespace") || GetKind() == wxT("project") || GetKind() == "cenum";
    }
    //-----------------------------------------------------------------------------------
    void TagEntry::UpdatePath(wxString& path)
    {
        if(!path.IsEmpty()) {
            wxString name(path);
            name += wxT("::");
            name += GetName();
            SetPath(name);
        }
    }
    //-----------------------------------------------------------------------------------
    wxString TagEntry::TypeFromTyperef() const
    {
        wxString typeref = GetTyperef();
        if(typeref.IsEmpty() == false) {
            wxString name = typeref.BeforeFirst(wxT(':'));
            return name;
        }
        return wxEmptyString;
    }
    //-----------------------------------------------------------------------------------
    static bool GetMacroArgList(CppScanner& scanner, wxArrayString& argList)
    {
        int depth(0);
        int type(0);
        bool cont(true);
        bool isValid(false);

        wxString word;

        while(cont) {
            type = scanner.yylex();
            if(type == 0) {
                // eof
                break;
            }

            switch(type) {
            case(int)'(':
                isValid = true;
                depth++;

                if(word.empty() == false) word << wxT("(");

                break;

            case(int)')':
                depth--;
                if(depth == 0) {
                    cont = false;
                    break;
                } else {
                    word << wxT(")");
                }
                break;

            case(int)',':
                word.Trim().Trim(false);
                if(!word.empty()) {
                    argList.Add(word);
                }
                word.clear();
                break;

            default:
                word << wxString::From8BitData(scanner.YYText()) << wxT(" ");
                break;
            }
        }

        if(word.empty() == false) {
            argList.Add(word);
        }

        return (depth == 0) && isValid;
    }
    //-----------------------------------------------------------------------------------
    wxString TagEntry::NameFromTyperef(wxString& templateInitList, bool nameIncludeTemplate)
    {
        wxString typeref = GetTyperef();
        if(typeref.IsEmpty() == false) {
            wxString name = typeref.AfterFirst(wxT(':'));
            return name;
        }

        // incase our entry is a typedef, and it is not marked as typeref,
        // try to get the real name from the pattern
        if(GetKind() == wxT("typedef")) {

            wxString pat(GetPattern());
            if(!GetPattern().Contains(wxT("typedef"))) {
                // The pattern does not contain 'typedef' however this *is* a typedef
                // try to see if this is a macro
                pat.StartsWith(wxT("/^"), &pat);
                pat.Trim().Trim(false);

                // we take the first token
                CppScanner scanner;
                scanner.SetText(pat.To8BitData());
                int type = scanner.yylex();
                if(type == IDENTIFIER) {
                    wxString token = wxString::From8BitData(scanner.YYText());

                    PPToken tok = TagsManagerST::Get()->GetDatabase()->GetMacro(token);
                    if(tok.flags & PPToken::IsValid) {
                        // we found a match!
                        if(tok.flags & PPToken::IsFunctionLike) {
                            wxArrayString argList;
                            if(GetMacroArgList(scanner, argList)) {
                                tok.expandOnce(argList);
                            }
                        }
                        pat = tok.replacement;
                        pat << wxT(";");

                        // Remove double spaces
                        while(pat.Replace(wxT("  "), wxT(" "))) {
                        }
                    }
                }
            }

            wxString name;
            if(TypedefFromPattern(pat, GetName(), name, templateInitList, nameIncludeTemplate)) return name;
        }

        return wxEmptyString;
    }
    //-----------------------------------------------------------------------------------
    bool TagEntry::TypedefFromPattern(const wxString& tagPattern, const wxString& typedefName, wxString& name,
                                      wxString& templateInit, bool nameIncludeTemplate)
    {
        wxString pattern(tagPattern);

        pattern.StartsWith(wxT("/^"), &pattern);
        const wxCharBuffer cdata = pattern.mb_str(wxConvUTF8);

        gsgs::TypedefList li;
        get_typedefs(cdata.data(), li);

        if(li.size() == 1) {
            Typedef td = *li.begin();
            templateInit = _U(td.m_realType.m_templateDecl.c_str());
            if(td.m_realType.m_typeScope.empty() == false) {
                name << _U(td.m_realType.m_typeScope.c_str());
                if(nameIncludeTemplate) {
                    name << templateInit;
                }
                name << wxT("::");
            }

            name << _U(td.m_realType.m_type.c_str());
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------------
    wxString TagEntry::GetPattern() const
    {
        wxString pattern(m_pattern);
        // since ctags's pattern is regex, forward slashes are escaped. ('/' becomes '\/')
        pattern.Replace(wxT("\\\\"), wxT("\\"));
        pattern.Replace(wxT("\\/"), wxT("/"));
        return pattern;
    }
    //-----------------------------------------------------------------------------------
    void TagEntry::FromLine(const wxString& line)
    {
        wxString pattern, kind;
        wxString strLine = line;
        long lineNumber = wxNOT_FOUND;
        gsgsStringMap extFields;

        // get the token name
        wxString name = strLine.BeforeFirst(wxT('\t'));
        strLine = strLine.AfterFirst(wxT('\t'));

        // get the file name
        wxString fileName = strLine.BeforeFirst(wxT('\t'));
        strLine = strLine.AfterFirst(wxT('\t'));

        // here we can get two options:
        // pattern followed by ;"
        // or
        // line number followed by ;"
        int end = strLine.Find(wxT(";\""));
        if(end == wxNOT_FOUND) {
            // invalid pattern found
            return;
        }

        if(strLine.StartsWith(wxT("/^"))) {
            // regular expression pattern found
            pattern = strLine.Mid(0, end);
            strLine = strLine.Right(strLine.Length() - (end + 2));
        } else {
            // line number pattern found, this is usually the case when
            // dealing with macros in C++
            pattern = strLine.Mid(0, end);
            strLine = strLine.Right(strLine.Length() - (end + 2));

            pattern = pattern.Trim();
            pattern = pattern.Trim(false);
            pattern.ToLong(&lineNumber);
        }

        // next is the kind of the token
        if(strLine.StartsWith(wxT("\t"))) {
            strLine = strLine.AfterFirst(wxT('\t'));
        }

        kind = strLine.BeforeFirst(wxT('\t'));
        strLine = strLine.AfterFirst(wxT('\t'));

        if(strLine.IsEmpty() == false) {
            wxStringTokenizer tkz(strLine, wxT('\t'));
            while(tkz.HasMoreTokens()) {
                wxString token = tkz.NextToken();
                wxString key = token.BeforeFirst(wxT(':'));
                wxString val = token.AfterFirst(wxT(':'));
                key = key.Trim();
                key = key.Trim(false);

                val = val.Trim();
                val = val.Trim(false);
                if(key == wxT("line") && !val.IsEmpty()) {
                    val.ToLong(&lineNumber);
                } else {
                    if(key == wxT("union") || key == wxT("struct")) {

                        // remove the anonymous part of the struct / union
                        if(!val.StartsWith(wxT("__anon"))) {
                            // an internal anonymous union / struct
                            // remove all parts of the
                            wxArrayString scopeArr;
                            wxString tmp, new_val;

                            scopeArr = wxStringTokenize(val, wxT(":"), wxTOKEN_STRTOK);
                            for(size_t i = 0; i < scopeArr.GetCount(); i++) {
                                if(scopeArr.Item(i).StartsWith(wxT("__anon")) == false) {
                                    tmp << scopeArr.Item(i) << wxT("::");
                                }
                            }

                            tmp.EndsWith(wxT("::"), &new_val);
                            val = new_val;
                        }
                    }

                    extFields[key] = val;
                }
            }
        }

        kind = kind.Trim();
        name = name.Trim();
        fileName = fileName.Trim();
        pattern = pattern.Trim();

        if(kind == "enumerator" && extFields.count("enum")) {
            // Remove the last parent
            wxString& scope = extFields["enum"];
            size_t where = scope.rfind("::");
            if(where != wxString::npos) {
                scope = scope.Mid(0, where);
            } else {
                // Global enum, remove this ext field
                extFields.erase("enum");
            }
        }

        //    if(kind == wxT("enumerator")) {
        //        // enums are specials, they are a scope, when they declared as "enum class ..." (C++11),
        //        // but not a scope when declared as "enum ...". So, for "enum class ..." declaration
        //        //(and anonymous enums) we appear enumerators when typed:
        //        // enumName::
        //        // Is global scope there aren't appears. For "enum ..." declaration we appear
        //        // enumerators when typed:
        //        // enumName::
        //        // and when it global (or same namespace) scope.
        //        gsgsStringMap::iterator enumField = extFields.find(wxT("enum"));
        //        if(enumField != extFields.end()) {
        //            wxString enumName = enumField->second;
        //            bool isAnonymous = enumName.AfterLast(wxT(':')).StartsWith(wxT("__anon"));
        //
        //            bool isInEnumNamespace = false;
        //            gsgsStringMap::const_iterator isInEnumNamespaceField = extFields.find(wxT("isInEnumNamespace"));
        //            if(isInEnumNamespaceField != extFields.end()) {
        //                wxString isInEnumNamespaceValue = isInEnumNamespaceField->second;
        //                isInEnumNamespace = isInEnumNamespaceValue.AfterLast(wxT(':')) == wxT("1") ? true : false;
        //            }
        //
        //            if(!isInEnumNamespace) {
        //                enumField->second = enumField->second.BeforeLast(wxT(':')).BeforeLast(wxT(':'));
        //                if(!isAnonymous) {
        //                    extFields[wxT("typeref")] = enumName;
        //                }
        //            }
        //        }
        //    }

        this->Create(fileName, name, lineNumber, pattern, kind, extFields);
    }
    //-----------------------------------------------------------------------------------
    bool TagEntry::IsConstructor() const
    {
        if(GetKind() != wxT("function") && GetKind() != wxT("prototype")) return false;

        return GetName() == GetScope();
    }
    //-----------------------------------------------------------------------------------
    bool TagEntry::IsDestructor() const
    {
        if(GetKind() != wxT("function") && GetKind() != wxT("prototype")) return false;

        return GetName().StartsWith(wxT("~"));
    }
    //-----------------------------------------------------------------------------------
    wxString TagEntry::GetReturnValue() const
    {
        wxString returnValue = GetExtField(_T("returns"));
        returnValue.Trim().Trim(false);
        returnValue.Replace(wxT("virtual"), wxT(""));
        return returnValue;
    }
    //-----------------------------------------------------------------------------------
    bool TagEntry::IsFunction() const { return GetKind() == wxT("function"); }
    //-----------------------------------------------------------------------------------
    bool TagEntry::IsMethod() const { return IsPrototype() || IsFunction(); }
    //-----------------------------------------------------------------------------------
    bool TagEntry::IsPrototype() const { return GetKind() == wxT("prototype"); }
    //-----------------------------------------------------------------------------------
    bool TagEntry::IsClass() const { return GetKind() == wxT("class"); }
    //-----------------------------------------------------------------------------------
    bool TagEntry::IsMacro() const { return GetKind() == wxT("macro"); }
    //-----------------------------------------------------------------------------------
    bool TagEntry::IsStruct() const { return GetKind() == wxT("struct"); }
    //-----------------------------------------------------------------------------------
    bool TagEntry::IsScopeGlobal() const { return GetScope().IsEmpty() || GetScope() == wxT("<global>"); }
    //-----------------------------------------------------------------------------------
    bool TagEntry::IsTypedef() const { return GetKind() == wxT("typedef"); }
    //-----------------------------------------------------------------------------------
    wxString TagEntry::GetInheritsAsString() const { return GetExtField(_T("inherits")); }
    //-----------------------------------------------------------------------------------
    wxArrayString TagEntry::GetInheritsAsArrayNoTemplates() const
    {
        // Parse the input string
        wxString inherits = GetInheritsAsString();
        wxString parent;
        wxArrayString parentsArr;

        int depth(0);
        for(size_t i = 0; i < inherits.Length(); i++) {
            wxChar ch = inherits.GetChar(i);

            switch(ch) {
            case wxT('<'):
                if(depth == 0 && parent.IsEmpty() == false) {
                    parent.Trim().Trim(false);
                    parentsArr.Add(parent);
                    parent.Clear();
                }
                depth++;
                break;

            case wxT('>'):
                depth--;
                break;

            case wxT(','):
                if(depth == 0 && parent.IsEmpty() == false) {
                    parent.Trim().Trim(false);
                    parentsArr.Add(parent);
                    parent.Clear();
                }
                break;

            default:
                if(depth == 0) {
                    parent << ch;
                }
                break;
            }
        }

        if(parent.IsEmpty() == false) {
            parent.Trim().Trim(false);
            parentsArr.Add(parent);
        }
        return parentsArr;
    }
    //-----------------------------------------------------------------------------------
    wxArrayString TagEntry::GetInheritsAsArrayWithTemplates() const
    {
        // Parse the input string
        wxString inherits = GetInheritsAsString();
        wxString parent;
        wxArrayString parentsArr;

        int depth(0);
        for(size_t i = 0; i < inherits.Length(); i++) {
            wxChar ch = inherits.GetChar(i);

            switch(ch) {
            case wxT('<'):
                depth++;
                parent << ch;
                break;

            case wxT('>'):
                depth--;
                parent << ch;
                break;

            case wxT(','):
                if(depth == 0 && parent.IsEmpty() == false) {
                    parent.Trim().Trim(false);
                    parentsArr.Add(parent);
                    parent.Clear();

                } else if(depth != 0) {
                    parent << ch;
                }
                break;

            default:
                parent << ch;
                break;
            }
        }

        if(parent.IsEmpty() == false) {
            parent.Trim().Trim(false);
            parentsArr.Add(parent);
        }
        return parentsArr;
    }
    //-----------------------------------------------------------------------------------
    TagEntryPtr TagEntry::ReplaceSimpleMacro()
    {
        if(IsMacro()) {
            PPToken tok = TagsManagerST::Get()->GetDatabase()->GetMacro(GetName());
            if(tok.flags & PPToken::IsValid && !(tok.flags & PPToken::IsFunctionLike)) {
                std::vector<TagEntryPtr> tags;
                TagsManagerST::Get()->FindByNameAndScope(tok.replacement, GetScopeName(), tags);
                if(tags.size() == 1) {
                    // replace the current tag content with the new match
                    return tags.at(0);
                }
            }
        }
        return NULL;
    }
    //-----------------------------------------------------------------------------------
    int TagEntry::CompareDisplayString(const TagEntryPtr& rhs) const
    {
        wxString d1, d2;

        d1 << GetReturnValue() << wxT(": ") << GetFullDisplayName() << wxT(":") << GetAccess();
        d2 << rhs->GetReturnValue() << wxT(": ") << rhs->GetFullDisplayName() << wxT(":") << rhs->GetAccess();
        return d1.Cmp(d2);
    }
    //-----------------------------------------------------------------------------------
    bool TagEntry::IsTemplateFunction() const
    {
        wxString pattern = GetPatternClean();
        pattern.Trim().Trim(false);
        return IsMethod() && pattern.StartsWith("template ");
    }
    //-----------------------------------------------------------------------------------
    wxString TagEntry::GetPatternClean() const
    {
        wxString p = GetPattern();
        p.Trim();
        if(p.StartsWith(wxT("/^"))) {
            p.Replace(wxT("/^"), wxT(""));
        }

        if(p.EndsWith(wxT("$/"))) {
            p.Replace(wxT("$/"), wxT(""));
        }
        return p;
    }
    //-----------------------------------------------------------------------------------
    wxString TagEntry::FormatComment()
    {
        if(m_isCommentForamtted) return m_formattedComment;
        m_isCommentForamtted = true;
        m_formattedComment.Clear();

        // Send the plugins an event requesting tooltip for this tag
        if(IsMethod()) {

            if(IsConstructor())
                m_formattedComment << wxT("<b>[Constructor]</b>\n");

            else if(IsDestructor())
                m_formattedComment << wxT("<b>[Destructor]</b>\n");

            TagEntryPtr p(new TagEntry(*this));
            m_formattedComment << wxT("<code>")
                               << TagsManagerST::Get()->FormatFunction(p, FunctionFormat_WithVirtual |
                                                                              FunctionFormat_Arg_Per_Line)
                               << wxT("</code>\n");
            m_formattedComment.Replace(GetName(), wxT("<b>") + GetName() + wxT("</b>"));
        } else if(IsClass()) {

            m_formattedComment << wxT("<b>Kind:</b> ");
            m_formattedComment << GetKind() << "\n";

            if(GetInheritsAsString().IsEmpty() == false) {
                m_formattedComment << wxT("<b>Inherits:</b> ");
                m_formattedComment << GetInheritsAsString() << wxT("\n");
            }

        } else if(IsMacro() || IsTypedef() || IsContainer() || GetKind() == wxT("member") || GetKind() == wxT("variable")) {

            m_formattedComment << wxT("<b>Kind:</b> ");
            m_formattedComment << GetKind() << "\n";

            m_formattedComment << wxT("<b>Match Pattern:</b> ");

            // Prettify the match pattern
            wxString matchPattern(GetPattern());
            matchPattern.Trim().Trim(false);

            if(matchPattern.StartsWith(wxT("/^"))) {
                matchPattern.Replace(wxT("/^"), wxT(""));
            }

            if(matchPattern.EndsWith(wxT("$/"))) {
                matchPattern.Replace(wxT("$/"), wxT(""));
            }

            matchPattern.Replace(wxT("\t"), wxT(" "));
            while(matchPattern.Replace(wxT("  "), wxT(" "))) {
            }

            matchPattern.Trim().Trim(false);

            // BUG#3082954: limit the size of the 'match pattern' to a reasonable size (200 chars)
            matchPattern = TagsManagerST::Get()->WrapLines(matchPattern);
            matchPattern.Replace(GetName(), wxT("<b>") + GetName() + wxT("</b>"));
            m_formattedComment << wxT("<code>") << matchPattern << wxT("</code>\n");
        }

        // Add comment section
        wxString tagComment;
        if(!GetFile().IsEmpty()) {

            CommentParseResult comments;
            ::ParseComments(GetFile().mb_str(wxConvUTF8).data(), comments);

            // search for comment in the current line, the line above it and 2 above it
            // use the first match we got
            for(size_t i = 0; i < 3; i++) {
                wxString comment = comments.getCommentForLine(GetLine() - i);
                if(!comment.IsEmpty()) {
                    SetComment(comment);
                    break;
                }
            }
        }

        if(!GetComment().IsEmpty()) {
            wxString theComment;
            theComment = GetComment();

            theComment = TagsManagerST::Get()->WrapLines(theComment);
            theComment.Trim(false);
            wxString tagComment = wxString::Format(wxT("%s\n"), theComment.c_str());
            if(m_formattedComment.IsEmpty() == false) {
                m_formattedComment.Trim().Trim(false);
                m_formattedComment << wxT("\n<hr>");
            }
            m_formattedComment << tagComment;
        }

        // Update all "doxy" comments and surround them with <green> tags
        static wxRegEx reDoxyParam("([@\\\\]{1}param)[ \t]+([_a-z][a-z0-9_]*)?", wxRE_DEFAULT | wxRE_ICASE);
        static wxRegEx reDoxyBrief("([@\\\\]{1}(brief|details))[ \t]*", wxRE_DEFAULT | wxRE_ICASE);
        static wxRegEx reDoxyThrow("([@\\\\]{1}(throw|throws))[ \t]*", wxRE_DEFAULT | wxRE_ICASE);
        static wxRegEx reDoxyReturn("([@\\\\]{1}(return|retval|returns))[ \t]*", wxRE_DEFAULT | wxRE_ICASE);
        static wxRegEx reDoxyToDo("([@\\\\]{1}todo)[ \t]*", wxRE_DEFAULT | wxRE_ICASE);
        static wxRegEx reDoxyRemark("([@\\\\]{1}(remarks|remark))[ \t]*", wxRE_DEFAULT | wxRE_ICASE);
        static wxRegEx reDate("([@\\\\]{1}date)[ \t]*", wxRE_DEFAULT | wxRE_ICASE);
        static wxRegEx reFN("([@\\\\]{1}fn)[ \t]*", wxRE_DEFAULT | wxRE_ICASE);

        if(reDoxyParam.IsValid() && reDoxyParam.Matches(m_formattedComment)) {
            reDoxyParam.ReplaceAll(&m_formattedComment, "\n<b>Parameter</b>\n<i>\\2</i>");
        }

        if(reDoxyBrief.IsValid() && reDoxyBrief.Matches(m_formattedComment)) {
            reDoxyBrief.ReplaceAll(&m_formattedComment, "");
        }

        if(reDoxyThrow.IsValid() && reDoxyThrow.Matches(m_formattedComment)) {
            reDoxyThrow.ReplaceAll(&m_formattedComment, "\n<b>Throws</b>\n");
        }

        if(reDoxyReturn.IsValid() && reDoxyReturn.Matches(m_formattedComment)) {
            reDoxyReturn.ReplaceAll(&m_formattedComment, "\n<b>Returns</b>\n");
        }

        if(reDoxyToDo.IsValid() && reDoxyToDo.Matches(m_formattedComment)) {
            reDoxyToDo.ReplaceAll(&m_formattedComment, "\n<b>TODO</b>\n");
        }

        if(reDoxyRemark.IsValid() && reDoxyRemark.Matches(m_formattedComment)) {
            reDoxyRemark.ReplaceAll(&m_formattedComment, "\n  ");
        }

        if(reDate.IsValid() && reDate.Matches(m_formattedComment)) {
            reDate.ReplaceAll(&m_formattedComment, "<b>Date</b> ");
        }

        if(reFN.IsValid() && reFN.Matches(m_formattedComment)) {
            size_t fnStart, fnLen, fnEnd;
            if(reFN.GetMatch(&fnStart, &fnLen)) {
                fnEnd = m_formattedComment.find('\n', fnStart);
                if(fnEnd != wxString::npos) {
                    // remove the string from fnStart -> fnEnd (including ther terminating \n)
                    m_formattedComment.Remove(fnStart, (fnEnd - fnStart) + 1);
                }
            }
        }

        // if nothing to display skip this
        m_formattedComment.Trim().Trim(false);
        return m_formattedComment;
    }
    //-----------------------------------------------------------------------------------
    //#ifdef __VISUALC__
    //#ifdef _DEBUG
    //#define new DEBUG_NEW
    //#endif
    //#endif

    //-----------------------------------------------------------------------------------
    TagTree::TagTree(const wxString& key, const TagEntry& data)
        : Tree<wxString, TagEntry>(key, data)
    {
    }
    //-----------------------------------------------------------------------------------
    TagTree::~TagTree()
    {
    }
    //-----------------------------------------------------------------------------------
    /// Util function to help us to build a tree from tags
    TagNode* TagTree::AddEntry(TagEntry& tag)
    {
        // If a node with thig tag already exist, we simply updating the 
        // data 
        wxString key(tag.Key());

        TagNode* newNode = Find(key);
        if (newNode)
        {
            if (tag.IsOk())
                newNode->SetData(tag);
            return newNode;

        }

        // To add an entry to the tree, we first must make sure
        // that all path to it exist
        wxString name = tag.GetPath();
        StringTokenizer tok(name, wxT("::"));

        wxString parentPath;
        TagNode* node = GetRoot();
        TagNode* lastFoundNode = GetRoot();
        for (int i = 0; i < tok.Count() - 1; i++)
        {
            parentPath += tok[i];

            // Try to find this node in the tree
            node = Find(parentPath);
            if (!node)
            {
                // Node does not exist add it, we copy key values
                // from 'tag'
                TagEntry ee;
                ee.SetPath(parentPath);
                ee.SetName(tok[i]);
                node = AddChild(parentPath, ee, lastFoundNode);
            }

            lastFoundNode = node;
            if (i < tok.Count() - 2)
                parentPath += _T("::");
        }

        return AddChild(key, tag, node);
    }
    //-----------------------------------------------------------------------------------
    //#ifdef __VISUALC__
    //#ifdef _DEBUG
    //#define new DEBUG_NEW
    //#endif
    //#endif 

    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////

    StringTokenizer::StringTokenizer(const wxString& str,
                                     const wxString& strDelimiter,
                                     const bool &bAllowEmptyTokens /* false */)
    {
        Initialize();

        int nEnd = (int)str.find(strDelimiter, 0);
        int nStart = 0;
        wxString token;
        while( nEnd != -1 )
        {
            if( nEnd != nStart)
                token = str.substr(nStart, nEnd-nStart);
            else
                token.Empty();

            if(!token.empty())
                m_tokensArr.push_back(token);
            else if( bAllowEmptyTokens )
                m_tokensArr.push_back(token);

            // next token
            nStart = nEnd + (int)strDelimiter.length();
            nEnd = (int)str.find(strDelimiter, nStart );
        }
        if( nStart != (int)str.length() )
        {
            //We have another token which is not delimited 
            wxString token = str.substr(nStart);
            m_tokensArr.push_back(token);
        }
    }
    //-----------------------------------------------------------------------------------
    StringTokenizer::StringTokenizer(const wxString& str, const wxArrayString& delimiterArr, const bool &allowEmptyTokens)
    {
        Initialize();
        wxString tmpStr( str );

        // Replace all delimiters to the first one
        if(delimiterArr.GetCount() >= 2)
        {
            size_t i=1;
            for(; i<delimiterArr.GetCount(); i++)
                tmpStr.Replace(delimiterArr[i], delimiterArr[0]);
        }
        *this = StringTokenizer(tmpStr, delimiterArr[0], allowEmptyTokens);
    }
    //-----------------------------------------------------------------------------------
    StringTokenizer::StringTokenizer()
    {
        Initialize();
    }
    //-----------------------------------------------------------------------------------
    StringTokenizer::StringTokenizer(const StringTokenizer &src)
    {
        *this = src;
    }
    //-----------------------------------------------------------------------------------
    StringTokenizer& StringTokenizer::operator =(const StringTokenizer &src)
    {
        if( &src == this)
            return *this;
        Initialize();

        // Copy the tokens
        m_tokensArr.clear();
        for( int i=0; i<(int)src.m_tokensArr.size() ; i++)
            m_tokensArr.push_back(src.m_tokensArr[i]);
        m_nCurr = src.m_nCurr;
        return *this;
    }
    //-----------------------------------------------------------------------------------
    StringTokenizer::~StringTokenizer()
    {
        m_tokensArr.clear();
        m_nCurr = 0;
    }
    //-----------------------------------------------------------------------------------
    // Return current token and advance to the next. If there are no more tokens, return empty string.
    wxString StringTokenizer::Next()
    {
        if( m_nCurr == (int)m_tokensArr.size() )
        {
            // We are at the end of the tokens array
            return wxEmptyString;
        }
        wxString strToken = m_tokensArr[m_nCurr];
        m_nCurr++;
        return strToken;
    }
    //-----------------------------------------------------------------------------------
    // We return the previous token, if we are already at the start, return empty string.
    wxString StringTokenizer::Previous()
    {
        if(m_nCurr == 0)
        {
            return wxEmptyString;
        }
        if(m_tokensArr.size() == 0)
        {
            return wxEmptyString;
        }
        m_nCurr--;
        return m_tokensArr[m_nCurr];
    }
    //-----------------------------------------------------------------------------------
    bool StringTokenizer::HasMore()
    {
        if(m_nCurr < (int)m_tokensArr.size())
        {
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------------
    wxString StringTokenizer::First()
    {
        if(m_tokensArr.size()>0)
        {
            m_nCurr = 1;	//The next one
            return m_tokensArr[0];
        }
        else
        {
            return wxEmptyString;
        }
    }
    //-----------------------------------------------------------------------------------
    // This function is much similar to the 'next()' function with one difference, 
    // it does not advance the pointer to the next token.
    wxString StringTokenizer::Current()
    {
        if( m_nCurr == (int)m_tokensArr.size() )
        {
            // We are at the end of the tokens array
            return wxEmptyString;
        }
        return m_tokensArr[m_nCurr];
    }
    //-----------------------------------------------------------------------------------
    const int StringTokenizer::Count() const
    {
        return (int)m_tokensArr.size();
    }
    //-----------------------------------------------------------------------------------
    void StringTokenizer::Initialize()
    {
        m_tokensArr.clear();
        m_nCurr = 0;
    }
    //-----------------------------------------------------------------------------------
    wxString StringTokenizer::Last()
    {
        if(m_tokensArr.size() == 0)
            return wxEmptyString;
        m_nCurr = (int)m_tokensArr.size()-1;
        return m_tokensArr[m_tokensArr.size()-1];
    }
    //-----------------------------------------------------------------------------------
    wxString StringTokenizer::operator[](const int nIndex)
    {
        if(m_tokensArr.size() == 0)
            return wxEmptyString;
        if( nIndex>=(int)m_tokensArr.size() || nIndex<0)
            return wxEmptyString;
        return m_tokensArr[nIndex];
    }
    //-----------------------------------------------------------------------------------
}