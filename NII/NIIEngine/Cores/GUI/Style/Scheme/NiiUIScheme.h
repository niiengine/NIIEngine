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

#ifndef _NII_UI_Scheme_H_
#define _NII_UI_Scheme_H_

#include "NiiUIPreInclude.h"

namespace NII
{
namespace UI
{
    /** UI方案
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Scheme : public UIAlloc
    {
        friend class SchemeXmlAnalyze;
    public:
        struct FileInfo
        {
            ResourceID mRID;
            String mFile;
            GroupID mGroup;
        };
        
        typedef vector<FileInfo>::type FileList;
    public:
        ~Scheme();

        inline SchemeID getID() const               {  return mName;  }

        static inline void setGroup(GroupID gid)    {  mResourceGroup = gid;  }

        static inline GroupID getGroup()            {  return mResourceGroup;  }

        void loadResources();

        void unloadResources();

        bool resourcesLoaded() const;

        bool isPixelGroupLoad() const;

        bool isFontLoad() const;

        bool isStyleLoad() const;

        bool isWidgetModuleLoad() const;

        bool isWidgetViewModuleLoad() const;

        bool isAliasLoad() const;

        bool isStyleDefineLoad() const;

        const FileList & getPixelList() const           { return mPixelList; }

        const FileList & getPixelGroupList() const      { return mPixelGroupList; }

        const FileList & getFontList() const            { return mFontList; }

        const FileList & getStyleList() const           { return mStyleList; }
    protected:
        Scheme(SchemeID sid);
        
        void loadPixelGroup();

        void loadFont();

        void loadStyle();

        void loadWidgetModule();

        void loadWidgetViewModule();

        void loadAliasFactory();

        void loadStyleView();

        void unloadPixelGroup();

        void unloadFont();

        void unloadStyle();

        void unloadWidgetModule();

        void unloadWidgetViewModule();

        void unloadAliasFactory();

        void unloadStyleView();
    private:
        typedef vector<FactoryID>::type TypeList;
        struct UIModule
        {
            String mName;
            DynamicModule * mModule;
            TypeList mTypeList;
        };

        struct AliasMapping
        {
            String mName;
            FactoryID mTargetID;
        };

        struct StyleDefine
        {
            GroupID mGroupID;
            StyleViewID mID;
            FactoryID mModelID;
            WidgetViewlID mViewID;
            StyleID mStyleID;
            EffectID mEffectID;
            String mName;
        };

        typedef vector<UIModule>::type UIModuleList;
        typedef vector<AliasMapping>::type AliasList;
        typedef vector<StyleDefine>::type StyleDefineList;

        SchemeID mName;

        FileList mPixelList;
        FileList mPixelGroupList;
        FileList mFontList;
        FileList mStyleList;

        UIModuleList mWidgetModuleList;
        UIModuleList mWidgetViewModuleList;
        AliasList mAliasList;

        StyleDefineList mStyleDefineList;

        static GroupID mResourceGroup;
    };
}
}
#endif