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

#ifndef XDEBUGUNKNOWNCOMMAND_H
#define XDEBUGUNKNOWNCOMMAND_H

#include "XDebugCommandHandler.h" // Base class: XDebugCommandHandler

class XDebugUnknownCommand : public XDebugCommandHandler
{
public:
    XDebugUnknownCommand(XDebugManager* mgr, int transcationId);
    virtual ~XDebugUnknownCommand();

public:
    virtual void Process(const wxXmlNode* response);
};

#endif // XDEBUGUNKNOWNCOMMAND_H
