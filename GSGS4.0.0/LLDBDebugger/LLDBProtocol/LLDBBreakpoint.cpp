
#include "LLDBBreakpoint.h"
#include "JSON.h"
#include <wx/filename.h>
#include "gsgsWorkspace.h"

static int s_internalGdbBpId = 50000;

LLDBBreakpoint::~LLDBBreakpoint()
{
}

LLDBBreakpoint::LLDBBreakpoint(const wxFileName& filename, int line)
    : m_id(wxNOT_FOUND)
    , m_type(kFileLine)
    , m_lineNumber(line)
{
    SetFilename(filename.GetFullPath());
}

LLDBBreakpoint::LLDBBreakpoint(const wxString& name)
    : m_id(wxNOT_FOUND)
    , m_type(kFunction)
    , m_name(name)
    , m_lineNumber(wxNOT_FOUND)
{
}

bool LLDBBreakpoint::SameAs(LLDBBreakpoint::Ptr_t other) const
{
    if ( m_type == kFunction ) {
        return  m_type == other->m_type &&
                m_filename == other->m_filename &&
                m_lineNumber == other->m_lineNumber
                && m_name == other->m_name;
    } else {
        return  m_type == other->m_type &&
                m_filename == other->m_filename &&
                m_lineNumber == other->m_lineNumber;
    }
}

LLDBBreakpoint::Vec_t LLDBBreakpoint::FromBreakpointInfoVector(const BreakpointInfo::Vec_t& breakpoints)
{
    LLDBBreakpoint::Vec_t bps;
    for(size_t i=0; i<breakpoints.size(); ++i) {
        if ( breakpoints.at(i).bp_type == BP_type_break ) {
            LLDBBreakpoint::Ptr_t bp(new LLDBBreakpoint() );
            const BreakpointInfo &gdbBp = breakpoints.at(i);
            bp->SetName( gdbBp.function_name );
            bp->SetFilename( gdbBp.file );
            bp->SetLineNumber( gdbBp.lineno );
            bp->SetType( kFileLine );
            bps.push_back( bp );
        }
    }
    return bps;
}

BreakpointInfo::Vec_t LLDBBreakpoint::ToBreakpointInfoVector(const LLDBBreakpoint::Vec_t& breakpoints)
{
    BreakpointInfo::Vec_t bps;
    for(size_t i=0; i<breakpoints.size(); ++i) {

        LLDBBreakpoint::Ptr_t bp = breakpoints.at(i);
        BreakpointInfo gdbBp;
        gdbBp.Create(bp->GetFilename(), bp->GetLineNumber(), ++ s_internalGdbBpId);
        gdbBp.bp_type = BP_type_break;

        // dont add breakpoints to a non existent location
        bps.push_back( gdbBp );
    }
    return bps;
}

wxString LLDBBreakpoint::ToString() const
{
    wxString str;
    str << "Breakpoint ID=" << m_id << ". ";
    if ( GetType() == kFileLine ) {
         str << "kFileLine " << GetFilename() << "," << GetLineNumber();

    } else if ( GetType() == kFunction ) {
        str << "kFunction " << GetName() << "()";
    }
    return str;
}

bool LLDBBreakpoint::IsValid() const
{
    switch ( GetType() ) {
    case kFileLine:
        return m_lineNumber != wxNOT_FOUND && !m_filename.IsEmpty();
    case kFunction:
        return !m_name.IsEmpty();
    case kInvalid:
    default:
        return false;
    }
}

void LLDBBreakpoint::Copy(LLDBBreakpoint::Ptr_t other)
{
    m_type       = other->m_type;
    m_name       = other->m_name;
    m_filename   = other->m_filename;
    m_lineNumber = other->m_lineNumber;
    m_children   = other->m_children;
    m_id = wxNOT_FOUND;
}

void LLDBBreakpoint::Invalidate()
{
    m_id = wxNOT_FOUND;
    // remove all children for this breakpoint
    m_children.clear();
}

void LLDBBreakpoint::FromJSON(const JSONObject& json)
{
    m_children.clear();
    m_id = json.getObject("m_id").toInt(wxNOT_FOUND);
    m_type = json.getObject("m_type").toInt(kInvalid);
    m_name = json.getObject("m_name").toString();
    SetFilename(json.getObject("m_filename").toString(), false);
    m_lineNumber = json.getObject("m_lineNumber").toInt();
    JSONObject arr = json.getObject("m_children");
    for(int i=0; i<arr.getArraySize(); ++i) {
        LLDBBreakpoint::Ptr_t bp(new LLDBBreakpoint() );
        bp->FromJSON( arr.getArray(i) );
        m_children.push_back( bp );
    }
}

JSONObject LLDBBreakpoint::ToJSON() const
{
    JSONObject json = JSONObject::createObject();
    json.addProperty("m_id", m_id);
    json.addProperty("m_type", m_type);
    json.addProperty("m_name", m_name);
    json.addProperty("m_filename", m_filename);
    json.addProperty("m_lineNumber", m_lineNumber);

    JSONObject arr = JSONObject::createArray("m_children");
    json.append( arr );
    for(size_t i=0; i<m_children.size(); ++i) {
        arr.arrayAppend( m_children.at(i)->ToJSON() );
    }
    return json;
}
