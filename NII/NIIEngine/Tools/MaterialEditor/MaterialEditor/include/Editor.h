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
#ifndef _EDITOR_H_
#define _EDITOR_H_

#include <wx/string.h>

#include "OgreString.h"

#include "EventArgs.h"
#include "EventContainer.h"

class wxControl;

class EditorContributor;
class EditorInput;

using Ogre::String;

class Editor : public EventContainer
{
public:
	enum EditorEvent
	{
		NameChanged,
		DirtyStateChanged
	};

	Editor();
	Editor(EditorInput* input);
	virtual ~Editor();

	wxControl* getControl() const;
	void setControl(wxControl* control);

	EditorInput* getEditorInput() const;
	void setEditorInput(EditorInput* input);
	
	EditorContributor* getEditorContributor() const;

	const wxString& getName() const;
	void setName(const wxString& name);

	virtual void activate();
	virtual void deactivate();

	virtual bool isDirty();
	virtual void save();
	virtual void saveAs();
	virtual bool isSaveAsAllowed();

	virtual bool isRedoable();
	virtual void redo();
	virtual bool isUndoable();
	virtual void undo();

	virtual bool isCuttable();
	virtual void cut();
	virtual bool isCopyable();
	virtual void copy();
	virtual bool isPastable();
	virtual void paste();

private:
	void registerEvents();

	EditorInput* mEditorInput;
	wxControl* mControl;
	wxString mName;
};

#endif // _EDITOR_H_
