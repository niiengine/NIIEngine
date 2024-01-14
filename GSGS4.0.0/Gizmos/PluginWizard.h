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

#ifndef PLUGINWIZARD_H
#define PLUGINWIZARD_H

#include "gizmos_base.h"
#include "newplugindata.h"

class PluginWizard : public PluginWizardBase
{
public:
    PluginWizard(wxWindow* parent);
    virtual ~PluginWizard();

    bool Run(NewPluginData& pd);
protected:
    virtual void OnProjectPathChanged(wxFileDirPickerEvent& event);
    virtual void OnFinish(wxWizardEvent& event);
    virtual void OnPageChanging(wxWizardEvent& event);
};
#endif // PLUGINWIZARD_H
