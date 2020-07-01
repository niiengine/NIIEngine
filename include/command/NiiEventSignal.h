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

#ifndef _NII_EVENT_SIGNAL_H_
#define _NII_EVENT_SIGNAL_H_

#include "NiiPreInclude.h"
#include "NiiEvent.h"

namespace NII
{
namespace NII_COMMAND
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI EventSignal : public SignalPtr
    {
    public:
        /**
        */
        EventSignal();

        /**
        */
        EventSignal(const SignalPtr & signal);

        ~EventSignal();

        /**
        */
        EventSignal & operator=(const SignalPtr & signal);

        /**
        */
        bool connected() const;
        
        /**
        */
        void disconnect();
    private:
        SignalPtr mSignal;
    };
}
}
#endif