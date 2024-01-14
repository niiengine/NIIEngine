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
#ifndef RESPONSEMESSAGE_H
#define RESPONSEMESSAGE_H

#include "LSP/Message.h"
#include <wx/sharedptr.h>
#include <gsgsPreInclude.h>
#include "LSP/basic_types.h"

namespace LSP
{

class _gsgsAPI ResponseMessage : public LSP::Message
{
    int m_id = wxNOT_FOUND;
    wxSharedPtr<JSON> m_json;
    wxString m_jsonMessage;
public:
    ResponseMessage(wxString& message);
    virtual ~ResponseMessage();
    virtual JSONObject ToJSON(const wxString& name) const;
    virtual void FromJSON(const JSONObject& json);

    virtual std::string ToString() const;
    ResponseMessage& SetId(int id)
    {
        this->m_id = id;
        return *this;
    }
    const wxString& GetMessageString() const { return m_jsonMessage; }
    int GetId() const { return m_id; }
    bool IsOk() const { return m_json && m_json->isValid(); }
    bool Has(const wxString& property) const;
    JSONObject Get(const wxString& property) const;

    /**
     * @brief is this a "textDocument/publishDiagnostics" message?
     */
    bool IsPushDiagnostics() const { wxString temp;  Get("method").getValue(temp); return temp == "textDocument/publishDiagnostics"; }
    
    /**
     * @brief return list of diagnostics
     */
    std::vector<LSP::Diagnostic> GetDiagnostics() const;
    /**
     * @brief return the URI diagnostics
     */
    wxString GetDiagnosticsUri() const;
protected:
    int ReadHeaders(const wxString& message, gsgsStringMap & headers);
};
}; // namespace LSP

#endif // RESPONSEMESSAGE_H
