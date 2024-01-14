/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/

#ifndef gsgsCXXFILECACHESYMBOLS_H
#define gsgsCXXFILECACHESYMBOLS_H

#include "gsgsPreInclude.h"
#include "gsgsEvent.h"
#include "gsgsTag.h"
#include <wx/thread.h>

namespace gsgs
{
    class SourceToTagsThread;
    class _gsgsAPI CxxFileCacheSymbols : public wxEvtHandler
    {
    public:
        enum eFilter 
        {
            kAllSymbols = 0,
            kFunctions = (1 << 0),
        };
    public:
        typedef wxSharedPtr<CxxFileCacheSymbols> Ptr_t;

        CxxFileCacheSymbols();
        virtual ~CxxFileCacheSymbols();

        bool Contains(const wxFileName & filename) const;

        bool Contains(const wxString & filename) const;

        void Clear();

        void Update(const wxFileName & filename, const TagEntryPtrList & tags);

        void Delete(const wxFileName & filename);

        /**
         * @brief fetch from the cache, apply "flags" on the result. See eFilter
         */
        bool Find(const wxFileName & filename, TagEntryPtrList & tags, size_t flags = 0);

        /**
         * @brief Request parsing of a file from the parser thread. The results will be cached
         * and an event 'gsgs::Event' is fired to notify that the cache was updated
         */
        void RequestSymbols(const wxFileName & filename);

        void OnPraseCompleted(const wxString & filename, const wxString & strTags);
    protected:
        void OnFileSave(gsgs::Event & e);
        void OnWorkspaceAction(wxCommandEvent & e);
    private:
        std::unordered_map<wxString, std::vector<TagEntryPtr> > m_cache;
        std::unordered_set<wxString> m_pendingFiles;
    private:
        wxCriticalSection m_cs;
        SourceToTagsThread * m_helperThread;
    };

    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CXX_SYMBOLS_CACHE_UPDATED, gsgs::Event);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CXX_SYMBOLS_CACHE_INVALIDATED, gsgs::Event);
}
#endif
