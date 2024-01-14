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
#include "NiiUIScheme.h"
#include "NiiUIManager.h"
#include "NiiUIWidgetManager.h"
#include "NiiUISchemeXmlAnalyze.h"
#include "NiiUIException.h"
#include "NiiUISchemeManager.h"
#include "NiiPixelManager.h"
#include "NiiFontManager.h"
#include "NiiXmlPattern.h"
#include "NiiDynamicModule.h"

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    GroupID Scheme::mResourceGroup;
    //------------------------------------------------------------------------
    Scheme::Scheme(SchemeID sid) :
        mName(sid)
    {
    }
    //------------------------------------------------------------------------
    Scheme::~Scheme(void)
    {
        unloadResources();

        Ntchar addr_buff[32];
        Nsntprintf(addr_buff, sizeof(addr_buff), _T("(%p)"), static_cast<void *>(this));
        N_Only(Log).log(_I18n("GUI scheme '") + N_conv(mName) + _I18n("' has been ")
            _I18n("unloaded (object destructor). ") + addr_buff);
    }
    //------------------------------------------------------------------------
    void Scheme::loadResources(void)
    {
        N_Only(Log).log(_I18n("---- Begining resource loading for GUI scheme '") +  N_conv(mName) + _T("' ----"));

        FileList::iterator i, iend = mPixelList.end();
        for(i = mPixelList.begin(); i != iend; ++i)
        {
            N_Only(Pixel).create((*i).mFile, (*i).mGroup);
        }
        loadPixelGroup();
        loadFont();
        loadStyle();
        loadWidgetViewModule();
        loadWidgetModule();
        loadAliasFactory();
        loadStyleView();

        N_Only(Log).log(_I18n("---- Resource loading for GUI scheme '") + N_conv(mName) + _I18n("' completed ----"));
    }
    //------------------------------------------------------------------------
    void Scheme::unloadResources(void)
    {
        N_Only(Log).log(_I18n("---- Begining resource cleanup for GUI scheme '") + N_conv(mName) + _T("' ----"));

        unloadFont();
        //unloadXMLImagesets(); FIXME: ????
        unloadPixelGroup();
        unloadWidgetModule();
        unloadWidgetViewModule();
        unloadAliasFactory();
        unloadStyle();
        unloadStyleView();

        N_Only(Log).log(_I18n("---- Resource cleanup for GUI scheme '") + N_conv(mName) + _I18n("' completed ----"));
    }
    //------------------------------------------------------------------------
    bool Scheme::resourcesLoaded(void) const
    {
        // test state of all loadable resources for this scheme.
        if (//areXMLImagesetsLoaded() && FIXME: ????
            isPixelGroupLoad() &&
            isFontLoad() &&
            isWidgetViewModuleLoad() &&
            isWidgetModuleLoad() &&
            isAliasLoad() &&
            isStyleDefineLoad())
        {
            return true;
        }

        return false;
    }
    //------------------------------------------------------------------------
    void Scheme::loadPixelGroup()
    {
        PixelManager * pbm = N_OnlyPtr(PixelBuffer);

        FileList::iterator i, iend = mPixelGroupList.end();
        for (i = mPixelGroupList.begin(); i != iend; ++i)
        {
            if(!pbm->isExist((*i).mRID))
                pbm->create((*i).mRID, (*i).mFile, (*i).mGroup);
        }
    }
    //------------------------------------------------------------------------
    void Scheme::loadFont()
    {
        FontManager * fntmgr = N_OnlyPtr(Font);

        for(FileList::iterator pos = mFontList.begin(); pos != mFontList.end(); ++pos)
        {
            // skip if a font with this name is already loaded
            if ((*pos).mRID != 0 && fntmgr->isExist((*pos).mRID))
                continue;

            // create font using specified xml file.
            Font * font = fntmgr->create((*pos).mFile, (*pos).mGroup);
            FontID realname = font->getID();

            // if name was not in scheme, set it now and proceed to next font
            if ((*pos).mRID == 0)
            {
                (*pos).mRID = realname;
                continue;
            }

            // confirm the font loaded has same name specified in scheme
            if (realname != (*pos).mRID)
            {
                fntmgr->destroyFont(font->getID());
            }
        }
    }
    //------------------------------------------------------------------------
    void Scheme::loadStyle()
    {
        WidgetManager * wlfMgr = N_OnlyPtr(Widget);

        for(FileList::const_iterator pos = mStyleList.begin(); pos != mStyleList.end(); ++pos)
        {
            wlfMgr->loadStyle((*pos).mFile, (*pos).mGroup);
        }
    }
    //------------------------------------------------------------------------
    void Scheme::loadWidgetModule()
    {
        for (UIModuleList::iterator cmod = mWidgetModuleList.begin(); cmod != mWidgetModuleList.end(); ++cmod)
        {

        }
    }
    //------------------------------------------------------------------------
    void Scheme::loadWidgetViewModule()
    {
        UIModuleList::iterator cmod, cmodend = mWidgetViewModuleList.end();
        for(cmod = mWidgetViewModuleList.begin(); cmod != cmodend; ++cmod)
        {
        }
    }
    //------------------------------------------------------------------------
    void Scheme::loadAliasFactory()
    {
        WidgetManager & wfmgr = N_Only(Widget);
        AliasList::iterator i, iend = mAliasList.end();
        for (i = mAliasList.begin(); i != iend; ++i)
        {

        }
    }
    //------------------------------------------------------------------------
    void Scheme::loadStyleView()
    {
        WidgetManager & wfmgr = N_Only(Widget);
        
        for(StyleDefineList::iterator sdef = mStyleDefineList.begin();
            sdef != mStyleDefineList.end(); ++sdef)
        {
            WidgetManager::StyleViewList::const_iterator i, iend = wfmgr.getStyleViewList().end();
            for(i = wfmgr.getStyleViewList().begin(); i != iend; ++i)
            {
                if(i->first != (*sdef).mID)
                    continue;

                // check if the current target and looks and window renderer match
                if((i->second.mView == (*sdef).mViewID) && (i->second.mStyle == (*sdef).mStyleID))
                {
                    // assume this mapping is ours and skip to next
                    break;
                }
                // create a new mapping entry
                wfmgr.addStyleView((*sdef).mID, (*sdef).mModelID, (*sdef).mViewID, (*sdef).mStyleID, (*sdef).mEffectID,
                    (*sdef).mName);
            }
        }
    }
    //------------------------------------------------------------------------
     /*void Scheme::unloadXMLImagesets()
    {
    FIXME ?
        PixelManager * imgr = N_OnlyPtr(PixelBuffer);

        // unload all xml based Imagesets
        for(FileList::const_iterator pos = mPixelList.begin();
            pos != mPixelList.end(); ++pos)
        {
            if (!(*pos).name.empty())
                ismgr->destroy((*pos).name);
        }
    
    }*/
    //------------------------------------------------------------------------
    void Scheme::unloadPixelGroup()
    {
        PixelManager * mag = N_OnlyPtr(PixelBuffer);
        FileList::const_iterator i, iend = mPixelGroupList.end();
        for(i = mPixelGroupList.begin(); i != iend; ++i)
        {
            if ((*i).mRID != 0)
                mag->destroy((*i).mRID, true);
        }
    }
    //------------------------------------------------------------------------
    void Scheme::unloadFont()
    {
        FontManager * mag = N_OnlyPtr(Font);
        FileList::const_iterator i, iend = mFontList.end();
        for (i = mFontList.begin(); i != iend; ++i)
        {
            if ((*i).mRID != 0)
                mag->destroy((*i).mRID);
        }
    }
    //------------------------------------------------------------------------
    void Scheme::unloadStyle()
    {
        // While it's possible to remove WidgetLooks etc, currently we can't
        // extract which looks were loaded from a given xml file, so we just
        // do nothing for now.
    }
    //------------------------------------------------------------------------
    void Scheme::unloadWidgetModule()
    {
        UIModuleList::iterator i, iend = mWidgetModuleList.end();
        for (i = mWidgetModuleList.begin(); i != iend; ++i)
        {
            if ((*i).mModule)
            {
                N_delete (*i).mModule;
                (*i).mModule = 0;
            }
        }
    }
    //------------------------------------------------------------------------
    void Scheme::unloadWidgetViewModule()
    {
        UIModuleList::iterator i, iend = mWidgetViewModuleList.end();
        for (i = mWidgetViewModuleList.begin(); i != iend; ++i)
        {
            if ((*i).mModule)
            {
                N_delete (*i).mModule;
                (*i).mModule = 0;
            }
        }
    }
    //------------------------------------------------------------------------
    void Scheme::unloadAliasFactory()
    {
        WidgetManager& wfmgr = N_Only(Widget);
        AliasList::iterator i, iend = mAliasList.end();
        for (i = mAliasList.begin(); i != iend; ++i)
        {
        }
    }
    //------------------------------------------------------------------------
    void Scheme::unloadStyleView()
    {
        WidgetManager & wfmgr = N_Only(Widget);

        for(StyleDefineList::iterator j = mStyleDefineList.begin(); j != mStyleDefineList.end(); ++j)
        {
            WidgetManager::StyleViewList::const_iterator i, iend = wfmgr.getStyleViewList().end();
            for(i = wfmgr.getStyleViewList().begin(); i != iend; ++ i)
            {
                if(i->first != (*j).mID)
                    continue;

                // if the current target and looks match
                if((i->second.mView == (*j).mViewID) && (i->second.mStyle == (*j).mStyleID))
                {
                    wfmgr.removeStyleView((*j).mStyleID);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    /*bool Scheme::areXMLImagesetsLoaded() const
    {
     FIXME ?
        PixelManager& imgr = N_Only(Pixel);

        // check imagesets
        FileList::const_iterator i, iend = mPixelList.end();
        for (i = mPixelList.begin(); i != iend; ++i)
        {
            if ((*i).mRID.empty() || !ismgr.isExist((*i).mRID))
                return false;
        }
    
        return true;
    }*/
    //------------------------------------------------------------------------
    bool Scheme::isPixelGroupLoad() const
    {
        PixelManager * pbm = N_OnlyPtr(PixelBuffer);

        FileList::const_iterator i, iend = mPixelGroupList.end();
        for(i = mPixelGroupList.begin(); i != iend; ++i)
        {
            if ((*i).mRID == 0 || !pbm->isExist((*i).mRID))
                return false;
        }

        return true;
    }
    //------------------------------------------------------------------------
    bool Scheme::isFontLoad() const
    {
        FontManager * fntmgr = N_OnlyPtr(Font);

        FileList::const_iterator i, iend = mFontList.end();
        for(i = mFontList.begin(); i != iend; ++i)
        {
            if ((*i).mRID == 0 || !fntmgr->isExist((*i).mRID))
                return false;
        }

        return true;
    }
    //------------------------------------------------------------------------
    bool Scheme::isStyleLoad() const
    {
        // can't check these at the moment, just say yes!
        return true;
    }
    //------------------------------------------------------------------------
    bool Scheme::isWidgetModuleLoad() const
    {
        WidgetManager& wfmgr = N_Only(Widget);

        // check factory modules
        for (UIModuleList::const_iterator cmod = mWidgetModuleList.begin();
            cmod != mWidgetModuleList.end(); ++cmod)
        {
            // see if we should just test all factories available in the
            // module (i.e. No factories explicitly specified)
            if ((*cmod).mTypeList.size() == 0)
            {
                // TODO: This is not supported yet!
            }
            // check all window factories explicitly registered for this module
            else
            {
                TypeList::const_iterator elem = (*cmod).mTypeList.begin();
                for(; elem != (*cmod).mTypeList.end(); ++elem)
                {
                    if(!wfmgr.isViewFactoryExist(*elem))
                        return false;
                }
            }
        }

        return true;
    }
    //------------------------------------------------------------------------
    bool Scheme::isWidgetViewModuleLoad() const
    {
        WidgetManager & wfmgr = N_Only(Widget);

        // check factory modules
        for (UIModuleList::const_iterator cmod = mWidgetViewModuleList.begin();
            cmod != mWidgetViewModuleList.end(); ++cmod)
        {
            // see if we should just test all factories available in the
            // module (i.e. No factories explicitly specified)
            if ((*cmod).mTypeList.size() == 0)
            {
                // TODO: This is not supported yet!
            }
            // check all window factories explicitly registered for this module
            else
            {
                TypeList::const_iterator elem = (*cmod).mTypeList.begin();
                for(; elem != (*cmod).mTypeList.end(); ++elem)
                    if(!wfmgr.isViewFactoryExist(*elem))
                        return false;
            }
        }

        return true;
    }
    //------------------------------------------------------------------------
    bool Scheme::isAliasLoad() const
    {
        WidgetManager & wfmgr = N_Only(Widget);

        for(AliasList::const_iterator alias = mAliasList.begin();
            alias != mAliasList.end(); ++alias)
        {
        }

        return true;
    }
    //------------------------------------------------------------------------
    bool Scheme::isStyleDefineLoad() const
    {
        WidgetManager & wfmgr = N_Only(Widget);

        for(StyleDefineList::const_iterator sdef = mStyleDefineList.begin();
            sdef != mStyleDefineList.end(); ++sdef)
        {
            WidgetManager::StyleViewList::const_iterator i, iend = wfmgr.getStyleViewList().end();
            for(i = wfmgr.getStyleViewList().begin(); i != iend; ++i)
            {
                if(i->first != (*sdef).mID)
                    continue;

                // if the current target, effect and looks match
                if((i->second.mView == (*sdef).mViewID) &&
                    (i->second.mStyle == (*sdef).mStyleID) &&
                        (i->second.mEffect == (*sdef).mEffectID))
                {
                    // assume this mapping is ours and skip to next
                    break;
                }
                return false;
            }
        }
        return true;
    }
    //------------------------------------------------------------------------
}
}