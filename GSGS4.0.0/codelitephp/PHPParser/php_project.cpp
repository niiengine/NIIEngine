#include "FilesCollector.h"
#include "gsgsLogManager.h"
#include "php_project.h"
#include "php_workspace.h"
#include <plugin.h>
#include <wx/busyinfo.h>
#include <wx/msgdlg.h>
#include <wx/utils.h>
#include "gsgsFileUtil.h"

wxDEFINE_EVENT(wxEVT_PHP_PROJECT_FILES_SYNC_START, gsgs::Event);
wxDEFINE_EVENT(wxEVT_PHP_PROJECT_FILES_SYNC_END, gsgs::Event);

class PHPProjectSyncThread : public wxThread
{
public:
    struct Data {
        wxString excludes;
        wxString includes;
        wxString directory;
        wxString projectName;
    };

protected:
    wxEvtHandler* m_owner;
    PHPProjectSyncThread::Data m_projectData;

public:
    PHPProjectSyncThread(wxEvtHandler* owner, const PHPProjectSyncThread::Data& data)
        : wxThread(wxTHREAD_DETACHED)
        , m_owner(owner)
        , m_projectData(data)
    {
    }

    virtual ~PHPProjectSyncThread() {}
    void* Entry()
    {
        gsgsDEBUG() << "Scanning files for project:" << m_projectData.projectName << "..." << clEndl;

        // Report the start event
        {
            gsgs::Event event(wxEVT_PHP_PROJECT_FILES_SYNC_START);
            event.SetString(m_projectData.projectName);
            m_owner->AddPendingEvent(event);
        }

        // Scan and collect the files
        gsgs::Event event(wxEVT_PHP_PROJECT_FILES_SYNC_END);
        FilesCollector collector(event.GetStrings(), m_projectData.includes, m_projectData.excludes, NULL);
        collector.Collect(m_projectData.directory);

        // Notify about sync completed
        event.SetString(m_projectData.projectName);
        m_owner->AddPendingEvent(event);

        gsgsDEBUG() << "Scanning files for project:" << m_projectData.projectName << "... is completed" << clEndl;
        return NULL;
    }

    void Start()
    {
        Create();
        Run();
    }
};

PHPProject::PHPProject()
    : m_isActive(false)
    , m_importFileSpec(
          "*.php;*.php5;*.inc;*.phtml;*.js;*.html;*.css;*.scss;*.less;*.json;*.xml;*.ini;*.md;*.txt;*.text;."
          "htaccess;*.ctp;*.sql")
    , m_excludeFolders(".git;.svn;.codelite;.clang")
{
    Bind(wxEVT_PHP_PROJECT_FILES_SYNC_START, &PHPProject::OnFileScanStart, this);
    Bind(wxEVT_PHP_PROJECT_FILES_SYNC_END, &PHPProject::OnFileScanEnd, this);
}

PHPProject::~PHPProject()
{
    Unbind(wxEVT_PHP_PROJECT_FILES_SYNC_START, &PHPProject::OnFileScanStart, this);
    Unbind(wxEVT_PHP_PROJECT_FILES_SYNC_END, &PHPProject::OnFileScanEnd, this);
}

void PHPProject::FromJSON(const JSONObject& element)
{
    element.getObject("m_importFileSpec").getValue(m_importFileSpec, m_importFileSpec);
    element.getObject("m_excludeFolders").getValue(m_excludeFolders, m_excludeFolders);
    element.getObject("m_name").getValue(m_name);
    element.getObject("m_isActive").getValue(m_isActive);
    m_settings.FromJSON(element.getObject("settings"));
}

void PHPProject::ToJSON(JSONObject& pro) const
{
    pro.addProperty("m_name", m_name);
    pro.addProperty("m_isActive", m_isActive);
    pro.addProperty("m_importFileSpec", m_importFileSpec);
    pro.addProperty("m_excludeFolders", m_excludeFolders);
    pro.addProperty(m_settings.ToJSON());
}

wxArrayString& PHPProject::GetFiles(wxProgressDialog* progress)
{
    wxUnusedVar(progress);
    return m_files;
}

void PHPProject::GetFilesArray(wxArrayString& files) const
{
    files.Alloc(m_files.size());
    files.insert(files.end(), m_files.begin(), m_files.end());
}

void PHPProject::Create(const wxFileName& filename, const wxString& name)
{
    m_filename = filename;
    m_name = name;
    Save();
}

void PHPProject::Load(const wxFileName& filename)
{
    m_filename = filename;
    JSON root(m_filename);
    FromJSON(root.getObject());
}

void PHPProject::Save()
{
    JSON root(cJSON_Object);
    JSONObject pro = root.getObject();
    ToJSON(pro);
    root.save(m_filename);
}

void PHPProject::FolderDeleted(const wxString& name, bool notify)
{
    // Normalize the folder name by using wxFileName
    wxFileName folder(name, "");
    wxString folderRemoved = folder.GetPath();

    wxArrayString updatedArray;
    wxArrayString deletedFiles;

    updatedArray.Alloc(m_files.size());
    deletedFiles.Alloc(m_files.size());
    for(size_t i = 0; i < m_files.GetCount(); ++i) {
        if(!m_files.Item(i).StartsWith(name)) {
            updatedArray.Add(m_files.Item(i));
        } else {
            deletedFiles.Add(m_files.Item(i));
        }
    }

    // Free extra memory
    updatedArray.Shrink();
    deletedFiles.Shrink();

    // Update the list
    m_files.swap(updatedArray);
    m_files.Sort();
    if(notify) {
        gsgs::Event event(wxEVT_PROJ_FILE_REMOVED);
        event.SetStrings(deletedFiles);
        gsgs_Event().AddPendingEvent(event);
    }
}

void PHPProject::FileRenamed(const wxString& oldname, const wxString& newname, bool notify)
{
    int where = m_files.Index(oldname);
    if(where != wxNOT_FOUND) { m_files.Item(where) = newname; }

    if(notify && (where != wxNOT_FOUND)) {
        {
            wxArrayString arr;
            arr.Add(oldname);
            gsgs::Event event(wxEVT_PROJ_FILE_REMOVED);
            event.SetStrings(arr);
            gsgs_Event().AddPendingEvent(event);
        }
        {
            wxArrayString arr;
            arr.Add(newname);
            gsgs::Event event(wxEVT_PROJ_FILE_ADDED);
            event.SetStrings(arr);
            gsgs_Event().AddPendingEvent(event);
        }

        // And finally notify about rename-event
        gsgs::FileEvent renameEvent(wxEVT_FILE_RENAMED);
        renameEvent.SetPath(oldname);
        renameEvent.SetNewpath(newname);
        gsgs_Event().AddPendingEvent(renameEvent);
    }
}

void PHPProject::SynchWithFileSystem()
{
    m_files.Clear();
    // Call GetFiles so the m_files will get populated again
    GetFiles(NULL);
}

void PHPProject::FilesDeleted(const wxArrayString& files, bool notify)
{
    if(files.IsEmpty()) return;

    // Normalize the folder name by using wxFileName
    for(size_t i = 0; i < files.GetCount(); ++i) {
        int where = m_files.Index(files.Item(i));
        if(where != wxNOT_FOUND) { m_files.RemoveAt(where); }
    }

    if(notify) {
        gsgs::Event event(wxEVT_PROJ_FILE_REMOVED);
        event.SetStrings(files);
        gsgs_Event().AddPendingEvent(event);
    }
}

bool PHPProject::HasFile(const wxFileName& filename) const
{
    return filename.GetFullPath().StartsWith(GetFilename().GetPath());
}

void PHPProject::FileAdded(const wxString& filename, bool notify)
{
    if(m_files.Index(filename) == wxNOT_FOUND) {
        m_files.Add(filename);
        m_files.Sort();
    }

    if(notify) {
        gsgs::Event event(wxEVT_PROJ_FILE_ADDED);
        wxArrayString files;
        files.Add(filename);
        event.SetStrings(files);
        gsgs_Event().AddPendingEvent(event);
    }
}

void PHPProject::FolderAdded(const wxString& folderpath)
{
    wxFileName fakeFile(folderpath, FOLDER_MARKER);
    if(m_files.Index(fakeFile.GetFullPath()) == wxNOT_FOUND) {
        m_files.Add(fakeFile.GetFullPath());
        m_files.Sort();
    }
}

void PHPProject::SyncWithFileSystemAsync(wxEvtHandler* owner)
{
    // Build the info needed for the thread
    PHPProjectSyncThread::Data data;
    data.directory = GetFilename().GetPath();
    data.projectName = GetName();
    data.excludes = m_excludeFolders;
    data.includes = m_importFileSpec;

    // Start the background thread, a detached one which deletes itself at the end
    PHPProjectSyncThread* thr = new PHPProjectSyncThread((owner ? owner : this), data);
    thr->Start();
}

void PHPProject::OnFileScanStart(gsgs::Event& event) {}

void PHPProject::OnFileScanEnd(gsgs::Event& event) { m_files.swap(event.GetStrings()); }

void PHPProject::SetFiles(const wxArrayString& files) { m_files = files; }
