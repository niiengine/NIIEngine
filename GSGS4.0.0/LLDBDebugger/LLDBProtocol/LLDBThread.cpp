
#include "LLDBThread.h"
#include "LLDBEnums.h"

LLDBThread::LLDBThread()
    : m_id(wxNOT_FOUND)
    , m_line(wxNOT_FOUND)
    , m_active(false)
    , m_suspended(false)
    , m_stopReason( kStopReasonInvalid )
{
}

LLDBThread::~LLDBThread()
{
}

void LLDBThread::FromJSON(const JSONObject& json)
{
    m_id   = json.getObject("m_id").toInt();
    m_func = json.getObject("m_func").toString();
    m_file = json.getObject("m_file").toString();
    m_line = json.getObject("m_line").toInt();
    m_active = json.getObject("m_active").toBool();
    m_suspended = json.getObject("m_suspended").toBool();
    m_stopReason = json.getObject("m_stopReason").toInt();
    m_stopReasonString = json.getObject("m_stopReasonString").toString();
    m_name = json.getObject("m_name").toString();
}

JSONObject LLDBThread::ToJSON() const
{
    JSONObject json = JSONObject::createObject();
    json.addProperty("m_id", m_id);
    json.addProperty("m_func", m_func);
    json.addProperty("m_file", m_file);
    json.addProperty("m_line", m_line);
    json.addProperty("m_active", m_active);
    json.addProperty("m_suspended", m_suspended);
    json.addProperty("m_stopReason", m_stopReason);
    json.addProperty("m_stopReasonString", m_stopReasonString);
    json.addProperty("m_name", m_name);
    return json;
}

JSONObject LLDBThread::ToJSON(const LLDBThread::Vect_t& threads, const wxString &name)
{
    JSONObject arr = JSONObject::createArray(name);
    for(size_t i=0; i<threads.size(); ++i) {
        arr.arrayAppend( threads.at(i).ToJSON() );
    }
    return arr;
}

LLDBThread::Vect_t LLDBThread::FromJSON(const JSONObject& json, const wxString& name)
{
    LLDBThread::Vect_t v;
    JSONObject arr = json.getObject(name);
    for(int i=0; i<arr.getArraySize(); ++i) {
        LLDBThread thr;
        thr.FromJSON( arr.getArray(i) );
        v.push_back( thr );
    }
    return v;
}
