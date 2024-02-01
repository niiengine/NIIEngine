//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : newwxprojectdlg.h              
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
 #ifndef __newwxprojectdlg__
#define __newwxprojectdlg__

/**
@file
Subclass of NewWxProjectBaseDlg, which is generated by wxFormBuilder.
*/

#include "newwxprojectbasedlg.h"
#include "newwxprojectinfo.h"


/** Implementing NewWxProjectBaseDlg */
class NewWxProjectDlg : public NewWxProjectBaseDlg
{
	IManager *m_mgr;
	StringManager m_stringManager;
protected:
	void OnButtonCancel(wxCommandEvent &e);
	void OnButtonOK(wxCommandEvent &e);
	bool ValidateInput();
	void OnChoiceChanged(wxCommandEvent &e);
	 
public:
	/** Constructor */
	NewWxProjectDlg( wxWindow* parent, IManager *mgr  );
	virtual ~NewWxProjectDlg();
	
	void GetProjectInfo(NewWxProjectInfo &info);
};

#endif // __newwxprojectdlg__