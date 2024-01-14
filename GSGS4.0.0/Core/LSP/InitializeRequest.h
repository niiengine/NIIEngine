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

#ifndef INITIALIZEREQUEST_H
#define INITIALIZEREQUEST_H

#include "LSP/Request.h"
namespace LSP
{

class _gsgsAPI InitializeRequest : public LSP::Request
{
    int m_processId = wxNOT_FOUND;
    wxString m_rootUri;

public:
    InitializeRequest(const wxString& rootUri = "");
    virtual ~InitializeRequest();
    InitializeRequest& SetProcessId(int processId)
    {
        this->m_processId = processId;
        return *this;
    }
    InitializeRequest& SetRootUri(const wxString& rootUri)
    {
        this->m_rootUri = rootUri;
        return *this;
    }
    int GetProcessId() const { return m_processId; }
    const wxString& GetRootUri() const { return m_rootUri; }
    JSONObject ToJSON(const wxString& name) const;
    void OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner);
    bool IsPositionDependantRequest() const { return false; }
};
};     // namespace LSP
#endif // INITIALIZEREQUEST_H
