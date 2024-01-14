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
#include "NiiOverlayManager.h"
#include "NiiGeometrySpace.h"
#include "NiiException.h"
#include "NiiViewport.h"
#include "NiiLogManager.h"
#include "NiiStrConv.h"
#include "NiiOverlayGeoFactory.h"
#include "NiiEngine.h"

namespace NII
{
    //---------------------------------------------------------------------
    template<> OverlayManager * Singleton<OverlayManager>::mOnly = 0;
    /*OverlayManager * OverlayManager::getOnlyPtr()
    {
        return mOnly;
    }
    //---------------------------------------------------------------------
    OverlayManager & OverlayManager::getOnly()
    {
        N_assert(mOnly, "error");
        return (*mOnly);
    }*/
    //---------------------------------------------------------------------
    OverlayManager::OverlayManager() :
        mValidID(N_Max_Valid_ID)
    {
        // Scripting is supported by this manager
        mScriptPattern.push_back(_T("*.overlay"));
        N_Only(ResourceScheme).add(this);
    }
    //---------------------------------------------------------------------
    OverlayManager::~OverlayManager()
    {
        destroyAllGeo(false);
        destroyAllGeo(true);
        destroyAll();

        // Unregister with resource group manager
        N_Only(ResourceScheme).remove(this);
    }
    //---------------------------------------------------------------------
    void OverlayManager::render(Viewport * vp, Camera * cam, RenderQueue * queue)
    {
        OverlayList::iterator i = mOverlayList.find(vp->getID());
        if(i != mOverlayList.end())
        {
            OverlayMap::iterator o, oend = i->second.end();
            for(o = i->second.begin(); o != oend; ++o)
            {
                Overlay * olay = o->second;
#if NII_VIEWPORT_DIR_NONE == 0
                if(!vp->isSyncAspect())
                {
                    // trick to trigger transform update of the overlay
                    olay->offset(0.f, 0.f);
                }
#endif
                olay->renderImpl(cam, queue);
            }
            vp->syncAspectImpl();
        }
    }
    //---------------------------------------------------------------------
    Overlay * OverlayManager::create(Nid vpid, OverlayID id)
    {
        Overlay * re = 0;
        OverlayMap * om;
        OverlayList::iterator o = mOverlayList.find(vpid);
        if(o != mOverlayList.end())
        {
            om = &o->second;
        }
        else
        {
            om = &mOverlayList.insert(Npair(vpid, OverlayMap())).first->second;
        }

        OverlayMap::iterator i = om->find(id);
        if(i == om->end())
        {
            if (id == 0)
            {
                N_lock_mutex(mValidIDMutex)
                id = ++mValidID;
            }
            re = N_new Overlay(id, N_EnginePtr()->getRender()->getViewports());
            om->insert(Npair(id, re));
        }
        else
        {
            N_EXCEPT(UniqueRepeat, _I18n("Overlay with id '") + N_conv(id) + _I18n("' 已经存在!"));
        }
        return re;
    }
    //---------------------------------------------------------------------
    Overlay * OverlayManager::get(Nid vpid, OverlayID id)
    {
        OverlayList::iterator o = mOverlayList.find(vpid);
        if(o != mOverlayList.end())
        {
            OverlayMap & om = o->second;
            OverlayMap::iterator i = om.find(id);
            if(i != om.end())
            {
                return i->second;
            }
            else
            {
                return 0;
            }
        }
        return 0;
    }
    //---------------------------------------------------------------------
    void OverlayManager::destroy(Nid vpid, OverlayID id)
    {
        OverlayList::iterator o = mOverlayList.find(vpid);
        if(o != mOverlayList.end())
        {
            OverlayMap & om = o->second;
            OverlayMap::iterator i = om.find(id);
            if(i == om.end())
            {
                N_EXCEPT(NotExist, _I18n("Overlay with name '") + 
                    N_conv(id) + _I18n("' not found."));
            }
            else
            {
                N_delete i->second;
                om.erase(i);
                if(om.empty())
                {
                    mOverlayList.erase(o);
                }
            }
        }
    }
    //---------------------------------------------------------------------
    void OverlayManager::destroyAll(Nid vpid)
    {
        OverlayList::iterator j = mOverlayList.find(vpid);
        if(j != mOverlayList.end())
        {
            OverlayMap & om = j->second;
            OverlayMap::iterator i, iend = om.end();
            for(i = om.begin(); i != iend; ++i)
            {
                N_delete i->second;
            }
            om.clear();
            mOverlayList.erase(j);
        }
    }
    //---------------------------------------------------------------------
    void OverlayManager::destroyAll()
    {
        OverlayList::iterator o, oend = mOverlayList.end();
        for(o = mOverlayList.begin(); o != oend; ++o)
        {
            OverlayMap::iterator i, iend = o->second.end();
            for(i = o->second.begin(); i != iend; ++i)
            {
                N_delete i->second;
            }
            o->second.clear();
        }
        mOverlayList.clear();
        mLoadedScripts.clear();
    }
    //---------------------------------------------------------------------
    const OverlayManager::OverlayMap & OverlayManager::getList(Nid vpid)
    {
        OverlayList::iterator o = mOverlayList.find(vpid);
        if(o != mOverlayList.end())
        {
            return o->second;
        }
        static OverlayMap temp;
        return temp;
    }
    //---------------------------------------------------------------------
    void OverlayManager::getList(OverlayMapList & maplist)
    {
        maplist.clear();
        OverlayList::iterator o, oend = mOverlayList.end();
        for(o = mOverlayList.begin(); o != oend; ++o)
        {
            maplist.push_back(o->second);
        }
    }
    //---------------------------------------------------------------------
    OverlayID OverlayManager::genValidID()
    {
        N_lock_mutex(mValidIDMutex)
        OverlayID temp = ++mValidID;
        return temp;
    }
    //---------------------------------------------------------------------
    void OverlayManager::parse(DataStream * stream, GroupID gid, PtrList & out)
    {
        // check if we've seen this script before (can happen if included
        // multiple times)
        if(!stream->getName().empty() && mLoadedScripts.find(stream->getName()) != mLoadedScripts.end())
        {
            N_Only(Log).log(_I18n("Skipping loading overlay include: '") + 
                stream->getName() + _I18n(" as it is already loaded."));
            return;
        }

        String line;
        Overlay * pOverlay = 0;
        bool skipLine;

        while(!stream->end())
        {
            bool templet = false;
            skipLine = false;
            line = stream->readLine();
            // Ignore comments & blanks
            if(!(line.length() == 0 || line.substr(0,2) == _T("//")))
            {
                if(line.substr(0,8) == _T("#include"))
                {
                    StringList params;
                    StrUtil::split(line, params, _T("\t\n ()<>"));
                    DataStream * includeStream = N_Only(ResourceScheme).open(params[1], gid);
                    parse(includeStream, gid, out);
                    N_delete includeStream;
                    continue;
                }
                if(!pOverlay)
                {
                    // No current overlay
                    // check to see if there is a template
                    if(line.substr(0, 8) == _T("template"))
                    {
                        templet = true;
                    }
                    else
                    {

                        // So first valid data should be overlay name
                        if(StrUtil::beginsWith(line, _T("overlay ")))
                        {
                            // chop off the 'particle_system ' needed by new compilers
                            line = line.substr(8);
                        }
                        Nui32 tempid;
                        StrConv::conv(line, tempid);
                        pOverlay = create(0, tempid);
                        pOverlay->setSrc(stream->getName());
                        // Skip to and over next {
                        stream->skipOpenBrace();
                        skipLine = true;
                        out.push_back(pOverlay);
                    }
                }
                if((pOverlay && !skipLine) || templet)
                {
                    // Already in overlay
                    StringList params;
                    StrUtil::split(line, params, _T("\t\n ()"));

                    if(line == _T("}"))
                    {
                        // Finished overlay
                        pOverlay = 0;
                    }
                    else if(parseGeoProperty(stream, line, pOverlay, NULL, templet))
                    {

                    }
                    else
                    {
                        // Attribute
                        if(!templet)
                        {
                            // Split params on first space
                            StringList vecparams;
                            StrUtil::split(line, vecparams, _T("\t "), 1);
                            Nui32 temp;
                            // Look up first param (command setting)
                            StrUtil::toLower(vecparams[0]);
                            if(vecparams[0] == _T("zorder"))
                            {
                                StrConv::conv(vecparams[1], temp);
                                pOverlay->setZOrder((Nui16)temp);
                            }
                            else
                            {
                                N_Only(Log).log(_I18n("Bad overlay attribute line: '")
                                    + line + _I18n("' for overlay ") + N_conv(pOverlay->getID()));
                            }
                        }
                    }
                }
            }
        }

        // record as parsed
        mLoadedScripts.insert(stream->getName());
    }
    //---------------------------------------------------------------------
    void OverlayManager::parseGeo(DataStream * stream, OverlayGeoType type,
        OverlayGeoID elemName, OverlayGeoID templateName, bool isContainer,
            Overlay * obj, OverlayView * container, bool templet)
    {
        String line;

        OverlayGeo * newe = NULL;
        newe = N_Only(Overlay).createGeo(templateName, type, elemName, templet);

        // do not add a template to an overlay

        // add new element to parent
        if(container)
        {
            // Attach to container
            container->add(newe);
        }
        // do not add a template to the overlay. For templates overlay = 0
        else if(obj)
        {
            obj->add((OverlayView *)newe);
        }

        while(!stream->end())
        {
            line = stream->readLine();
            // Ignore comments & blanks
            if(!(line.length() == 0 || line.substr(0,2) == _T("//")))
            {
                if(line == _T("}"))
                {
                    // Finished element
                    break;
                }
                else
                {
                    if(isContainer && parseGeoProperty(stream, line, obj, static_cast<OverlayView *>(newe), templet))
                    {
                        // nested children... don't reparse it
                    }
                    else
                    {
                        // 属性
                        // Split params on first space
                        StringList vecparams;
                        StrUtil::split(line, vecparams, _T("\t "), 1);

                        // Look up first param (command setting)
                        StrUtil::toLower(vecparams[0]);
                        Nui32 temp;
                        StrConv::conv(vecparams[0], temp);
                        if(!newe->set(temp, vecparams[1]))
                        {
                            // BAD command. BAD!
                            N_Only(Log).log(_I18n("Bad element attribute line: '") + line +
                                _I18n("' for element ") + N_conv(newe->getID()) + _I18n(" in overlay ") +
                                    (!obj ? StrUtil::WBLANK : N_conv(obj->getID())));
                        }
                    }
                }
            }
        }
    }
    //---------------------------------------------------------------------
    bool OverlayManager::parseGeoProperty(DataStream * stream, const String & line,
        Overlay * obj, OverlayView * parent, bool templet)
    {
        bool ret = false;
        Nui32 skipParam = 0;
        StringList params;
        StrUtil::split(line, params, _T("\t\n ()"));

        if(templet)
        {
            if(params[0] == _T("template"))
            {
                skipParam++;        // the first param = 'template' on a new child element
            }
        }

        // top level component cannot be an element, it must be a container unless it is a template
        if(params[0 + skipParam] == _T("container") || (params[0 + skipParam] == _T("element") && (templet || parent != NULL)))
        {
            String templateName;
            ret = true;
            // nested container/element
            if(params.size() > 3 + skipParam)
            {
                if(params.size() != 5 + skipParam)
                {
                    N_Only(Log).log(_I18n("Bad element/container line: '") + line + _I18n("' in ") +
                        N_conv(parent->getTypeID()) + _T(" ") + N_conv(parent->getID()) +
                            _I18n(", expecting ':' templateName"));

                    stream->skipCloseBrace();
                    // barf
                    return ret;
                }
                if(params[3 + skipParam] != _T(":"))
                {
                    N_Only(Log).log(_I18n("Bad element/container line: '")+ line + _I18n("' in ") +
                        N_conv(parent->getTypeID()) + _T(" ") + N_conv(parent->getID()) +
                            _I18n(", expecting ':' for element inheritance"));

                    stream->skipCloseBrace();
                    // barf
                    return ret;
                }

                templateName = params[4 + skipParam];
            }

            else if(params.size() != 3 + skipParam)
            {
                N_Only(Log).log(_I18n("Bad element/container line: '") + line + _I18n("' in ") +
                    N_conv(parent->getTypeID()) + _T(" ") + N_conv(parent->getID()) +
                        _I18n(", expecting 'element type(name)'"));
                stream->skipCloseBrace();
                // barf
                return ret;
            }

            stream->skipOpenBrace();
            Nui32 tid;
            Nui32 eleid;
            Nui32 templateid;
            StrConv::conv(params[1 + skipParam], tid);
            StrConv::conv(params[2 + skipParam], eleid);
            StrConv::conv(templateName, templateid);
            parseGeo(stream, tid, eleid, templateid, true, obj, (OverlayView *)parent, templet);
        }
        return ret;
    }
    //---------------------------------------------------------------------
    OverlayGeo * OverlayManager::createGeo(OverlayGeoID templateid,
        OverlayGeoType type, OverlayGeoID id, bool templet)
    {
        OverlayGeo * newObj  = NULL;

        if(templateid == 0)
        {
            newObj = createGeo(type, id, templet);
        }
        else
        {
            // no template
            OverlayGeo * tgeo = getGeo(templateid, true);

            OverlayGeoType ctype;
            if(type == 0)
            {
                ctype = tgeo->getTypeID();
            }
            else
            {
                ctype = type;
            }

            newObj = createGeo(ctype, id, templet);
            newObj->setTemplate(tgeo);
        }

        return newObj;
    }
    //---------------------------------------------------------------------
    OverlayGeo * OverlayManager::createGeo(OverlayGeoID templateid, OverlayGeoID id)
    {
        OverlayGeo * tgeo = getGeo(templateid, true);
        return tgeo->clone(id);
    }
    //---------------------------------------------------------------------
    OverlayGeo * OverlayManager::createGeo(OverlayGeoType type, OverlayGeoID id, bool templet)
    {
        if(id == 0)
            return createGeo(type, templet);

        GeoList & emap = templet ? mTemplates : mInstances;
        GeoList::iterator ii = emap.find(id);
        if(ii != emap.end())
        {
            N_EXCEPT(UniqueRepeat, _I18n("OverlayGeo with name ") + N_conv(id) + _I18n(" already exists."));
        }
        OverlayGeo * newElem = createGeoImpl(type, id);
        emap.insert(GeoList::value_type(id, newElem));
        return newElem;
    }
    //---------------------------------------------------------------------
    OverlayGeo * OverlayManager::createGeo(OverlayGeoType type, bool templet)
    {
        N_lock_mutex(mValidIDMutex)

        GeoList & emap = templet ? mTemplates : mInstances;
        OverlayGeoID temp = ++mValidID;
        OverlayGeo * newElem = createGeoImpl(type, temp);
        emap.insert(GeoList::value_type(temp, newElem));
        return newElem;
    }
    //---------------------------------------------------------------------
    OverlayGeo * OverlayManager::createGeoImpl(OverlayGeoType type, OverlayGeoID id)
    {
        FactoryMap::iterator fi = mFactories.find(type);
        if(fi == mFactories.end())
        {
            N_EXCEPT(NotExist, _I18n("Cannot locate factory for element type ") + type);
        }
        return fi->second->createGeo(id);
    }
    //---------------------------------------------------------------------
    bool OverlayManager::isTemplate(OverlayGeoID id) const
    {
        return (mTemplates.find(id) != mTemplates.end()) ;
    }
    //---------------------------------------------------------------------
    OverlayGeo * OverlayManager::getGeo(OverlayGeoID id, bool templet)
    {
        GeoList & emap = templet ? mTemplates : mInstances;
        GeoList::iterator ii = emap.find(id);
        if(ii == emap.end())
        {
            N_EXCEPT(NotExist, _I18n("OverlayGeo with name ") + N_conv(id) + _I18n(" not found."));
        }
        return ii->second;
    }
    //---------------------------------------------------------------------
    bool OverlayManager::isGeoExist(OverlayGeoID id, bool templet)
    {
        GeoList & emap = templet ? mTemplates : mInstances;
        GeoList::iterator ii = emap.find(id);
        return ii != emap.end();
    }
    //---------------------------------------------------------------------
    void OverlayManager::destroyGeo(OverlayGeoID id, bool templet)
    {
        GeoList & emap = templet ? mTemplates : mInstances;

        GeoList::iterator ii = emap.find(id);
        if(ii == emap.end())
        {
            N_EXCEPT(NotExist, _I18n("OverlayGeo with name ") + N_conv(id) + _I18n(" not found."));
        }
        // Look up factory
        OverlayGeoType type = ii->second->getTypeID();
        FactoryMap::iterator fi = mFactories.find(type);
        if(fi != mFactories.end())
        {
            fi->second->destroyGeo(ii->second);
            emap.erase(ii);
        }
    }
    //---------------------------------------------------------------------
    void OverlayManager::destroyAllGeo(bool templet)
    {
        GeoList & emap = templet ? mTemplates : mInstances;
        GeoList::iterator i;
        while((i = emap.begin()) != emap.end())
        {
            OverlayGeo * element = i->second;

            // Get factory to delete
            FactoryMap::iterator fi = mFactories.find(element->getTypeID());
            if(fi == mFactories.end())
            {
                N_EXCEPT(NotExist, _I18n("Cannot locate factory for element ")
                    + element->getID());
            }

            // remove from parent, if any
            OverlayView * parent;
            if((parent = element->getParent()) != 0)
            {
                parent->remove(element->getID());
            }

            // children of containers will be auto-removed when container is destroyed.
            // destroy the element and remove it from the list
            fi->second->destroyGeo(element);
            emap.erase(i);
        }
    }
    //---------------------------------------------------------------------
    NIIf OverlayManager::getScriptLevel() const
    {
        return 1100.0f;
    }
    //---------------------------------------------------------------------
    void OverlayManager::add(OverlayGeoFactory * obj)
    {
        mFactories[obj->getTypeID()] = obj;

        N_Only(Log).log(_I18n("OverlayGeoFactory for type ") + 
            N_conv(obj->getTypeID()) + _I18n(" registered."));
    }
    //---------------------------------------------------------------------
    const OverlayManager::FactoryMap & OverlayManager::getFactoryList() const
    {
        return mFactories;
    }
    //---------------------------------------------------------------------
}