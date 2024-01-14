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

#ifndef CSSCODECOMPLETION_H
#define CSSCODECOMPLETION_H

#include "gsgsSharedPtr.h"
#include "gsgsEvent.h"

class WebTools;
class CSSCodeCompletion : public ServiceProvider
{
public:
    typedef SharedPtr<CSSCodeCompletion> Ptr_t;
    struct Entry {
        wxString property;
        wxArrayString values;
        typedef std::vector<Entry> Vec_t;
    };

protected:
    bool m_isEnabled;
    Entry::Vec_t m_entries;
    WebTools* m_plugin = nullptr;

protected:
    wxString GetPreviousWord(IEditor* editor, int pos);
    void OnCodeComplete(CodeCompleteEvent& event);

public:
    CSSCodeCompletion(WebTools* plugin);
    virtual ~CSSCodeCompletion();

    void CssCodeComplete(IEditor* editor);
};

#endif // CSSCODECOMPLETION_H
