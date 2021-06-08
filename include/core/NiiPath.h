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
#ifndef _NII_PATH_H_
#define _NII_PATH_H_

#include "NiiPreInclude.h"
#include "NiiString.h"

namespace NII
{
    /** 路径控制
    @version NIIEngine 4.0.0
    */
    class _EngineAPI Path : public EngineAlloc
    {
    public:
        static void startup();

        /**
        @version NIIEngine 4.0.0
        */
        static inline void setAppName(const String & name)      { mAppName = name; }

        /**
        @version NIIEngine 4.0.0
        */
        static inline const String & getAppName()               { return mAppName; }

        /**
        @version NIIEngine 4.0.0
        */
        static inline void setUserName(const String & name)     { mUserName = name; }

        /**
        @version NIIEngine 4.0.0
        */
        static inline const String &  getUserName()             { return mUserName; }

        /**
        @version NIIEngine 4.0.0
        */
        static inline void setOriginalCwd(const String & path)  { mOriginalCwd = path; }

        /**
        @version NIIEngine 4.0.0
        */
        static inline const String & getOriginalCwd()           { return mOriginalCwd; }

        /**
        @version NIIEngine 4.0.0
        */
        static inline void setStartupPath(const String& path)   { mStartupPath = path; }

        /**
        @version NIIEngine 4.0.0
        */
        static inline const String & getStartupPath()           { return mStartupPath; }

        /**
        @version NIIEngine 4.0.0
        */
        static inline void setInstallPath(const String& dir)    { mInstallPath = dir; }

        /**
        @version NIIEngine 4.0.0
        */
        static inline const String & getInstallPath()           { return mInstallPath; }

        /**
        @version NIIEngine 4.0.0
        */
        static inline void setLauncherPath(const String & path) { mLauncher = path; }

        /**
        @version NIIEngine 4.0.0
        */
        static inline const String & getLauncherPath()          { return mLauncher; }

        /**
        @version NIIEngine 4.0.0
        */
        static void setUserDataDir(const String & path)         { mUserPath = path; }

        /**
        @version NIIEngine 4.0.0
        */
        static const String & getUserDataDir()                  { return mUserPath; }

        /**
        @version NIIEngine 4.0.0
        */
        static const String & getTempDir()                      { return mTempPath; }

        static void removeTempDir();

        static const String & getPluginPath()                   { return mPluginPath; }

        /**
        @retrun Unix: prefix/share/appinfo
            Windows: 安装目录
            Mac: appinfo.app/Contents/SharedSupport/*
         */
        static const String & getDataDir();

        /**
        @retrun MSW: 安装目录
            GTK: /usr/bin
            OSX: /path/to/gsgs.app/Contents/MacOS
        */
        static const String & getBinFolder();

        /**
        */
        static const String & getBinaryFullPath(const String &toolname);

        /**
        @version NIIEngine 4.0.0
         */
        static const String & getExecutablePath()               { return mExecutablePath; }

        /**
        @version NIIEngine 3.0.0
        */
        static const String & getProjectTemplatesDir();

        static const String & getUserProjectTemplatesDir();

        /**
        @retrun Windows: C:\Users\name\Documents
            Unix: /home/name/Documents
            OSX: /Users/name/Documents
        */
        static const String & getDocumentsDir();

        static const String & locate(const String & name);

        static const String & getDefault(const String & name);

        static void deleteLocal(const String & name);

        static void getSpecialFolderPath(int code, String & name);

        static bool removePath(const String & path);
    private:
        static String mInstallPath;
        static String mStartupPath;
        static String mOriginalCwd;
        static String mExecutablePath;
        static String mUserPath;
        static String mPluginPath;
        static String mUserName;
        static String mTempPath;
        static String mLauncher;
        static String mAppName;
    };
}
#endif