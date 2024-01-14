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

#ifndef __opentypevlistctrl__
#define __opentypevlistctrl__

#include "gsgsPreInclude.h"
#include <wx/listctrl.h>
#include "gsgsTag.h"

class _gsgsAPI OpenTypeVListCtrl : public wxListView 
{
public:
	OpenTypeVListCtrl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
	virtual ~OpenTypeVListCtrl();

	void SetItems(const std::vector<gsgs::TagEntryPtr> &tags) { m_tags = tags; }
	
	virtual wxListItemAttr* OnGetItemAttr(long item) const;
	virtual int OnGetItemColumnImage(long item, long column) const;
	virtual int OnGetItemImage(long item) const;
	virtual wxString OnGetItemText(long item, long column) const;
	int FindMatch(const wxString &word);
    gsgs::TagEntryPtr GetTagAt(long item);
private:
	std::vector<gsgs::TagEntryPtr> m_tags;
};

#endif // __opentypevlistctrl__

