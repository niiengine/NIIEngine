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

#ifndef _NII_UI_TREEITEM_H_
#define _NII_UI_TREEITEM_H_

#include "NiiUIPreInclude.h"
#include "NiiUIPixelUnitGrid.h"
#include "NiiColourArea.h"

namespace NII
{
namespace UI
{
    typedef vector<TreeItem *>::type TreeItemList;
    
    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TreeItem : public UIAlloc
    {
        friend class Tree;
    public:
        TreeItem(const String & text, Nui32 id = 0, void * data = 0, bool disable = false, bool autoDestroy = true);
        virtual ~TreeItem();

        virtual bool operator<(const TreeItem & o) const;

        virtual bool operator>(const TreeItem & o) const;
        
        /** 
        @version NIIEngine 3.0.0
        */
        void setFont(const Font * font);
        
        /** 
        @version NIIEngine 3.0.0
        */
        const Font * getFont() const;
        
        /** 
        @version NIIEngine 3.0.0
        */
        void setText(const String & text);
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline const String & getText() const {return mRawText;}

        /**
        @version NIIEngine 3.0.0
        */
        const String & getVisualText() const;

        /**
        @version NIIEngine 3.0.0
        */
        void setColour(const ColourArea & colour);
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline const ColourArea & getColour() const { return mColour; }

        /**
        @version NIIEngine 3.0.0
        */
        inline void setTooltipText(const String & text){ mTipsText = text; }
        
        /**
        @version NIIEngine 3.0.0
        */
        inline const String & getTooltipText() const { return mTipsText; }

        /**
        @version NIIEngine 3.0.0
        */
        inline void setID(Nui32 id){ mID = id; }

        /** 
        @verison NIIEngine 3.0.0
        */
        inline Nui32 getID() const { return mID; }
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline void setCustomData(void * data) { mCustomData = data; }

        /** 
        @verison NIIEngine 3.0.0
        */
        inline void * getCustomData() const { return mCustomData; }

        /** 
        @version NIIEngine 3.0.0
        */
        inline void select(bool set) { mSelect = set; }
        
        /** 
        @verison NIIEngine 3.0.0
        */
        inline bool isSelect() const { return mSelect; }

        /** 
        @version NIIEngine 3.0.0
        */
        inline void setDisable(bool set) { mDisable = set; }
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline bool isDisable() const { return mDisable; }

        /** 
        @version NIIEngine 3.0.0
        */
        inline void setAutoDestroy(bool set) { mAutoDestroy = set; }
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline bool isAutoDestroy() const { return mAutoDestroy; }
        
        /** 
        @version NIIEngine 3.0.0
         */
        inline void toggle(){ mUnfold = !mUnfold; }
        
        /** 
        @version NIIEngine 3.0.0
         */
        inline bool isUnfold() const{ return mUnfold; }

        /** 
        @version NIIEngine 3.0.0
        */
        inline void setOwnerWindow(const Container * parent) { mParent = parent; }
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline const Container * getOwnerWindow() const { return mParent; }

        /** 
        @version NIIEngine 3.0.0
        */
        inline void setSelectColour(const ColourArea & colour) { mSelectColour = colour; }
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline const ColourArea & getSelectColour() const { return mSelectColour; }

        /**
        @version NIIEngine 3.0.0
        */
        inline void setSelectView(const PixelBuffer * pb){ mSelectView = pb; }
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline const PixelBuffer * getSelectView() const { return mSelectView; }
        
        /** 
        @version NIIEngine 3.0.0
         */
        inline void setIcon(const PixelBuffer * pb) { mIconView = pb; }
        
        /** 
        @version NIIEngine 3.0.0
         */
        inline const PixelBuffer * getIcon() const  { return mIconView; }

        /** 
        @version NIIEngine 3.0.0
         */
        inline void setToggleArea(const Rectf & area){ mToggleArea = area; }
        
        /** 
        @version NIIEngine 3.0.0
         */
        inline const Rectf & getToggleArea() const{ return mToggleArea; }

        /** 
        @version NIIEngine 3.0.0
         */
        void addItem(TreeItem * item);
        
        /** 
        @version NIIEngine 3.0.0
        */
        void removeItem(const TreeItem * item);

        /** 
        @version NIIEngine 3.0.0
         */
        inline NCount getItemCount() const { return mItemList.size(); }

        /** 
        @version NIIEngine 3.0.0
         */
        inline TreeItemList & getItemList() { return mItemList; }

        /** 
        @version NIIEngine 3.0.0
         */
        inline const TreeItemList & getItemList() const { return mItemList; }

        /** 
        @version NIIEngine 3.0.0
        */
        virtual PlaneSizef getPixelSize() const;

        /** 
        @version NIIEngine 3.0.0
        */
        virtual void draw(GeometryPixel & buffer, const Rectf & area, NIIf alpha, const Rectf * clip) const;

        /** 
        @version NIIEngine 3.0.0
        */
        virtual bool notifyFont(const Font * font);
    protected:
        Nui32 mID;
        const Font * mFont;
        const Container * mParent;
        TreeItemList mItemList;
        String mRawText;
        BidiText * mBidiText;
        String mTipsText;
        Rectf mToggleArea;
        ColourArea mSelectColour;
        ColourArea mColour;
        void * mCustomData;
        mutable PixelUnitGrid mPixelGrid;
        const PixelBuffer * mSelectView;
        const PixelBuffer * mIconView;
        bool mSelect;
        bool mDisable;
        bool mAutoDestroy;
        bool mUnfold;
        mutable bool mGridValid;
        mutable bool mValidBidi;
    };
}
}
#endif