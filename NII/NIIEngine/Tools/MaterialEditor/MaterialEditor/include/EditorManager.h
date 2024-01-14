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
#ifndef _EDITORMANAGER_H_
#define _EDITORMANAGER_H_

#include <list>
#include <map>

#include <wx/event.h>

#include "OgreSingleton.h"

#include "EventContainer.h"

class wxAuiNotebook;
class wxAuiNotebookEvent;

class Editor;
class EditorInput;
class EventArgs;
class Project;
class Workspace;

typedef std::list<Editor*> EditorList;
typedef std::map<Editor*, int> EditorIndexMap;

using Ogre::String;

class EditorManager : public wxEvtHandler, public Ogre::Singleton<EditorManager>, public EventContainer
{
public:
	enum EditorManagerEvent
	{
		EditorOpened,
		EditorClosed,
		ActiveEditorChanged
	};

	EditorManager(wxAuiNotebook* notebook);
	virtual ~EditorManager();

	wxAuiNotebook* getEditorNotebook() const;
	void setEditorNotebook(wxAuiNotebook* notebook);
	
	void openEditor(Editor* editor);
	//void openEditor(EditorInput* input);
	void closeEditor(Editor* editor);
	//void closeEditor(EditorInput* input);
	Editor* findEditor(const wxString& name);

	Editor* getActiveEditor() const;
	void setActiveEditor(Editor* editor);

	const EditorList& getEditors() const;

	void nameChanged(EventArgs& args);

	void OnPageChanged(wxAuiNotebookEvent& event);
	void OnPageClosed(wxAuiNotebookEvent& event);

	/** Override standard Singleton retrieval.
	@remarks
	Why do we do this? Well, it's because the Singleton
	implementation is in a .h file, which means it gets compiled
	into anybody who includes it. This is needed for the
	Singleton template to work, but we actually only want it
	compiled into the implementation of the class based on the
	Singleton, not all of them. If we don't change this, we get
	link errors when trying to use the Singleton-based class from
	an outside dll.
	@par
	This method just delegates to the template version anyway,
	but the implementation stays in this single compilation unit,
	preventing link errors.
	*/
	static EditorManager& getSingleton(void);

	/** Override standard Singleton retrieval.
	@remarks
	Why do we do this? Well, it's because the Singleton
	implementation is in a .h file, which means it gets compiled
	into anybody who includes it. This is needed for the
	Singleton template to work, but we actually only want it
	compiled into the implementation of the class based on the
	Singleton, not all of them. If we don't change this, we get
	link errors when trying to use the Singleton-based class from
	an outside dll.
	@par
	This method just delegates to the template version anyway,
	but the implementation stays in this single compilation unit,
	preventing link errors.
	*/
	static EditorManager* getSingletonPtr(void);

protected:
	void registerEvents();

	EditorList mEditors;
	EditorIndexMap mEditorIndexMap;
	Editor* mActiveEditor;
	wxAuiNotebook* mEditorNotebook;
};

#endif // _EDITORMANAGER_H_
