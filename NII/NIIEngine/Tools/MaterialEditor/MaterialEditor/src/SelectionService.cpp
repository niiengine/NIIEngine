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
#include "SelectionService.h"

#include "SelectionEventArgs.h"

template<> SelectionService* Ogre::Singleton<SelectionService>::msSingleton = 0;

SelectionService& SelectionService::getSingleton(void)
{  
	assert( msSingleton );  return ( *msSingleton );  
}

SelectionService* SelectionService::getSingletonPtr(void)
{
	return msSingleton;
}

SelectionService::SelectionService()
{
	registerEvents();
}

SelectionService::~SelectionService()
{
}

void SelectionService::registerEvents()
{
	registerEvent(SelectionChanged);
}

const SelectionList& SelectionService::getSelection() const
{
	return mSelection;
}

void SelectionService::setSelection(boost::any& sel)
{
	mSelection.clear();
	
	mSelection.push_back(sel);
	
	fireEvent(SelectionChanged, SelectionEventArgs(mSelection));
}

void SelectionService::setSelection(const SelectionList& sel)
{
	mSelection = sel;

	fireEvent(SelectionChanged, SelectionEventArgs(mSelection));
}

