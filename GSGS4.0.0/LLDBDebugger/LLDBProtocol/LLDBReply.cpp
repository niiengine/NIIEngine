
#include "LLDBReply.h"
#include "LLDBEnums.h"

LLDBReply::~LLDBReply() {}

LLDBReply::LLDBReply(const wxString& str)
{
    JSON root(str);
    FromJSON(root.getObject());
}

void LLDBReply::FromJSON(const JSONObject& json)
{
    m_replyType = json.getObject("m_replyType").toInt(kReplyTypeInvalid);
    m_interruptResaon = json.getObject("m_stopResaon").toInt(kInterruptReasonNone);
    m_line = json.getObject("m_line").toInt(wxNOT_FOUND);
    m_filename = json.getObject("m_filename").toString();
    m_lldbId = json.getObject("m_lldbId").toInt();
    m_expression = json.getObject("m_expression").toString();
    m_debugSessionType = json.getObject("m_debugSessionType").toInt(kDebugSessionTypeNormal);
    m_text = json.getObject("m_text").toString();
    
    m_breakpoints.clear();
    JSONObject arr = json.getObject("m_breakpoints");
    for(int i = 0; i < arr.getArraySize(); ++i) {
        LLDBBreakpoint::Ptr_t bp(new LLDBBreakpoint());
        bp->FromJSON(arr.getArray(i));
        m_breakpoints.push_back(bp);
    }

    m_variables.clear();
    JSONObject localsArr = json.getObject("m_locals");
    m_variables.reserve(localsArr.getArraySize());
    for(int i = 0; i < localsArr.getArraySize(); ++i) {
        LLDBVariable::Ptr_t variable(new LLDBVariable());
        variable->FromJSON(localsArr.getArray(i));
        m_variables.push_back(variable);
    }

    m_backtrace.Clear();
    JSONObject backtrace = json.getObject("m_backtrace");
    m_backtrace.FromJSON(backtrace);

    m_threads = LLDBThread::FromJSON(json, "m_threads");
}

JSONObject LLDBReply::ToJSON() const
{
    JSONObject json = JSONObject::createObject();
    json.addProperty("m_replyType", m_replyType);
    json.addProperty("m_stopResaon", m_interruptResaon);
    json.addProperty("m_line", m_line);
    json.addProperty("m_filename", m_filename);
    json.addProperty("m_lldbId", m_lldbId);
    json.addProperty("m_expression", m_expression);
    json.addProperty("m_debugSessionType", m_debugSessionType);
    json.addProperty("m_text", m_text);
    JSONObject bparr = JSONObject::createArray("m_breakpoints");
    json.append(bparr);
    for(size_t i = 0; i < m_breakpoints.size(); ++i) {
        bparr.arrayAppend(m_breakpoints.at(i)->ToJSON());
    }

    JSONObject localsArr = JSONObject::createArray("m_locals");
    json.append(localsArr);
    for(size_t i = 0; i < m_variables.size(); ++i) {
        localsArr.arrayAppend(m_variables.at(i)->ToJSON());
    }

    json.addProperty("m_backtrace", m_backtrace.ToJSON());
    json.append(LLDBThread::ToJSON(m_threads, "m_threads"));
    return json;
}

void LLDBReply::UpdatePaths(const LLDBPivot& pivot)
{
    if(pivot.IsValid()) {

        // Update the file name
        m_filename = pivot.ToLocal(m_filename);

        // Update the breakpoint list
        for(size_t i = 0; i < m_breakpoints.size(); ++i) {
            m_breakpoints.at(i)->SetFilename(pivot.ToLocal(m_breakpoints.at(i)->GetFilename()), false);
        }

        // Update the call stack entries
        LLDBBacktrace::EntryVec_t callstack = m_backtrace.GetCallstack();
        for(size_t i = 0; i < callstack.size(); ++i) {
            callstack.at(i).filename = pivot.ToLocal(callstack.at(i).filename);
        }
        m_backtrace.SetCallstack(callstack);

        // Update the thread stack
        for(size_t i = 0; i < m_threads.size(); ++i) {
            m_threads.at(i).SetFile(pivot.ToLocal(m_threads.at(i).GetFile()));
        }
    }
}
