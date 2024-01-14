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
#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include "gsgsSingleton.h"
#include "gsgsSharedPtr.h"
#include "gsgsEvent.h"
#include <wx/xrc/xmlres.h>

namespace gsgs
{
    /**
     * The interface for menu event handler classes
     */
    class MenuEventHandler
    {
    protected:
        int m_id;

    public:
        MenuEventHandler(int id)
            : m_id(id){};
        virtual ~MenuEventHandler(){};

        // handle an event
        virtual void ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event) = 0;
        virtual void ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event) = 0;

        // Setters/Getters
        const int& GetEventId() const { return m_id; }
        void SetEventId(const int& id) { m_id = id; }
    };

    typedef SharedPtr<MenuEventHandler> MenuEventHandlerPtr;

    //-----------------------------------------------------------------
    // Define here a class per event/group
    //-----------------------------------------------------------------

    //------------------------------------
    // Common edit tasks
    //------------------------------------
    class EditHandler : public MenuEventHandler
    {
    public:
        EditHandler(int id)
            : MenuEventHandler(id){};
        virtual ~EditHandler(){};

    public:
        virtual void ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event);
        virtual void ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event);
    };

    //------------------------------------
    // Brace matching
    //------------------------------------
    class BraceMatchHandler : public MenuEventHandler
    {
    public:
        BraceMatchHandler(int id)
            : MenuEventHandler(id){};
        virtual ~BraceMatchHandler(){};

    public:
        virtual void ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event);
        virtual void ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event);
    };

    //------------------------------------
    // Find / Repalce
    //------------------------------------
    class FindReplaceHandler : public MenuEventHandler
    {
    public:
        FindReplaceHandler(int id)
            : MenuEventHandler(id){};
        virtual ~FindReplaceHandler(){};

    public:
        virtual void ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event);
        virtual void ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event);
    };

    //------------------------------------
    // Goto
    //------------------------------------
    class GotoHandler : public MenuEventHandler
    {
    public:
        GotoHandler(int id)
            : MenuEventHandler(id){};
        virtual ~GotoHandler(){};

    public:
        virtual void ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event);
        virtual void ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event);
    };

    //------------------------------------
    // Bookmarks
    //------------------------------------
    class BookmarkHandler : public MenuEventHandler
    {
    public:
        BookmarkHandler(int id)
            : MenuEventHandler(id){};
        virtual ~BookmarkHandler(){};

    public:
        virtual void ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event);
        virtual void ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event);
    };

    //------------------------------------
    // Go to definition
    //------------------------------------
    class GotoDefinitionHandler : public MenuEventHandler
    {
    public:
        GotoDefinitionHandler(int id)
            : MenuEventHandler(id){};
        virtual ~GotoDefinitionHandler(){};

    public:
        virtual void ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event);
        virtual void ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event);
    };

    //-------------------------------------------------
    // Word wrap
    //-------------------------------------------------
    class WordWrapHandler : public MenuEventHandler
    {
    public:
        WordWrapHandler(int id)
            : MenuEventHandler(id){};
        virtual ~WordWrapHandler(){};

    public:
        virtual void ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event);
        virtual void ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event);
    };

    //-------------------------------------------------
    // Folding
    //-------------------------------------------------
    class FoldHandler : public MenuEventHandler
    {
    public:
        FoldHandler(int id)
            : MenuEventHandler(id){};
        virtual ~FoldHandler(){};

    public:
        virtual void ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event);
        virtual void ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event);
    };

    //-------------------------------------------------
    // Debugger handler
    //-------------------------------------------------
    class DebuggerMenuHandler : public MenuEventHandler
    {
    public:
        DebuggerMenuHandler(int id)
            : MenuEventHandler(id){};
        virtual ~DebuggerMenuHandler(){};

    public:
        virtual void ProcessCommandEvent(wxWindow* owner, wxCommandEvent& event);
        virtual void ProcessUpdateUIEvent(wxWindow* owner, wxUpdateUIEvent& event);
    };

    //-------------------------------------------------
    // The menu event manager.
    //-------------------------------------------------

    WX_DECLARE_HASH_MAP(int, MenuEventHandlerPtr, wxIntegerHash, wxIntegerEqual, HandlesrHash);

    class MenuManager : public gsgs::Singleton<MenuManager>
    {
        friend class gsgs::Singleton<MenuManager>;
        HandlesrHash m_handlers;
    public:
        // register new event handler
        void PushHandler(MenuEventHandlerPtr handler);

        MenuEventHandlerPtr GetHandler(int id);
    private:
        MenuManager(void);
        virtual ~MenuManager(void);
    };
}
#endif // MENUMANAGER_H
