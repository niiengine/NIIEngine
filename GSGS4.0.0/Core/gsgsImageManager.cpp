
#include "gsgsImageManager.h"
#include "gsgsPathManager.h"
#include "gsgsEditorConfig.h"
#include "gsgsLogManager.h"
#include "gsgsStyleManager.h"
#include "gsgsOptionsConfig.h"
#include "gsgsZip.h"
#include "globals.h"
#include <wx/dcscreen.h>

#if wxUSE_GUI
#include <wx/bitmap.h>
#include <wx/dcscreen.h>
#ifdef __WXGTK20__
#include <gtk/gtk.h>
#endif
#endif
namespace gsgs
{
    //---------------------------------------------------------------------------
    #if wxUSE_GUI
        class Bitmap : public wxBitmap
        {
        public:
            static bool ShouldLoadHiResImages();

        public:
            Bitmap() {}
            Bitmap(const wxImage& img, double scale = 1.0);
            virtual ~Bitmap() {}

            virtual bool LoadFile(const wxString& name, wxBitmapType type = wxBITMAP_DEFAULT_TYPE);
        };

        Bitmap::Bitmap(const wxImage& img, double scale)
    #if wxVERSION_NUMBER >= 3100 
            : wxBitmap(img, -1, scale)
    #else
            : wxBitmap(img, -1)
    #endif
        {
        }

        bool Bitmap::LoadFile(const wxString& name, wxBitmapType type)
        {
    #if defined(__WXMSW__)
            wxFileName filename(name);
            double scale = 1.0;
            if (ShouldLoadHiResImages()) {
                wxFileName hiResFileName = filename;
                wxString hiresName = hiResFileName.GetName();
                hiresName << "@2x";
                hiResFileName.SetName(hiresName);
                if (hiResFileName.Exists()) {
                    filename = hiResFileName;
                    scale = 2.0;
                }
            }
            wxImage img(filename.GetFullPath(), type);
            *this = Bitmap(img, scale);
            return true;
    #else
            return wxBitmap::LoadFile(name, type);
    #endif
        }

        bool Bitmap::ShouldLoadHiResImages()
        {
            static bool once = false;
            static bool shouldLoad = false;
            if (!once) {
                once = true;
    #ifdef __WXGTK__
                GdkScreen *screen = gdk_screen_get_default();
                if (screen) {
                    double res = gdk_screen_get_resolution(screen);
                    shouldLoad = ((res / 96.) >= 1.5);
                }
    #else
                shouldLoad = ((wxScreenDC().GetPPI().y / 96.) >= 1.5);
    #endif
            }
            return shouldLoad;
        }
    #endif
    //---------------------------------------------------------------------------
    std::unordered_map<wxString, wxBitmap> ImageManager::mToolbarImage;
    std::unordered_map<wxString, wxString> ImageManager::mManifestImage;
    //---------------------------------------------------------------------------
    ImageManager * ImageManager::gInstance = 0;
    //---------------------------------------------------------------------------
    ImageManager & ImageManager::singleton()
    {
        if (gInstance == 0)
        {
            gInstance = new ImageManager();
        }
        return *gInstance;
    }
    //---------------------------------------------------------------------------
    void ImageManager::Release()
    {
        if (gInstance)
        {
            delete gInstance;
        }
        gInstance = 0;
    }
    //---------------------------------------------------------------------------
    ImageManager::~ImageManager()
    {
    }
    //---------------------------------------------------------------------------
    ImageManager::ImageManager() :
        mCurrentCustom(bCID),
        mCurrentInter(bIID),
        m_bMapPopulated(false)
    {
#if 0
        m_toolbarIconSize = 24;
#else
        m_toolbarIconSize = 16;
#endif
        initialize();
    }
    //---------------------------------------------------------------------------
    const wxBitmap & ImageManager::LoadBitmap(const wxString& name, int size)
    {
        // try to load a new bitmap first
        wxString newName;
        newName << size << "-" << name.AfterLast('/');

#ifdef __WXGTK__
        if (Bitmap::ShouldLoadHiResImages()) { newName << "@2x"; }
#endif

        std::unordered_map<wxString, wxBitmap>::const_iterator iter = mToolbarImage.find(newName);
        if (iter != mToolbarImage.end())
        {
            return iter->second;
        }

        iter = mToolbarImage.find(name);
        if (iter != mToolbarImage.end())
        {
            return iter->second;
        }

        return wxNullBitmap;
    }
    //---------------------------------------------------------------------------
    wxBitmap ImageManager::LoadBitmapImpl(const wxString & filepath)
    {
        wxString bitmapFile;
        if (ExtractFileFromZip(m_zipPath.GetFullPath(), filepath, gsgs_Path().GetUserDataDir(), bitmapFile))
        {
            Bitmap bmp;
            if (bmp.LoadFile(bitmapFile, wxBITMAP_TYPE_PNG))
            {
                gsgsRemoveFile(bitmapFile);
                return bmp;
            }
            gsgsRemoveFile(bitmapFile);
        }
        return wxNullBitmap;
    }
    //---------------------------------------------------------------------------
    /*wxIcon ImageManager::GetIcon(const wxBitmap & bmp) const
    {
        wxIcon icn;
        icn.CopyFromBitmap(bmp);
        return icn;
    }*/
    //---------------------------------------------------------------------------
    #define DARK_ICONS _("Dark Theme Icons Set")
    #define LIGHT_ICONS _("Light Theme Icons Set")
    //---------------------------------------------------------------------------
    void ImageManager::initialize()
    {
        if (mManifestImage.empty() || mToolbarImage.empty())
        {
            wxString zipname = "gsgs.zip";
            // Under linux, take into account the --prefix
#ifdef __WXGTK__
            wxString bitmapPath = wxString(INSTALL_DIR, wxConvUTF8);
            m_zipPath = = wxFileName(bitmapPath, zipname);
#else
            m_zipPath = wxFileName(gsgs_Path().GetDataDir(), zipname);
#endif
            if (m_zipPath.FileExists())
            {
                wxString targetFile;
                if (ExtractFileFromZip(m_zipPath.GetFullPath(), wxT("manifest.ini"), gsgs_Path().GetUserDataDir(), targetFile))
                {
                    // we got the file extracted, read it
                    wxFileName manifest(targetFile);
                    wxFFile fp(manifest.GetFullPath(), wxT("r"));
                    if (fp.IsOpened())
                    {
                        wxString content;
                        fp.ReadAll(&content);

                        mManifestImage.clear();
                        wxArrayString entries = wxStringTokenize(content, wxT("\n"), wxTOKEN_STRTOK);
                        for (size_t i = 0; i < entries.size(); i++) {
                            wxString entry = entries[i];
                            entry.Trim().Trim(false);

                            // empty?
                            if (entry.empty()) continue;

                            // comment?
                            if (entry.StartsWith(wxT(";"))) continue;

                            wxString key = entry.BeforeFirst(wxT('='));
                            wxString val = entry.AfterFirst(wxT('='));
                            key.Trim().Trim(false);
                            val.Trim().Trim(false);

                            wxString key16, key24;
                            key16 = key;
                            key24 = key;

                            key16.Replace(wxT("<size>"), wxT("16"));
                            key24.Replace(wxT("<size>"), wxT("24"));

                            key16.Replace(wxT("."), wxT("/"));
                            key24.Replace(wxT("."), wxT("/"));

                            mManifestImage[key16] = val;
                            mManifestImage[key24] = val;
                        }

                        fp.Close();
                        gsgsRemoveFile(manifest.GetFullPath());
                    }
                    gsgsRemoveFile(targetFile);
                }

                std::unordered_map<wxString, wxString>::iterator iter = mManifestImage.begin();
                for (; iter != mManifestImage.end(); iter++) {
                    wxString key = iter->first;
                    key = key.BeforeLast(wxT('/'));
                    mToolbarImage[iter->first] =
                        LoadBitmapImpl(wxString::Format(wxT("%s/%s"), key.c_str(), iter->second.c_str()));
                }
            }
        }

        // Load the bitmaps based on the current theme background colour
        wxFileName fnNewZip(gsgs_Path().GetDataDir(), "");
        if (Colour::IsDark(gsgs_Style().GetColour(wxSYS_COLOUR_3DFACE))) {
            fnNewZip.SetFullName("gsgsbm-dark.zip");
        }
        else
        {
            fnNewZip.SetFullName("gsgsbm-light.zip");
        }
        if (fnNewZip.FileExists())
        {
            ZipReader zip(fnNewZip);
            wxFileName tmpFolder(gsgs_Path().GetTempDir(), "");
            tmpFolder.AppendDir("gsgsbm");
            if (tmpFolder.DirExists())
            {
                tmpFolder.Rmdir(wxPATH_RMDIR_RECURSIVE);
            }

            tmpFolder.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

            // Extract all images into this folder
            zip.Extract("*", tmpFolder.GetPath());

            wxArrayString files;
            wxDir::GetAllFiles(tmpFolder.GetPath(), &files, "*.png");

            for (size_t i = 0; i < files.size(); ++i) {
                wxFileName pngFile(files.Item(i));
                wxString fullpath = pngFile.GetFullPath();
#ifndef __WXGTK__
                if (pngFile.GetFullName().Contains("@2x")) {
                    // No need to load the hi-res images manually,
                    // this is done by wxWidgets
                    continue;
                }
#endif
                Bitmap bmp;
                if (bmp.LoadFile(fullpath, wxBITMAP_TYPE_PNG)) {
                    gsgsDEBUG1() << "Adding new image:" << pngFile.GetName() << clEndl;
                    mToolbarImage.erase(pngFile.GetName());
                    mToolbarImage.insert({ pngFile.GetName(), bmp });
                }
            }
        }

        // Create the mime-list
        if (IsEmpty()) {
            AddBitmap(LoadBitmap("console", m_toolbarIconSize), FileUtil::TypeExe);
            AddBitmap(LoadBitmap("mime-html", m_toolbarIconSize), FileUtil::TypeHtml);
            AddBitmap(LoadBitmap("archive", m_toolbarIconSize), FileUtil::TypeArchive);
            AddBitmap(LoadBitmap("mime-php", m_toolbarIconSize), FileUtil::TypePhp);
            AddBitmap(LoadBitmap("dll", m_toolbarIconSize), FileUtil::TypeDll);
            AddBitmap(LoadBitmap("blocks", m_toolbarIconSize), FileUtil::TypeFormbuilder);
            AddBitmap(LoadBitmap("mime-txt", m_toolbarIconSize), FileUtil::TypeCodedesigner);
            AddBitmap(LoadBitmap("mime-bmp", m_toolbarIconSize), FileUtil::TypeBmp);
            AddBitmap(LoadBitmap("cog", m_toolbarIconSize), FileUtil::TypeMakefile);
            AddBitmap(LoadBitmap("mime-c", m_toolbarIconSize), FileUtil::TypeSourceC);
            AddBitmap(LoadBitmap("mime-cpp", m_toolbarIconSize), FileUtil::TypeSourceCpp);
            AddBitmap(LoadBitmap("mime-h", m_toolbarIconSize), FileUtil::TypeHeader);
            AddBitmap(LoadBitmap("mime-txt", m_toolbarIconSize), FileUtil::TypeText);
            AddBitmap(LoadBitmap("execute", m_toolbarIconSize), FileUtil::TypeScript);
            AddBitmap(LoadBitmap("mime-xml", m_toolbarIconSize), FileUtil::TypeXml);
            AddBitmap(LoadBitmap("mime-txt", m_toolbarIconSize), FileUtil::TypeErd);
            AddBitmap(LoadBitmap("mime-python", m_toolbarIconSize), FileUtil::TypePython);
            AddBitmap(LoadBitmap("mime-css", m_toolbarIconSize), FileUtil::TypeCSS);
            AddBitmap(LoadBitmap("mime-js", m_toolbarIconSize), FileUtil::TypeJS);
            AddBitmap(LoadBitmap("cxx-workspace", m_toolbarIconSize), FileUtil::TypeWorkspace);
            AddBitmap(LoadBitmap("php-workspace", m_toolbarIconSize), FileUtil::TypeWorkspacePHP);
            AddBitmap(LoadBitmap("docker", m_toolbarIconSize), FileUtil::TypeWorkspaceDocker);
            AddBitmap(LoadBitmap("nodejs-workspace", m_toolbarIconSize), FileUtil::TypeWorkspaceNodeJS);
            AddBitmap(LoadBitmap("project", m_toolbarIconSize), FileUtil::TypeProject);
            AddBitmap(LoadBitmap("project-opened", m_toolbarIconSize), FileUtil::TypeProjectExpanded);
            AddBitmap(LoadBitmap("blocks", m_toolbarIconSize), FileUtil::TypeWxCrafter);
            AddBitmap(LoadBitmap("mime-xml", m_toolbarIconSize), FileUtil::TypeXRC);
            AddBitmap(LoadBitmap("mime-res", m_toolbarIconSize), FileUtil::TypeResource);
            AddBitmap(LoadBitmap("mime-sql", m_toolbarIconSize), FileUtil::TypeSQL);
            AddBitmap(LoadBitmap("folder-yellow", m_toolbarIconSize), FileUtil::TypeFolder);
            AddBitmap(LoadBitmap("folder-yellow-opened", m_toolbarIconSize), FileUtil::TypeFolderExpanded);
            AddBitmap(LoadBitmap("mime-as", m_toolbarIconSize), FileUtil::TypeAsm);
            AddBitmap(LoadBitmap("cmake", m_toolbarIconSize), FileUtil::TypeCMake);
            AddBitmap(LoadBitmap("qt", m_toolbarIconSize), FileUtil::TypeQMake);
            AddBitmap(LoadBitmap("docker", m_toolbarIconSize), FileUtil::TypeDockerfile);
            AddBitmap(LoadBitmap("yml", m_toolbarIconSize), FileUtil::TypeYAML);
            AddBitmap(LoadBitmap("database", m_toolbarIconSize), FileUtil::TypeDatabase);
            AddBitmap(LoadBitmap("mime-svg", m_toolbarIconSize), FileUtil::TypeSvg);
            AddBitmap(LoadBitmap("workspace-folder-opened", m_toolbarIconSize), FileUtil::TypeWorkspaceFolderExpanded);
            AddBitmap(LoadBitmap("workspace-folder", m_toolbarIconSize), FileUtil::TypeWorkspaceFolder);

            // Non mime bitmaps
            AddBitmap(LoadBitmap("file_save", m_toolbarIconSize), kSave);
            AddBitmap(LoadBitmap("file_save_all", m_toolbarIconSize), kSaveAll);
            AddBitmap(LoadBitmap("file_close", m_toolbarIconSize), kClose);
            AddBitmap(LoadBitmap("db-table", m_toolbarIconSize), kTable);
            AddBitmap(LoadBitmap("cscope", m_toolbarIconSize), kFind);
            AddBitmap(LoadBitmap("database", m_toolbarIconSize), kDatabase);
            AddBitmap(LoadBitmap("db-column", m_toolbarIconSize), kColumn);
            AddBitmap(LoadBitmap("globals", m_toolbarIconSize), kAngleBrackets);
            AddBitmap(LoadBitmap("cc/16/namespace", m_toolbarIconSize), kNamespace);
            AddBitmap(LoadBitmap("cc/16/class", m_toolbarIconSize), kClass);
            AddBitmap(LoadBitmap("cc/16/struct", m_toolbarIconSize), kStruct);
            AddBitmap(LoadBitmap("cc/16/function_public", m_toolbarIconSize), kFunctionPublic);
            AddBitmap(LoadBitmap("cc/16/function_protected", m_toolbarIconSize), kFunctionProtected);
            AddBitmap(LoadBitmap("cc/16/function_private", m_toolbarIconSize), kFunctionPrivate);
            AddBitmap(LoadBitmap("cc/16/member_public", m_toolbarIconSize), kMemberPublic);
            AddBitmap(LoadBitmap("cc/16/member_protected", m_toolbarIconSize), kMemberProtected);
            AddBitmap(LoadBitmap("cc/16/member_private", m_toolbarIconSize), kMemberPrivate);
            AddBitmap(LoadBitmap("cc/16/typedef", m_toolbarIconSize), kTypedef);
            AddBitmap(LoadBitmap("cc/16/macro", m_toolbarIconSize), kMacro);
            AddBitmap(LoadBitmap("cc/16/enum", m_toolbarIconSize), kEnum);
            AddBitmap(LoadBitmap("cc/16/enum", m_toolbarIconSize), kCEnum);
            AddBitmap(LoadBitmap("cc/16/macro", m_toolbarIconSize), kConstant);
            AddBitmap(LoadBitmap("cc/16/enumerator", m_toolbarIconSize), kEnumerator);
            AddBitmap(LoadBitmap("cc/16/cpp_keyword", m_toolbarIconSize), kCxxKeyword);
            AddBitmap(LoadBitmap("sort", m_toolbarIconSize), kSort);
        }
    }
    //---------------------------------------------------------------------------
    int ImageManager::AddResMap(const wxBitmap & bitmap)
    {
        int re = ++mCurrentCustom;
        mToolbarCustomImage.insert({ re, bitmap });
        return re;
    }
    //---------------------------------------------------------------------------
    int ImageManager::AddInterMap(const wxBitmap & bitmap)
    {
        int re = ++mCurrentInter;
        mToolbarInterImage.insert({ re, &bitmap });
        return re;
    }
    //---------------------------------------------------------------------------
    const wxBitmap & ImageManager::GetBitmap(const wxString & filename) const
    {
        FileUtil::FileType ft = FileUtil::GetType(filename, FileUtil::TypeText);

        int index = GetIndex(ft);
        if (index == wxNOT_FOUND)
        {
            return gsgsEmptyBitmap;
        }
        return mImageList.at(index);
    }
    //---------------------------------------------------------------------------
    const wxBitmap & ImageManager::GetBitmap(int type)
    {
        if (mFileIndexMap.count(type) != 0)
        {
            return mImageList.at(mFileIndexMap.at(type));
        }
        else if (mToolbarCustomImage.count(type) != 0)
        {
            return mToolbarCustomImage.at(type);
        }
        else if (mToolbarInterImage.count(type) != 0)
        {
            return *mToolbarInterImage.at(type);
        }
        return gsgsEmptyBitmap;
    }
    //---------------------------------------------------------------------------
    int ImageManager::GetIndex(int type) const
    {
        if (mFileIndexMap.count(type) == 0)
        {
            return wxNOT_FOUND;
        }
        return mFileIndexMap.at(type);
    }
    //---------------------------------------------------------------------------
    int ImageManager::GetIndex(const wxString& filename) const
    {
        FileUtil::Init();
        int ft = FileUtil::GetType(filename, FileUtil::TypeText);
        return GetIndex(ft);
    }
    //---------------------------------------------------------------------------
    void ImageManager::AddBitmap(const wxBitmap& bitmap, int type)
    {
        mImageList.push_back(bitmap);
        mFileIndexMap.insert({ type, mImageList.size() - 1 });
    }
    //---------------------------------------------------------------------------
    void ImageManager::Clear()
    {
        mImageList.clear();
        mFileIndexMap.clear();
    }
    //---------------------------------------------------------------------------
}