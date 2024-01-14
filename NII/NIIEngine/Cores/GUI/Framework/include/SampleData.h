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
#ifndef _Sample_Data_h_
#define _Sample_Data_h_

#include "CEGUI/String.h"

// forward declarations
#include "CEGUI/ForwardRefs.h"

namespace CEGUI
{
    class PixelBuffer;
}

enum SampleType
{
    ST_Module,
    ST_Lua,
    ST_Python,

    ST_Count
};

class Sample;

class SampleData
{
public:
    SampleData(CEGUI::String sampleName, CEGUI::String summary,
        CEGUI::String description, SampleType sampleTypeEnum, CEGUI::String credits);
    virtual ~SampleData();

    virtual void initialise(int width, int height);
    virtual void deinitialise();

    virtual NII::UISheet * getGuiContext();
    virtual void handleNewWindowSize(NCount width, NCount height);

    void setTextureTargetImageArea(float height, float width);
    virtual CEGUI::PixelBuffer & getRTTImage();

    virtual void setGUIContextRTT();
    virtual void clearRTTTexture();

    virtual void onEnteringSample() = 0;
    virtual void update(float timeSinceLastUpdate) {};

    void setSampleWindow(CEGUI::Window* sampleWindow);
    CEGUI::Window * getSampleWindow();

    CEGUI::String getName();
    CEGUI::String getSummary();
    CEGUI::String getSampleTypeString();
    CEGUI::String getVerbose();
    CEGUI::String getUsedFilesString();
    CEGUI::String getCredits();
protected:
    CEGUI::String d_name;
    CEGUI::String d_summary;
    CEGUI::String d_description;
    SampleType d_type;
    CEGUI::String d_usedFilesString;
    CEGUI::String d_credits;

    CEGUI::Window * d_sampleWindow;

    NII::UISheet * d_guiContext;
    NII::UISubSheet * d_textureTarget;
    NII::PixelBuffer * d_textureTargetImage;
};

class SampleDataModule : public SampleData
{
public:
    SampleDataModule(CEGUI::String sampleName, CEGUI::String summary,
    CEGUI::String description, SampleType sampleTypeEnum, CEGUI::String credits);
    virtual ~SampleDataModule();

    virtual void getSampleInstanceFromDLL();

    virtual void initialise(NCount width, NCount height);
    virtual void deinitialise();

    virtual void onEnteringSample();

    virtual void update(float timeSinceLastUpdate);
private:

    NII::DynamicModule * d_dynamicModule;
    Sample * d_sample;
};

#endif

