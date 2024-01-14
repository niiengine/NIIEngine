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

#ifndef gsgsCompilerCmdParser_H
#define gsgsCompilerCmdParser_H

#include "gsgsPreInclude.h"

namespace gsgs
{
    class _gsgsAPI CompilerCmdParser
    {
    public:
        typedef std::set<wxString> Set_t;
    public:
        CompilerCmdParser(const wxString& cmdline, const wxString& workingDirectory = wxEmptyString);
        virtual ~CompilerCmdParser();

        wxString GetCompileLine() const;
        void MakeAbsolute(const wxString& path);
        inline const wxString& GetStandard() const              { return m_standard; }

        wxString GetStandardWithPrefix() const;

        inline const wxArrayString& GetSysroots() const         { return m_sysroots; }
        inline const wxArrayString& GetOtherOptions() const     { return m_otherOptions; }
        inline const wxArrayString& GetFramworks() const        { return m_framworks; }
        inline const wxString& GetPchFile() const               { return m_pchFile; }
        inline void SetArgc(int argc)                           { this->m_argc = argc; }
        inline void SetArgv(char** argv)                        { this->m_argv = argv; }
        inline void SetIncludes(const wxArrayString& includes)  { this->m_includes = includes; }
        inline void SetMacros(const wxArrayString& macros)      { this->m_macros = macros; }
        inline const wxArrayString& GetIncludes() const         { return m_includes; }
        inline const wxArrayString& GetMacros() const           { return m_macros; }
        inline char** GetArgv()                                 { return m_argv; }

        inline int GetArgc() const                                  { return m_argc; }
        inline const wxString& GetDiretory() const                  { return m_diretory; }
        inline const wxArrayString& GetIncludesWithPrefix() const   { return m_includesWithPrefix; }
        inline const wxArrayString& GetMacrosWithPrefix() const     { return m_macrosWithPrefix; }
    protected:
        void AddIncludesFromFile(const wxFileName& includeFile);
    protected:
        wxArrayString m_includes;
        wxArrayString m_macros;
        wxArrayString m_macrosWithPrefix;
        wxArrayString m_includesWithPrefix;
        wxArrayString m_framworks;
        wxArrayString m_sysroots; // -isysroot
        wxString m_standard;
        wxArrayString m_otherOptions;

        int m_argc;
        char** m_argv;
        wxString m_diretory;
        wxString m_pchFile;
    };
}
#endif
