
#include "gsgsPathManager.h"
#include "gsgsFileUtil.h"
#include <wx/datetime.h>

namespace gsgs
{
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    // DirTraverser
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    DirTraverser::DirTraverser(const wxString & filespec, bool includeExtLessFiles)
        : wxDirTraverser()
        , m_filespec(filespec)
        , m_extlessFiles(includeExtLessFiles)
    {
        m_specArray = wxStringTokenize(filespec, wxT(";"), wxTOKEN_STRTOK);
    }
    //---------------------------------------------------------------------------
    wxDirTraverseResult DirTraverser::OnFile(const wxString & filename)
    {
        // add the file to our array
        wxFileName fn(filename);
        
        if(FileUtil::WildMatch(m_filespec, fn)) {
            m_files.Add(filename);
            return wxDIR_CONTINUE;
        }

        // if we reached this point, no pattern was suitable for our file
        // test for extensionless file flag
        if (fn.GetExt().IsEmpty() && m_extlessFiles) {
            m_files.Add(filename);
        }

        return wxDIR_CONTINUE;
    }
    //---------------------------------------------------------------------------
    wxDirTraverseResult DirTraverser::OnDir(const wxString & dirname)
    {
        for (size_t i=0; i<m_excludeDirs.GetCount(); i++) {
            wxString tmpDir(dirname);
            tmpDir.Replace(wxT("\\"), wxT("/"));
            wxArrayString toks = wxStringTokenize(tmpDir, wxT("/"), wxTOKEN_STRTOK);
            wxString dir = m_excludeDirs.Item(i);
            wxString onlyDirName;

            if (toks.GetCount() > 0) {
                onlyDirName = toks.Last();
            }

            if (onlyDirName == dir) {
                return wxDIR_IGNORE;
            }
        }
        return wxDIR_CONTINUE;
    }
    //---------------------------------------------------------------------------
    DirTraverser::~DirTraverser()
    {
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    // PathManager
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    PathManager::PathManager()
    {
        IgnoreAppSubDir("bin");
        mLauncher = wxFileName(wxT("gsgs_launcher"));
        wxString name = wxGetUserName();

        // The wx doc says that 'name' may now be e.g. "Mr. John Smith"
        // So try to make it more suitable to be an extension
        name.MakeLower();
        name.Replace(wxT(" "), wxT("_"));
        for (size_t i = 0; i < name.Len(); ++i)
        {
            wxChar ch = name.GetChar(i);
            if ((ch < wxT('a') || ch > wxT('z')) && ch != wxT('_'))
            {
                // Non [a-z_] character: skip it
            }
            else
            {
                mUserName << ch;
            }
        }
        if (mUserName.IsEmpty())
            mUserName << wxDateTime::Now().GetTicks();

#if defined(__WXGTK__) || defined(__WXOSX__)
        mTempPath << "/tmp/GsGs." << mUserName;
#else
        mTempPath << wxStandardPaths::Get().GetTempDir() << "\\GsGs." << mUserName;
#endif
        // Create the temp folder
        wxFileName::Mkdir(mTempPath, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

#ifdef __WXGTK__

#ifndef NDEBUG
        // Debug mode
        wxFileName fn(wxStandardPaths::Get().GetUserDataDir());
        fn.SetFullName(fn.GetFullName() + "-dbg");
        m_path = fn.GetFullPath();
#else
        // Release mode
        m_path = wxStandardPaths::Get().GetUserDataDir();
#endif

#else // Windows / OSX
        m_path = wxStandardPaths::Get().GetUserDataDir();
#endif
    }
    //---------------------------------------------------------------------------
    PathManager::~PathManager() 
    {
    }
    //---------------------------------------------------------------------------
    PathManager & PathManager::singleton()
    {
        static PathManager pathmag;
        return pathmag;
    }
    //---------------------------------------------------------------------------
    wxString PathManager::GetPluginsDirectory() const
    {
#ifdef __WXGTK__
        wxString pluginsDir = PLUGINS_DIR;
#else
    #ifdef USE_POSIX_LAYOUT
        wxFileName path(GetInstallDir() + wxT(PLUGINS_DIR), "");
    #else
        wxFileName path(GetDataDir(), "");
        path.AppendDir("plugins");
    #endif
        wxString pluginsDir = path.GetPath();
#endif
        return pluginsDir;
    }
    //---------------------------------------------------------------------------
    wxString PathManager::GetDataDir() const
    {
#ifdef USE_POSIX_LAYOUT
        wxFileName path(wxStandardPaths::Get().GetDataDir() + wxT(INSTALL_DIR), "");
        return path.GetPath();
#else
        return wxStandardPaths::Get().GetDataDir();
#endif
    }
    //---------------------------------------------------------------------------
    wxString PathManager::GetBinFolder() const
    {
        wxFileName fnExe(wxStandardPaths::Get().GetExecutablePath());
        return fnExe.GetPath();
    }
    //---------------------------------------------------------------------------
    wxString PathManager::GetBinaryFullPath(const wxString& toolname) const
    {
        wxFileName binary(GetBinFolder(), toolname);
        binary.SetExt(wxEmptyString);
#ifdef __WXMSW__
        binary.SetExt("exe");
#endif
        return binary.GetFullPath();
    }
    //---------------------------------------------------------------------------
    wxString PathManager::GetLexersDir() const
    {
        wxFileName fn(GetDataDir(), "");
        fn.AppendDir("lexers");
        return fn.GetPath();
    }
    //---------------------------------------------------------------------------
    wxString PathManager::GetProjectTemplatesDir() const
    {
        wxFileName fn(GetDataDir(), "");
        fn.AppendDir("templates");
        fn.AppendDir("projects");
        return fn.GetPath();
    }
    //---------------------------------------------------------------------------
    wxString PathManager::GetUserLexersDir() const
    {
        wxFileName fn(GetUserDataDir(), "");
        fn.AppendDir("lexers");
        return fn.GetPath();
    }
    //---------------------------------------------------------------------------
    wxString PathManager::GetUserProjectTemplatesDir() const
    {
        wxFileName fn(GetUserDataDir(), "");
        fn.AppendDir("templates");
        fn.AppendDir("projects");
        return fn.GetPath();
    }
    //---------------------------------------------------------------------------
    wxString PathManager::GetDocumentsDir() const
    {
        wxString path = wxStandardPaths::Get().GetDocumentsDir();
#ifdef __WXGTK__
        // On linux, according to the docs, GetDocumentsDir() return the home directory
        // but what we really want is ~/Documents
        wxFileName fp(path, "");
        fp.AppendDir("Documents");
        if (fp.DirExists()) { return fp.GetPath(); }
#endif
        return path;
    }
    //---------------------------------------------------------------------------
    wxString PathManager::GetPosixInstallDir() const
    {
#ifdef __WXGTK__
        return GetBinFolder();
#else
#ifdef USE_POSIX_LAYOUT
        wxFileName path(wxStandardPaths::Get().GetDataDir(), "");
        return path.GetPath();
#else
        return GetBinFolder();
#endif
#endif
    }
    //---------------------------------------------------------------------------
    void PathManager::IgnoreAppSubDir(const wxString & subdirPattern)
    {
#ifdef USE_POSIX_LAYOUT
        wxStandardPaths::Get().IgnoreAppSubDir(subdirPattern);
#endif
    }
    //---------------------------------------------------------------------------
    wxString PathManager::Locate(const wxString & name)
    {
        wxFileName usrFile(GetUserDataDir() + wxT("/") + name);
        if (usrFile.FileExists()) {
            return usrFile.GetFullPath();
        }
        else
        {
            wxFileName defaultFile(m_installDir + wxT("/") + name + wxT(".default"));
            return defaultFile.GetFullPath();
        }
    }
    //---------------------------------------------------------------------------
    void PathManager::DeleteLocal(const wxString & name)
    {
        wxFileName usrFile(GetUserDataDir() + wxT("/") + name);
        if (usrFile.FileExists())
        {
            gsgsRemoveFile(usrFile.GetFullPath());
        }
    }
    //---------------------------------------------------------------------------
}