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

#include "CEGUISamplesConfig.h"
#include "CEGuiBaseApplication.h"
#include "SamplesFramework.h"
#include "NiiUIManager.h"
#include "NiiPixelBuffer.h"
#include "NiiPixelManager.h"
#include "CEGUI/Font.h"
#include "CEGUI/Scheme.h"
#include "NiiUIWidgetManager.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/ScriptModule.h"
#include "NiiXmlPattern.h"
#include "NiiGeometryPixel.h"
#include "NiiUISheet.h"
#include "CEGUI/FrameObj.h"
#include "NiiAnimationManager.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#ifdef __APPLE__
#   include <Carbon/Carbon.h>
#endif

// setup default-default path
#ifndef CEGUI_SAMPLE_DATAPATH
    #define CEGUI_SAMPLE_DATAPATH "../datafiles"
#endif

    /***********************************************************************
        Static / Const data
    *************************************************************************/
    const char CEGuiBaseApplication::DATAPATH_VAR_NAME[] = "CEGUI_SAMPLE_DATAPATH";
    SamplesFrameworkBase * CEGuiBaseApplication::d_sampleApp(0);
    //------------------------------------------------------------------------
    CEGuiBaseApplication::CEGuiBaseApplication() :
        d_quitting(false),
        d_logoGeometry(0),
        d_FPSGeometry(0),
        d_FPSElapsed(0.0f),
        d_FPSFrames(0),
        d_FPSValue(0),
        d_spinLogo(false)
    {
    }
    //------------------------------------------------------------------------
    CEGuiBaseApplication::~CEGuiBaseApplication()
    {
    }
    //------------------------------------------------------------------------
    void CEGuiBaseApplication::renderSingleFrame(float elapsed)
    {
        UIManager * mag = N_OnlyPtr(UI);

        mag->update(elapsed);
        d_sampleApp->update(elapsed);

        updateFPS(elapsed);
        updateLogo(elapsed);

        beginRendering(elapsed);

        mag->beginRendering();
        d_sampleApp->renderGUIContexts();
        mag->endRendering();

        WidgetManager::getOnly().recover();

        endRendering();
    }
    //------------------------------------------------------------------------
    bool CEGuiBaseApplication::execute(SamplesFrameworkBase * sampleApp)
    {
        d_sampleApp = sampleApp;

        initialiseDefaultResourceGroups();

        UIManager * mag = N_OnlyPtr(UI);
        PixelBuffer * pb;
        const Rectf scrn(Vector2f(0, 0), mag->getDisplaySize());

        // setup for FPS value
        mag->create(d_FPSGeometry);
        d_FPSGeometry->setClippingRegion(scrn);

        // setup for spinning logo
        mag->create(d_logoGeometry);
        d_logoGeometry->setPivot(Vector3f(91.5f, 44.5f, 0));
        positionLogo();

        // create logo imageset and draw the pb (we only ever draw this once)
        PixelManager::getOnly().load("cegui_logo", "logo.png");
        pb = PixelManager::getOnly().get("cegui_logo");
        d_logoGeometry->add(pb, &Rectf(0, 0, 183, 89), 0, &ColourArea(0xFFFFFFFF));

        // clearing this queue actually makes sure it's created(!)
        N_Only(UI).getMainSheet()->clearGeometry(GL_Overlay);

        // subscribe handler to render overlay items
        N_Only(UI).getMainSheet()->add(RenderingSurface::PreRenderEvent,
            EventFunctor(this, &CEGuiBaseApplication::overlayHandler));

        // subscribe handler to reposition logo when window is sized.
        N_Only(UI).add(UIManager::ViewSizeChangeEvent,
            EventFunctor(this, &CEGuiBaseApplication::resizeHandler));

        const Rectf & area(N_OnlyPtr(UI)->getDefaultRenderTarget().getArea());

        d_sampleApp->setApplicationWindowSize(static_cast<int>(area.getWidth()),
            static_cast<int>(area.getHeight()));

        run();

        cleanup();
        destroyWindow();

        return true;
    }
    //------------------------------------------------------------------------
    void CEGuiBaseApplication::cleanup()
    {
        UIManager * mag = N_OnlyPtr(UI);
        PixelManager::getOnly().destroy("cegui_logo");
        mag->destroy(*d_logoGeometry);
        mag->destroy(*d_FPSGeometry);
    }
    //------------------------------------------------------------------------
    void CEGuiBaseApplication::initialiseDefaultResourceGroups()
    {
        // set the default resource groups to be used
        PixelManager::setResourceGroup(10);
        Font::setGroup("fonts");
        Scheme::setGroup("schemes");
        WidgetLookManager::setGroup("looknfeels");
        WidgetManager::setGroup("layouts");
        ScriptModule::setGroup("lua_scripts");
        AnimationManager::setGroup("animations");
    }
    //------------------------------------------------------------------------
    const char * CEGuiBaseApplication::getDataPathPrefix() const
    {
        static char dataPathPrefix[PATH_MAX];

    #ifdef __APPLE__
        CFURLRef datafilesURL =
            CFBundleCopyResourceURL(CFBundleGetMainBundle(), CFSTR("datafiles"), 0, 0);
        CFURLGetFileSystemRepresentation(datafilesURL, true,
            reinterpret_cast<UInt8 *>(dataPathPrefix), PATH_MAX);
        CFRelease(datafilesURL);
    #else
        char* envDataPath = 0;

        // get data path from environment var
        envDataPath = getenv(DATAPATH_VAR_NAME);

        // set data path prefix / base directory.  This will
        // be either from an environment variable, or from
        // a compiled in default based on original configure
        // options
        if (envDataPath != 0)
            strcpy(dataPathPrefix, envDataPath);
        else
            strcpy(dataPathPrefix, CEGUI_SAMPLE_DATAPATH);
    #endif

        return dataPathPrefix;
    }
    //------------------------------------------------------------------------
    bool CEGuiBaseApplication::overlayHandler(const EventArgs & args)
    {
        using namespace CEGUI;

        if(static_cast<const GeometryLevelEventArgs &>(args).mGL != GL_Overlay)
            return false;

        // draw the logo
        d_logoGeometry->flush();
        // draw FPS value
        d_FPSGeometry->flush();

        return true;
    }
    //------------------------------------------------------------------------
    void CEGuiBaseApplication::updateFPS(float elapsed)
    {
        // another frame
        ++d_FPSFrames;

        if((d_FPSElapsed += elapsed) >= 1.0f)
        {
            if(d_FPSFrames != d_FPSValue)
            {
                d_FPSValue = d_FPSFrames;

                Font * fnt;
                N_Only(UI).getMainSheet()->get(fnt);
                if(!fnt)
                    return;

                // update FPS imagery
                char fps_textbuff[16];
                sprintf(fps_textbuff , "FPS: %d", d_FPSValue);

                d_FPSGeometry->clear();
                fnt->drawText(*d_FPSGeometry, fps_textbuff, Vector2f(0, 0), 0,
                    Colour(0xFF000000));
            }

            // reset counter state
            d_FPSFrames = 0;
            d_FPSElapsed = 0.0f;
        }
    }
    //------------------------------------------------------------------------
    void CEGuiBaseApplication::updateLogo(float elapsed)
    {
        if (!d_spinLogo)
            return;

        static float rot = 0.0f;
        d_logoGeometry->setRotation(Quaternion::eulerAnglesDegrees(rot, 0, 0));

        rot = fmodf(rot + 180.0f * elapsed, 360.0f);
    }
    //------------------------------------------------------------------------
    void CEGuiBaseApplication::positionLogo()
    {
        const Rectf scrn(N_OnlyPtr(UI)->getDefaultRenderTarget().getArea());

        d_logoGeometry->setClippingRegion(scrn);
        d_logoGeometry->setTranslation(
            Vector3f(10.0f, scrn.getSize().mHeight - 89.0f, 0.0f));
    }
    //------------------------------------------------------------------------
    bool CEGuiBaseApplication::resizeHandler(const EventArgs & /*args*/)
    {
        // clear FPS geometry and see that it gets recreated in the next frame
        d_FPSGeometry->clear();
        d_FPSValue = 0;

        const Rectf & area(N_OnlyPtr(UI)->getDefaultRenderTarget().getArea());
        d_sampleApp->handleNewWindowSize(area.getWidth(), area.getHeight());

        positionLogo();
        return true;
    }
    //------------------------------------------------------------------------