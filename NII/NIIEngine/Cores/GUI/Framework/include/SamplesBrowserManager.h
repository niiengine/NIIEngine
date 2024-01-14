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
#ifndef _SamplesBrowserManager_h_
#define _SamplesBrowserManager_h_

#include <vector>
#include <map>

#include "CEGUI/Base.h"
#include "CEGUI/String.h"

#include "CEGUI/ForwardRefs.h"

class SamplesFramework;

class SamplesBrowserManager
{
public:
    SamplesBrowserManager(SamplesFramework* owner, CEGUI::Window* samplesWindow);
    virtual ~SamplesBrowserManager() {}

    CEGUI::Window* getWindow();

    CEGUI::FrameWindow* createSampleWindow(const CEGUI::String& name, const CEGUI::PixelBuffer& pb);

    void setWindowRatio(float aspectRatio);

    void selectSampleWindow(CEGUI::Window* wnd);

private:
    SamplesBrowserManager(const SamplesBrowserManager&) {}

    void init();
    SamplesBrowserManager& operator=(const SamplesBrowserManager&) {}

    static const CEGUI::uint32 d_sampleWindowFrameNormal;
    static const CEGUI::uint32 d_sampleWindowFrameSelected;

    void updateWindows();

    bool handleMouseClickSampleWindow(const CEGUI::EventArgs& args);
    bool handleMouseMoveSampleWindow(const CEGUI::EventArgs& args);
    bool handleLeaveSampleWindow(const CEGUI::EventArgs& args);

    bool handleSampleEnterButtonClicked(const CEGUI::EventArgs& args);

    CEGUI::VerticalLayoutContainer* createPreviewLayoutContainer();
    CEGUI::DefaultWindow* createPreviewHeaderNameWindow(const CEGUI::String& name);
    CEGUI::FrameWindow* createPreviewSampleWindow(const CEGUI::String& name, const CEGUI::PixelBuffer & pb);
    CEGUI::PushButton* createPreviewHeaderEnterButton();
    CEGUI::HorizontalLayoutContainer* createPreviewHeader();

    SamplesFramework* d_owner;

    CEGUI::Window* d_root;
    CEGUI::VerticalLayoutContainer* d_verticalLayoutContainerSamples;
    int d_childCount;
    float d_aspectRatio;
    CEGUI::Window* d_selectedWindow;

    
    std::map<CEGUI::Window*, CEGUI::Window*> d_buttonToSampleWindowMap;


    std::vector<CEGUI::Window*> d_sampleWindows;
};

#endif