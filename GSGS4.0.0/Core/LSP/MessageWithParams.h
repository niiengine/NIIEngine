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

#ifndef REQUESTMESSAGE_H
#define REQUESTMESSAGE_H

#include "LSP/Message.h" // Base class: LSP::Message
#include "LSP/basic_types.h"
#include "json_rpc_params.h"
#include <wx/sharedptr.h>
#include <wx/event.h>
#include "LSP/ResponseMessage.h"

namespace LSP
{

class _gsgsAPI MessageWithParams : public LSP::Message
{
public:
    typedef wxSharedPtr<LSP::MessageWithParams> Ptr_t;

protected:
    wxString m_method;
    Params::Ptr_t m_params;
    wxString m_uuid;
    wxString m_statusMessage; // A custom message to be displayed to the user when this message is being processed

public:
    MessageWithParams();
    virtual ~MessageWithParams();
    virtual JSONObject ToJSON(const wxString& name) const;
    virtual void FromJSON(const JSONObject& json);
    virtual std::string ToString() const;

    void SetMethod(const wxString& method) { this->m_method = method; }
    const wxString& GetMethod() const { return m_method; }
    void SetParams(Params::Ptr_t params) { m_params = params; }
    Params::Ptr_t GetParams() { return m_params; }

    /**
     * @brief factory method for creating Ptr_t type
     */
    static LSP::MessageWithParams::Ptr_t MakeRequest(LSP::MessageWithParams* message_ptr);

    void SetStatusMessage(const wxString& statusMessage) { this->m_statusMessage = statusMessage; }
    const wxString& GetStatusMessage() const { return m_statusMessage; }
};

} // namespace LSP

#endif // REQUESTMESSAGE_H
