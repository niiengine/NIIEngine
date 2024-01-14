#include "ResourcePanel.h"

#include <wx/sizer.h>
#include <wx/treectrl.h>

#include "OgreResourceLoadManager.h"
#include "OgreResourceManager.h"
#include "OgreStringList.h"

using Ogre::ResourceSchemeManager;
using Ogre::ResourceManager;
using Ogre::StringList;

ResourcePanel::ResourcePanel(wxWindow * parent, wxWindowID id /* = wxID_ANY */, const wxPoint& pos /* = wxDefaultPosition */, const wxSize& size /* = wxDeafultSize */, long style /* = wxTAB_TRAVERSAL */, const wxString& name /* =  */)
: wxPanel(parent, id, pos, size, style, name)
{
	mBoxSizer = new wxBoxSizer(wxVERTICAL);

	mTreeControl = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
	mBoxSizer->Add(mTreeControl, 1, wxEXPAND | wxALL);

	wxTreeItemId rootId = mTreeControl->AddRoot("Resources");

	GroupIDList groups = N_Only(ResourceScheme).getGroups();
	for(GroupIDList::iterator it = groups.begin(); it != groups.end(); ++it)
	{
		wxTreeItemId gid = mTreeControl->AppendItem(rootId, *it);
		ResourceIDList resources = N_Only(ResourceScheme).getResources(*it);
		ResourceIDList::iterator rit, ritend = resources.end();
        for(rit = resources.begin(); rit != ritend; ++rit)
		{
			mTreeControl->AppendItem(gid, *rit);
		}
	}

	SetSizer(mBoxSizer);
	Layout();
}

ResourcePanel::~ResourcePanel()
{}