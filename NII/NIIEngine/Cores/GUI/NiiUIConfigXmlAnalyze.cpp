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
#include "NiiUIConfigXmlAnalyze.h"
#include "NiiUIManager.h"
#include "NiiUIWidgetManager.h"
#include "NiiUISchemeManager.h"
#include "NiiUITooltip.h"
#include "NiiScriptModule.h"
#include "NiiFontManager.h"
#include "NiiPixelManager.h"
#include "NiiScriptModule.h"
#include "NiiXmlPattern.h"
#include "NiiXMLAnalyzeList.h"
#include "NiiEngine.h"

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    ConfigXmlAnalyze::ResourceType stringToResourceType(const String & type)
    {
        if(type == _T("View"))
            return ConfigXmlAnalyze::RT_PixelGroup;
        else if(type == _T("Font"))
            return ConfigXmlAnalyze::RT_Font;
        else if(type == _T("Scheme"))
            return ConfigXmlAnalyze::RT_Scheme;
        else if(type == _T("Style"))
            return ConfigXmlAnalyze::RT_Style;
        else if(type == _T("Layout"))
            return ConfigXmlAnalyze::RT_Layout;
        else if(type == _T("Script"))
            return ConfigXmlAnalyze::RT_Script;
        else
            return ConfigXmlAnalyze::RT_Other;
    }
    //------------------------------------------------------------------------
    ConfigXmlAnalyze::ConfigXmlAnalyze(LangID lid) :
        XmlAnalyze(0, N_Script_UIConfig, _T("UIConfig.xsd"), lid),
        mLogLevel(LL_Info)
    {
        setBeginPrc(N_PropertyUIConfig_Log,         (PrcCB)&ConfigXmlAnalyze::PrcLogCB);        // Logging
        setBeginPrc(N_PropertyUIConfig_AutoLoadRes, (PrcCB)&ConfigXmlAnalyze::PrcAutoLoadCB);   // AutoLoad
        setBeginPrc(N_PropertyUIConfig_AutoLoadRes, (PrcCB)&ConfigXmlAnalyze::PrcPathCB);       // ResPath
        setBeginPrc(N_PropertyUIConfig_ResGroup,    (PrcCB)&ConfigXmlAnalyze::PrcResGroupCB);   // ResGroup
        setBeginPrc(N_PropertyUIConfig_Script,      (PrcCB)&ConfigXmlAnalyze::PrcScriptCB);     // Scripting
        setBeginPrc(N_PropertyUIConfig_XmlPattern,  (PrcCB)&ConfigXmlAnalyze::PrcXmlPatternCB); // DefaultXMLParser
        setBeginPrc(N_PropertyUIConfig_ImageCodec,  (PrcCB)&ConfigXmlAnalyze::PrcImageCodecCB); // DefaultImageCodec
        setBeginPrc(N_PropertyUIConfig_Font,        (PrcCB)&ConfigXmlAnalyze::PrcDefaultFontCB);// DefaultFont
        setBeginPrc(N_PropertyUIConfig_CursorView,  (PrcCB)&ConfigXmlAnalyze::PrcDefaultCursorCB);  // DefaultMouseCursor
        setBeginPrc(N_PropertyUIConfig_Tooltip,     (PrcCB)&ConfigXmlAnalyze::PrcDefaultTooltipCB);// DefaultTooltip
    }
    //------------------------------------------------------------------------
    ConfigXmlAnalyze::~ConfigXmlAnalyze()
    {
    }
    //------------------------------------------------------------------------
    const String & ConfigXmlAnalyze::getXSD() const
    {
        return mXSD;
    }
    //------------------------------------------------------------------------
    XmlUnitID ConfigXmlAnalyze::getUnit() const
    {
        return N_XML_TAG_UIConfig;
    }
    //------------------------------------------------------------------------
    GroupID ConfigXmlAnalyze::getGroup() const
    {
        return GroupUnknow;
    }
    //------------------------------------------------------------------------
    void ConfigXmlAnalyze::PrcTagUnitCB(const PropertyData *)
    {
    }
    //------------------------------------------------------------------------
    void ConfigXmlAnalyze::PrcLogCB(const PropertyData * pdc)
    {
        pdc->get(SourceProperty, mLogFile, _T(""));

        const String & logLevel(pdc->get(N_PropertyUIConfig_LogLevel)); // level

        if(logLevel == _T("LL_Error"))
            mLogLevel = LL_Error;
        else if(logLevel == _T("LL_Info"))
            mLogLevel = LL_Info;
        else if(logLevel == _T("LL_Trace"))
            mLogLevel = LL_Trace;
        else
            mLogLevel = LL_Info;
    }
    //------------------------------------------------------------------------
    void ConfigXmlAnalyze::PrcAutoLoadCB(const PropertyData * pdc)
    {
        AutoLoadRes ob;
        pdc->get(TypeProperty, ob.mType, _T(""));
        ob.mResourceType = stringToResourceType(ob.mType);
        pdc->get(N_PropertyUIConfig_ResPattern, ob.mFilePattern, _T("*")); // pattern
        pdc->get(GroupProperty, ob.mGroup, GroupUnknow);
        mAutoLoad.push_back(ob);
    }
    //------------------------------------------------------------------------
    void ConfigXmlAnalyze::PrcPathCB(const PropertyData * pdc)
    {
        ResPath ob;
        pdc->get(GroupProperty, ob.mGroup, GroupUnknow);
        pdc->get(N_PropertyUIConfig_ResPath, ob.directory, _T("./")); // directory
        mResPathList.push_back(ob);
    }
    //------------------------------------------------------------------------
    void ConfigXmlAnalyze::PrcResGroupCB(const PropertyData * pdc)
    {
        ResGroup ob;
        ob.mType = stringToResourceType(pdc->get(TypeProperty));
        pdc->get(GroupProperty, ob.mGroup, GroupUnknow);
        mResGroupList.push_back(ob);
    }
    //------------------------------------------------------------------------
    void ConfigXmlAnalyze::PrcScriptCB(const PropertyData * pdc)
    {
        pdc->get(N_PropertyUIConfig_InitExec, mInitExec, _T(""));           // initScript
        pdc->get(N_PropertyUIConfig_TerminateExec, mTerminateExec, _T("")); // terminateScript
    }
    //------------------------------------------------------------------------
    void ConfigXmlAnalyze::PrcXmlPatternCB(const PropertyData * pdc)
    {
        pdc->get(NameProperty, mXmlPattern, _T(""));
    }
    //------------------------------------------------------------------------
    void ConfigXmlAnalyze::PrcImageCodecCB(const PropertyData * pdc)
    {
        pdc->get(NameProperty, mImageCodec, _T(""));
    }
    //------------------------------------------------------------------------
    void ConfigXmlAnalyze::PrcDefaultFontCB(const PropertyData * pdc)
    {
        pdc->get(NameProperty, mFont);
    }
    //------------------------------------------------------------------------
    void ConfigXmlAnalyze::PrcDefaultCursorCB(const PropertyData * pdc)
    {
        pdc->get(N_PropertyUIConfig_CursorImage, mCursorView); // image
    }
    //------------------------------------------------------------------------
    void ConfigXmlAnalyze::PrcDefaultTooltipCB(const PropertyData * pdc)
    {
        pdc->get(IDProperty, mTooltipFactoryID, 0);
    }
    //------------------------------------------------------------------------
    void ConfigXmlAnalyze::initModule() const
    {
        if(!mXmlPattern.empty())
            N_Engine().loadPlugin(mXmlPattern);

        ResGroupList::const_iterator i, iend = mResGroupList.end();
        for(i = mResGroupList.begin(); i != iend; ++i)
        {
            switch((*i).mType)
            {
            case RT_PixelGroup:
                PixelManager::setGroup((*i).mGroup);
                break;
            case RT_Font:
                FontManager::setGroup((*i).mGroup);
                break;
            case RT_Scheme:
                Scheme::setGroup((*i).mGroup);
                break;
            case RT_Style:
                WidgetManager::setStyleGroup((*i).mGroup);
                break;
            case RT_Layout:
                WidgetManager::setGroup((*i).mGroup);
                break;
            case RT_Script:
                ScriptModule::setGroup((*i).mGroup);
                break;
            default:
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    void ConfigXmlAnalyze::loadResource() const
    {
        AutoLoadResList::const_iterator i, iend = mAutoLoad.end();
        for(i = mAutoLoad.begin(); i != iend; ++i)
        {
            switch((*i).mResourceType)
            {
            case RT_PixelGroup:
            {
                StringList names;
                N_Only(ResourceScheme).find((*i).mGroup, names, (*i).mFilePattern);
                StringList::iterator j, jend = names.end();
                for (j = names.begin(); j != jend; ++j)
                {
                    N_Only(Pixel).create(*j, (*i).mGroup);
                }
            }
            break;
            case RT_Font:
                N_Only(Font).createDefine((*i).mFilePattern, (*i).mGroup);
                break;
            case RT_Scheme:
                N_Only(UI::Scheme).createDefine((*i).mFilePattern, (*i).mGroup);
                break;
            case RT_Style:
            {
                StringList names;
                N_Only(ResourceScheme).find((*i).mGroup, names, (*i).mFilePattern);
                StringList::iterator j, jend = names.end();
                for(j = names.begin(); j != jend; ++j)
                    N_Only(Widget).loadStyle(*j, (*i).mGroup);
            }
            break;
            default:
                N_EXCEPT(InvalidRequest, _I18n("AutoLoad of resource type '") +
                    (*i).mType + _I18n("' is not currently supported.Pattern was: ") +
                    (*i).mFilePattern + _I18n(" Group was: ") + N_conv((*i).mGroup));
            }
        }
    }
    //------------------------------------------------------------------------
    void ConfigXmlAnalyze::initResource() const
    {
        if(mFont == 0)
        {
            N_Only(UI).getMainSheet()->setFont(0);
        }
        else
        {
            N_Only(UI).getMainSheet()->setFont(N_Only(Font).getFont(mFont));
        }

        if (mCursorView != 0)
        {
            PixelBuffer * temp = N_Only(Pixel).get(mCursorView);
            N_Only(UI).getMainSheet()->getCursor()->setDefaultImage(temp);
        }

        if(mTooltipFactoryID != 0)
        {
            Tooltip * ttip = N_Only(UI).getMainSheet()->getTooltip();
            if(!ttip && mTooltipFactoryID != 0)
            {
                ttip = dynamic_cast<Tooltip *>(N_OnlyPtr(Widget)->create(mTooltipFactoryID, 0));

                if(ttip)
                {
                    ttip->setAutoWindow(true);
                    ttip->setDataWrite(false);
                    N_Only(UI).getMainSheet()->setTooltip(ttip, true);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void ConfigXmlAnalyze::execInitExec() const
    {
        if(!mInitExec.empty())
            N_Engine().executeScript(N_ScriptModule_UI, mInitExec);
    }
    //------------------------------------------------------------------------
    void ConfigXmlAnalyze::executeTerminateExec() const
    {
        if(!mTerminateExec.empty())
            N_Engine().executeScript(N_ScriptModule_UI, mTerminateExec);
    }
    //------------------------------------------------------------------------
}
}