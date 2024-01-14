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
#include "SampleData.h"
#include "Sample.h"

#include "Samples_xmlHandler.h"
#include "NiiDynamicModule.h"
#include "NiiUIManager.h"
#include "NiiPixelManager.h"
#include "NiiUIWindow.h"

using namespace CEGUI;

#define S_(X) #X
#define STRINGIZE(X) S_(X)

typedef Sample& (*getSampleInstance)();
#define GetSampleInstanceFuncName "getSampleInstance"

SampleData::SampleData(CEGUI::String sampleName, CEGUI::String summary,
    CEGUI::String description, SampleType sampleTypeEnum, CEGUI::String credits) :
    d_name(sampleName),
    d_summary(summary),
    d_description(description),
    d_type(sampleTypeEnum),
    d_usedFilesString(""),
    d_credits(credits),
    d_sampleWindow(0),
    d_guiContext(0),
    d_textureTarget(0),
    d_textureTargetImage(0)
{
}

SampleData::~SampleData()
{

}

CEGUI::String SampleData::getName()
{
    return d_name;
}

CEGUI::String SampleData::getSummary()
{
    return "Summary:\n" + d_summary;
}

CEGUI::String SampleData::getCredits()
{
    return "Credits:\n" + d_credits;
}

CEGUI::String SampleData::getSampleTypeString()
{
    switch(d_type)
    {
    case ST_Module:
        return SampleDataHandler::SampleTypeCppModule;
        break;
    case ST_Lua:
        return SampleDataHandler::SampleTypeLua;
        break;
    case ST_Python:
        return SampleDataHandler::SampleTypePython;
    default:
        return "";
    }
}

CEGUI::String SampleData::getVerbose()
{
    return "Description:\n" + d_description;
}

CEGUI::String SampleData::getUsedFilesString()
{
    return "Used files:\n" + d_usedFilesString;
}

void SampleData::setSampleWindow(CEGUI::Window* sampleWindow)
{
    d_sampleWindow = sampleWindow;
}

CEGUI::Window * SampleData::getSampleWindow()
{
    return d_sampleWindow;
}

void SampleData::initialise(NCount width, NCount height)
{
    CEGUI::UIManager & system(UIManager::getOnly());
    system.create(, , d_guiContext);
    d_guiContext->create(d_textureTarget);
    d_textureTarget->setNativeSize(PlaneSizei(width, height));

    CEGUI::String imageName(d_textureTarget->getTexture().getSrc());
    d_textureTargetImage = static_cast<CEGUI::PixelBuffer*>(&CEGUI::PixelManager::getOnly().create("PixelBuffer", "SampleBrowser/" + imageName));
    d_textureTargetImage->setTexture(&d_textureTarget->getTexture());

    setTextureTargetImageArea(static_cast<float>(height), static_cast<float>(width));
}

void SampleData::deinitialise()
{
    CEGUI::UIManager & system(UIManager::getOnly());

    if(d_textureTarget)
    {
        d_guiContext.destroy(d_textureTarget);
        d_textureTarget = 0;
    }

    if(d_guiContext)
    {
        system.destroy(*d_guiContext);
        d_guiContext = 0;
    }

    if(d_textureTargetImage)
    {
        CEGUI::PixelManager::getOnly().destroy(*d_textureTargetImage);
        d_textureTargetImage = 0;
    }
}

UISheet* SampleData::getGuiContext()
{
    return d_guiContext;
}

void SampleData::handleNewWindowSize(NCount width, NCount height)
{
    setTextureTargetImageArea(height, width);

    if(d_textureTarget)
    {
        d_textureTarget->setNativeSize(PlaneSizei(width, height));
        d_sampleWindow->getRenderingSurface()->invalidate();
    }
}

CEGUI::PixelBuffer& SampleData::getRTTImage()
{
    return *d_textureTargetImage;
}

void SampleData::setGUIContextRTT()
{
    d_guiContext->add(d_textureTarget);
}

void SampleData::clearRTTTexture()
{
    d_textureTarget->refresh();
}

void SampleData::setTextureTargetImageArea(float height, float width)
{
    if(d_textureTarget)
    {
        CEGUI::Rectf renderArea;
        if(false)
            renderArea = CEGUI::Rectf(0.0f, height, width, 0.0f);
        else
            renderArea = CEGUI::Rectf(0.0f, 0.0f, width, height);

        if(d_textureTargetImage)
            d_textureTargetImage->setArea(renderArea);
    }
}


SampleDataModule::SampleDataModule(CEGUI::String sampleName, CEGUI::String summary,
    CEGUI::String description, SampleType sampleTypeEnum, CEGUI::String credits)
    : SampleData(sampleName, summary, description, sampleTypeEnum, credits)
{
}

SampleDataModule::~SampleDataModule()
{
}

void SampleDataModule::initialise(int width, int height)
{
    SampleData::initialise(width, height);

    getSampleInstanceFromDLL();

    d_sample->initialise(d_guiContext);
    d_usedFilesString = d_sample->getUsedFilesString();
}

void SampleDataModule::deinitialise()
{
    d_sample->deinitialise();

    SampleData::deinitialise();
}

void SampleDataModule::getSampleInstanceFromDLL()
{
    NII::DynamicModule * sampleModule = N_new NII::DynamicModule(d_name);
    getSampleInstance functionPointerGetSample = (getSampleInstance)sampleModule->getSymbol(CEGUI::String(GetSampleInstanceFuncName));

    if(functionPointerGetSample == 0)
    {
        CEGUI::String errorMessage = "The sample creation function is not defined in the dynamic library of " + d_name;
        CEGUI_THROW(CEGUI::InvalidRequestException(errorMessage.c_str()));
    }

    d_sample = &(functionPointerGetSample());
}

void SampleDataModule::onEnteringSample()
{
    d_sample->onEnteringSample();
}

void SampleDataModule::update(float timeSinceLastUpdate)
{
    d_sample->update(timeSinceLastUpdate);
}