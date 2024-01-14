#include "LSP/json_rpc_results.h"

void LSP::ResultString::FromJSON(const JSONObject& json) { json.getValue(m_text); }
void LSP::ResultNumber::FromJSON(const JSONObject& json) { json.getValue(m_number, m_number); }
void LSP::ResultBoolean::FromJSON(const JSONObject& json) { json.getValue(m_value, m_value); }
