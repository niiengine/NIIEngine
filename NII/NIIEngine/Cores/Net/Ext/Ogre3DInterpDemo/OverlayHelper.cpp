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
#include "OverlayHelper.h"
#include "OgreTextOverlayGeo.h"
#include "OgreContainerOverlayGeo.h"
#include "OgreOverlayManager.h"
#include "OgreOverlayGeoFactory.h"

using namespace Ogre;

    OverlayHelper::TimedOverlay::TimedOverlay()
    {
    }
    OverlayHelper::TimedOverlay::~TimedOverlay()
    {
    }
    OverlayHelper::TimedOverlay::TimedOverlay(OverlayGeo *overlayElement, unsigned int totalTime, unsigned int fadeTimeMS, NIIf finalAlpha, bool deleteAfterFade)
    {
        startFadeAlpha = -1.0f;
        this->overlayElement = overlayElement;
        this->remainingTimeMS = totalTime;
        this->fadeTimeMS = fadeTimeMS;
        this->finalAlpha = finalAlpha;
        this->deleteAfterFade = deleteAfterFade;
    }
    OverlayHelper::OverlayHelper()
    {
        globalOverlay=0;
    }
    OverlayHelper::~OverlayHelper()
    {

    }
    void OverlayHelper::Startup(void)
    {
        globalOverlay = N_Only(Overlay).create("OverlayHelperRoot");
        globalOverlay->show();
    }
    void OverlayHelper::Shutdown(void)
    {
        timedOverlays.Clear(false);
        if (globalOverlay)
            N_Only(Overlay).destroy(globalOverlay);
    }
    void OverlayHelper::update(unsigned int elapsedTimeMS)
    {
        unsigned i;
        i=0;
        while (i < timedOverlays.Size())
        {
            if (timedOverlays[i].remainingTimeMS < elapsedTimeMS)
            {
                if (timedOverlays[i].deleteAfterFade)
                {
                    SafeDestroyOverlayGeo(timedOverlays[i].overlayElement);
                }
                else
                {
                    const Colour &color = timedOverlays[i].overlayElement->getColour();
                    Colour newColor = color;
                    newColor.a=timedOverlays[i].finalAlpha;
                    timedOverlays[i].overlayElement->setColour(newColor);
                    timedOverlays.RemoveAtIndex(i);
                }
            }
            else
            {
                timedOverlays[i].remainingTimeMS-=elapsedTimeMS;
                if (timedOverlays[i].remainingTimeMS < timedOverlays[i].fadeTimeMS)
                {
                    const Colour &color = timedOverlays[i].overlayElement->getColour();
                    if (timedOverlays[i].startFadeAlpha==-1.0f)
                        timedOverlays[i].startFadeAlpha=color.a;
                    Colour newColor = color;
                    newColor.a=timedOverlays[i].finalAlpha - (timedOverlays[i].finalAlpha-timedOverlays[i].startFadeAlpha) * (NIIf) timedOverlays[i].remainingTimeMS / (NIIf) timedOverlays[i].fadeTimeMS;
                    timedOverlays[i].overlayElement->setColour(newColor);
                }
                i++;
            }
        }
    }
    Overlay * OverlayHelper::GetGlobalOverlay(void) const
    {
        return globalOverlay;
    }
    
    void OverlayHelper::FadeOverlayGeo(OverlayGeo* element, unsigned int totalTime, unsigned int fadeTimeMS, NIIf finalAlpha, bool deleteAfterFade)
    {
        timedOverlays.Insert(TimedOverlay(element, totalTime, fadeTimeMS, finalAlpha,deleteAfterFade));
    }
    
    OverlayView * OverlayHelper::CreatePanel(const char *instanceName, bool addToGlobalOverlay)
    {
        OverlayView * element = (OverlayView *)N_Only(Overlay).createGeo("Panel", instanceName);
        if (addToGlobalOverlay)
            globalOverlay->add(element);
        return element;
    }
    
    TextOverlayGeo * OverlayHelper::CreateTextArea(const char * instanceName, const char *fontName, OverlayView* parent)
    {
        TextOverlayGeo * element = (TextOverlayGeo *) N_Only(Overlay).createGeo("TextArea", instanceName);
        if (parent)
            parent->addChild(element);
        element->setFontID(fontName);
        return element;
    }
    
    FrameOverlayGeo *OverlayHelper::CreateBorderPanel(const char *instanceName, OverlayView* parent)
    {
        FrameOverlayGeo *element = (FrameOverlayGeo *) N_Only(Overlay).createGeo("BorderPanel", instanceName);
        if (parent)
            parent->addChild(element);
        return element;
    }
    
    void OverlayHelper::SafeDestroyOverlayGeo(OverlayGeo * item)
    {
        //if (item->isContainer())
        {
            OverlayView *container = (OverlayView*) item;

            // Arrggghh the variable is protected
            // ((OverlayView*)item)->mOverlay->remove((OverlayView*)item);
            OverlayManager::OverlayMap::iterator iter1, iter1end = N_Only(Overlay).getList().end();
            for(iter1 = N_Only(Overlay).getList().begin(); iter1 != iter1end; ++iter1)
            {
                iter1->second->remove(container);
            }

            OverlayView::ChildMap::iterator iter2, iter2end = container->getList().end();
            for(iter2 = container->getList().begin(); iter2 != iter2end; ++iter2)
            {
                iter2->second->setParent(0);
            }
        }

        if (item->getParent())
            item->getParent()->removeChild(item->getID());
        N_Only(Overlay).destroyGeo(item);
        unsigned i;
        i=0;
        while (i < timedOverlays.Size())
        {
            if (timedOverlays[i].overlayElement==item)
                timedOverlays.RemoveAtIndex(i);
            else
                i++;
        }
    }
