/*
 -----------------------------------------------------------------------------
 This source file is part of OGRE
 (Object-oriented Graphics Rendering Engine)
 For the latest info, see http://www.ogre3d.org/

 Copyright (c) 2000-2012 Torus Knot Software Ltd

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 -----------------------------------------------------------------------------
 */
#ifndef __SdkTrays_H__
#define __SdkTrays_H__

#include "Ogre.h"
#include "OgreFontManager.h"
#include "OgreFrameOverlayGeo.h"
#include "OgreTextOverlayGeo.h"
#include <math.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#include "OgreStringSerialiser.h"
#endif
#include "OgreTimer.h"

#if OGRE_COMPILER == OGRE_COMPILER_MSVC
// TODO - remove this
#   pragma warning (disable : 4244)
#endif

#if OGRE_UNICODE_SUPPORT
    #define DISPLAY_STRING_TO_STRING(DS) (DS.asUTF8())
#else
    #define DISPLAY_STRING_TO_STRING(DS) (DS)
#endif
namespace OgreBites
{
    enum TrayLocation   // enumerator values for widget tray anchoring locations
    {
        TL_TOPLEFT,
        TL_TOP,
        TL_TOPRIGHT,
        TL_LEFT,
        TL_CENTER,
        TL_RIGHT,
        TL_BOTTOMLEFT,
        TL_BOTTOM,
        TL_BOTTOMRIGHT,
        TL_NONE
    };

    enum ButtonState   // enumerator values for button states
    {
        BS_UP,
        BS_OVER,
        BS_DOWN
    };

    // forward widget class declarations
    class Widget;
    class Button;
    class SelectMenu;
    class Label;
    class Slider;
    class CheckBox;

    /*=============================================================================
    | Listener class for responding to tray events.
    =============================================================================*/
    class SdkTrayListener
    {
    public:

        virtual ~SdkTrayListener() {}
        virtual void buttonHit(Button* button) {}
        virtual void itemSelected(SelectMenu* menu) {}
        virtual void labelHit(Label* label) {}
        virtual void sliderMoved(Slider* slider) {}
        virtual void checkBoxToggled(CheckBox* box) {}
        virtual void okDialogClosed(const NII::ViewString & message) {}
        virtual void yesNoDialogClosed(const NII::ViewString & question, bool yesHit) {}
    };

    /*=============================================================================
    | Abstract base class for all widgets.
    =============================================================================*/
    class Widget
    {
    public:

        Widget()
        {
            mTrayLoc = TL_NONE;
            mElement = 0;
            mListener = 0;
        }

        virtual ~Widget() {}

        void cleanup()
        {
            if (mElement) nukeOverlayGeo(mElement);
            mElement = 0;
        }

        /*-----------------------------------------------------------------------------
        | Static utility method to recursively delete an overlay element plus
        | all of its children from the system.
        -----------------------------------------------------------------------------*/
        static void nukeOverlayGeo(Ogre::OverlayGeo * element)
        {
            Ogre::OverlayView * container = dynamic_cast<Ogre::OverlayView*>(element);
            if(container)
            {
                std::vector<Ogre::OverlayGeo*> toDelete;

                NII::OverlayView::ChildMap::iterator c, cend = container->getList().end();
                for(c = container->getList().begin(); c != cend; ++c)
                {
                    toDelete.push_back(c->second);
                }

                for(unsigned int i = 0; i < toDelete.size(); i++)
                {
                    nukeOverlayGeo(toDelete[i]);
                }
            }
            if(element)
            {
                Ogre::OverlayView * parent = element->getParent();
                if(parent) 
                    parent->remove(element->getName());
                N_Only(Overlay).destroyGeo(element);
            }
        }

        /*-----------------------------------------------------------------------------
        | Static utility method to check if the cursor is over an overlay element.
        -----------------------------------------------------------------------------*/
        static bool isCursorOver(Ogre::OverlayGeo * element, const Ogre::Vector2 & cursorPos, Ogre::NIIf voidBorder = 0)
        {
            Ogre::NIIf l = element->getEnvLeft() * element->getScreenWidth();
            Ogre::NIIf t = element->getEnvTop() * element->getScreenHeight();
            Ogre::NIIf r = l + element->getWidth();
            Ogre::NIIf b = t + element->getHeight();

            return(cursorPos.x >= l + voidBorder && cursorPos.x <= r - voidBorder &&
                cursorPos.y >= t + voidBorder && cursorPos.y <= b - voidBorder);
        }

        /*-----------------------------------------------------------------------------
        | Static utility method used to get the cursor's offset from the center
        | of an overlay element in pixels.
        -----------------------------------------------------------------------------*/
        static Ogre::Vector2 cursorOffset(Ogre::OverlayGeo * element, const Ogre::Vector2 & cursorPos)
        {
            Ogre::OverlayManager & om = Ogre::OverlayManager::getSingleton();
            return Ogre::Vector2(cursorPos.x - (element->getEnvLeft() * element->getScreenWidth() + element->getWidth() / 2),
                cursorPos.y - (element->getEnvTop() * element->getScreenHeight() + element->getHeight() / 2));
        }

        /*-----------------------------------------------------------------------------
        | Static utility method used to get the width of a caption in a text area.
        -----------------------------------------------------------------------------*/
        static Ogre::NIIf getCaptionWidth(const NII::ViewString & caption, Ogre::TextOverlayGeo* area)
        {
            Ogre::Font * font = (Ogre::Font*)Ogre::FontManager::getSingleton().getByName(area->getFontID()).getPointer();
            Ogre::String current = DISPLAY_STRING_TO_STRING(caption);
            Ogre::NIIf lineWidth = 0;

            for (unsigned int i = 0; i < current.length(); i++)
            {
                // be sure to provide a line width in the text area
                if (current[i] == ' ')
                {
                    if (area->getSpaceWidth() != 0) lineWidth += area->getSpaceWidth();
                    else lineWidth += font->getCharAspect(' ') * area->getGlyphHeight();
                }
                else if (current[i] == '\n') break;
                // use glyph information to calculate line width
                else lineWidth += font->getCharAspect(current[i]) * area->getGlyphHeight();
            }

            return (unsigned int)lineWidth;
        }

        /*-----------------------------------------------------------------------------
        | Static utility method to cut off a string to fit in a text area.
        -----------------------------------------------------------------------------*/
        static void fitCaptionToArea(const NII::ViewString & caption, Ogre::TextOverlayGeo* area, Ogre::NIIf maxWidth)
        {
            Ogre::Font* f = (Ogre::Font*)Ogre::FontManager::getSingleton().getByName(area->getFontID()).getPointer();
            Ogre::String s = DISPLAY_STRING_TO_STRING(caption);

            int nl = s.find('\n');
            if (nl != -1) s = s.substr(0, nl);

            Ogre::NIIf width = 0;

            for (unsigned int i = 0; i < s.length(); i++)
            {
                if (s[i] == ' ' && area->getSpaceWidth() != 0) width += area->getSpaceWidth();
                else width += f->getCharAspect(s[i]) * area->getGlyphHeight();
                if (width > maxWidth)
                {
                    s = s.substr(0, i);
                    break;
                }
            }

            area->setCaption(s);
        }

        Ogre::OverlayGeo* getOverlayGeo()
        {
            return mElement;
        }

        const Ogre::String& getName()
        {
            return mElement->getName();
        }

        TrayLocation getTrayLocation()
        {
            return mTrayLoc;
        }

        void hide()
        {
            mElement->hide();
        }

        void show()
        {
            mElement->show();
        }

        bool isVisible()
        {
            return mElement->isVisible();
        }

        // callbacks

        virtual void _cursorPressed(const Ogre::Vector2& cursorPos) {}
        virtual void _cursorReleased(const Ogre::Vector2& cursorPos) {}
        virtual void _cursorMoved(const Ogre::Vector2& cursorPos) {}
        virtual void _focusLost() {}

        // internal methods used by SdkTrayManager. do not call directly.

        void _assignToTray(TrayLocation trayLoc) { mTrayLoc = trayLoc; }
        void _assignListener(SdkTrayListener* listener) { mListener = listener; }

    protected:

        Ogre::OverlayGeo* mElement;
        TrayLocation mTrayLoc;
        SdkTrayListener* mListener;
    };

    typedef std::vector<Widget*> WidgetList;
    typedef Ogre::VectorIterator<WidgetList> WidgetIterator;

    /*=============================================================================
    | Basic button class.
    =============================================================================*/
    class Button : public Widget
    {
    public:

        // Do not instantiate any widgets directly. Use SdkTrayManager.
        Button(const Ogre::String& name, const NII::ViewString & caption, Ogre::NIIf width)
        {
            mElement = Ogre::OverlayManager::getSingleton().createGeo("SdkTrays/Button", "BorderPanel", name);
            mBP = (Ogre::FrameOverlayGeo*)mElement;
            mTextArea = (Ogre::TextOverlayGeo*)mBP->get(mBP->getName() + "/ButtonCaption");
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            mTextArea->setGlyphHeight(mTextArea->getGlyphHeight() - 3);
#endif
            mTextArea->setTop(-(mTextArea->getGlyphHeight() / 2));

            if (width > 0)
            {
                mElement->setWidth(width);
                mFitToContents = false;
            }
            else mFitToContents = true;

            setCaption(caption);
            mState = BS_UP;
        }

        virtual ~Button() {}

        const NII::ViewString & getCaption()
        {
            return mTextArea->getCaption();
        }

        void setCaption(const NII::ViewString & caption)
        {
            mTextArea->setCaption(caption);
            if (mFitToContents) mElement->setWidth(getCaptionWidth(caption, mTextArea) + mElement->getHeight() - 12);
        }

        const ButtonState& getState() { return mState; }

        void _cursorPressed(const Ogre::Vector2& cursorPos)
        {
            if (isCursorOver(mElement, cursorPos, 4)) setState(BS_DOWN);
        }

        void _cursorReleased(const Ogre::Vector2& cursorPos)
        {
            if (mState == BS_DOWN)
            {
                setState(BS_OVER);
                if (mListener) mListener->buttonHit(this);
            }
        }

        void _cursorMoved(const Ogre::Vector2& cursorPos)
        {
            if (isCursorOver(mElement, cursorPos, 4))
            {
                if (mState == BS_UP) setState(BS_OVER);
            }
            else
            {
                if (mState != BS_UP) setState(BS_UP);
            }
        }

        void _focusLost()
        {
            setState(BS_UP);   // reset button if cursor was lost
        }

    protected:

        void setState(const ButtonState& bs)
        {
            if (bs == BS_OVER)
            {
                mBP->setMaterial("SdkTrays/Button/Over");
                mBP->setMaterialName("SdkTrays/Button/Over");
            }
            else if (bs == BS_UP)
            {
                mBP->setMaterial("SdkTrays/Button/Up");
                mBP->setMaterialName("SdkTrays/Button/Up");
            }
            else
            {
                mBP->setMaterial("SdkTrays/Button/Down");
                mBP->setMaterialName("SdkTrays/Button/Down");
            }

            mState = bs;
        }

        ButtonState mState;
        Ogre::FrameOverlayGeo* mBP;
        Ogre::TextOverlayGeo* mTextArea;
        bool mFitToContents;
    };

    /*=============================================================================
    | Scrollable text box widget.
    =============================================================================*/
    class TextBox : public Widget
    {
    public:
        // Do not instantiate any widgets directly. Use SdkTrayManager.
        TextBox(const Ogre::String & name, const Ogre::ViewString & caption,
            Ogre::NIIf width, Ogre::NIIf height)
        {
            mElement = Ogre::OverlayManager::getSingleton().createGeo("SdkTrays/TextBox", "BorderPanel", name);
            mElement->setWidth(width);
            mElement->setHeight(height);
            Ogre::OverlayView* container = (Ogre::OverlayView*)mElement;
            mTextArea = (Ogre::TextOverlayGeo*)container->get(getName() + "/TextBoxText");
            mCaptionBar = (Ogre::FrameOverlayGeo*)container->get(getName() + "/TextBoxCaptionBar");
            mCaptionBar->setWidth(width - 4);
            mCaptionTextArea = (Ogre::TextOverlayGeo*)mCaptionBar->get(mCaptionBar->getName() + "/TextBoxCaption");
            setCaption(caption);
            mScrollTrack = (Ogre::FrameOverlayGeo*)container->get(getName() + "/TextBoxScrollTrack");
            mScrollHandle = (Ogre::ContainerOverlayGeo*)mScrollTrack->get(mScrollTrack->getName() + "/TextBoxScrollHandle");
            mScrollHandle->hide();
            mDragging = false;
            mScrollPercentage = 0;
            mStartingLine = 0;
            mPadding = 15;
            mText = "";
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            mTextArea->setGlyphHeight(mTextArea->getGlyphHeight() - 3);
            mCaptionTextArea->setGlyphHeight(mCaptionTextArea->getGlyphHeight() - 3);
#endif
            refitContents();
        }

        void setPadding(Ogre::NIIf padding)
        {
            mPadding = padding;
            refitContents();
        }

        Ogre::NIIf getPadding()
        {
            return mPadding;
        }

        const Ogre::ViewString& getCaption()
        {
            return mCaptionTextArea->getCaption();
        }

        void setCaption(const Ogre::ViewString& caption)
        {
            mCaptionTextArea->setCaption(caption);
        }

        const Ogre::ViewString& getText()
        {
            return mText;
        }

        /*-----------------------------------------------------------------------------
        | Sets text box content. Most of this method is for wordwrap.
        -----------------------------------------------------------------------------*/
        void setText(const Ogre::ViewString& text)
        {
            mText = text;
            mLines.clear();

            Ogre::Font* font = (Ogre::Font*)Ogre::FontManager::getSingleton().get(mTextArea->getFontID()).getPointer();

            Ogre::String current = DISPLAY_STRING_TO_STRING(text);
            bool firstWord = true;
            unsigned int lastSpace = 0;
            unsigned int lineBegin = 0;
            Ogre::NIIf lineWidth = 0;
            Ogre::NIIf rightBoundary = mElement->getWidth() - 2 * mPadding + mScrollTrack->getLeft() + 10;

            for (unsigned int i = 0; i < current.length(); i++)
            {
                if (current[i] == ' ')
                {
                    if (mTextArea->getSpaceWidth() != 0) lineWidth += mTextArea->getSpaceWidth();
                    else lineWidth += font->getCharAspect(' ') * mTextArea->getGlyphHeight();
                    firstWord = false;
                    lastSpace = i;
                }
                else if (current[i] == '\n')
                {
                    firstWord = true;
                    lineWidth = 0;
                    mLines.push_back(current.substr(lineBegin, i - lineBegin));
                    lineBegin = i + 1;
                }
                else
                {
                    // use glyph information to calculate line width
                    lineWidth += font->getCharAspect(current[i]) * mTextArea->getGlyphHeight();
                    if (lineWidth > rightBoundary)
                    {
                        if (firstWord)
                        {
                            current.insert(i, "\n");
                            i = i - 1;
                        }
                        else
                        {
                            current[lastSpace] = '\n';
                            i = lastSpace - 1;
                        }
                    }
                }
            }

            mLines.push_back(current.substr(lineBegin));

            unsigned int maxLines = getHeightInLines();

            if (mLines.size() > maxLines)     // if too much text, filter based on scroll percentage
            {
                mScrollHandle->show();
                filterLines();
            }
            else       // otherwise just show all the text
            {
                mTextArea->setCaption(current);
                mScrollHandle->hide();
                mScrollPercentage = 0;
                mScrollHandle->setTop(0);
            }
        }

        /*-----------------------------------------------------------------------------
        | Sets text box content horizontal alignment.
        -----------------------------------------------------------------------------*/
        void setTextAlignment(NII::TextOverlayGeo::AlignMode ta)
        {
            if (ta == NII::TextOverlayGeo::AM_Left) mTextArea->setXAlign(NII::HA_LEFT);
            else if (ta == NII::TextOverlayGeo::AM_Center) mTextArea->setXAlign(NII::HA_CENTRE);
            else mTextArea->setXAlign(NII::HA_RIGHT);
            refitContents();
        }

        void clearText()
        {
            setText("");
        }

        void appendText(const NII::ViewString& text)
        {
            setText(getText() + text);
        }

        /*-----------------------------------------------------------------------------
        | Makes adjustments based on new padding, size, or alignment info.
        -----------------------------------------------------------------------------*/
        void refitContents()
        {
            mScrollTrack->setHeight(mElement->getHeight() - mCaptionBar->getHeight() - 20);
            mScrollTrack->setTop(mCaptionBar->getHeight() + 10);

            mTextArea->setTop(mCaptionBar->getHeight() + mPadding - 5);
            if (mTextArea->getXAlign() == Ogre::HA_RIGHT) mTextArea->setLeft(-mPadding + mScrollTrack->getLeft());
            else if (mTextArea->getXAlign() == Ogre::HA_LEFT) mTextArea->setLeft(mPadding);
            else mTextArea->setLeft(mScrollTrack->getLeft() / 2);

            setText(getText());
        }

        /*-----------------------------------------------------------------------------
        | Sets how far scrolled down the text is as a percentage.
        -----------------------------------------------------------------------------*/
        void setScrollPercentage(Ogre::NIIf percentage)
        {
            mScrollPercentage = Ogre::Clamp<Ogre::NIIf>(percentage, 0, 1);
            mScrollHandle->setTop((int)(percentage * (mScrollTrack->getHeight() - mScrollHandle->getHeight())));
            filterLines();
        }

        /*-----------------------------------------------------------------------------
        | Gets how far scrolled down the text is as a percentage.
        -----------------------------------------------------------------------------*/
        Ogre::NIIf getScrollPercentage()
        {
            return mScrollPercentage;
        }

        /*-----------------------------------------------------------------------------
        | Gets how many lines of text can fit in this window.
        -----------------------------------------------------------------------------*/
        unsigned int getHeightInLines()
        {
            return (unsigned int) ((mElement->getHeight() - 2 * mPadding - mCaptionBar->getHeight() + 5) / mTextArea->getGlyphHeight());
        }

        void _cursorPressed(const Ogre::Vector2& cursorPos)
        {
            if (!mScrollHandle->isVisible()) return;   // don't care about clicks if text not scrollable

            Ogre::Vector2 co = Widget::cursorOffset(mScrollHandle, cursorPos);

            if (co.lengthSquared() <= 81)
            {
                mDragging = true;
                mDragOffset = co.y;
            }
            else if (Widget::isCursorOver(mScrollTrack, cursorPos))
            {
                Ogre::NIIf newTop = mScrollHandle->getTop() + co.y;
                Ogre::NIIf lowerBoundary = mScrollTrack->getHeight() - mScrollHandle->getHeight();
                mScrollHandle->setTop(Ogre::Clamp<int>((int)newTop, 0, (int)lowerBoundary));

                // update text area contents based on new scroll percentage
                mScrollPercentage = Ogre::Clamp<Ogre::NIIf>(newTop / lowerBoundary, 0, 1);
                filterLines();
            }
        }

        void _cursorReleased(const Ogre::Vector2& cursorPos)
        {
            mDragging = false;
        }

        void _cursorMoved(const Ogre::Vector2& cursorPos)
        {
            if (mDragging)
            {
                Ogre::Vector2 co = Widget::cursorOffset(mScrollHandle, cursorPos);
                Ogre::NIIf newTop = mScrollHandle->getTop() + co.y - mDragOffset;
                Ogre::NIIf lowerBoundary = mScrollTrack->getHeight() - mScrollHandle->getHeight();
                mScrollHandle->setTop(Ogre::Clamp<int>((int)newTop, 0, (int)lowerBoundary));

                // update text area contents based on new scroll percentage
                mScrollPercentage = Ogre::Clamp<Ogre::NIIf>(newTop / lowerBoundary, 0, 1);
                filterLines();
            }
        }

        void _focusLost()
        {
            mDragging = false;  // stop dragging if cursor was lost
        }
    protected:
        /*-----------------------------------------------------------------------------
        | Decides which lines to show.
        -----------------------------------------------------------------------------*/
        void filterLines()
        {
            Ogre::String shown = "";
            unsigned int maxLines = getHeightInLines();
            unsigned int newStart = (unsigned int) (mScrollPercentage * (mLines.size() - maxLines) + 0.5);

            mStartingLine = newStart;

            for (unsigned int i = 0; i < maxLines; i++)
            {
                shown += mLines[mStartingLine + i] + "\n";
            }

            mTextArea->setCaption(shown);    // show just the filtered lines
        }

        Ogre::TextOverlayGeo* mTextArea;
        Ogre::FrameOverlayGeo* mCaptionBar;
        Ogre::TextOverlayGeo* mCaptionTextArea;
        Ogre::FrameOverlayGeo* mScrollTrack;
        Ogre::ContainerOverlayGeo* mScrollHandle;
        Ogre::ViewString mText;
        Ogre::StringList mLines;
        Ogre::NIIf mPadding;
        bool mDragging;
        Ogre::NIIf mScrollPercentage;
        Ogre::NIIf mDragOffset;
        unsigned int mStartingLine;
    };

    /*=============================================================================
    | Basic selection menu widget.
    =============================================================================*/
    class SelectMenu : public Widget
    {
    public:
        // Do not instantiate any widgets directly. Use SdkTrayManager.
        SelectMenu(const Ogre::String & name, const Ogre::ViewString & caption,
            Ogre::NIIf width, Ogre::NIIf boxWidth, unsigned int maxItemsShown):
            mHighlightIndex(0),
            mDisplayIndex(0),
            mDragOffset(0.0f)
        {
            mSelectionIndex = -1;
            mFitToContents = false;
            mCursorOver = false;
            mExpanded = false;
            mDragging = false;
            mMaxItemsShown = maxItemsShown;
            mItemsShown = 0;
            mElement = (Ogre::FrameOverlayGeo*)Ogre::OverlayManager::getSingleton().createGeo
                ("SdkTrays/SelectMenu", "BorderPanel", name);
            mTextArea = (Ogre::TextOverlayGeo*)((Ogre::OverlayView*)mElement)->get(name + "/MenuCaption");
            mSmallBox = (Ogre::FrameOverlayGeo*)((Ogre::OverlayView*)mElement)->get(name + "/MenuSmallBox");
            mSmallBox->setWidth(width - 10);
            mSmallTextArea = (Ogre::TextOverlayGeo*)mSmallBox->get(name + "/MenuSmallBox/MenuSmallText");
            mElement->setWidth(width);
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            mTextArea->setGlyphHeight(mTextArea->getGlyphHeight() - 3);
            mSmallTextArea->setGlyphHeight(mSmallTextArea->getGlyphHeight() - 3);
#endif

            if (boxWidth > 0)  // long style
            {
                if (width <= 0) mFitToContents = true;
                mSmallBox->setWidth(boxWidth);
                mSmallBox->setTop(2);
                mSmallBox->setLeft(width - boxWidth - 5);
                mElement->setHeight(mSmallBox->getHeight() + 4);
                mTextArea->setXAlign(Ogre::HA_LEFT);
                mTextArea->setAlignMode(Ogre::TextOverlayGeo::AM_Left);
                mTextArea->setLeft(12);
                mTextArea->setTop(10);
            }

            mExpandedBox = (Ogre::FrameOverlayGeo*)((Ogre::OverlayView*)mElement)->get(name + "/MenuExpandedBox");
            mExpandedBox->setWidth(mSmallBox->getWidth() + 10);
            mExpandedBox->hide();
            mScrollTrack = (Ogre::FrameOverlayGeo*)mExpandedBox->get(mExpandedBox->getName() + "/MenuScrollTrack");
            mScrollHandle = (Ogre::ContainerOverlayGeo*)mScrollTrack->get(mScrollTrack->getName() + "/MenuScrollHandle");

            setCaption(caption);
        }

        bool isExpanded()
        {
            return mExpanded;
        }

        const Ogre::ViewString& getCaption()
        {
            return mTextArea->getCaption();
        }

        void setCaption(const Ogre::ViewString& caption)
        {
            mTextArea->setCaption(caption);
            if (mFitToContents)
            {
                mElement->setWidth(getCaptionWidth(caption, mTextArea) + mSmallBox->getWidth() + 23);
                mSmallBox->setLeft(mElement->getWidth() - mSmallBox->getWidth() - 5);
            }
        }

        const Ogre::StringList& getItems()
        {
            return mItems;
        }

        unsigned int getNumItems()
        {
            return mItems.size();
        }

        void setItems(const Ogre::StringList& items)
        {
            mItems = items;
            mSelectionIndex = -1;

            for (unsigned int i = 0; i < mItemElements.size(); i++)   // destroy all the item elements
            {
                nukeOverlayGeo(mItemElements[i]);
            }
            mItemElements.clear();

            mItemsShown = std::max<int>(2, std::min<int>(mMaxItemsShown, mItems.size()));

            for (unsigned int i = 0; i < mItemsShown; i++)   // create all the item elements
            {
                Ogre::FrameOverlayGeo* e =
                    (Ogre::FrameOverlayGeo*)Ogre::OverlayManager::getSingleton().createGeo
                    ("SdkTrays/SelectMenuItem", "BorderPanel",
                    mExpandedBox->getName() + "/Item" + Ogre::StrConv::conv(i + 1));

                e->setTop(6 + i * (mSmallBox->getHeight() - 8));
                e->setWidth(mExpandedBox->getWidth() - 32);

                mExpandedBox->add(e);
                mItemElements.push_back(e);
            }

            if (!items.empty()) selectItem(0, false);
            else mSmallTextArea->setCaption("");
        }

        void addItem(const Ogre::ViewString& item)
        {
            mItems.push_back(item);
            setItems(mItems);
        }

        void removeItem(const Ogre::ViewString& item)
        {
            Ogre::StringList::iterator it;

            for (it = mItems.begin(); it != mItems.end(); it++)
            {
                if (item == *it) break;
            }

            if (it != mItems.end())
            {
                mItems.erase(it);
                if (mItems.size() < mItemsShown)
                {
                    mItemsShown = mItems.size();
                    nukeOverlayGeo(mItemElements.back());
                    mItemElements.pop_back();
                }
            }
            else
            {
                Ogre::String desc = "Menu \"" + getName() + "\" contains no item \"" + item + "\".";
                N_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, desc, "SelectMenu::removeItem");
            }
        }

        void removeItem(unsigned int index)
        {
            Ogre::StringList::iterator it;
            unsigned int i = 0;

            for (it = mItems.begin(); it != mItems.end(); it++)
            {
                if (i == index) break;
                i++;
            }

            if (it != mItems.end())
            {
                mItems.erase(it);
                if (mItems.size() < mItemsShown)
                {
                    mItemsShown = mItems.size();
                    nukeOverlayGeo(mItemElements.back());
                    mItemElements.pop_back();
                }
            }
            else
            {
                Ogre::String desc = "Menu \"" + getName() + "\" contains no item at position " +
                    Ogre::StrConv::conv(index) + ".";
                N_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, desc, "SelectMenu::removeItem");
            }
        }

        void clearItems()
        {
            mItems.clear();
            mSelectionIndex = -1;
            mSmallTextArea->setCaption("");
        }

        void selectItem(unsigned int index, bool notifyListener = true)
        {
            if (index >= mItems.size())
            {
                Ogre::String desc = "Menu \"" + getName() + "\" contains no item at position " +
                    Ogre::StrConv::conv(index) + ".";
                N_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, desc, "SelectMenu::selectItem");
            }

            mSelectionIndex = index;
            fitCaptionToArea(mItems[index], mSmallTextArea, mSmallBox->getWidth() - mSmallTextArea->getLeft() * 2);

            if (mListener && notifyListener) mListener->itemSelected(this);
        }

        void selectItem(const Ogre::ViewString& item, bool notifyListener = true)
        {
            for (unsigned int i = 0; i < mItems.size(); i++)
            {
                if (item == mItems[i])
                {
                    selectItem(i, notifyListener);
                    return;
                }
            }

            Ogre::String desc = "Menu \"" + getName() + "\" contains no item \"" + item + "\".";
            N_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, desc, "SelectMenu::selectItem");
        }

        Ogre::ViewString getSelectItem()
        {
            if (mSelectionIndex == -1)
            {
                Ogre::String desc = "Menu \"" + getName() + "\" has no item selected.";
                N_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, desc, "SelectMenu::getSelectItem");
                return "";
            }
            else return mItems[mSelectionIndex];
        }

        int getSelectionIndex()
        {
            return mSelectionIndex;
        }

        void _cursorPressed(const Ogre::Vector2& cursorPos)
        {
            Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();

            if (mExpanded)
            {
                if (mScrollHandle->isVisible())   // check for scrolling
                {
                    Ogre::Vector2 co = Widget::cursorOffset(mScrollHandle, cursorPos);

                    if (co.lengthSquared() <= 81)
                    {
                        mDragging = true;
                        mDragOffset = co.y;
                        return;
                    }
                    else if (Widget::isCursorOver(mScrollTrack, cursorPos))
                    {
                        Ogre::NIIf newTop = mScrollHandle->getTop() + co.y;
                        Ogre::NIIf lowerBoundary = mScrollTrack->getHeight() - mScrollHandle->getHeight();
                        mScrollHandle->setTop(Ogre::Clamp<int>((int)newTop, 0, (int)lowerBoundary));

                        Ogre::NIIf scrollPercentage = Ogre::Clamp<Ogre::NIIf>(newTop / lowerBoundary, 0, 1);
                        setDisplayIndex((unsigned int)(scrollPercentage * (mItems.size() - mItemElements.size()) + 0.5));
                        return;
                    }
                }

                if(!isCursorOver(mExpandedBox, cursorPos, 3)) 
                    retract();
                else
                {
                    Ogre::NIIf l = mItemElements.front()->getEnvLeft() * mItemElements.front()->getScreenWidth() + 5;
                    Ogre::NIIf t = mItemElements.front()->getEnvTop() * mItemElements.front()->getScreenHeight() + 5;
                    Ogre::NIIf r = l + mItemElements.back()->getWidth() - 10;
                    Ogre::NIIf b = mItemElements.back()->getEnvTop() * mItemElements.getScreenHeight() +
                        mItemElements.back()->getHeight() - 5;

                    if (cursorPos.x >= l && cursorPos.x <= r && cursorPos.y >= t && cursorPos.y <= b)
                    {
                        if (mHighlightIndex != mSelectionIndex) selectItem(mHighlightIndex);
                        retract();
                    }
                }
            }
            else
            {
                if (mItems.size() < 2) return;   // don't waste time showing a menu if there's no choice

                if (isCursorOver(mSmallBox, cursorPos, 4))
                {
                    mExpandedBox->show();
                    mSmallBox->hide();

                    // calculate how much vertical space we need
                    Ogre::NIIf idealHeight = mItemsShown * (mSmallBox->getHeight() - 8) + 20;
                    mExpandedBox->setHeight(idealHeight);
                    mScrollTrack->setHeight(mExpandedBox->getHeight() - 20);

                    mExpandedBox->setLeft(mSmallBox->getLeft() - 4);

                    // if the expanded menu goes down off the screen, make it go up instead
                    if (mSmallBox->getEnvTop() * mSmallBox->getScreenHeight() + idealHeight > mSmallBox->getScreenHeight())
                    {
                        mExpandedBox->setTop(mSmallBox->getTop() + mSmallBox->getHeight() - idealHeight + 3);
                        // if we're in thick style, hide the caption because it will interfere with the expanded menu
                        if (mTextArea->getXAlign() == Ogre::HA_CENTRE) mTextArea->hide();
                    }
                    else mExpandedBox->setTop(mSmallBox->getTop() + 3);

                    mExpanded = true;
                    mHighlightIndex = mSelectionIndex;
                    setDisplayIndex(mHighlightIndex);

                    if (mItemsShown < mItems.size())  // update scrollbar position
                    {
                        mScrollHandle->show();
                        Ogre::NIIf lowerBoundary = mScrollTrack->getHeight() - mScrollHandle->getHeight();
                        mScrollHandle->setTop((int)(mDisplayIndex * lowerBoundary / (mItems.size() - mItemElements.size())));
                    }
                    else mScrollHandle->hide();
                }
            }
        }

        void _cursorReleased(const Ogre::Vector2& cursorPos)
        {
            mDragging = false;
        }

        void _cursorMoved(const Ogre::Vector2& cursorPos)
        {
            Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();

            if (mExpanded)
            {
                if (mDragging)
                {
                    Ogre::Vector2 co = Widget::cursorOffset(mScrollHandle, cursorPos);
                    Ogre::NIIf newTop = mScrollHandle->getTop() + co.y - mDragOffset;
                    Ogre::NIIf lowerBoundary = mScrollTrack->getHeight() - mScrollHandle->getHeight();
                    mScrollHandle->setTop(Ogre::Clamp<int>((int)newTop, 0, (int)lowerBoundary));

                    Ogre::NIIf scrollPercentage = Ogre::Clamp<Ogre::NIIf>(newTop / lowerBoundary, 0, 1);
                    int newIndex = (int) (scrollPercentage * (mItems.size() - mItemElements.size()) + 0.5);
                    if (newIndex != mDisplayIndex) setDisplayIndex(newIndex);
                    return;
                }

                Ogre::NIIf l = mItemElements.front()->getEnvLeft() * mItemElements.front()->getScreenWidth() + 5;
                Ogre::NIIf t = mItemElements.front()->getEnvTop() * mItemElements.front()->getScreenHeight() + 5;
                Ogre::NIIf r = l + mItemElements.back()->getWidth() - 10;
                Ogre::NIIf b = mItemElements.back()->getEnvTop() * mItemElements.back()->getScreenHeight() +
                    mItemElements.back()->getHeight() - 5;

                if (cursorPos.x >= l && cursorPos.x <= r && cursorPos.y >= t && cursorPos.y <= b)
                {
                    int newIndex = (int)(mDisplayIndex + (cursorPos.y - t) / (b - t) * mItemElements.size());
                    if (mHighlightIndex != newIndex)
                    {
                        mHighlightIndex = newIndex;
                        setDisplayIndex(mDisplayIndex);
                    }
                }
            }
            else
            {
                if (isCursorOver(mSmallBox, cursorPos, 4))
                {
                    mSmallBox->setMaterialName("SdkTrays/MiniTextBox/Over");
                    mSmallBox->setMaterial("SdkTrays/MiniTextBox/Over");
                    mCursorOver = true;
                }
                else
                {
                    if (mCursorOver)
                    {
                        mSmallBox->setMaterialName("SdkTrays/MiniTextBox");
                        mSmallBox->setMaterial("SdkTrays/MiniTextBox");
                        mCursorOver = false;
                    }
                }
            }
        }

        void _focusLost()
        {
            if (mExpandedBox->isVisible()) retract();
        }

    protected:
        /*-----------------------------------------------------------------------------
        | Internal method - sets which item goes at the top of the expanded menu.
        -----------------------------------------------------------------------------*/
        void setDisplayIndex(unsigned int index)
        {
            index = std::min<int>(index, mItems.size() - mItemElements.size());
            mDisplayIndex = index;
            Ogre::FrameOverlayGeo* ie;
            Ogre::TextOverlayGeo* ta;

            for (int i = 0; i < (int)mItemElements.size(); i++)
            {
                ie = mItemElements[i];
                ta = (Ogre::TextOverlayGeo*)ie->get(ie->getName() + "/MenuItemText");

                fitCaptionToArea(mItems[mDisplayIndex + i], ta, ie->getWidth() - 2 * ta->getLeft());

                if ((mDisplayIndex + i) == mHighlightIndex)
                {
                    ie->setMaterialName("SdkTrays/MiniTextBox/Over");
                    ie->setMaterial("SdkTrays/MiniTextBox/Over");
                }
                else
                {
                    ie->setMaterialName("SdkTrays/MiniTextBox");
                    ie->setMaterial("SdkTrays/MiniTextBox");
                }
            }
        }

        /*-----------------------------------------------------------------------------
        | Internal method - cleans up an expanded menu.
        -----------------------------------------------------------------------------*/
        void retract()
        {
            mDragging = false;
            mExpanded = false;
            mExpandedBox->hide();
            mTextArea->show();
            mSmallBox->show();
            mSmallBox->setMaterialName("SdkTrays/MiniTextBox");
            mSmallBox->setMaterial("SdkTrays/MiniTextBox");
        }

        Ogre::FrameOverlayGeo* mSmallBox;
        Ogre::FrameOverlayGeo* mExpandedBox;
        Ogre::TextOverlayGeo* mTextArea;
        Ogre::TextOverlayGeo* mSmallTextArea;
        Ogre::FrameOverlayGeo* mScrollTrack;
        Ogre::ContainerOverlayGeo* mScrollHandle;
        std::vector<Ogre::FrameOverlayGeo*> mItemElements;
        unsigned int mMaxItemsShown;
        unsigned int mItemsShown;
        bool mCursorOver;
        bool mExpanded;
        bool mFitToContents;
        bool mDragging;
        Ogre::StringList mItems;
        int mSelectionIndex;
        int mHighlightIndex;
        int mDisplayIndex;
        Ogre::NIIf mDragOffset;
    };

    /*=============================================================================
    | Basic label widget.
    =============================================================================*/
    class Label : public Widget
    {
    public:
        // Do not instantiate any widgets directly. Use SdkTrayManager.
        Label(const Ogre::String& name, const Ogre::ViewString& caption, Ogre::NIIf width)
        {
            mElement = Ogre::OverlayManager::getSingleton().createGeo("SdkTrays/Label", "BorderPanel", name);
            mTextArea = (Ogre::TextOverlayGeo*)((Ogre::OverlayView*)mElement)->get(getName() + "/LabelCaption");
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            mTextArea->setGlyphHeight(mTextArea->getGlyphHeight() - 3);
#endif
            setCaption(caption);
            if (width <= 0) mFitToTray = true;
            else
            {
                mFitToTray = false;
                mElement->setWidth(width);
            }
        }

        const Ogre::ViewString& getCaption()
        {
            return mTextArea->getCaption();
        }

        void setCaption(const Ogre::ViewString& caption)
        {
            mTextArea->setCaption(caption);
        }

        void _cursorPressed(const Ogre::Vector2& cursorPos)
        {
            if (mListener && isCursorOver(mElement, cursorPos, 3)) mListener->labelHit(this);
        }

        bool _isFitToTray()
        {
            return mFitToTray;
        }

    protected:
        Ogre::TextOverlayGeo * mTextArea;
        bool mFitToTray;
    };

    /*=============================================================================
    | Basic separator widget.
    =============================================================================*/
    class Separator : public Widget
    {
    public:
        // Do not instantiate any widgets directly. Use SdkTrayManager.
        Separator(const Ogre::String& name, Ogre::NIIf width)
        {
            mElement = Ogre::OverlayManager::getSingleton().createGeo("SdkTrays/Separator", "Panel", name);
            if (width <= 0) mFitToTray = true;
            else
            {
                mFitToTray = false;
                mElement->setWidth(width);
            }
        }

        bool _isFitToTray()
        {
            return mFitToTray;
        }
    protected:
        bool mFitToTray;
    };

    /*=============================================================================
    | Basic slider widget.
    =============================================================================*/
    class Slider : public Widget
    {
    public:

        // Do not instantiate any widgets directly. Use SdkTrayManager.
        Slider(const Ogre::String& name, const Ogre::ViewString& caption, Ogre::NIIf width, Ogre::NIIf trackWidth,
            Ogre::NIIf valueBoxWidth, Ogre::NIIf minValue, Ogre::NIIf maxValue, unsigned int snaps)
            : mDragOffset(0.0f)
            , mValue(0.0f)
            , mMinValue(0.0f)
            , mMaxValue(0.0f)
            , mInterval(0.0f)
        {
            mDragging = false;
            mFitToContents = false;
            mElement = Ogre::OverlayManager::getSingleton().createGeo
                ("SdkTrays/Slider", "BorderPanel", name);
            mElement->setWidth(width);
            Ogre::OverlayView* c = (Ogre::OverlayView*)mElement;
            mTextArea = (Ogre::TextOverlayGeo*)c->get(getName() + "/SliderCaption");
            Ogre::OverlayView* valueBox = (Ogre::OverlayView*)c->get(getName() + "/SliderValueBox");
            valueBox->setWidth(valueBoxWidth);
            valueBox->setLeft(-(valueBoxWidth + 5));
            mValueTextArea = (Ogre::TextOverlayGeo *)valueBox->get(valueBox->getName() + "/SliderValueText");
            mTrack = (Ogre::FrameOverlayGeo *)c->get(getName() + "/SliderTrack");
            mHandle = (Ogre::ContainerOverlayGeo *)mTrack->get(mTrack->getName() + "/SliderHandle");
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            mTextArea->setGlyphHeight(mTextArea->getGlyphHeight() - 3);
            mValueTextArea->setGlyphHeight(mValueTextArea->getGlyphHeight() - 3);
#endif

            if (trackWidth <= 0)  // tall style
            {
                mTrack->setWidth(width - 16);
            }
            else  // long style
            {
                if (width <= 0) mFitToContents = true;
                mElement->setHeight(34);
                mTextArea->setTop(10);
                valueBox->setTop(2);
                mTrack->setTop(-23);
                mTrack->setWidth(trackWidth);
                mTrack->setXAlign(Ogre::HA_RIGHT);
                mTrack->setLeft(-(trackWidth + valueBoxWidth + 5));
            }

            setCaption(caption);
            setRange(minValue, maxValue, snaps, false);
        }

        /*-----------------------------------------------------------------------------
        | Sets the minimum value, maximum value, and the number of snapping points.
        -----------------------------------------------------------------------------*/
        void setRange(Ogre::NIIf minValue, Ogre::NIIf maxValue, unsigned int snaps, bool notifyListener = true)
        {
            mMinValue = minValue;
            mMaxValue = maxValue;

            if (snaps <= 1 || mMinValue >= mMaxValue)
            {
                mInterval = 0;
                mHandle->hide();
                mValue = minValue;
                if (snaps == 1) mValueTextArea->setCaption(Ogre::StrConv::conv(mMinValue));
                else mValueTextArea->setCaption("");
            }
            else
            {
                mHandle->show();
                mInterval = (maxValue - minValue) / (snaps - 1);
                setValue(minValue, notifyListener);
            }
        }

        const Ogre::ViewString& getValueCaption()
        {
            return mValueTextArea->getCaption();
        }

        /*-----------------------------------------------------------------------------
        | You can use this method to manually format how the value is displayed.
        -----------------------------------------------------------------------------*/
        void setValueCaption(const Ogre::ViewString& caption)
        {
            mValueTextArea->setCaption(caption);
        }

        void setValue(Ogre::NIIf value, bool notifyListener = true)
        {
            if(mInterval == 0)
                return;

            mValue = Ogre::Clamp<Ogre::NIIf>(value, mMinValue, mMaxValue);

            setValueCaption(Ogre::StrConv::conv(mValue));

            if(mListener && notifyListener)
                mListener->sliderMoved(this);

            if(!mDragging)
                mHandle->setLeft((int)((mValue - mMinValue) / (mMaxValue - mMinValue) *
                (mTrack->getWidth() - mHandle->getWidth())));
        }

        Ogre::NIIf getValue()
        {
            return mValue;
        }

        const Ogre::ViewString& getCaption()
        {
            return mTextArea->getCaption();
        }

        void setCaption(const Ogre::ViewString& caption)
        {
            mTextArea->setCaption(caption);

            if (mFitToContents) mElement->setWidth(getCaptionWidth(caption, mTextArea) +
                mValueTextArea->getParent()->getWidth() + mTrack->getWidth() + 26);
        }

        void _cursorPressed(const Ogre::Vector2& cursorPos)
        {
            if(!mHandle->isVisible())
                return;

            Ogre::Vector2 co = Widget::cursorOffset(mHandle, cursorPos);

            if(co.lengthSquared() <= 81)
            {
                mDragging = true;
                mDragOffset = co.x;
            }
            else if(Widget::isCursorOver(mTrack, cursorPos))
            {
                Ogre::NIIf newLeft = mHandle->getLeft() + co.x;
                Ogre::NIIf rightBoundary = mTrack->getWidth() - mHandle->getWidth();

                mHandle->setLeft(Ogre::Clamp<int>((int)newLeft, 0, (int)rightBoundary));
                setValue(getSnappedValue(newLeft / rightBoundary));
            }
        }

        void _cursorReleased(const Ogre::Vector2& cursorPos)
        {
            if (mDragging)
            {
                mDragging = false;
                mHandle->setLeft((int)((mValue - mMinValue) / (mMaxValue - mMinValue) *
                    (mTrack->getWidth() - mHandle->getWidth())));
            }
        }

        void _cursorMoved(const Ogre::Vector2& cursorPos)
        {
            if (mDragging)
            {
                Ogre::Vector2 co = Widget::cursorOffset(mHandle, cursorPos);
                Ogre::NIIf newLeft = mHandle->getLeft() + co.x - mDragOffset;
                Ogre::NIIf rightBoundary = mTrack->getWidth() - mHandle->getWidth();

                mHandle->setLeft(Ogre::Clamp<int>((int)newLeft, 0, (int)rightBoundary));
                setValue(getSnappedValue(newLeft / rightBoundary));
            }
        }

        void _focusLost()
        {
            mDragging = false;
        }

    protected:

        /*-----------------------------------------------------------------------------
        | Internal method - given a percentage (from left to right), gets the
        | value of the nearest marker.
        -----------------------------------------------------------------------------*/
        Ogre::NIIf getSnappedValue(Ogre::NIIf percentage)
        {
            percentage = Ogre::Clamp<Ogre::NIIf>(percentage, 0, 1);
            unsigned int whichMarker = (unsigned int) (percentage * (mMaxValue - mMinValue) / mInterval + 0.5);
            return whichMarker * mInterval + mMinValue;
        }

        Ogre::TextOverlayGeo* mTextArea;
        Ogre::TextOverlayGeo* mValueTextArea;
        Ogre::FrameOverlayGeo* mTrack;
        Ogre::ContainerOverlayGeo* mHandle;
        bool mDragging;
        bool mFitToContents;
        Ogre::NIIf mDragOffset;
        Ogre::NIIf mValue;
        Ogre::NIIf mMinValue;
        Ogre::NIIf mMaxValue;
        Ogre::NIIf mInterval;
    };

    /*=============================================================================
    | Basic parameters panel widget.
    =============================================================================*/
    class ParamsPanel : public Widget
    {
    public:

        // Do not instantiate any widgets directly. Use SdkTrayManager.
        ParamsPanel(const Ogre::String& name, Ogre::NIIf width, unsigned int lines)
        {
            mElement = Ogre::OverlayManager::getSingleton().createGeo
                ("SdkTrays/ParamsPanel", "BorderPanel", name);
            Ogre::OverlayView* c = (Ogre::OverlayView*)mElement;
            mNamesArea = (Ogre::TextOverlayGeo*)c->get(getName() + "/ParamsPanelNames");
            mValuesArea = (Ogre::TextOverlayGeo*)c->get(getName() + "/ParamsPanelValues");
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            mNamesArea->setGlyphHeight(mNamesArea->getGlyphHeight() - 3);
            mValuesArea->setGlyphHeight(mValuesArea->getGlyphHeight() - 3);
#endif
            mElement->setWidth(width);
            mElement->setHeight(mNamesArea->getTop() * 2 + lines * mNamesArea->getGlyphHeight());
        }

        void setAllParamNames(const Ogre::StringList& paramNames)
        {
            mNames = paramNames;
            mValues.clear();
            mValues.resize(mNames.size(), "");
            mElement->setHeight(mNamesArea->getTop() * 2 + mNames.size() * mNamesArea->getGlyphHeight());
            updateText();
        }

        const Ogre::StringList& getAllParamNames()
        {
            return mNames;
        }

        void setAllParamValues(const Ogre::StringList& paramValues)
        {
            mValues = paramValues;
            mValues.resize(mNames.size(), "");
            updateText();
        }

        void setParamValue(const Ogre::ViewString& paramName, const Ogre::ViewString& paramValue)
        {
            for (unsigned int i = 0; i < mNames.size(); i++)
            {
                if (mNames[i] == DISPLAY_STRING_TO_STRING(paramName))
                {
                    mValues[i] = DISPLAY_STRING_TO_STRING(paramValue);
                    updateText();
                    return;
                }
            }

            Ogre::String desc = "ParamsPanel \"" + getName() + "\" has no parameter \"" + DISPLAY_STRING_TO_STRING(paramName) + "\".";
            N_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, desc, "ParamsPanel::setParamValue");
        }

        void setParamValue(unsigned int index, const Ogre::ViewString& paramValue)
        {
            if (index >= mNames.size())
            {
                Ogre::String desc = "ParamsPanel \"" + getName() + "\" has no parameter at position " +
                    Ogre::StrConv::conv(index) + ".";
                N_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, desc, "ParamsPanel::setParamValue");
            }

            mValues[index] = DISPLAY_STRING_TO_STRING(paramValue);
            updateText();
        }

        Ogre::ViewString getParamValue(const Ogre::ViewString& paramName)
        {
            for (unsigned int i = 0; i < mNames.size(); i++)
            {
                if (mNames[i] == DISPLAY_STRING_TO_STRING(paramName)) return mValues[i];
            }

            Ogre::String desc = "ParamsPanel \"" + getName() + "\" has no parameter \"" + DISPLAY_STRING_TO_STRING(paramName) + "\".";
            N_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, desc, "ParamsPanel::getParamValue");
            return "";
        }

        Ogre::ViewString getParamValue(unsigned int index)
        {
            if (index >= mNames.size())
            {
                Ogre::String desc = "ParamsPanel \"" + getName() + "\" has no parameter at position " +
                    Ogre::StrConv::conv(index) + ".";
                N_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, desc, "ParamsPanel::getParamValue");
            }

            return mValues[index];
        }

        const Ogre::StringList& getAllParamValues()
        {
            return mValues;
        }

    protected:

        /*-----------------------------------------------------------------------------
        | Internal method - updates text areas based on name and value lists.
        -----------------------------------------------------------------------------*/
        void updateText()
        {
            Ogre::ViewString namesDS;
            Ogre::ViewString valuesDS;

            for (unsigned int i = 0; i < mNames.size(); i++)
            {
                namesDS.append(mNames[i] + ":\n");
                valuesDS.append(mValues[i] + "\n");
            }

            mNamesArea->setCaption(namesDS);
            mValuesArea->setCaption(valuesDS);
        }

        Ogre::TextOverlayGeo* mNamesArea;
        Ogre::TextOverlayGeo* mValuesArea;
        Ogre::StringList mNames;
        Ogre::StringList mValues;
    };

    /*=============================================================================
    | Basic check box widget.
    =============================================================================*/
    class CheckBox : public Widget
    {
    public:

        // Do not instantiate any widgets directly. Use SdkTrayManager.
        CheckBox(const Ogre::String& name, const Ogre::ViewString& caption, Ogre::NIIf width)
        {
            mCursorOver = false;
            mFitToContents = width <= 0;
            mElement = Ogre::OverlayManager::getSingleton().createGeo
                ("SdkTrays/CheckBox", "BorderPanel", name);
            Ogre::OverlayView* c = (Ogre::OverlayView*)mElement;
            mTextArea = (Ogre::TextOverlayGeo*)c->get(getName() + "/CheckBoxCaption");
            mSquare = (Ogre::FrameOverlayGeo*)c->get(getName() + "/CheckBoxSquare");
            mX = mSquare->get(mSquare->getName() + "/CheckBoxX");
            mX->hide();
            mElement->setWidth(width);
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            mTextArea->setGlyphHeight(mTextArea->getGlyphHeight() - 3);
#endif
            setCaption(caption);
        }

        const Ogre::ViewString& getCaption()
        {
            return mTextArea->getCaption();
        }

        void setCaption(const Ogre::ViewString& caption)
        {
            mTextArea->setCaption(caption);
            if (mFitToContents) mElement->setWidth(getCaptionWidth(caption, mTextArea) + mSquare->getWidth() + 23);
        }

        bool isChecked()
        {
            return mX->isVisible();
        }

        void setChecked(bool checked, bool notifyListener = true)
        {
            if (checked) mX->show();
            else mX->hide();
            if (mListener && notifyListener) mListener->checkBoxToggled(this);
        }

        void toggle(bool notifyListener = true)
        {
            setChecked(!isChecked(), notifyListener);
        }

        void _cursorPressed(const Ogre::Vector2& cursorPos)
        {
            if (mCursorOver && mListener) toggle();
        }

        void _cursorMoved(const Ogre::Vector2& cursorPos)
        {
            if (isCursorOver(mSquare, cursorPos, 5))
            {
                if (!mCursorOver)
                {
                    mCursorOver = true;
                    mSquare->setMaterialName("SdkTrays/MiniTextBox/Over");
                    mSquare->setMaterial("SdkTrays/MiniTextBox/Over");
                }
            }
            else
            {
                if (mCursorOver)
                {
                    mCursorOver = false;
                    mSquare->setMaterialName("SdkTrays/MiniTextBox");
                    mSquare->setMaterial("SdkTrays/MiniTextBox");
                }
            }
        }

        void _focusLost()
        {
            mSquare->setMaterialName("SdkTrays/MiniTextBox");
            mSquare->setMaterial("SdkTrays/MiniTextBox");
            mCursorOver = false;
        }

    protected:
        Ogre::TextOverlayGeo * mTextArea;
        Ogre::FrameOverlayGeo * mSquare;
        Ogre::OverlayGeo * mX;
        bool mFitToContents;
        bool mCursorOver;
    };

    /*=============================================================================
    | Custom, decorative widget created from a template.
    =============================================================================*/
    class DecorWidget : public Widget
    {
    public:
        // Do not instantiate any widgets directly. Use SdkTrayManager.
        DecorWidget(const Ogre::String& name, const Ogre::String& templateName)
        {
            mElement = Ogre::OverlayManager::getSingleton().createGeo(templateName, "", name);
        }
    };

    /*=============================================================================
    | Basic progress bar widget.
    =============================================================================*/
    class ProgressBar : public Widget
    {
    public:

        // Do not instantiate any widgets directly. Use SdkTrayManager.
        ProgressBar(const Ogre::String & name, const Ogre::ViewString & caption, Ogre::NIIf width, Ogre::NIIf commentBoxWidth)
            : mProgress(0.0f)
        {
            mElement = Ogre::OverlayManager::getSingleton().createGeo
                ("SdkTrays/ProgressBar", "BorderPanel", name);
            mElement->setWidth(width);
            Ogre::OverlayView* c = (Ogre::OverlayView*)mElement;
            mTextArea = (Ogre::TextOverlayGeo*)c->get(getName() + "/ProgressCaption");
            Ogre::OverlayView* commentBox = (Ogre::OverlayView*)c->get(getName() + "/ProgressCommentBox");
            commentBox->setWidth(commentBoxWidth);
            commentBox->setLeft(-(commentBoxWidth + 5));
            mCommentTextArea = (Ogre::TextOverlayGeo*)commentBox->get(commentBox->getName() + "/ProgressCommentText");
            mMeter = c->get(getName() + "/ProgressMeter");
            mMeter->setWidth(width - 10);
            mFill = ((Ogre::OverlayView*)mMeter)->get(mMeter->getName() + "/ProgressFill");
            setCaption(caption);
        }

        /*-----------------------------------------------------------------------------
        | Sets the progress as a percentage.
        -----------------------------------------------------------------------------*/
        void setProgress(Ogre::NIIf progress)
        {
            mProgress = Ogre::Clamp<Ogre::NIIf>(progress, 0, 1);
            mFill->setWidth(std::max<int>((int)mFill->getHeight(), (int)(mProgress * (mMeter->getWidth() - 2 * mFill->getLeft()))));
        }

        /*-----------------------------------------------------------------------------
        | Gets the progress as a percentage.
        -----------------------------------------------------------------------------*/
        Ogre::NIIf getProgress()
        {
            return mProgress;
        }

        const Ogre::ViewString& getCaption()
        {
            return mTextArea->getCaption();
        }

        void setCaption(const Ogre::ViewString& caption)
        {
            mTextArea->setCaption(caption);
        }

        const Ogre::ViewString& getComment()
        {
            return mCommentTextArea->getCaption();
        }

        void setComment(const Ogre::ViewString& comment)
        {
            mCommentTextArea->setCaption(comment);
        }


    protected:

        Ogre::TextOverlayGeo* mTextArea;
        Ogre::TextOverlayGeo* mCommentTextArea;
        Ogre::OverlayGeo* mMeter;
        Ogre::OverlayGeo* mFill;
        Ogre::NIIf mProgress;
    };

    /*=============================================================================
    | Main class to manage a cursor, backdrop, trays and widgets.
    =============================================================================*/
    class SdkTrayManager : public SdkTrayListener, public Ogre::ResourceGroupListener
    {
    public:

        /*-----------------------------------------------------------------------------
        | Creates backdrop, cursor, and trays.
        -----------------------------------------------------------------------------*/
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
        SdkTrayManager(const Ogre::String& name, Ogre::ViewWindow* window, OIS::MultiTouch* mouse, SdkTrayListener* listener = 0) :
#else
        SdkTrayManager(const Ogre::String& name, Ogre::ViewWindow* window, OIS::Mouse* mouse, SdkTrayListener* listener = 0) :
#endif
          mName(name), mWindow(window), mMouse(mouse), mWidgetDeathRow(), mListener(listener), mWidgetPadding(8),
                mWidgetSpacing(2), mTrayPadding(0), mTrayDrag(false), mExpandedMenu(0), mDialog(0), mOk(0), mYes(0),
                mNo(0), mCursorWasVisible(false), mFpsLabel(0), mStatsPanel(0), mLogo(0), mLoadBar(0),
                mGroupInitProportion(0.0f), mGroupLoadProportion(0.0f), mLoadInc(0.0f)
        {
            mTimer = Ogre::Root::getSingleton().getTimer();
            mLastStatUpdateTime = 0;

            Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();

            Ogre::String nameBase = mName + "/";
            std::replace(nameBase.begin(), nameBase.end(), ' ', '_');

            // create overlay layers for everything
            mBackdropLayer = om.create(nameBase + "BackdropLayer");
            mTraysLayer = om.create(nameBase + "WidgetsLayer");
            mPriorityLayer = om.create(nameBase + "PriorityLayer");
            mCursorLayer = om.create(nameBase + "CursorLayer");
            mBackdropLayer->setZOrder(100);
            mTraysLayer->setZOrder(200);
            mPriorityLayer->setZOrder(300);
            mCursorLayer->setZOrder(400);

            // make backdrop and cursor overlay containers
            mCursor = (Ogre::OverlayView *)om.createGeo("SdkTrays/Cursor", "Panel", nameBase + "Cursor");
            mCursorLayer->add(mCursor);
            mBackdrop = (Ogre::OverlayView *)om.createGeo("Panel", nameBase + "Backdrop");
            mBackdropLayer->add(mBackdrop);
            mDialogShade = (Ogre::OverlayView *)om.createGeo("Panel", nameBase + "DialogShade");
            mDialogShade->setMaterialName("SdkTrays/Shade");
            mDialogShade->hide();
            mPriorityLayer->add(mDialogShade);

            Ogre::String trayNames[] =
            { "TopLeft", "Top", "TopRight", "Left", "Center", "Right", "BottomLeft", "Bottom", "BottomRight" };

            for (unsigned int i = 0; i < 9; i++)    // make the real trays
            {
                mTrays[i] = (Ogre::OverlayView *)om.createGeo
                    ("SdkTrays/Tray", "BorderPanel", nameBase + trayNames[i] + "Tray");
                mTraysLayer->add(mTrays[i]);

                mTrayWidgetAlign[i] = Ogre::HA_CENTRE;

                // align trays based on location
                if (i == TL_TOP || i == TL_CENTER || i == TL_BOTTOM) mTrays[i]->setXAlign(Ogre::HA_CENTRE);
                if (i == TL_LEFT || i == TL_CENTER || i == TL_RIGHT) mTrays[i]->setYAlign(Ogre::VA_CENTRE);
                if (i == TL_TOPRIGHT || i == TL_RIGHT || i == TL_BOTTOMRIGHT) mTrays[i]->setXAlign(Ogre::HA_RIGHT);
                if (i == TL_BOTTOMLEFT || i == TL_BOTTOM || i == TL_BOTTOMRIGHT) mTrays[i]->setYAlign(Ogre::VA_BOTTOM);
            }

            // create the null tray for free-floating widgets
            mTrays[9] = (Ogre::OverlayView*)om.createGeo("Panel", nameBase + "NullTray");
            mTrayWidgetAlign[9] = Ogre::HA_LEFT;
            mTraysLayer->add(mTrays[9]);
            adjustTrays();

            showTrays();
            showCursor();
        }

        /*-----------------------------------------------------------------------------
        | Destroys background, cursor, widgets, and trays.
        -----------------------------------------------------------------------------*/
        virtual ~SdkTrayManager()
        {
            Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();

            destroyAllWidgets();

            for (unsigned int i = 0; i < mWidgetDeathRow.size(); i++)   // delete widgets queued for destruction
            {
                delete mWidgetDeathRow[i];
            }
            mWidgetDeathRow.clear();

            om.destroy(mBackdropLayer);
            om.destroy(mTraysLayer);
            om.destroy(mPriorityLayer);
            om.destroy(mCursorLayer);

            closeDialog();
            hideLoadingBar();

            Widget::nukeOverlayGeo(mBackdrop);
            Widget::nukeOverlayGeo(mCursor);
            Widget::nukeOverlayGeo(mDialogShade);

            for (unsigned int i = 0; i < 10; i++)
            {
                Widget::nukeOverlayGeo(mTrays[i]);
            }
        }

        /*-----------------------------------------------------------------------------
        | Converts a 2D screen coordinate (in pixels) to a 3D ray into the scene.
        -----------------------------------------------------------------------------*/
        static Ogre::Ray screenToScene(Ogre::Camera * cam, const Ogre::Vector2 & pt)
        {
            return cam->getRay(pt.x, pt.y);
        }

        /*-----------------------------------------------------------------------------
        | Converts a 3D scene position to a 2D screen position (in relative screen size, 0.0-1.0).
        -----------------------------------------------------------------------------*/
        static Ogre::Vector2 sceneToScreen(Ogre::Camera * cam, const Ogre::Vector3 & pt)
        {
            Ogre::Vector3 result = cam->getProjMatrix() * cam->getViewMatrix() * pt;
            return Ogre::Vector2((result.x + 1) / 2, (-result.y + 1) / 2);
        }

        // these methods get the underlying overlays and overlay elements

        Ogre::OverlayView* getTrayContainer(TrayLocation trayLoc) { return mTrays[trayLoc]; }
        Ogre::Overlay* getBackdropLayer() { return mBackdropLayer; }
        Ogre::Overlay* getTraysLayer() { return mTraysLayer; }
        Ogre::Overlay* getCursorLayer() { return mCursorLayer; }
        Ogre::OverlayView* getBackdropContainer() { return mBackdrop; }
        Ogre::OverlayView* getCursorContainer() { return mCursor; }
        Ogre::OverlayGeo* getCursorImage() { return mCursor->get(mCursor->getName() + "/CursorImage"); }

        void set(SdkTrayListener* listener)
        {
            mListener = listener;
        }

        SdkTrayListener* getListener()
        {
            return mListener;
        }

        void showAll()
        {
            showBackdrop();
            showTrays();
            showCursor();
        }

        void hideAll()
        {
            hideBackdrop();
            hideTrays();
            hideCursor();
        }

        /*-----------------------------------------------------------------------------
        | Displays specified material on backdrop, or the last material used if
        | none specified. Good for pause menus like in the browser.
        -----------------------------------------------------------------------------*/
        void showBackdrop(const Ogre::String& materialName = Ogre::StringUtil::BLANK)
        {
            if (materialName != Ogre::StringUtil::BLANK) mBackdrop->setMaterialName(materialName);
            mBackdropLayer->show();
        }

        void hideBackdrop()
        {
            mBackdropLayer->hide();
        }

        /*-----------------------------------------------------------------------------
        | Displays specified material on cursor, or the last material used if
        | none specified. Used to change cursor type.
        -----------------------------------------------------------------------------*/
        void showCursor(const Ogre::String& materialName = Ogre::StringUtil::BLANK)
        {
            if (materialName != Ogre::StringUtil::BLANK) getCursorImage()->setMaterialName(materialName);

            if (!mCursorLayer->isVisible())
            {
                mCursorLayer->show();
                refreshCursor();
            }
        }

        void hideCursor()
        {
            mCursorLayer->hide();

            // give widgets a chance to reset in case they're in the middle of something
            for (unsigned int i = 0; i < 10; i++)
            {
                for (unsigned int j = 0; j < mWidgets[i].size(); j++)
                {
                    mWidgets[i][j]->_focusLost();
                }
            }

            setExpandedMenu(0);
        }

        /*-----------------------------------------------------------------------------
        | Updates cursor position based on unbuffered mouse state. This is necessary
        | because if the tray manager has been cut off from mouse events for a time,
        | the cursor position will be out of date.
        -----------------------------------------------------------------------------*/
        void refreshCursor()
        {
#if (NII_VIEWPORT_DIR_NONE == 0) || (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS)
            // TODO:
            // the position should be based on the orientation, for now simply return
            return;
#endif
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            std::vector<OIS::MultiTouchState> states = mMouse->getMultiTouchStates();
            if(states.size() > 0)
                mCursor->setPos(states[0].X.abs, states[0].Y.abs);
#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
            // TODO: handle cursor positioning
#else
            mCursor->setPos(mMouse->getMouseState().X.abs, mMouse->getMouseState().Y.abs);
#endif
        }

        void showTrays()
        {
            mTraysLayer->show();
            mPriorityLayer->show();
        }

        void hideTrays()
        {
            mTraysLayer->hide();
            mPriorityLayer->hide();

            // give widgets a chance to reset in case they're in the middle of something
            for (unsigned int i = 0; i < 10; i++)
            {
                for (unsigned int j = 0; j < mWidgets[i].size(); j++)
                {
                    mWidgets[i][j]->_focusLost();
                }
            }

            setExpandedMenu(0);
        }

        bool isCursorVisible() { return mCursorLayer->isVisible(); }
        bool isBackdropVisible() { return mBackdropLayer->isVisible(); }
        bool areTraysVisible() { return mTraysLayer->isVisible(); }

        /*-----------------------------------------------------------------------------
        | Sets horizontal alignment of a tray's contents.
        -----------------------------------------------------------------------------*/
        void setTrayWidgetAlignment(TrayLocation trayLoc, Ogre::HAlign gha)
        {
            mTrayWidgetAlign[trayLoc] = gha;

            for (unsigned int i = 0; i < mWidgets[trayLoc].size(); i++)
            {
                mWidgets[trayLoc][i]->getOverlayGeo()->setXAlign(gha);
            }
        }

        // padding and spacing methods

        void setWidgetPadding(Ogre::NIIf padding)
        {
            mWidgetPadding = std::max<int>((int)padding, 0);
            adjustTrays();
        }

        void setWidgetSpacing(Ogre::NIIf spacing)
        {
            mWidgetSpacing = std::max<int>((int)spacing, 0);
            adjustTrays();
        }
        void setTrayPadding(Ogre::NIIf padding)
        {
            mTrayPadding = std::max<int>((int)padding, 0);
            adjustTrays();
        }

        virtual Ogre::NIIf getWidgetPadding() const { return mWidgetPadding; }
        virtual Ogre::NIIf getWidgetSpacing() const { return mWidgetSpacing; }
        virtual Ogre::NIIf getTrayPadding() const { return mTrayPadding; }
        /*-----------------------------------------------------------------------------
        | Fits trays to their contents and snaps them to their anchor locations.
        -----------------------------------------------------------------------------*/
        virtual void adjustTrays()
        {
            for(unsigned int i = 0; i < 9; i++)   // resizes and hides trays if necessary
            {
                Ogre::NIIf trayWidth = 0;
                Ogre::NIIf trayHeight = mWidgetPadding;
                std::vector<Ogre::OverlayGeo *> labelsAndSeps;

                if(mWidgets[i].empty())   // hide tray if empty
                {
                    mTrays[i]->hide();
                    continue;
                }
                else 
                    mTrays[i]->show();

                // arrange widgets and calculate final tray size and position
                for(unsigned int j = 0; j < mWidgets[i].size(); j++)
                {
                    Ogre::OverlayGeo * e = mWidgets[i][j]->getOverlayGeo();

                    if(j != 0) trayHeight += mWidgetSpacing;   // don't space first widget

                    e->setYAlign(Ogre::VA_TOP);
                    e->setTop(trayHeight);

                    switch(e->getXAlign())
                    {
                    case Ogre::HA_LEFT:
                        e->setLeft(mWidgetPadding);
                        break;
                    case Ogre::HA_RIGHT:
                        e->setLeft(-(e->getWidth() + mWidgetPadding));
                        break;
                    default:
                        e->setLeft(-(e->getWidth() / 2));
                    }

                    // prevents some weird texture filtering problems (just some)
                    e->setPos((int)e->getLeft(), (int)e->getTop());
                    e->setSize((int)e->getWidth(), (int)e->getHeight());

                    trayHeight += e->getHeight();

                    Label* l = dynamic_cast<Label*>(mWidgets[i][j]);
                    if (l && l->_isFitToTray())
                    {
                        labelsAndSeps.push_back(e);
                        continue;
                    }
                    Separator* s = dynamic_cast<Separator*>(mWidgets[i][j]);
                    if (s && s->_isFitToTray())
                    {
                        labelsAndSeps.push_back(e);
                        continue;
                    }

                    if (e->getWidth() > trayWidth) trayWidth = e->getWidth();
                }

                // add paddings and resize trays
                mTrays[i]->setWidth(trayWidth + 2 * mWidgetPadding);
                mTrays[i]->setHeight(trayHeight + mWidgetPadding);

                for (unsigned int j = 0; j < labelsAndSeps.size(); j++)
                {
                    labelsAndSeps[j]->setWidth((int)trayWidth);
                    labelsAndSeps[j]->setLeft(-(int)(trayWidth / 2));
                }
            }

            for (unsigned int i = 0; i < 9; i++)    // snap trays to anchors
            {
                if (i == TL_TOPLEFT || i == TL_LEFT || i == TL_BOTTOMLEFT)
                    mTrays[i]->setLeft(mTrayPadding);
                if (i == TL_TOP || i == TL_CENTER || i == TL_BOTTOM)
                    mTrays[i]->setLeft(-mTrays[i]->getWidth() / 2);
                if (i == TL_TOPRIGHT || i == TL_RIGHT || i == TL_BOTTOMRIGHT)
                    mTrays[i]->setLeft(-(mTrays[i]->getWidth() + mTrayPadding));

                if (i == TL_TOPLEFT || i == TL_TOP || i == TL_TOPRIGHT)
                    mTrays[i]->setTop(mTrayPadding);
                if (i == TL_LEFT || i == TL_CENTER || i == TL_RIGHT)
                    mTrays[i]->setTop(-mTrays[i]->getHeight() / 2);
                if (i == TL_BOTTOMLEFT || i == TL_BOTTOM || i == TL_BOTTOMRIGHT)
                    mTrays[i]->setTop(-mTrays[i]->getHeight() - mTrayPadding);

                // prevents some weird texture filtering problems (just some)
                mTrays[i]->setPos((int)mTrays[i]->getLeft(), (int)mTrays[i]->getTop());
                mTrays[i]->setSize((int)mTrays[i]->getWidth(), (int)mTrays[i]->getHeight());
            }
        }

        /*-----------------------------------------------------------------------------
        | Returns a 3D ray into the scene that is directly underneath the cursor.
        -----------------------------------------------------------------------------*/
        Ogre::Ray getCursorRay(Ogre::Camera* cam)
        {
            return screenToScene(cam, Ogre::Vector2(mCursor->getUnitLeft(), mCursor->getUnitTop()));
        }

        Button* createButton(TrayLocation trayLoc, const Ogre::String& name, const Ogre::String& caption, Ogre::NIIf width = 0)
        {
            Button* b = new Button(name, caption, width);
            moveWidgetToTray(b, trayLoc);
            b->_assignListener(mListener);
            return b;
        }

        TextBox* createTextBox(TrayLocation trayLoc, const Ogre::String& name, const Ogre::ViewString& caption,
            Ogre::NIIf width, Ogre::NIIf height)
        {
            TextBox* tb = new TextBox(name, caption, width, height);
            moveWidgetToTray(tb, trayLoc);
            tb->_assignListener(mListener);
            return tb;
        }

        SelectMenu* createThickSelectMenu(TrayLocation trayLoc, const Ogre::String& name, const Ogre::ViewString& caption,
            Ogre::NIIf width, unsigned int maxItemsShown, const Ogre::StringList& items = Ogre::StringList())
        {
            SelectMenu* sm = new SelectMenu(name, caption, width, 0, maxItemsShown);
            moveWidgetToTray(sm, trayLoc);
            sm->_assignListener(mListener);
            if (!items.empty()) sm->setItems(items);
            return sm;
        }

        SelectMenu* createLongSelectMenu(TrayLocation trayLoc, const Ogre::String& name, const Ogre::ViewString& caption,
            Ogre::NIIf width, Ogre::NIIf boxWidth, unsigned int maxItemsShown, const Ogre::StringList& items = Ogre::StringList())
        {
            SelectMenu* sm = new SelectMenu(name, caption, width, boxWidth, maxItemsShown);
            moveWidgetToTray(sm, trayLoc);
            sm->_assignListener(mListener);
            if (!items.empty()) sm->setItems(items);
            return sm;
        }

        SelectMenu* createLongSelectMenu(TrayLocation trayLoc, const Ogre::String& name, const Ogre::ViewString& caption,
            Ogre::NIIf boxWidth, unsigned int maxItemsShown, const Ogre::StringList& items = Ogre::StringList())
        {
            return createLongSelectMenu(trayLoc, name, caption, 0, boxWidth, maxItemsShown, items);
        }

        Label* createLabel(TrayLocation trayLoc, const Ogre::String& name, const Ogre::ViewString& caption, Ogre::NIIf width = 0)
        {
            Label* l = new Label(name, caption, width);
            moveWidgetToTray(l, trayLoc);
            l->_assignListener(mListener);
            return l;
        }

        Separator* createSeparator(TrayLocation trayLoc, const Ogre::String& name, Ogre::NIIf width = 0)
        {
            Separator* s = new Separator(name, width);
            moveWidgetToTray(s, trayLoc);
            return s;
        }

        Slider* createThickSlider(TrayLocation trayLoc, const Ogre::String& name, const Ogre::ViewString& caption,
            Ogre::NIIf width, Ogre::NIIf valueBoxWidth, Ogre::NIIf minValue, Ogre::NIIf maxValue, unsigned int snaps)
        {
            Slider* s = new Slider(name, caption, width, 0, valueBoxWidth, minValue, maxValue, snaps);
            moveWidgetToTray(s, trayLoc);
            s->_assignListener(mListener);
            return s;
        }

        Slider* createLongSlider(TrayLocation trayLoc, const Ogre::String& name, const Ogre::ViewString& caption, Ogre::NIIf width,
            Ogre::NIIf trackWidth, Ogre::NIIf valueBoxWidth, Ogre::NIIf minValue, Ogre::NIIf maxValue, unsigned int snaps)
        {
            if (trackWidth <= 0) trackWidth = 1;
            Slider* s = new Slider(name, caption, width, trackWidth, valueBoxWidth, minValue, maxValue, snaps);
            moveWidgetToTray(s, trayLoc);
            s->_assignListener(mListener);
            return s;
        }

        Slider* createLongSlider(TrayLocation trayLoc, const Ogre::String& name, const Ogre::ViewString& caption,
            Ogre::NIIf trackWidth, Ogre::NIIf valueBoxWidth, Ogre::NIIf minValue, Ogre::NIIf maxValue, unsigned int snaps)
        {
            return createLongSlider(trayLoc, name, caption, 0, trackWidth, valueBoxWidth, minValue, maxValue, snaps);
        }

        ParamsPanel* createParamsPanel(TrayLocation trayLoc, const Ogre::String& name, Ogre::NIIf width, unsigned int lines)
        {
            ParamsPanel* pp = new ParamsPanel(name, width, lines);
            moveWidgetToTray(pp, trayLoc);
            return pp;
        }

        ParamsPanel* createParamsPanel(TrayLocation trayLoc, const Ogre::String& name, Ogre::NIIf width,
            const Ogre::StringList& paramNames)
        {
            ParamsPanel* pp = new ParamsPanel(name, width, paramNames.size());
            pp->setAllParamNames(paramNames);
            moveWidgetToTray(pp, trayLoc);
            return pp;
        }

        CheckBox* createCheckBox(TrayLocation trayLoc, const Ogre::String& name, const Ogre::ViewString& caption,
            Ogre::NIIf width = 0)
        {
            CheckBox* cb = new CheckBox(name, caption, width);
            moveWidgetToTray(cb, trayLoc);
            cb->_assignListener(mListener);
            return cb;
        }

        DecorWidget* createDecorWidget(TrayLocation trayLoc, const Ogre::String& name, const Ogre::String& templateName)
        {
            DecorWidget* dw = new DecorWidget(name, templateName);
            moveWidgetToTray(dw, trayLoc);
            return dw;
        }

        ProgressBar* createProgressBar(TrayLocation trayLoc, const Ogre::String& name, const Ogre::ViewString& caption,
            Ogre::NIIf width, Ogre::NIIf commentBoxWidth)
        {
            ProgressBar* pb = new ProgressBar(name, caption, width, commentBoxWidth);
            moveWidgetToTray(pb, trayLoc);
            return pb;
        }

        /*-----------------------------------------------------------------------------
        | Shows frame statistics widget set in the specified location.
        -----------------------------------------------------------------------------*/
        void showFrameStats(TrayLocation trayLoc, int place = -1)
        {
            if (!areFrameStatsVisible())
            {
                Ogre::StringList stats;
                stats.push_back("Average FPS");
                stats.push_back("Best FPS");
                stats.push_back("Worst FPS");
                stats.push_back("Triangles");
                stats.push_back("Batches");

                mFpsLabel = createLabel(TL_NONE, mName + "/FpsLabel", "FPS:", 180);
                mFpsLabel->_assignListener(this);
                mStatsPanel = createParamsPanel(TL_NONE, mName + "/StatsPanel", 180, stats);
            }

            moveWidgetToTray(mFpsLabel, trayLoc, place);
            moveWidgetToTray(mStatsPanel, trayLoc, locateWidgetInTray(mFpsLabel) + 1);
        }

        /*-----------------------------------------------------------------------------
        | Hides frame statistics widget set.
        -----------------------------------------------------------------------------*/
        void hideFrameStats()
        {
            if (areFrameStatsVisible())
            {
                destroyWidget(mFpsLabel);
                destroyWidget(mStatsPanel);
                mFpsLabel = 0;
                mStatsPanel = 0;
            }
        }

        bool areFrameStatsVisible()
        {
            return mFpsLabel != 0;
        }

        /*-----------------------------------------------------------------------------
        | Toggles visibility of advanced statistics.
        -----------------------------------------------------------------------------*/
        void toggleAdvancedFrameStats()
        {
            if (mFpsLabel) labelHit(mFpsLabel);
        }

        /*-----------------------------------------------------------------------------
        | Shows logo in the specified location.
        -----------------------------------------------------------------------------*/
        void showLogo(TrayLocation trayLoc, int place = -1)
        {
            if (!isLogoVisible()) mLogo = createDecorWidget(TL_NONE, mName + "/Logo", "SdkTrays/Logo");
            moveWidgetToTray(mLogo, trayLoc, place);
        }

        void hideLogo()
        {
            if (isLogoVisible())
            {
                destroyWidget(mLogo);
                mLogo = 0;
            }
        }

        bool isLogoVisible()
        {
            return mLogo != 0;
        }

        /*-----------------------------------------------------------------------------
        | Shows loading bar. Also takes job settings: the number of resource groups
        | to be initialised, the number of resource groups to be loaded, and the
        | proportion of the job that will be taken up by initialisation. Usually,
        | script parsing takes up most time, so the default value is 0.7.
        -----------------------------------------------------------------------------*/
        void showLoadingBar(unsigned int numGroupsInit = 1, unsigned int numGroupsLoad = 1,
            Ogre::NIIf initProportion = 0.7)
        {
            if (mDialog) closeDialog();
            if (mLoadBar) hideLoadingBar();

            mLoadBar = new ProgressBar(mName + "/LoadingBar", "Loading...", 400, 308);
            Ogre::OverlayGeo* e = mLoadBar->getOverlayGeo();
            mDialogShade->add(e);
            e->setYAlign(Ogre::VA_CENTRE);
            e->setLeft(-(e->getWidth() / 2));
            e->setTop(-(e->getHeight() / 2));

            Ogre::ResourceSchemeManager::getSingleton().addResourceGroupListener(this);
            mCursorWasVisible = isCursorVisible();
            hideCursor();
            mDialogShade->show();

            // calculate the proportion of job required to init/load one group

            if (numGroupsInit == 0 && numGroupsLoad != 0)
            {
                mGroupInitProportion = 0;
                mGroupLoadProportion = 1;
            }
            else if (numGroupsLoad == 0 && numGroupsInit != 0)
            {
                mGroupLoadProportion = 0;
                if (numGroupsInit != 0) mGroupInitProportion = 1;
            }
            else if (numGroupsInit == 0 && numGroupsLoad == 0)
            {
                mGroupInitProportion = 0;
                mGroupLoadProportion = 0;
            }
            else
            {
                mGroupInitProportion = initProportion / numGroupsInit;
                mGroupLoadProportion = (1 - initProportion) / numGroupsLoad;
            }
        }

        void hideLoadingBar()
        {
            if (mLoadBar)
            {
                mLoadBar->cleanup();
                delete mLoadBar;
                mLoadBar = 0;

                Ogre::ResourceSchemeManager::getSingleton().removeResourceGroupListener(this);
                if (mCursorWasVisible) showCursor();
                mDialogShade->hide();
            }
        }

        bool isLoadingBarVisible()
        {
            return mLoadBar != 0;
        }

        /*-----------------------------------------------------------------------------
        | Pops up a message dialog with an OK button.
        -----------------------------------------------------------------------------*/
        void showOkDialog(const Ogre::ViewString& caption, const Ogre::ViewString& message)
        {
            if (mLoadBar) hideLoadingBar();

            Ogre::OverlayGeo* e;

            if (mDialog)
            {
                mDialog->setCaption(caption);
                mDialog->setText(message);

                if (mOk) return;
                else
                {
                    mYes->cleanup();
                    mNo->cleanup();
                    delete mYes;
                    delete mNo;
                    mYes = 0;
                    mNo = 0;
                }
            }
            else
            {
                // give widgets a chance to reset in case they're in the middle of something
                for (unsigned int i = 0; i < 10; i++)
                {
                    for (unsigned int j = 0; j < mWidgets[i].size(); j++)
                    {
                        mWidgets[i][j]->_focusLost();
                    }
                }

                mDialogShade->show();

                mDialog = new TextBox(mName + "/DialogBox", caption, 300, 208);
                mDialog->setText(message);
                e = mDialog->getOverlayGeo();
                mDialogShade->add(e);
                e->setYAlign(Ogre::VA_CENTRE);
                e->setLeft(-(e->getWidth() / 2));
                e->setTop(-(e->getHeight() / 2));

                mCursorWasVisible = isCursorVisible();
                showCursor();
            }

            mOk = new Button(mName + "/OkButton", "OK", 60);
            mOk->_assignListener(this);
            e = mOk->getOverlayGeo();
            mDialogShade->add(e);
            e->setYAlign(Ogre::VA_CENTRE);
            e->setLeft(-(e->getWidth() / 2));
            e->setTop(mDialog->getOverlayGeo()->getTop() + mDialog->getOverlayGeo()->getHeight() + 5);
        }

        /*-----------------------------------------------------------------------------
        | Pops up a question dialog with Yes and No buttons.
        -----------------------------------------------------------------------------*/
        void showYesNoDialog(const Ogre::ViewString& caption, const Ogre::ViewString& question)
        {
            if (mLoadBar) hideLoadingBar();

            Ogre::OverlayGeo* e;

            if (mDialog)
            {
                mDialog->setCaption(caption);
                mDialog->setText(question);

                if (mOk)
                {
                    mOk->cleanup();
                    delete mOk;
                    mOk = 0;
                }
                else return;
            }
            else
            {
                // give widgets a chance to reset in case they're in the middle of something
                for (unsigned int i = 0; i < 10; i++)
                {
                    for (unsigned int j = 0; j < mWidgets[i].size(); j++)
                    {
                        mWidgets[i][j]->_focusLost();
                    }
                }

                mDialogShade->show();

                mDialog = new TextBox(mName + "/DialogBox", caption, 300, 208);
                mDialog->setText(question);
                e = mDialog->getOverlayGeo();
                mDialogShade->add(e);
                e->setYAlign(Ogre::VA_CENTRE);
                e->setLeft(-(e->getWidth() / 2));
                e->setTop(-(e->getHeight() / 2));

                mCursorWasVisible = isCursorVisible();
                showCursor();
            }

            mYes = new Button(mName + "/YesButton", "Yes", 58);
            mYes->_assignListener(this);
            e = mYes->getOverlayGeo();
            mDialogShade->add(e);
            e->setYAlign(Ogre::VA_CENTRE);
            e->setLeft(-(e->getWidth() + 2));
            e->setTop(mDialog->getOverlayGeo()->getTop() + mDialog->getOverlayGeo()->getHeight() + 5);

            mNo = new Button(mName + "/NoButton", "No", 50);
            mNo->_assignListener(this);
            e = mNo->getOverlayGeo();
            mDialogShade->add(e);
            e->setYAlign(Ogre::VA_CENTRE);
            e->setLeft(3);
            e->setTop(mDialog->getOverlayGeo()->getTop() + mDialog->getOverlayGeo()->getHeight() + 5);
        }

        /*-----------------------------------------------------------------------------
        | Hides whatever dialog is currently showing.
        -----------------------------------------------------------------------------*/
        void closeDialog()
        {
            if (mDialog)
            {
                if (mOk)
                {
                    mOk->cleanup();
                    delete mOk;
                    mOk = 0;
                }
                else
                {
                    mYes->cleanup();
                    mNo->cleanup();
                    delete mYes;
                    delete mNo;
                    mYes = 0;
                    mNo = 0;
                }

                mDialogShade->hide();
                mDialog->cleanup();
                delete mDialog;
                mDialog = 0;

                if (!mCursorWasVisible) hideCursor();
            }
        }

        /*-----------------------------------------------------------------------------
        | Determines if any dialog is currently visible.
        -----------------------------------------------------------------------------*/
        bool isDialogVisible()
        {
            return mDialog != 0;
        }

        /*-----------------------------------------------------------------------------
        | Gets a widget from a tray by place.
        -----------------------------------------------------------------------------*/
        Widget* getWidget(TrayLocation trayLoc, unsigned int place)
        {
            if (place < mWidgets[trayLoc].size()) return mWidgets[trayLoc][place];
            return 0;
        }

        /*-----------------------------------------------------------------------------
        | Gets a widget from a tray by name.
        -----------------------------------------------------------------------------*/
        Widget* getWidget(TrayLocation trayLoc, const Ogre::String& name)
        {
            for (unsigned int i = 0; i < mWidgets[trayLoc].size(); i++)
            {
                if (mWidgets[trayLoc][i]->getName() == name) return mWidgets[trayLoc][i];
            }
            return 0;
        }

        /*-----------------------------------------------------------------------------
        | Gets a widget by name.
        -----------------------------------------------------------------------------*/
        Widget* getWidget(const Ogre::String& name)
        {
            for (unsigned int i = 0; i < 10; i++)
            {
                for (unsigned int j = 0; j < mWidgets[i].size(); j++)
                {
                    if (mWidgets[i][j]->getName() == name) return mWidgets[i][j];
                }
            }
            return 0;
        }

        /*-----------------------------------------------------------------------------
        | Gets the number of widgets in total.
        -----------------------------------------------------------------------------*/
        unsigned int getNumWidgets()
        {
            unsigned int total = 0;

            for (unsigned int i = 0; i < 10; i++)
            {
                total += mWidgets[i].size();
            }

            return total;
        }

        /*-----------------------------------------------------------------------------
        | Gets the number of widgets in a tray.
        -----------------------------------------------------------------------------*/
        unsigned int getNumWidgets(TrayLocation trayLoc)
        {
            return mWidgets[trayLoc].size();
        }

        /*-----------------------------------------------------------------------------
        | Gets all the widgets of a specific tray.
        -----------------------------------------------------------------------------*/
        WidgetIterator getWidgetIterator(TrayLocation trayLoc)
        {
            return WidgetIterator(mWidgets[trayLoc].begin(), mWidgets[trayLoc].end());
        }

        /*-----------------------------------------------------------------------------
        | Gets a widget's position in its tray.
        -----------------------------------------------------------------------------*/
        int locateWidgetInTray(Widget* widget)
        {
            for (unsigned int i = 0; i < mWidgets[widget->getTrayLocation()].size(); i++)
            {
                if (mWidgets[widget->getTrayLocation()][i] == widget) return i;
            }
            return -1;
        }

        /*-----------------------------------------------------------------------------
        | Destroys a widget.
        -----------------------------------------------------------------------------*/
        void destroyWidget(Widget* widget)
        {
            if (!widget) N_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, "Widget does not exist.", "TrayManager::destroyWidget");

            // in case special widgets are destroyed manually, set them to 0
            if (widget == mLogo) mLogo = 0;
            else if (widget == mStatsPanel) mStatsPanel = 0;
            else if (widget == mFpsLabel) mFpsLabel = 0;

            mTrays[widget->getTrayLocation()]->remove(widget->getName());

            WidgetList& wList = mWidgets[widget->getTrayLocation()];
            wList.erase(std::find(wList.begin(), wList.end(), widget));
            if (widget == mExpandedMenu) setExpandedMenu(0);

            widget->cleanup();

            mWidgetDeathRow.push_back(widget);

            adjustTrays();
        }

        void destroyWidget(TrayLocation trayLoc, unsigned int place)
        {
            destroyWidget(getWidget(trayLoc, place));
        }

        void destroyWidget(TrayLocation trayLoc, const Ogre::String& name)
        {
            destroyWidget(getWidget(trayLoc, name));
        }

        void destroyWidget(const Ogre::String& name)
        {
            destroyWidget(getWidget(name));
        }

        /*-----------------------------------------------------------------------------
        | Destroys all widgets in a tray.
        -----------------------------------------------------------------------------*/
        void destroyAllWidgetsInTray(TrayLocation trayLoc)
        {
            while (!mWidgets[trayLoc].empty()) destroyWidget(mWidgets[trayLoc][0]);
        }

        /*-----------------------------------------------------------------------------
        | Destroys all widgets.
        -----------------------------------------------------------------------------*/
        void destroyAllWidgets()
        {
            for (unsigned int i = 0; i < 10; i++)  // destroy every widget in every tray (including null tray)
            {
                destroyAllWidgetsInTray((TrayLocation)i);
            }
        }

        /*-----------------------------------------------------------------------------
        | Adds a widget to a specified tray.
        -----------------------------------------------------------------------------*/
        void moveWidgetToTray(Widget* widget, TrayLocation trayLoc, int place = -1)
        {
            if (!widget) N_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, "Widget does not exist.", "TrayManager::moveWidgetToTray");

            // remove widget from old tray
            WidgetList& wList = mWidgets[widget->getTrayLocation()];
            WidgetList::iterator it = std::find(wList.begin(), wList.end(), widget);
            if (it != wList.end())
            {
                wList.erase(it);
                mTrays[widget->getTrayLocation()]->remove(widget->getName());
            }

            // insert widget into new tray at given position, or at the end if unspecified or invalid
            if (place == -1 || place > (int)mWidgets[trayLoc].size()) place = mWidgets[trayLoc].size();
            mWidgets[trayLoc].insert(mWidgets[trayLoc].begin() + place, widget);
            mTrays[trayLoc]->add(widget->getOverlayGeo());

            widget->getOverlayGeo()->setXAlign(mTrayWidgetAlign[trayLoc]);

            // adjust trays if necessary
            if (widget->getTrayLocation() != TL_NONE || trayLoc != TL_NONE) adjustTrays();

            widget->_assignToTray(trayLoc);
        }

        void moveWidgetToTray(const Ogre::String& name, TrayLocation trayLoc, unsigned int place = -1)
        {
            moveWidgetToTray(getWidget(name), trayLoc, place);
        }

        void moveWidgetToTray(TrayLocation currentTrayLoc, const Ogre::String& name, TrayLocation targetTrayLoc,
            int place = -1)
        {
            moveWidgetToTray(getWidget(currentTrayLoc, name), targetTrayLoc, place);
        }

        void moveWidgetToTray(TrayLocation currentTrayLoc, unsigned int currentPlace, TrayLocation targetTrayLoc,
            int targetPlace = -1)
        {
            moveWidgetToTray(getWidget(currentTrayLoc, currentPlace), targetTrayLoc, targetPlace);
        }

        /*-----------------------------------------------------------------------------
        | Removes a widget from its tray. Same as moving it to the null tray.
        -----------------------------------------------------------------------------*/
        void removeWidgetFromTray(Widget* widget)
        {
            moveWidgetToTray(widget, TL_NONE);
        }

        void removeWidgetFromTray(const Ogre::String& name)
        {
            removeWidgetFromTray(getWidget(name));
        }

        void removeWidgetFromTray(TrayLocation trayLoc, const Ogre::String& name)
        {
            removeWidgetFromTray(getWidget(trayLoc, name));
        }

        void removeWidgetFromTray(TrayLocation trayLoc, int place)
        {
            removeWidgetFromTray(getWidget(trayLoc, place));
        }

        /*-----------------------------------------------------------------------------
        | Removes all widgets from a widget tray.
        -----------------------------------------------------------------------------*/
        void clearTray(TrayLocation trayLoc)
        {
            if (trayLoc == TL_NONE) return;      // can't clear the null tray

            while (!mWidgets[trayLoc].empty())   // remove every widget from given tray
            {
                removeWidgetFromTray(mWidgets[trayLoc][0]);
            }
        }

        /*-----------------------------------------------------------------------------
        | Removes all widgets from all widget trays.
        -----------------------------------------------------------------------------*/
        void clearAllTrays()
        {
            for (unsigned int i = 0; i < 9; i++)
            {
                clearTray((TrayLocation)i);
            }
        }

        /*-----------------------------------------------------------------------------
        | Process frame events. Updates frame statistics widget set and deletes
        | all widgets queued for destruction.
        -----------------------------------------------------------------------------*/
        bool onRenderCore(const Ogre::RenderFrameArgs * evt)
        {
            for(unsigned int i = 0; i < mWidgetDeathRow.size(); i++)
            {
                delete mWidgetDeathRow[i];
            }
            mWidgetDeathRow.clear();

            unsigned long currentTime = mTimer->getMS();
            if(areFrameStatsVisible() && currentTime - mLastStatUpdateTime > 250)
            {
                NII::FramesPerSecond stats = mWindow->getFPS();

                mLastStatUpdateTime = currentTime;

                Ogre::String s("FPS: ");
                s += Ogre::StrConv::conv((int)stats.mLast);

                for(int i = s.length() - 5; i > 5; i -= 3)
                {
                    s.insert(i, 1, ',');
                }
                mFpsLabel->setCaption(s);

                if(mStatsPanel->getOverlayGeo()->isVisible())
                {
                    Ogre::StringList values;
                    std::ostringstream oss;

                    oss.str("");
                    oss << std::fixed << std::setprecision(1) << stats.mAvg;
                    Ogre::String str = oss.str();
                    for (int i = str.length() - 5; i > 0; i -= 3) { str.insert(i, 1, ','); }
                    values.push_back(str);

                    oss.str("");
                    oss << std::fixed << std::setprecision(1) << stats.mBest;
                    str = oss.str();
                    for (int i = str.length() - 5; i > 0; i -= 3) { str.insert(i, 1, ','); }
                    values.push_back(str);

                    oss.str("");
                    oss << std::fixed << std::setprecision(1) << stats.mWorst;
                    str = oss.str();
                    for (int i = str.length() - 5; i > 0; i -= 3) { str.insert(i, 1, ','); }
                    values.push_back(str);

                    str = Ogre::StrConv::conv(mWindow->mTriCount);
                    for (int i = str.length() - 3; i > 0; i -= 3) { str.insert(i, 1, ','); }
                    values.push_back(str);

                    str = Ogre::StrConv::conv(mWindow->mBatchCount);
                    for (int i = str.length() - 3; i > 0; i -= 3) { str.insert(i, 1, ','); }
                    values.push_back(str);

                    mStatsPanel->setAllParamValues(values);
                }
            }

            return true;
        }

        void windowUpdate()
        {
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS && OGRE_PLATFORM != OGRE_PLATFORM_NACL
            mWindow->flush();
#endif
        }

        void resourceGroupScriptingStarted(const Ogre::String& groupName, size_t scriptCount)
        {
            mLoadInc = mGroupInitProportion / scriptCount;
            mLoadBar->setCaption("Parsing...");
            windowUpdate();
        }

        void scriptParseStarted(const Ogre::String& scriptName, bool& skipThisScript)
        {
            mLoadBar->setComment(scriptName);
            windowUpdate();
        }

        void scriptParseEnded(const Ogre::String& scriptName, bool skipped)
        {
            mLoadBar->setProgress(mLoadBar->getProgress() + mLoadInc);
            windowUpdate();
        }

        void resourceGroupScriptingEnded(const Ogre::String& groupName) {}

        void resourceGroupLoadStarted(const Ogre::String& groupName, size_t resourceCount)
        {
            mLoadInc = mGroupLoadProportion / resourceCount;
            mLoadBar->setCaption("Loading...");
            windowUpdate();
        }

        void resourceLoadStarted(const Ogre::ResourcePtr& resource)
        {
            mLoadBar->setComment(resource->getName());
            windowUpdate();
        }

        void resourceLoadEnded()
        {
            mLoadBar->setProgress(mLoadBar->getProgress() + mLoadInc);
            windowUpdate();
        }

        void worldGeometryStageStarted(const Ogre::String& description)
        {
            mLoadBar->setComment(description);
            windowUpdate();
        }

        void worldGeometryStageEnded()
        {
            mLoadBar->setProgress(mLoadBar->getProgress() + mLoadInc);
            windowUpdate();
        }

        void resourceGroupLoadEnded(const Ogre::String& groupName) {}

        /*-----------------------------------------------------------------------------
        | Toggles visibility of advanced statistics.
        -----------------------------------------------------------------------------*/
        void labelHit(Label* label)
        {
            if (mStatsPanel->getOverlayGeo()->isVisible())
            {
                mStatsPanel->getOverlayGeo()->hide();
                mFpsLabel->getOverlayGeo()->setWidth(150);
                removeWidgetFromTray(mStatsPanel);
            }
            else
            {
                mStatsPanel->getOverlayGeo()->show();
                mFpsLabel->getOverlayGeo()->setWidth(180);
                moveWidgetToTray(mStatsPanel, mFpsLabel->getTrayLocation(), locateWidgetInTray(mFpsLabel) + 1);
            }
        }

        /*-----------------------------------------------------------------------------
        | Destroys dialog widgets, notifies listener, and ends high priority session.
        -----------------------------------------------------------------------------*/
        void buttonHit(Button* button)
        {
            if (mListener)
            {
                if (button == mOk) mListener->okDialogClosed(mDialog->getText());
                else mListener->yesNoDialogClosed(mDialog->getText(), button == mYes);
            }
            closeDialog();
        }

        /*-----------------------------------------------------------------------------
        | Processes mouse button down events. Returns true if the event was
        | consumed and should not be passed on to other handlers.
        -----------------------------------------------------------------------------*/
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
        bool injectMouseDown(const OIS::MultiTouchEvent& evt)
#else
        bool injectMouseDown(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
#endif
        {
#if (OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS) && (OGRE_PLATFORM != OGRE_PLATFORM_ANDROID)
            // only process left button when stuff is visible
            if (!mCursorLayer->isVisible() || id != OIS::MB_Left) return false;
#else
            // only process touches when stuff is visible
            if (!mCursorLayer->isVisible()) return false;
#endif
            Ogre::Vector2 cursorPos(mCursor->getLeft(), mCursor->getTop());

            mTrayDrag = false;

            if (mExpandedMenu)   // only check top priority widget until it passes on
            {
                mExpandedMenu->_cursorPressed(cursorPos);
                if (!mExpandedMenu->isExpanded()) setExpandedMenu(0);
                return true;
            }

            if (mDialog)   // only check top priority widget until it passes on
            {
                mDialog->_cursorPressed(cursorPos);
                if (mOk) mOk->_cursorPressed(cursorPos);
                else
                {
                    mYes->_cursorPressed(cursorPos);
                    mNo->_cursorPressed(cursorPos);
                }
                return true;
            }

            for (unsigned int i = 0; i < 9; i++)   // check if mouse is over a non-null tray
            {
                if (mTrays[i]->isVisible() && Widget::isCursorOver(mTrays[i], cursorPos, 2))
                {
                    mTrayDrag = true;   // initiate a drag that originates in a tray
                    break;
                }
            }

            for (unsigned int i = 0; i < mWidgets[9].size(); i++)  // check if mouse is over a non-null tray's widgets
            {
                if (mWidgets[9][i]->getOverlayGeo()->isVisible() &&
                    Widget::isCursorOver(mWidgets[9][i]->getOverlayGeo(), cursorPos))
                {
                    mTrayDrag = true;   // initiate a drag that originates in a tray
                    break;
                }
            }

            if (!mTrayDrag) return false;   // don't process if mouse press is not in tray

            for (unsigned int i = 0; i < 10; i++)
            {
                if (!mTrays[i]->isVisible()) continue;

                for (unsigned int j = 0; j < mWidgets[i].size(); j++)
                {
                    Widget* w = mWidgets[i][j];
                    if (!w->getOverlayGeo()->isVisible()) continue;
                    w->_cursorPressed(cursorPos);    // send event to widget

                    SelectMenu* m = dynamic_cast<SelectMenu*>(w);
                    if (m && m->isExpanded())       // a menu has begun a top priority session
                    {
                        setExpandedMenu(m);
                        return true;
                    }
                }
            }

            return true;   // a tray click is not to be handled by another party
        }

        /*-----------------------------------------------------------------------------
        | Processes mouse button up events. Returns true if the event was
        | consumed and should not be passed on to other handlers.
        -----------------------------------------------------------------------------*/
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
        bool injectMouseUp(const OIS::MultiTouchEvent& evt)
#else
        bool injectMouseUp(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
#endif
        {
#if (OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS) && (OGRE_PLATFORM != OGRE_PLATFORM_ANDROID)
            // only process left button when stuff is visible
            if (!mCursorLayer->isVisible() || id != OIS::MB_Left) return false;
#else
            // only process touches when stuff is visible
            if (!mCursorLayer->isVisible()) return false;
#endif
            Ogre::Vector2 cursorPos(mCursor->getLeft(), mCursor->getTop());

            if (mExpandedMenu)   // only check top priority widget until it passes on
            {
                mExpandedMenu->_cursorReleased(cursorPos);
                return true;
            }

            if (mDialog)   // only check top priority widget until it passes on
            {
                mDialog->_cursorReleased(cursorPos);
                if (mOk) mOk->_cursorReleased(cursorPos);
                else
                {
                    mYes->_cursorReleased(cursorPos);
                    // very important to check if second button still exists, because first button could've closed the popup
                    if (mNo) mNo->_cursorReleased(cursorPos);
                }
                return true;
            }

            if (!mTrayDrag) return false;    // this click did not originate in a tray, so don't process

            Widget* w;

            for (unsigned int i = 0; i < 10; i++)
            {
                if (!mTrays[i]->isVisible()) continue;

                for (unsigned int j = 0; j < mWidgets[i].size(); j++)
                {
                    w = mWidgets[i][j];
                    if (!w->getOverlayGeo()->isVisible()) continue;
                    w->_cursorReleased(cursorPos);    // send event to widget
                }
            }

            mTrayDrag = false;   // stop this drag
            return true;         // this click did originate in this tray, so don't pass it on
        }

        /*-----------------------------------------------------------------------------
        | Updates cursor position. Returns true if the event was
        | consumed and should not be passed on to other handlers.
        -----------------------------------------------------------------------------*/
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
        bool injectMouseMove(const OIS::MultiTouchEvent& evt)
#else
        bool injectMouseMove(const OIS::MouseEvent& evt)
#endif
        {
            if (!mCursorLayer->isVisible()) return false;   // don't process if cursor layer is invisible

            Ogre::Vector2 cursorPos(evt.state.X.abs, evt.state.Y.abs);
            mCursor->setPos(cursorPos.x, cursorPos.y);

            if (mExpandedMenu)   // only check top priority widget until it passes on
            {
                mExpandedMenu->_cursorMoved(cursorPos);
                return true;
            }

            if (mDialog)   // only check top priority widget until it passes on
            {
                mDialog->_cursorMoved(cursorPos);
                if (mOk) mOk->_cursorMoved(cursorPos);
                else
                {
                    mYes->_cursorMoved(cursorPos);
                    mNo->_cursorMoved(cursorPos);
                }
                return true;
            }

            Widget* w;

            for (unsigned int i = 0; i < 10; i++)
            {
                if (!mTrays[i]->isVisible()) continue;

                for (unsigned int j = 0; j < mWidgets[i].size(); j++)
                {
                    w = mWidgets[i][j];
                    if (!w->getOverlayGeo()->isVisible()) continue;
                    w->_cursorMoved(cursorPos);    // send event to widget
                }
            }

            if (mTrayDrag) return true;  // don't pass this event on if we're in the middle of a drag
            return false;
        }
    protected:
        /*-----------------------------------------------------------------------------
        | Internal method to prioritise / deprioritise expanded menus.
        -----------------------------------------------------------------------------*/
        void setExpandedMenu(SelectMenu * m)
        {
            if(!mExpandedMenu && m)
            {
                Ogre::OverlayView * c = (Ogre::OverlayView *)m->getOverlayGeo();
                Ogre::OverlayView * eb = (Ogre::OverlayView *)c->get(m->getName() + "/MenuExpandedBox");
                eb->_update();
                eb->setPos
                    ((unsigned int)(eb->getEnvLeft() * eb->getScreenWidth()),
                    (unsigned int)(eb->getEnvTop() * eb->getScreenHeight()));
                c->remove(eb->getName());
                mPriorityLayer->add(eb);
            }
            else if(mExpandedMenu && !m)
            {
                Ogre::OverlayView* eb = mPriorityLayer->get(mExpandedMenu->getName() + "/MenuExpandedBox");
                mPriorityLayer->remove(eb);
                ((Ogre::OverlayView*)mExpandedMenu->getOverlayGeo())->add(eb);
            }

            mExpandedMenu = m;
        }

        Ogre::String mName;                   // name of this tray system
        Ogre::ViewWindow* mWindow;          // render window
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
        OIS::MultiTouch* mMouse;              // multitouch device
#else
        OIS::Mouse* mMouse;                   // mouse device
#endif
        Ogre::Overlay* mBackdropLayer;        // backdrop layer
        Ogre::Overlay* mTraysLayer;           // widget layer
        Ogre::Overlay* mPriorityLayer;        // top priority layer
        Ogre::Overlay* mCursorLayer;          // cursor layer
        Ogre::OverlayView* mBackdrop;    // backdrop
        Ogre::OverlayView* mTrays[10];   // widget trays
        WidgetList mWidgets[10];              // widgets
        WidgetList mWidgetDeathRow;           // widget queue for deletion
        Ogre::OverlayView* mCursor;      // cursor
        SdkTrayListener* mListener;           // tray listener
        Ogre::NIIf mWidgetPadding;            // widget padding
        Ogre::NIIf mWidgetSpacing;            // widget spacing
        Ogre::NIIf mTrayPadding;              // tray padding
        bool mTrayDrag;                       // a mouse press was initiated on a tray
        SelectMenu* mExpandedMenu;            // top priority expanded menu widget
        TextBox* mDialog;                     // top priority dialog widget
        Ogre::OverlayView* mDialogShade; // top priority dialog shade
        Button* mOk;                          // top priority OK button
        Button* mYes;                         // top priority Yes button
        Button* mNo;                          // top priority No button
        bool mCursorWasVisible;               // cursor state before showing dialog
        Label* mFpsLabel;                     // FPS label
        ParamsPanel* mStatsPanel;             // frame stats panel
        DecorWidget* mLogo;                   // logo
        ProgressBar* mLoadBar;                // loading bar
        Ogre::NIIf mGroupInitProportion;      // proportion of load job assigned to initialising one resource group
        Ogre::NIIf mGroupLoadProportion;      // proportion of load job assigned to loading one resource group
        Ogre::NIIf mLoadInc;                  // loading increment
        Ogre::HAlign mTrayWidgetAlign[10];   // tray widget alignments
        Ogre::Timer* mTimer;                  // Root::getSingleton().getTimer()
        unsigned long mLastStatUpdateTime;    // The last time the stat text were updated

    };
}

#endif
