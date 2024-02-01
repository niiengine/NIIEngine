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
#ifndef BOOKMARKMANAGER_H
#define BOOKMARKMANAGER_H

#include "gsgsPreInclude.h"
#include <wx/menu.h>
#include <wx/stc/stc.h>
#include "ieditor.h"

#define MAX_BOOKMARK CL_N0_OF_BOOKMARK_TYPES

namespace gsgs
{
    // NB The following are sci markers, which are zero based. So smt_bookmark is actually the eighth of them (important
    // when masking it!)
    // If you add another type here, watch out for smt_LAST_BP_TYPE; and you need also to add to the enum 'marker_mask_type'
    // below
    // The higher the value, the nearer the top of the pecking order displaywise. So keep the most important breakpoint at
    // the top i.e. smt_breakpoint,
    // but have smt_breakpointsmt_indicator above it, so you can see the indicator when there's a breakpt too
    enum sci_marker_types { /* markers 0-2 are unused atm */
        smt_FIRST_BMK_TYPE = 3,
        smt_LAST_BMK_TYPE = smt_FIRST_BMK_TYPE + CL_N0_OF_BOOKMARK_TYPES - 1,
        smt_bookmark1 = smt_FIRST_BMK_TYPE,       // 3
        smt_bookmark2,                            // 4
        smt_bookmark3,                            // 5
        smt_bookmark4,                            // 6
        smt_find_bookmark = smt_LAST_BMK_TYPE,    // 7
        smt_FIRST_BP_TYPE = 8,                    // 8
        smt_cond_bp_disabled = smt_FIRST_BP_TYPE, // 8
        smt_bp_cmdlist_disabled,                  // 9
        smt_bp_disabled,                          // 10
        smt_bp_ignored,                           // 11
        smt_cond_bp,                              // 12
        smt_bp_cmdlist,                           // 13
        smt_breakpoint,                           // 14
        smt_LAST_BP_TYPE = smt_breakpoint,        // 14
        smt_indicator,                            // 15
        smt_warning,                              // 16
        smt_error,                                // 17
        smt_line_marker,                          // 18
    };

    // These are bitmap masks of the various margin markers.
    // So 256 == 0x100 == 100000000, 2^9, and masks the ninth marker, smt_cond_bp_disabled==8 (as the markers are
    // zero-based)
    // 0x7f00 is binary 111111100000000 and masks all the 7 current breakpoint types. If you add others, change it
    enum marker_mask_type {
        mmt_standard_bookmarks = 0x78,
        mmt_find_bookmark = 0x80,
        mmt_all_bookmarks = 0xF8,
        mmt_FIRST_BP_TYPE = 0x100,
        mmt_cond_bp_disabled = mmt_FIRST_BP_TYPE,
        mmt_bp_cmdlist_disabled = 0x200,
        mmt_bp_disabled = 0x400,
        mmt_bp_ignored = 0x800,
        mmt_cond_bp = 0x1000,
        mmt_bp_cmdlist = 0x2000,
        mmt_breakpoint = 0x4000,
        mmt_LAST_BP_TYPE = mmt_breakpoint,
        mmt_all_breakpoints = 0x7f00,
        mmt_indicator = 1 << 15,
        mmt_warning = 1 << 16,
        mmt_error = 1 << 17,
        mmt_compiler = mmt_warning | mmt_error, // Compiler errors and warnings
        mmt_line_marker = 1 << 18,              // Mask for the 18th bit (smt_line_marker)
        mmt_folds = wxSTC_MASK_FOLDERS          /* 0xFE000000 */
    };
    class _gsgsAPI BookmarkManager : public wxEvtHandler
    {
    public:
        typedef std::unordered_map<int, wxString> Map_t;
    public:
        static BookmarkManager& singleton();

        inline void SetActiveBookmarkType(int type) { m_activeBookmarkType = type; }
        inline int GetActiveBookmarkType() const { return m_activeBookmarkType; }

        wxString GetMarkerLabel(int index) const;
        wxMenu* CreateBookmarksSubmenu(wxMenu* parentMenu);
    protected:
        BookmarkManager();
        virtual ~BookmarkManager();

        void OnEditorSettingsChanged(wxCommandEvent& e);
        void DoPopulateDefaultLabels();
    private:
        int m_activeBookmarkType;
        BookmarkManager::Map_t m_markerLabels;
    };
}

#define gsgs_Bookmark() gsgs::BookmarkManager::singleton()

#endif