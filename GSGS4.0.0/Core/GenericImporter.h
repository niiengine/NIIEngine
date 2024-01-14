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

#ifndef GENERICIMPORTER_H
#define GENERICIMPORTER_H

#include <wx/string.h>
#include <wx/arrstr.h>
#include <vector>
#include <map>
#include <memory>

#ifdef __WXMSW__
#define STATIC_LIBRARY_EXT wxT(".lib")
#define DYNAMIC_LIBRARY_EXT wxT(".dll")
#define EXECUTABLE_EXT wxT(".exe")
#else
#define STATIC_LIBRARY_EXT wxT(".a")
#define DYNAMIC_LIBRARY_EXT wxT(".so")
#define EXECUTABLE_EXT wxT("")
#endif

typedef std::map<wxString, wxString> GenericProjectDataType;
typedef std::vector<GenericProjectDataType> GenericProjectDataListType;
typedef std::map<wxString, wxString> GenericEnvVarsType;
typedef std::pair<wxString, wxString> GenericEnvVarsValueType;

enum class GenericCfgType { DYNAMIC_LIBRARY, STATIC_LIBRARY, EXECUTABLE };

struct GenericProjectFile {
    wxString name;
    wxString vpath;
};

typedef std::shared_ptr<GenericProjectFile> GenericProjectFilePtr;

struct GenericProjectCfg {
    wxString name;
    wxString outputFilename;
    wxString intermediateDirectory;
    wxString includePath;
    wxString preprocessor;
    wxString libraries;
    wxString libPath;
    wxString cCompilerOptions;
    wxString cppCompilerOptions;
    wxString linkerOptions;
    wxString preCompiledHeader;
    wxString command;
    wxString workingDirectory;
    std::vector<wxString> preBuildCommands;
    std::vector<wxString> postBuildCommands;
    std::vector<GenericProjectFilePtr> excludeFiles;
    GenericEnvVarsType envVars;
    GenericCfgType type;
    bool enableCustomBuild;
    wxString customBuildCmd;
    wxString customCleanCmd;
    wxString customRebuildCmd;
};

typedef std::shared_ptr<GenericProjectCfg> GenericProjectCfgPtr;

struct GenericProject {
    wxString name;
    wxString path;
    wxArrayString deps;
    GenericCfgType cfgType;
    std::vector<GenericProjectCfgPtr> cfgs;
    std::vector<GenericProjectFilePtr> files;
};

typedef std::shared_ptr<GenericProject> GenericProjectPtr;

struct GenericWorkspace {
    wxString name;
    wxString path;
    std::vector<GenericProjectPtr> projects;
};

typedef std::shared_ptr<GenericWorkspace> GenericWorkspacePtr;

class GenericImporter
{
public:
    virtual bool OpenWordspace(const wxString& filename, const wxString& defaultCompiler) = 0;
    virtual bool isSupportedWorkspace() = 0;
    virtual GenericWorkspacePtr PerformImport() = 0;
};

#endif // GENERICIMPORTER_H
