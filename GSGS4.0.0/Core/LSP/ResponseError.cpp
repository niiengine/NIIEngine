#include "ResponseError.h"

LSP::ResponseError::ResponseError() {}

LSP::ResponseError::ResponseError(const wxString& message)
{
    JSON json(message);
    FromJSON(json.getObject());
}

LSP::ResponseError::~ResponseError() {}

void LSP::ResponseError::FromJSON(const JSONObject& json)
{
    if(json.isObjectExist("error")) {
        // serialize the parent
        this->Message::FromJSON(json);

        // extract the error message
        JSONObject error = json.getObject("error");
        // an error message
        error.getObject("code").getValue(m_errorCode);
        error.getObject("message").getValue(m_message);
    }
}

std::string LSP::ResponseError::ToString() const { return ""; }
