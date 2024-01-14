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

#ifndef PPTABLE_H
#define PPTABLE_H

#include "gsgsPreInclude.h"

namespace gsgs
{
    struct _gsgsAPI Replacement 
    {
        std::string full_pattern;
        std::string searchFor;
        std::string replaceWith;
        bool is_compound;
        bool is_ok;
        void construct(const std::string& pattern, const std::string& replacement);
    };

    typedef std::list<Replacement> CLReplacementList;

    /**
     * @brief perform search and replace using CL pattern
     * an example:
     * pattern=wx_dynamic_cast(%0, %1)
     * replacement=dynamic_cast<%0>(%1)
     * in=wx_dynamic_cast(wxApp*, ptr)->OnInit();
     *
     * the expected result is:
     * dynamic_cast<wxApp*>(ptr)->OnInit()
     *
     * It also supports simple search and replace
     */
    bool ReplacePattern(const wxString& in, const wxString& pattern, const wxString& replacement, wxString& output);

    /**
     * @brief perform search and replace using CL pattern
     * an example:
     * pattern=wx_dynamic_cast(%0, %1)
     * replacement=dynamic_cast<%0>(%1)
     * in=wx_dynamic_cast(wxApp*, ptr)->OnInit();
     *
     * the expected result is:
     * dynamic_cast<wxApp*>(ptr)->OnInit()
     *
     * It also supports simple search and replace
     */
    bool ReplacePatternA(const std::string& in, const Replacement& repl, std::string& outStr);

    /**
     *
     */
    struct _gsgsAPI PPToken 
    {
        enum 
        { 
            IsFunctionLike  = 0x00000001, 
            IsValid         = 0x00000002, 
            IsOverridable   = 0x00000004 
        };

        PPToken()
            : line(0)
            , flags(IsOverridable)
        {
        }

        ~PPToken() {}

        void expandOnce(const wxArrayString & initList);
        void processArgs(const wxString & argsList);
        wxString signature() const;
        void print(wxFFile& fp);
        static bool readInitList(const wxString& in, int from, wxString& str, wxArrayString& initListArr);
        static bool readInitList(const std::string& in, size_t from, std::string& str, std::vector<std::string>& initListArr);
        void squeeze();
        wxString fullname() const;

        int line;             // line where found
        wxString fileName;
        wxString name;        // preprocessor name
        wxString replacement; // un processed replacement
        wxArrayString args;   // for function like macros, contains the argument's names
        size_t flags;         // PP token flags
    };

    class _gsgsAPI PPTable
    {
    public:
        PPToken Token(const wxString& name);
        bool Contains(const wxString& name);
        void Add(const PPToken& token);
        void AddUsed(const wxString& name);
        void Print(wxFFile& fp);
        wxString Export();
        void Clear();
        void ClearNamesUsed();
        void Squeeze();

        inline const std::map<wxString, PPToken>& GetTable() const  { return m_table; }

        inline const std::set<wxString>& GetNamesUsed() const       { return m_namesUsed; }
        
        static PPTable* Instance();
        static void Release();
    private:
        PPTable();
        ~PPTable();
    private:
        std::map<wxString, PPToken> m_table;
        std::set<wxString> m_namesUsed;
    };
}
#endif
