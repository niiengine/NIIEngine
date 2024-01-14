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

#ifndef PHPSYMBOLSCACHER_H
#define PHPSYMBOLSCACHER_H

#include "gsgsJob.h" // Base class: Job
#include <wx/string.h>
#include <wx/event.h>

class PHPCodeCompletion;
class PHPSymbolsCacher : public gsgs::Job
{
    PHPCodeCompletion* m_owner;
    wxString m_filename;
    
public:
    PHPSymbolsCacher(PHPCodeCompletion* owner, const wxString& dbfile);
    virtual ~PHPSymbolsCacher();

public:
    virtual void Process(wxThread* thread);
};

#endif // PHPSYMBOLSCACHER_H
