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

#include "Samples_xmlHandler.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/Logger.h"
#include "NiiPropertyData.h"
#include "SamplesFramework.h"
#include "Sample.h"
#include "SampleData.h"

    //------------------------------------------------------------------------
    const String Samples_xmlHandler::ElementName("Samples");

    const String SampleDataHandler::ElementName("SampleDefinition");
    const String SampleDataHandler::NameAttribute("name");
    const String SampleDataHandler::SummaryAttribute("summary");
    const String SampleDataHandler::DescriptionAttribute("description");
    const String SampleDataHandler::CreditsAttribute("credits");
    const String SampleDataHandler::SampleTypeAttribute("type");
    const String SampleDataHandler::SampleTypeCppModule("C++ Module");
    const String SampleDataHandler::SampleTypeLua("Lua");
    const String SampleDataHandler::SampleTypePython("Python");
    //------------------------------------------------------------------------
    Samples_xmlHandler::Samples_xmlHandler(SamplesFramework * samplesFramework)
        : d_samplesFramework(samplesFramework)
    {
    }
    //------------------------------------------------------------------------
    Samples_xmlHandler::~Samples_xmlHandler()
    {
    }
    //------------------------------------------------------------------------
    const String & Samples_xmlHandler::getXSD() const
    {
        static String re("Samples.xsd");
        return re;
    }
    //------------------------------------------------------------------------
    void Samples_xmlHandler::onComBegin(PropertyID pid, const PropertyData * pdc)
    {
        if(pid == ElementName)
        {
            CEGUI_LOGINSANE("===== Begin Animations parsing =====");
        }
        else if(pid == SampleDataHandler::ElementName)
        {
            mTag = new SampleDataHandler(pdc, "", d_samplesFramework);
        }
        else
            Logger::getOnly().logEvent("Animation_xmlHandler::onBegin: "
                "<" + pid + "> is invalid at this location.", Errors);
    }
    //------------------------------------------------------------------------
    void Samples_xmlHandler::onComEnd(PropertyID pid)
    {
        if(pid == ElementName)
        {
            CEGUI_LOGINSANE("===== End Animations parsing =====");
        }
        else
            Logger::getOnly().logEvent("Animation_xmlHandler::onEnd: "
                "</" + pid + "> is invalid at this location.", Errors);
    }
    //------------------------------------------------------------------------
    SampleDataHandler::SampleDataHandler(const PropertyData * pdc,
        const String & name_prefix, SamplesFramework * samplesFramework) :
            d_samplesFramework(samplesFramework)
    {
        const String & sampleName(pdc->getValue(NameAttribute));
        const String & summary(pdc->getValue(SummaryAttribute));
        const String & description(pdc->getValue(DescriptionAttribute));
        const String & sampleType(pdc->getValue(SampleTypeAttribute));
        const String & credits(pdc->getValue(CreditsAttribute));

        CEGUI_LOGINSANE(
            "Defining sample named: " +
            sampleName +
            "  Summary: " +
            summary +
            "  Description: " +
            description +
            "  Sample Type: " +
            sampleType +
            "  Credits: " +
            credits
            );

        SampleType sampleTypeEnum;

        if(sampleType == SampleTypeCppModule)
            sampleTypeEnum = ST_Module;
        else if(sampleType == SampleTypePython)
            sampleTypeEnum = ST_Python;
        else
            sampleTypeEnum = ST_Lua;

        d_samplesFramework->addSampleDataCppModule(sampleName, summary, description, sampleTypeEnum, credits);
    }
    //------------------------------------------------------------------------
    SampleDataHandler::~SampleDataHandler()
    {
    }
    //------------------------------------------------------------------------
    void SampleDataHandler::onComBegin(PropertyID pid, const PropertyData * pdc)
    {
        Logger::getOnly().logEvent(
            "AnimationXmlAnalyze::onBegin: "
            "<" + pid + "> is invalid at this location.", Errors);
    }
    //------------------------------------------------------------------------
    void SampleDataHandler::onComEnd(PropertyID pid)
    {
        // set completed status when we encounter our own end element
        if (pid == ElementName)
            ;
    }
    //------------------------------------------------------------------------