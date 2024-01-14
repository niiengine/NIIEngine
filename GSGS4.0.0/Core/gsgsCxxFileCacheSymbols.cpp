#include "gsgsCxxFileCacheSymbols.h"
#include "gsgsTagManager.h"
#include "gsgsLogManager.h"
#include "gsgsParseWorker.h"
#include "gsgsThread.h"
namespace gsgs
{
    //-----------------------------------------------------------------------------------
    wxDEFINE_EVENT(wxEVT_CXX_SYMBOLS_CACHE_UPDATED, gsgs::Event);
    wxDEFINE_EVENT(wxEVT_CXX_SYMBOLS_CACHE_INVALIDATED, gsgs::Event);
    //-----------------------------------------------------------------------------------
    class SourceToTagsThread : public wxThread
    {
    public:
        SourceToTagsThread(CxxFileCacheSymbols * cache)
            : wxThread(wxTHREAD_JOINABLE)
            , m_cache(cache){}

        virtual ~SourceToTagsThread() 
        { 
            gsgsDEBUG1() << "SourceToTagsThread helper thread is going down" << clEndl; 
        }
        void Stop()
        {
            // Notify the thread to exit and
            // wait for it
            if(IsAlive()) {
                Delete(NULL, wxTHREAD_WAIT_BLOCK);

            } else {
                Wait(wxTHREAD_WAIT_BLOCK);
            }
        }
        void Start()
        {
            gsgsDEBUG1() << "SourceToTagsThread helper thread started" << clEndl;
            Create();
            SetPriority(wxPRIORITY_DEFAULT);
            Run();
        }

        virtual void * Entry()
        {
            while(true) 
            {
                wxString filename;
                if(m_queue.ReceiveTimeout(50, filename) == wxMSGQUEUE_NO_ERROR) 
                {
                    if(TagsManagerST::Get()->IsBinaryFile(filename)) { continue; }

                    wxString strTags;
                    TagsManagerST::Get()->SourceToTags(filename, strTags);

                    // Fire the event
                    m_cache->CallAfter(&CxxFileCacheSymbols::OnPraseCompleted, filename, strTags);
                }
                if(TestDestroy()) break;
            }
            return NULL;
        }

        void ParseFile(const wxString& filename) { m_queue.Post(filename); }
    private:
        CxxFileCacheSymbols* m_cache;
        wxMessageQueue<wxString> m_queue;
    };
    //-----------------------------------------------------------------------------------
    CxxFileCacheSymbols::CxxFileCacheSymbols()
    {
        gsgs_Event().Bind(wxEVT_FILE_SAVED, &CxxFileCacheSymbols::OnFileSave, this);
        gsgs_Event().Bind(wxEVT_WORKSPACE_CLOSED, &CxxFileCacheSymbols::OnWorkspaceAction, this);
        gsgs_Event().Bind(wxEVT_WORKSPACE_LOADED, &CxxFileCacheSymbols::OnWorkspaceAction, this);
        m_helperThread = new SourceToTagsThread(this);
        m_helperThread->Start();
    }
    //-----------------------------------------------------------------------------------
    CxxFileCacheSymbols::~CxxFileCacheSymbols()
    {
        Clear();
        m_helperThread->Stop();
        wxDELETE(m_helperThread);

        gsgs_Event().Unbind(wxEVT_FILE_SAVED, &CxxFileCacheSymbols::OnFileSave, this);
        gsgs_Event().Unbind(wxEVT_WORKSPACE_CLOSED, &CxxFileCacheSymbols::OnWorkspaceAction, this);
        gsgs_Event().Unbind(wxEVT_WORKSPACE_LOADED, &CxxFileCacheSymbols::OnWorkspaceAction, this);
    }
    //-----------------------------------------------------------------------------------
    bool CxxFileCacheSymbols::Contains(const wxFileName& filename) const 
    { 
        return Contains(filename.GetFullPath()); 
    }
    //-----------------------------------------------------------------------------------
    bool CxxFileCacheSymbols::Contains(const wxString& filename) const
    {
        // wxCriticalSectionLocker locker(m_cs);
        return (m_cache.count(filename) > 0);
    }
    //-----------------------------------------------------------------------------------
    void CxxFileCacheSymbols::Clear()
    {
        // wxCriticalSectionLocker locker(m_cs);
        m_cache.clear();
        m_pendingFiles.clear();
        gsgsDEBUG1() << "Symbols cache cleared" << clEndl;
    }
    //-----------------------------------------------------------------------------------
    void CxxFileCacheSymbols::Update(const wxFileName& filename, const TagEntryPtrList& tags)
    {
        // wxCriticalSectionLocker locker(m_cs);
        if(m_cache.count(filename.GetFullPath())) 
        { 
            m_cache.erase(filename.GetFullPath()); 
        }
        m_cache[filename.GetFullPath()] = tags;
        gsgsDEBUG1() << "Updating Symbols cache for file:" << filename << clEndl;
    }
    //-----------------------------------------------------------------------------------
    void CxxFileCacheSymbols::Delete(const wxFileName& filename)
    {
        // wxCriticalSectionLocker locker(m_cs);
        m_cache.erase(filename.GetFullPath());
        gsgsDEBUG1() << "Deleting Symbols cache for file:" << filename << clEndl;

        // Notify that the symbols for this file were invalidated
        gsgs::Event event(wxEVT_CXX_SYMBOLS_CACHE_INVALIDATED);
        event.SetFileName(filename.GetFullPath());
        gsgs_Event().AddPendingEvent(event);
    }
    //-----------------------------------------------------------------------------------
    bool CxxFileCacheSymbols::Find(const wxFileName& filename, TagEntryPtrList& tags, size_t flags)
    {
        {
            // wxCriticalSectionLocker locker(m_cs);
            if(m_cache.count(filename.GetFullPath())) 
            {
                tags = m_cache[filename.GetFullPath()];
                gsgsDEBUG1() << "Symbols fetched from cache for file:" << filename << clEndl;
            } 
            else 
            {
                gsgsDEBUG1() << "Symbols for file:" << filename << "do not exist in the cache" << clEndl;
                return false;
            }
        }

        // Apply the flags
        if((flags & kFunctions) && !tags.empty()) 
        {
            // Remove all non functions
            TagEntryPtrList::iterator iter = tags.begin();
            for(; iter != tags.end();) {
                if(!(*iter)->IsMethod()) {
                    iter = tags.erase(iter);
                } else {
                    ++iter;
                }
            }
        }
        return true;
    }
    //-----------------------------------------------------------------------------------
    void CxxFileCacheSymbols::OnFileSave(gsgs::Event& e)
    {
        e.Skip();
        Delete(e.GetFileName());
    }
    //-----------------------------------------------------------------------------------
    void CxxFileCacheSymbols::OnWorkspaceAction(wxCommandEvent& e)
    {
        e.Skip();
        Clear();
    }
    //-----------------------------------------------------------------------------------
    void CxxFileCacheSymbols::RequestSymbols(const wxFileName & filename)
    {
        // If we are waiting for this file parse to complete, dont ask for it again
        if(m_pendingFiles.count(filename.GetFullPath())) 
        {
            gsgsDEBUG1() << "Ignoring duplicate parse request for file:" << filename.GetFullPath() << clEndl;
            return;
        }

        m_helperThread->ParseFile(filename.GetFullPath());
        m_pendingFiles.insert(filename.GetFullPath());
    }
    //-----------------------------------------------------------------------------------
    void CxxFileCacheSymbols::OnPraseCompleted(const wxString & filename, const wxString & strTags)
    {
        TagEntryPtrList tags;
        // Convert the string into array of tags
        wxArrayString lines = ::wxStringTokenize(strTags, "\n", wxTOKEN_STRTOK);
        for(size_t i = 0; i < lines.size(); ++i) 
        {
            wxString& strLine = lines.Item(i);
            strLine.Trim().Trim(false);
            if(strLine.IsEmpty()) continue;

            TagEntryPtr tag(new TagEntry());
            tag->FromLine(strLine);
            tags.push_back(tag);
        }
        // Update the cache
        Update(filename, tags);
        gsgs::Event event(wxEVT_CXX_SYMBOLS_CACHE_UPDATED);
        event.SetFileName(filename);
        gsgs_Event().AddPendingEvent(event);

        m_pendingFiles.erase(filename);
    }
    //-----------------------------------------------------------------------------------
}