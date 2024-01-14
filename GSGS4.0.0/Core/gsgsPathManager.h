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
#ifndef gsgs_PathManager_H
#define gsgs_PathManager_H

#include "gsgsPreInclude.h"

namespace gsgs
{
    class _gsgsAPI DirTraverser : public wxDirTraverser
    {
    public:
        DirTraverser(const wxString & filespec, bool includExtLessFiles = false);
        virtual ~DirTraverser();

        inline const wxArrayString & GetFiles()const                { return m_files; }

        /**
         * This function is called once a file is found. The traverse of the directories
         * can be stopped based on the return value from this function:
         * - wxDIR_IGNORE = -1,      // ignore this directory but continue with others
         * - wxDIR_STOP,             // stop traversing
         * - wxDIR_CONTINUE          // continue into this directory
         * \param filename name of the file that was found
         * \return one of the values wxDIR_STOP, wxDIR_IGNORE or wxDIR_CONTINUE
         */
        virtual wxDirTraverseResult OnFile(const wxString & filename);

        /**
         * This function is called once a directory is found. The traverse of the directories
         * can be stopped based on the return value from this function:
         * - wxDIR_IGNORE = -1,      // ignore this directory but continue with others
         * - wxDIR_STOP,             // stop traversing
         * - wxDIR_CONTINUE          // continue into this directory
         * \param dirname name of the directory that was found
         * \return one of the values wxDIR_STOP, wxDIR_IGNORE or wxDIR_CONTINUE
         */
        virtual wxDirTraverseResult OnDir(const wxString & dirname);

        inline void SetExcludeDirs(const wxArrayString & dirs)      { m_excludeDirs = dirs; }

        inline const wxArrayString & GetExcludeDirs() const         { return m_excludeDirs; }
    private:
        wxArrayString m_files;
        wxString m_filespec;
        wxArrayString m_specArray;
        wxArrayString m_excludeDirs;
        bool m_extlessFiles;
    };
    
    class _gsgsAPI PathManager
    {
    public:
        static PathManager& singleton();
    
        inline void SetOriginalCwd(const wxString & path)       { m_originalCwd = path; }
        
        inline const wxString & GetOriginalCwd() const          { return m_originalCwd; }

        inline void SetStartupDirectory(const wxString& path)   { m_startupDir = path; }
        
        inline const wxString & GetStartupDirectory() const     { return m_startupDir; }

        inline  void SetInstallDir(const wxString& dir)         { m_installDir = dir; }
        
        inline const wxString & GetInstallDir() const           { return m_installDir; }
        
        void SetUserDataDir(const wxString & path)              { m_path = path;  }

        wxString GetUserDataDir() const                         { return m_path; }

        const wxString & GetTempDir() const                     { return mTempPath; }
        
        void RemoveTempDir()                                    { wxFileName::Rmdir(mTempPath, wxPATH_RMDIR_RECURSIVE); }

        wxString GetPluginsDirectory() const;

        /**
         * @brief Return the location of the applications global, i.e. not user-specific, data files.
         * Example return values:
            Unix: prefix/share/appinfo
            Windows: the directory where the executable file is located
            Mac: appinfo.app/Contents/SharedSupport bundle subdirectory
         */
        wxString GetDataDir() const;

        // Under MSW, OSX and GTK the bin folder is the same directory as the
        // main executable (gsgs{.exe})
        // Typically this function returns:
        // MSW: /path/to/gsgs.exe
        // GTK: /usr/bin
        // OSX: /path/to/gsgs.app/Contents/MacOS
        wxString GetBinFolder() const;

        /**
         * @brief return the full path for an executable. This function
         * usually returns: GetBinFolder() + "/" + toolname + ".exe"
         * @note the .exe and "/" are platform dependant
         */
        wxString GetBinaryFullPath(const wxString &toolname) const;

        /**
         * @brief get gsgs executale path
         */
        wxString GetExecutablePath() const                      { return wxStandardPaths::Get().GetExecutablePath(); }

        wxString GetUserLexersDir() const;

        wxString GetLexersDir() const;

        wxString GetProjectTemplatesDir() const;

        wxString GetUserProjectTemplatesDir() const;

        /**
         * @brief return the documents directory.
         * Windows: C:\Users\name\Documents
         * Unix: /home/name/Documents
         * OSX: /Users/name/Documents
         */
        wxString GetDocumentsDir() const;

        /**
         * @brief return the installation directory.
         */
        wxString GetPosixInstallDir() const;
        
        void SetLauncherPath(const wxString & path)     { mLauncher = path; }
        
        wxString GetLauncherPath() const                { return mLauncher.GetFullPath();  }

        void IgnoreAppSubDir(const wxString & subdirPattern);

        wxString Locate(const wxString & name);

        wxString GetDefault(const wxString & name)      { return m_installDir + wxT("/") + name + wxT(".default"); }

        void DeleteLocal(const wxString & name);
    private:
        PathManager();
        virtual ~PathManager();
    private:
        wxString m_installDir;
        wxString m_startupDir;
        wxString m_originalCwd;
        wxString m_path;
        wxString mUserName;
        wxString mTempPath;
        wxFileName mLauncher;
    };
}
#define gsgs_Path() gsgs::PathManager::singleton()
#endif
