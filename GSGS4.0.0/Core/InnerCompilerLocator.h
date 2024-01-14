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

#ifndef COMPILERLOCATORCLANG_H
#define COMPILERLOCATORCLANG_H

#include "gsgsToolsetManager.h"

class CompilerLocatorCLANG : public ToolsetLocator
{
public:
    CompilerLocatorCLANG();
    virtual ~CompilerLocatorCLANG();

    bool Locate();
    CompilerPtr Locate(const wxString& folder);
protected:
    void MSWLocate();
    void AddTools(CompilerPtr compiler, const wxString& installFolder, const wxString& suffix = "");
    void AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& toolpath, const wxString& extraArgs = "");
    wxString GetClangVersion(const wxString& clangBinary);
    wxString GetCompilerFullName(const wxString& clangBinary);
    bool ReadMSWInstallLocation(const wxString& regkey, wxString& installPath, wxString& llvmVersion);
};

class CompilerLocatorGCC : public ToolsetLocator
{
public:
    CompilerLocatorGCC();
    virtual ~CompilerLocatorGCC();

    virtual bool Locate();
    virtual CompilerPtr Locate(const wxString& folder);
protected:
    void AddTools(CompilerPtr compiler, const wxString &binFolder, const wxString &suffix = "");
    void AddTool(CompilerPtr compiler, const wxString &toolname, const wxString &toolpath, const wxString &suffix, const wxString &extraArgs = "");
};

class _gsgsAPI CompilerLocatorMinGW : public ToolsetLocator
{
public:
    CompilerLocatorMinGW();
    virtual ~CompilerLocatorMinGW();

    virtual bool Locate();
    virtual CompilerPtr Locate(const wxString& folder);
protected:
    void AddTools(const wxString &binFolder, const wxString &name = "");
    void AddTool(CompilerPtr compiler, const wxString &toolname, const wxString &toolpath, const wxString &extraArgs = "");
    wxString FindBinFolder(const wxString &parentPath);
    wxString GetGCCVersion(const wxString &gccBinary);
    void CheckRegKey(const wxString& displayName, const wxString& installFolder);
private:
    gsgsStringSet m_locatedFolders;
};

//-------------------------------------------------------------------------
// For a complete list of MSVC compilers
// See here: https://en.wikipedia.org/wiki/Microsoft_Visual_Studio#History
//-------------------------------------------------------------------------
class CompilerLocatorMSVC : public ToolsetLocator
{
public:
    CompilerLocatorMSVC();
    virtual ~CompilerLocatorMSVC();
    
    virtual bool Locate();
    virtual CompilerPtr Locate(const wxString& folder) { return NULL; }
protected:
    void AddTools(const wxString& masterFolder, const wxString& name, const wxString& platform);
    void AddTool(const wxString& toolpath, const wxString& extraArgs, const wxString& toolname, CompilerPtr compiler);
    void AddCompilerOptions(CompilerPtr compiler);
    void AddLinkerOptions(CompilerPtr compiler);
};

class _gsgsAPI CompilerLocatorCrossGCC : public ToolsetLocator
{
public:
    CompilerLocatorCrossGCC();
    virtual ~CompilerLocatorCrossGCC();
    virtual bool Locate();
    virtual CompilerPtr Locate(const wxString& folder);
protected:
    void AddTools(CompilerPtr compiler, const wxString &binFolder, const wxString &prefix = "", const wxString &suffix = "");
    void AddTool(CompilerPtr compiler, const wxString &toolname, const wxString &toolpath, const wxString &extraArgs = "");
    CompilerPtr Locate(const wxString& folder, bool clear);
private:
    gsgsStringSet m_locatedFolders;
};

class _gsgsAPI CompilerLocatorCygwin : public ToolsetLocator
{
public:
    CompilerLocatorCygwin();
    virtual ~CompilerLocatorCygwin(){}
    wxString GetGCCVersion(const wxString& gccBinary);
    
    /**
     * @brief return an array of suffixes for the gcc executable
     * Under Cygwin, the 'gcc.exe' is a symlink which pointer to either
     * gcc-3.exe or gcc-4.exe (and in the future gcc-5.exe)
     */
    wxArrayString GetSuffixes(const wxString &binFolder);
    
private:
    void AddTools(const wxString& binFolder, const wxString& name, const wxString &suffix);
    void AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& toolpath, const wxString& extraArgs = "");
    
public:
    virtual bool Locate();
    virtual CompilerPtr Locate(const wxString& folder);
};

class _gsgsAPI CompilerLocatorEosCDT : public ToolsetLocator
{
public:
    CompilerLocatorEosCDT();
    virtual ~CompilerLocatorEosCDT();

    virtual bool Locate();
    virtual CompilerPtr Locate(const wxString& folder);
protected:
    bool CheckExists(const wxString& path, wxString& foundPath) const;
    void AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& path, const wxString& args = "") const;
    CompilerPtr CreateCompiler(const wxString& path) const;
};

#endif // COMPILERLOCATORCLANG_H
