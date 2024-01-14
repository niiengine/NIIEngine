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
#ifndef gsgs_IMAGE_MANAGER_H_
#define gsgs_IMAGE_MANAGER_H_

#include "gsgsPreInclude.h"
#include "gsgsFileUtil.h"
#include <wx/bitmap.h>
#include <wx/imaglist.h>

#ifndef __WXMSW__
namespace std
{
template <> struct hash<FileUtil::FileType> {
    std::size_t operator()(const FileUtil::FileType& t) const { return hash<int>{}((int)t); }
};
} // namespace std
#endif
namespace gsgs
{
    class _gsgsAPI ImageManager
    {
    public:
        enum eBitmapId {
            kClass = 1000,
            kStruct = 1001,
            kNamespace = 1002,
            kTypedef = 1003,
            kMemberPrivate = 1004,
            kMemberProtected = 1005,
            kMemberPublic = 1006,
            kFunctionPrivate = 1007,
            kFunctionProtected = 1008,
            kFunctionPublic = 1009,
            kEnum = 1010,
            kCEnum = 1011,
            kEnumerator = 1012,
            kConstant = 1013,
            kMacro = 1014,
            kCxxKeyword = 1015,
            kClose = 1016,
            kSave = 1017,
            kSaveAll = 1018,
            kTable = 1019,
            kDatabase = 1020,
            kColumn = 1021,
            kFind = 1022,
            kAngleBrackets = 1023,
            kSort = 1024,

            bIID = 10000,
            bCID = 100000,
        };
    public:
        ImageManager();

        ~ImageManager();

        static ImageManager & singleton();
        static void Release();

        inline bool IsEmpty() const                     { return mImageList.empty(); }

        const wxBitmap & LoadBitmap(const wxString & name, int size = 16);

        inline BitmapList & GetBitmaps()                { return mImageList; }

        int AddResMap(const wxBitmap & bitmap);

        int AddInterMap(const wxBitmap & bitmap);

        const wxBitmap & GetBitmap(const wxString & filename) const;

        const wxBitmap & GetBitmap(int type);

        /**
         * @brief return the image index in the image list prepared by GetBitmaps()
         * @return wxNOT_FOUND if no match is found, the index otherwise
         */
        int GetMimeImageId(const wxString & filename)   { return GetIndex(filename); }

        /**
         * @brief return the image index in the image list prepared by GetBitmaps()
         * @return wxNOT_FOUND if no match is found, the index otherwise
         */
        int GetMimeImageId(int type)                    { return GetIndex(type); }
    protected:
        void initialize();

        wxBitmap LoadBitmapImpl(const wxString & filepath);

        void AddBitmap(const wxBitmap & bitmap, int type);

        int GetIndex(int type) const;

        int GetIndex(const wxString & filename) const;

        void Clear();
    protected:
        static std::unordered_map<wxString, wxBitmap> mToolbarImage;
        static std::unordered_map<wxString, wxString> mManifestImage;
        BitmapList mImageList;
        wxFileName m_zipPath;
        std::unordered_map<int, int> mFileIndexMap;
        std::unordered_map<int, wxBitmap> mToolbarCustomImage;
        std::unordered_map<int, const wxBitmap *> mToolbarInterImage;
        size_t m_toolbarIconSize;
        int mCurrentCustom;
        int mCurrentInter;
        bool m_bMapPopulated;
        static ImageManager * gInstance;
    };
}
#define gsgs_Image() gsgs::ImageManager::singleton()
#endif // BITMAP_LOADER_H
