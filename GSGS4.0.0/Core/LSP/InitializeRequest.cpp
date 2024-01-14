#include "InitializeRequest.h"
#include <wx/filesys.h>

LSP::InitializeRequest::InitializeRequest(const wxString& rootUri)
{
    SetMethod("initialize");
    m_processId = ::wxGetProcessId();
    m_rootUri = rootUri;
}

LSP::InitializeRequest::~InitializeRequest() {}

JSONObject LSP::InitializeRequest::ToJSON(const wxString& name) const
{
    JSONObject json = Request::ToJSON(name);

    // add the 'params'
    JSONObject params = JSONObject::createObject("params");
    json.addProperty(params);
    params.addProperty("processId", GetProcessId());
    if(GetRootUri().IsEmpty()) {
        JSON nullItem(cJSON_NULL);
        JSONObject nullObj = nullItem.getObject();
        params.addProperty(nullObj);
        (void)nullItem.release(); // dont delete it on destruction, it is now owned by 'params'
    } else {
        params.addProperty("rootUri", wxFileSystem::FileNameToURL(GetRootUri()));
    }
    JSONObject capabilities = JSONObject::createObject("capabilities");
    params.addProperty(capabilities);
    JSONObject textDocument = JSONObject::createObject("textDocument");
    capabilities.addProperty(textDocument);
    return json;
}

void LSP::InitializeRequest::OnResponse(const LSP::ResponseMessage& response, wxEvtHandler* owner)
{
    wxUnusedVar(response);
    wxUnusedVar(owner);
}
