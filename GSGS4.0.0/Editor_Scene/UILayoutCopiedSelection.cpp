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
#include "UILayoutCopiedSelection.h"
#include "UILayoutOptions.h"
#include "ExceptionManager.h"
#include "NiiUIWindow.h"
#include "NiiUIWidgetManager.h"
#include "NiiUIManager.h"
#include "UILayoutEditorFrame.h"
#include "StrHelper.h"
#include "app.h"

using namespace NII;
using namespace NII::UI;
namespace gsgs
{
    //------------------------------------------------------------------------
    size_t Selection::Size() const
    {
        // Use the largest collection
        return m_allBoxes.size() ;
    }
    //------------------------------------------------------------------------
    bool Selection::Select (Widget* aWindow) 
    {
        // Toggle when already selected
        if (IsSelected (aWindow))
        {
            Unselect(aWindow);
        }
        else if (!IsParentOrChild(aWindow))
        {
            // Only top windows are moveable
            m_moveableBoxes.push_back (WindowBox(aWindow));
            // Select the window and its children
            m_allBoxes.push_back (WindowBox(aWindow));
            //SelectChildren(aWindow);
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool Selection::SelectAll (Widget * aWindow) 
    {
        wxASSERT_MSG(aWindow != NULL, wxT("Given Windows may not be NULL"));

        // Reset
        ClearSelection();

        // Select
        m_moveableBoxes.push_back (WindowBox(aWindow));
        m_allBoxes.push_back (WindowBox(aWindow));

        // Children too, but not resizable
        SelectChildren(dynamic_cast<const Container *>(aWindow));

        return true;
    }
    //------------------------------------------------------------------------
    void Selection::Unselect(const Widget * aWindow)
    {
        DeleteWindowBox(m_allBoxes, aWindow);
        DeleteWindowBox(m_moveableBoxes, aWindow);
        // Remove selected children too
        UnselectChildren(dynamic_cast<const Container *>(aWindow));
    }
    //------------------------------------------------------------------------
    bool Selection::IsSelected(const Widget* aWindow) 
    {
        // Use the largest collection
        for (Boxes::iterator boxIt = m_allBoxes.begin(); boxIt != m_allBoxes.end(); ++boxIt)
        {
            if (aWindow == boxIt->GetWindow())
            {			
                return true;
            }
        }
        return false;
    }
    //------------------------------------------------------------------------
    void Selection::ClearSelection() 
    {
        // Remove all boxes
        m_allBoxes.clear();
        m_moveableBoxes.clear();
    }
    //------------------------------------------------------------------------
    WindowBox* Selection::FindBoxFor(const Widget* aWindow) 
    {
        // Look in the largest collection
        for (Boxes::iterator boxIt = m_allBoxes.begin(); boxIt != m_allBoxes.end(); ++boxIt)
        {
            if (boxIt->GetWindow() == aWindow)
            {
                return &*boxIt;
            }
        }
        return NULL;
    }
    //------------------------------------------------------------------------
    void Selection::Update()
    {	// Tick all boxes
        ResetBoxes(m_allBoxes);
        ResetBoxes(m_moveableBoxes);
    }
    //------------------------------------------------------------------------
    void Selection::ResetBoxes(Boxes& aBoxes)
    {
        for (Boxes::iterator boxIt = aBoxes.begin(); boxIt != aBoxes.end(); ++boxIt)
        {
            boxIt->Reset();
        }
    }
    // ------------------------------------------------------------------------
    bool Selection::IsParentOrChild(const Widget * aWindow) 
    {
        Container * temp = 0;
        for (Boxes::iterator boxIt = m_allBoxes.begin(); boxIt != m_allBoxes.end(); ++boxIt)
        {
            // Is the given element the parent of one of the currently selected elements,
            // or is the given element the child of one of the currently selected elements? 
            // (We only have to look one level deep)
            if (boxIt->GetWindow()->getParent() == aWindow)
            {
                // Bummer... cannot select then
                return true;
            }
            else if(boxIt->GetWindow()->isContainer() && dynamic_cast<Container *>(temp)->isExist(aWindow))
            {
                return true;
            }
        }
        return false;
    }
    //------------------------------------------------------------------------
    void Selection::SelectChildren(const Container* aWindow)
    {
        for (size_t i=0; i<aWindow->getCount(); ++i)
        {
            Widget * child = aWindow->get(i);
            // Skip __auto windows
            if (child->getName().find(_T("__auto_")) == String::npos && child->getName().find(_T("__TabPane__")))
            {	// Add as an non-resizable box
                m_allBoxes.push_back (WindowBox(child, false));
                // Recurse!
                SelectChildren(dynamic_cast<const Container *>(child));
            }
        }
    }
    //------------------------------------------------------------------------
    void Selection::UnselectChildren(const Container* aWindow)
    {
        for (size_t i=0; i<aWindow->getCount(); ++i)
        {
            Widget * child = aWindow->get(i);
            // Delete from the large collection, cause that one contains the children
            // Also, the moveable collection does *not* contain children
            if (DeleteWindowBox(m_allBoxes, child))
            {	// Recurse!
                UnselectChildren(dynamic_cast<const Container *>(child));
            }
        }
    }

    //------------------------------------------------------------------------
    bool Selection::DeleteWindowBox(Boxes& aBoxes, const Widget* aWindow)
    {
        for (Boxes::iterator boxIt = aBoxes.begin(); boxIt != aBoxes.end(); ++boxIt)
        {
            if (boxIt->GetWindow() == aWindow)
            {
                aBoxes.erase(boxIt) ;
                return true;
            }
        }
        return false;
    }
    //-----------------------------------------------------------------------
    
    //////////////////////////////////////////////////////////////////////////
    // COPIEDSELECTION CONSTRUCTORS/DESTRUCTORS
    //////////////////////////////////////////////////////////////////////////
    //------------------------------------------------------------------------
    CopiedSelection::CopiedSelection() :
        m_pasteCounter(0),
        m_applyPasteOffset(false),
        m_deepCopies()
    {
        // NOTE: Prefer constructor initialization lists (which are 
        // often more efficient) to assignment inside the 
        // constructor. Members in the initialization list should 
        // appear in the order they are declared in the class, as 
        // this is the order they will be initialized (no matter what).
        // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers
    }
    //------------------------------------------------------------------------
    CopiedSelection::~CopiedSelection() 
    {
        // Swallow any exceptions found to prevent the destructor from emitting them.
        // NOTE: An exception should never leave a destructor as it may yield
        // undefined behavior or even lead to program abnormal termination 
        // (see Item 8 in 'Effective C++ 3rd Edition' by Scott Meyers)
        try
        {
            Reset();
        }
        // Add exception to log
        LOG_EXCEPTION_SAFE("CopiedSelection::~CopiedSelection", "Failed to destroy copied selection", "Failed to destroy copied selection", "Failed to destroy copied selection");
    }
    //////////////////////////////////////////////////////////////////////////
    // COPIEDSELECTION PUBLIC FUNCTIONS
    //////////////////////////////////////////////////////////////////////////
    //------------------------------------------------------------------------
    void CopiedSelection::Copy (const Widget * widget)
    {
        if (widget)
        {	
            DeepCopy* const deepCopy = new DeepCopy();
            CopyWindowRecursive(*deepCopy, widget);

            // copy to its address
            // NOTE: No need to free the custodial pointer newDeepCopy since
            // it will be handled internally by std::vector::push_back()
            m_deepCopies.push_back(deepCopy);
        }
    }
    //------------------------------------------------------------------------
    void CopiedSelection::Paste(Widget * pTarget)
    {
        if (pTarget)
        {
            LogDebug(wxT("Start pasting on '%s'"), StrHelper::ToWXString(pTarget->getName()).c_str());

            m_pasteCounter++;   // For the unique names.
            m_applyPasteOffset = true;  // Will be toggled off after the first call.
            for (size_t i = 0; i < m_deepCopies.size(); ++i)
            {
                PasteWindowRecursive( *m_deepCopies[i], dynamic_cast<Container *>(pTarget) );
            }
        }
    }
    //------------------------------------------------------------------------
    void CopiedSelection::Reset () 
    {
        for(size_t i = 0; i < m_deepCopies.size(); ++i)
        {
            delete m_deepCopies[i];
        }
        // Clear selection
        m_deepCopies.clear () ;
        // Reset counter
        m_pasteCounter = 0;
    }
    //------------------------------------------------------------------------
    size_t CopiedSelection::Size() const
    {
        return m_deepCopies.size() ;
    }
    //------------------------------------------------------------------------
    void CopiedSelection::CopyWindowRecursive( DeepCopy& data, const Widget* widget )
    {
        // Validations
        wxASSERT_MSG(widget != NULL, wxT("Supplied window is NULL"));

        LogDebug(wxT("Copying '%s'"), StrHelper::ToWXString(widget->getName()).c_str());

        CopyWindow( data, widget);
        if (widget->isContainer())
        {
            const Container * ttSource = static_cast<const Container *>(widget);
            for (size_t i = 0; i < ttSource->getCount(); ++i)
            {	// Skip __auto windows, which will be recreated automatically
                Widget * child = ttSource->get(i);
                if (child->getName().find(_T("__auto_")) == String::npos && child->getName().find(_T("__TabPane__")))
                {
                    data.children.push_back(new DeepCopy());
                    CopyWindowRecursive(*data.children[data.children.size() - 1], child);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void CopiedSelection::CopyWindow( DeepCopy& data, const Widget* widget)
    {
        // Validations
        wxASSERT_MSG(widget != NULL, wxT("Supplied window is NULL"));

        data.strName = widget->getName();
        data.strType = widget->getStyleView();
        data.mParent = widget->getParent();
        // Copy the properties
        // NOTE: getPropertyIterator() makes sure the compiler doesn't confuse EventSet 
        // and PropertySet (it would if we used getIterator()) thus avoiding useless casts
        const PropertyCmdMap & cmap = widget->getProperty()->getList();
        PropertyCmdMap::const_iterator i, iend = cmap.end();
        for(i = cmap.begin(); i != iend; ++i)
        {
            PropertyID propertyName = i->first;
            String PropertyValue;
            widget->getProperty()->get(propertyName)->get(widget, PropertyValue);
            LogDebug (wxT("Copy '%s' with value '%s'"), StrHelper::ToWXString(i->second->getName()).c_str(), StrHelper::ToWXString(PropertyValue).c_str());
            data.mPropertyList.insert(std::make_pair(propertyName, PropertyValue));
        }
    }
    //------------------------------------------------------------------------
    void CopiedSelection::PasteWindowRecursive( const DeepCopy& data, Container* pTarget )
    {
        Widget* const pNew = PasteWindow( data, pTarget );

        LogDebug(wxT("Pasting '%s' on '%s'"), StrHelper::ToWXString(pNew->getName()).c_str(), StrHelper::ToWXString(pTarget->getName()).c_str());

        for( size_t i = 0; i < data.children.size(); ++i )
        {
            PasteWindowRecursive( *data.children[i], static_cast<Container*>(pNew) );
        }
    }
    //------------------------------------------------------------------------
    Widget* CopiedSelection::PasteWindow( const DeepCopy& data, Container* pTarget )
    {
        // Validations
        wxASSERT_MSG(pTarget != NULL, wxT("Supplied target is NULL"));

        std::map<PropertyID, String>::const_iterator it, itend = data.mPropertyList.end();
        Widget *pNew = 0;
        try
        {
            // Combine original name and counter into a unique name
            const String  strUniqueName = CreateUniqueName( data.strName );

            LogDebug(wxT("New type: '%s'"), StrHelper::ToWXString(N_conv(data.strType).c_str()));
            pNew = N_Only(Widget).create(data.strType, 0, data.mParent);
            pNew->setName(strUniqueName);
            pTarget->add (pNew);
            for (it = data.mPropertyList.begin(); it != itend; ++it)
            {
                try
                {
                    // Only when different (to avoid falagard reset when the 'lookNFeel' property gets overwritten)
                    // (Mantis ticket 170)
                    String tempstr;
                    pNew->getProperty()->get(it->first)->get(pNew, tempstr);
                    if (it->second.size() && it->second != tempstr)
                    {   // This needs a fix because of this possible exception:
                        // LogDebug(wxT("pasteSelection - set '%s' to '%s'"), StrHelper::ToWXString(it->first), StrHelper::ToWXString(it->second));
                        // CCopiedSelection::pasteSelection - Window::setLookNFeel - There is already a look'n'feel assigned to the window 'XXX'
                        pNew->getProperty()->get(it->first)->set(pNew, it->second);
                    }
                }
                catch (const NII::Exception& e) 
                {
                    // No show-stopper, but do log it - log basic info (description, location, message), line and 
                    // file info (GUI exception specific) and VEH info if under Windows (stack trace, exception code & flags)
                    LogDebug(ExceptionManager::GetInstancePtr()->GetExceptionInfo(wxT("CopiedSelection::PasteWindowInt"), wxT("Unexpected Error - Property already exists."), StrHelper::ToWXString(e.getVerbose()), StrHelper::ToWXString(e.getFile()), e.getLine()));
                }
            }
            // As promised...
            if (m_applyPasteOffset)
            {   // Offset depends on paste counter. To get a cascading effect.
                RelVector2f position = pNew->getPos();  // Get rid of the const
                position += RelVector2f(RelPosf(0.05f * m_pasteCounter, 0.0f), RelPosf(0.05f * m_pasteCounter, 0.0f));
                pNew->setPos(position);
                m_applyPasteOffset = false;
            }
        }
        // If something goes wrong, add to log
        LOG_EXCEPTION("CopiedSelection::PasteWindowInt", "Error", "Unexpected standard exception while pasting window", "Unexpected non-standard exception while pasting window");
        return pNew;
    }
    //------------------------------------------------------------------------
    String CopiedSelection::CreateUniqueName( const String& oldName ) const
    {
        int nCounter = m_pasteCounter;
        String res = oldName;

        while (N_Only(Widget).isExist(res))
        {
            wxChar tempPostfix[100];
            (void)wxSnprintf (tempPostfix, 100, wxT("%d"), nCounter++);
            res = oldName + StrHelper::ToNIIString(tempPostfix);
        }
        return res;
    }
    //------------------------------------------------------------------------
    //////////////////////////////////////////////////////////////////////////
    // SELECTION MOVER FUNCTIONS
    //////////////////////////////////////////////////////////////////////////
    //------------------------------------------------------------------------
    SelectionMover::SelectionMover(Selection *s) : 
        m_selection(s)
    {
        // NOTE: Prefer constructor initialization lists (which are 
        // often more efficient) to assignment inside the 
        // constructor. Members in the initialization list should 
        // appear in the order they are declared in the class, as 
        // this is the order they will be initialized (no matter what).
        // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers
    }
    //------------------------------------------------------------------------
    void SelectionMover::AlignSelection(const Alignment alignment)
    {
        // Validations
        wxASSERT_MSG(m_selection != NULL, wxT("Selection member is NULL"));

        if(m_selection->Size() <= 1) 
        {
            // Should not happen because of the disabled menu/toolbar item in this case
            LogWarning(wxT("You must select more than one window to align!"));
            return;
        }

        // The first selected window is the one to match. This is for example how Visual Studio's 
        // dialog editor works as well.
        Selection::Boxes::iterator boxIt = m_selection->GetMoveableBoxes().begin();

        Widget *current = boxIt->GetWindow();

        const RelRectf & rect = current->getRelArea();
        ++boxIt;

        for(; boxIt != m_selection->GetMoveableBoxes().end(); ++boxIt) {
            // Deny when it is blocked
            if (boxIt->IsLocked())
                continue;

            current = boxIt->GetWindow();

            switch(alignment)
            {
            case AlignLeft:
                current->setPos(RelVector2f(rect.mLeft, current->getPos().y));
                break;
            case AlignRight:
                current->setPos(RelVector2f(rect.mRight - current->getWidth(), current->getPos().y));
                break;
            case AlignTop:
                current->setPos(RelVector2f(current->getPos().x, rect.mTop));
                break;
            case AlignBottom:
                current->setPos(RelVector2f(current->getPos().x, rect.mBottom - current->getHeight()));
                break;
            case AlignWidth:
                // The svn diff for this block will be big; no clue what the previous code
                // was doing there..
                current->setWidth(rect.getWidth());
                break;
            case AlignHeight:
                // The svn diff for this block will be big; no clue what the previous code
                // was doing there..
                current->setHeight(rect.getHeight());
                break;
            default:
                LogError(wxT("SelectionMover::AlignSelection - Unrecognized align option (%d)"), alignment);
                return;
            }
        }
        // Request for a repaint
        wxGetApp().GetLayoutEditorFrame()->Refresh();
    }
    //------------------------------------------------------------------------
    void SelectionMover::MoveUsingScale(const Widget * window, RelRectf & windowArea, const float left, const float top, const float right, const float bottom) const
    {
        // Validations
        wxASSERT_MSG(window != NULL, wxT("Supplied window is NULL"));

        // Calculate relative versions of the given values
        const PlaneSizei renderSize(N_Only(UI).getDisplaySize((Nid)0));

        // We need to know how much 1 pixel means relatively for the current resolution
        float scaleDeltaLeft = left / renderSize.mWidth;
        float scaleDeltaTop = top / renderSize.mHeight;
        float scaleDeltaRight = right / renderSize.mWidth;
        float scaleDeltaBottom = bottom / renderSize.mHeight;

        Rectf windowRect;
        Widget * tempwin = window->getParent();
        if (tempwin)
        {
            windowRect = abs(windowArea, tempwin->getPixelSize());
            windowRect.offset(Vector2f(tempwin->getActualPosX(), window->getActualPosY()));
            
        }
        else
        {
            windowRect = abs(windowArea, window->getPixelSize());
            windowRect.offset(Vector2f(tempwin->getActualPosX(), window->getActualPosY()));
            
        }

        // Avoid negative widths and heights
        if (windowRect.getWidth() + (right - left) > 1.0f && 
            windowRect.getHeight() + (bottom - top) > 1.0f)
        {
            const Widget * parent = window->getParent();
            while (parent)
            {
                // Update relative values when a parent is set.
                // When a parent is set, make the difference relatively larger.
                if (parent->getWidth().mScale)
                    scaleDeltaLeft /= parent->getWidth().mScale;
                if (parent->getHeight().mScale)
                    scaleDeltaTop /= parent->getHeight().mScale;
                if (parent->getWidth().mScale)
                    scaleDeltaRight /= parent->getWidth().mScale;
                if (parent->getHeight().mScale)
                    scaleDeltaBottom /= parent->getHeight().mScale;
                // next
                parent = parent->getParent();
            }

            // In RelPos land, there is no metrics mode anymore.
            // Only update the 'scale' part. The 'offset' part can only
            // be changed from the property grid in the main dialog.

            // Left
            windowArea.mLeft.mScale += scaleDeltaLeft;

            // Top
            windowArea.mTop.mScale += scaleDeltaTop;

            // Right
            windowArea.mRight.mScale += scaleDeltaRight;

            // Bottom
            windowArea.mBottom.mScale += scaleDeltaBottom;
        }
    }
    //------------------------------------------------------------------------
    void SelectionMover::MoveUsingOffset(RelRectf& windowArea, const float left, const float top, const float right, const float bottom) const
    {
        windowArea.mLeft.mOffset += left;
        windowArea.mTop.mOffset += top;
        windowArea.mRight.mOffset += right;
        windowArea.mBottom.mOffset += bottom;
    }
    //------------------------------------------------------------------------
    void SelectionMover::UpdateWindowAreas(float left, float top, float right, float bottom)
    {
        // Validations
        wxASSERT_MSG(m_selection != NULL, wxT("Selection member is NULL"));

        if (m_selection->Size() < 1)
        {	// Nothing to move
            return;
        }
        //LogDebug (wxT("UpdateWindowAreas - left: %f, top: %f, right: %f, bottom: %f"),
        //    left, top, right, bottom);

        //LogDebug (wxT("UpdateWindowAreas - renderWidth: %f, renderHeight: %f"),
        //  renderWidth, renderHeight);

        // We only move the moveable windows. These will automatically update their children
        Selection::Boxes& boxes = m_selection->GetMoveableBoxes();
        for (Selection::Boxes::iterator boxIt = boxes.begin(); boxIt != boxes.end(); ++boxIt)
        {   // Deny when it is blocked
            if (boxIt->IsLocked())
                continue;

            // Handy pointer
            const Widget * window = boxIt->GetWindow();

            // Get the entire rectangle in one call.
            RelRectf windowArea = window->getRelArea();    // Relative

            static bool autoMovement = true;
            if (autoMovement)
            {
                // Automatically use either scale or offset, depending which is dominant
                float sleft = 0.0f;
                float sright = 0.0f;
                float stop = 0.0f;
                float sbottom = 0.0f;
                float aleft = 0.0f;
                float aright = 0.0f;
                float atop = 0.0f;
                float abottom = 0.0f;
                // If offset being used, move absolutely
                if (windowArea.mLeft.mOffset == 0)		sleft = left;		else	aleft = left;
                if (windowArea.mRight.mOffset == 0)		sright = right;		else	aright = right;
                if (windowArea.mTop.mOffset == 0)		stop = top;			else	atop = top;
                if (windowArea.mBottom.mOffset == 0)		sbottom = bottom;	else	abottom = bottom;

                MoveUsingScale(window, windowArea, sleft, stop, sright, sbottom);
                MoveUsingOffset(windowArea, aleft, atop, aright, abottom);

                // Update via wrapper, so it updates the resize box as well.
                boxIt->SetNewWindowArea(windowArea);
            }
            else 
            {
                // Scale movement
                MoveUsingScale(window, windowArea, left, top, right, bottom);
                // Update via wrapper, so it updates the resize box as well.
                boxIt->SetNewWindowArea(windowArea);
            }    
        }
        // Update all boxes
        m_selection->Update();
    }

    //------------------------------------------------------------------------
    void SelectionMover::SnapToGrid()
    {
        // Validations
        wxASSERT_MSG(m_selection != NULL, wxT("Selection member is NULL"));

        const int step = UILayoutOptions::GetInstancePtr()->GetGridSize();

        // (grid does not have to be visible in order to be able to snap)
        if (m_selection->Size() < 1 || step < 1)
        {	// Nothing to snap
            return;
        }

        // Check with current settings
        const int snapMode = UILayoutOptions::GetInstancePtr()->GetSnapMode();
        const bool snapX = (snapMode == SNAP_X || snapMode == SNAP_XY);
        const bool snapY = (snapMode == SNAP_Y || snapMode == SNAP_XY);

        // Calculate relative versions of the given values
        const PlaneSizei renderSize(N_Only(UI).getDisplaySize((Nid)0));

        // Loop all windows and use the enum to check what direction to move to
        Selection::Boxes& boxes = m_selection->GetMoveableBoxes();
        for (Selection::Boxes::iterator boxIt = boxes.begin(); boxIt != boxes.end(); ++boxIt)
        {   // Handy pointer
            const Widget * window = boxIt->GetWindow();
            // "Floor" values.
            const Rectf absoluteRect = window->getArea(false).getFloatCache();
            RelVector2f relativePosition = window->getPos();

            // Will eventually be replaced by unified grid modifications.
            if (snapX)
            {
                const int deltaX = wx_static_cast(int, absoluteRect.mLeft) % step;
                // Apply this pixel value to the scale(relative) value.
                relativePosition.x.mScale -= deltaX / renderSize.mWidth;
            }
            if (snapY)
            {
                const int deltaY = wx_static_cast(int, absoluteRect.mTop) % step;
                // Apply this pixel value to the scale(relative) value.
                relativePosition.y.mScale -= deltaY / renderSize.mHeight;
            }

            // Update via the wrapper, so it can update the resize box as well.
            boxIt->SetNewWindowPosition(relativePosition);
        }
        // Update all boxes
        m_selection->Update();
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    WindowBox::WindowBox (Widget* aWindow, bool aResizable) :
        m_boxedWindow(aWindow),
        m_resizable(aResizable),
        m_locked(false)
    {
        // NOTE: Prefer constructor initialization lists (which are 
        // often more efficient) to assignment inside the 
        // constructor. Members in the initialization list should 
        // appear in the order they are declared in the class, as 
        // this is the order they will be initialized (no matter what).
        // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers

        // Validations
        wxASSERT_MSG(aWindow != NULL, wxT("NULL windows cannot be boxed."));

        LogDebug(wxT("Boxing '%s'"), StrHelper::ToWXString(m_boxedWindow->getName()).c_str());

        // Calculate the point positions
        Reset ();
    }
    //////////////////////////////////////////////////////////////////////////
    // WINDOWBOX FUNCTIONS
    //////////////////////////////////////////////////////////////////////////
    //------------------------------------------------------------------------
    bool WindowBox::operator==(WindowBox& aBox)
    {
        return (m_boxedWindow == aBox.m_boxedWindow);
    }

    //------------------------------------------------------------------------
    void WindowBox::Reset () 
    {
        m_locked = false;
        // Find the amount to convert from relative- to screen coordinates
        // Do this by finding the parent of the element (until no parent)
        const Rectf winRc = m_boxedWindow->getArea(false).getFloatCache();

        // Make boxes 3x3 and always in pixels
        //int width = 7;
        //int height = 7;
        const int paddX = 3;
        const int paddY = 3;

        // Position the "TopLeft" resizer
        m_resizePoints [RESIZE_POINT_WN].mLeft = winRc.mLeft - paddX;
        m_resizePoints [RESIZE_POINT_WN].mTop = winRc.mTop - paddY;
        // Position the "Top" resizer
        m_resizePoints [RESIZE_POINT_N].mLeft = winRc.mLeft + winRc.getWidth() / 2 - paddX;
        m_resizePoints [RESIZE_POINT_N].mTop = winRc.mTop - paddY;
        // Position the "TopRight" resizer
        m_resizePoints [RESIZE_POINT_NE].mLeft = winRc.mLeft + winRc.getWidth() - paddX;
        m_resizePoints [RESIZE_POINT_NE].mTop = winRc.mTop - paddY;
        // Position the "Right" resizer
        m_resizePoints [RESIZE_POINT_E].mLeft = winRc.mLeft + winRc.getWidth() - paddX;
        m_resizePoints [RESIZE_POINT_E].mTop = winRc.mTop + winRc.getHeight() / 2 - paddY;
        // Position the "BottomRight" resizer
        m_resizePoints [RESIZE_POINT_ES].mLeft = winRc.mLeft + winRc.getWidth() - paddX;
        m_resizePoints [RESIZE_POINT_ES].mTop = winRc.mTop + winRc.getHeight() - paddY;
        // Position the "Bottom" resizer
        m_resizePoints [RESIZE_POINT_S].mLeft = winRc.mLeft + winRc.getWidth() / 2 - paddX;
        m_resizePoints [RESIZE_POINT_S].mTop = winRc.mTop + winRc.getHeight() - paddY;
        // Position the "BottomLeft" resizer
        m_resizePoints [RESIZE_POINT_SW].mLeft = winRc.mLeft - paddX;
        m_resizePoints [RESIZE_POINT_SW].mTop = winRc.mTop + winRc.getHeight() - paddY;
        // Position the "Left" resizer
        m_resizePoints [RESIZE_POINT_W].mLeft = winRc.mLeft - paddX;
        m_resizePoints [RESIZE_POINT_W].mTop = winRc.mTop + winRc.getHeight() / 2 - paddY;

        for (int i = 0; i < NUM_RESIZE_POINTS; ++i) 
        {
            m_resizePoints [i].mRight = m_resizePoints [i].mLeft + (paddX * 2.0f) ;
            m_resizePoints [i].mBottom = m_resizePoints [i].mTop + (paddY * 2.0f) ;
        }
    }
    //------------------------------------------------------------------------
    wxRect WindowBox::GetPointRect (const int anIndex) const 
    {
        const Rectf rc = m_resizePoints [anIndex];
        return wxRect (wxPoint(wx_static_cast(int, rc.mLeft), wx_static_cast(int, rc.mTop)), wxPoint(wx_static_cast(int, rc.mRight), wx_static_cast(int, rc.mBottom)));
    }
    //------------------------------------------------------------------------
    wxPoint WindowBox::GetSidePoint(const int anDir, const bool udimOrigin) const
    {
        wxASSERT_MSG(m_boxedWindow != NULL, wxT("Boxed Window must exist"));
        RelVector2f boxPos;
        RelPosf hwidth;
        RelPosf hheight;
        switch (anDir)
        {
        case RESIZE_DIR_N:
            boxPos.x = m_boxedWindow->getPos().x;
            boxPos.y = m_boxedWindow->getPos().y;
            boxPos.x += m_boxedWindow->getWidth()/ RelPosf(2.0f,2.0f);
            if (udimOrigin)
            {
                boxPos.y.mOffset = 0;
            }
            break;
        case RESIZE_DIR_W:
            boxPos.x = m_boxedWindow->getPos().x;
            boxPos.y = m_boxedWindow->getPos().y;
            boxPos.y += m_boxedWindow->getHeight()/RelPosf(2.0f,2.0f);
            if (udimOrigin)
            {
                boxPos.x.mOffset = 0;
            }
            break;
        case RESIZE_DIR_S:
            boxPos.x = m_boxedWindow->getPos().x;
            boxPos.y = m_boxedWindow->getPos().y;
            boxPos.x += m_boxedWindow->getWidth()/RelPosf(2.0f,2.0f);
            boxPos.y += m_boxedWindow->getHeight();
            if (udimOrigin)
            {
                boxPos.y.mOffset = 0;
            }
            break;
        case RESIZE_DIR_E:
            boxPos.x = m_boxedWindow->getPos().x;
            boxPos.y = m_boxedWindow->getPos().y;
            boxPos.x += m_boxedWindow->getWidth();
            boxPos.y += m_boxedWindow->getHeight()/RelPosf(2.0f,2.0f);
            if (udimOrigin)
            {
                boxPos.x.mOffset = 0;
            }
            break;
        default:
            LogError(wxT("Invalid resize direction (%d) supplied"), anDir);
            return wxPoint(-1, -1);
        };
        // Now find it
        Vector2f absPos;
        Widget * temp = m_boxedWindow->getParent();
        if (temp)
        {
            absPos = Vector2f(temp->getActualPosX(), temp->getActualPosY()) + abs(boxPos, temp->getPixelSize());
        }
        else
        {
            // hmm what to do here?
            absPos = Vector2f(m_boxedWindow->getActualPosX(), m_boxedWindow->getActualPosY()) + abs(boxPos, m_boxedWindow->getPixelSize());
        }
        return wxPoint(wx_static_cast(int, absPos.x), wx_static_cast(int, absPos.y));
    }
    //------------------------------------------------------------------------
    wxRect WindowBox::GetWindowRect() const
    {
        const Rectf rc = m_boxedWindow->getArea().getFloatCache();
        return wxRect (wxPoint(wx_static_cast(int, rc.mLeft), wx_static_cast(int, rc.mTop)), wxPoint(wx_static_cast(int, rc.mRight), wx_static_cast(int, rc.mBottom)));
    }
    //------------------------------------------------------------------------
    int WindowBox::GetPointAtPosition (const Vector2f & aPoint) const
    {
        // If the window is locked or not resizable, we just cheat the Document
        if (m_locked || !m_resizable)
        {
            return RESIZE_POINT_NONE;
        }
        for (int i = 0; i < NUM_RESIZE_POINTS; ++i) 
        {
            if (m_resizePoints [i].intersects (aPoint))
            {
                return i ;
            }    
        }
        return RESIZE_POINT_NONE;
    }
    //------------------------------------------------------------------------
    void WindowBox::SetNewWindowArea(const RelRectf & newArea)
    {
        // Apply it to the GUI window
        m_boxedWindow->setRelArea(newArea);

        // Update the resizer positions
        Reset();
    }
    //------------------------------------------------------------------------
    void WindowBox::SetNewWindowPosition(const RelVector2f & newPosition)
    {
        // Apply it to the GUI window
        m_boxedWindow->setPos(newPosition);
        // Update the resizer positions
        Reset();
    }
    //------------------------------------------------------------------------
}