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

#ifndef _NII_UI_VIEWCOM_H_
#define _NII_UI_VIEWCOM_H_

#include "NiiUIPreInclude.h"
#include "NiiUICommon.h"
#include "NiiVector2.h"
#include "NiiRect.h"
#include "NiiColour.h"

namespace NII
{
namespace UI
{
    /** ����Ԫ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelUnit : public UIAlloc
    {
    public:
        virtual ~PixelUnit();

        /** ����x�����Ű�
        @version NIIEngine 3.0.0
        */
        inline void setHLayout(HLayout hl){ mHLayout = hl;}

        /** ��ȡx�����Ű�
        @version NIIEngine 3.0.0
        */
        inline HLayout getHLayout() const{ return mHLayout;}

        /** ����y�����Ű�
        @version NIIEngine 3.0.0
        */
        inline void setVLayout(VLayout vl){ mVLayout = vl; }

        /** ��ȡy�����Ű�
        @version NIIEngine 3.0.0
        */
        inline VLayout getVLayout() const{ return mVLayout;}

        /** �����ı����
        @version NIIEngine 3.0.0
        */
        inline void setPadding(const Rectf & pad){ mPadding = pad; }

        /** ��ȡ�ı����
        @version NIIEngine 3.0.0
        */
        inline const Rectf & getPadding() const{ return mPadding;}

        /** �����Ƿ�̶���߱�
        @version NIIEngine 3.0.0
        */
        inline void setFixedAspect(bool set){ mFixedAspect = set;}

        /** ��ȡ�Ƿ�̶���߱�
        @version NIIEngine 3.0.0
        */
        inline bool getFixedAspect() const{ return mFixedAspect; }

        /** ��Ⱦ���ε�Ԫ������������
        @param[in] target ʵ��Ӧ�õ���UI��Ԫ
        @version NIIEngine 3.0.0
        */
        virtual void render(GeometryPixel * gp, const Widget * target, const Rectf * clip,
            const Vector2f & pos, NIIf height, NIIf widthext, const ColourArea * colour) const = 0;

        /** ��ȡ��ԪӦ��ʱʵ�ʴ�С
        @param[in] target ʵ��Ӧ�õ���UI��Ԫ
        @version NIIEngine 3.0.0
        */
        virtual PlaneSizef getPixelSize(const Widget * dest) const = 0;

        /** ���ϵ�Ԫ
        @version NIIEngine 3.0.0
        */
        virtual bool isMultiUnit() const = 0;

        /** ѡ�к��ͼ��
        @note һ���ǰ�͸��״
        @version NIIEngine 3.0.0
        */
        inline void setSelectView(PixelBuffer * view){ mSelectPixel = view;}
        
        /** ѡ�к��ͼ��
        @note һ���ǰ�͸��״
        @version NIIEngine 3.0.0
        */
        inline const PixelBuffer * getSelectView() const { return mSelectPixel;}

        /** ���ø���ѡ��
        @param[in] start ��ʼ���
        @param[in] end �������
        @version NIIEngine 3.0.0
        */
        virtual void setSelect(const Widget * target, NIIf start, NIIf end) = 0;

        /** ���븴�ϵ�Ԫ
        @param[in] start �ָʼ���
        @return ����������
        @version NIIEngine 3.0.0
        */
        virtual PixelUnit * split(const Widget * target, NIIf pixelsplitpoint, Nidx startindex);

        /** �ո�����
        @note MultiUnit,�հ׷�������,Space/Tab(Space4X)���ǿհ׷���
        @version NIIEngine 3.0.0
        */
        virtual NCount getBlankCount() const;

        /** ��������
        @note ʹ�� N_delete �ͷ��ڴ�
        @version NIIEngine 3.0.0
        */
        virtual PixelUnit * clone() const = 0;
    protected:
        PixelUnit();
    protected:
        PixelBuffer * mSelectPixel;
        Rectf mPadding;
        VLayout mVLayout;
        HLayout mHLayout;
        bool mFixedAspect;
    };
    
    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TextPixelUnit : public PixelUnit
    {
    public:
        TextPixelUnit();
        TextPixelUnit(const String & text);
        TextPixelUnit(const String & text, const Font * font);

        /**
        @version NIIEngine 3.0.0
        */
        inline void setText(const String & text){ mText = text;}

        /**
        @version NIIEngine 3.0.0
        */
        inline const String & getText() const{return mText;}

        /**
        @version NIIEngine 3.0.0
        */
        inline void setFont(const Font * font){mFont = font;}

        /**
        @version NIIEngine 3.0.0
        */
        inline const Font * getFont() const{ return mFont; }

        /**
        @version NIIEngine 3.0.0
        */
        inline void setColour(const ColourArea & colour){mColour = colour;}

        /**
        @version NIIEngine 3.0.0
        */
        inline const ColourArea & getColour() const{return mColour;}

        /// @copydetails PixelUnit::render
        void render(GeometryPixel * gp, const Widget * dest, const Rectf * clip,
            const Vector2f & pos, NIIf hegiht, NIIf widthext, const ColourArea * ca) const;

        /// @copydetails PixelUnit::getPixelSize
        PlaneSizef getPixelSize(const Widget * dest) const;

        /// @copydetails PixelUnit::isMultiUnit
        bool isMultiUnit() const;

        /// @copydetails PixelUnit::setSelect
        void setSelect(const Widget * dest, NIIf start, NIIf end);
        
        /// @copydetails PixelUnit::split
        PixelUnit * split(const Widget * dest, NIIf pixelsplitpoint, Nidx startindex);

        /// @copydetails PixelUnit::getBlankCount
        NCount getBlankCount() const;
        
        /// @copydetails PixelUnit::clone
        PixelUnit * clone() const;
    protected:
        const Font * getDestFont(const Widget * dest) const;
    protected:
        String mText;
        const Font * mFont;
        ColourArea mColour;
        NCount mSelectBegin;
        NCount mSelectLength;
    };
    
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ImagePixelUnit : public PixelUnit
    {
    public:
        ImagePixelUnit();
        ImagePixelUnit(const PixelBuffer * pb);

        /**
        @version NIIEngine 3.0.0
        */
        inline void setImage(const PixelBuffer * pb) { mImage = pb; }

        /**
        @version NIIEngine 3.0.0
        */
        inline const PixelBuffer * getImage() const { return mImage; }

        /**
        @version NIIEngine 3.0.0
        */
        inline void setColour(const ColourArea & cr) { mColour = cr;}

        /**
        @version NIIEngine 3.0.0
        */
        inline const ColourArea & getColour() const{return mColour;}

        /**
        @version NIIEngine 3.0.0
        */
        inline void setSize(const PlaneSizef & sz) { mDestSize = sz;}

        /**
        @version NIIEngine 3.0.0
        */
        inline const PlaneSizef & getSize() const{ return mDestSize; }

        /// @copydetails PixelUnit::isMultiUnit
        bool isMultiUnit() const;
        
        /// @copydetails PixelUnit::render
        void render(GeometryPixel * gp, const Widget * dest, const Rectf * clip,
            const Vector2f & pos, NIIf height, NIIf widthext, const ColourArea * ca) const;

        /// @copydetails PixelUnit::setSelect
        void setSelect(const Widget * dest, NIIf start, NIIf end);
        
        /// @copydetails PixelUnit::getPixelSize
        PlaneSizef getPixelSize(const Widget * dest) const;
        
        /// @copydetails PixelUnit::clone
        PixelUnit * clone() const;
    protected:
        const PixelBuffer * mImage;
        ColourArea mColour;
        PlaneSizef mDestSize;
        bool mSelect;
    };
    
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI WidgetPixelUnit : public PixelUnit
    {
    public:
        WidgetPixelUnit();
        WidgetPixelUnit(WidgetID wid);
        WidgetPixelUnit(Widget * widget);

        /**
        @version NIIEngine 3.0.0
        */
        void setWindow(WidgetID wid);

        /**
        @version NIIEngine 3.0.0
        */
        void setWindow(Widget * widget);

        /**
        @version NIIEngine 3.0.0
        */
        const Widget * getWindow() const;

        /// @copydetails PixelUnit::render
        void render(GeometryPixel * gp, const Widget * dest, const Rectf * clip,
            const Vector2f & pos, NIIf height, NIIf widthext, const ColourArea * ca) const;

        /// @copydetails PixelUnit::getPixelSize
        PlaneSizef getPixelSize(const Widget * dest) const;

        /// @copydetails PixelUnit::isMultiUnit
        bool isMultiUnit() const;

        /// @copydetails PixelUnit::setSelect
        void setSelect(const Widget * dest, NIIf start, NIIf end);

        /// @copydetails PixelUnit::clone
        PixelUnit * clone() const;
    protected:
        Widget * getTarget(const Widget * dest) const;
    protected:
        WidgetID mWidgetID;
        mutable Widget * mTarget;
        mutable bool mTargetValid;
        bool mSelect;
    };
}
}
#endif