#include "NodeJSWorkspaceUserConfiguration.h"
#include <globals.h>
#include "gsgsJSON.h"

NodeJSWorkspaceUser::NodeJSWorkspaceUser(const wxString& workspacePath)
    : m_workspacePath(workspacePath)
    , m_debuggerPort(5858)
    , m_debuggerHost("127.0.0.1")
{
}

NodeJSWorkspaceUser::~NodeJSWorkspaceUser() {}

wxFileName NodeJSWorkspaceUser::GetFileName() const
{
    wxFileName workspaceFile(m_workspacePath);
    wxFileName fn(workspaceFile.GetPath(), workspaceFile.GetFullName() + ".nodejs." + GetUserName());
    fn.AppendDir(".gsgsworkspace");
    if(!fn.FileExists()) {
        fn.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    }
    return fn;
}

NodeJSWorkspaceUser& NodeJSWorkspaceUser::Load()
{
    wxFileName fn = GetFileName();
    JSON root(fn);
    JSONObject element = root.getObject();

    element.getObject("m_debuggerPort").getValue(m_debuggerPort, m_debuggerPort);
    element.getObject("m_debuggerHost").getValue(m_debuggerHost, m_debuggerHost);
    element.getObject("m_scriptToExecute").getValue(m_scriptToExecute, m_scriptToExecute);
    element.getObject("m_commandLineArgs").getValue(m_commandLineArgs);
    element.getObject("m_workingDirectory").getValue(m_workingDirectory);

    m_breakpoints.clear();
    JSONObject bpArr = element.getObject("m_breakpoints");
    int bpcount = bpArr.getArraySize();
    for(int i = 0; i < bpcount; ++i) {
        NodeJSBreakpoint bp;
        bp.FromJSON(bpArr.getArray(i));
        m_breakpoints.push_back(bp);
    }
    return *this;
}

NodeJSWorkspaceUser& NodeJSWorkspaceUser::Save()
{
    // Serialize the breakpoints
    JSON root(cJSON_Object);
    JSONObject json = root.getObject();
    json.addProperty("m_debuggerPort", m_debuggerPort);
    json.addProperty("m_debuggerHost", m_debuggerHost);
    json.addProperty("m_scriptToExecute", m_scriptToExecute);
    json.addProperty("m_commandLineArgs", m_commandLineArgs);
    json.addProperty("m_workingDirectory", m_workingDirectory);
    JSONObject bpArr = JSONObject::createArray("m_breakpoints");
    json.arrayAppend(bpArr);

    NodeJSBreakpoint::Vec_t::const_iterator iter = m_breakpoints.begin();
    for(; iter != m_breakpoints.end(); ++iter) {
        bpArr.arrayAppend(iter->ToJSON(""));
    }
    root.save(GetFileName());
    return *this;
}
