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
#include "NiiUIWidgetManager.h"
#include "NiiUIManager.h"
#include "NiiUILayoutXmlAnalyze.h"
#include "NiiUIException.h"
#include "NiiRenderEffectManager.h"
#include "NiiUISheet.h"
#include "NiiUIWindow.h"
#include "NiiEngine.h"
#include "NiiXmlPattern.h"
#include "NiiDefaultStream.h"
#include "NiiXmlSerializer.h"
#include "NiiUIStyleXmlAnalyze.h"
#include "NiiResourceSchemeManager.h"
#include "NiiUIPixel.h"
#include <algorithm>

namespace NII
{
    //------------------------------------------------------------------------
    EffectTextView * WidgetManager::gDefaultTextView = 0;
    TextView * WidgetManager::gNormalTextView = 0;
    //------------------------------------------------------------------------
    const Colour WidgetManager::DefaultSelectionColour = 0xFF4444AA;
    const Colour WidgetManager::DefaultTextColour = 0xFFFFFFFF;
    GroupID WidgetManager::StyleResourceGroup = GroupUnknow;
    //------------------------------------------------------------------------
    template<> WidgetManager * Singleton<WidgetManager>::mOnly = 0;
    GroupID WidgetManager::ResourceGroup = GroupGlobal;
    //------------------------------------------------------------------------
    WidgetManager::WidgetManager() :
        mValidWidgetID(N_Max_Valid_ID),
        mValidStyleViewID(N_Max_Valid_ID)
    {
        gDefaultTextView = N_new EffectTextView();
        gNormalTextView = N_new TextView();
    }
    //------------------------------------------------------------------------
    WidgetManager::~WidgetManager()
    {
        destroyAll();
        recover();
    }
    //------------------------------------------------------------------------
    Widget * WidgetManager::create(StyleViewID id, WidgetID wid, Container * parent, FactoryID modelhint)
    {
        wid = (wid == 0 ? genValidWidgetID() : wid);

        Widget * nwidget = 0;
        const WidgetManager::StyleView * fwm = getStyleView(id);
        if(fwm)
        {
            nwidget = getFactory(fwm->mModel)->create(wid, parent);

            if (parent != 0)
                parent->add(nwidget);
            else
                return 0;
            nwidget->mViewID = id;
            nwidget->setWidgetView(fwm->mView);
            if(fwm->mStyle)
                nwidget->setStyle(fwm->mStyle);

            if(fwm->mEffect && !N_Only(RenderEffect).isEffectAvailable(fwm->mEffect))
            {
                Window * tempwin = dynamic_cast<Window *>(nwidget);
                if (tempwin)
                {
                    UISubSheet * tmp = tempwin->getRender();
                    if (tmp)
                    {
                        tmp->setEffect(N_Only(RenderEffect).create(fwm->mEffect, tempwin));
                    }
                    else
                    {
                        tempwin->setAutoRender(true);
                    }
                }
            }
        }

        mCreateWidgets.push_back(nwidget);

        onCreate(nwidget);

        return nwidget;
    }
    //------------------------------------------------------------------------
    Widget * WidgetManager::create(GroupID gid, FactoryID mid, WidgetViewlID vid, WidgetID wid, Container * parent)
    {
        StyleViewList::const_iterator i, iend = mStyleViewList.end();
        for (i = mStyleViewList.begin(); i != iend; ++i)
        {
            const StyleView & it = i->second;
            if(it.mGroup == gid && it.mModel == mid && it.mView == vid)
                return create(i->first, wid, parent, mid);
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void WidgetManager::destroy(Widget * obj)
    {
        Widgets::iterator iter = std::find(mCreateWidgets.begin(), mCreateWidgets.end(), obj);
        if(iter != mCreateWidgets.end())
        {
            mCreateWidgets.erase(iter);
            obj->destroy();
            mDestroyWidgets.push_back(obj);

            onDestroy(obj);
        }
    }
    //------------------------------------------------------------------------
    void WidgetManager::destroyAll()
    {
        Widgets::iterator i, iend = mCreateWidgets.end();
        for(i = mCreateWidgets.begin(); i != iend; ++i)
        {
            destroy(*i);
        }
    }
    //------------------------------------------------------------------------
    bool WidgetManager::isExist(const Widget * obj) const
    {
        Widgets::const_iterator iter = std::find(mCreateWidgets.begin(), mCreateWidgets.end(), obj);

        return iter != mCreateWidgets.end();
    }
    //------------------------------------------------------------------------
    bool WidgetManager::isExist(const String & name, bool path) const
    {
        Widgets::const_iterator it, itend = mCreateWidgets.end();
        for (it = mCreateWidgets.begin(); it != itend; ++it)
        {
            if (path)
            {
                if ((*it)->getNamePath() == name)
                {
                    return true;
                }
            }
            else
            {
                if ((*it)->getName() == name)
                {
                    return true;
                }
            }
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool WidgetManager::isViewFactoryExist(FactoryID fid) const
    {
        return (mWidgetViewList.find(fid) != mWidgetViewList.end());
    }
    //------------------------------------------------------------------------
    WidgetViewFactory * WidgetManager::getViewFactory(FactoryID fid) const
    {
        WidgetViewList::const_iterator i = mWidgetViewList.find(fid);
        if (i != mWidgetViewList.end())
        {
            return (*i).second;
        }
        return 0;
    }
    //------------------------------------------------------------------------
    bool WidgetManager::addViewFactory(WidgetViewFactory * obj)
    {
        N_assert(obj, "error logic");

        if(mWidgetViewList.insert(Npair(obj->getID(), obj)).second == false)
        {
            return false;
        }

        return true;
    }
    //------------------------------------------------------------------------
    void WidgetManager::removeViewFactory(FactoryID fid)
    {
        WidgetViewList::iterator i = mWidgetViewList.find(fid);

        // non-existing or already removed? The latter can happen when more then one Scheme
        // was loaded using the same renderer.
        if(i != mWidgetViewList.end())
        {
            mWidgetViewList.erase(fid);
        }
    }
    //------------------------------------------------------------------------
    void WidgetManager::loadStyle(const String & file, GroupID gid)
    {
        // valid filenames are required!
        if (file.empty())
        {
            N_EXCEPT(InvalidRequest, _I18n("Filename supplied for look & feel file must be valid"));
        }

        StyleXmlAnalyze handler;
        DataStream * data = N_Only(ResourceScheme).open(file, gid == GroupUnknow ? StyleResourceGroup : gid);

        if (data)
        {
            try
            {
                N_Engine().getXmlPattern()->parse(&handler, data);
            }
            catch (...)
            {
                N_Only(Log).log(_I18n("不能从文件中加载风格 '") + file);
                N_delete data;
                throw;
            }
            N_delete data;
        }
    }
    //------------------------------------------------------------------------
    void WidgetManager::loadStyle(const MemStream * data)
    {
        StyleXmlAnalyze handler;

        try
        {
            N_Engine().getXmlPattern()->parse(&handler, data);
        }
        catch (...)
        {
            N_Only(Log).log(_I18n("loading of look and feel data from raw data container has failed."));
            throw;
        }
    }
    //------------------------------------------------------------------------
    bool WidgetManager::isStyleExist(StyleID widget) const
    {
        return mStyleList.find(widget) != mStyleList.end();
    }
    //------------------------------------------------------------------------
    Style * WidgetManager::getStyle(StyleID widget) const
    {
        StyleList::const_iterator style = mStyleList.find(widget);

        if (style != mStyleList.end())
        {
            return style->second;
        }

        N_EXCEPT(UnknownObject, _I18n("WidgetView '") + N_conv(widget) + _I18n("' does not exist."));
    }
    //------------------------------------------------------------------------
    void WidgetManager::removeStyle(StyleID widget)
    {
        StyleList::iterator style = mStyleList.find(widget);
        if (style != mStyleList.end())
        {
            mStyleList.erase(style);
        }
    }
    //------------------------------------------------------------------------
    void WidgetManager::removeAllStyle()
    {
        mStyleList.clear();
    }
    //------------------------------------------------------------------------
    void WidgetManager::addStyle(Style * style)
    {
        if (isStyleExist(style->getID()))
        {
            N_Only(Log).log(_I18n("Widget look and feel '") + N_conv(style->getID()) +
                _I18n("' already exists.  Replacing previous definition."));
        }

        mStyleList[style->getID()] = style;
    }
    //------------------------------------------------------------------------
    void WidgetManager::writeStyle(StyleID style, Nostream & out) const
    {
        StyleXmlAnalyze analyze;
        // output xml header
        XmlSerializer xml(&out, &analyze);
        // output root element
        xml.begin(StyleXmlAnalyze::StyleUnitProperty);
        xml.attribute(StyleXmlAnalyze::VersionProperty, "300000");

        try
        {
            getStyle(style)->write(&xml);
        }
        catch (UnknownObjectException &)
        {
            N_Only(Log).log(_I18n("Failed to write widget look XML data to stream."));
        }

        // close the root tags to terminate the file
        xml.end();
    }
    //------------------------------------------------------------------------
    void WidgetManager::writeStyleList(Nui16 Ser, Nostream & out) const
    {
        XmlSerializer xml(&out);
        // output root element
        xml.begin(N_XML_TAG_UIStyle);
        StyleList::const_iterator i, iend = mStyleList.end();
        for (i = mStyleList.begin(); i != iend; ++i)
        {
            if (i->first << 24 == Ser)
                i->second->write(&xml);
        }

        // close the root tags to terminate the file
        xml.end();
    }
    //------------------------------------------------------------------------
    WidgetView * WidgetManager::createWindow(FactoryID fid)
    {
        return getViewFactory(fid)->create();
    }
    //------------------------------------------------------------------------
    void WidgetManager::destroyView(WidgetView * obj)
    {
        getViewFactory(obj->getID())->destroy(obj);
    }
    //------------------------------------------------------------------------
    bool WidgetManager::addFactory(WidgetFactory * obj)
    {
        if(mWidgetFactoryList.find(obj->getID()) != mWidgetFactoryList.end())
        {
            return false;
        }
        mWidgetFactoryList[obj->getID()] = obj;
        return true;
    }
    //------------------------------------------------------------------------
    void WidgetManager::removeFactory(FactoryID fid)
    {
        WidgetFactoryList::iterator i = mWidgetFactoryList.find(fid);
        if (i != mWidgetFactoryList.end())
        {
            mWidgetFactoryList.erase(fid);
        }
    }
    //------------------------------------------------------------------------
    void WidgetManager::removeAllFactory()
    {
        /*
        WidgetFactoryList::iterator i, iend = mWidgetFactoryList.end();
        for(i = mWidgetFactoryList.begin(); i != iend; ++i)
        {
            //N_delete i->second;
        }*/
        mWidgetFactoryList.clear();
    }
    //------------------------------------------------------------------------
    WidgetFactory * WidgetManager::getFactory(FactoryID fid) const
    {
        WidgetFactoryList::const_iterator pos = mWidgetFactoryList.find(fid);
        if(pos != mWidgetFactoryList.end())
        {
            return pos->second;
        }

        return 0;
    }
    //------------------------------------------------------------------------
    bool WidgetManager::isFactoryExist(FactoryID fid) const
    {
        if(mWidgetFactoryList.find(fid) != mWidgetFactoryList.end())
        {
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    void WidgetManager::addStyleView(StyleViewID id, FactoryID mid, WidgetViewlID vid, StyleID sid,
        EffectID eid, const String & name)
    {
        WidgetManager::StyleView sv;
        sv.mName = name;
        sv.mID = id;
        sv.mModel = mid;
        sv.mStyle = sid;
        sv.mView = vid;
        sv.mEffect = eid;

        mStyleViewList[id] = sv;
    }
    //------------------------------------------------------------------------
    bool WidgetManager::isStyleViewExist(FactoryID mid, WidgetViewlID vid, StyleID sid) const
    {
        StyleViewList::const_iterator i, iend = mStyleViewList.end();
        for (i = mStyleViewList.begin(); i != iend; ++i)
        {
            const StyleView & view = i->second;
            if (view.mModel == mid && view.mStyle == sid && view.mView == vid)
                return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    void WidgetManager::removeStyleView(StyleViewID fid)
    {
        StyleViewList::iterator i = mStyleViewList.find(fid);
        if(i != mStyleViewList.end())
        {
            mStyleViewList.erase(i);
        }
    }
    //------------------------------------------------------------------------
    void WidgetManager::removeAllStyleView()
    {
        mStyleViewList.clear();
    }
    //------------------------------------------------------------------------
    bool WidgetManager::isStyleViewExist(StyleViewID fid) const
    {
        return mStyleViewList.find(fid) != mStyleViewList.end();
    }
    //------------------------------------------------------------------------
    const WidgetManager::StyleView * WidgetManager::getStyleView(FactoryID mid, WidgetViewlID vid, StyleID sid) const
    {
        StyleViewList::const_iterator i, iend = mStyleViewList.end();
        for (i = mStyleViewList.begin(); i != iend; ++i)
        {
            const StyleView & view = i->second;
            if (view.mModel == mid && view.mStyle == sid && view.mView == vid)
                return &view;
        }
        return 0;
    }
    //------------------------------------------------------------------------
    const WidgetManager::StyleView * WidgetManager::getStyleView(StyleViewID fid) const
    {
        StyleViewList::const_iterator iter = mStyleViewList.find(fid);

        if(iter != mStyleViewList.end())
        {
            return &(*iter).second;
        }

        return 0;
    }
    //------------------------------------------------------------------------
    Widget * WidgetManager::load(const String & file, GroupID gid)
    {
        UILayoutXmlAnalyze prc(0, 0);

        DataStream * data = N_Only(ResourceScheme).open(file, gid == GroupUnknow ? ResourceGroup : gid);

        if(data)
        {
            try
            {
                N_Engine().getXmlPattern()->parse(&prc, data);
            }
            catch(...)
            {
                N_Only(Log).log(_T("loading of layout from file '") + file + _I18n("' failed."));
                N_delete data;
                throw;
            }
            N_delete data;
        }
        return prc.getRootWindow();
    }
    //------------------------------------------------------------------------
    Widget * WidgetManager::load(const MemStream * data)
    {
        UILayoutXmlAnalyze prc(0, 0);
        try
        {
            N_Engine().getXmlPattern()->parse(&prc, data);
        }
        catch(...)
        {
            N_Only(Log).log(_I18n("error analyze") + data->getName());
            throw;
        }

        return prc.getRootWindow();
    }
    //------------------------------------------------------------------------
    void WidgetManager::recover()
    {
        Widgets::reverse_iterator curr = mDestroyWidgets.rbegin();
        for(; curr != mDestroyWidgets.rend(); ++curr)
        {
            getFactory((*curr)->getType())->destroy(*curr);
        }

        // all done here, so clear all pointers from dead pool
        mDestroyWidgets.clear();
    }
    //------------------------------------------------------------------------
    void WidgetManager::onCreate(const Widget * obj)
    {
        N_Only(UI).onCreate(obj);
    }
    //------------------------------------------------------------------------
    void WidgetManager::onDestroy(const Widget * obj)
    {
        N_Only(UI).onDestroy(obj);
    }
    //------------------------------------------------------------------------
    void WidgetManager::writeWidget(const Widget * obj, Nostream & out_stream) const
    {
        UILayoutXmlAnalyze analyze;

        XmlSerializer xml(&out_stream, &analyze);

        xml.begin(N_XML_TAG_UILayout);

        xml.attribute(UILayoutXmlAnalyze::VersionProperty, "30000");
        obj->write(&xml);

        xml.end();
    }
    //------------------------------------------------------------------------
    void WidgetManager::writeWidget(const Widget * obj, const String & filename) const
    {
        Nofstream stream;
        stream.open(filename.c_str(), std::ios::out);
        stream.imbue(locale(locale(), "", LC_CTYPE));
        if (stream.good())
        {
            writeWidget(obj, stream);
        }
        stream.close();
    }
    //------------------------------------------------------------------------
    WidgetID WidgetManager::genValidWidgetID()
    {
        return ++mValidWidgetID;
    }
    //------------------------------------------------------------------------
    StyleViewID WidgetManager::genValidStyleViewID()
    {
        return ++mValidStyleViewID;
    }
    //------------------------------------------------------------------------
}