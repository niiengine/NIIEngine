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
#include "NiiLogManager.h"
#include "NiiUISchemeXmlAnalyze.h"
#include "NiiUIScheme.h"
#include "NiiXmlPattern.h"
#include "NiiXmlAnalyzeList.h"

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    SchemeXmlAnalyze::SchemeXmlAnalyze(LangID lid):
        XmlAnalyze(5, N_Script_UIScheme, _T("UIDefine.xsd"), lid),
        mScheme(0)
    {
        setBeginPrc(N_PropertyUIScheme_WidgetAlias,     (PrcCB)&SchemeXmlAnalyze::PrcWidgetAliasCB);    // WindowAlias
        setBeginPrc(N_PropertyUIScheme_ViewUnit,        (PrcCB)&SchemeXmlAnalyze::PrcViewCB);           // View
        setBeginPrc(N_PropertyUIScheme_PixelGroup,      (PrcCB)&SchemeXmlAnalyze::PrcPixelGroupCB);     // ImagesetFromImage
        setBeginPrc(N_PropertyUIScheme_FontUnit,        (PrcCB)&SchemeXmlAnalyze::PrcFontCB);           // Font
        setBeginPrc(N_PropertyUIScheme_WidgetModule,    (PrcCB)&SchemeXmlAnalyze::PrcWidgetSrcCB);      // WidgetSrc
        setBeginPrc(N_PropertyUIScheme_WidgetFactory,   (PrcCB)&SchemeXmlAnalyze::PrcWidgetFactoryCB);  // WidgetFactory
        setBeginPrc(N_PropertyUIScheme_WidgetViewModule,(PrcCB)&SchemeXmlAnalyze::PrcWidgetViewModuleCB);// WidgetRenderSrc
        setBeginPrc(N_PropertyUIScheme_WidgetViewFactory,(PrcCB)&SchemeXmlAnalyze::PrcWidgetViewFactory);// WidgetModelFactory
        setBeginPrc(N_PropertyUIScheme_WidgetView,      (PrcCB)&SchemeXmlAnalyze::PrcWidgetViewCB);     // WidgetView
        setBeginPrc(N_PropertyUIScheme_Style,           (PrcCB)&SchemeXmlAnalyze::PrcStyleCB);          // Style
    }
    //------------------------------------------------------------------------
    SchemeXmlAnalyze::~SchemeXmlAnalyze()
    {
    }
    //------------------------------------------------------------------------
    const String & SchemeXmlAnalyze::getXSD() const
    {
        return mXSD;
    }
    //------------------------------------------------------------------------
    XmlUnitID SchemeXmlAnalyze::getUnit() const
    {
        return N_XML_TAG_UIScheme;
    }
    //------------------------------------------------------------------------
    GroupID SchemeXmlAnalyze::getGroup() const
    {
        return Scheme::getGroup();
    }
    //------------------------------------------------------------------------
    void SchemeXmlAnalyze::PrcTagUnitCB(const PropertyData * pdc)
    {
        Nui32 name;
        pdc->get(IDProperty, name);
        N_Only(Log).log(_I18n("Started creation of Scheme from XML specification:"));
        N_Only(Log).log(_I18n("---- UIDefine name: ") + N_conv(name));

        Nui32 version;
        pdc->get(VersionProperty, version);
        if(version != mVersion)
            N_EXCEPT(InvalidRequest, _I18n("UIÐòÁÐ°æ±¾´íÎó! "));

        // create empty scheme with desired name
        mScheme = N_new Scheme(name);
    }
    //------------------------------------------------------------------------
    void SchemeXmlAnalyze::PrcViewCB(const PropertyData * pdc)
    {
        Scheme::FileInfo imageset;

        pdc->get(NameProperty, imageset.mRID);
        pdc->get(SourceProperty, imageset.mFile);
        pdc->get(GroupProperty, imageset.mGroup);

        mScheme->mPixelList.push_back(imageset);
    }
    //------------------------------------------------------------------------
    void SchemeXmlAnalyze::PrcPixelGroupCB(const PropertyData * pdc)
    {
        Scheme::FileInfo imageset;

        pdc->get(SourceProperty, imageset.mFile);
        pdc->get(NameProperty, imageset.mRID);
        pdc->get(GroupProperty, imageset.mGroup);

        mScheme->mPixelGroupList.push_back(imageset);
    }
    //------------------------------------------------------------------------
    void SchemeXmlAnalyze::PrcFontCB(const PropertyData * pdc)
    {
        Scheme::FileInfo font;

        pdc->get(NameProperty, font.mRID);
        pdc->get(SourceProperty, font.mFile);
        pdc->get(GroupProperty, font.mGroup);

        mScheme->mFontList.push_back(font);
    }
    //------------------------------------------------------------------------
    void SchemeXmlAnalyze::PrcWidgetSrcCB(const PropertyData * pdc)
    {
        Scheme::UIModule module;
        module.mName = pdc->get(SourceProperty);
        module.mModule = 0;

        mScheme->mWidgetModuleList.push_back(module);
    }
    //------------------------------------------------------------------------
    void SchemeXmlAnalyze::PrcWidgetFactoryCB(const PropertyData * pdc)
    {
        Nui32 temp;
        pdc->get(NameProperty, temp);
        mScheme->mWidgetModuleList[mScheme->mWidgetModuleList.size() - 1].mTypeList.push_back(temp);
    }
    //------------------------------------------------------------------------
    void SchemeXmlAnalyze::PrcWidgetViewModuleCB(const PropertyData * pdc)
    {
        Scheme::UIModule module;
        module.mName = pdc->get(SourceProperty);
        module.mModule = 0;

        mScheme->mWidgetViewModuleList.push_back(module);
    }
    //------------------------------------------------------------------------
    void SchemeXmlAnalyze::PrcWidgetViewFactory(const PropertyData * pdc)
    {
        Nui32 temp;
        pdc->get(NameProperty, temp);
        mScheme->mWidgetViewModuleList[mScheme->mWidgetViewModuleList.size() - 1].mTypeList.push_back(temp);
    }
    //------------------------------------------------------------------------
    void SchemeXmlAnalyze::PrcWidgetAliasCB(const PropertyData * pdc)
    {
        Scheme::AliasMapping alias;

        pdc->get(N_PropertyUIScheme_WidgetAliasName, alias.mName);          // alias
        pdc->get(N_PropertyUIScheme_WidgetAliasTarget, alias.mTargetID);     // target
        mScheme->mAliasList.push_back(alias);
    }
    //------------------------------------------------------------------------
    void SchemeXmlAnalyze::PrcWidgetViewCB(const PropertyData * pdc)
    {
        Scheme::StyleDefine fmap;
        pdc->get(IDProperty, fmap.mID);
        pdc->get(NameProperty, fmap.mName);
        pdc->get(GroupProperty, fmap.mGroupID);
        pdc->get(N_PropertyUIScheme_WidgetViewModelID, fmap.mModelID);
        pdc->get(N_PropertyUIScheme_WidgetViewStyleID, fmap.mStyleID);      // type
        pdc->get(N_PropertyUIScheme_WidgetViewID, fmap.mViewID);            // renderer
        pdc->get(N_PropertyUIScheme_WidgetViewEffectID, fmap.mEffectID);   // renderEffect
        mScheme->mStyleDefineList.push_back(fmap);
    }
    //------------------------------------------------------------------------
    void SchemeXmlAnalyze::PrcStyleCB(const PropertyData * pdc)
    {
        Scheme::FileInfo lnf;
        pdc->get(SourceProperty, lnf.mFile);
        pdc->get(GroupProperty, lnf.mGroup);

        mScheme->mStyleList.push_back(lnf);
    }
    //------------------------------------------------------------------------
}
}