//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : editsnippetsdlg.h              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )                     
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/                     
//              \____/\___/ \__,_|\___\_____/_|\__\___|                     
//                                                                          
//                                                  F i l e                 
//                                                                          
//    This program is free software; you can redistribute it and/or modify  
//    it under the terms of the GNU General Public License as published by  
//    the Free Software Foundation; either version 2 of the License, or     
//    (at your option) any later version.                                   
//                                                                          
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef __editsnippetsdlg__
#define __editsnippetsdlg__

/**
@file
Subclass of EditSnippetsBaseDlg, which is generated by wxFormBuilder.
*/

#include "editsnippetsbasedlg.h"
#include "swStringDb.h"
#include "imanager.h"

class SnipWiz;


/** Implementing EditSnippetsBaseDlg */
class EditSnippetsDlg : public EditSnippetsBaseDlg
{
	bool 			m_modified;		// modified flag
	SnipWiz* 		m_pPlugin;		// pointer to plugin
	IManager*		m_manager;
	
protected:
	// Handlers for EditSnippetsBaseDlg events.
	void OnItemSelected( wxCommandEvent& event );
	void OnAddSnippet( wxCommandEvent& event );
	void OnAddSnippetUI( wxUpdateUIEvent& event );
	void OnChangeSnippet( wxCommandEvent& event );
	void OnChangeSnippetUI( wxUpdateUIEvent& event );
	void OnRemoveSnippet( wxCommandEvent& event );
	void OnRemoveSnippetUI( wxUpdateUIEvent& event );
	
	void SelectItem(long index);
	void OnButtonKeyShortcut(wxCommandEvent &e);
	void DoItemSelected(const wxString& text);
	
public:
	/** Constructor */
	EditSnippetsDlg( wxWindow* parent, SnipWiz *plugin, IManager *manager );
	
	void Initialize();
	
	swStringDb* GetStringDb() ;
	
	void SetModified(const bool& modified) {
		this->m_modified = modified;
	}
	
	void SetPPlugin(SnipWiz* pPlugin) {
		this->m_pPlugin = pPlugin;
	}
	
	const bool& GetModified() const {
		return m_modified;
	}
	SnipWiz* GetPPlugin() {
		return m_pPlugin;
	}
};

#endif // __editsnippetsdlg__