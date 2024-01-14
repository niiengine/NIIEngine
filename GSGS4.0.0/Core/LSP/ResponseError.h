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

#ifndef RESPONSEERROR_H
#define RESPONSEERROR_H

#include "gsgsPreInclude.h"
#include "gsgsJSON.h"
#include "LSP/Message.h"

namespace LSP
{

class _gsgsAPI ResponseError : public Message
{
    int m_errorCode = wxNOT_FOUND;
    wxString m_message;

public:
    enum eErrorCodes {
        kErrorCodeParseError = -32700,
        kErrorCodeInvalidRequest = -32600,
        kErrorCodeMethodNotFound = -32601,
        kErrorCodeInvalidParams = -32602,
        kErrorCodeInternalError = -32603,
        kErrorCodeserverErrorStart = -32099,
        kErrorCodeserverErrorEnd = -32000,
        kErrorCodeServerNotInitialized = -32002,
        kErrorCodeUnknownErrorCode = -32001,
        kErrorCodeRequestCancelled = -32800,
        kErrorCodeContentModified = -32801,
    };

public:
    ResponseError(const wxString& message);
    ResponseError();
    virtual ~ResponseError();
    void FromJSON(const JSONObject& json);
    virtual std::string ToString() const;
    void SetErrorCode(int errorCode) { this->m_errorCode = errorCode; }
    void SetMessage(const wxString& message) { this->m_message = message; }
    int GetErrorCode() const { return m_errorCode; }
    const wxString& GetMessage() const { return m_message; }
    bool IsOk() const { return m_errorCode != wxNOT_FOUND; }
};

}; // namespace LSP

#endif // RESPONSEERROR_H
