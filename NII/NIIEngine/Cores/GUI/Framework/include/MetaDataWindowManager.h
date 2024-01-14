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
#ifndef _MetaDataWindowManager_h_
#define _MetaDataWindowManager_h_

namespace CEGUI
{
    class Window;
}

class SampleData;

class MetaDataWindowManager
{
public:
    MetaDataWindowManager(CEGUI::Window* metaDataWindow);
    virtual ~MetaDataWindowManager() {}

    void setSampleInfo(SampleData* sampleData);

    CEGUI::Window* getWindow();

private:
    MetaDataWindowManager(const MetaDataWindowManager&) {}
    MetaDataWindowManager& operator=(const MetaDataWindowManager&) {}

    void init();

    CEGUI::Window* d_root;

    CEGUI::Window* d_wndSampleType;
    CEGUI::Window* d_wndSummary;
    CEGUI::Window* d_wndDescription;
    CEGUI::Window* d_wndUsedFiles;
    CEGUI::Window* d_wndCredits;
};

#endif