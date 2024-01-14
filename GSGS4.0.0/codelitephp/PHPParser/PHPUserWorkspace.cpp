#include "PHPUserWorkspace.h"
#include <globals.h>
#include "gsgsJSON.h"

PHPUserWorkspace::PHPUserWorkspace(const wxString& workspacePath)
    : m_workspacePath(workspacePath)
{
}

PHPUserWorkspace::~PHPUserWorkspace()
{
}

wxFileName PHPUserWorkspace::GetFileName() const
{
    wxFileName workspaceFile( m_workspacePath );
    wxFileName fn(workspaceFile.GetPath(), workspaceFile.GetFullName() + "." + gsgs::GetUserName());
    fn.AppendDir(".gsgsworkspace");
    if ( !fn.FileExists() ) {
        fn.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    }
    return fn;
}

PHPUserWorkspace& PHPUserWorkspace::Load()
{
    wxFileName fn = GetFileName();
    JSON root(fn);
    JSONObject element = root.getObject();
    m_breakpoints.clear();
    
    JSONObject bpArr = element.getObject("m_breakpoints");
    int bpcount = bpArr.getArraySize();
    for( int i=0; i<bpcount; ++i ) {
        XDebugBreakpoint bp;
        bp.FromJSON( bpArr.getArray(i) );
        m_breakpoints.push_back( bp );
    }
    return *this;
}

PHPUserWorkspace& PHPUserWorkspace::Save()
{
    // Serialize the breakpoints
    JSON root(cJSON_Object);
    JSONObject json = root.getObject();
    JSONObject bpArr = JSONObject::createArray("m_breakpoints");
    json.addProperty( bpArr );
    
    XDebugBreakpoint::List_t::const_iterator iter = m_breakpoints.begin();
    for( ; iter != m_breakpoints.end(); ++iter ) {
        bpArr.arrayAppend( iter->ToJSON() );
    }
    root.save( GetFileName() );
    return *this;
}
