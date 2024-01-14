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
#include "EditorManager.h"

#include <boost/bind.hpp>

#include <wx/aui/auibook.h>

#include "Editor.h"
#include "EditorEventArgs.h"

template<> EditorManager* Ogre::Singleton<EditorManager>::msSingleton = 0;

EditorManager& EditorManager::getSingleton(void)
{  
	assert( msSingleton );  return ( *msSingleton );  
}

EditorManager* EditorManager::getSingletonPtr(void)
{
	return msSingleton;
}

EditorManager::EditorManager(wxAuiNotebook* notebook)
: mEditorNotebook(notebook), mActiveEditor(NULL)
{
	registerEvents();

	if(mEditorNotebook != NULL)
	{
		Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEventHandler(EditorManager::OnPageChanged));
		Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE, wxAuiNotebookEventHandler(EditorManager::OnPageClosed));

		mEditorNotebook->PushEventHandler(this);
	}
}

EditorManager::~EditorManager()
{

}

wxAuiNotebook* EditorManager::getEditorNotebook() const
{
	return mEditorNotebook;
}

void EditorManager::setEditorNotebook(wxAuiNotebook* notebook)
{
	if(mEditorNotebook != NULL)
	{
		Disconnect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEventHandler(EditorManager::OnPageChanged));
		Disconnect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE, wxAuiNotebookEventHandler(EditorManager::OnPageClosed));

		mEditorNotebook->RemoveEventHandler(this);
	}
	
	mEditorNotebook = notebook;
	
	if(mEditorNotebook != NULL)
	{
		Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEventHandler(EditorManager::OnPageChanged));
		Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE, wxAuiNotebookEventHandler(EditorManager::OnPageClosed));

		mEditorNotebook->PushEventHandler(this);
	}
}

void EditorManager::openEditor(Editor * editor)
{
	assert(mEditorNotebook != NULL);

	mEditors.push_back(editor);
	mEditorNotebook->AddPage(editor->getControl(), editor->getName(), true);

	mEditorIndexMap[editor] = mEditorNotebook->GetPageIndex(editor->getControl());

	editor->subscribe(Editor::NameChanged, boost::bind(&EditorManager::nameChanged, this, _1));

	setActiveEditor(editor);
}

void EditorManager::closeEditor(Editor * editor)
{
	assert(mEditorNotebook != NULL);

	if(mEditorIndexMap.find(editor) != mEditorIndexMap.end())
	{
		if(mActiveEditor == editor)
		{
			mActiveEditor->deactivate();
			mActiveEditor = NULL;
		}

		int index = mEditorIndexMap[editor];
		mEditorNotebook->RemovePage(index);

		fireEvent(EditorClosed, EditorEventArgs(editor));
	}
}

Editor* EditorManager::findEditor(const wxString& name)
{
	EditorList::iterator it;
	Editor* editor = NULL;
	for(it = mEditors.begin(); it != mEditors.end(); ++it)
	{
		editor = (*it);
		if(editor->getName() == name) return editor;
	}

	return NULL;
}

Editor* EditorManager::getActiveEditor() const
{
	return mActiveEditor;
}

void EditorManager::setActiveEditor(Editor* editor)
{
	if(mActiveEditor == editor) return;

	if(mActiveEditor != NULL) mActiveEditor->deactivate();

	mActiveEditor = editor;

	if(mActiveEditor != NULL) 
	{
		mActiveEditor->activate();

		if(mEditorNotebook != NULL && (mEditorIndexMap.find(mActiveEditor) != mEditorIndexMap.end()))
		{
			mEditorNotebook->SetSelection(mEditorIndexMap[mActiveEditor]);
		}
	}

	fireEvent(ActiveEditorChanged, EditorEventArgs(mActiveEditor));
}

const EditorList& EditorManager::getEditors() const
{
	return mEditors;
}

void EditorManager::registerEvents()
{
	registerEvent(EditorOpened);
	registerEvent(EditorClosed);
	registerEvent(ActiveEditorChanged);
}

void EditorManager::nameChanged(EventArgs& args)
{
	EditorEventArgs eea = dynamic_cast<EditorEventArgs&>(args);
	Editor* editor = eea.getEditor();
	if(mEditorIndexMap.find(editor) != mEditorIndexMap.end())
	{
		int index = mEditorIndexMap[editor];
		mEditorNotebook->SetPageText(index, editor->getName());
	}
}

void EditorManager::OnPageChanged(wxAuiNotebookEvent& event)
{
	int index = event.GetSelection();

	if(mActiveEditor != NULL) 
	{
		if(mEditorIndexMap.find(mActiveEditor) != mEditorIndexMap.end())
		{
			int oldIndex = mEditorIndexMap[mActiveEditor];

			if(index != oldIndex)
			{
				mActiveEditor->deactivate();

				EditorIndexMap::iterator it;
				for(it = mEditorIndexMap.begin(); it != mEditorIndexMap.end(); ++it)
				{
					if(it->second == index)
					{
						setActiveEditor(it->first);
						break;
					}
				}
			}
		}
	}
	else
	{
		EditorIndexMap::iterator it;
		for(it = mEditorIndexMap.begin(); it != mEditorIndexMap.end(); ++it)
		{
			if(it->second == index)
			{
				setActiveEditor(it->first);
			}
		}
	}
}

void EditorManager::OnPageClosed(wxAuiNotebookEvent& event)
{
	int index = event.GetSelection();

	Editor* editor = NULL;
	EditorIndexMap::iterator it;
	for(it = mEditorIndexMap.begin(); it != mEditorIndexMap.end(); ++it)
	{
		if(it->second == index)
		{
			editor = it->first;
			editor->deactivate();
			mEditorIndexMap.erase(it);
			break;
		}
	}

	if(editor != NULL)
	{
		if(editor == mActiveEditor)
			mActiveEditor = NULL;

		EditorList::iterator lit;
		for(lit = mEditors.begin(); lit != mEditors.end(); ++lit)
		{
			if((*lit) == editor)
			{
				mEditors.erase(lit);
				break;
			}
		}	
	}

	fireEvent(EditorClosed, EditorEventArgs(editor));

	// Is this handled by OnPageChanged?
	int selIndex = event.GetSelection();
	for(it = mEditorIndexMap.begin(); it != mEditorIndexMap.end(); ++it)
	{
		if(it->second == index)
		{
			setActiveEditor(it->first);
			break;
		}
	}
}

