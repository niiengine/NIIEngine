#include "WSImporter.h"
#include "gsgsCommonDialog.h"
#include "gsgsWorkspace.h"

#include "GenericImporter.h"
#include <wx/xml/xml.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

class BorlandCppBuilderImporter : public GenericImporter
{
public:
    virtual bool OpenWordspace(const wxString& filename, const wxString& defaultCompiler);
    virtual bool isSupportedWorkspace();
    virtual GenericWorkspacePtr PerformImport();

private:
    wxFileName wsInfo;
};

class DevCppImporter : public GenericImporter
{
public:
    virtual bool OpenWordspace(const wxString& filename, const wxString& defaultCompiler);
    virtual bool isSupportedWorkspace();
    virtual GenericWorkspacePtr PerformImport();

private:
    wxFileName wsInfo;
};

class VisualCppImporter : public GenericImporter
{
public:
    virtual bool OpenWordspace(const wxString& filename, const wxString& defaultCompiler);
    virtual bool isSupportedWorkspace();
    virtual GenericWorkspacePtr PerformImport();

private:
    void ConvertToLinuxStyle(wxString& filepath);
    void GenerateFromVC6(GenericWorkspacePtr genericWorkspace);
    void GenerateFromProjectVC6(GenericWorkspacePtr genericWorkspace, GenericProjectDataType& genericProjectData);
    void GenerateFromVC7_11(GenericWorkspacePtr genericWorkspace);
    void GenerateFromProjectVC7(GenericWorkspacePtr genericWorkspace, GenericProjectDataType& genericProjectData);
    void GenerateFromProjectVC11(GenericWorkspacePtr genericWorkspace, GenericProjectDataType& genericProjectData);
    wxString ExtractProjectCfgName(const wxString& parentCondition, const wxString& elemCondition);
    wxString ReplaceDefaultEnvVars(const wxString& str);
    void AddFilesVC7(wxXmlNode* filterChild, GenericProjectPtr genericProject,
        std::map<wxString, GenericProjectCfgPtr>& genericProjectCfgMap, wxString preVirtualPath);
        
    void AddFilesVC11(wxXmlNode* itemGroupChild, GenericProjectPtr genericProject,
        std::map<wxString, GenericProjectCfgPtr>& genericProjectCfgMap);
        
    GenericProjectFilePtr FindProjectFileByName(GenericProjectPtr genericProject, wxString filename);
    wxFileName wsInfo;
    wxString extension;
    long version;
};

class CodeBlocksImporter : public GenericImporter
{
public:
    virtual bool OpenWordspace(const wxString& filename, const wxString& defaultCompiler);
    virtual bool isSupportedWorkspace();
    virtual GenericWorkspacePtr PerformImport();
private:
    void GenerateFromWorkspace(GenericWorkspacePtr genericWorkspace);
    void GenerateFromProject(GenericWorkspacePtr genericWorkspace, GenericProjectDataType& genericProjectData);

    wxFileName wsInfo;
    wxString extension;
};

WSImporter::WSImporter()
{
    AddImporter(std::make_shared<VisualCppImporter>());
    AddImporter(std::make_shared<DevCppImporter>());
    AddImporter(std::make_shared<BorlandCppBuilderImporter>());
    AddImporter(std::make_shared<CodeBlocksImporter>());
}

WSImporter::~WSImporter() {}

void WSImporter::AddImporter(std::shared_ptr<GenericImporter> importer) { importers.push_back(importer); }

void WSImporter::Load(const wxString& filename, const wxString& defaultCompiler)
{
    this->filename = filename;
    this->defaultCompiler = defaultCompiler;
}

bool WSImporter::Import(wxString& errMsg)
{
    wxString compileName = defaultCompiler.Lower();
    bool isGccCompile = compileName.Contains(wxT("gnu")) || compileName.Contains(wxT("gcc")) ||
                        compileName.Contains(wxT("g++")) || compileName.Contains(wxT("mingw"));

    for(std::shared_ptr<GenericImporter> importer : importers) {
        if(importer->OpenWordspace(filename, defaultCompiler)) {
            if(importer->isSupportedWorkspace()) {
                GenericWorkspacePtr gworskspace = importer->PerformImport();
                wxString errMsgLocal;
                bool showDlg = true;

                if(!gsgs_CppWorker().CreateWorkspace(gworskspace->name, gworskspace->path, errMsgLocal))
                    return false;

                CxxWS* clWorkspace = NULL;
                WorkspaceConfiguration::ConfigMappingList cmlDebug;
                WorkspaceConfiguration::ConfigMappingList cmlRelease;

                for(GenericProjectPtr project : gworskspace->projects) {
                    GenericCfgType cfgType = project->cfgType;
                    wxString projectType, errMsg;

                    switch(cfgType) {
                    case GenericCfgType::DYNAMIC_LIBRARY:
                        projectType = PROJECT_TYPE_DYNAMIC_LIBRARY;
                        break;
                    case GenericCfgType::STATIC_LIBRARY:
                        projectType = PROJECT_TYPE_STATIC_LIBRARY;
                        break;
                    case GenericCfgType::EXECUTABLE:
                    default:
                        projectType = PROJECT_TYPE_EXECUTABLE;
                        break;
                    }

                    if(!gsgs_CppWorker().CreateProject(project->name, project->path, projectType, "", true,
                                                               errMsg))
                        return false;

                    ProjectPtr proj = gsgs_CppWorker().FindProjectByName(project->name, errMsg);
                    ProjectSettingsPtr le_settings(new ProjectSettings(NULL));

                    le_settings->RemoveConfiguration(wxT("Debug"));
                    le_settings->SetProjectType(projectType);

                    if(clWorkspace == NULL) 
                        clWorkspace = proj->GetWorkspace();

                    for(GenericProjectCfgPtr cfg : project->cfgs) {
                        BuildConfigPtr le_conf(new BuildConfig(NULL));

                        wxString outputFileName = wxT("");

                        switch(cfgType) {
                        case GenericCfgType::DYNAMIC_LIBRARY:
                            outputFileName = wxT("$(IntermediateDirectory)/$(ProjectName)");
                            outputFileName += DYNAMIC_LIBRARY_EXT;
                            break;
                        case GenericCfgType::STATIC_LIBRARY:
                            outputFileName = wxT("$(IntermediateDirectory)/$(ProjectName)");
                            outputFileName += STATIC_LIBRARY_EXT;
                            if(isGccCompile && outputFileName.Contains(wxT(".lib"))) {
                                outputFileName.Replace(wxT(".lib"), wxT(".a"));
                            }
                            break;
                        case GenericCfgType::EXECUTABLE:
                            outputFileName = wxT("$(IntermediateDirectory)/$(ProjectName)");
                            outputFileName += EXECUTABLE_EXT;
                            break;
                        }

                        std::vector<wxString> envVarElems;

                        le_conf->SetName(cfg->name);

                        if(!cfg->includePath.IsEmpty()) {
                            envVarElems.push_back(cfg->includePath);
                            le_conf->SetIncludePath(cfg->includePath);
                        }

                        if(!cfg->libraries.IsEmpty()) {
                            envVarElems.push_back(cfg->libraries);
                            if(isGccCompile && cfg->libraries.Contains(wxT(".lib"))) {
                                cfg->libraries.Replace(wxT(".lib"), wxT(".a"));
                            }
                            le_conf->SetLibraries(cfg->libraries);
                        }

                        if(!cfg->libPath.IsEmpty()) {
                            envVarElems.push_back(cfg->libPath);
                            le_conf->SetLibPath(cfg->libPath);
                        }

                        if(!cfg->preprocessor.IsEmpty()) {
                            envVarElems.push_back(cfg->preprocessor);
                            le_conf->SetPreprocessor(cfg->preprocessor);
                        }

                        if(!cfg->intermediateDirectory.IsEmpty())
                            le_conf->SetIntermediateDirectory(cfg->intermediateDirectory);

                        if(!cfg->outputFilename.IsEmpty()) {
                            if(isGccCompile && cfg->outputFilename.Contains(wxT(".lib"))) {
                                cfg->outputFilename.Replace(wxT(".lib"), wxT(".a"));
                            }
                            le_conf->SetOutputFileName(cfg->outputFilename);
                        } else
                            le_conf->SetOutputFileName(outputFileName);

                        if(!cfg->cCompilerOptions.IsEmpty()) le_conf->SetCCompileOptions(cfg->cCompilerOptions);

                        if(!cfg->cppCompilerOptions.IsEmpty()) le_conf->SetCompileOptions(cfg->cppCompilerOptions);

                        if(!cfg->linkerOptions.IsEmpty()) le_conf->SetLinkOptions(cfg->linkerOptions);

                        if(!cfg->preCompiledHeader.IsEmpty()) le_conf->SetPrecompiledHeader(cfg->preCompiledHeader);

                        wxString outputFileNameCommand, outputFileNameWorkingDirectory;
                        if(!cfg->outputFilename.IsEmpty()) {
                            wxFileName outputFileNameInfo(cfg->outputFilename);
                            outputFileNameCommand = wxT("./") + outputFileNameInfo.GetFullName();
                            outputFileNameWorkingDirectory = outputFileNameInfo.GetPath();
                            outputFileNameWorkingDirectory.Replace(wxT("\\"), wxT("/"));
                        }

                        if(!cfg->command.IsEmpty())
                            le_conf->SetCommand(cfg->command);
                        else if(!outputFileNameCommand.IsEmpty())
                            le_conf->SetCommand(outputFileNameCommand);
                        else
                            le_conf->SetCommand(wxT("./$(ProjectName)"));

                        if(!cfg->workingDirectory.IsEmpty())
                            le_conf->SetWorkingDirectory(cfg->workingDirectory);
                        else if(!outputFileNameWorkingDirectory.IsEmpty())
                            le_conf->SetWorkingDirectory(outputFileNameWorkingDirectory);
                        else
                            le_conf->SetWorkingDirectory(wxT("./$(IntermediateDirectory)"));

                        le_conf->SetCompilerType(defaultCompiler);

                        wxString cfgName = cfg->name.Lower();

                        if(cfgName.Contains(wxT("debug"))) {
                            ConfigMappingEntry cme(project->name, cfg->name);
                            cmlDebug.push_back(cme);
                        } else if(cfgName.Contains(wxT("release"))) {
                            ConfigMappingEntry cme(project->name, cfg->name);
                            cmlRelease.push_back(cme);
                        } else {
                            ConfigMappingEntry cme(project->name, cfg->name);
                            cmlDebug.push_back(cme);
                            cmlRelease.push_back(cme);
                        }

                        wxString buildConfigType;

                        switch(cfgType) {
                        case GenericCfgType::DYNAMIC_LIBRARY:
                            buildConfigType = PROJECT_TYPE_DYNAMIC_LIBRARY;
                            break;
                        case GenericCfgType::STATIC_LIBRARY:
                            buildConfigType = PROJECT_TYPE_STATIC_LIBRARY;
                            break;
                        case GenericCfgType::EXECUTABLE:
                        default:
                            buildConfigType = PROJECT_TYPE_EXECUTABLE;
                            break;
                        }

                        le_conf->SetProjectType(buildConfigType);

                        if(showDlg) {
                            if(envVarElems.size() > 0 && ContainsEnvVar(envVarElems)) {
                                std::set<wxString> listEnvVar = GetListEnvVarName(envVarElems);

                                for(GenericEnvVarsValueType envVar : cfg->envVars) {
                                    listEnvVar.erase(envVar.first);
                                }

                                if(listEnvVar.size() > 0) {
                                    EnvVarImporterDlg envVarImporterDlg(NULL, project->name, cfg->name, listEnvVar,
                                                                        le_conf, &showDlg);
                                    envVarImporterDlg.ShowModal();
                                }
                            }
                        }

                        wxString envVars =
                            !le_conf->GetEnvvars().IsEmpty() ? le_conf->GetEnvvars() + wxT("\n") : wxT("");

                        for(GenericEnvVarsValueType envVar : cfg->envVars) {
                            envVars += envVar.first + wxT("=") + envVar.second + wxT("\n");
                        }

                        le_conf->SetEnvvars(envVars);

                        BuildCommandList preBuildCommandList;
                        BuildCommandList postBuildCommandList;

                        for(wxString preBuildCmd : cfg->preBuildCommands) {
                            BuildCommand preBuildCommand;
                            preBuildCommand.SetCommand(preBuildCmd);
                            preBuildCommand.SetEnabled(true);

                            preBuildCommandList.push_back(preBuildCommand);
                        }

                        for(wxString postBuildCmd : cfg->postBuildCommands) {
                            BuildCommand postBuildCommand;
                            postBuildCommand.SetCommand(postBuildCmd);
                            postBuildCommand.SetEnabled(true);

                            postBuildCommandList.push_back(postBuildCommand);
                        }

                        le_conf->SetPreBuildCommands(preBuildCommandList);
                        le_conf->SetPostBuildCommands(postBuildCommandList);

                        if(cfg->enableCustomBuild) {
                            le_conf->EnableCustomBuild(cfg->enableCustomBuild);
                            le_conf->SetCustomBuildCmd(cfg->customBuildCmd);
                            le_conf->SetCustomCleanCmd(cfg->customCleanCmd);
                            le_conf->SetCustomRebuildCmd(cfg->customRebuildCmd);
                        }

                        le_settings->SetBuildConfiguration(le_conf);

                        if(!project->deps.IsEmpty()) proj->SetDependencies(project->deps, cfg->name);
                    }

                    proj->SetSettings(le_settings);

                    proj->BeginTranscation();

                    // Delete default virtual directory
                    proj->DeleteVirtualDir("include");
                    proj->DeleteVirtualDir("src");

                    for(GenericProjectFilePtr file : project->files) {
                        wxString vpath = GetVPath(file->name, file->vpath);

                        wxString vDir = wxT("");
                        wxStringTokenizer vDirList(vpath, wxT(":"));
                        while(vDirList.HasMoreTokens()) {
                            wxString vdName = vDirList.NextToken();
                            vDir += vdName;
                            proj->CreateVirtualDir(vDir);
                            vDir += wxT(":");
                        }

                        proj->AddFile(file->name, vpath);
                    }

                    proj->CommitTranscation();

                    for(GenericProjectCfgPtr cfg : project->cfgs) {
                        for(GenericProjectFilePtr excludeFile : cfg->excludeFiles) {
                            wxString vpath = GetVPath(excludeFile->name, excludeFile->vpath);

                            wxFileName excludeFileNameInfo(project->path + gsgsPathSep +
                                                           excludeFile->name);
                            proj->AddExcludeConfigForFile(excludeFileNameInfo.GetFullPath());
                        }
                    }
                }

                if(clWorkspace) {
                    BuildMatrixPtr clMatrix = clWorkspace->GetBuildMatrix();

                    WorkspaceConfigurationPtr wsconf = clMatrix->GetConfigurationByName(wxT("Debug"));
                    if(wsconf) { wsconf->SetConfigMappingList(cmlDebug); }

                    wsconf = clMatrix->GetConfigurationByName(wxT("Release"));
                    if(wsconf) { wsconf->SetConfigMappingList(cmlRelease); }

                    clWorkspace->SetBuildMatrix(clMatrix);
                }

                return true;
            }
        }
    }

    return false;
}

bool WSImporter::ContainsEnvVar(std::vector<wxString> elems)
{
    for(wxString elem : elems) {
        if(elem.Contains("$(") && elem.Contains(")")) return true;
    }

    return false;
}

std::set<wxString> WSImporter::GetListEnvVarName(std::vector<wxString> elems)
{
    bool app = false;
    wxString word = wxT(""), data = wxT("");
    std::set<wxString> list;

    for(wxString elem : elems) {
        if(!elem.IsEmpty()) { data += elem; }
    }

    const int length = data.length();

    for(int pos = 0; pos < length; pos++) {
        if(data.GetChar(pos) == wxT('$') && data.GetChar(pos + 1) == wxT('(')) {
            app = true;
            pos++;
        } else if(data.GetChar(pos) == wxT(')')) {
            if(!word.IsEmpty()) {
                list.insert(word);
                word = wxT("");
                app = false;
            }
        } else if(app) {
            word += data.GetChar(pos);
        }
    }

    return list;
}

wxString WSImporter::GetVPath(const wxString& filename, const wxString& virtualPath)
{
    wxString vpath;
    if(virtualPath.IsEmpty()) {
        wxFileName fileInfo(filename);
        wxString ext = fileInfo.GetExt().Lower();

        if(ext == wxT("h") || ext == wxT("hpp") || ext == wxT("hxx") || ext == wxT("hh") || ext == wxT("inl") ||
           ext == wxT("inc")) {
            vpath = wxT("include");
        } else if(ext == wxT("c") || ext == wxT("cpp") || ext == wxT("cxx") || ext == wxT("cc")) {
            vpath = wxT("src");
        } else if(ext == wxT("s") || ext == wxT("S") || ext == wxT("asm")) {
            vpath = wxT("src");
        } else {
            vpath = wxT("resource");
        }
    } else {
        vpath = virtualPath;

        if(vpath.Contains(wxT("\\"))) {
            vpath.Replace(wxT("\\"), wxT(":"));
        } else if(vpath.Contains(wxT("/"))) {
            vpath.Replace(wxT("/"), wxT(":"));
        }
    }

    return vpath;
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// BorlandCppBuilderImporter
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
bool BorlandCppBuilderImporter::OpenWordspace(const wxString& filename, const wxString& defaultCompiler)
{
    wsInfo.Assign(filename);

    wxString extension = wsInfo.GetExt().Lower();

    bool isValidExt = extension == wxT("bpr");

    bool result = wsInfo.FileExists() && wsInfo.IsFileReadable() && isValidExt;

    return result;
}
//------------------------------------------------------------------------------------------------
bool BorlandCppBuilderImporter::isSupportedWorkspace()
{
    wxXmlDocument projectDoc;
    if(projectDoc.Load(wsInfo.GetFullPath())) {
        wxXmlNode* root = projectDoc.GetRoot();
        if(root) {
            wxXmlNode* projectChild = root->GetChildren();
            if(projectChild) {
                wxXmlNode* macrosChild = projectChild->GetChildren();
                if(macrosChild && macrosChild->GetName() == wxT("VERSION")) {
                    wxString value = macrosChild->GetAttribute("value");
                    if(value == wxT("BCB.06.00"))
                        return true;
                }
            }
        }
    }

    return false;
}
//------------------------------------------------------------------------------------------------
GenericWorkspacePtr BorlandCppBuilderImporter::PerformImport()
{
    GenericWorkspacePtr genericWorkspace = std::make_shared<GenericWorkspace>();
    genericWorkspace->name = wsInfo.GetName();
    genericWorkspace->path = wsInfo.GetPath();

    GenericProjectPtr genericProject = std::make_shared<GenericProject>();
    genericProject->name = wsInfo.GetName();
    genericProject->path = wsInfo.GetPath();

    GenericProjectCfgPtr genericProjectCfgDebug = std::make_shared<GenericProjectCfg>();
    GenericProjectCfgPtr genericProjectCfgRelease = std::make_shared<GenericProjectCfg>();

    genericProjectCfgDebug->name = wxT("Debug");
    genericProjectCfgRelease->name = wxT("Release");

    genericProjectCfgDebug->intermediateDirectory = wxT("./Debug");
    genericProjectCfgRelease->intermediateDirectory = wxT("./Release");

    genericProject->cfgs.push_back(genericProjectCfgDebug);
    genericProject->cfgs.push_back(genericProjectCfgRelease);

    genericWorkspace->projects.push_back(genericProject);

    wxXmlDocument projectDoc;
    if(projectDoc.Load(wsInfo.GetFullPath())) {
        wxXmlNode* root = projectDoc.GetRoot();

        if(root) {
            wxXmlNode* projectChild = root->GetChildren();
            while(projectChild) {
                if(projectChild->GetName() == wxT("MACROS")) {
                    wxXmlNode* macrosChild = projectChild->GetChildren();
                    while(macrosChild) {
                        if(macrosChild->GetName() == wxT("INCLUDEPATH")) {
                            wxString projectIncludes = macrosChild->GetAttribute(wxT("value"));

                            genericProjectCfgDebug->includePath = projectIncludes;
                            genericProjectCfgRelease->includePath = projectIncludes;
                        }

                        if(macrosChild->GetName() == wxT("LIBPATH")) {
                            wxString projectLibs = macrosChild->GetAttribute(wxT("value"));

                            genericProjectCfgDebug->libPath = projectLibs;
                            genericProjectCfgRelease->libPath = projectLibs;
                        }

                        if(macrosChild->GetName() == wxT("USERDEFINES")) {
                            wxString projectUserDefines = macrosChild->GetAttribute(wxT("value"));

                            genericProjectCfgDebug->preprocessor = projectUserDefines;
                            genericProjectCfgRelease->preprocessor = projectUserDefines;
                        }

                        if(macrosChild->GetName() == wxT("MAINSOURCE")) {
                            wxString projectMainSource = macrosChild->GetAttribute(wxT("value"));
                            projectMainSource.Replace(wxT("\""), wxT(""));

                            wxFileName fnMainSource(wsInfo.GetPath() + gsgsPathSep +
                                                    projectMainSource);

                            if(fnMainSource.GetExt().Lower() == wxT("bpf")) {
                                wxFileInputStream fis(fnMainSource.GetFullPath());
                                wxTextInputStream tis(fis);

                                while(!fis.Eof()) {
                                    wxString line = tis.ReadLine();

                                    int index = line.Find(wxT("USEUNIT(\""));
                                    if(index != wxNOT_FOUND) {
                                        int begin = index + 9;
                                        int end = line.Find(wxT("\");")) - 1;

                                        wxString projectFilename = line.SubString(begin, end).Trim().Trim(false);
                                        projectFilename.Replace(wxT("\\"), wxT("/"));

                                        GenericProjectFilePtr genericProjectFile =
                                            std::make_shared<GenericProjectFile>();
                                        genericProjectFile->name = projectFilename;
                                        genericProject->files.push_back(genericProjectFile);
                                    }
                                }
                            }
                        }

                        macrosChild = macrosChild->GetNext();
                    }
                }

                if(projectChild->GetName() == wxT("LINKER")) {
                    wxXmlNode* linkerChild = projectChild->GetChildren();
                    while(linkerChild) {
                        if(linkerChild->GetName() == wxT("ALLLIB")) {
                            wxString projectLibraries = linkerChild->GetAttribute(wxT("value"));
                            projectLibraries.Replace(wxT(" "), wxT(";"));

                            genericProjectCfgDebug->libraries = projectLibraries;
                            genericProjectCfgRelease->libraries = projectLibraries;
                        }

                        linkerChild = linkerChild->GetNext();
                    }
                }

                if(projectChild->GetName() == wxT("FILELIST")) {
                    wxXmlNode* fileListChild = projectChild->GetChildren();
                    while(fileListChild) {
                        if(fileListChild->GetName() == wxT("FILE")) {
                            wxString projectFilename = fileListChild->GetAttribute(wxT("FILENAME"));
                            projectFilename.Replace(wxT("\\"), wxT("/"));

                            GenericProjectFilePtr genericProjectFile = std::make_shared<GenericProjectFile>();
                            genericProjectFile->name = projectFilename;
                            genericProject->files.push_back(genericProjectFile);
                        }

                        fileListChild = fileListChild->GetNext();
                    }
                }

                projectChild = projectChild->GetNext();
            }
        }
    }

    return genericWorkspace;
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// DevCppImporter
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
bool DevCppImporter::OpenWordspace(const wxString& filename, const wxString& defaultCompiler)
{
    wsInfo.Assign(filename);
    
    wxString extension = wsInfo.GetExt().Lower();

    bool isValidExt = extension == wxT("dev");

    bool result = wsInfo.FileExists() && wsInfo.IsFileReadable() && isValidExt;

    return result;
}
//------------------------------------------------------------------------------------------------
bool DevCppImporter::isSupportedWorkspace()
{
    wxFileInputStream fis(wsInfo.GetFullPath());
    wxTextInputStream tis(fis);

    while(!fis.Eof()) {
        wxString line = tis.ReadLine();

        if(line.Contains(wxT("[Project]"))) {
            return true;
        }
    }
    
    return false;
}
//------------------------------------------------------------------------------------------------
GenericWorkspacePtr DevCppImporter::PerformImport()
{
    GenericWorkspacePtr genericWorkspace = std::make_shared<GenericWorkspace>();
    genericWorkspace->name = wsInfo.GetName();
    genericWorkspace->path = wsInfo.GetPath();

    wxFileInputStream fis(wsInfo.GetFullPath());
    wxTextInputStream tis(fis);

    GenericProjectPtr genericProject;
    GenericProjectCfgPtr genericProjectCfgDebug;
    GenericProjectCfgPtr genericProjectCfgRelease;

    while(!fis.Eof()) {
        wxString line = tis.ReadLine();

        if(line.Contains(wxT("[Project]"))) {
            while(!line.IsEmpty()) {
                line = tis.ReadLine();
                wxStringTokenizer part(line, wxT("="));
                wxString tagName = part.GetNextToken().Trim().Trim(false);
                wxString tagValue = part.GetNextToken().Trim().Trim(false);

                if(tagName == wxT("Name")) {
                    wxString projectName = tagValue;

                    genericProject = std::make_shared<GenericProject>();
                    genericProject->name = projectName;
                    genericProject->path = wsInfo.GetPath();

                    genericProjectCfgDebug = std::make_shared<GenericProjectCfg>();
                    genericProjectCfgRelease = std::make_shared<GenericProjectCfg>();

                    genericProjectCfgDebug->name = wxT("Debug");
                    genericProjectCfgRelease->name = wxT("Release");

                    genericProjectCfgDebug->intermediateDirectory = wxT("./Debug");
                    genericProjectCfgRelease->intermediateDirectory = wxT("./Release");

                    genericProject->cfgs.push_back(genericProjectCfgDebug);
                    genericProject->cfgs.push_back(genericProjectCfgRelease);

                    genericWorkspace->projects.push_back(genericProject);
                }

                if(tagName == wxT("Type")) {
                    wxString projectType = tagValue;

                    if(projectType == wxT("2")) {
                        genericProject->cfgType = GenericCfgType::STATIC_LIBRARY;
                    } else if(projectType == wxT("3")) {
                        genericProject->cfgType = GenericCfgType::DYNAMIC_LIBRARY;
                    } else {
                        genericProject->cfgType = GenericCfgType::EXECUTABLE;
                    }

                    genericProjectCfgDebug->type = genericProject->cfgType;
                    genericProjectCfgRelease->type = genericProject->cfgType;
                }

                if(tagName == wxT("Includes")) {
                    wxString projectIncludes = tagValue;
                    projectIncludes.Replace(wxT("\""), wxT(""));
                    projectIncludes.Replace(wxT("_@@_"), wxT(" "));

                    genericProjectCfgDebug->includePath = projectIncludes;
                    genericProjectCfgRelease->includePath = projectIncludes;
                }

                if(tagName == wxT("Libs")) {
                    wxString projectLibs = tagValue;
                    projectLibs.Replace(wxT("\""), wxT(""));
                    projectLibs.Replace(wxT("_@@_"), wxT(" "));

                    genericProjectCfgDebug->libPath = projectLibs;
                    genericProjectCfgRelease->libPath = projectLibs;
                }

                if(tagName == wxT("Compiler")) {
                    wxString projectCompiler = tagValue;
                    projectCompiler.Replace(wxT("_@@_"), wxT(" "));

                    genericProjectCfgDebug->cCompilerOptions = projectCompiler;
                    genericProjectCfgRelease->cCompilerOptions = projectCompiler;
                    genericProjectCfgDebug->cppCompilerOptions = projectCompiler;
                    genericProjectCfgRelease->cppCompilerOptions = projectCompiler;
                }

                if(tagName == wxT("CppCompiler")) {
                    wxString projectCppCompiler = tagValue;
                    projectCppCompiler.Replace(wxT("_@@_"), wxT(" "));

                    genericProjectCfgDebug->cppCompilerOptions = projectCppCompiler;
                    genericProjectCfgRelease->cppCompilerOptions = projectCppCompiler;
                }

                if(tagName == wxT("Linker")) {
                    wxString projectLinker = tagValue;
                    projectLinker.Replace(wxT("_@@_"), wxT(" "));

                    genericProjectCfgDebug->linkerOptions = projectLinker;
                    genericProjectCfgRelease->linkerOptions = projectLinker;
                }
            }
        }

        if(line.Contains(wxT("[Unit"))) {
            GenericProjectFilePtr genericProjectFile = std::make_shared<GenericProjectFile>();

            while(!line.IsEmpty()) {
                line = tis.ReadLine();

                wxStringTokenizer part(line, wxT("="));
                wxString tagName = part.GetNextToken().Trim().Trim(false);
                wxString tagValue = part.GetNextToken().Trim().Trim(false);

                if(tagName == wxT("FileName")) {
                    wxString fileName = tagValue;
                    fileName.Replace(wxT("\\"), wxT("/"));
                    
                    genericProjectFile->name = fileName;
                }

                if(tagName == wxT("Folder")) {
                    wxString fileFolder = tagValue;
                    genericProjectFile->vpath = fileFolder;
                }
            }

            genericProject->files.push_back(genericProjectFile);
        }
    }

    return genericWorkspace;
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//VisualCppImporter
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
bool VisualCppImporter::OpenWordspace(const wxString& filename, const wxString& defaultCompiler)
{
    wsInfo.Assign(filename);

    extension = wsInfo.GetExt().Lower();

    bool isValidExt = extension == wxT("dsw") || extension == wxT("sln");
    bool result = wsInfo.FileExists() && wsInfo.IsFileReadable() && isValidExt;

    if(result) {
        version = 0;
        wxFileInputStream fis(filename);
        wxTextInputStream tis(fis);

        while(!fis.Eof()) {
            wxString line = tis.ReadLine();
            int index = line.Find(wxT("Format Version"));
            if(index != wxNOT_FOUND) {
                wxString value = line.Mid(index + 14).Trim().Trim(false);
                value.ToLong(&version);
                break;
            }
        }

        result = fis.IsOk() && version != 0;
    }

    return result;
}
//------------------------------------------------------------------------------------------------
bool VisualCppImporter::isSupportedWorkspace() { return version >= 5 && version <= 12; }
//------------------------------------------------------------------------------------------------
GenericWorkspacePtr VisualCppImporter::PerformImport()
{
    GenericWorkspacePtr genericWorkspace = std::make_shared<GenericWorkspace>();
    genericWorkspace->name = wsInfo.GetName();
    genericWorkspace->path = wsInfo.GetPath();

    switch(version) {
    case 5:
    case 6:
        GenerateFromVC6(genericWorkspace);
        break;
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
        GenerateFromVC7_11(genericWorkspace);
        break;
    }

    return genericWorkspace;
}
//------------------------------------------------------------------------------------------------
void VisualCppImporter::GenerateFromVC6(GenericWorkspacePtr genericWorkspace)
{
    GenericProjectDataListType genericProjectDataList;

    wxFileInputStream fis(wsInfo.GetFullPath());
    wxTextInputStream tis(fis);

    while(!fis.Eof()) {
        wxString line = tis.ReadLine();
        int index = line.Find(wxT("Project:"));
        if(index != wxNOT_FOUND) {
            int end = line.Find(wxT("- Package Owner")) - 1;
            wxString value = line.SubString(index + 8, end).Trim().Trim(false);
            value.Replace(wxT("\""), wxT(""));

            wxStringTokenizer part(value, wxT("="));

            GenericProjectDataType genericProjectData;
            genericProjectData[wxT("projectName")] = part.GetNextToken().Trim().Trim(false);
            genericProjectData[wxT("projectFile")] = part.GetNextToken().Trim().Trim(false);
            genericProjectData[wxT("projectFullPath")] =
                wsInfo.GetPath() + gsgsPathSep + genericProjectData[wxT("projectFile")];

            // Fix the paths to match linux style
            ConvertToLinuxStyle(genericProjectData["projectFile"]);
            ConvertToLinuxStyle(genericProjectData["projectFullPath"]);
            
            wxString deps = wxT("");
            while(!fis.Eof()) {
                line = tis.ReadLine();
                index = line.Find(wxT("Project_Dep_Name"));
                if(index != wxNOT_FOUND) {
                    wxString value = line.Mid(index + 16, end).Trim().Trim(false);
                    deps += value + wxT(";");
                }

                index = line.Find(wxT("##########"));
                if(index != wxNOT_FOUND) break;
            }

            genericProjectData[wxT("projectDeps")] = deps;

            genericProjectDataList.push_back(genericProjectData);
        }
    }

    for(GenericProjectDataType& genericProjectData : genericProjectDataList) {
        GenerateFromProjectVC6(genericWorkspace, genericProjectData);
    }
}
//------------------------------------------------------------------------------------------------
void VisualCppImporter::GenerateFromProjectVC6(GenericWorkspacePtr genericWorkspace,
                                               GenericProjectDataType& genericProjectData)
{
    wxFileName projectInfo(genericProjectData[wxT("projectFullPath")]);
    wxFileInputStream projectFIS(projectInfo.GetFullPath());

    if(projectFIS.IsOk()) {
        GenericProjectPtr genericProject = std::make_shared<GenericProject>();
        genericProject->name = genericProjectData[wxT("projectName")];
        genericProject->path = projectInfo.GetPath();

        wxStringTokenizer deps(genericProjectData[wxT("projectDeps")], wxT(";"));

        while(deps.HasMoreTokens()) {
            wxString projectNameDep = deps.GetNextToken().Trim().Trim(false);
            genericProject->deps.Add(projectNameDep);
        }

        std::map<wxString, GenericProjectCfgPtr> genericProjectCfgMap;

        wxTextInputStream projectTIS(projectFIS);

        while(!projectFIS.Eof()) {
            wxString line = projectTIS.ReadLine();

            int index = line.Find(wxT("TARGTYPE"));
            if(index != wxNOT_FOUND) {
                index = line.find_last_of(wxT("\""));
                wxString projectType = line.Mid(index + 1).Trim().Trim(false);

                if(projectType == wxT("0x0102")) {
                    genericProject->cfgType = GenericCfgType::DYNAMIC_LIBRARY;
                } else if(projectType == wxT("0x0104")) {
                    genericProject->cfgType = GenericCfgType::STATIC_LIBRARY;
                } else {
                    genericProject->cfgType = GenericCfgType::EXECUTABLE;
                }
            }

            index = line.Find(wxT("!MESSAGE \""));
            if(index != wxNOT_FOUND) {
                int begin = index + 10;
                int end = line.Find(wxT("\" (based on"));

                wxString projectCfgName = line.SubString(begin, end);
                projectCfgName.Replace(genericProjectData[wxT("projectName")] + wxT(" - "), wxT(""));
                projectCfgName.Replace(wxT("\""), wxT(""));
                projectCfgName.Replace(wxT(" "), wxT("_"));

                GenericProjectCfgPtr genericProjectCfg = std::make_shared<GenericProjectCfg>();
                genericProjectCfg->name = projectCfgName;
                genericProjectCfg->type = genericProject->cfgType;

                genericProjectCfgMap[projectCfgName] = genericProjectCfg;
            }

            index = line.Find(wxT("Begin Project"));
            if(index != wxNOT_FOUND) break;
        }

        wxString virtualPath = wxT("");
        GenericProjectCfgPtr genericProjectCfg;

        wxString line;

        while(!projectFIS.Eof()) {
            line = projectTIS.ReadLine();

            int index = line.Find(wxT("\"$(CFG)\" == "));
            if(index != wxNOT_FOUND) {
                wxString projectCfgName = line.Mid(index + 12).Trim().Trim(false);
                projectCfgName.Replace(genericProjectData[wxT("projectName")] + wxT(" - "), wxT(""));
                projectCfgName.Replace(wxT("\""), wxT(""));
                projectCfgName.Replace(wxT(" "), wxT("_"));

                genericProjectCfg = genericProjectCfgMap[projectCfgName];
            }

            if(genericProjectCfg) {
                index = line.Find(wxT("PROP Intermediate_Dir"));
                if(index != wxNOT_FOUND) {
                    wxString intermediateDirectory = line.Mid(index + 21).Trim().Trim(false);
                    intermediateDirectory.Replace(wxT("\\"), wxT("/"));
                    intermediateDirectory.Replace(wxT("\""), wxT(""));
                    intermediateDirectory.Replace(wxT(" "), wxT("_"));

                    genericProjectCfg->intermediateDirectory = wxT("./") + intermediateDirectory;
                }

                index = line.Find(wxT("ADD CPP"));
                if(index != wxNOT_FOUND) {
                    line = line.Mid(index + 7).Trim().Trim(false);
                    wxStringTokenizer options(line, wxT(" "));

                    while(options.HasMoreTokens()) {
                        wxString value = options.GetNextToken();

                        if(value.Contains(wxT("/I"))) {
                            wxString includePath = options.GetNextToken().Trim().Trim(false);
                            includePath.Replace(wxT("\""), wxT(""));

                            genericProjectCfg->includePath += includePath + wxT(";");
                        } else if(value.Contains(wxT("/D"))) {
                            wxString preprocessor = options.GetNextToken().Trim().Trim(false);
                            preprocessor.Replace(wxT("\""), wxT(""));

                            genericProjectCfg->preprocessor += preprocessor + wxT(";");
                        } else if(value.Contains(wxT("/Yu"))) {
                            wxString preCompiledHeader = value.Trim().Trim(false);
                            preCompiledHeader.Replace(wxT("/Yu"), wxT(""));
                            preCompiledHeader.Replace(wxT("\""), wxT(""));

                            genericProjectCfg->preCompiledHeader += preCompiledHeader + wxT(";");
                        }
                    }
                }

                index = line.Find(wxT("ADD LIB32"));
                if(index != wxNOT_FOUND) {
                    line = line.Mid(index + 9).Trim().Trim(false);
                    wxStringTokenizer options(line, wxT(" "));

                    while(options.HasMoreTokens()) {
                        wxString value = options.GetNextToken();

                        if(value.Contains(wxT("/out:"))) {
                            wxString outputFilename = value.Trim().Trim(false);
                            outputFilename.Replace(wxT("/out:"), wxT(""));
                            outputFilename.Replace(wxT("\""), wxT(""));
                            genericProjectCfg->outputFilename = outputFilename;

                            wxFileName outputFilenameInfo(outputFilename);
                            genericProjectCfg->command = wxT("./") + outputFilenameInfo.GetFullName();
                        }
                    }
                }

                index = line.Find(wxT("ADD LINK32"));
                if(index != wxNOT_FOUND) {
                    line = line.Mid(index + 10).Trim().Trim(false);
                    wxStringTokenizer options(line, wxT(" "));

                    while(options.HasMoreTokens()) {
                        wxString value = options.GetNextToken();

                        if(value.Contains(wxT(".lib"))) {
                            wxString lib = value.Trim().Trim(false);
                            genericProjectCfg->libraries += lib + wxT(";");
                        } else if(value.Contains(wxT("/libpath:"))) {
                            wxString libPath = value.Trim().Trim(false);
                            libPath.Replace(wxT("/libpath:"), wxT(""));
                            libPath.Replace(wxT("\""), wxT(""));
                            genericProjectCfg->libPath += libPath + wxT(";");
                        } else if(value.Contains(wxT("/out:"))) {
                            wxString outputFilename = value.Trim().Trim(false);
                            outputFilename.Replace(wxT("/out:"), wxT(""));
                            outputFilename.Replace(wxT("\""), wxT(""));
                            genericProjectCfg->outputFilename = outputFilename;

                            wxFileName outputFilenameInfo(outputFilename);
                            genericProjectCfg->command = wxT("./") + outputFilenameInfo.GetFullName();
                        }
                    }
                }

                index = line.Find(wxT("PROP Cmd_Line"));
                if(index != wxNOT_FOUND) {
                    wxString cmdLine = line.Mid(index + 13).Trim().Trim(false);
                    cmdLine.Replace(wxT("\""), wxT(""));

                    genericProjectCfg->enableCustomBuild = true;
                    genericProjectCfg->customBuildCmd = cmdLine;
                }

                index = line.Find(wxT("Begin Group"));
                if(index != wxNOT_FOUND) {
                    wxString vpName = line.Mid(index + 11).Trim().Trim(false);
                    vpName.Replace(wxT("\""), wxT(""));

                    if(virtualPath.IsEmpty()) {
                        virtualPath = vpName;
                    } else {
                        virtualPath += wxT("/") + vpName;
                    }
                }

                index = line.Find(wxT("End Group"));
                if(index != wxNOT_FOUND) {
                    int posSeparator = virtualPath.Find(wxT("/"));

                    if(posSeparator == wxNOT_FOUND) {
                        virtualPath = wxT("");
                    } else {
                        virtualPath = virtualPath.Remove(posSeparator);
                    }
                }

                index = line.Find(wxT("Begin Source File"));
                if(index != wxNOT_FOUND) {
                    GenericProjectFilePtr genericProjectFile;

                    while(!projectFIS.Eof()) {
                        line = projectTIS.ReadLine();

                        int index = line.Find(wxT("\"$(CFG)\" == "));
                        if(index != wxNOT_FOUND) {
                            wxString projectCfgName = line.Mid(index + 12).Trim().Trim(false);
                            projectCfgName.Replace(genericProjectData[wxT("projectName")] + wxT(" - "), wxT(""));
                            projectCfgName.Replace(wxT("\""), wxT(""));
                            projectCfgName.Replace(wxT(" "), wxT("_"));

                            genericProjectCfg = genericProjectCfgMap[projectCfgName];
                        }

                        index = line.Find(wxT("SOURCE="));
                        if(index != wxNOT_FOUND) {
                            wxString filename = line.Mid(index + 7).Trim().Trim(false);
                            filename.Replace(wxT("\""), wxT(""));
                            filename.Replace(wxT("\\"), wxT("/"));

                            genericProjectFile = std::make_shared<GenericProjectFile>();
                            genericProjectFile->name = filename;
                            genericProjectFile->vpath = virtualPath;

                            genericProject->files.push_back(genericProjectFile);
                        }

                        index = line.Find(wxT("PROP Exclude_From_Build"));
                        if(index != wxNOT_FOUND) {
                            wxString excludeValue = line.Mid(index + 23).Trim().Trim(false);
                            if(genericProjectFile && excludeValue == wxT("1")) {
                                genericProjectCfg->excludeFiles.push_back(genericProjectFile);
                            }
                        }

                        index = line.Find(wxT("Begin Custom Build"));
                        if(index != wxNOT_FOUND) {
                            while(!projectFIS.Eof()) {
                                line = projectTIS.ReadLine();

                                index = line.Find(wxT("="));
                                if(index != wxNOT_FOUND) {
                                    wxStringTokenizer keyValue(line, wxT("="));
                                    wxString key = keyValue.GetNextToken().Trim().Trim(false);
                                    wxString value = keyValue.GetNextToken().Trim().Trim(false);
                                    genericProjectCfg->envVars[key] = value;
                                }

                                index = line.Find(wxT("\t"));
                                if(index != wxNOT_FOUND) {
                                    wxString command = line;
                                    command.Replace(wxT("\t"), wxT(""));

                                    if(!command.IsEmpty()) { genericProjectCfg->preBuildCommands.push_back(command); }
                                }

                                index = line.Find(wxT("End Custom Build"));
                                if(index != wxNOT_FOUND) break;
                            }
                        }

                        index = line.Find(wxT("End Source File"));
                        if(index != wxNOT_FOUND) break;
                    }
                }
            }
        }

        for(std::pair<wxString, GenericProjectCfgPtr> genericProjectCfg : genericProjectCfgMap) {
            if(genericProjectCfg.second) { genericProject->cfgs.push_back(genericProjectCfg.second); }
        }

        genericWorkspace->projects.push_back(genericProject);
    }
}
//------------------------------------------------------------------------------------------------
void VisualCppImporter::GenerateFromVC7_11(GenericWorkspacePtr genericWorkspace)
{
    GenericProjectDataListType genericProjectDataList;

    wxFileInputStream fis(wsInfo.GetFullPath());
    wxTextInputStream tis(fis);

    while(!fis.Eof()) {
        wxString line = tis.ReadLine();

        int index = line.Find(wxT("Project("));
        if(index != wxNOT_FOUND) {
            wxStringTokenizer projectToken(line, wxT("="));
            projectToken.GetNextToken();

            projectToken.SetString(projectToken.GetNextToken(), wxT(","));

            wxString projectName = projectToken.GetNextToken().Trim().Trim(false);
            projectName.Replace(wxT("\""), wxT(""));
            wxString projectFile = projectToken.GetNextToken().Trim().Trim(false);
            projectFile.Replace(wxT("\""), wxT(""));
            wxString projectId = projectToken.GetNextToken().Trim().Trim(false);
            projectId.Replace(wxT("\""), wxT(""));

            GenericProjectDataType genericProjectData;
            genericProjectData[wxT("projectId")] = projectId;
            genericProjectData[wxT("projectName")] = projectName;
            genericProjectData[wxT("projectFile")] = projectFile;
            genericProjectData[wxT("projectFullPath")] =
                wsInfo.GetPath() + gsgsPathSep + projectFile;

            wxString deps = wxT("");

            while(!fis.Eof()) {
                line = tis.ReadLine();

                index = line.Find(wxT("ProjectSection(ProjectDependencies)"));
                if(index != wxNOT_FOUND) {
                    while(!fis.Eof()) {
                        line = tis.ReadLine();

                        index = line.Find(wxT("="));
                        if(index != wxNOT_FOUND) {
                            wxStringTokenizer values(line, wxT("="));
                            if(values.HasMoreTokens()) {
                                wxString depId = values.NextToken().Trim().Trim(false);
                                depId.Replace(wxT("\""), wxT(""));
                                deps += depId + wxT(";");
                            }
                        }

                        index = line.Find(wxT("EndProjectSection"));
                        if(index != wxNOT_FOUND) break;
                    }
                }

                index = line.Find(wxT("EndProject"));
                if(index != wxNOT_FOUND) break;
            }

            genericProjectData[wxT("projectDeps")] = deps;

            genericProjectDataList.push_back(genericProjectData);
        }
    }

    for(GenericProjectDataType& genericProjectData : genericProjectDataList) {
        for(GenericProjectDataType& genericProjectDataSub : genericProjectDataList) {
            if(genericProjectData[wxT("projectDeps")].Contains(genericProjectDataSub[wxT("projectId")])) {
                genericProjectData[wxT("projectDeps")].Replace(genericProjectDataSub[wxT("projectId")],
                                                               genericProjectDataSub[wxT("projectName")]);
            }
        }
    }

    for(GenericProjectDataType& genericProjectData : genericProjectDataList) {
        if(version >= 7 && version <= 10)
            GenerateFromProjectVC7(genericWorkspace, genericProjectData);
        else if(version >= 11 && version <= 12)
            GenerateFromProjectVC11(genericWorkspace, genericProjectData);
    }
}
//------------------------------------------------------------------------------------------------
void VisualCppImporter::GenerateFromProjectVC7(GenericWorkspacePtr genericWorkspace,
                                               GenericProjectDataType& genericProjectData)
{
    wxFileName projectInfo(genericProjectData[wxT("projectFullPath")]);

    GenericProjectPtr genericProject = std::make_shared<GenericProject>();
    genericProject->name = genericProjectData[wxT("projectName")];
    genericProject->path = projectInfo.GetPath();

    wxStringTokenizer deps(genericProjectData[wxT("projectDeps")], wxT(";"));

    while(deps.HasMoreTokens()) {
        wxString projectNameDep = deps.GetNextToken().Trim().Trim(false);
        genericProject->deps.Add(projectNameDep);
    }

    wxXmlDocument projectDoc;
    if(projectDoc.Load(projectInfo.GetFullPath())) {
        wxXmlNode* root = projectDoc.GetRoot();

        wxXmlNode* vspChild = root->GetChildren();
        std::map<wxString, GenericProjectCfgPtr> genericProjectCfgMap;
        while(vspChild) {
            if(vspChild->GetName() == wxT("Configurations")) {
                wxXmlNode* confChild = vspChild->GetChildren();

                while(confChild) {
                    if(confChild->GetName() == wxT("Configuration")) {
                        wxString name = confChild->GetAttribute(wxT("Name"));
                        wxString projectCfgName = name;
                        projectCfgName.Replace(wxT("|"), wxT("_"));

                        wxStringTokenizer projectCfgNamePart(name, wxT("|"));
                        wxString configurationName = projectCfgNamePart.NextToken();
                        configurationName.Replace(wxT(" "), wxT("_"));
                        wxString platformName = projectCfgNamePart.NextToken();

                        wxString configurationType = confChild->GetAttribute(wxT("ConfigurationType"));
                        wxString outputDirectory = confChild->GetAttribute(wxT("OutputDirectory"));
                        outputDirectory.Replace(wxT("\\"), wxT("/"));
                        outputDirectory = ReplaceDefaultEnvVars(outputDirectory);
                        wxString intermediateDirectory = confChild->GetAttribute(wxT("IntermediateDirectory"));
                        intermediateDirectory.Replace(wxT("\\"), wxT("/"));
                        intermediateDirectory = ReplaceDefaultEnvVars(intermediateDirectory);
                        wxString solutionDir = genericWorkspace->path + wxT("/");
                        solutionDir.Replace(wxT("\\"), wxT("/"));
                        wxString projectDir = genericProject->path + wxT("/");
                        projectDir.Replace(wxT("\\"), wxT("/"));

                        GenericProjectCfgPtr genericProjectCfg = std::make_shared<GenericProjectCfg>();
                        genericProjectCfg->name = projectCfgName;

                        genericProjectCfg->envVars[wxT("VS_ConfigurationName")] = configurationName;
                        genericProjectCfg->envVars[wxT("VS_PlatformName")] = platformName;
                        genericProjectCfg->envVars[wxT("VS_OutDir")] = outputDirectory + wxT("/");
                        genericProjectCfg->envVars[wxT("VS_IntDir")] = intermediateDirectory + wxT("/");
                        genericProjectCfg->envVars[wxT("VS_SolutionDir")] = solutionDir;
                        genericProjectCfg->envVars[wxT("VS_ProjectName")] = genericProject->name;
                        genericProjectCfg->envVars[wxT("VS_ProjectDir")] = projectDir;

                        if(!intermediateDirectory.IsEmpty()) {
                            genericProjectCfg->intermediateDirectory = intermediateDirectory;
                        } else {
                            genericProjectCfg->intermediateDirectory = wxT("./") + configurationName;
                        }

                        genericProjectCfg->intermediateDirectory = intermediateDirectory;

                        if(configurationType == wxT("4")) {
                            genericProjectCfg->type = GenericCfgType::STATIC_LIBRARY;
                        } else if(configurationType == wxT("2")) {
                            genericProjectCfg->type = GenericCfgType::DYNAMIC_LIBRARY;
                        } else {
                            genericProjectCfg->type = GenericCfgType::EXECUTABLE;
                        }

                        wxXmlNode* toolChild = confChild->GetChildren();

                        while(toolChild) {
                            if(toolChild->GetName() == wxT("Tool")) {
                                if(toolChild->GetAttribute(wxT("Name")) == wxT("VCCLCompilerTool")) {
                                    wxString preprocessorDefinitions =
                                        toolChild->GetAttribute(wxT("PreprocessorDefinitions"));
                                    preprocessorDefinitions.Replace(wxT(","), wxT(";"));
                                    preprocessorDefinitions = ReplaceDefaultEnvVars(preprocessorDefinitions);
                                    genericProjectCfg->preprocessor = preprocessorDefinitions;

                                    wxString additionalIncludeDirectories =
                                        toolChild->GetAttribute(wxT("AdditionalIncludeDirectories"));
                                    additionalIncludeDirectories.Replace(wxT(","), wxT(";"));
                                    additionalIncludeDirectories.Replace(wxT("\\"), wxT("/"));
                                    additionalIncludeDirectories = ReplaceDefaultEnvVars(additionalIncludeDirectories);
                                    genericProjectCfg->includePath = additionalIncludeDirectories;

                                    if(toolChild->GetAttribute(wxT("UsePrecompiledHeader")) == wxT("3")) {
                                        wxString precompiledHeaderThrough =
                                            toolChild->GetAttribute(wxT("PrecompiledHeaderThrough"));
                                        genericProjectCfg->preCompiledHeader = precompiledHeaderThrough;
                                    }
                                }

                                if(toolChild->GetAttribute(wxT("Name")) == wxT("VCLinkerTool")) {
                                    if(toolChild->HasAttribute(wxT("AdditionalDependencies"))) {
                                        wxString additionalDependencies =
                                            toolChild->GetAttribute(wxT("AdditionalDependencies"));
                                        additionalDependencies.Replace(wxT(" "), wxT(";"));
                                        additionalDependencies = ReplaceDefaultEnvVars(additionalDependencies);
                                        genericProjectCfg->libraries = additionalDependencies;
                                    }

                                    if(toolChild->HasAttribute(wxT("AdditionalLibraryDirectories"))) {
                                        wxString additionalLibraryDirectories =
                                            toolChild->GetAttribute(wxT("AdditionalLibraryDirectories"));
                                        additionalLibraryDirectories.Replace(wxT(","), wxT(";"));
                                        additionalLibraryDirectories.Replace(wxT("\\"), wxT("/"));
                                        additionalLibraryDirectories =
                                            ReplaceDefaultEnvVars(additionalLibraryDirectories);
                                        genericProjectCfg->libPath = additionalLibraryDirectories;
                                    }

                                    if(toolChild->HasAttribute(wxT("OutputFile"))) {
                                        wxString outputFile = toolChild->GetAttribute(wxT("OutputFile"));
                                        outputFile.Replace(wxT("\\"), wxT("/"));
                                        outputFile.Replace(wxT(" "), wxT("_"));
                                        outputFile = ReplaceDefaultEnvVars(outputFile);
                                        genericProjectCfg->outputFilename = outputFile;

                                        wxFileName outputFilenameInfo(outputFile);
                                        genericProjectCfg->command = wxT("./") + outputFilenameInfo.GetFullName();
                                    }
                                }

                                if(toolChild->GetAttribute(wxT("Name")) == wxT("VCLibrarianTool")) {
                                    if(toolChild->HasAttribute(wxT("OutputFile"))) {
                                        wxString outputFile = toolChild->GetAttribute(wxT("OutputFile"));
                                        outputFile.Replace(wxT("\\"), wxT("/"));
                                        outputFile.Replace(wxT(" "), wxT("_"));
                                        outputFile = ReplaceDefaultEnvVars(outputFile);
                                        genericProjectCfg->outputFilename = outputFile;

                                        wxFileName outputFilenameInfo(outputFile);
                                        genericProjectCfg->command = wxT("./") + outputFilenameInfo.GetFullName();
                                    }
                                }

                                if(toolChild->GetAttribute(wxT("Name")) == wxT("VCPreBuildEventTool")) {
                                    if(toolChild->HasAttribute(wxT("CommandLine"))) {
                                        wxString commandLine = toolChild->GetAttribute(wxT("CommandLine"));
                                        if(!commandLine.IsEmpty()) {
                                            genericProjectCfg->preBuildCommands.push_back(commandLine);
                                        }
                                    }
                                }

                                if(toolChild->GetAttribute(wxT("Name")) == wxT("VCPostBuildEventTool")) {
                                    if(toolChild->HasAttribute(wxT("CommandLine"))) {
                                        wxString commandLine = toolChild->GetAttribute(wxT("CommandLine"));
                                        if(!commandLine.IsEmpty()) {
                                            genericProjectCfg->postBuildCommands.push_back(commandLine);
                                        }
                                    }
                                }

                                if(toolChild->GetAttribute(wxT("Name")) == wxT("VCNMakeTool")) {
                                    if(toolChild->HasAttribute(wxT("BuildCommandLine"))) {
                                        wxString buildCommandLine = toolChild->GetAttribute(wxT("BuildCommandLine"));
                                        if(!buildCommandLine.IsEmpty()) {
                                            if(!genericProjectCfg->enableCustomBuild)
                                                genericProjectCfg->enableCustomBuild = true;

                                            genericProjectCfg->customBuildCmd = buildCommandLine;
                                        }
                                    }

                                    if(toolChild->HasAttribute(wxT("ReBuildCommandLine"))) {
                                        wxString reBuildCommandLine =
                                            toolChild->GetAttribute(wxT("ReBuildCommandLine"));
                                        if(!reBuildCommandLine.IsEmpty()) {
                                            if(!genericProjectCfg->enableCustomBuild)
                                                genericProjectCfg->enableCustomBuild = true;
                                            genericProjectCfg->customRebuildCmd = reBuildCommandLine;
                                        }
                                    }

                                    if(toolChild->HasAttribute(wxT("CleanCommandLine"))) {
                                        wxString cleanCommandLine = toolChild->GetAttribute(wxT("CleanCommandLine"));
                                        if(!cleanCommandLine.IsEmpty()) {
                                            if(!genericProjectCfg->enableCustomBuild)
                                                genericProjectCfg->enableCustomBuild = true;
                                            genericProjectCfg->customCleanCmd = cleanCommandLine;
                                        }
                                    }
                                }
                            }

                            toolChild = toolChild->GetNext();
                        }

                        if(genericProject->cfgType != genericProjectCfg->type)
                            genericProject->cfgType = genericProjectCfg->type;

                        genericProjectCfgMap[projectCfgName] = genericProjectCfg;
                        genericProject->cfgs.push_back(genericProjectCfg);
                    }

                    confChild = confChild->GetNext();
                }
            }

            if(vspChild->GetName() == wxT("Files")) {
                wxXmlNode* filterChild = vspChild->GetChildren();
                AddFilesVC7(filterChild, genericProject, genericProjectCfgMap, wxT(""));
            }

            vspChild = vspChild->GetNext();
        }

        genericWorkspace->projects.push_back(genericProject);
    }
}
//------------------------------------------------------------------------------------------------
void VisualCppImporter::GenerateFromProjectVC11(GenericWorkspacePtr genericWorkspace,
                                                GenericProjectDataType& genericProjectData)
{
    wxFileName projectInfo(genericProjectData[wxT("projectFullPath")]);
    wxFileName filterInfo(projectInfo.GetFullPath() + wxT(".filters"));

    GenericProjectPtr genericProject = std::make_shared<GenericProject>();
    genericProject->name = genericProjectData[wxT("projectName")];
    genericProject->path = projectInfo.GetPath();

    wxStringTokenizer deps(genericProjectData[wxT("projectDeps")], wxT(";"));

    while(deps.HasMoreTokens()) {
        wxString projectNameDep = deps.GetNextToken().Trim().Trim(false);
        genericProject->deps.Add(projectNameDep);
    }

    std::map<wxString, GenericProjectCfgPtr> genericProjectCfgMap;

    wxXmlDocument projectDoc;
    if(projectDoc.Load(projectInfo.GetFullPath())) {
        wxXmlNode* root = projectDoc.GetRoot();
        wxXmlNode* projectChild = root->GetChildren();

        while(projectChild) {
            if(projectChild->GetName() == wxT("ItemGroup") && projectChild->HasAttribute(wxT("Label")) &&
               projectChild->GetAttribute(wxT("Label")) == wxT("ProjectConfigurations")) {
                wxXmlNode* projectConfiguration = projectChild->GetChildren();

                while(projectConfiguration && projectConfiguration->HasAttribute(wxT("Include"))) {
                    wxString projectCfgKey = projectConfiguration->GetAttribute(wxT("Include"));
                    wxString projectCfgName = projectCfgKey;
                    projectCfgName.Replace(wxT("|"), wxT("_"));

                    wxStringTokenizer projectCfgNamePart(projectCfgKey, wxT("|"));
                    wxString configurationName = projectCfgNamePart.NextToken();
                    configurationName.Replace(wxT(" "), wxT("_"));
                    wxString platformName = projectCfgNamePart.NextToken();
                    wxString intermediateDirectory = wxT("./") + configurationName;
                    wxString solutionDir = genericWorkspace->path + wxT("/");
                    solutionDir.Replace(wxT("\\"), wxT("/"));
                    wxString projectDir = genericProject->path + wxT("/");
                    projectDir.Replace(wxT("\\"), wxT("/"));

                    GenericProjectCfgPtr genericProjectCfg = std::make_shared<GenericProjectCfg>();
                    genericProjectCfg->name = projectCfgName;

                    genericProjectCfg->intermediateDirectory = intermediateDirectory;
                    genericProjectCfg->envVars[wxT("VS_Configuration")] = configurationName;
                    genericProjectCfg->envVars[wxT("VS_Platform")] = platformName;
                    genericProjectCfg->envVars[wxT("VS_IntDir")] = intermediateDirectory + wxT("/");
                    genericProjectCfg->envVars[wxT("VS_OutDir")] = intermediateDirectory + wxT("/");
                    genericProjectCfg->envVars[wxT("VS_SolutionDir")] = solutionDir;
                    genericProjectCfg->envVars[wxT("VS_ProjectName")] = genericProject->name;
                    genericProjectCfg->envVars[wxT("VS_ProjectDir")] = projectDir;

                    genericProjectCfgMap[projectCfgKey] = genericProjectCfg;

                    projectConfiguration = projectConfiguration->GetNext();
                }
            }

            if(projectChild->GetName() == wxT("ItemGroup")) {
                wxXmlNode* itemGroupChild = projectChild->GetChildren();

                while(itemGroupChild) {
                    if(itemGroupChild->GetName() == wxT("CustomBuild")) {
                        wxXmlNode* customBuildChild = itemGroupChild->GetChildren();

                        while(customBuildChild) {
                            if(customBuildChild->GetName() == wxT("Command") &&
                               customBuildChild->HasAttribute(wxT("Condition"))) {
                                wxString elemCondition = customBuildChild->GetAttribute(wxT("Condition"));
                                wxString commandLine = customBuildChild->GetNodeContent();
                                wxString projectCfgKey = ExtractProjectCfgName(wxT(""), elemCondition);

                                GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];

                                if(genericProjectCfg && !commandLine.IsEmpty()) {
                                    genericProjectCfg->preBuildCommands.push_back(commandLine);
                                }
                            }

                            customBuildChild = customBuildChild->GetNext();
                        }
                    }

                    if(itemGroupChild->GetName() == wxT("ClInclude") || itemGroupChild->GetName() == wxT("ClCompile") ||
                       itemGroupChild->GetName() == wxT("None") || itemGroupChild->GetName() == wxT("Text") ||
                       itemGroupChild->GetName() == wxT("ResourceCompile") ||
                       itemGroupChild->GetName() == wxT("Image") || itemGroupChild->GetName() == wxT("CustomBuild")) {
                        AddFilesVC11(itemGroupChild, genericProject, genericProjectCfgMap);
                    }

                    itemGroupChild = itemGroupChild->GetNext();
                }
            }

            if(projectChild->GetName() == wxT("PropertyGroup")) {
                wxXmlNode* propertyGroupChild = projectChild->GetChildren();

                while(propertyGroupChild) {
                    wxString parentCondition = projectChild->GetAttribute(wxT("Condition"));

                    if(propertyGroupChild->GetName() == wxT("ConfigurationType")) {
                        wxString elemCondition = propertyGroupChild->GetAttribute(wxT("Condition"));
                        wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                        wxString configurationType = propertyGroupChild->GetNodeContent();

                        wxString outputFilename;

                        GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];

                        if(genericProjectCfg) {
                            if(configurationType == wxT("StaticLibrary")) {
                                genericProjectCfg->type = GenericCfgType::STATIC_LIBRARY;
                            } else if(configurationType == wxT("DynamicLibrary")) {
                                genericProjectCfg->type = GenericCfgType::DYNAMIC_LIBRARY;
                            } else {
                                genericProjectCfg->type = GenericCfgType::EXECUTABLE;
                            }

                            if(genericProject->cfgType != genericProjectCfg->type)
                                genericProject->cfgType = genericProjectCfg->type;
                        }
                    }

                    if(propertyGroupChild->GetName() == wxT("IntDir")) {
                        wxString elemCondition = propertyGroupChild->GetAttribute(wxT("Condition"));
                        wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                        wxString intermediateDirectory = propertyGroupChild->GetNodeContent();
                        intermediateDirectory.Replace(wxT("\\"), wxT("/"));
                        intermediateDirectory = ReplaceDefaultEnvVars(intermediateDirectory);

                        GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                        if(genericProjectCfg) {
                            genericProjectCfg->intermediateDirectory =
                                intermediateDirectory.SubString(0, intermediateDirectory.Length() - 1);
                            genericProjectCfg->envVars[wxT("VS_IntDir")] = intermediateDirectory;
                        }
                    }

                    if(propertyGroupChild->GetName() == wxT("OutDir")) {
                        wxString elemCondition = propertyGroupChild->GetAttribute(wxT("Condition"));
                        wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                        wxString outDir = propertyGroupChild->GetNodeContent();
                        outDir.Replace(wxT("\\"), wxT("/"));
                        outDir = ReplaceDefaultEnvVars(outDir);

                        GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                        if(genericProjectCfg) { genericProjectCfg->envVars[wxT("VS_OutDir")] = outDir; }
                    }

                    if(propertyGroupChild->GetName() == wxT("NMakeBuildCommandLine")) {
                        wxString elemCondition = propertyGroupChild->GetAttribute(wxT("Condition"));
                        wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                        wxString nMakeBuildCommandLine = propertyGroupChild->GetNodeContent();
                        nMakeBuildCommandLine = ReplaceDefaultEnvVars(nMakeBuildCommandLine);

                        GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                        if(genericProjectCfg && !nMakeBuildCommandLine.IsEmpty()) {
                            if(!genericProjectCfg->enableCustomBuild) genericProjectCfg->enableCustomBuild = true;

                            genericProjectCfg->customBuildCmd = nMakeBuildCommandLine;
                        }
                    }

                    if(propertyGroupChild->GetName() == wxT("NMakeReBuildCommandLine")) {
                        wxString elemCondition = propertyGroupChild->GetAttribute(wxT("Condition"));
                        wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                        wxString nMakeReBuildCommandLine = propertyGroupChild->GetNodeContent();
                        nMakeReBuildCommandLine = ReplaceDefaultEnvVars(nMakeReBuildCommandLine);

                        GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                        if(genericProjectCfg && !nMakeReBuildCommandLine.IsEmpty()) {
                            if(!genericProjectCfg->enableCustomBuild) genericProjectCfg->enableCustomBuild = true;

                            genericProjectCfg->customRebuildCmd = nMakeReBuildCommandLine;
                        }
                    }

                    if(propertyGroupChild->GetName() == wxT("NMakeCleanCommandLine")) {
                        wxString elemCondition = propertyGroupChild->GetAttribute(wxT("Condition"));
                        wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                        wxString nMakeCleanCommandLine = propertyGroupChild->GetNodeContent();
                        nMakeCleanCommandLine = ReplaceDefaultEnvVars(nMakeCleanCommandLine);

                        GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                        if(genericProjectCfg && !nMakeCleanCommandLine.IsEmpty()) {
                            if(!genericProjectCfg->enableCustomBuild) genericProjectCfg->enableCustomBuild = true;

                            genericProjectCfg->customCleanCmd = nMakeCleanCommandLine;
                        }
                    }

                    propertyGroupChild = propertyGroupChild->GetNext();
                }
            }

            if(projectChild->GetName() == wxT("ItemDefinitionGroup")) {
                wxString parentCondition = projectChild->GetAttribute(wxT("Condition"));

                wxXmlNode* itemDefinitionGroupChild = projectChild->GetChildren();

                while(itemDefinitionGroupChild) {
                    if(itemDefinitionGroupChild->GetName() == wxT("ClCompile")) {
                        wxXmlNode* clcompileChild = itemDefinitionGroupChild->GetChildren();

                        while(clcompileChild) {
                            if(clcompileChild->GetName() == wxT("PreprocessorDefinitions")) {
                                wxString elemCondition = clcompileChild->GetAttribute(wxT("Condition"));
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString preprocessorDefinitions = clcompileChild->GetNodeContent();
                                preprocessorDefinitions.Replace(wxT("%(PreprocessorDefinitions)"), wxT(""));
                                preprocessorDefinitions = ReplaceDefaultEnvVars(preprocessorDefinitions);

                                GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                                if(genericProjectCfg) { genericProjectCfg->preprocessor = preprocessorDefinitions; }
                            }

                            if(clcompileChild->GetName() == wxT("PrecompiledHeaderFile")) {
                                wxString elemCondition = clcompileChild->GetAttribute(wxT("Condition"));
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString precompiledHeaderFile = clcompileChild->GetNodeContent();

                                GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                                if(genericProjectCfg) { genericProjectCfg->preCompiledHeader = precompiledHeaderFile; }
                            }

                            if(clcompileChild->GetName() == wxT("AdditionalIncludeDirectories")) {
                                wxString elemCondition = clcompileChild->GetAttribute(wxT("Condition"));
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString additionalIncludeDirectories = clcompileChild->GetNodeContent();
                                additionalIncludeDirectories.Replace(wxT("%(AdditionalIncludeDirectories)"), wxT(""));
                                additionalIncludeDirectories.Replace(wxT("\\"), wxT("/"));
                                additionalIncludeDirectories = ReplaceDefaultEnvVars(additionalIncludeDirectories);

                                GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                                if(genericProjectCfg) { genericProjectCfg->includePath = additionalIncludeDirectories; }
                            }

                            clcompileChild = clcompileChild->GetNext();
                        }
                    }

                    if(itemDefinitionGroupChild->GetName() == wxT("Link")) {
                        wxXmlNode* linkChild = itemDefinitionGroupChild->GetChildren();

                        while(linkChild) {
                            if(linkChild->GetName() == wxT("AdditionalDependencies")) {
                                wxString elemCondition = linkChild->GetAttribute(wxT("Condition"));
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString additionalDependencies = linkChild->GetNodeContent();
                                additionalDependencies.Replace(wxT("%(AdditionalDependencies)"), wxT(""));
                                additionalDependencies = ReplaceDefaultEnvVars(additionalDependencies);

                                GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                                if(genericProjectCfg) { genericProjectCfg->libraries = additionalDependencies; }
                            }

                            if(linkChild->GetName() == wxT("AdditionalLibraryDirectories")) {
                                wxString elemCondition = linkChild->GetAttribute(wxT("Condition"));
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString additionalLibraryDirectories = linkChild->GetNodeContent();
                                additionalLibraryDirectories.Replace(wxT("%(AdditionalLibraryDirectories)"), wxT(""));
                                additionalLibraryDirectories = ReplaceDefaultEnvVars(additionalLibraryDirectories);

                                GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                                if(genericProjectCfg) { genericProjectCfg->libPath = additionalLibraryDirectories; }
                            }

                            if(linkChild->GetName() == wxT("OutputFile")) {
                                wxString elemCondition = linkChild->GetAttribute(wxT("Condition"));
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString outputFile = linkChild->GetNodeContent();
                                outputFile.Replace(wxT("\\"), wxT("/"));
                                outputFile.Replace(wxT(" "), wxT("_"));
                                outputFile = ReplaceDefaultEnvVars(outputFile);

                                wxFileName outputFilenameInfo(outputFile);

                                GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                                if(genericProjectCfg) {
                                    genericProjectCfg->outputFilename = outputFile;
                                    genericProjectCfg->command = wxT("./") + outputFilenameInfo.GetFullName();
                                }
                            }

                            linkChild = linkChild->GetNext();
                        }
                    }

                    if(itemDefinitionGroupChild->GetName() == wxT("Lib")) {
                        wxXmlNode* LibChild = itemDefinitionGroupChild->GetChildren();

                        while(LibChild) {
                            if(LibChild->GetName() == wxT("OutputFile")) {
                                wxString elemCondition = LibChild->GetAttribute(wxT("Condition"));
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString outputFile = LibChild->GetNodeContent();
                                outputFile.Replace(wxT("\\"), wxT("/"));
                                outputFile.Replace(wxT(" "), wxT("_"));
                                outputFile = ReplaceDefaultEnvVars(outputFile);

                                wxFileName outputFilenameInfo(outputFile);

                                GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                                if(genericProjectCfg) {
                                    genericProjectCfg->outputFilename = outputFile;
                                    genericProjectCfg->command = wxT("./") + outputFilenameInfo.GetFullName();
                                }
                            }

                            LibChild = LibChild->GetNext();
                        }
                    }

                    if(itemDefinitionGroupChild->GetName() == wxT("PreBuildEvent")) {
                        wxXmlNode* preBuildEventChild = itemDefinitionGroupChild->GetChildren();

                        while(preBuildEventChild) {
                            if(preBuildEventChild->GetName() == wxT("Command")) {
                                wxString elemCondition = preBuildEventChild->GetAttribute(wxT("Condition"));
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString command = preBuildEventChild->GetNodeContent();
                                if(!command.IsEmpty()) {
                                    GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                                    if(genericProjectCfg) { genericProjectCfg->preBuildCommands.push_back(command); }
                                }
                            }

                            preBuildEventChild = preBuildEventChild->GetNext();
                        }
                    }

                    if(itemDefinitionGroupChild->GetName() == wxT("PostBuildEvent")) {
                        wxXmlNode* postBuildEventChild = itemDefinitionGroupChild->GetChildren();

                        while(postBuildEventChild) {
                            if(postBuildEventChild->GetName() == wxT("Command")) {
                                wxString elemCondition = postBuildEventChild->GetAttribute(wxT("Condition"));
                                wxString projectCfgKey = ExtractProjectCfgName(parentCondition, elemCondition);

                                wxString command = postBuildEventChild->GetNodeContent();
                                if(!command.IsEmpty()) {
                                    GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];
                                    if(genericProjectCfg) { genericProjectCfg->postBuildCommands.push_back(command); }
                                }
                            }

                            postBuildEventChild = postBuildEventChild->GetNext();
                        }
                    }

                    itemDefinitionGroupChild = itemDefinitionGroupChild->GetNext();
                }
            }

            projectChild = projectChild->GetNext();
        }

        for(std::pair<wxString, GenericProjectCfgPtr> genericProjectCfg : genericProjectCfgMap) {
            if(genericProjectCfg.second) { genericProject->cfgs.push_back(genericProjectCfg.second); }
        }
    }

    wxXmlDocument filterDoc;
    if(filterDoc.Load(filterInfo.GetFullPath())) {
        wxXmlNode* root = filterDoc.GetRoot();
        wxXmlNode* projectChild = root->GetChildren();

        while(projectChild) {
            if(projectChild->GetName() == wxT("ItemGroup")) {
                wxXmlNode* itemGroupChild = projectChild->GetChildren();

                while(itemGroupChild) {
                    if(itemGroupChild->GetName() == wxT("ClInclude") || itemGroupChild->GetName() == wxT("ClCompile") ||
                       itemGroupChild->GetName() == wxT("None") || itemGroupChild->GetName() == wxT("Text") ||
                       itemGroupChild->GetName() == wxT("ResourceCompile") ||
                       itemGroupChild->GetName() == wxT("Image") || itemGroupChild->GetName() == wxT("CustomBuild")) {
                        AddFilesVC11(itemGroupChild, genericProject, genericProjectCfgMap);
                    }

                    itemGroupChild = itemGroupChild->GetNext();
                }
            }

            projectChild = projectChild->GetNext();
        }
    }

    genericWorkspace->projects.push_back(genericProject);
}
//------------------------------------------------------------------------------------------------
wxString VisualCppImporter::ExtractProjectCfgName(const wxString& parentCondition, const wxString& elemCondition)
{
    wxString tmp = !elemCondition.IsEmpty() ? elemCondition : parentCondition;
    tmp.Replace(wxT("'$(Configuration)|$(Platform)'=='"), wxT(""));
    tmp.Replace(wxT("'"), wxT(""));
    return tmp;
}
//------------------------------------------------------------------------------------------------
wxString VisualCppImporter::ReplaceDefaultEnvVars(const wxString& str)
{
    wxString tmp = str;
    tmp.Replace(wxT("$(ConfigurationName)"), wxT("$(VS_ConfigurationName)"));
    tmp.Replace(wxT("$(Configuration)"), wxT("$(VS_Configuration)"));
    tmp.Replace(wxT("$(PlatformName)"), wxT("$(VS_PlatformName)"));
    tmp.Replace(wxT("$(Platform)"), wxT("$(VS_Platform)"));
    tmp.Replace(wxT("$(IntDir)"), wxT("$(VS_IntDir)"));
    tmp.Replace(wxT("$(OutDir)"), wxT("$(VS_OutDir)"));
    tmp.Replace(wxT("$(SolutionDir)"), wxT("$(VS_SolutionDir)"));
    tmp.Replace(wxT("$(ProjectName)"), wxT("$(VS_ProjectName)"));
    tmp.Replace(wxT("$(ProjectDir)"), wxT("$(VS_ProjectDir)"));

    return tmp;
}
//------------------------------------------------------------------------------------------------
void VisualCppImporter::AddFilesVC7(wxXmlNode* filterChild, GenericProjectPtr genericProject,
                                    std::map<wxString, GenericProjectCfgPtr>& genericProjectCfgMap,
                                    wxString preVirtualPath)
{
    while(filterChild) {
        if(filterChild->GetName() == wxT("Filter")) {
            wxString virtualPath = filterChild->GetAttribute(wxT("Name"));
            wxXmlNode* fileChild = filterChild->GetChildren();

            if(!preVirtualPath.IsEmpty()) { virtualPath = preVirtualPath + wxT("/") + virtualPath; }

            AddFilesVC7(fileChild, genericProject, genericProjectCfgMap, virtualPath);
        } else if(filterChild->GetName() == wxT("File")) {
            wxString filename = filterChild->GetAttribute(wxT("RelativePath"));
            filename.Replace(wxT("\\"), wxT("/"));

            GenericProjectFilePtr genericProjectFile = std::make_shared<GenericProjectFile>();
            genericProjectFile->name = filename;
            genericProjectFile->vpath = preVirtualPath;

            genericProject->files.push_back(genericProjectFile);

            wxXmlNode* fileConfChild = filterChild->GetChildren();

            while(fileConfChild) {
                if(fileConfChild->GetName() == wxT("FileConfiguration")) {
                    wxString name = fileConfChild->GetAttribute(wxT("Name"));
                    wxString excludedFromBuild = fileConfChild->GetAttribute(wxT("ExcludedFromBuild"));
                    wxString projectCfgName = name;
                    projectCfgName.Replace(wxT("|"), wxT("_"));

                    if(excludedFromBuild == wxT("true")) {
                        GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgName];
                        if(genericProjectCfg) { genericProjectCfg->excludeFiles.push_back(genericProjectFile); }
                    }

                    wxXmlNode* toolChild = fileConfChild->GetChildren();

                    while(toolChild) {
                        if(toolChild->GetName() == wxT("Tool")) {
                            if(toolChild->GetAttribute(wxT("Name")) == wxT("VCCustomBuildTool")) {
                                wxString commandLine = toolChild->GetAttribute(wxT("CommandLine"));

                                GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgName];
                                if(genericProjectCfg && !commandLine.IsEmpty()) {
                                    genericProjectCfg->preBuildCommands.push_back(commandLine);
                                }
                            }
                        }

                        toolChild = toolChild->GetNext();
                    }
                }

                fileConfChild = fileConfChild->GetNext();
            }
        }

        filterChild = filterChild->GetNext();
    }
}
//------------------------------------------------------------------------------------------------
void VisualCppImporter::AddFilesVC11(wxXmlNode* itemGroupChild, GenericProjectPtr genericProject,
                                     std::map<wxString, GenericProjectCfgPtr>& genericProjectCfgMap)
{
    wxString filename = itemGroupChild->GetAttribute(wxT("Include"));
    filename.Replace(wxT("\\"), wxT("/"));

    GenericProjectFilePtr genericProjectFile = FindProjectFileByName(genericProject, filename);

    if(genericProjectFile) {
        genericProjectFile->name = filename;
    } else {
        genericProjectFile = std::make_shared<GenericProjectFile>();
        genericProjectFile->name = filename;

        genericProject->files.push_back(genericProjectFile);
    }

    wxXmlNode* otherChild = itemGroupChild->GetChildren();
    while(otherChild) {
        if(otherChild->GetName() == wxT("Filter")) {
            wxString content = otherChild->GetNodeContent();
            if(!content.IsEmpty()) { genericProjectFile->vpath = content; }
        }

        if(otherChild->GetName() == wxT("ExcludedFromBuild")) {
            wxString elemCondition = otherChild->GetAttribute(wxT("Condition"));
            wxString projectCfgKey = ExtractProjectCfgName(wxT(""), elemCondition);
            wxString content = otherChild->GetNodeContent();

            if(content == wxT("true")) {
                if(!projectCfgKey.IsEmpty()) {
                    GenericProjectCfgPtr genericProjectCfg = genericProjectCfgMap[projectCfgKey];

                    if(genericProjectCfg) { genericProjectCfg->excludeFiles.push_back(genericProjectFile); }
                } else {
                    for(std::pair<wxString, GenericProjectCfgPtr> genericProjectCfgPair : genericProjectCfgMap) {
                        if(genericProjectCfgPair.second) {
                            genericProjectCfgPair.second->excludeFiles.push_back(genericProjectFile);
                        }
                    }
                }
            }
        }

        otherChild = otherChild->GetNext();
    }
}
//------------------------------------------------------------------------------------------------
GenericProjectFilePtr VisualCppImporter::FindProjectFileByName(GenericProjectPtr genericProject, wxString filename)
{
    for(GenericProjectFilePtr projectFile : genericProject->files) {
        if(projectFile->name == filename) return projectFile;
    }

    return nullptr;
}
//------------------------------------------------------------------------------------------------
void VisualCppImporter::ConvertToLinuxStyle(wxString& filepath)
{
    filepath.Replace("\\", "/");
    wxFileName fn(filepath);
    fn.Normalize(wxPATH_NORM_DOTS); // Remove . and ..
    filepath = fn.GetFullPath(wxPATH_UNIX);
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// CodeBlocksImporter
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
bool CodeBlocksImporter::OpenWordspace(const wxString& filename, const wxString& defaultCompiler)
{
    wsInfo.Assign(filename);

    extension = wsInfo.GetExt().Lower();

    bool isValidExt = extension == wxT("cbp") || extension == wxT("workspace");

    bool result = wsInfo.FileExists() && wsInfo.IsFileReadable() && isValidExt;

    return result;
}
//------------------------------------------------------------------------------------------------
bool CodeBlocksImporter::isSupportedWorkspace()
{
    wxXmlDocument codeBlocksProject;
    if(codeBlocksProject.Load(wsInfo.GetFullPath())) {
        wxXmlNode* root = codeBlocksProject.GetRoot();
        CHECK_PTR_RET_FALSE(root);
        wxString nodeName = root->GetName();
        if((nodeName == wxT("CodeBlocks_workspace_file") || nodeName == wxT("CodeBlocks_project_file")))
            return true;
    }
    return false;
}
//------------------------------------------------------------------------------------------------
GenericWorkspacePtr CodeBlocksImporter::PerformImport()
{
    GenericWorkspacePtr genericWorkspace = std::make_shared<GenericWorkspace>();
    genericWorkspace->name = wsInfo.GetName();
    genericWorkspace->path = wsInfo.GetPath();

    if(extension == wxT("cbp")) {
        GenericProjectDataType genericProjectData;
        genericProjectData[wxT("projectFullPath")] = wsInfo.GetFullPath();
        GenerateFromProject(genericWorkspace, genericProjectData);
    } else if(extension == wxT("workspace")) {
        GenerateFromWorkspace(genericWorkspace);
    }

    return genericWorkspace;
}
//------------------------------------------------------------------------------------------------
void CodeBlocksImporter::GenerateFromWorkspace(GenericWorkspacePtr genericWorkspace)
{
    GenericProjectDataListType genericProjectDataList;
    wxCopyFile(wsInfo.GetFullPath(), wsInfo.GetFullPath() + wxT(".backup"));
    wxXmlDocument codeBlocksProject;
    if(codeBlocksProject.Load(wsInfo.GetFullPath())) {
        wxXmlNode* root = codeBlocksProject.GetRoot();
        if(root) {
            wxXmlNode* rootChild = root->GetChildren();

            while(rootChild) {
                if(rootChild->GetName() == wxT("Workspace")) {
                    wxXmlNode* workspaceChild = rootChild->GetChildren();

                    while(workspaceChild) {
                        if(workspaceChild->GetName() == wxT("Project") &&
                           workspaceChild->HasAttribute(wxT("filename"))) {
                            wxString filename = workspaceChild->GetAttribute(wxT("filename"));
                            filename.Replace(wxT("\\"), wxT("/"));
                            wxFileName filenameInfo(filename);

                            GenericProjectDataType genericProjectData;
                            genericProjectData[wxT("projectFullPath")] =
                                wsInfo.GetPath() + gsgsPathSep + filename;

                            wxString deps = wxT("");
                            wxXmlNode* projectChild = workspaceChild->GetChildren();
                            while(projectChild) {
                                if(projectChild->GetName() == wxT("Depends") &&
                                   projectChild->HasAttribute(wxT("filename"))) {
                                    wxString filename = projectChild->GetAttribute(wxT("filename"));
                                    wxString projectFullPath =
                                        wsInfo.GetPath() + gsgsPathSep + filename;

                                    wxXmlDocument depsProject;
                                    if(depsProject.Load(projectFullPath)) {
                                        wxXmlNode* rootDeps = depsProject.GetRoot();
                                        if(rootDeps) {
                                            wxXmlNode* rootDepsChild = rootDeps->GetChildren();

                                            while(rootDepsChild) {
                                                if(rootDepsChild->GetName() == wxT("Project")) {
                                                    wxXmlNode* projectDepsChild = rootDepsChild->GetChildren();

                                                    while(projectDepsChild) {
                                                        if(projectDepsChild->GetName() == wxT("Option") &&
                                                           projectDepsChild->HasAttribute(wxT("title"))) {
                                                            wxString title =
                                                                projectDepsChild->GetAttribute(wxT("title"));
                                                            deps += title + wxT(";");
                                                        }

                                                        projectDepsChild = projectDepsChild->GetNext();
                                                    }
                                                }

                                                rootDepsChild = rootDepsChild->GetNext();
                                            }
                                        }
                                    }
                                }

                                projectChild = projectChild->GetNext();
                            }

                            genericProjectData[wxT("projectDeps")] = deps;

                            genericProjectDataList.push_back(genericProjectData);
                        }

                        workspaceChild = workspaceChild->GetNext();
                    }
                }

                rootChild = rootChild->GetNext();
            }
        }
    }

    for(GenericProjectDataType& genericProjectData : genericProjectDataList) {
        GenerateFromProject(genericWorkspace, genericProjectData);
    }
}
//------------------------------------------------------------------------------------------------
void CodeBlocksImporter::GenerateFromProject(GenericWorkspacePtr genericWorkspace,
                                             GenericProjectDataType& genericProjectData)
{
    wxXmlDocument codeBlocksProject;
    if(codeBlocksProject.Load(genericProjectData[wxT("projectFullPath")])) {
        wxXmlNode* root = codeBlocksProject.GetRoot();
        if(root) {
            wxXmlNode* rootChild = root->GetChildren();

            while(rootChild) {
                if(rootChild->GetName() == wxT("Project")) {
                    wxString globalCompilerOptions = wxT(""), globalIncludePath = wxT(""),
                             globalLinkerOptions = wxT(""), globalLibPath = wxT(""), globalLibraries = wxT("");

                    wxFileName projectInfo(genericProjectData[wxT("projectFullPath")]);
                    GenericProjectPtr genericProject = std::make_shared<GenericProject>();
                    genericProject->path = projectInfo.GetPath();

                    wxStringTokenizer deps(genericProjectData[wxT("projectDeps")], wxT(";"));

                    while(deps.HasMoreTokens()) {
                        wxString projectNameDep = deps.GetNextToken().Trim().Trim(false);
                        genericProject->deps.Add(projectNameDep);
                    }

                    genericWorkspace->projects.push_back(genericProject);

                    wxXmlNode* projectChild = rootChild->GetChildren();

                    while(projectChild) {
                        if(projectChild->GetName() == wxT("Option") && projectChild->HasAttribute(wxT("title"))) {
                            genericProject->name = projectChild->GetAttribute(wxT("title"));
                        }

                        if(projectChild->GetName() == wxT("Build")) {
                            wxXmlNode* buildChild = projectChild->GetChildren();

                            while(buildChild) {
                                if(buildChild->GetName() == wxT("Target") && buildChild->HasAttribute(wxT("title"))) {
                                    GenericProjectCfgPtr genericProjectCfg = std::make_shared<GenericProjectCfg>();
                                    genericProjectCfg->name = buildChild->GetAttribute(wxT("title"));
                                    genericProject->cfgs.push_back(genericProjectCfg);

                                    wxXmlNode* targetChild = buildChild->GetChildren();
                                    while(targetChild) {
                                        if(targetChild->GetName() == wxT("Option") &&
                                           targetChild->HasAttribute(wxT("output"))) {
                                            wxString output = targetChild->GetAttribute(wxT("output"));
                                            if(output.Contains(wxT("#")))
                                                output.Replace(wxT("#"), wxT(""));
                                            genericProjectCfg->outputFilename = output;
                                        }

                                        if(targetChild->GetName() == wxT("Option") &&
                                           targetChild->HasAttribute(wxT("working_dir"))) {
                                            wxString working_dir = targetChild->GetAttribute(wxT("working_dir"));
                                            if(working_dir.Contains(wxT("#")))
                                                working_dir.Replace(wxT("#"), wxT(""));
                                            genericProjectCfg->workingDirectory = working_dir;
                                        }

                                        if(targetChild->GetName() == wxT("Option") &&
                                           targetChild->HasAttribute(wxT("type"))) {
                                            wxString projectType = targetChild->GetAttribute(wxT("type"));

                                            if(projectType == wxT("2")) {
                                                genericProject->cfgType = GenericCfgType::STATIC_LIBRARY;
                                            } else if(projectType == wxT("3")) {
                                                genericProject->cfgType = GenericCfgType::DYNAMIC_LIBRARY;
                                            } else {
                                                genericProject->cfgType = GenericCfgType::EXECUTABLE;
                                            }

                                            genericProjectCfg->type = genericProject->cfgType;
                                        }

                                        if(targetChild->GetName() == wxT("Compiler")) {
                                            wxString compilerOptions = wxT(""), includePath = wxT("");

                                            wxXmlNode* compilerChild = targetChild->GetChildren();
                                            while(compilerChild) {
                                                if(compilerChild->GetName() == wxT("Add") &&
                                                   compilerChild->HasAttribute(wxT("option"))) {
                                                    compilerOptions +=
                                                        compilerChild->GetAttribute(wxT("option")) + wxT(" ");
                                                }

                                                if(compilerChild->GetName() == wxT("Add") &&
                                                   compilerChild->HasAttribute(wxT("directory"))) {
                                                    includePath +=
                                                        compilerChild->GetAttribute(wxT("directory")) + wxT(";");
                                                }

                                                compilerChild = compilerChild->GetNext();
                                            }

                                            if(includePath.Contains(wxT("#")))
                                                includePath.Replace(wxT("#"), wxT(""));

                                            genericProjectCfg->cCompilerOptions = compilerOptions;
                                            genericProjectCfg->cppCompilerOptions = compilerOptions;
                                            genericProjectCfg->includePath = includePath;
                                        }

                                        if(targetChild->GetName() == wxT("Linker")) {
                                            wxString linkerOptions = wxT(""), libPath = wxT(""), libraries = wxT("");

                                            wxXmlNode* linkerChild = targetChild->GetChildren();
                                            while(linkerChild) {
                                                if(linkerChild->GetName() == wxT("Add") &&
                                                   linkerChild->HasAttribute(wxT("option"))) {
                                                    linkerOptions +=
                                                        linkerChild->GetAttribute(wxT("option")) + wxT(" ");
                                                }

                                                if(linkerChild->GetName() == wxT("Add") &&
                                                   linkerChild->HasAttribute(wxT("directory"))) {
                                                    libPath += linkerChild->GetAttribute(wxT("directory")) + wxT(";");
                                                }

                                                if(linkerChild->GetName() == wxT("Add") &&
                                                   linkerChild->HasAttribute(wxT("library"))) {
                                                    libraries += linkerChild->GetAttribute(wxT("library")) + wxT(";");
                                                }

                                                linkerChild = linkerChild->GetNext();
                                            }

                                            if(libPath.Contains(wxT("#")))
                                                libPath.Replace(wxT("#"), wxT(""));

                                            if(libraries.Contains(wxT("#")))
                                                libraries.Replace(wxT("#"), wxT(""));

                                            genericProjectCfg->linkerOptions = linkerOptions;
                                            genericProjectCfg->libPath = libPath;
                                            genericProjectCfg->libraries = libraries;
                                        }

                                        if(targetChild->GetName() == wxT("ExtraCommands")) {
                                            wxXmlNode* extraCommandsChild = targetChild->GetChildren();
                                            while(extraCommandsChild) {
                                                if(extraCommandsChild->GetName() == wxT("Add") &&
                                                   extraCommandsChild->HasAttribute(wxT("before"))) {
                                                    wxString command = extraCommandsChild->GetAttribute(wxT("before"));
                                                    if(command.Contains(wxT("#")))
                                                        command.Replace(wxT("#"), wxT(""));
                                                    genericProjectCfg->preBuildCommands.push_back(command);
                                                }

                                                if(extraCommandsChild->GetName() == wxT("Add") &&
                                                   extraCommandsChild->HasAttribute(wxT("after"))) {
                                                    wxString command = extraCommandsChild->GetAttribute(wxT("after"));
                                                    if(command.Contains(wxT("#")))
                                                        command.Replace(wxT("#"), wxT(""));
                                                    genericProjectCfg->postBuildCommands.push_back(command);
                                                }

                                                extraCommandsChild = extraCommandsChild->GetNext();
                                            }
                                        }

                                        if(targetChild->GetName() == wxT("MakeCommands")) {
                                            wxXmlNode* makeCommandsChild = targetChild->GetChildren();

                                            while(makeCommandsChild) {
                                                if(makeCommandsChild->GetName() == wxT("Build") &&
                                                   makeCommandsChild->HasAttribute(wxT("command"))) {
                                                    wxString buildCommand = makeCommandsChild->GetAttribute(wxT("command"));

                                                    if(!genericProjectCfg->enableCustomBuild)
                                                        genericProjectCfg->enableCustomBuild = true;

                                                    genericProjectCfg->customBuildCmd = buildCommand;
                                                } else if(makeCommandsChild->GetName() == wxT("Clean") &&
                                                          makeCommandsChild->HasAttribute(wxT("command"))) {
                                                    wxString cleanCommand = makeCommandsChild->GetAttribute(wxT("command"));

                                                    if(!genericProjectCfg->enableCustomBuild)
                                                        genericProjectCfg->enableCustomBuild = true;

                                                    genericProjectCfg->customCleanCmd = cleanCommand;
                                                }

                                                makeCommandsChild = makeCommandsChild->GetNext();
                                            }
                                        }

                                        targetChild = targetChild->GetNext();
                                    }
                                }

                                buildChild = buildChild->GetNext();
                            }
                        }

                        if(projectChild->GetName() == wxT("Compiler")) {
                            wxXmlNode* compilerChild = projectChild->GetChildren();
                            while(compilerChild) {
                                if(compilerChild->GetName() == wxT("Add") &&
                                   compilerChild->HasAttribute(wxT("option"))) {
                                    globalCompilerOptions += compilerChild->GetAttribute(wxT("option")) + wxT(" ");
                                }

                                if(compilerChild->GetName() == wxT("Add") &&
                                   compilerChild->HasAttribute(wxT("directory"))) {
                                    globalIncludePath += compilerChild->GetAttribute(wxT("directory")) + wxT(";");
                                }

                                compilerChild = compilerChild->GetNext();
                            }

                            if(globalIncludePath.Contains(wxT("#")))
                                globalIncludePath.Replace(wxT("#"), wxT(""));
                        }

                        if(projectChild->GetName() == wxT("Linker")) {
                            wxXmlNode* linkerChild = projectChild->GetChildren();
                            while(linkerChild) {
                                if(linkerChild->GetName() == wxT("Add") && linkerChild->HasAttribute(wxT("option"))) {
                                    globalLinkerOptions += linkerChild->GetAttribute(wxT("option")) + wxT(" ");
                                }

                                if(linkerChild->GetName() == wxT("Add") &&
                                   linkerChild->HasAttribute(wxT("directory"))) {
                                    globalLibPath += linkerChild->GetAttribute(wxT("directory")) + wxT(";");
                                }

                                if(linkerChild->GetName() == wxT("Add") && linkerChild->HasAttribute(wxT("library"))) {
                                    globalLibraries += linkerChild->GetAttribute(wxT("library")) + wxT(";");
                                }

                                linkerChild = linkerChild->GetNext();
                            }

                            if(globalLibPath.Contains(wxT("#")))
                                globalLibPath.Replace(wxT("#"), wxT(""));
                        }

                        if(projectChild->GetName() == wxT("Unit") && projectChild->HasAttribute(wxT("filename"))) {
                            wxString vpath = wxT("");
                            wxXmlNode* unitChild = projectChild->GetChildren();

                            while(unitChild) {
                                if(unitChild->GetName() == wxT("Option") &&
                                   unitChild->HasAttribute(wxT("virtualFolder"))) {
                                    vpath = unitChild->GetAttribute(wxT("virtualFolder"));
                                }
                                unitChild = unitChild->GetNext();
                            }

                            wxString projectFilename = projectChild->GetAttribute(wxT("filename"));
                            GenericProjectFilePtr genericProjectFile = std::make_shared<GenericProjectFile>();
                            genericProjectFile->name = projectFilename;
                            genericProjectFile->vpath = vpath;
                            genericProject->files.push_back(genericProjectFile);
                        }

                        projectChild = projectChild->GetNext();
                    }

                    for(GenericProjectCfgPtr genericProjectCfg : genericProject->cfgs) {
                        genericProjectCfg->cCompilerOptions += globalCompilerOptions;
                        genericProjectCfg->cppCompilerOptions += globalCompilerOptions;
                        genericProjectCfg->includePath += globalIncludePath;
                        genericProjectCfg->linkerOptions += globalLinkerOptions;
                        genericProjectCfg->libPath += globalLibPath;
                        genericProjectCfg->libraries += globalLibraries;
                    }
                }

                rootChild = rootChild->GetNext();
            }
        }
    }
}
//------------------------------------------------------------------------------------------------