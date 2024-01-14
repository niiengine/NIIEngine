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

#ifndef _UILayout_COPIEDSELECTION_H_
#define _UILayout_COPIEDSELECTION_H_

#include "NiiPreInclude.h"
#include "gsgsPreInclude.h"
#include "NiiString.h"
#include "NiiUIWindow.h"

/** The resize box exists of 8 points.*/
#define NUM_RESIZE_POINTS 8

/** Locations of the points.*/
#define RESIZE_POINT_WN 0   // West-North
#define RESIZE_POINT_N  1   // North
#define RESIZE_POINT_NE 2   // North-East
#define RESIZE_POINT_E  3   // East
#define RESIZE_POINT_ES 4   // East-South
#define RESIZE_POINT_S  5   // South
#define RESIZE_POINT_SW 6   // South-West
#define RESIZE_POINT_W  7   // West

/** The resize box has 4 cardinal directions.*/
#define NUM_RESIZE_DIRS 4

/** Locations of the directions.*/
#define RESIZE_DIR_N  0   // North
#define RESIZE_DIR_E  1   // East
#define RESIZE_DIR_S  2   // South
#define RESIZE_DIR_W  3   // West

/** Value indicating that the mouse is not hovering on a resize point.*/
#define RESIZE_POINT_NONE -1

using namespace NII::UI;
using namespace NII;

namespace gsgs
{
    class WindowBox;
    //////////////////////////////////////////////////////////////////////////
    // SELECTION
    //////////////////////////////////////////////////////////////////////////

    /** This class maintains one ore more selected GUI windows, which the user can then modify as a 
     * group. Selected windows get "boxed" in @link=WindowBox}s, which provide a 
     * Powerpoint-like way to drag and resize the windows. An example of a class which uses a Selection to modify 
     * window properties, is the {@link=EditorDocument} class.
     */
    class Selection 
    {
    public:

        /************************************************************************
         * PUBLIC TYPEDEFS
         ************************************************************************/
        
        /** Helper types.*/
        typedef std::list<WindowBox>	Boxes;

        /************************************************************************
         * PUBLIC FUNCTIONS
         ************************************************************************/
        
        /** Returns the number of selected windows.*/
        size_t Size() const;

        /** Selects the given window, when it is not selected yet.
         * @param aWindow the window to add to the selection.
         */
        bool Select(Widget * aWindow);

        /** Selects all windows, starting with the given window. Only the given
         * window will be resizable and such. Children will get a non-resizable 'box'.
         * @param aWindow the window which is the start of our SelectAll process.
        */
        bool SelectAll(Widget * aWindow);

        /** Removes the given window from current selection.
         * @param aWindow the element to remove from the selection and resizers.
         */
        void Unselect(const Widget * aWindow);

        /** Returns whether the given window is currently selected.
         * @param aWindow the window which should be checked for being selected.
         */
        bool IsSelected(const Widget * aWindow);

        /** Unselects the entire current selection.*/
        void ClearSelection () ;

        /** Refreshes the boxes.*/
        void Update();

        /** @Returns the collection of ALL selected windows.*/
        Boxes& GetAllBoxes() 
        {
            return m_allBoxes;
        }

        /** Returns the collection of Boxes whos positions may be updated. To avoid dragging
        both parents and children, cause it would have a double effect on the children.*/
        Boxes& GetMoveableBoxes()
        {
            return m_moveableBoxes;
        }

        /** @Returns the box of the given window. Or NULL.*/
        WindowBox* FindBoxFor(const Widget* aWindow);

    private:
        /************************************************************************
         * PRIVATE FUNCTIONS
         ************************************************************************/

        /** Selects children of the given window (recursive)*/
        void SelectChildren(const Container* aWindow);

        /** Unselects children of the given window (recursive)*/
        void UnselectChildren(const Container * aWindow);

        /** Clears the given windowBox which stored the given window, from the given container.*/
        bool DeleteWindowBox(Boxes& aBoxes, const Widget* aWindow);

        /** Recalculates the resize boxes.*/
        void ResetBoxes(Boxes& aBoxes);
        
        /** Returns whether the given element is a parent or a child of one of the selected elements. Needed
         * to avoid problems when a parent and its child are 'dragged' at the same time. The child would get double
         * changes, because the parent forwards it already...
         *
         * @param aWindow the window to check.
         */
        bool IsParentOrChild(const Widget * aWindow);    
    private:
        /************************************************************************
         * PRIVATE VARIABLES
         ************************************************************************/
        
        /** Current (boxed) selection. Cleared when another overlay is being activated, or when
         * another element is being selected without shift or ctrl being pressed.*/
        Boxes m_allBoxes;	// Contains all selected windows, used for copy & pasting
        Boxes m_moveableBoxes;	// Only contains windows which may be moved (parents)
    };
    
    //////////////////////////////////////////////////////////////////////////
    // COPIEDSELECTION
    //////////////////////////////////////////////////////////////////////////

    /** This class contains a deep copy of a selection of Windows in the gui area. 
     * @see CELayoutEditorDoc::copySelection. Note that we only store the windows; not the entire "boxes".
     *
     * Deep copies have advantages over "shallow" copies, because copied or cut Windows can 
     * be deleted from the layout and still be pasted. There is no need for references.
     *
     * The class uses a counter variable to create unique names, in case a same object is 
     * pasted multiple times.
     */
    class CopiedSelection
    {
    public:
        CopiedSelection ();
        ~CopiedSelection ();
        
        /** Stores a deep copy of the given Window and all of its children. It does not 
         * check for duplicates or anything.
         *
         * @param pWindow the Window to create a deep copy of.
         */
        void Copy (const Widget * pSource);

        /** Pastes the deep copy recursively onto the given Window, which may not be NULL.
         *
         * @param aTarget the Window to paste onto.
         */
        void Paste(Widget * aTarget);

        /** Clears current copies.*/
        void Reset () ;

        /** Returns whether this object contains a copy at the moment.*/
        size_t Size () const;
    private:
        // Recursive storage
        struct DeepCopy {
            String strName;
            StyleViewID strType;
            Container * mParent;
            std::map<PropertyID, String> mPropertyList;
            std::vector<struct DeepCopy*> children;
            DeepCopy() {}
            ~DeepCopy()
            {
                for(unsigned int i = 0; i < children.size(); ++i)
                    delete children[i];
            }
        };

        /************************************************************************
         * PRIVATE VARIABLES
         ************************************************************************/
        
        /** Used to create unique names during pasting. This value is added to the currently
         * pasted name. E.g. "Panel/Button" becomes in additional pastes "Panel/Button1",
         * "Panel/Button2" and so on.*/
        int m_pasteCounter;

        /** To avoid many argument to the paste methods, we toggle this member.*/
        bool m_applyPasteOffset; // True for the first window, to avoid an exact overlap of positions.

        /** Contains deep copies of GUI elements.*/
        std::vector<DeepCopy*> m_deepCopies;

        /************************************************************************
         * PRIVATE FUNCTIONS
         ************************************************************************/
        
        void CopyWindow( DeepCopy& data, const Widget* pSource );
        void CopyWindowRecursive( DeepCopy& data, const Widget* pSource );
        Widget* PasteWindow( const DeepCopy& data, Container* pTarget );
        void PasteWindowRecursive( const DeepCopy& data, Container* pTarget );
        String CreateUniqueName( const String& oldName ) const;
    };
    
    //////////////////////////////////////////////////////////////////////////
    // SELECTIONMOVER
    //////////////////////////////////////////////////////////////////////////
    /** This class provides several operation to modify a selection of windows.*/
    class SelectionMover
    {
    public:

        /************************************************************************
         * PUBLIC ENUMS
         ************************************************************************/
        enum Alignment { AlignLeft, AlignRight, AlignTop, AlignBottom, AlignWidth, 
            AlignHeight
        };

        
        SelectionMover(Selection *);

        /************************************************************************
         * PUBLIC FUNCTIONS
         ************************************************************************/
        
        void AlignSelection(const Alignment alignment);

        /** Updates each window area in the current selection. It only updates the 'scales'; the 'offsets' can only be changed
         * in the property grid in the main dialog.*/
        void UpdateWindowAreas(float left, float top, float right, float bottom);

        /** Snaps the current selection to the current grid. The grid does not have to be visible.
         */
        void SnapToGrid();

    private:
        /************************************************************************
         * PRIVATE VARIABLES
         ************************************************************************/
        /** Move using scale
         */
        void MoveUsingScale(const Widget * window, RelRectf & windowArea, const float left, const float top, const float right, const float bottom) const;

        /** Move using offset
         */
        void MoveUsingOffset(RelRectf& windowArea, const float left, const float top, const float right, const float bottom) const;
        
        Selection * m_selection;
    };
    
    /** Class which wraps a selected Window. Provides for resizing and repositioning of that window,
    * besides it holds whether the window's properties may be updated (the 'locked' member.)*/
    class WindowBox
    {
    public:
        /** Constructor, accepts a window and a setting whether it can be resized.*/
        WindowBox(Widget * aWindow, bool aResizable = true);

        // Operator to help STL::list::remove
        bool operator==(WindowBox& aBox);
        
        /** Re-positions the resize-points. Used when the resizer is initialized or when the 
        * contained GuiElement has been resized.*/
        void Reset();

        /** Returns the window to which this resizer is attached.*/
        Widget * GetWindow()                  { return m_boxedWindow; }

        /** Checks if the mouse is within a resize point. If so, it returns the point's index (0-7).*/
        int GetPointAtPosition(const Vector2f& point) const;

        /** Returns an absolute rectangle for the given point.
         *
         * @param iIndex the point (0-7).
         * @return absolute rectangle.
         */
        wxRect GetPointRect(const int anIndex) const;

        /** Returns a point for the given side.
         * If udimOrigin is set we actually return the point corresponding
         * to the udim position with 0 scale.  That point is of particular
         * significance because it is the point we 'offset' from.
         *
         * @param anDir the point (0-3).
         * @param udimOrigin if true, finds the udim origin position
         * @return absolute point.
         */
        wxPoint GetSidePoint(const int anDir, const bool udimOrigin) const;

        /** Returns an absolute rectangle for the boxed Window.
         *
         * @return absolute rectangle.
         */
        wxRect GetWindowRect() const;

        /** Applies a new rectangle to the boxed window, and then updates the box itself.*/
        void SetNewWindowArea(const RelRectf& newArea);

        /** Applies a new position to the boxed window, and then updates the box itself.*/
        void SetNewWindowPosition(const RelVector2f & newPosition);

        /** Locks or unlocks the boxed window from allowing its properties to be changed.*/
        void Lock(bool lock = true)
        {
            m_locked = lock;
        }

        /** Returns the lock state of the boxed window.*/
        bool IsLocked()
        {
            return m_locked;
        }

        /** Returns the resize state of the boxed window.*/
        bool IsResizable()
        {
            return m_resizable;
        }
    private:
        /** The windows around which this resize box resides.*/
        Widget * m_boxedWindow ;

        /** 8 GUI rectangles with absolute and unclipped positions and sizes per point.*/
        Rectf m_resizePoints [NUM_RESIZE_POINTS] ;

        /** Holds whether the boxed window is locked. When true, its position and size can not be changed.
         * in the future, each window should be wrapped in this box so that locking information can also be remembered,
         * when a window is not selected anymore. Then add methods for "setSelected" and such.*/
        bool m_locked;
        bool m_resizable;	// False when selected but not resizable. Happens when a parent is selected.
    };
}
#endif
