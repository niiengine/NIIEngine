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
#ifndef __contextdiff__
#define __contextdiff__

namespace gsgs
{
    class Editor;
    
    class ContextDiff : public ContextBase 
    {
    public:
        ContextDiff();
        ContextDiff(Editor *container);
        ~ContextDiff();

        virtual void ApplySettings();
        virtual ContextBase* NewInstance(Editor *container);
        
        // ctrl-click style navigation support
        virtual int  GetHyperlinkRange(int pos, int &start, int &end);
        virtual void GoHyperlink(int start, int end, int type, bool alt);
    };
}
#endif // __contextdiff__
