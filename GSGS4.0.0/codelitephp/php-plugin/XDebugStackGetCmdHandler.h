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

#ifndef XDEBUGSTACKGETCMDHANDLER_H
#define XDEBUGSTACKGETCMDHANDLER_H

#include "XDebugCommandHandler.h"
class XDebugStackGetCmdHandler : public XDebugCommandHandler
{
    int m_requestedStack;
public:
    XDebugStackGetCmdHandler(XDebugManager* mgr, int transcationId, int requestedStack = 0);
    virtual ~XDebugStackGetCmdHandler();

    virtual void Process(const wxXmlNode* response);
};

#endif // XDEBUGSTACKGETCMDHANDLER_H
