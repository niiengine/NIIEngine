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
#ifndef _Samples_Framework_h_
#define _Samples_Framework_h_

#include "SamplesFrameworkBase.h"
#include "SampleData.h"

#include <vector>

// forward declarations
#include "CEGUI/ForwardRefs.h"

class SampleData;
class Sample;
class MetaDataWindowManager;
class SamplesBrowserManager;


/*!
\brief
This is
*/
class SamplesFramework : public SamplesFrameworkBase
{
public:
    SamplesFramework(const CEGUI::String & xml_filename);
    virtual ~SamplesFramework();

    static void setGroup(const CEGUI::String & gid);

    bool initialise();
    void deinitialise();

    void addSampleDataCppModule(CEGUI::String sampleName, CEGUI::String summary,
        CEGUI::String description, SampleType sampleTypeEnum, CEGUI::String credits);

    void handleSampleSelection(CEGUI::Window* sampleWindow);
    void handleStartDisplaySample(CEGUI::Window* sampleWindow);

    virtual void update(float passedTime);

    virtual void handleNewWindowSize(float width, float height);

    virtual void renderGUIContexts();

    SampleData* findSampleData(CEGUI::Window* sampleWindow);

    virtual bool onKeyDown(NII::KeyValue key);
    virtual bool onKeyUp(NII::KeyValue key);
    virtual bool onChar(Nui32 chr);
    virtual bool onButtonDown(NII::MouseButton button);
    virtual bool onButtonUp(NII::MouseButton button);
    virtual bool onCursorDrag(NIIi drag);
    virtual bool onCursorMove(NIIi x, NIIi y);
protected:
    void initialiseLoadScreenLayout();

    void initialiseSampleBrowserLayout();
    void loadSamplesDataFromXML(const CEGUI::String & file);
    void getSampleInstanceFromDLL(SampleData & sampleData);

    void addSample(SampleData* sampleData);

    bool initialiseSampleStepwise(int sampleNumber);

    void displaySampleLoadProgress(int sampleNumber);
    void initialisationFinalisation();
    void unloadSamples();

    void stopDisplaySample();

    void updateSamples(float passedTime);
    bool updateInitialisationStep();

    void displaySampleBrowserLayoutLoadProgress();
    bool handleSampleExitButtonClicked(const CEGUI::EventArgs& args);
    void renderSampleGUIContexts();

    bool areWindowsIntersecting(CEGUI::Window* window1, CEGUI::Window* window2);

    CEGUI::Window*              d_root;

    CEGUI::PushButton*          d_sampleExitButton;

    std::vector<SampleData*>    d_samples;

    MetaDataWindowManager*      d_metaDataWinMgr;
    SamplesBrowserManager*      d_samplesWinMgr;

    SampleData*                 d_selectedSampleData;

    CEGUI::ProgressBar*         d_loadingProgressBar;
    CEGUI::Window*              d_loadingScreenText;
    CEGUI::Window*              d_loadScreenChunkProgressText;

    bool                        d_quittingSampleView;

    CEGUI::String d_samplesXMLFilename;
};

#endif
