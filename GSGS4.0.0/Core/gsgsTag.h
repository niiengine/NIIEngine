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
#ifndef gsgs_Tag_H
#define gsgs_Tag_H

#if wxUSE_GUI
#include <wx/treectrl.h>
#endif

#include "gsgsPreInclude.h"
#include "readtags.h"
#include "gsgsTree.h"
#include "gsgsConfig.h"

namespace gsgs
{
    typedef SharedPtr<TagEntry> TagEntryPtr;
    typedef std::vector<TagEntryPtr> TagEntryPtrList;

    #define KIND_CLASS "class"
    #define KIND_ENUM "enum"
    #define KIND_CLASS_ENUM "cenum"
    #define KIND_ENUMERATOR "enumerator"
    #define KIND_FUNCTION "function"
    #define KIND_PROTOTYPE "prototype"
    #define KIND_MEMBER "member"
    #define KIND_NAMESPACE "namespace"
    #define KIND_VARIABLE "variable"
    #define KIND_UNION "union"
    #define KIND_TYPEDEF "typedef"
    #define KIND_MACRO "macro"
    #define KIND_STRUCT "struct"
    #define KIND_FILE "file"

    /**
     * StringTokenizer helps you to break a string up into a number of tokens. 
     * It replaces the standard C function strtok() and also extends it in a number of ways.
     *
     * Usage example:
     *
     * \code
     * StringTokenizer tok(_T("first second third"), _T(" "));
     * while( tok.HasMore() )
     * {
     *		wxString token = tok.Next();
     * }
     * \endcode
     * 
     * An alternate way to iterate over the tokenizer:
     *
     * \code 
     * StringTokenizer tok(_T("first second third"), _T(" "));
     * for(int i=0; i<tok.GetCount(); i++)
     * {
     *		wxString token = tok[i];
     * }
     * \endcode
     *
     * StringTokenizer also allows you to break string with number of delimiters:
     *
     * \code
     * wxArrayString dlimArr;
     * delimArr.Add("_T(" "));
     * delimArr.Add("_T("-"));
     * delimArr.Add("_T(":"));
     * StringTokenizer tok(_T("first-second:third"), delimArr);
     * \endcode 
     * 
     * The above exmple will result with: first, second and third.
     *
     * \ingroup gsgs
     * \version 1.0
     * first version
     *
     * \date 09-02-2006
     * \author Eran
     */
    class _gsgsAPI StringTokenizer  
    {
    public:
        /**
         * Construct a tokenizer with given input string and delimiter
         * \param str source string
         * \param delimiter delimiter to use
         * \param bAllowEmptyTokens if set to true, empty tokens will be returned as well. Default is no empty tokens
         */
        StringTokenizer(const wxString& str,const wxString& delimiter = _T(" ") , const bool &bAllowEmptyTokens = false);

        /**
         * Construct a tokenizer with given input string and array of delimiters
         * \param str source string
         * \param delimiterArr array of delimiters
         * \param bAllowEmptyTokens if set to true, empty tokens will be returned as well. Default is no empty tokens
         */
        StringTokenizer(const wxString& str,const wxArrayString& delimiterArr, const bool &bAllowEmptyTokens = false);

        StringTokenizer(const StringTokenizer& src);
        StringTokenizer();
        virtual ~StringTokenizer();

        //-----------------------------------------------------
        // Operations
        //-----------------------------------------------------

        /**
         * Return the last token 
         * \return last token
         */
        wxString Last();
        /**
         * Get the number of tokens
         * \return number of tokens
         */
        const int Count() const;
        /**
         * Return the current token without progressing the internal pointer
         * \return current token
         */
        wxString Current();

        /**
         * Return the first token and progress the internal pointer
         * \return First token
         */
        wxString First();
        /**
         * Do we have more tokens?
         * \return true if the next call to Next() will return a token
         */
        bool HasMore();
        /** 
         * Return the previous string, this function moves the internal pointer backward.
         * So, subsequent calls to this function, will eventually return us to the start of the tokenizer
         * \return previous token of empty string if we are already at start
         */
        wxString Previous();
        /**
         * Return next token, usually combined with HasMore() function. This function progress the 
         * internal pointer to next token
         * \return next token
         */
        wxString Next();
        /**
         * Random acess operator, statring from zero
         * \param nIndex token index
         * \return token at nIndex (copy of it)
         */
        wxString operator[](const int nIndex);

        /**
         * Copy one tokenizer to another
         * \param src source tokenizer to copy from
         * \return this
         */
        StringTokenizer& operator=(const StringTokenizer& src);

    private:
        void Initialize();
    private:
        std::vector<wxString> m_tokensArr;
        int m_nCurr;
    };

    enum CodeCompletionOpts {
        CC_PARSE_COMMENTS               = 0x00000001,
        CC_DISP_COMMENTS                = 0x00000002,
        CC_DISP_TYPE_INFO               = 0x00000004,
        CC_DISP_FUNC_CALLTIP            = 0x00000008,
        CC_LOAD_EXT_DB                  = 0x00000010,
        CC_AUTO_INSERT_SINGLE_CHOICE    = 0x00000020,
        CC_PARSE_EXT_LESS_FILES         = 0x00000040,
        CC_COLOUR_VARS                  = 0x00000080,
        CC_CPP_KEYWORD_ASISST           = 0x00000200,
        CC_WORD_ASSIST                  = 0x00000400,
        CC_DISABLE_AUTO_PARSING         = 0x00000800,
        CC_MARK_TAGS_FILES_IN_BOLD      = 0x00001000,
        CC_RETAG_WORKSPACE_ON_STARTUP   = 0x00004000,
        CC_ACCURATE_SCOPE_RESOLVING     = 0x00008000,
        CC_DEEP_SCAN_USING_NAMESPACE_RESOLVING = 0x00010000,
        CC_IS_CASE_SENSITIVE            = 0x00020000,
        CC_KEEP_FUNCTION_SIGNATURE_UNFORMATTED = 0x00040000
    };

    enum CodeCompletionColourOpts {
        CC_COLOUR_CLASS = 0x00000001,
        CC_COLOUR_STRUCT = 0x00000002,
        CC_COLOUR_FUNCTION = 0x00000004,
        CC_COLOUR_ENUM = 0x00000008,
        CC_COLOUR_UNION = 0x00000010,
        CC_COLOUR_PROTOTYPE = 0x00000020,
        CC_COLOUR_TYPEDEF = 0x00000040,
        CC_COLOUR_MACRO = 0x00000080,
        CC_COLOUR_NAMESPACE = 0x00000100,
        CC_COLOUR_ENUMERATOR = 0x00000200,
        CC_COLOUR_VARIABLE = 0x00000400,
        CC_COLOUR_MEMBER = 0x00000800,
        CC_COLOUR_MACRO_BLOCKS = 0x00001000,
        CC_COLOUR_ALL = CC_COLOUR_CLASS | CC_COLOUR_STRUCT | CC_COLOUR_FUNCTION | CC_COLOUR_ENUM | CC_COLOUR_UNION |
                        CC_COLOUR_PROTOTYPE | CC_COLOUR_TYPEDEF | CC_COLOUR_MACRO | CC_COLOUR_NAMESPACE |
                        CC_COLOUR_ENUMERATOR | CC_COLOUR_VARIABLE | CC_COLOUR_MEMBER,
        CC_COLOUR_DEFAULT = CC_COLOUR_CLASS | CC_COLOUR_STRUCT | CC_COLOUR_NAMESPACE | CC_COLOUR_ENUM | CC_COLOUR_TYPEDEF
    };

    enum CodeCompletionClangOptions {
        CC_CLANG_ENABLED            = (1 << 0),
        CC_CLANG_FIRST              = (1 << 1),
        CC_CTAGS_ENABLED            = (1 << 2),
        CC_CLANG_INLINE_ERRORS      = (1 << 3),
    };

    class _gsgsAPI TagsOptionsData : public gsgs::ObjectData
    {
    public:
        TagsOptionsData();
        virtual ~TagsOptionsData();

        void Merge(const TagsOptionsData& tod);
        void SetVersion(size_t version)                                     { m_version = version; }

        inline void SetCcNumberOfDisplayItems(size_t cnt)                   { m_ccNumberOfDisplayItems = cnt; }
        inline size_t GetCcNumberOfDisplayItems() const                     { return m_ccNumberOfDisplayItems; }

        inline void SetClangCachePolicy(const wxString& str)                { m_clangCachePolicy = str; }

        inline const wxString& GetClangCachePolicy() const                  { return m_clangCachePolicy; }

        inline void SetClangMacros(const wxString & clangMacros)            { m_clangMacros = clangMacros; }
        inline void SetClangSearchPaths(const wxString & paths)             { m_clangSearchPaths = ::wxStringTokenize(paths, "\r\n", wxTOKEN_STRTOK); }

        inline const wxString & GetClangMacros() const                      { return m_clangMacros; }
        inline wxString GetClangSearchPaths() const                         { return DoJoinArray(m_clangSearchPaths); }

        inline const wxArrayString& GetClangSearchPathsArray() const        { return m_clangSearchPaths; }

        inline void SetClangSearchPathsArray(const wxArrayString& paths)    { m_clangSearchPaths = paths; }

        inline void SetClangCmpOptions(const wxString& opts)                { m_clangCmpOptions = opts; }
        inline const wxString& GetClangCmpOptions() const                   { return m_clangCmpOptions; }
        inline void SetClangOptions(size_t clangOptions)                    { m_clangOptions = clangOptions; }
        inline size_t GetClangOptions() const                               { return m_clangOptions; }
        inline void SetClangBinary(const wxString& clangBinary)             { m_clangBinary = clangBinary; }
        inline const wxString& GetClangBinary() const                       { return m_clangBinary; }
        inline void SetFlags(size_t flags)                                  { m_ccFlags = flags; }

        inline void SetFileSpec(const wxString& filespec)                   { m_fileSpec = filespec; }

        inline void SetLanguages(const wxArrayString& langs)                { m_languages = langs; }

        inline void SetLanguageSelection(const wxString& lang);

        inline const wxArrayString& GetLanguages() const                    { return m_languages; }

        void SetTokens(const wxString& tokens);
        inline void SetTypes(const wxString& types)                         { m_types = ::wxStringTokenize(types, "\r\n", wxTOKEN_STRTOK); }
        inline wxString GetTokens() const                                   { return DoJoinArray(m_tokens); }
        inline wxString GetTypes() const                                    { return DoJoinArray(m_types); }

        std::map<std::string, std::string> GetTokensMap() const;

        inline const gsgsStringMap& GetTokensReversedWxMap() const          { return m_tokensWxMapReversed; }
        inline const gsgsStringMap& GetTokensWxMap() const                  { return m_tokensWxMap; }

        gsgsStringMap GetTypesMap() const;

        inline size_t GetFlags() const                                      { return m_ccFlags; }
        inline const wxString& GetFileSpec() const                          { return m_fileSpec; }

        // Setters
        inline void SetMinWordLen(int minWordLen)                           { m_minWordLen = minWordLen; }
        // Getters
        inline int GetMinWordLen() const                                    { return m_minWordLen; }

        // Serialization API

        wxString ToString() const;
        
        /**
         * @brief Sync the data stored in this object with the file system 
         */
        void SyncData();

        inline void SetCcColourFlags(size_t ccColourFlags)                  { m_ccColourFlags = ccColourFlags; }

        inline size_t GetCcColourFlags() const                              { return m_ccColourFlags; }

        inline void SetParserEnabled(bool set)                              { m_parserEnabled = set; }

        inline bool GetParserEnabled() const                                { return m_parserEnabled; }

        inline void SetParserSearchPaths(const wxArrayString & list)        { m_parserSearchPaths = list; }

        inline const wxArrayString& GetParserSearchPaths() const            { return m_parserSearchPaths; }

        inline void SetParserExcludePaths(const wxArrayString& list)        { m_parserExcludePaths = list; }

        inline const wxArrayString& GetParserExcludePaths() const           { return m_parserExcludePaths; }

        inline void SetMaxItemToColour(int clour)                           { m_maxItemToColour = clour; }
        inline int GetMaxItemToColour() const                               { return m_maxItemToColour; }
        inline void SetMacrosFiles(const wxString& macrosFiles)             { m_macrosFiles = macrosFiles; }
        inline const wxString & GetMacrosFiles() const                      { return m_macrosFiles; }
    protected:
        virtual void FromJSON(const gsgs::JSONObject& json);
        virtual gsgs::JSONObject ToJSON() const;

        void DoUpdateTokensWxMap();
        void DoUpdateTokensWxMapReversed();
        wxString DoJoinArray(const wxArrayString& arr) const;
    public:
        static size_t CURRENT_VERSION;
        static wxString CLANG_CACHE_LAZY;
        static wxString CLANG_CACHE_ON_FILE_LOAD;
    protected:
        size_t m_ccFlags;
        size_t m_ccColourFlags;
        size_t m_clangOptions;
        size_t m_ccNumberOfDisplayItems;
        size_t m_version;
        int m_maxItemToColour;
        int m_minWordLen;
        gsgsStringMap m_tokensWxMap;
        gsgsStringMap m_tokensWxMapReversed;
        wxArrayString m_languages;
        wxArrayString m_parserSearchPaths;
        wxArrayString m_parserExcludePaths;
        wxArrayString m_clangSearchPaths;
        wxArrayString m_tokens;
        wxArrayString m_types;
        wxString m_macrosFiles;
        wxString m_clangBinary;
        wxString m_clangCmpOptions;
        wxString m_fileSpec;
        wxString m_clangMacros;
        wxString m_clangCachePolicy;
        bool m_parserEnabled;
    };

    /**
     * TagEntry is a persistent object which is capable of storing and loading itself from
     * various inputs
     */
    class _gsgsAPI TagEntry
    {
    public:
        enum {
            Tag_No_Signature_Format = 0x00000001, // Do not attempt to format the signature. Use the GetSignature() as is
            Tag_No_Return_Value_Eval = 0x00000002 // Do not evaluate the return value. Use GetReturnValue() instead
        };

        // Used by std::for_each to copy elements which are constructors
        class ForEachCopyIfCtor
        {
        public:
            ForEachCopyIfCtor(TagEntryPtrList& v)
                : m_matches(v)
            {
            }
            void operator()(TagEntryPtr tag)
            {
                if(tag->IsConstructor()) {
                    m_matches.push_back(tag);
                }
            }
        private:
            TagEntryPtrList& m_matches;
        };
    public:
        TagEntry();
        TagEntry(const tagEntry& entry);
        TagEntry(const TagEntry& rhs);
        virtual ~TagEntry();

        TagEntry& operator=(const TagEntry& rhs);
        bool operator==(const TagEntry& rhs);
        
        inline void SetComment(const wxString& comment)         { this->m_comment = comment; }
        inline const wxString& GetComment() const               { return m_comment; }
        inline void SetFlags(size_t flags)                      { this->m_flags = flags; }
        inline size_t GetFlags() const                          { return m_flags; }

        void FromLine(const wxString& line);

        /**
         * @brief return if this tag entry is a function tempalte
         */
        bool IsTemplateFunction() const;

        /**
         * Construct a TagEntry from tagEntry struct.
         * \param entry Tag entry
         */
        void Create(const tagEntry& entry);

        /**
         * Construct a TagEntry from values.
         * \param fileName File name
         * \param name Tag name
         * \param lineNumber Tag line number
         * \param pattern Pattern
         * \param kind Tag kind (class, struct, etc)
         * \param extFields Map of extenstion fields (key:value)
         * \param project Project name
         */
        void Create(const wxString & fileName, const wxString & name, int lineNumber, const wxString & pattern,
                    const wxString & kind, gsgsStringMap & extFields);

        /**
         * Test if this entry has been initialised.
         * \return true if this tag entry has been initialised
         */
        inline const bool IsOk() const                          { return GetKind() != _T("<unknown>"); }

        /**
         * Test of this tag is a container (class, union, struct or namespace
         */
        const bool IsContainer() const;

        /**
         * @brief return true if this tag represents a constructor
         */
        bool IsConstructor() const;
        /**
         * @brief return true if this tag represents a destructor
         */
        bool IsDestructor() const;

        /**
         * @brief return true of the this tag is a function or prototype
         */
        bool IsMethod() const;

        bool IsFunction() const;
        bool IsPrototype() const;
        bool IsMacro() const;
        bool IsClass() const;
        bool IsStruct() const;
        bool IsScopeGlobal() const;
        bool IsTypedef() const;

        //------------------------------------------
        // Operations
        //------------------------------------------
        inline bool GetDifferOnByLineNumber() const             { return m_differOnByLineNumber; }

        inline int GetId() const                                { return m_id; }
        inline void SetId(int id)                               { m_id = id; }

        inline const wxString& GetName() const                  { return m_name; }
        inline void SetName(const wxString& name)               { m_name = name; }

        inline const wxString& GetPath() const                  { return m_path; }
        inline void SetPath(const wxString& path)               { m_path = path; }

        inline const wxString& GetFile() const                  { return m_file; }
        inline void SetFile(const wxString& file)               { m_file = file; }

        inline int GetLine() const                              { return m_lineNumber; }
        inline void SetLine(int line)                           { m_lineNumber = line; }

        wxString GetPattern() const;
        /**
         * @brief return the entry pattern without the regex prefix/suffix
         */
        wxString GetPatternClean() const;

        inline void SetPattern(const wxString& pattern)         { m_pattern = pattern; }

        wxString GetKind() const;
        inline void SetKind(const wxString& kind)               { m_kind = kind; }

        inline const wxString& GetParent() const                { return m_parent; }
        inline void SetParent(const wxString& parent)           { m_parent = parent; }
    #if wxUSE_GUI
        inline wxTreeItemId& GetTreeItemId()                    { return m_hti; }
        inline void SetTreeItemId(wxTreeItemId& hti)            { m_hti = hti; }
    #endif

        inline wxString GetAccess() const                       { return GetExtField(_T("access")); }
        inline void SetAccess(const wxString& access)           { m_extFields[wxT("access")] = access; }

        inline wxString GetSignature() const                    { return GetExtField(_T("signature")); }
        inline void SetSignature(const wxString& sig)           { m_extFields[wxT("signature")] = sig; }

        inline void SetInherits(const wxString& inherits)       { m_extFields[_T("inherits")] = inherits; }
        inline void SetTyperef(const wxString& typeref)         { m_extFields[_T("typeref")] = typeref; }

        wxString GetInheritsAsString() const;
        wxArrayString GetInheritsAsArrayNoTemplates() const;
        wxArrayString GetInheritsAsArrayWithTemplates() const;

        inline wxString GetTyperef() const                      { return GetExtField(_T("typeref")); }

        inline void SetReturnValue(const wxString& retVal)      { m_extFields[_T("returns")] = retVal; }
        inline wxString GetReturnValue() const;

        inline const wxString& GetScope() const                 { return m_scope; }
        inline void SetScope(const wxString& scope)             { m_scope = scope; }

        /**
         * \return Scope name of the tag.
         * If path is empty in db or contains just the project name, it will return the literal <global>.
         * For project tags, an empty string is returned.
         */
        wxString GetScopeName() const;

        /**
         * Generate a Key for this tag based on its attributes
         * \return tag key
         */
        wxString Key() const;

        /**
         * Generate a display name for this tag to be used by the symbol tree
         * \return tag display name
         */
        wxString GetDisplayName() const;

        /**
         * Generate a full display name for this tag that includes:
         * full scope + name + signature
         * \return tag full display name
         */
        wxString GetFullDisplayName() const;

        /**
         * Return the actual name as described in the 'typeref' field
         * \return real name or wxEmptyString
         */
        wxString NameFromTyperef(wxString& templateInitList, bool nameIncludeTemplate = false);

        /**
         * Return the actual type as described in the 'typeref' field
         * \return real name or wxEmptyString
         */
        wxString TypeFromTyperef() const;
        //------------------------------------------
        // Extenstion fields
        //------------------------------------------
        wxString GetExtField(const wxString& extField) const
        {
            gsgsStringMap::const_iterator iter = m_extFields.find(extField);
            if(iter == m_extFields.end()) return wxEmptyString;
            return iter->second;
        }

        /**
         * @brief mark this tag has clang generated tag
         */
        inline void SetIsClangTag(bool isClangTag)              { this->m_isClangTag = isClangTag; }

        /**
         * @brief return true if this tag was generated by clang
         */
        inline bool GetIsClangTag() const                       { return m_isClangTag; }
        //------------------------------------------
        // Misc
        //------------------------------------------
        void Print();

        TagEntryPtr ReplaceSimpleMacro();

        /**
         * @brief return 0 if the values are the same. < 0 if a < b and > 0 if a > b
         */
        int CompareDisplayString(const TagEntryPtr& rhs) const;

        /**
         * @brief format a comment for this tag. The format uses gsgs's syntax formatting
         * that can be used later on in the various tooltip windows
         */
        wxString FormatComment();

    private:
        /**
         * Update the path with full path (e.g. namespace::class)
         * \param path path to add
         */
        void UpdatePath(wxString & path);
        bool TypedefFromPattern(const wxString & tagPattern, const wxString & typedefName, wxString & name,
            wxString & templateInit, bool nameIncludeTemplate = false);
    private:
        wxString m_path;           ///< Tag full path
        wxString m_file;           ///< File this tag is found
        long m_id;
        int m_lineNumber;          ///< Line number
        wxString m_pattern;        ///< A pattern that can be used to locate the tag in the file
        wxString m_kind;           ///< Member, function, class, typedef etc.
        wxString m_parent;         ///< Direct parent
    #if wxUSE_GUI
        wxTreeItemId m_hti;        ///< Handle to tree item, not persistent item
    #endif
        wxString m_name;           ///< Tag name (short name, excluding any scope names)
        gsgsStringMap m_extFields; ///< Additional extension fields
        wxString m_scope;
        size_t m_flags;     // This member is not saved into the database
        wxString m_comment; // This member is not saved into the database
        wxString m_formattedComment;
        bool m_isCommentForamtted;
        bool m_differOnByLineNumber;
        bool m_isClangTag;
    };

    typedef TreeNode<wxString, TagEntry> TagNode;

    /**
     * Tree representation of tags.
     * This is basically a data structure representing the GUI symbol tree.
     */
    class _gsgsAPI TagTree : public Tree<wxString, TagEntry>
    {
    public:
        /**
         * Construct a tree with roots' values key & data.
         * \param key Root's key
         * \param data Root's data
         */
        TagTree(const wxString& key, const TagEntry& data);
        virtual ~TagTree();

        /**
         * Add an entry to the tree.
         * This functions will add all parents of the tag to the tree if non exist (or some of some exist).
         * For example: if TagName is box::m_string, this functions will make sure that a node 'box' exists.
         * If not, it will add it and then will add m_string to it as its child.
         * \param tag Tag to add
         * \return new node that was added to the tree.
         */
        TagNode* AddEntry(TagEntry& tag);

    };

    typedef SharedPtr<TagTree> TagTreePtr;
}
#endif
