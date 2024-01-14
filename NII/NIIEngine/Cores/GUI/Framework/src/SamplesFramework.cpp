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

#include "SamplesFramework.h"

#include "Samples_xmlHandler.h"
#include "Sample.h"
#include "SampleData.h"
#include "CEGuiBaseApplication.h"

#include "MetaDataWindowManager.h"
#include "SamplesBrowserManager.h"

#include "CEGUI/CEGUI.h"
#include "CEGUI/Logger.h"
#include "CEGUI/widgets/PushButton.h"
#include "CEGUI/widgets/ProgressBar.h"


#include <string>
#include <iostream>

using namespace CEGUI;

//platform-dependant DLL delay-loading includes
#if (defined( __WIN32__ ) || defined( _WIN32 ))
#include "windows.h"
#endif
    //------------------------------------------------------------------------
    int main(int argc, char* argv[])
    {
        // Basic start-up for the sample browser application.
        // Will remain in run() until quitting
        int argidx = 1;
    #ifdef __APPLE__
        if (argc > 1 && !std::strncmp(argv[argidx], "-psn", 4))
        {
            --argc;
            ++argidx;
        }
    #endif

        SamplesFramework sampleFramework(argc > 1 ? argv[argidx] : "");
        return sampleFramework.run();
    }
    //------------------------------------------------------------------------
    SamplesFramework::SamplesFramework(const CEGUI::String & xml_filename) :
        d_sampleExitButton(0),
        d_metaDataWinMgr(0),
        d_samplesWinMgr(0),
        d_selectedSampleData(0),
        d_loadingProgressBar(0),
        d_quittingSampleView(false),
        d_samplesXMLFilename(xml_filename)
    {
    }
    //------------------------------------------------------------------------
    SamplesFramework::~SamplesFramework()
    {
        if(d_metaDataWinMgr)
            delete d_metaDataWinMgr;
    }
    //------------------------------------------------------------------------
    bool SamplesFramework::initialise()
    {
        using namespace CEGUI;

        initialiseLoadScreenLayout();

        // return true to signalize the initialisation was sucessful and run the
        // SamplesFramework
        return true;
    }
    //------------------------------------------------------------------------
    void SamplesFramework::deinitialise()
    {
        unloadSamples();
    }
    //------------------------------------------------------------------------
    void SamplesFramework::initialiseLoadScreenLayout()
    {
        NII::Font * font = N_Only(Font).create("DejaVuSans-12.font");

        N_ONLY(UI).getMainSheet()->set(font);

        N_Only(Scheme).create("SampleBrowser.scheme");

        WidgetManager & winMgr(WidgetManager::getOnly());
        Window * loadScreenRoot = winMgr.loadLayoutFromFile("SampleBrowserLoadScreen.layout");
        N_ONLY(UI).getMainSheet()->add(loadScreenRoot);

        d_loadingProgressBar = static_cast<NII::ProgressBar *>(loadScreenRoot->getChild("LoadScreenProgressBar"));
        d_loadingScreenText = loadScreenRoot->getChild("LoadScreenText");
        d_loadScreenChunkProgressText = d_loadingScreenText->getChild("LoadScreenTextChunkProgress");
        d_loadingScreenText->setText("Parsing samples XML file...");
        d_loadingProgressBar->setProgress(0.0f);
    }
    //------------------------------------------------------------------------
    void SamplesFramework::unloadSamples()
    {
        while(d_samples.size() > 0)
        {
            SampleData*& sampleData = d_samples.back();

            sampleData->deinitialise();
            delete sampleData;

            d_samples.pop_back();
        }
    }
    //------------------------------------------------------------------------
    void SamplesFramework::loadSamplesDataFromXML(const String & file)
    {
        if(file.empty())
            CEGUI_THROW(InvalidRequestException(
            "SamplesFramework::loadSamplesDataFromXML: "
            "filename supplied for file loading must be valid."));

        Samples_xmlHandler handler(this);

        CEGUI_TRY
        {
            UIManager::getOnly().getXmlPattern()->parse(handler, file);
        }
        CEGUI_CATCH(...)
        {
            Logger::getOnly().logEvent(
                "SamplesFramework::loadSamplesDataFromXML: "
                "loading of sample data from file '" + file + "' has failed.",
                Errors);

            CEGUI_RETHROW;
        }
    }
    //------------------------------------------------------------------------
    void SamplesFramework::addSampleDataCppModule(CEGUI::String sampleName, CEGUI::String summary, 
        CEGUI::String description, SampleType sampleTypeEnum, CEGUI::String credits)
    {
        SampleData * sampleData = new SampleDataModule(sampleName, summary,
            description, sampleTypeEnum, credits);

        addSample(sampleData);
    }
    //------------------------------------------------------------------------
    bool SamplesFramework::onKeyDown(CEGUI::KeyValue key)
    {
        if(d_selectedSampleData)
        {
            if(Key::Esc != key)
                return d_selectedSampleData->getGuiContext()->onKeyDown(key);
            else
                stopDisplaySample();
        }
        else
        {
            if(Key::Esc != key)
            {
                if(CEGUI::UIManager * ceguiSystem = CEGUI::UIManager::getOnlyPtr())
                    return ceguiSystem->getMainSheet()->onKeyDown(key);
                else
                    return false;
            }
            else
                setQuitting(true);
        }

        return false;
    }
    //------------------------------------------------------------------------
    bool SamplesFramework::onKeyUp(NII::KeyValue key)
    {
        if(d_selectedSampleData)
            return d_selectedSampleData->getGuiContext()->onKeyUp(key);

        if(CEGUI::UIManager * ceguiSystem = CEGUI::UIManager::getOnlyPtr())
            ceguiSystem->getMainSheet()->onKeyUp(key);

        return false;
    }
    //------------------------------------------------------------------------
    bool SamplesFramework::onChar(Nui32 chr)
    {
        if(d_selectedSampleData)
            return d_selectedSampleData->getGuiContext()->onChar(chr);

        if(CEGUI::UIManager * ceguiSystem = CEGUI::UIManager::getOnlyPtr())
            ceguiSystem->getMainSheet()->onChar(chr);

        return false;
    }
    //------------------------------------------------------------------------
    bool SamplesFramework::onButtonDown(NII::MouseButton button)
    {
        if(d_selectedSampleData)
            return d_selectedSampleData->getGuiContext()->onButtonDown(button);

        if(CEGUI::UIManager * ceguiSystem = CEGUI::UIManager::getOnlyPtr())
            ceguiSystem->getMainSheet()->onButtonDown(button);

        return false;
    }
    //------------------------------------------------------------------------
    bool SamplesFramework::onButtonUp(NII::MouseButton button)
    {
        if(d_selectedSampleData)
            return d_selectedSampleData->getGuiContext()->onButtonUp(button);

        if(CEGUI::UIManager * ceguiSystem = CEGUI::UIManager::getOnlyPtr())
            ceguiSystem->getMainSheet()->onButtonUp(button);

        return false;
    }
    //------------------------------------------------------------------------
    bool SamplesFramework::onCursorDrag(NIIi drag)
    {
        if(d_selectedSampleData)
            return d_selectedSampleData->getGuiContext()->onCursorDrag(drag);

        if(CEGUI::UIManager* ceguiSystem = CEGUI::UIManager::getOnlyPtr())
            ceguiSystem->getMainSheet()->onCursorDrag(drag);

        return false;
    }
    //------------------------------------------------------------------------
    bool SamplesFramework::onCursorMove(NIIi x, NIIi y)
    {
        if(d_selectedSampleData)
            return d_selectedSampleData->getGuiContext()->onCursorMove(x, y);

        if(CEGUI::UIManager* ceguiSystem = CEGUI::UIManager::getOnlyPtr())
            ceguiSystem->getMainSheet()->onCursorMove(x, y);

        return false;
    }
    //------------------------------------------------------------------------
    void SamplesFramework::update(TimeDurMS passedTime)
    {
        static bool init(false);

        if(!init)
        {
            init = updateInitialisationStep();

            N_Only(UI).getMainSheet()->update(passedTime);
        }
        else
        {
            if(d_quittingSampleView)
                stopDisplaySample();

            if(!d_selectedSampleData)
            {
                updateSamples(passedTime);

                N_Only(UI).getMainSheet()->update(passedTime);
            }
            else
            {
                d_selectedSampleData->getGuiContext()->update(passedTime);

                d_selectedSampleData->update(passedTime);
            }
        }

    }
    //------------------------------------------------------------------------
    void SamplesFramework::handleNewWindowSize(float width, float height)
    {
        d_appWindowWidth = static_cast<int>(width);
        d_appWindowHeight = static_cast<int>(height);

        std::vector<SampleData*>::iterator iter = d_samples.begin();
        std::vector<SampleData*>::iterator end = d_samples.end();
        for(; iter != end; ++iter)
        {
            SampleData* sampleData = *iter;

            sampleData->handleNewWindowSize(width, height);
        }

        if (d_samplesWinMgr)
            d_samplesWinMgr->setWindowRatio(width / height);
    }
    //------------------------------------------------------------------------
    void SamplesFramework::addSample(SampleData * sampleData)
    {
        d_samples.push_back(sampleData);
    }
    //------------------------------------------------------------------------
    void SamplesFramework::renderGUIContexts()
    {
        if(!d_selectedSampleData)
        {
            renderSampleGUIContexts();

            N_Only(UI).getMainSheet()->flush();
        }
        else
        {
            d_selectedSampleData->getGuiContext()->flush();
        }
    }
    //------------------------------------------------------------------------
    void SamplesFramework::handleSampleSelection(CEGUI::Window * sampleWindow)
    {
        SampleData * correspondingSampleData = findSampleData(sampleWindow);

        d_metaDataWinMgr->setSampleInfo(correspondingSampleData);
    }
    //------------------------------------------------------------------------
    void SamplesFramework::handleStartDisplaySample(CEGUI::Window * sampleWindow)
    {
        SampleData * correspondingSampleData = findSampleData(sampleWindow);

        ViewWindow * defaultRenderTarget;
        N_OnlyPtr(UI)->get(defaultRenderTarget);
        UISheet * sampleContext(correspondingSampleData->getGuiContext());
        sampleContext->setRender(defaultRenderTarget);

        sampleContext->getActive()->add(d_sampleExitButton);
        sampleContext->getMouseCursor().setPosition(
            NII::UIManager::getOnly().getMainSheet()->getMouseCursor().getPosition());

        d_selectedSampleData = correspondingSampleData;

        d_selectedSampleData->onEnteringSample();
    }
    //------------------------------------------------------------------------
    void SamplesFramework::stopDisplaySample()
    {
        UISheet * sampleGUIContext = d_selectedSampleData->getGuiContext();

        // Since we switch our contexts, the mouse release won't be injected if we
        // don't do it manually
        sampleGUIContext->onButtonUp(CEGUI::LeftButton);
        sampleGUIContext->update(0.0f);

        sampleGUIContext->getActive()->remove(d_sampleExitButton);
        d_selectedSampleData->setGUIContextRTT();

        CEGUI::UIManager::getOnly().getMainSheet()->getMouseCursor().
            setPosition(sampleGUIContext->getMouseCursor().getPosition());

        d_selectedSampleData = 0;
        d_quittingSampleView = false;
    }
    //------------------------------------------------------------------------
    SampleData * SamplesFramework::findSampleData(CEGUI::Window * sampleWindow)
    {
        //Find corresponding SampleData
        std::vector<SampleData *>::iterator iter = d_samples.begin();
        std::vector<SampleData *>::iterator end = d_samples.end();
        for(; iter != end; ++iter)
        {
            SampleData * sampleData = *iter;

            if(sampleData->getSampleWindow() == sampleWindow)
                return sampleData;
        }

        return 0;
    }
    //------------------------------------------------------------------------
    bool SamplesFramework::handleSampleExitButtonClicked(const CEGUI::EventArgs & args)
    {
        d_quittingSampleView = true;

        return true;
    }
    //------------------------------------------------------------------------
    bool SamplesFramework::initialiseSampleStepwise(int sampleNumber)
    {
        if(static_cast<int>(d_samples.size()) <= sampleNumber)
            return true;

        if(static_cast<int>(d_samples.size()) > sampleNumber + 1)
            displaySampleLoadProgress(sampleNumber);

        if(sampleNumber >= 0)
        {
            SampleData* sampleData = d_samples[sampleNumber];
            sampleData->initialise(d_appWindowWidth, d_appWindowHeight);
            CEGUI::FrameWindow* sampleWindow = d_samplesWinMgr->createSampleWindow(
                sampleData->getName(), sampleData->getRTTImage());

            sampleData->setSampleWindow(sampleWindow);
        }

        return false;
    }
    //------------------------------------------------------------------------
    void SamplesFramework::initialiseSampleBrowserLayout()
    {
        NII::FontManager::getOnly().createFreeType("DejaVuSans-14", 14.f, true, "DejaVuSans.ttf");

        WidgetManager & winMgr(WidgetManager::getOnly());

        NII::FontManager::getOnly().create("DejaVuSans-10-NoScale.font");
        NII::FontManager::getOnly().create("Junicode-13.font");

        NII::SchemeManager::getOnly().create("Generic.scheme");

        if(!N_Only(Pixel).isExist("BackgroundSampleBrowser"))
            PixelManager::getOnly().load("BackgroundSampleBrowser", "BackgroundSampleBrowser.jpg");

        d_root = winMgr.loadLayoutFromFile("SampleBrowser.layout");
        d_root->getChild("BackgroundImage")->set("Image", "BackgroundSampleBrowser");

        CEGUI::Window* metaDataWindow = d_root->getChild("SampleBrowserMetaData");
        d_metaDataWinMgr = new MetaDataWindowManager(metaDataWindow);

        CEGUI::Window* samplesScrollContainer = d_root->getChild(
            "SampleFrameWindowContainer/SamplesFrameWindow/SamplesScrollContainer");
        d_samplesWinMgr = new SamplesBrowserManager(this, samplesScrollContainer);

        d_sampleExitButton = static_cast<CEGUI::PushButton*>(winMgr.createWindow(
            "SampleBrowserSkin/Button", "SampleExitButton"));

        d_sampleExitButton->setSize(NII::RelPlaneSize(cegui_absdim(34.f), cegui_absdim(34.f)));
        d_sampleExitButton->setPosition(NII::RelVector2(cegui_absdim(0.0f), cegui_absdim(0.0f)));
        d_sampleExitButton->setXAlign(HA_RIGHT);
        d_sampleExitButton->setYAlign(VA_TOP);

        d_sampleExitButton->set("NormalImage", "SampleBrowserSkin/ExitButtonNormal");
        d_sampleExitButton->set("HoverImage", "SampleBrowserSkin/ExitButtonHover");
        d_sampleExitButton->set("PushedImage", "SampleBrowserSkin/ExitButtonClicked");

        d_sampleExitButton->add(PushButton::EventClicked,
            EventFunctor(this, &SamplesFramework::handleSampleExitButtonClicked));

        d_sampleExitButton->setAlwaysOnTop(true);
    }
    //------------------------------------------------------------------------
    bool SamplesFramework::updateInitialisationStep()
    {
        static int step(0);

        switch (step)
        {
        case 0:
        {
            const String filename(d_samplesXMLFilename.empty() ?
                String(d_baseApp->getDataPathPrefix()) + "/samples/samples.xml" :
                d_samplesXMLFilename);

            loadSamplesDataFromXML(filename);
            ++step;
            displaySampleBrowserLayoutLoadProgress();

            break;
        }

        case 1:
        {
            initialiseSampleBrowserLayout();
            ++step;
            break;
        }

        default:
        {
            bool sampleInitFinished = initialiseSampleStepwise(step - 3); // -2 for the previous 2 steps, -1 for extra step to display the text before actually loading
            if (sampleInitFinished)
            {
                //Loading finished, switching layout to sample browser
                initialisationFinalisation();
                return true;
            }
            else
                ++step;

            break;
        }
        }

        return false;
    }
    //------------------------------------------------------------------------
    void SamplesFramework::initialisationFinalisation()
    {
        UIManager::getOnly().getMainSheet()->getMouseCursor().
            setDefaultImage("SampleBrowserSkin/MouseArrow");
        d_samplesWinMgr->setWindowRatio(d_appWindowWidth / (float)d_appWindowHeight);

        UIManager::getOnly().getMainSheet()->add(d_root);

        if (d_samples.size() > 0)
        {
            if (CEGUI::Window* wnd = d_samples[0]->getSampleWindow())
            {
                handleSampleSelection(wnd);
                d_samplesWinMgr->selectSampleWindow(wnd);
            }
        }
    }
    //------------------------------------------------------------------------
    void SamplesFramework::updateSamples(float passedTime)
    {
        std::vector<SampleData*>::iterator iter = d_samples.begin();
        std::vector<SampleData*>::iterator end = d_samples.end();
        for (; iter != end; ++iter)
        {
            SampleData* sampleData = *iter;

            UISheet* guiContext = sampleData->getGuiContext();
            guiContext->update(passedTime);

            sampleData->update(passedTime);
        }
    }
    //------------------------------------------------------------------------
    void SamplesFramework::renderSampleGUIContexts()
    {
        std::vector<SampleData*>::iterator iter = d_samples.begin();
        std::vector<SampleData*>::iterator end = d_samples.end();
        for (; iter != end; ++iter)
        {
            SampleData* sampleData = *iter;

            if (!sampleData->getGuiContext())
                continue;

            CEGUI::Window* sampleWindow = sampleData->getSampleWindow();

            if (!areWindowsIntersecting(d_samplesWinMgr->getWindow(), sampleWindow))
                continue;

            if (sampleData->getGuiContext()->isRedraw())
            {
                sampleData->getSampleWindow()->invalidate();
                sampleData->clearRTTTexture();
                sampleData->getGuiContext()->flush();
            }
        }
    }
    //------------------------------------------------------------------------
    void SamplesFramework::displaySampleBrowserLayoutLoadProgress()
    {
        int totalNum = d_samples.size() + 2;

        CEGUI::String loadText = CEGUI::String("Loading SampleBrowser skin ...");
        d_loadingScreenText->setText(loadText);

        CEGUI::String progressText = "1/" + StrConv::conv(totalNum - 1);
        d_loadScreenChunkProgressText->setText(progressText);

        d_loadingProgressBar->setProgress(1.0f / (totalNum - 1.0f));
    }
    //------------------------------------------------------------------------
    void SamplesFramework::displaySampleLoadProgress(int sampleNumber)
    {
        SampleData* sampleData = d_samples[sampleNumber + 1];

        int totalNum = d_samples.size() + 2;
        CEGUI::String loadText = "Loading " + sampleData->getName() + " ...";
        d_loadingScreenText->setText(loadText);

        CEGUI::String progressText = StrConv::conv(sampleNumber + 3) + "/" + StrConv::conv(totalNum - 1);
        d_loadScreenChunkProgressText->setText(progressText);

        d_loadingProgressBar->setProgress( (sampleNumber + 3.f) / (totalNum - 1.0f) );
    }
    //------------------------------------------------------------------------
    bool SamplesFramework::areWindowsIntersecting(CEGUI::Window * window1,
                                                  CEGUI::Window * window2)
    {
        const CEGUI::Rectf& clipRect1 = window1->getOuterRectClipper();
        const CEGUI::Rectf& clipRect2 = window2->getOuterRectClipper();

        return  clipRect1.left() < clipRect2.right()
            && clipRect1.right() > clipRect2.left()
            && clipRect1.top() < clipRect2.bottom()
            && clipRect1.bottom() > clipRect2.top()
            ;
    }
    //------------------------------------------------------------------------
