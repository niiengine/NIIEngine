
#include "InnerCompilerLocator.h"
#include "globals.h"
#include "gsgsProcess.h"
#include "gsgsBuildConfig.h"
#include "gsgsLogManager.h"
#include "gsgsToolsetManager.h"

#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/filefn.h>
#include <wx/regex.h>
#include <wx/thread.h>

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#include <wx/volume.h>
#endif

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// CompilerLocatorCLANG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CompilerLocatorCLANG::CompilerLocatorCLANG() {}

CompilerLocatorCLANG::~CompilerLocatorCLANG() {}

bool CompilerLocatorCLANG::Locate()
{
    m_compilers.clear();
    MSWLocate();

    // POSIX locate
    wxFileName clang("/usr/bin", "clang");
    wxArrayString suffixes;
    suffixes.Add("");
    suffixes.Add("-3.3");
    suffixes.Add("-3.4");
    suffixes.Add("-3.5");
    suffixes.Add("-3.6");
    for(size_t i = 0; i < suffixes.GetCount(); ++i) {
        clang.SetFullName("clang" + suffixes.Item(i));
        if(clang.FileExists()) {
            CompilerPtr compiler(new Compiler(NULL));
            compiler->SetCompilerFamily(COMPILER_FAMILY_CLANG);
            // get the compiler version
            compiler->SetName(GetCompilerFullName(clang.GetFullPath()));
            compiler->SetGenerateDependeciesFile(true);
            m_compilers.push_back(compiler);
            clang.RemoveLastDir();
            AddTools(compiler, clang.GetPath(), suffixes.Item(i));
        }
    }
    return true;
}

CompilerPtr CompilerLocatorCLANG::Locate(const wxString& folder)
{
    m_compilers.clear();
    wxFileName clang(folder, "clang");
#ifdef __WXMSW__
    clang.SetExt("exe");
#endif
    bool found = clang.FileExists();
    if(!found) {
        // try to see if we have a bin folder here
        clang.AppendDir("bin");
        found = clang.FileExists();
    }

    if(found) {
        CompilerPtr compiler(new Compiler(NULL));
        compiler->SetCompilerFamily(COMPILER_FAMILY_CLANG);
        // get the compiler version
        compiler->SetName(GetCompilerFullName(clang.GetFullPath()));
        compiler->SetGenerateDependeciesFile(true);
        m_compilers.push_back(compiler);
        clang.RemoveLastDir();
        AddTools(compiler, clang.GetPath());
        return compiler;
    }
    return NULL;
}

void CompilerLocatorCLANG::MSWLocate()
{
#ifdef __WXMSW__
    wxString llvmInstallPath, llvmVersion;
    wxArrayString regKeys;
    regKeys.Add("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LLVM");
    regKeys.Add("Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LLVM");
    for(size_t i = 0; i < regKeys.size(); ++i) {
        if(ReadMSWInstallLocation(regKeys.Item(i), llvmInstallPath, llvmVersion)) {
            CompilerPtr compiler(new Compiler(NULL));
            compiler->SetCompilerFamily(COMPILER_FAMILY_CLANG);
            compiler->SetGenerateDependeciesFile(true);
            compiler->SetName(wxString() << "clang ( " << llvmVersion << " )");
            m_compilers.push_back(compiler);
            AddTools(compiler, llvmInstallPath);
            break;
        }
    }
#endif
}

void CompilerLocatorCLANG::AddTool(CompilerPtr compiler,
                                   const wxString& toolname,
                                   const wxString& toolpath,
                                   const wxString& extraArgs)
{
    wxString tool = toolpath;
    ::WrapWithQuotes(tool);
    if(!extraArgs.IsEmpty()) {
        tool << " " << extraArgs;
    }
    compiler->SetTool(toolname, tool);
}

void CompilerLocatorCLANG::AddTools(CompilerPtr compiler, const wxString& installFolder, const wxString& suffix)
{
    compiler->SetInstallationPath(installFolder);
    wxFileName toolFile(installFolder, "");
    toolFile.AppendDir("bin");
#ifdef __WXMSW__
    toolFile.SetExt("exe");
#endif

    toolFile.SetName("clang++" + suffix);
    AddTool(compiler, "CXX", toolFile.GetFullPath());
    AddTool(compiler, "LinkerName", toolFile.GetFullPath());

#ifdef __WXMAC__
    AddTool(compiler, "SharedObjectLinkerName", toolFile.GetFullPath(), "-dynamiclib -fPIC");
#else
    AddTool(compiler, "SharedObjectLinkerName", toolFile.GetFullPath(), "-shared -fPIC");
#endif

    toolFile.SetName("clang" + suffix);
    AddTool(compiler, "CC", toolFile.GetFullPath());

    // Add the archive tool
    toolFile.SetName("llvm-ar");
    if(toolFile.FileExists()) {
        AddTool(compiler, "AR", toolFile.GetFullPath(), "rcu");

    } else {
        toolFile.SetName("ar");
        AddTool(compiler, "AR", toolFile.GetFullPath(), "rcu");
    }

#ifdef __WXMSW__
    AddTool(compiler, "ResourceCompiler", "windres.exe");
#else
    AddTool(compiler, "ResourceCompiler", "");
#endif

    // Add the assembler tool
    toolFile.SetName("llvm-as");
    if(toolFile.FileExists()) {
        AddTool(compiler, "AS", toolFile.GetFullPath());

    } else {
        toolFile.SetName("as");
        AddTool(compiler, "AS", toolFile.GetFullPath());
    }

    wxString makeExtraArgs;
    if(wxThread::GetCPUCount() > 1) {
        makeExtraArgs << "-j" << wxThread::GetCPUCount();
    }

#ifdef __WXMSW__
    AddTool(compiler, "MAKE", "mingw32-make.exe", makeExtraArgs);
#else
    AddTool(compiler, "MAKE", "make", makeExtraArgs);
#endif
}

wxString CompilerLocatorCLANG::GetClangVersion(const wxString& clangBinary)
{
    wxString command;
    wxArrayString stdoutArr;
    command << clangBinary << " --version";
    ProcUtils::SafeExecuteCommand(command, stdoutArr);
    if(!stdoutArr.IsEmpty()) {
        wxString versionString = stdoutArr.Item(0);
        versionString = versionString.AfterLast('(');
        versionString = versionString.BeforeLast(')');
        return versionString;
    }
    return "";
}

wxString CompilerLocatorCLANG::GetCompilerFullName(const wxString& clangBinary)
{
    wxString version = GetClangVersion(clangBinary);
    wxString fullname;
    fullname << "clang";
    if(!version.IsEmpty()) {
        fullname << "( " << version << " )";
    }
    return fullname;
}

bool CompilerLocatorCLANG::ReadMSWInstallLocation(const wxString& regkey, wxString& installPath, wxString& llvmVersion)
{
#ifdef __WXMSW__
    wxRegKey reg(wxRegKey::HKLM, regkey);
    installPath.Clear();
    llvmVersion.Clear();
    if(reg.Exists()) {
        reg.QueryValue("DisplayIcon", installPath);
        reg.QueryValue("DisplayVersion", llvmVersion);
    }
    return !installPath.IsEmpty() && !llvmVersion.IsEmpty();
#else
    return false;
#endif
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// CompilerLocatorCLANG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CompilerLocatorGCC::CompilerLocatorGCC()
{
}

CompilerLocatorGCC::~CompilerLocatorGCC()
{
}

CompilerPtr CompilerLocatorGCC::Locate(const wxString& folder)
{
    m_compilers.clear();
    wxFileName gcc(folder, "gcc");
    wxFileName tmpfn(folder, "");
    
    wxString name;
    if( tmpfn.GetDirCount() > 1 && tmpfn.GetDirs().Last() == "bin" ) {
        tmpfn.RemoveLastDir();
        name = tmpfn.GetDirs().Last();
    }
    
#ifdef __WXMSW__
    gcc.SetExt("exe");
#endif

    bool found = gcc.FileExists();
    if ( ! found ) {
        // try to see if we have a bin folder here
        gcc.AppendDir("bin");
        found = gcc.FileExists();
    }
    
    if ( found ) {
        CompilerPtr compiler( new Compiler(NULL) );
        compiler->SetCompilerFamily(COMPILER_FAMILY_GCC);
        
        // get the compiler version
        compiler->SetName( name.IsEmpty() ? "GCC" : name );
        compiler->SetGenerateDependeciesFile(true);
        m_compilers.push_back( compiler );
        
        // we path the bin folder
        AddTools(compiler, gcc.GetPath());
        return compiler;
    }
    return NULL;
}

bool CompilerLocatorGCC::Locate()
{
    // Locate GCC under /usr/bin
    m_compilers.clear();

    wxArrayString gcc_versions;
    gcc_versions.Add(""); // Default gcc
    gcc_versions.Add("4.2");
    gcc_versions.Add("4.3");
    gcc_versions.Add("4.4");
    gcc_versions.Add("4.5");
    gcc_versions.Add("4.6");
    gcc_versions.Add("4.7");
    gcc_versions.Add("4.8");
    gcc_versions.Add("4.9");

    for(size_t i=0; i<gcc_versions.GetCount(); ++i) {
        wxString suffix = gcc_versions.Item(i);
        if ( !suffix.IsEmpty() ) {
            suffix.Prepend("-");
        }

        wxFileName gccFile("/usr/bin", "gcc" + suffix);
        if ( gccFile.FileExists() ) {
            // add this compiler
            CompilerPtr compiler( new Compiler(NULL) );
            wxString toolchainName;
            toolchainName << "GCC";
            if ( !gcc_versions.Item(i).IsEmpty() ) {
                toolchainName << " ( " << gcc_versions.Item(i) << " )";
            }
            compiler->SetName( toolchainName );
            compiler->SetGenerateDependeciesFile(true);
            compiler->SetCompilerFamily(COMPILER_FAMILY_GCC);
            m_compilers.push_back( compiler );
            AddTools(compiler, "/usr/bin", gcc_versions.Item(i));
        }
    }

    // XCode GCC is installed under /Applications/Xcode.app/Contents/Developer/usr/bin
    wxFileName xcodeGcc("/Applications/Xcode.app/Contents/Developer/usr/bin", "gcc");
    if ( xcodeGcc.FileExists() ) {
        // add this compiler
        CompilerPtr compiler( new Compiler(NULL) );
        compiler->SetCompilerFamily(COMPILER_FAMILY_GCC);
        compiler->SetName("GCC ( XCode )");
        m_compilers.push_back( compiler );
        AddTools(compiler, xcodeGcc.GetPath());
    }

    return !m_compilers.empty();
}


void CompilerLocatorGCC::AddTools(CompilerPtr compiler,
                                  const wxString& binFolder,
                                  const wxString& suffix)
{
    wxFileName masterPath(binFolder, "");
    wxString defaultBinFolder = "/usr/bin";
    compiler->SetCompilerFamily(COMPILER_FAMILY_GCC);
    compiler->SetInstallationPath( binFolder );

    gsgs_Debug("Found GNU GCC compiler under: %s. \"%s\"", masterPath.GetPath(), compiler->GetName());
    wxFileName toolFile(binFolder, "");

    // ++++-----------------------------------------------------------------
    // With XCode installation, only
    // g++, gcc, and make are installed under the Xcode installation folder
    // the rest (mainly ar and as) are taken from /usr/bin
    // ++++-----------------------------------------------------------------

    toolFile.SetFullName("g++");
    AddTool(compiler, "CXX", toolFile.GetFullPath(), suffix);
    AddTool(compiler, "LinkerName", toolFile.GetFullPath(), suffix);
#ifndef __WXMAC__
    AddTool(compiler, "SharedObjectLinkerName", toolFile.GetFullPath(), suffix, "-shared -fPIC");
#else
    AddTool(compiler, "SharedObjectLinkerName", toolFile.GetFullPath(), suffix, "-dynamiclib -fPIC");
#endif
    toolFile.SetFullName("gcc");
    AddTool(compiler, "CC", toolFile.GetFullPath(), suffix);
    toolFile.SetFullName("make");
    wxString makeExtraArgs;
    if ( wxThread::GetCPUCount() > 1 ) {
        makeExtraArgs << "-j" << wxThread::GetCPUCount();
    }
    AddTool(compiler, "MAKE", toolFile.GetFullPath(), "", makeExtraArgs);

    // ++++-----------------------------------------------------------------
    // From this point on, we use /usr/bin only
    // ++++-----------------------------------------------------------------

    toolFile.AssignDir( defaultBinFolder );
    toolFile.SetFullName("ar");
    AddTool(compiler, "AR", toolFile.GetFullPath(), "", "rcu");

    toolFile.SetFullName("windres");
    AddTool(compiler, "ResourceCompiler", "", "");

    toolFile.SetFullName("as");
    AddTool(compiler, "AS", toolFile.GetFullPath(), "");
    
    toolFile.SetFullName("gdb");
    if(toolFile.Exists()) {
        AddTool(compiler, "Debugger", toolFile.GetFullPath(), "");
    }
}

void CompilerLocatorGCC::AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& toolpath, const wxString& suffix, const wxString& extraArgs)
{
    wxString tool = toolpath;
    if ( !suffix.IsEmpty() ) {
        tool << "-" << suffix;
    }

    ::WrapWithQuotes(tool);
    if(!extraArgs.IsEmpty()) {
        tool << " " << extraArgs;
    }
    compiler->SetTool(toolname, tool);

    gsgs_Debug("Adding tool: %s => %s", toolname, tool);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// CompilerLocatorMinGW
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CompilerLocatorMinGW::CompilerLocatorMinGW()
{
}

CompilerLocatorMinGW::~CompilerLocatorMinGW()
{
}

CompilerPtr CompilerLocatorMinGW::Locate(const wxString& folder)
{
    m_compilers.clear();
    wxFileName gcc(folder, "gcc");
#ifdef __WXMSW__
    gcc.SetExt("exe");
#endif

    bool found = gcc.FileExists();
    if ( ! found ) {
        // try to see if we have a bin folder here
        gcc.AppendDir("bin");
        found = gcc.FileExists();
    }

    if ( found ) {
        AddTools(gcc.GetPath(), GetGCCVersion(gcc.GetFullPath() ));
        return *m_compilers.begin();
    }
    return NULL;
}

wxString CompilerLocatorMinGW::GetGCCVersion(const wxString& gccBinary)
{
    wxString command;
    wxArrayString stdoutArr;
    command << gccBinary << " --version";
    ProcUtils::SafeExecuteCommand(command, stdoutArr);
    if ( !stdoutArr.IsEmpty() ) {
        wxString versionString = stdoutArr.Item(0).Trim().Trim(false);
        return versionString;
    }
    return "";
}

bool CompilerLocatorMinGW::Locate()
{
    m_compilers.clear();
    m_locatedFolders.clear();

    // for wxRegKey
#ifdef __WXMSW__

    {
        // HKEY_LOCAL_MACHINE\SOFTWARE\gsgs\settings
        wxRegKey regClMinGW(wxRegKey::HKLM, "SOFTWARE\\gsgs\\settings");
        wxString clInstallFolder;
        if ( regClMinGW.QueryValue("MinGW", clInstallFolder) && wxDirExists(clInstallFolder)) {
            wxFileName gccExe(clInstallFolder, "gcc.exe");
            wxString ver;
            regClMinGW.QueryValue("MinGW_Version", ver);
            gccExe.AppendDir("bin");
            if ( gccExe.FileExists() ) {
                AddTools(gccExe.GetPath(), "gsgs-" + ver);
            }
        }
    }

    {
        // HKEY_LOCAL_MACHINE\SOFTWARE\gsgs\settings
        wxRegKey regClMinGW(wxRegKey::HKLM, "SOFTWARE\\Wow6432Node\\gsgs\\settings");
        wxString clInstallFolder;
        if ( regClMinGW.QueryValue("MinGW", clInstallFolder) && wxDirExists(clInstallFolder)) {
            wxFileName gccExe(clInstallFolder, "gcc.exe");
            wxString ver;
            regClMinGW.QueryValue("MinGW_Version", ver);
            gccExe.AppendDir("bin");
            if ( gccExe.FileExists() ) {
                AddTools(gccExe.GetPath(), "gsgs-" + ver);
            }
        }
    }

    // locate codeblock's MinGW
    wxRegKey regCB(wxRegKey::HKCU, "SOFTWARE\\CodeBlocks");
    wxString cbInstallPath;
    if ( regCB.QueryValue("Path", cbInstallPath) ) {
        wxFileName mingwBinFolder( cbInstallPath, "" );
        mingwBinFolder.AppendDir("MinGW");
        mingwBinFolder.AppendDir("bin");
        if ( mingwBinFolder.DirExists() && wxFileName(mingwBinFolder.GetFullPath(), "gcc.exe").FileExists() ) {
            AddTools(mingwBinFolder.GetPath(), "Code::Blocks");
        }
    }
    
    // TDM-GCC installations in %ProgramData%/TDM-GCC/installations.txt
    wxString programDataDir;
    wxGetEnv("ProgramData", &programDataDir);
    wxFileName tdmgccInstall(programDataDir, "installations.txt");
    tdmgccInstall.AppendDir("TDM-GCC");
    
    if (tdmgccInstall.FileExists()) {
        wxFileInputStream input(tdmgccInstall.GetFullPath());
        wxTextInputStream text(input);
        while (input.IsOk() && !input.Eof()) {
            wxString instDir = text.ReadLine();
            if (!instDir.IsEmpty() && wxDirExists(instDir)) {
                wxFileName tdmBinDir(instDir, "g++.exe");
                tdmBinDir.AppendDir("bin");
                if (tdmBinDir.FileExists())
                    AddTools(tdmBinDir.GetPath());
            }
        }
    }

    // check uninstall keys
    std::vector<wxString> unInstKey;
    unInstKey.push_back("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
    unInstKey.push_back("SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall");

    std::vector<wxRegKey::StdKey> regBase;
    regBase.push_back(wxRegKey::HKCU);
    regBase.push_back(wxRegKey::HKLM);

    for (size_t i = 0; i < regBase.size(); ++i) {
        for (size_t j = 0; j < unInstKey.size(); ++j) {
            wxRegKey regKey(regBase[i], unInstKey[j]);
            if (!regKey.Exists() || !regKey.Open(wxRegKey::Read))
                continue;

            size_t subkeys = 0;
            regKey.GetKeyInfo(&subkeys, NULL, NULL, NULL);
            wxString keyName;
            long keyIndex = 0;
            regKey.GetFirstKey(keyName, keyIndex);

            for (size_t k = 0; k < subkeys; ++k) {
                wxRegKey subKey(regKey, keyName);
                if (!subKey.Exists() || !subKey.Open(wxRegKey::Read))
                    continue;

                wxString displayName, installFolder;
                if (subKey.HasValue("DisplayName")
                    && subKey.HasValue("InstallLocation")
                    && subKey.QueryValue("DisplayName", displayName)
                    && subKey.QueryValue("InstallLocation", installFolder))
                {
                    CheckRegKey(displayName, installFolder);
                }

                subKey.Close();
                regKey.GetNextKey(keyName, keyIndex);
            }
            regKey.Close();
        }
    }

    // Last: many people install MinGW by simply extracting it into the
    // root folder:
    // C:\MinGW-X.Y.Z
    wxArrayString volumes = wxFSVolume::GetVolumes();
    wxArrayString mingwFolderArr;
    // Get list of folders for the volume only
    for(size_t i=0; i<volumes.GetCount(); ++i) {
        wxDir dir( volumes.Item(i) );
        if ( dir.IsOpened() ) {
            wxString path;
            bool cont = dir.GetFirst(&path, "*mingw*", wxDIR_DIRS);
            while (cont ) {
                wxString fullpath;
                fullpath << volumes.Item(i) << path;
                gsgs_Debug("Found folder containing MinGW: %s", fullpath);
                mingwFolderArr.Add( fullpath );
                cont = dir.GetNext( &path );
            }
        }
    }

    for(size_t i=0; i<mingwFolderArr.GetCount(); ++i) {
        wxString binFolder = FindBinFolder( mingwFolderArr.Item(i) );
        if ( binFolder.IsEmpty() )
            continue;

        wxFileName gcc(binFolder, "gcc.exe");
        if( gcc.FileExists() ) {
            AddTools(gcc.GetPath());
        }
    }
#endif

    // try to find MinGW in environment variable PATH (last)
    wxString pathValues;
    wxGetEnv("PATH", &pathValues);

    if ( !pathValues.IsEmpty() ) {
        wxArrayString pathArray = ::wxStringTokenize(pathValues, wxPATH_SEP, wxTOKEN_STRTOK);
        for (size_t i = 0; i < pathArray.GetCount(); ++i) {
            wxFileName gccComp( pathArray.Item(i), "gcc.exe" );
            if ( gccComp.GetDirs().Last() == "bin" && gccComp.Exists() ) {
                // We found gcc.exe
                wxString pathToGcc = gccComp.GetPath();
                pathToGcc.MakeLower();

                // Don't mix cygwin and mingw
                if ( !pathToGcc.Contains("cygwin") ) {
                    AddTools( gccComp.GetPath() );
                }
            }
        }
    }

    return !m_compilers.empty();
}

void CompilerLocatorMinGW::CheckRegKey(const wxString& displayName, const wxString& installFolder)
{
    if (displayName.StartsWith("TDM-GCC")) {
        wxFileName fnTDMBinFolder(installFolder, "");
        fnTDMBinFolder.AppendDir("bin");
        fnTDMBinFolder.SetFullName("g++.exe");
        if (fnTDMBinFolder.FileExists())
            AddTools(fnTDMBinFolder.GetPath(), displayName);
    }

    else if (displayName.StartsWith("MSYS2")) {
        wxFileName fnMingw32BinFolder(installFolder, "");
        fnMingw32BinFolder.AppendDir("mingw32");
        fnMingw32BinFolder.AppendDir("bin");
        fnMingw32BinFolder.SetFullName("g++.exe");
        if (fnMingw32BinFolder.FileExists())
            AddTools(fnMingw32BinFolder.GetPath(), "MinGW 32bit ( " + displayName + " )");

        wxFileName fnMingw64BinFolder(installFolder, "");
        fnMingw64BinFolder.AppendDir("mingw64");
        fnMingw64BinFolder.AppendDir("bin");
        fnMingw64BinFolder.SetFullName("g++.exe");
        if (fnMingw64BinFolder.FileExists())
            AddTools(fnMingw64BinFolder.GetPath(), "MinGW 64bit ( " + displayName + " )");
    }
}

void CompilerLocatorMinGW::AddTools(const wxString& binFolder, const wxString& name)
{
    wxFileName masterPath(binFolder, "");
    masterPath.RemoveLastDir();
    if ( m_locatedFolders.count(masterPath.GetPath()) ) {
        return;
    }

    // Create an empty compiler
    CompilerPtr compiler( new Compiler(NULL) );
    compiler->SetCompilerFamily(COMPILER_FAMILY_MINGW);
    compiler->SetGenerateDependeciesFile(true);
    m_compilers.push_back( compiler );
    m_locatedFolders.insert( masterPath.GetPath() );

    if ( name.IsEmpty() )
        compiler->SetName("MinGW ( " + masterPath.GetDirs().Last() + " )");
    else if (!name.Lower().Contains("mingw"))
        compiler->SetName("MinGW ( " + name + " )");
    else
        compiler->SetName(name);
    compiler->SetInstallationPath( masterPath.GetPath() );

    gsgs_Debug("Found MinGW compiler under: %s. \"%s\"", masterPath.GetPath(), compiler->GetName());
    wxFileName toolFile(binFolder, "");

    toolFile.SetFullName("g++.exe");
    AddTool(compiler, "CXX", toolFile.GetFullPath());
    AddTool(compiler, "LinkerName", toolFile.GetFullPath());
    AddTool(compiler, "SharedObjectLinkerName", toolFile.GetFullPath(), "-shared -fPIC");

    toolFile.SetFullName("gcc.exe");
    AddTool(compiler, "CC", toolFile.GetFullPath());

    toolFile.SetFullName("ar.exe");
    AddTool(compiler, "AR", toolFile.GetFullPath(), "rcu");

    toolFile.SetFullName("windres.exe");
    AddTool(compiler, "ResourceCompiler", toolFile.GetFullPath());

    toolFile.SetFullName("as.exe");
    AddTool(compiler, "AS", toolFile.GetFullPath());

    toolFile.SetFullName("make.exe");
    wxString makeExtraArgs;
    if ( wxThread::GetCPUCount() > 1 ) {
        makeExtraArgs << "-j" << wxThread::GetCPUCount();
    }

    // This is needed under MinGW
    makeExtraArgs <<  " SHELL=cmd.exe ";

    if ( toolFile.FileExists() ) {
        AddTool(compiler, "MAKE", toolFile.GetFullPath(), makeExtraArgs);

    } else {
        toolFile.SetFullName("mingw32-make.exe");
        if ( toolFile.FileExists() ) {
            AddTool(compiler, "MAKE", toolFile.GetFullPath(), makeExtraArgs);
        }
    }

    toolFile.SetFullName("gdb.exe");
    if(toolFile.Exists()) {
        AddTool(compiler, "Debugger", toolFile.GetFullPath());
    }
}

void CompilerLocatorMinGW::AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& toolpath, const wxString& extraArgs)
{
    wxString tool = toolpath;
    ::WrapWithQuotes(tool);
    if(!extraArgs.IsEmpty()) {
        tool << " " << extraArgs;
    }
    compiler->SetTool(toolname, tool);
}

wxString CompilerLocatorMinGW::FindBinFolder(const wxString& parentPath)
{
    std::queue<wxString> dirs;
    dirs.push( parentPath );

    while ( !dirs.empty() ) {
        wxString curdir = dirs.front();
        dirs.pop();

        wxFileName fn(curdir, "" );
        fn.AppendDir("bin");

        if ( fn.DirExists() && fn.GetDirCount() && fn.GetDirs().Last() == "bin") {
            return fn.GetPath();
        }

        // Check to see if there are more directories to recurse
        wxDir dir;
        if ( dir.Open( curdir ) ) {
            wxString dirname;
            bool cont = dir.GetFirst( &dirname, "", wxDIR_DIRS );
            while ( cont ) {
                wxString new_dir;
                new_dir << curdir << gsgsPathSep << dirname;
                dirs.push( new_dir );
                dirname.Clear();
                cont = dir.GetNext( &dirname );
            }
        }
    }
    return wxEmptyString;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// CompilerLocatorMSVC
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CompilerLocatorMSVC::CompilerLocatorMSVC() {}

CompilerLocatorMSVC::~CompilerLocatorMSVC() {}

bool CompilerLocatorMSVC::Locate()
{
    m_compilers.clear();

    wxArrayString vc_platforms;
    vc_platforms.Add("x86");
    vc_platforms.Add("x64");

    wxEnvVariableHashMap envvars;
    ::wxGetEnvMap(&envvars);

    for (wxEnvVariableHashMap::const_iterator it = envvars.begin(); it != envvars.end(); ++it) {
        wxString const& envvarName = it->first;
        wxString const& envvarPath = it->second;

        if (!envvarName.Matches("VS??*COMNTOOLS") || envvarPath.IsEmpty() || (envvarName.Find('C') < 3)) {
            continue;
        }

        wxString vcVersion = envvarName.Mid(2, envvarName.Find('C') - 3);
        for(size_t j = 0; j < vc_platforms.GetCount(); ++j) {
            wxString compilerName = "Visual C++ " + vcVersion + " (" + vc_platforms[j] + ")";
            AddTools(envvarPath, compilerName, vc_platforms[j]);
        }
    }

    return !m_compilers.empty();
}

void CompilerLocatorMSVC::AddTools(const wxString& masterFolder,
                                   const wxString& name,
                                   const wxString& platform)
{
    wxFileName installPath(masterFolder, "");
    installPath.RemoveLastDir();
    installPath.RemoveLastDir();

    CompilerPtr compiler(new Compiler(NULL, Compiler::kRegexVC));
    compiler->SetCompilerFamily(COMPILER_FAMILY_VC);
    compiler->SetName(name);
    compiler->SetInstallationPath(installPath.GetPath());

    // CXX
    AddTool("cl.exe", "/nologo /TP /FC", "CXX", compiler);

    // CC
    AddTool("cl.exe", "/nologo /TC /FC", "CC", compiler);

    // AR
    AddTool("lib.exe", "/nologo", "AR", compiler);

    // SharedObjectLinkerName
    AddTool("link.exe", "/nologo /DLL", "SharedObjectLinkerName", compiler);

    // LinkerName
    AddTool("link.exe", "/nologo", "LinkerName", compiler);

    // AS
    AddTool("ml.exe", "/nologo", "AS", compiler);

    // Resource
    AddTool("rc.exe", "/nologo", "ResourceCompiler", compiler);
    compiler->AddCmpFileType("rc",
                       Compiler::CmpFileKindResource,
                       "$(RcCompilerName) $(RcCmpOptions) "
                       "$(Objective)$(IntermediateDirectory)/"
                       "$(ObjectName)$(ObjectSuffix) $(RcIncludePath) \"$(FileFullPath)\"");

    //Make
    wxFileName fnVCvars(installPath);
    fnVCvars.AppendDir("VC");
    fnVCvars.SetFullName("vcvarsall.bat");
    wxString makeArgs = platform + " > nul";
    AddTool(fnVCvars.GetFullPath(), makeArgs, "MAKE", compiler);

    compiler->SetSwitch("ArchiveOutput", "/OUT:");
    compiler->SetSwitch("Debug", "/Zi ");
    compiler->SetSwitch("Include", "/I");
    compiler->SetSwitch("Library", " ");
    compiler->SetSwitch("LibraryPath", "/LIBPATH:");
    compiler->SetSwitch("Object", "/Fo");
    compiler->SetSwitch("Output", "/OUT:");
    compiler->SetSwitch("PreprocessOnly", "/P");
    compiler->SetSwitch("Preprocessor", "/D");
    compiler->SetSwitch("Source", "");
    compiler->SetObjectSuffix(".obj");

    // IDE path
    wxFileName fnIdeFolder(masterFolder, "");
    fnIdeFolder.RemoveLastDir();
    fnIdeFolder.AppendDir("IDE");
    compiler->SetPathVariable(fnIdeFolder.GetPath() + ";$PATH");

    // include and lib path, check if cl.exe exists
    wxString includePathCmd = "echo \%INCLUDE\%";
    WrapInShell(includePathCmd);
    wxString libPathCmd = "echo \%LIB\%";
    WrapInShell(libPathCmd);
    wxString clCheck = "where cl.exe";
    wxString command = compiler->GetTool("MAKE") + " & " + includePathCmd + " & " + libPathCmd + " & " + clCheck;
    WrapInShell(command);

    wxArrayString output;
    wxArrayString errors;
    wxExecute(command, output, errors);

    if (output.size() >= 2) {
        wxString includePath = output[0];
        if (includePath.Trim().Trim(false) != "\%INCLUDE\%") {
            compiler->SetGlobalIncludePath(includePath);
        }

        wxString libPath = output[1];
        if (libPath.Trim().Trim(false) != "\%LIB\%") {
            compiler->SetGlobalLibPath(libPath);
        }
    }

    AddCompilerOptions(compiler);
    AddLinkerOptions(compiler);

    // cl.exe exists
    if (errors.IsEmpty()) {
        m_compilers.push_back(compiler);
    }
}

void CompilerLocatorMSVC::AddTool(const wxString& toolpath,
                                      const wxString& extraArgs,
                                      const wxString& toolname,
                                      CompilerPtr compiler)
{
    wxString tool = toolpath;
    ::WrapWithQuotes(tool);

    if(!extraArgs.IsEmpty()) {
        tool << " " << extraArgs;
    }
    compiler->SetTool(toolname, tool);
}

void CompilerLocatorMSVC::AddCompilerOptions(CompilerPtr compiler)
{
    compiler->AddCompilerOption("/c", "Compiles without linking.");
    compiler->AddCompilerOption("/MD", "Causes the application to use the multithread-specific and DLL-specific version of the run-time library.");
    compiler->AddCompilerOption("/MDd", "Causes the application to use the debug multithread-specific and DLL-specific version of the run-time library.");
    compiler->AddCompilerOption("/MT", "Causes the application to use the multithread, static version of the run-time library.");
    compiler->AddCompilerOption("/MTd", "Causes the application to use the debug multithread, static version of the run-time library.");

    compiler->AddCompilerOption("/O1", "Creates small code.");
    compiler->AddCompilerOption("/O2", "Creates fast code.");
    compiler->AddCompilerOption("/Od", "Disables optimization.");
    compiler->AddCompilerOption("/Ox", "Uses maximum optimization.");
    compiler->AddCompilerOption("/Oi", "Generates intrinsic functions.");

    compiler->AddCompilerOption("/MP", "Compiles multiple source files by using multiple processes.");
    compiler->AddCompilerOption("/sdl", "Enables additional security features and warnings.");
    compiler->AddCompilerOption("/errorReport:none", "Reports about internal compiler errors will not be collected or sent to Microsoft.");
    compiler->AddCompilerOption("/errorReport:prompt", "Prompts you to send a report when you receive an internal compiler error.");
    compiler->AddCompilerOption("/FS", "Forces writes to the program database (PDB) file to be serialized through MSPDBSRV.EXE.");
    compiler->AddCompilerOption("/Zs", "Checks syntax only.");
    compiler->AddCompilerOption("/GA", "Optimizes code for Windows application.");
    compiler->AddCompilerOption("/GL", "Enables whole program optimization.");
    compiler->AddCompilerOption("/Gm", "Enables minimal rebuild.");
    compiler->AddCompilerOption("/Gy", "Enables function-level linking.");
    compiler->AddCompilerOption("/EHa", "Enable C++ Exceptions with SEH exceptions");
    compiler->AddCompilerOption("/EHs", "Enable C++ Exceptions with Extern C functions.");
    compiler->AddCompilerOption("/EHsc", "Enable C++ Exceptions with SEH and Extern C functions.");

    compiler->AddCompilerOption("/Z7", "Produces an .obj file containing full symbolic debugging information for use with the debugger.");
    compiler->AddCompilerOption("/Zi", "Produces a program database (PDB) that contains type information and symbolic debugging information for use with the debugger.");
    compiler->AddCompilerOption("/ZI", "Produces a program database, as described above, in a format that supports the Edit and Continue feature.");

    compiler->AddCompilerOption("/w", "Disables all compiler warnings.");
    compiler->AddCompilerOption("/W0", "Disables all warnings.");
    compiler->AddCompilerOption("/W1", "Displays level 1 (severe) warnings.");
    compiler->AddCompilerOption("/W2", "Displays level 1 and level 2 (significant) warnings.");
    compiler->AddCompilerOption("/W3", "Displays level 1, level 2 and level 3 (production quality) warnings.");
    compiler->AddCompilerOption("/W4", "Displays level 1, level 2, and level 3 warnings, and all level 4 (informational) warnings that are not turned off by default.");
    compiler->AddCompilerOption("/Wall", "Displays all warnings displayed by /W4 and all other warnings that /W4 does not include.");
    compiler->AddCompilerOption("/WX", "Treats all compiler warnings as errors.");
}

void CompilerLocatorMSVC::AddLinkerOptions(CompilerPtr compiler)
{
    compiler->AddLinkerOption("/DEBUG", "Creates debugging information.");
    compiler->AddLinkerOption("/DYNAMICBASE", "Use address space layout randomization.");
    compiler->AddLinkerOption("/DYNAMICBASE:NO", "Don't use address space layout randomization");
    compiler->AddLinkerOption("/ERRORREPORT:NONE", "Reports about internal compiler errors will not be collected or sent to Microsoft.");
    compiler->AddLinkerOption("/ERRORREPORT:PROMPT", "Prompts you to send a report when you receive an internal compiler error.");
    compiler->AddLinkerOption("/INCREMENTAL", "Enables incremental linking.");
    compiler->AddLinkerOption("/INCREMENTAL:NO", "Disables incremental linking.");
    compiler->AddLinkerOption("/LARGEADDRESSAWARE", "Tells the compiler that the application supports addresses larger than two gigabytes.");
    compiler->AddLinkerOption("/LARGEADDRESSAWARE:NO", "Tells the compiler that the application does not support addresses larger than two gigabytes.");

    compiler->AddLinkerOption("/LTCG:INCREMENTAL", "Specifies link-time code generation.");
    compiler->AddLinkerOption("/LTCG:STATUS", "Specifies link-time code generation.");
    compiler->AddLinkerOption("/LTCG:NOSTATUS", "Specifies link-time code generation.");
    compiler->AddLinkerOption("/LTCG:OFF", "Specifies link-time code generation.");
    compiler->AddLinkerOption("/MACHINE:X64", "Specifies the target platform.");
    compiler->AddLinkerOption("/MACHINE:X86", "Specifies the target platform.");
    compiler->AddLinkerOption("/NOENTRY", "Creates a resource-only DLL.");
    compiler->AddLinkerOption("/NXCOMPAT", "Specify Compatibility with Data Execution Prevention.");
    compiler->AddLinkerOption("/NXCOMPAT:NO", "Specify Compatibility with Data Execution Prevention.");

    compiler->AddLinkerOption("/OPT:REF", "Controls LINK optimizations.");
    compiler->AddLinkerOption("/OPT:NOREF", "Controls LINK optimizations.");
    compiler->AddLinkerOption("/OPT:ICF", "Controls LINK optimizations.");
    compiler->AddLinkerOption("/OPT:NOICF", "Controls LINK optimizations.");
    compiler->AddLinkerOption("/OPT:LBR", "Controls LINK optimizations.");
    compiler->AddLinkerOption("/OPT:NOLBR", "Controls LINK optimizations.");

    compiler->AddLinkerOption("/PROFILE", "Produces an output file that can be used with the Performance Tools profiler.");
    compiler->AddLinkerOption("/SAFESEH", "Image has Safe Exception Handlers.");
    compiler->AddLinkerOption("/SAFESEH:NO", "Image does not have Safe Exception Handlers");
    compiler->AddLinkerOption("/SUBSYSTEM:CONSOLE", "Tells the operating system how to run the .exe file.");
    compiler->AddLinkerOption("/SUBSYSTEM:WINDOWS", "Tells the operating system how to run the .exe file.");
    compiler->AddLinkerOption("/VERBOSE", "Prints linker progress messages.");
    compiler->AddLinkerOption("/WX", "Treats linker warnings as errors.");
    compiler->AddLinkerOption("/WX:NO", "Do not treats linker warnings as errors.");
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// CompilerLocatorCrossGCC
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CompilerLocatorCrossGCC::CompilerLocatorCrossGCC() {}

CompilerLocatorCrossGCC::~CompilerLocatorCrossGCC() {}

CompilerPtr CompilerLocatorCrossGCC::Locate(const wxString& folder) { return Locate(folder, true); }

CompilerPtr CompilerLocatorCrossGCC::Locate(const wxString& folder, bool clear)
{
    if(clear) {
        m_compilers.clear();
    }

    wxArrayString matches;
    wxFileName fnFolder(folder, "");

    // We collect "*-gcc" files
    wxString pattern = "*-gcc";
#ifdef __WXMSW__
    pattern << ".exe";
#endif

    int count = wxDir::GetAllFiles(fnFolder.GetPath(), &matches, pattern, wxDIR_FILES);
    if(count == 0) {
        // try to see if we have a 'bin' folder under 'folder'
        fnFolder.AppendDir("bin");
        if(wxDir::Exists(fnFolder.GetPath())) {
            count = wxDir::GetAllFiles(fnFolder.GetPath(), &matches, pattern, wxDIR_FILES);
        }
    }

    if(count == 0) return NULL;

    for(int i = 0; i < count; ++i) {
#ifndef __WXMSW__
        // Check if this is a script
        char sha[2];
        wxFile(matches[i]).Read(sha, 2);
        if(strncmp(sha, "#!", 2) == 0) {
            continue;
        }
#endif
        wxFileName filename(matches.Item(i));
        if(filename.GetName() == "mingw32-gcc" || filename.GetName() == "x86_64-w64-mingw32-gcc") {
            // Don't include standard mingw32-gcc (32 and 64 bit) binaries
            // they will be picked up later by the MinGW locator
            continue;
        }

        CompilerPtr compiler(new Compiler(NULL));
        compiler->SetCompilerFamily(COMPILER_FAMILY_GCC);

        // get the compiler version
        compiler->SetName(filename.GetName());
        compiler->SetGenerateDependeciesFile(true);
        m_compilers.push_back(compiler);

        // we path the bin folder
        AddTools(compiler, filename.GetPath(), filename.GetName().BeforeLast('-'), filename.GetExt());
    }

    if(m_compilers.empty()) {
        return NULL;
    } else {
        return *m_compilers.begin();
    }
}

bool CompilerLocatorCrossGCC::Locate()
{
    m_compilers.clear();

    // try to find a cross GCC in the PATH
    wxString pathValues;
    wxGetEnv("PATH", &pathValues);
    gsgsStringSet tried;

    if(!pathValues.IsEmpty()) {
        wxArrayString pathArray = ::wxStringTokenize(pathValues, wxPATH_SEP, wxTOKEN_STRTOK);
        for(size_t i = 0; i < pathArray.GetCount(); ++i) {
            if(tried.count(pathArray[i])) continue;
            Locate(pathArray[i], false);
            tried.insert(pathArray[i]);
        }
    }

    return !m_compilers.empty();
}

void CompilerLocatorCrossGCC::AddTools(CompilerPtr compiler,
                                       const wxString& binFolder,
                                       const wxString& prefix,
                                       const wxString& suffix)
{
    compiler->SetName("Cross GCC ( " + prefix + " )");
    compiler->SetInstallationPath(binFolder);

    gsgs_Debug("Found CrossGCC compiler under: %s. \"%s\"", binFolder, compiler->GetName());
    wxFileName toolFile(binFolder, "");

    toolFile.SetFullName(prefix + "-g++");
    toolFile.SetExt(suffix);
    AddTool(compiler, "CXX", toolFile.GetFullPath());
    AddTool(compiler, "LinkerName", toolFile.GetFullPath());
    AddTool(compiler, "SharedObjectLinkerName", toolFile.GetFullPath(), "-shared -fPIC");

    toolFile.SetFullName(prefix + "-gcc");
    toolFile.SetExt(suffix);
    AddTool(compiler, "CC", toolFile.GetFullPath());

    toolFile.SetFullName(prefix + "-ar");
    toolFile.SetExt(suffix);
    AddTool(compiler, "AR", toolFile.GetFullPath(), "rcu");

    toolFile.SetFullName(prefix + "-windres");
    toolFile.SetExt(suffix);
    if(toolFile.FileExists()) AddTool(compiler, "ResourceCompiler", toolFile.GetFullPath());

    toolFile.SetFullName(prefix + "-as");
    toolFile.SetExt(suffix);
    AddTool(compiler, "AS", toolFile.GetFullPath());

    toolFile.SetFullName(prefix + "-gdb");
    toolFile.SetExt(suffix);
    AddTool(compiler, "Debugger", toolFile.GetFullPath());

    toolFile.SetFullName("make");
    toolFile.SetExt(suffix);
    wxString makeExtraArgs;
    if(wxThread::GetCPUCount() > 1) {
        makeExtraArgs << "-j" << wxThread::GetCPUCount();
    }

    // XXX Need this on Windows?
    // makeExtraArgs <<  " SHELL=cmd.exe ";

    // What to do if there's no make here? (on Windows)
    if(toolFile.FileExists()) AddTool(compiler, "MAKE", toolFile.GetFullPath(), makeExtraArgs);
}

void CompilerLocatorCrossGCC::AddTool(CompilerPtr compiler,
                                      const wxString& toolname,
                                      const wxString& toolpath,
                                      const wxString& extraArgs)
{
    wxString tool = toolpath;
    ::WrapWithQuotes(tool);
    if(!extraArgs.IsEmpty()) {
        tool << " " << extraArgs;
    }
    compiler->SetTool(toolname, tool);
    gsgs_Debug("Adding tool: %s => %s", toolname, tool);
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// CompilerLocatorCygwin
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CompilerLocatorCygwin::CompilerLocatorCygwin()
    : ToolsetLocator()
{
}

bool CompilerLocatorCygwin::Locate()
{
    m_compilers.clear();
#ifdef __WXMSW__ // for wxRegKey
    {
        wxRegKey regkey(wxRegKey::HKLM, "SOFTWARE\\Cygwin\\setup");
        wxString cygwinInstallDir;
        if ( regkey.QueryValue("rootdir", cygwinInstallDir) && wxDirExists(cygwinInstallDir)) {
            Locate( cygwinInstallDir );
        }
    }   
    {
        // If we are running a 64 bit version of gsgs, we should search under the 
        // Wow6432Node
        wxRegKey regkey(wxRegKey::HKLM, "SOFTWARE\\Wow6432Node\\Cygwin\\setup");
        wxString cygwinInstallDir;
        if ( regkey.QueryValue("rootdir", cygwinInstallDir) && wxDirExists(cygwinInstallDir)) {
            Locate( cygwinInstallDir );
        }
    }
    
#endif
    return !m_compilers.empty();
}

void CompilerLocatorCygwin::AddTools(const wxString& binFolder, const wxString& name, const wxString& suffix)
{
    wxFileName masterPath(binFolder, "");
    masterPath.RemoveLastDir();
    
    // Create an empty compiler
    CompilerPtr compiler( new Compiler(NULL) );
    compiler->SetCompilerFamily(COMPILER_FAMILY_CYGWIN);
    compiler->SetGenerateDependeciesFile(true);
    compiler->SetName( name );
    compiler->SetInstallationPath( masterPath.GetPath() );
    m_compilers.push_back( compiler );
    
    gsgs_Debug("Found Cygwin compiler under: %s. \"%s\"", masterPath.GetPath(), compiler->GetName());
    wxFileName toolFile(binFolder, "");

    toolFile.SetFullName( "g++-" + suffix + ".exe" );
    AddTool(compiler, "CXX", toolFile.GetFullPath());
    AddTool(compiler, "LinkerName", toolFile.GetFullPath());
    AddTool(compiler, "SharedObjectLinkerName", toolFile.GetFullPath(), "-shared -fPIC");

    toolFile.SetFullName( "gcc-" + suffix + ".exe" );
    AddTool(compiler, "CC", toolFile.GetFullPath());

    toolFile.SetFullName("ar.exe");
    AddTool(compiler, "AR", toolFile.GetFullPath(), "rcu");

    toolFile.SetFullName("windres.exe");
    AddTool(compiler, "ResourceCompiler", toolFile.GetFullPath());

    toolFile.SetFullName("as.exe");
    AddTool(compiler, "AS", toolFile.GetFullPath());
    
    toolFile.SetFullName("make.exe");
    wxString makeExtraArgs;
    if ( wxThread::GetCPUCount() > 1 ) {
        makeExtraArgs << "-j" << wxThread::GetCPUCount();
    }

    if ( toolFile.FileExists() ) {
        AddTool(compiler, "MAKE", toolFile.GetFullPath(), makeExtraArgs);
        
    } else {
        toolFile.SetFullName("mingw32-make.exe");
        if ( toolFile.FileExists() ) {
            AddTool(compiler, "MAKE", toolFile.GetFullPath(), makeExtraArgs);
        }
    }
    AddTool(compiler, "MakeDirCommand", "mkdir", "-p");
    
    toolFile.SetFullName("gdb.exe");
    if ( toolFile.FileExists() ) {
        AddTool(compiler, "Debugger", toolFile.GetFullPath());
    }
}

void CompilerLocatorCygwin::AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& toolpath, const wxString& extraArgs)
{
    wxString tool = toolpath;
    ::WrapWithQuotes(tool);
    
    // Cygwin does not like backslahes... replace the tools to use / 
    tool.Replace("\\", "/");
    if(!extraArgs.IsEmpty()) {
        tool << " " << extraArgs;
    }
    compiler->SetTool(toolname, tool);
}

wxString CompilerLocatorCygwin::GetGCCVersion(const wxString& gccBinary)
{
    static wxRegEx reVersion("([0-9]+\\.[0-9]+\\.[0-9]+)");
    wxString command;
    command << gccBinary << " --version";
    wxString versionString = ProcUtils::SafeExecuteCommand(command);
    if ( !versionString.IsEmpty() && reVersion.Matches( versionString ) ) {
        return reVersion.GetMatch( versionString );
    }
    return wxEmptyString;
}

CompilerPtr CompilerLocatorCygwin::Locate(const wxString& folder)
{
    m_compilers.clear();
    
    wxString binFolder;
    wxFileName gcc(folder, "gcc.exe");
    if ( gcc.FileExists() ) {
        binFolder = gcc.GetPath();
    } else {
        gcc.AppendDir("bin");
        if ( gcc.FileExists() ) {
            binFolder = gcc.GetPath();
        }
    }
    
    if ( binFolder.IsEmpty() )
        return NULL;
    
    wxArrayString suffixes = GetSuffixes(binFolder);
    if ( suffixes.IsEmpty() )
        return NULL;
    
    for(size_t i=0; i<suffixes.GetCount(); ++i) {
        gcc.SetFullName( "gcc-" + suffixes.Item(i) + ".exe" );
        wxString gccVer = GetGCCVersion( gcc.GetFullPath() );

        wxString compilerName;
        compilerName << "Cygwin";
        if ( !gccVer.IsEmpty() ) {
            compilerName <<  " - " << gccVer;
        }
        // Add the tools (use the bin folder)
        AddTools(gcc.GetPath(), compilerName, suffixes.Item(i));
    }
    return m_compilers.at(0);
}

wxArrayString CompilerLocatorCygwin::GetSuffixes(const wxString& binFolder)
{
    wxFileName gcc3(binFolder, "gcc-3.exe");
    wxFileName gcc4(binFolder, "gcc-4.exe");
    wxFileName gcc5(binFolder, "gcc-5.exe");
    
    wxArrayString arr;
    if ( gcc3.FileExists() ) 
        arr.Add("3");
    if ( gcc4.FileExists() )
        arr.Add("4");
    if ( gcc5.FileExists() )
        arr.Add("5");
    return arr;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// CompilerLocatorEosCDT
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CompilerLocatorEosCDT::CompilerLocatorEosCDT() {}

CompilerLocatorEosCDT::~CompilerLocatorEosCDT() {}

bool CompilerLocatorEosCDT::Locate()
{
    std::vector<wxString> possiblePaths{ "/usr/bin", "/usr/local/bin" };
    std::for_each(possiblePaths.begin(), possiblePaths.end(), [&](const wxString& path) {
        wxString foundPath;
        if(CheckExists(path, foundPath)) { m_compilers.push_back(CreateCompiler(foundPath)); }
    });
    return !m_compilers.empty();
}

CompilerPtr CompilerLocatorEosCDT::Locate(const wxString& folder)
{
    m_compilers.clear();
    wxString foundPath;
    if(!CheckExists(folder, foundPath)) { return NULL; }

    m_compilers.push_back(CreateCompiler(foundPath));
    return m_compilers[0];
}

void CompilerLocatorEosCDT::AddTool(CompilerPtr compiler, const wxString& toolname, const wxString& path,
                                    const wxString& args) const
{
    wxString tool = path;
    ::WrapWithQuotes(tool);
    if(!args.IsEmpty()) { tool << " " << args; }
    compiler->SetTool(toolname, tool);
}

bool CompilerLocatorEosCDT::CheckExists(const wxString& path, wxString& foundPath) const
{
    wxFileName eosio_tool(path, "eosio-cc");
    wxFileName tmpfn(path, "");
#ifdef __WXMSW__
    eosio_tool.SetExt("exe");
#endif

    bool found = eosio_tool.FileExists();
    if(!found) {
        // try to see if we have a bin folder here
        eosio_tool.AppendDir("bin");
        found = eosio_tool.FileExists();
        if(found) { foundPath = eosio_tool.GetPath(); }
    } else {
        foundPath = eosio_tool.GetPath();
    }
    return found;
}

CompilerPtr CompilerLocatorEosCDT::CreateCompiler(const wxString& path) const
{

    CompilerPtr compiler(new Compiler(NULL));
    compiler->SetCompilerFamily(COMPILER_FAMILY_CLANG);

    // get the compiler version
    compiler->SetName("eosio");
    compiler->SetGenerateDependeciesFile(true);
    compiler->SetInstallationPath(path);
    
    // Add the tools
    wxFileName eosio_tool(path, "eosio-cc");
#ifdef __WXMSW__
    eosio_tool.SetExt("exe");
#endif
    AddTool(compiler, "CC", eosio_tool.GetFullPath());

    eosio_tool.SetName("eosio-cpp");
    AddTool(compiler, "CXX", eosio_tool.GetFullPath());
    AddTool(compiler, "LinkerName", eosio_tool.GetFullPath());
#ifdef __WXMAC__
    AddTool(compiler, "SharedObjectLinkerName", eosio_tool.GetFullPath(), "-dynamiclib -fPIC");
#else
    AddTool(compiler, "SharedObjectLinkerName", eosio_tool.GetFullPath(), "-shared -fPIC");
#endif
    eosio_tool.SetName("ar");
    AddTool(compiler, "AR", eosio_tool.GetFullPath(), "rcu");
    wxString makeExtraArgs;
    if(wxThread::GetCPUCount() > 1) {
        makeExtraArgs << "-j" << wxThread::GetCPUCount();
    }

#ifdef __WXMSW__
    AddTool(compiler, "MAKE", "mingw32-make.exe", makeExtraArgs);
#else
    AddTool(compiler, "MAKE", "make", makeExtraArgs);
#endif
    return compiler;
}