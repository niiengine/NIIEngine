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

#ifndef _NII_UI_STYLE_XML_COM_H_
#define _NII_UI_STYLE_XML_COM_H_

#include "NiiUIPreInclude.h"
#include "NiiXmlAnalyzeManager.h"
#include "NiiUIWidget.h"
#include "NiiSizeDefine.h"
#include "NiiUIStyleProperty.h"

namespace NII
{
namespace UI
{
    /** ���Xml������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI StyleXmlAnalyze : public StyleProperty, public XmlAnalyze
    {
    public:
        StyleXmlAnalyze(LangID lid = N_PrimaryLang);
        ~StyleXmlAnalyze();
        
        /// @copydetails XmlAnalyze::getXSD
        const String & getXSD() const;

        /// @copydetails XmlAnalyze::getUnit
        XmlUnitID getUnit() const;

        /// @copydetails XmlAnalyze::getGroup
        GroupID getGroup() const;
    private:
        void PrcStyleCB(const PropertyData * pdc);
        void PrcWidgetStyleCB(const PropertyData * pdc);
        void PrcStyleWidgetCB(const PropertyData * pdc);
        void PrcStyleSectionCB(const PropertyData * pdc);
        void PrcStyleLayoutCB(const PropertyData * pdc);
        void PrcLayerCB(const PropertyData * pdc);
        void PrcSectionCB(const PropertyData * pdc);
        void PrcImageryComCB(const PropertyData * pdc);
        void PrcTextComCB(const PropertyData * pdc);
        void PrcFrameComCB(const PropertyData * pdc);
        void PrcComAreaCB(const PropertyData * pdc);
        void PrcComPixelCB(const PropertyData * pdc);
        void PrcColoursCB(const PropertyData * pdc);
        void PrcVLayoutCB(const PropertyData * pdc);
        void PrcHLayoutCB(const PropertyData * pdc);
        void PrcVAlignCB(const PropertyData * pdc);
        void PrcHAlignCB(const PropertyData * pdc);
        void PrcPropertyCB(const PropertyData * pdc);
        void PrcSizeDefineCB(const PropertyData * pdc);
        void PrcRelSizeCB(const PropertyData * pdc);
        void PrcAbsSizeCB(const PropertyData * pdc);
        void PrcPixelSizeCB(const PropertyData * pdc);
        void PrcPixelSizeIDCB(const PropertyData * pdc);
        void PrcWidgetSizeCB(const PropertyData * pdc);
        void PrcFontSizeCB(const PropertyData * pdc);
        void PrcSizeIDCB(const PropertyData * pdc);
        void PrcTextCB(const PropertyData * pdc);
        void PrcColoursIDCB(const PropertyData * pdc);
        void PrcStyleSpecialCB(const PropertyData * pdc);
        void PrcStylePropertyCB(const PropertyData * pdc);
        void PrcListenerPropertyCB(const PropertyData * pdc);
        void PrcSizeOpCB(const PropertyData * pdc);
        void PrcVLayoutIDCB(const PropertyData * pdc);
        void PrcHLayoutIDCB(const PropertyData * pdc);
        void PrcComAreaIDCB(const PropertyData * pdc);
        void PrcPixelIDCB(const PropertyData * pdc);
        void PrcTextIDCB(const PropertyData * pdc);
        void PrcFontIDCB(const PropertyData * pdc);
        void PrcColourCB(const PropertyData * pdc);
        void PrcListenerTargetCB(const PropertyData * pdc);
        void PrcAnimationCB(const PropertyData * pdc);
        void PrcListenerEventCB(const PropertyData * pdc);
        void PrcEventTargetCB(const PropertyData * pdc);
        void PrcStyleSpecialIDCB(const PropertyData * pdc);

        void PrcStyleUnitECB();
        void PrcWidgetStyleECB();
        void PrcStyleWidgetECB();
        void PrcStyleSectionECB();
        void PrcStyleLayoutECB();
        void PrcLayerECB();
        void PrcSectionECB();
        void PrcImageryComECB();
        void PrcTextComECB();
        void PrcFrameComECB();
        void PrcComAreaECB();
        void PrcStyleSpecialECB();
        void PrcAnySizeECB();
        void PrcListenerPropertyECB();
        void PrcListenerEventECB();

        void assignColours(const ColourArea & colour);

        static Nui32 hexStringToARGB(const String & str);
    protected:
        /// @copydetails XmlAnalyze::PrcTagUnitCB
        void PrcTagUnitCB(const PropertyData * pdc);
    private:
        typedef vector<SizeBase *>::type SizeBaseList;
    private:
        WidgetManager * mStyleManager;
        ListenerProperty * mListenerProperty;
        ListenerEvent * mListenerEvent;
        Style * mStyle;
        StyleSpecial * mStyleSpecial;
        StyleWidget * mStyleWidget;
        StyleSection * mStyleSection;
        StyleSectionImageUnit * mImageryCom;
        StyleSectionTextUnit * mTextCom;
        StyleSectionFrameUnit * mFrameCom;
        StyleArea * mComArea;
        SizeDefine mSizeDefine;
        StyleLayout * mStyleLayout;
        Layer * mLayer;
        Section * mSection;
        SizeBaseList mSizeList;
    };

    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI OperatorSize : public SizeBase
    {
    public:
        OperatorSize();
        OperatorSize(SizeOpType op);
        OperatorSize(SizeOpType op, SizeBase * first, SizeBase * second);
        ~OperatorSize();

        /// ���ò���
        inline void setOpType(SizeOpType op)            { mOp = op; }

        /// ��ȡ����
        inline SizeOpType getOpType() const             { return mOp; }

        /// ���õ�һ������
        void setFirst(const SizeBase * value);

        /// ��ȡ�ڶ�������
        inline SizeBase * getFirst() const              { return mLeft; }

        /// ���õڶ�������
        void setSecond(const SizeBase * value);

        /// ��ȡ�ڶ�������
        inline SizeBase * getSecond() const             { return mRight; }

        /// 
        void setNextOperand(SizeBase * operand, bool copy);

        /// SizeBase::getValue
        NIIf getValue(const Widget & wnd) const;

        /// SizeBase::getValue
        NIIf getValue(const Widget & wnd, const Rectf & container) const;

        /// SizeBase::clone
        SizeBase * clone() const;
    protected:
        NIIf getValueImpl(NIIf lval, NIIf rval) const;
        // @copydetails SizeBase::write
        void write(XmlSerializer * dest) const;
        // @copydetails SizeBase::writeTag
        void writeTag(XmlSerializer * dest) const;
        // @copydetails SizeBase::writeValue
        void writeValue(XmlSerializer * dest) const;
    protected:
        SizeBase * mLeft;
        SizeBase * mRight;
        SizeOpType mOp;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ImageSizeBase : public SizeBase
    {
    public:
        ImageSizeBase() {};
        ImageSizeBase(SizeType type);
        virtual ~ImageSizeBase() {};

        /// @copydetails SizeBase::getValue
        NIIf getValue(const Widget & w) const;

        /// @copydetails SizeBase::getValue
        NIIf getValue(const Widget & w, const Rectf & container) const;
    protected:
        /// ��ȡ����Ԫ
        virtual const PixelBuffer * getSourceImage(const Widget & Wid) const = 0;

        // @copydetails SizeBase::writeValue
        void writeValue(XmlSerializer * out) const;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ImageSize : public ImageSizeBase
    {
    public:
        ImageSize() {};
        ImageSize(const String & src, SizeType dim);

        /** ����������Դ
        @version NIIEngine 3.0.0
        */
        inline void setSourceImage(const String & src)  { mImageSrc = src; }

        /** ��ȡ������Դ
        @version NIIEngine 3.0.0
        */
        inline const String & getSourceImage() const    { return mImageSrc; }

        // SizeBase::clone
        SizeBase * clone() const;
    protected:
        /// @copydetails ImageSizeBase::getSourceImage
        const PixelBuffer * getSourceImage(const Widget & wnd) const;

        /// @copydetails SizeBase::writeTag
        void writeTag(XmlSerializer * dest) const;

        /// @copydetails SizeBase::writeValue
        void writeValue(XmlSerializer * dest) const;
    protected:
        String mImageSrc;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ImagePropertySize : public ImageSizeBase
    {
    public:
        ImagePropertySize() {};

        ImagePropertySize(PropertyID pid, SizeType dim);
        
        /** ����������Դ����
        @version NIIEngine 3.0.0
        */ 
        inline void setPropertyFetch(PropertyID pid)   { mPropertyID = pid; }
        
        /** ��ȡ������Դ����
        @version NIIEngine 3.0.0
        */ 
        inline PropertyID getPropertyFetch() const     { return mPropertyID; }

        // SizeBase::clone
        SizeBase * clone() const;
    protected:
        /// @copydetails ImageSizeBase::getSourceImage 
        const PixelBuffer * getSourceImage(const Widget & wnd) const;

        /// @copydetails SizeBase::writeTag
        void writeTag(XmlSerializer * dest) const;

        /// @copydetails SizeBase::writeValue
        void writeValue(XmlSerializer * dest) const;
    protected:
        PropertyID mPropertyID;
    };

    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI WidgetSize : public SizeBase
    {
    public:
        WidgetSize() {};
        WidgetSize(WidgetID id, SizeType type);

        /** ��������UI��Ԫ��ID
        @version NIIEngine 3.0.0
        */
        inline void setWidgetID(WidgetID id)        { mWidgetID = wid; }

        /** ��ȡ����UI��Ԫ��ID
        @version NIIEngine 3.0.0
        */
        inline WidgetID getWidgetID() const         { return mWidgetID; }

        /// @copydetails SizeBase::getValue
        NIIf getValue(const Widget & wnd) const;

        /// @copydetails SizeBase::getValue
        NIIf getValue(const Widget & wnd, const Rectf & container) const;

        /// @copydetails SizeBase::clone
        SizeBase * clone() const;
    protected:
        /// @copydetails SizeBase::writeTag
        void writeTag(XmlSerializer * dest) const;
        /// @copydetails SizeBase::writeValue
        void writeValue(XmlSerializer * dest) const;
    private:
        WidgetID mWidgetID;
    };

    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FontSize : public SizeBase
    {
    public:
        FontSize(){}
        FontSize(WidgetID wid, FontID fontid, const String & text,
            FontSizeType type, NIIf padding = 0);

        /// ��������UI��Ԫ��ID
        inline void setWidgetID(WidgetID wid)           { mWidgetID = id; }

        /// ��ȡ����UI��Ԫ��ID
        inline WidgetID getWidgetID() const             { return mWidgetID; }

        /// ����������ԴID
        inline void setFont(ResourceID id)              { mFontID = id; }

        /// ��ȡ������ԴID
        inline ResourceID getFont() const               { return mFontID; }

        /// ������ʾ���ı�
        inline void setText(const String & text)        { mText = text; }

        /// ��ȡ��ʾ���ı�
        inline const String & getText() const           { return mText; }

        /// ���ò�������
        inline void setMetric(FontSizeType type)        { mMetricType = type;}

        /// ��ȡ��������
        inline FontSizeType getMetric() const           { return mMetricType; }

        /// ��������С
        inline void setPadding(NIIf padding)            { mPadding = padding;}

        /// ��ȡ����С
        inline NIIf getPadding() const                  { return mPadding;}

        /// @copydetails SizeBase::getValue
        NIIf getValue(const Widget & wnd) const;

        /// @copydetails SizeBase::getValue
        NIIf getValue(const Widget & wnd, const Rectf & container) const;

        /// @copydetails SizeBase::clone
        SizeBase * clone() const;
    protected:
        /// @copydetails SizeBase::writeTag
        void writeTag(XmlSerializer * dest) const;

        /// @copydetails SizeBase::writeValue
        void writeValue(XmlSerializer * dest) const;
    private:
        String mText;
        FontID mFontID;
        WidgetID mWidgetID;
        FontSizeType mMetricType;
        NIIf mPadding;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PropertySize : public SizeBase
    {
    public:
        PropertySize(){}
        PropertySize(WidgetID wid, PropertyID pid, SizeType type);

        /** ��������UI��Ԫ��ID
        @version NIIEngine 3.0.0
        */
        inline void setWidgetID(WidgetID wid)           { mWidgetID = wid; }

        /** ��ȡ����UI��Ԫ��ID
        @version NIIEngine 3.0.0
        */
        inline WidgetID getWidgetID() const             { return mWidgetID; }

        /** ������Դ����
        @version NIIEngine 3.0.0
        */
        inline void setPropertyID(PropertyID pid)       { mPropertyID = pid; }

        /** ��ȡ��Դ����
        @version NIIEngine 3.0.0
        */
        inline PropertyID getPropertyID() const         { return mPropertyID; }

        /// @copydetails SizeBase::getValue
        NIIf getValue(const Widget & wnd) const;

        /// @copydetails SizeBase::getValue
        NIIf getValue(const Widget & wnd, const Rectf & container) const;

        /// @copydetails SizeBase::clone
        SizeBase * clone() const;
    protected:
        /// @copydetails SizeBase::writeTag
        void writeTag(XmlSerializer * dest) const;

        /// @copydetails SizeBase::writeValue
        void writeValue(XmlSerializer * dest) const;
    private:
        WidgetID mWidgetID;
        PropertyID mPropertyID;
    };
}
}
#endif