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
#include "MetaDataWindowManager.h"

#include "SampleData.h"

#include "CEGUI/Window.h"
#include "CEGUI/SchemeManager.h"
#include "NiiUIWidgetManager.h"

using namespace CEGUI;

MetaDataWindowManager::MetaDataWindowManager(CEGUI::Window* metaDataWindow)
    : d_root(metaDataWindow)
{
    init();
}

void MetaDataWindowManager::init()
{
    d_wndSampleType = d_root->getChild("SampleType");
    d_wndSummary = d_root->getChild("Summary");
    d_wndDescription = d_root->getChild("Description");
    d_wndUsedFiles = d_root->getChild("UsedFiles");
    d_wndCredits = d_root->getChild("Credits");
}


CEGUI::Window* MetaDataWindowManager::getWindow()
{
    return d_root;
}

void MetaDataWindowManager::setSampleInfo(SampleData* sampleData)
{
    d_root->setText(sampleData->getName());
    d_wndSummary->setText(sampleData->getSummary());
    d_wndSampleType->setText(sampleData->getSampleTypeString());
    d_wndDescription->setText(sampleData->getVerbose());
    d_wndUsedFiles->setText(sampleData->getUsedFilesString());
    d_wndCredits->setText(sampleData->getCredits());
}