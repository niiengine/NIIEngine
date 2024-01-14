#include "CallFrame.h"
#include "CallFrameScope.h"

CallFrame::CallFrame() {}

CallFrame::~CallFrame() {}

gsgs::JSONObject CallFrame::ToJSON(const wxString& name) const
{
    gsgs::JSONObject json = gsgs::JSONObject::createObject(name);
    json.addProperty("callFrameId", m_callFrameId);
    json.addProperty("functionName", m_functionName);
    json.arrayAppend(m_location.ToJSON("location"));
    json.arrayAppend(m_this.ToJSON("this"));
    gsgs::JSONObject arr = gsgs::JSONObject::createArray("scopeChain");
    for(size_t i = 0; i < m_scopeChain.size(); ++i) {
        nSerializableObject::Ptr_t p = m_scopeChain[i];
        arr.arrayAppend(p->To<CallFrameScope>()->ToJSON(""));
    }
    json.arrayAppend(arr);
    return json;
}

void CallFrame::FromJSON(const gsgs::JSONObject& json)
{
    json.getObject("callFrameId").getValue(m_callFrameId);
    json.getObject("functionName").getValue(m_functionName);
    m_location.FromJSON(json.getObject("location"));
    m_this.FromJSON(json.getObject("this"));
    gsgs::JSONObject scopChain = json.getObject("scopeChain");
    int arr_size = scopChain.getArraySize();
    for(int i = 0; i < arr_size; ++i) {
        gsgs::JSONObject scope = scopChain.getArray(i);
        CallFrameScope::Ptr_t pScope(new CallFrameScope());
        pScope->FromJSON(scope);
        m_scopeChain.push_back(pScope);
    }
}
