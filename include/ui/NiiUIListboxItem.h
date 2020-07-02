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

#ifndef _NII_UI_ListboxItem_H_
#define _NII_UI_ListboxItem_H_

#include "NiiUIPreInclude.h"
#include "NiiColourArea.h"
#include "NiiRect.h"
#include "NiiString.h"

namespace NII
{
namespace UI
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ListboxItem : public UIAlloc
    {
    public:
        ListboxItem(const String & text, Nui32 id = 0, void * data = 0,
            bool disable = false, bool autoDestroy = true);
        virtual ~ListboxItem();

        virtual bool operator< (const ListboxItem & o) const;

        virtual bool operator> (const ListboxItem & o) const;
        
        /** 
		@version NIIEngine 3.0.0
        */
        void setID(Nui32 id){mID = id;}
        
        /**
        @version NIIEngine 3.0.0
        */
        inline Nui32 getID() const {return mID;}

        /**
        @version NIIEngine 3.0.0
        */
        virtual void setText(const String & text);
        
        /**
        @version NIIEngine 3.0.0
        */
        const String & getText() const {return mRawText;}

        /**
        @version NIIEngine 3.0.0
        */
        const String & getVisualText() const;

        /**
        @version NIIEngine 3.0.0
        */
        inline void select(bool set) {mSelect = set;}
        
        /**
        @version NIIEngine 3.0.0
        */
        inline bool isSelect() const {return mSelect;}

        /** 
		@version NIIEngine 3.0.0
        */
        inline void setDisable(bool set) {mDisable = set;}
        
        /**
        @version NIIEngine 3.0.0
        */
        inline bool isDisable() const {return mDisable;}

        /**
        @version NIIEngine 3.0.0
        */
        inline void setAutoDestroy(bool set) {mAutoDestroy = set;}

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isAutoDestroy() const {return mAutoDestroy;}
        
        /** 
        @version NIIEngine 3.0.0
        */
        void setOwnerWindow(const Container * owner) {mParent = owner;}

        /**
        @version NIIEngine 3.0.0
        */
        inline const Container * getOwnerWindow() const {return mParent;}

        /** 
        @version NIIEngine 3.0.0
        */
        inline void setSelectColour(const ColourArea & colour) {mSelectColour = colour;}
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline const ColourArea & getSelectColour() const {return mSelectColour;}

        /** 
		@version NIIEngine 3.0.0
        */
        inline void setSelectView(const PixelBuffer * pb) {mSelectView = pb;}
        
        /**
        @version NIIEngine 3.0.0
        */
        inline const PixelBuffer * getSelectView() const {return mSelectView;}

        /**
        @version NIIEngine 3.0.0
        */
        inline void setTooltipText(const String & text) {mTipsText = text;}
        
        /**
        @version NIIEngine 3.0.0
        */
        inline const String & getTooltipText() const {return mTipsText;}

        /**
        @version NIIEngine 3.0.0
        */
        inline void setCustomData(void * data) {mCustomData = data;}

        /**
        @version NIIEngine 3.0.0
        */
        inline void * getCustomData() const {return mCustomData;}

        /**
        @version NIIEngine 3.0.0
        */
        virtual bool notifyFont(const Font * font);

        /**
        @version NIIEngine 3.0.0
        */
        virtual PlaneSizef getPixelSize() const = 0;

        /**
        @version NIIEngine 3.0.0
        */
        virtual void draw(GeometryPixel & buffer, const Rectf & rect, NIIf alpha, const Rectf * clip) const = 0;
    protected:
        Nui32 mID;
        const Container * mParent;
        String mRawText;
        String mTipsText;
        ColourArea mSelectColour;
        BidiText * mBidiText;
        void * mCustomData;
        bool mSelect;
        bool mDisable;
        bool mAutoDestroy;
        mutable bool mValidBidi;
        const PixelBuffer * mSelectView;
    };
    typedef vector<ListboxItem *>::type ListboxItemList;
}
}
#endif