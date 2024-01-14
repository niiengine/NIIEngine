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
#ifndef gsgs_LANGUAGE_H
#define gsgs_LANGUAGE_H

#include "gsgsPreInclude.h"
#include "CxxTokenizer.h"
#include "CxxVariable.h"
#include "cpp_scanner.h"
#include "gsgsCommon.h"
#include "gsgsTag.h"
#include "gsgsSingleton.h"
#include "y.tab.h"

namespace gsgs
{
    enum SearchFlags {
        PartialMatch = 0x00000001,
        ExactMatch = 0x00000002,
        IgnoreCaseSensitive = 0x00000004,
        ReplaceTokens = 0x00000008,
    };

    class TemplateHelper
    {
    public:
        TemplateHelper() {}
        ~TemplateHelper() {}

        void SetTemplateDeclaration(const wxString& decl);
        void SetTemplateInstantiation(const wxString& tempalteInstantiation);

        inline void SetTemplateDeclaration(const wxArrayString& decl){ templateDeclaration = decl; }
        void SetTemplateInstantiation(const wxArrayString& templateInstantiation);

        inline const wxArrayString& GetTemplateDeclaration() const  { return templateDeclaration; }
        inline bool IsTemplate() const { return templateDeclaration.IsEmpty() == false; }

        inline void SetTypeScope(const wxString& typeScope)         { this->typeScope = typeScope; }
        inline const wxString& GetTypeScope() const                 { return typeScope; }
        inline void SetTypeName(const wxString& typeName)           { this->typeName = typeName; }
        inline const wxString& GetTypeName() const                  { return typeName; }
        wxString Substitute(const wxString& name);

        void Clear();

        wxString GetPath() const;
    private:
        std::vector<wxArrayString> templateInstantiationVector;
        wxArrayString templateDeclaration;
        wxString typeScope;
        wxString typeName;
    };

    class ParsedToken
    {
    public:
        ParsedToken();
        ~ParsedToken();

        void SetAutoExpression(const wxString& autoExpression) { this->m_autoExpression = autoExpression; }
        inline void SetIsAutoVariable(bool isAutoVariable)     { this->m_isAutoVariable = isAutoVariable; }
        inline const wxString& GetAutoExpression() const       { return m_autoExpression; }
        inline bool IsAutoVariable() const                     { return m_isAutoVariable; }
        bool IsThis() const;
        inline void SetCurrentScopeName(const wxString& currentScopeName)
        {
            this->m_currentScopeName = currentScopeName;
            this->m_currentScopeName.Trim().Trim(false);
        }
        inline const wxString& GetCurrentScopeName() const      { return m_currentScopeName; }
        inline void SetName(const wxString& name)
        {
            this->m_name = name;
            this->m_name.Trim().Trim(false);
        }
        inline const wxString& GetName() const                  { return m_name; }
        inline void SetNext(ParsedToken* next)                  { this->m_next = next; }
        inline void SetPrev(ParsedToken* prev)                  { this->m_prev = prev; }
        inline ParsedToken* GetNext() const                     { return m_next; }
        inline ParsedToken* GetPrev() const                     { return m_prev; }
        inline void SetSubscriptOperator(bool subscriptOperator){ this->m_subscriptOperator = subscriptOperator; }
        inline bool GetSubscriptOperator() const                { return m_subscriptOperator; }

        inline void SetIsTemplate(bool isTemplate)              { this->m_isTemplate = isTemplate; }
        inline bool GetIsTemplate() const                       { return m_isTemplate; }
        inline void SetOperator(const wxString& oper)
        {
            this->m_oper = oper;
            this->m_oper.Trim().Trim(false);
        }
        inline void SetTypeName(const wxString& type)
        {
            this->m_type = type;
            this->m_type.Trim().Trim(false);
        }
        void SetTypeScope(const wxString& typeScope)
        {
            this->m_typeScope = typeScope;
            this->m_typeScope.Trim().Trim(false);

            if(this->m_typeScope.IsEmpty()) {
                this->m_typeScope = wxT("<global>");
            }
        }

        inline const wxString& GetOperator() const                      { return m_oper; }
        inline const wxString& GetTypeName() const                      { return m_type; }
        inline const wxString& GetTypeScope() const                     { return m_typeScope; }
        inline void SetTemplateArgList(const wxArrayString& templateArgList, std::set<wxString>& argsSet)
        {
            this->m_templateArgList = templateArgList;
            // Keep track of the template arguments
            argsSet.insert(m_templateArgList.begin(), m_templateArgList.end());
        }
        
        inline void SetTemplateInitialization(const wxArrayString & templateInitialization)
        {
            this->m_templateInitialization = templateInitialization;
        }
        inline const wxArrayString& GetTemplateArgList() const          { return m_templateArgList; }
        inline const wxArrayString& GetTemplateInitialization() const   { return m_templateInitialization; }

        inline void SetArgumentList(const wxString& argumentList)       { this->m_argumentList = argumentList; }
        inline const wxString& GetArgumentList() const                  { return m_argumentList; }
        wxString GetContextScope() const;
        wxString GetPath() const;
        void RemoveScopeFromType();

        /**
         * @brief replace the current type which is template with an actual type
         */
        bool ResovleTemplate(TagsManager* lookup);

        /**
         * @brief fix the template type with actual type
         * This method aims to fix the template argument NOT the token's type.
         * so it will fix code like this:
         *
         * template <typename T> class A {
         *     typedef iterator<T> Iter;
         * };
         *
         * And assuming iterator class:
         *
         * template <typename _T1> class iterator {
         *    _T1 m_current;
         * };
         *
         * so this method replaces _T1 with T which can then be replaced with the actual type
         *
         * @param lookup
         */
        void ResolveTemplateType(TagsManager* lookup);

        /**
         * @brief replace template argument with the actual type (std::vector<wxString>: _Tp --> wxString)
         * @param templateArg the template argument to replace
         */
        wxString TemplateToType(const wxString& templateArg);
    public:
        static void DeleteTokens(ParsedToken* head);
    private:
        wxString m_type;
        wxString m_typeScope;
        wxString m_oper;
        wxArrayString m_templateInitialization;
        wxArrayString m_templateArgList;
        wxString m_name;
        wxString m_currentScopeName;
        wxString m_argumentList;
        wxString m_autoExpression;
        ParsedToken* m_next;
        ParsedToken* m_prev;
        bool m_isAutoVariable;
        bool m_subscriptOperator;
        bool m_isTemplate;
    };

    class TokenContainer
    {
    public:
        TokenContainer()
            : head(NULL)
            , current(NULL)
            , rew(false)
            , retries(0)
        {
        }

        inline bool Rewind()
        {
            if(retries > 3) 
                return false;
            return rew;
        }

        void SetRewind(bool r)
        {
            if(r) {
                retries++;
            } else {
                rew = r;
            }

            if(r && retries <= 3) { // we allow maximum of 3 retries
                rew = r;
            }
        }
    public:
        ParsedToken* head;
        ParsedToken* current;
        bool rew;
        int retries;
    };

    class _gsgsAPI Language
    {
        friend class LanguageST;
        friend class TagEntry;
        friend class TemplateHelper;
        friend class TagsManager;
    public:
        /**
         * Return the visible scope of syntax starting at depth zero.
         *
         * For example, given the following srcString:
         * \code
         * int main()
         * {
         *		{
         *			int i =0;
         *		}
         * }
         * \endcode
         *
         * the output will be:
         * \code
         * int main()
         * {
         *   ;
         * }
         * \code
         *
         * \note The scopes with depth greater then 0 will be removed.
         * This function is useful for searching for local members.
         * \param srcString Input string
         * \return visible scope
         */
        wxString OptimizeScope(const wxString& srcString, int lastFuncLine, wxString& localsScope);

        /**
         * @brief given fileContent, locate the best line to place a class forward declaration
         * statement
         */
        int GetBestLineForForwardDecl(const wxString& fileContent) const;

        void CheckForTemplateAndTypedef(ParsedToken* token);

        inline void SetLastFunctionSignature(const wxString& signature)
        {
            m_lastFunctionSignature = signature;
        }
        inline const wxString& GetLastFunctionSignature() const { return m_lastFunctionSignature; }

        void SetAdditionalScopes(const std::vector<wxString>& additionalScopes, const wxString& filename);

        /**
         * @brief update the 'additional scopes' cache with list of scopes
         * @param filename
         * @param additionalScopes
         */
        void UpdateAdditionalScopesCache(const wxString& filename, const std::vector<wxString>& additionalScopes);
        /**
         * @brief clear the additional scopes cache
         */
        void ClearAdditionalScopesCache();

        const std::vector<wxString>& GetAdditionalScopes() const;
        /**
         * Set the language specific auto completion delimeteres, for example: for C++ you should populate
         * the array with { . , -> , :: }
         * \param delimArr delimeter array
         */
        void SetAutoCompDeliemters(const std::vector<wxString>& delimArr);

        /**
         * \brief set the tags manager to be used by this language instance
         * \param tm
         */
        void SetTagsManager(TagsManager* tm);

        /**
         * \brief return the available tags manager
         * \return
         */
        TagsManager* GetTagsManager();

        /**
         * Parse comments from source file
         * \param name file name
         * \param comments [output] returned vector of comments
         */
        void ParseComments(const wxFileName& fileName, std::vector<CommentPtr>* comments);

        //==========================================================
        // New API based on the yacc grammar files
        //==========================================================

        /**
         * Evaluate a C++ expression. for example, the following expression: '((Notebook*)book)->'
         * will be processed into typeName=Notebook, and typeScope=<global> (assuming Notebook is not
         * placed within any namespace)
         * \param stmt c++ expression
         * \param text text where this expression was found
         * \param fn filename context
         * \param lineno current line number
         * \param typeName [output]
         * \param typeScope [output]
         * \param oper [output] return the operator used (::, ., ->)
         * \param scopeTemplateInitList [output] return the scope tempalte intialization (e.g. "std::auto_ptr<wxString>
         * str;" -> <wxString>
         * \return true on success, false otherwise. The output fields are only to be checked with the return
         * valus is 'true'
         */
        bool ProcessExpression(const wxString& stmt, const wxString& text, const wxFileName& fn, int lineno,
                               wxString& typeName, wxString& typeScope, wxString& oper, wxString& scopeTemplateInitList);

        /**
         * return scope name from given input string
         * \param in input string
         * \return scope name or empty string
         */
        wxString GetScopeName(const wxString& in, std::vector<wxString>* additionlNS);

        /**
         * parse an expression and return the result. this functions uses
         * the sqlite database as its symbol table
         * \param in input string expression
         * \return ExpressionResult, if it fails to parse it, check result.m_name.empty() for success
         */
        ExpressionResult ParseExpression(const wxString& in);

        /**
         * @brief process a token and resolve it into typeName / typeScope
         * @param token
         * @return
         */
        bool ProcessToken(TokenContainer* tokeContainer);

        /**
         * Collect local variables from given scope text (in) and an optional symbol name
         * \param in scope to search for
         * \param tags output, since we dont have full information about each token, all local variables returned are of
         *type
         *			   'variable' with public access
         * \param name optional name to look for (name can be partial).
         */
        void GetLocalVariables(const wxString& in, std::vector<TagEntryPtr>& tags, bool isFuncSignature,
                               const wxString& name = wxEmptyString, size_t flag = PartialMatch);

        wxString ApplyCtagsReplacementTokens(const wxString& in);

        bool VariableFromPattern(const wxString& pattern, const wxString& name, Variable& var);
        bool FunctionFromPattern(TagEntryPtr tag, Function& foo);

        /**
         * @brief insert functionBody into clsname. This function will search for best location
         * to place the function body. set visibility to 0 for 'pubilc' function, 1 for 'protected' and 2 for private
         * return true if this function succeeded, false otherwise
         */
        bool InsertFunctionDecl(const wxString& clsname, const wxString& functionDecl, wxString& sourceContent,
                                int visibility = 0);

        /**
         * @brief insert functionBody into clsname. This function will search for best location
         * to place the function body
         */
        void InsertFunctionImpl(const wxString& clsname, const wxString& functionImpl, const wxString& filename,
                                wxString& sourceContent, int& insertedLine);

    private:
        bool DoSearchByNameAndScope(const wxString& name, const wxString& scopeName, std::vector<TagEntryPtr>& tags,
                                    wxString& type, wxString& typeScope, bool testGlobalScope = true);

        bool CorrectUsingNamespace(wxString& type, wxString& typeScope, const wxString& parentScope,
                                   std::vector<TagEntryPtr>& tags);

        Language();
        virtual ~Language();

        /**
         * Return the next token and the delimiter found, the source string is taken from the
         * m_tokenScanner member of this class
         * @param token next token
         * @param delim delimiter found
         * @param subscriptOperator subscript operator was found
         * @param funcArgList the first argument list passed (e.g. MyCall(a)->, funcArgList will contain (a))
         * @return true if token was found false otherwise
         */
        bool NextToken(wxString& token, wxString& delim, bool& subscriptOperator, wxString& funcArgList);

        /**
         * @brief construct a list of tokens based on an expression and a give scope
         * For example: parsing the expression: variable->GetName().AfterFirst()
         * will yield a list of 3 tokens
         * @param scopeName the current scope name of where the expression was found
         * @return ParsedToken ptr (allocated on the heap. Must be freed by calling to ParsedToke::DeleteTokens( head ) or
         * NULL incase
         * parsing fails
         */
        ParsedToken* ParseTokens(const wxString& scopeName);

        /**
         * @brief Attempt to fix template results
         * @param typeName the type name that was detected by the parser
         * @param typeScope the type scope
         */
        bool OnTemplates(ParsedToken* token);

        /**
         * \brief attempt to expand 'typedef' to their actual value
         */
        bool OnTypedef(ParsedToken* token);
        void DoSimpleTypedef(ParsedToken* token);

        /**
         * @brief expand reference operator (->) overloading
         * @param typeName [input/output] the type name to check for the reference operator overloading
         * @param typeScope [input/output] its scope
         * @return true on success, false otherwise
         */
        bool OnArrowOperatorOverloading(ParsedToken* token);

        /**
         * @brief expand subscript operator
         * @param typeName [input/output] the type name to check for the reference operator overloading
         * @param typeScope [input/output] its scope
         * @return true on success, false otherwise
         */
        bool OnSubscriptOperator(ParsedToken* token);

        /**
         * @brief run the user defined types as they appear in the 'Types' replacement table
         * @param token current token
         */
        bool RunUserTypes(ParsedToken* token, const wxString& entryPath = wxEmptyString);

        void ParseTemplateArgs(const wxString& argListStr, wxArrayString& argsList);
        void ParseTemplateInitList(const wxString& argListStr, wxArrayString& argsList);
        void DoRemoveTempalteInitialization(wxString& str, wxArrayString& tmplInitList);
        void DoResolveTemplateInitializationList(wxArrayString& tmpInitList);
        void DoFixFunctionUsingCtagsReturnValue(Function& foo, TagEntryPtr tag);
        void DoReplaceTokens(wxString& inStr, const gsgsStringMap& ignoreTokens);
        wxArrayString DoExtractTemplateDeclarationArgsFromScope();
        wxArrayString DoExtractTemplateDeclarationArgs(ParsedToken* token);
        wxArrayString DoExtractTemplateDeclarationArgs(TagEntryPtr tag);
        void DoExtractTemplateInitListFromInheritance(TagEntryPtr tag, ParsedToken* token);
        void DoExtractTemplateInitListFromInheritance(ParsedToken* token);
        void DoExtractTemplateArgsFromSelf(ParsedToken* token);
        void DoFixTokensFromVariable(TokenContainer* tokeContainer, const wxString& variableDecl);

        // Wrapper methods to help the transition from the [wxString,wxString] API into the new ParsedToken
        // code
        bool DoIsTypeAndScopeExist(ParsedToken* token);
        bool DoCorrectUsingNamespaces(ParsedToken* token, std::vector<TagEntryPtr>& tags);

        /**
         * @brief read the class name. This function assumes that the last token
         * consumed by the scanner is 'class' (type: lexCLASS)
         */
        int DoReadClassName(CppScanner& scanner, wxString& clsname) const;
    protected:
        inline void SetVisibleScope(const wxString& visibleScope)      { this->m_visibleScope = visibleScope; }
        inline const wxString & GetVisibleScope() const                { return m_visibleScope; }
        CxxVariable::Ptr_t FindLocalVariable(const wxString& name);
        CxxVariable::Ptr_t FindVariableInScope(const wxString& scope, const wxString& name);
    private:
        std::map<char, char> m_braces;
        std::vector<wxString> m_delimArr;
        wxString m_expression;
        CppScannerPtr m_scanner;
        CxxTokenizer m_tokenScanner;
        TagsManager* m_tm;
        wxString m_visibleScope;
        wxString m_lastFunctionSignature;
        std::vector<wxString> m_additionalScopes;                           // collected by parsing 'using namespace XXX'
        std::map<wxString, std::vector<wxString> > m_additionalScopesCache; // collected by parsing 'using namespace XXX'
        TemplateHelper m_templateHelper;
        std::set<wxString> m_templateArgs;
        CxxVariable::Map_t m_locals;
    };

    class _gsgsAPI LanguageST
    {
    public:
        static void Free();
        static Language* Get();
    };
}
#endif
