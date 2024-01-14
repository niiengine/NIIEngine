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
#include "SamplesBrowserManager.h"

#include "SamplesFramework.h"

#include "CEGUI/Window.h"
#include "CEGUI/SchemeManager.h"
#include "CEGUI/WidgetManager.h"
#include "NiiEventArgs.h"
#include "CEGUI/widgets/DefaultWindow.h"
#include "CEGUI/widgets/VerticalLayoutContainer.h"
#include "CEGUI/widgets/HorizontalLayoutContainer.h"
#include "CEGUI/widgets/FrameWindow.h"
#include "CEGUI/widgets/PushButton.h"
#include "NiiPixelBuffer.h"
#include "CEGUI/falagard/WidgetLookManager.h"

#define InnerArea 1
using namespace CEGUI;

const CEGUI::uint32 SamplesBrowserManager::d_sampleWindowFrameNormal(0xFFFFFFFF);
const CEGUI::uint32 SamplesBrowserManager::d_sampleWindowFrameSelected(0xFF77FFB6);

SamplesBrowserManager::SamplesBrowserManager(SamplesFramework* owner, CEGUI::Window* samplesWindow)
    : d_owner(owner),
    d_root(samplesWindow),
    d_childCount(0),
    d_aspectRatio(1.0f),
    d_selectedWindow(0)
{
    init();
}


CEGUI::Window* SamplesBrowserManager::getWindow()
{
    return d_root;
}

CEGUI::FrameWindow* SamplesBrowserManager::createSampleWindow(const CEGUI::String& name, const CEGUI::PixelBuffer& pb)
{
    CEGUI::VerticalLayoutContainer* root = createPreviewLayoutContainer();

    CEGUI::HorizontalLayoutContainer* header = createPreviewHeader();
    root->addChild(header);

    CEGUI::DefaultWindow* windowName = createPreviewHeaderNameWindow(name);
    header->addChild(windowName);

    CEGUI::PushButton* entryButton = createPreviewHeaderEnterButton();
    windowName->addChild(entryButton);

    FrameWindow* sampleWindow = createPreviewSampleWindow(name, pb);
    root->addChild(sampleWindow);

    d_sampleWindows.push_back(sampleWindow);
    d_buttonToSampleWindowMap[entryButton] = sampleWindow;

    d_verticalLayoutContainerSamples->addChild(root);


    ++d_childCount;

    return sampleWindow;
}

void SamplesBrowserManager::setWindowRatio(float aspectRatio)
{
    d_aspectRatio = aspectRatio;

    updateWindows();
}

void SamplesBrowserManager::updateWindows()
{
    int max = d_sampleWindows.size();
    for(int i = 0; i < max; ++i)
    {
        CEGUI::Window * window(d_sampleWindows[i]);

        window->setAspectRatio(d_aspectRatio);
        window->setSize(RelPlaneSize(UDim(1.0f, -10.0f), cegui_absdim(1.0f)));
    }

    d_root->setSize(RelPlaneSize(cegui_reldim(1.0f), cegui_reldim(1.0f)));
}

bool SamplesBrowserManager::handleMouseClickSampleWindow(const CEGUI::EventArgs & args)
{
    const WidgetEventArgs& winArgs(static_cast<const WidgetEventArgs&>(args));

    CEGUI::Window* wnd(winArgs.mWidget);

    d_owner->handleStartDisplaySample(wnd);

    return true;
}


bool SamplesBrowserManager::handleMouseMoveSampleWindow(const CEGUI::EventArgs & args)
{
    const CursorEventArgs & mouseArgs(static_cast<const CursorEventArgs &>(args));

    CEGUI::Window * wnd(mouseArgs.mWidget);

    if(d_selectedWindow != wnd)
    {
        selectSampleWindow(wnd);
        d_owner->handleSampleSelection(wnd);
    }

    const CEGUI::String & lookNFeel(wnd->getLookNFeel());
    CEGUI::Rectf innerRectangle = CEGUI::WidgetLookManager::getOnly().getWidgetLook(lookNFeel).getSpecial(inner_rect).getArea().getPixelRect(*wnd);

    const CEGUI::Vector2f & mousePos(Vector2f(mouseArgs.mLocalX, mouseArgs.mLocalY));

    const CEGUI::Rectf & windowDimensions(wnd->getUnclippedOuterRect().getIntCache());

    float relPosX = (mousePos.x - windowDimensions.left() - innerRectangle.getPosition().x) / innerRectangle.getWidth();
    float relPosY = (mousePos.y - windowDimensions.top()  - innerRectangle.getPosition().y) / innerRectangle.getHeight();

    if(relPosX >= 0.0f && relPosX <= 1.0f && relPosY >= 0.0f && relPosY <= 1.0f)
    {
        SampleData * sampleData = d_owner->findSampleData(wnd);
        const CEGUI::PlaneSizef & contextSize(sampleData->getGuiContext()->getSurfaceSize());

        float absPosX = relPosX * contextSize.mWidth;
        float absPosY = relPosY * contextSize.mHeight;

        sampleData->getGuiContext()->onCursorMove(absPosX, absPosY);
        sampleData->getGuiContext()->markAsDirty();

        wnd->setMouseCursor("SampleBrowserSkin/MouseArrowHover");
    }
    else
    {
        wnd->setMouseCursor("SampleBrowserSkin/MouseArrow");
    }

    return true;
}


bool SamplesBrowserManager::handleLeaveSampleWindow(const CEGUI::EventArgs& args)
{
    const CursorEventArgs & mouseArgs(static_cast<const CursorEventArgs &>(args));

    CEGUI::Window * wnd(mouseArgs.mWidget);
    wnd->setMouseCursor("SampleBrowserSkin/MouseArrow");

    return true;
}

void SamplesBrowserManager::selectSampleWindow(CEGUI::Window* wnd)
{
    if(d_selectedWindow)
    {
        CEGUI::ColourArea colRectNormal = CEGUI::ColourArea(CEGUI::Colour(d_sampleWindowFrameNormal));
        d_selectedWindow->set("FrameColours", StrConv::conv(colRectNormal));
    }

    d_selectedWindow = wnd;

    CEGUI::ColourArea colRectSelected = CEGUI::ColourArea(CEGUI::Colour(d_sampleWindowFrameSelected));
    d_selectedWindow->set("FrameColours", StrConv::conv(colRectSelected));
}

void SamplesBrowserManager::init()
{
    WidgetManager & winMgr(WidgetManager::getOnly());

    d_verticalLayoutContainerSamples = static_cast<VerticalLayoutContainer*>(winMgr.createWindow("VerticalLayoutContainer"));

    d_verticalLayoutContainerSamples->setMargin(CEGUI::RelRect(cegui_reldim(0.1f),cegui_reldim(0.0f),  cegui_reldim(0.1f), cegui_absdim(14.f)));
    d_verticalLayoutContainerSamples->setMouseInputPropagationEnabled(true);

    d_root->addChild(d_verticalLayoutContainerSamples);

}

CEGUI::DefaultWindow* SamplesBrowserManager::createPreviewHeaderNameWindow(const CEGUI::String& name)
{
    WidgetManager& winMgr(WidgetManager::getOnly());

    CEGUI::DefaultWindow* windowName = static_cast<DefaultWindow*>(winMgr.createWindow("SampleBrowserSkin/StaticText"));
    windowName->setSize(CEGUI::RelPlaneSize(cegui_reldim(0.6f), cegui_absdim(44.f)));
    windowName->setText(name);
    windowName->setFont("DejaVuSans-12-NoScale");
    windowName->set("HorzFormatting", "Centre");
    windowName->setMouseInputPropagationEnabled(true);

    return windowName;
}

CEGUI::VerticalLayoutContainer* SamplesBrowserManager::createPreviewLayoutContainer()
{
    WidgetManager& winMgr(WidgetManager::getOnly());

    CEGUI::VerticalLayoutContainer* root = static_cast<VerticalLayoutContainer*>(winMgr.createWindow("VerticalLayoutContainer"));
    root->setSize(CEGUI::RelPlaneSize(cegui_reldim(0.8f), cegui_reldim(1.0f)));
    root->setMouseInputPropagationEnabled(true);
    root->setMargin(CEGUI::RelRect(UDim(0.0f, 0.0f),UDim(0.0f, 0.0f),UDim(0.0f, 0.0f), UDim(0.0f, 8.f)));

    return root;
}

CEGUI::FrameWindow* SamplesBrowserManager::createPreviewSampleWindow(const CEGUI::String& name, const CEGUI::PixelBuffer &pb)
{
    WidgetManager& winMgr(WidgetManager::getOnly());

    FrameWindow* sampleWindow = static_cast<FrameWindow*>(winMgr.createWindow("SampleBrowserSkin/SampleWindow", name));
    CEGUI::String imageName = pb.getName();
    sampleWindow->set("PixelBuffer", imageName);

    sampleWindow->setSize(RelPlaneSize(UDim(1.0f, -10.0f), cegui_absdim(1.0f)));
    sampleWindow->setMouseInputPropagationEnabled(true);

    sampleWindow->add(Window::EventMouseMove,
        EventFunctor(this, &SamplesBrowserManager::handleMouseMoveSampleWindow));

    sampleWindow->add(Window::EventMouseClick,
        EventFunctor(this, &SamplesBrowserManager::handleMouseClickSampleWindow));

    sampleWindow->add(Window::EventMouseLeavesArea,
        EventFunctor(this, &SamplesBrowserManager::handleLeaveSampleWindow));

    CEGUI::ColourArea colRect((CEGUI::Colour(d_sampleWindowFrameNormal)));
    sampleWindow->set("FrameColours", StrConv::conv(colRect));

    sampleWindow->setAspectMode(AM_Expand);

    return sampleWindow;
}

CEGUI::HorizontalLayoutContainer* SamplesBrowserManager::createPreviewHeader()
{
    WidgetManager& winMgr(WidgetManager::getOnly());

    CEGUI::HorizontalLayoutContainer* header = static_cast<HorizontalLayoutContainer*>(winMgr.createWindow("HorizontalLayoutContainer"));
    header->setSize(CEGUI::RelPlaneSize(cegui_reldim(1.0f), cegui_absdim(40.0f)));
    header->setMouseInputPropagationEnabled(true);
    header->setMargin(CEGUI::RelRect(UDim(0.0f, 0.0f), UDim(0.0f, 12.f), UDim(0.0f, 0.0f), UDim(0.0f, 0)));
    header->setXAlign(HA_CENTRE);

    return header;
}

CEGUI::PushButton* SamplesBrowserManager::createPreviewHeaderEnterButton()
{
    WidgetManager& winMgr(WidgetManager::getOnly());

    CEGUI::PushButton* button = static_cast<CEGUI::PushButton*>(winMgr.createWindow("SampleBrowserSkin/Button", "SampleEntryButton"));

    button->setSize(CEGUI::RelPlaneSize(cegui_absdim(1.0f), cegui_reldim(0.7f)));
    button->setAspectMode(AM_Expand);
    button->setAspectRatio(1.0f);
    button->setPosition(NII::RelVector2(cegui_absdim(-7.f), cegui_absdim(0.0f)));
    button->setMouseInputPropagationEnabled(true);
    button->set("NormalImage", "SampleBrowserSkin/EntryButtonNormal");
    button->set("HoverImage", "SampleBrowserSkin/EntryButtonHover");
    button->set("PushedImage", "SampleBrowserSkin/EntryButtonClicked");

    button->add(PushButton::EventClicked,
        EventFunctor(this, &SamplesBrowserManager::handleSampleEnterButtonClicked));

    button->setAlwaysOnTop(true);
    button->setXAlign(HA_RIGHT);
    button->setYAlign(VA_CENTRE);
    button->setAlwaysOnTop(true);

    return button;
}


bool SamplesBrowserManager::handleSampleEnterButtonClicked(const CEGUI::EventArgs& args)
{
    const WidgetEventArgs& winArgs(static_cast<const WidgetEventArgs&>(args));

    CEGUI::Window* sampleWindow = d_buttonToSampleWindowMap[winArgs.mWidget];
    d_owner->handleStartDisplaySample(sampleWindow);

    return true;
}
