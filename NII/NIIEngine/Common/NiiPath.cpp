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

#include "NiiPreProcess.h"
#include "NiiPath.h"

#if N_PLAT == N_PLAT_WIN32
#include <shellapi.h>
#include <windows.h>
#include <direct.h>
#include <iostream>
#include "shlobj_core.h"
#endif

namespace NII
{
#if N_PLAT == N_PLAT_WIN32
    class ItemIdList
    {
    public:
        // ctor takes ownership of the item and will free it
        ItemIdList(LPITEMIDLIST pidl)
        {
            m_pidl = pidl;
        }

        static void Free(LPITEMIDLIST pidl)
        {
            if (pidl)
            {
                LPMALLOC pMalloc;
                SHGetMalloc(&pMalloc);
                if (pMalloc)
                {
                    pMalloc->Free(pidl);
                    pMalloc->Release();
                }
            }
        }

        ~ItemIdList()
        {
            Free(m_pidl);
        }

        // implicit conversion to LPITEMIDLIST
        operator LPITEMIDLIST() const { return m_pidl; }

        // get the corresponding path, returns empty string on error
        void GetPath(String & out) const
        {
            Ntchar temp[MAX_PATH];
            if (!SHGetPathFromIDList(m_pidl, temp))
            {
                //    wxLogLastError(wxT("SHGetPathFromIDList"));
            }

            out = temp;
        }

    private:
        LPITEMIDLIST m_pidl;
    };
#endif
    //-----------------------------------------------------------------------
    String Path::mInstallPath;
    String Path::mStartupPath;
    String Path::mOriginalCwd;
    String Path::mExecutablePath;
    String Path::mUserPath;
    String Path::mPluginPath;
    String Path::mUserName;
    String Path::mLauncher;
    String Path::mTempPath;
    String Path::mAppName;
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Path
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void Path::startup()
    {
        if (mExecutablePath.empty())
        {
            HMODULE hMod = GetModuleHandle(_T("NIIEngine.dll"));
            TCHAR buffer[512] = { 0 };
            DWORD ddr2 = GetModuleFileName(hMod, buffer, sizeof(buffer));
            StrUtil::splitPath(buffer, mExecutablePath);
        }
        if(mPluginPath.size())
            mPluginPath = mExecutablePath + N_DIR_SEP_S + mPluginPath + N_DIR_SEP_S;
        else
            mPluginPath = mExecutablePath + N_DIR_SEP_S;
        mInstallPath = mExecutablePath;
        mStartupPath = mExecutablePath;
        mLauncher = mExecutablePath;
        Ntchar * tempp  = Ngetenv(_T("TMPDIR"));
        if (!tempp)
        {
            tempp = Ngetenv(_T("TMP"));
        }
        if (!tempp)
        {
            tempp = Ngetenv(_T("TEMP"));
        }
        if (tempp)
            mTempPath = tempp;
        if(mTempPath.size())
        {
#if N_PLAT == N_PLAT_WIN32
            mTempPath += _T("\\NIIEngine.") + mAppName;
#else
            mTempPath += _T("/tmp/NIIEngine.") + mAppName;
#endif
        }
        createPath(mTempPath);

        Ntchar * installbuffer;
        if ((installbuffer = Ngetcwd(NULL, 0)) != NULL)
        {
            mOriginalCwd = installbuffer;
            free(installbuffer);
        }

        getSpecialFolderPath(CSIDL_APPDATA, mUserPath);
        mUserPath += N_DIR_SEP_S _T("NIIEngine.") + mAppName;
        createPath(mUserPath);
    }
    //-----------------------------------------------------------------------
    void Path::removeTempDir()
    {
        if(mTempPath.size())
            removePath(mTempPath);
    }
    //-----------------------------------------------------------------------
    const String & Path::getDataDir()
    {
        return StrUtil::BLANK;
    }
    //-----------------------------------------------------------------------
    const String & Path::getBinFolder()
    {
        return StrUtil::BLANK;
    }
    //-----------------------------------------------------------------------
    const String & Path::getBinaryFullPath(const String &toolname)
    {
        return StrUtil::BLANK;
    }
    //-----------------------------------------------------------------------
    const String & Path::getProjectTemplatesDir()
    {
        return StrUtil::BLANK;
    }
    //-----------------------------------------------------------------------
    const String & Path::getUserProjectTemplatesDir()
    {
        return StrUtil::BLANK;
    }
    //-----------------------------------------------------------------------
    const String & Path::getDocumentsDir()
    {
        return StrUtil::BLANK;
    }
    //-----------------------------------------------------------------------
    const String & Path::locate(const String & name)
    {
        return StrUtil::BLANK;
    }
    //-----------------------------------------------------------------------
    const String & Path::getDefault(const String & name)
    {
        return StrUtil::BLANK;
    }
    //-----------------------------------------------------------------------
    void Path::deleteLocal(const String & name)
    {

    }
    //-----------------------------------------------------------------------
    void Path::getSpecialFolderPath(int csidl, String & name)
    {
#if N_PLAT == N_PLAT_WIN32
        Ntchar dir[MAX_PATH];
        HRESULT hr = E_FAIL;

        // test whether the function is available during compile-time (it must be
        // defined as either "SHGetFolderPathA" or "SHGetFolderPathW")
#ifdef SHGetFolderPath
    // and now test whether we have it during run-time
            hr = SHGetFolderPath
            (
                NULL,               // parent window, not used
                csidl,
                NULL,               // access token (current user)
                SHGFP_TYPE_CURRENT, // current path, not just default value
                dir
            );

            // somewhat incredibly, the error code in the Unicode version is
            // different from the one in ASCII version for this function
#if _UNICODE
            if (hr == E_FAIL)
#else
            if (hr == S_FALSE)
#endif
            {
                // directory doesn't exist, maybe we can get its default value?
                hr = SHGetFolderPath
                (
                    NULL,
                    csidl,
                    NULL,
                    SHGFP_TYPE_DEFAULT,
                    dir
                );
            }
#endif // SHGetFolderPath

#ifdef SHGetSpecialFolderPath
        if (FAILED(hr))
        {
            hr = SHGetSpecialFolderPath
            (
                NULL,               // parent window
                dir,
                csidl,
                FALSE               // don't create if doesn't exist
            );
        }
#endif // SHGetSpecialFolderPath

        // SHGetSpecialFolderLocation should be available with all compilers and
        // under all Win32 systems, so don't test for it (and as it doesn't exist
        // in "A" and "W" versions anyhow, testing would be more involved, too)
        if (FAILED(hr))
        {
            LPITEMIDLIST pidl;
            hr = SHGetSpecialFolderLocation(NULL, csidl, &pidl);

            if (SUCCEEDED(hr))
            {
                // creating this temp object has (nice) side effect of freeing pidl
                ItemIdList(pidl).GetPath(name);
            }
        }
        else
        {
            name = dir;
        }
#endif
    }
    //-----------------------------------------------------------------------
    void Path::createPath(const String & path)
    {
        if (0 != Naccess(path.c_str(), 0))
        {
            // if this folder not exist, create a new one.
            Nmkdir(path.c_str());   // 返回 0 表示创建成功，-1 表示失败
        }
    }
    //-----------------------------------------------------------------------
    bool Path::removePath(const String & path)
    {
#if N_PLAT == N_PLAT_WIN32
        // SHFileOperation needs double null termination string
        // but without separator at the end of the path
        //wxString path(dir);
        //if (path.Last() == wxFILE_SEP_PATH)
        //    path.RemoveLast();
        String dst = path + _T('\0');
        SHFILEOPSTRUCT fileop;
        ::ZeroMemory(&fileop, sizeof(fileop));
        fileop.wFunc = FO_DELETE;
        fileop.pFrom = dst.c_str();
        fileop.fFlags = FOF_SILENT | FOF_NOCONFIRMATION;
#ifndef __WXWINCE__
        // FOF_NOERRORUI is not defined in WinCE
        fileop.fFlags |= FOF_NOERRORUI;
#endif

        int ret = SHFileOperation(&fileop);
        if (ret != 0)
        {
            return false;
        }
        return true;
#elif N_PLAT == N_PLAT_OSX
        int res = unlink(file.fn_str());
        return res == 0;
#else
        int res = Nremove(file.fn_str());
        return res == 0;
#endif
    }
    //-----------------------------------------------------------------------
}