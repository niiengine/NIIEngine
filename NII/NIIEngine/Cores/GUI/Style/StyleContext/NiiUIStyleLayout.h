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

#ifndef _NII_UI_STYLESTATE_H_
#define _NII_UI_STYLESTATE_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidget.h"
#include "NiiColour.h"

namespace NII
{
namespace UI
{
    /** ����
    @verison NIIEngine 3.0.0
    */
    class _EngineAPI Section : public UIAlloc
    {
    public:
        Section();
        Section(StyleID style, Nid section, PropertyID vctrlpid, WidgetID vctrlwidget);
        Section(StyleID style, Nid section, PropertyID vctrlpid, WidgetID vctrlwidget, const ColourArea & cols);

        /** ����ʹ�õķ��
        @version NIIEngine 3.0.0
        */
        inline void setStyle(StyleID id)                    { mStyle = id; }

        /** ��ȡʹ�õķ��
        @version NIIEngine 3.0.0
        */
        inline StyleID getStyle() const                     { return mStyle;}

        /** ����ʹ�õķ������
        @version NIIEngine 3.0.0
        */
        inline void setStyleSection(Nid id)                 { mStyleSection = id;}

        /** ��ȡʹ�õķ������
        @version NIIEngine 3.0.0
        */
        inline Nid getStyleSection() const                  { return mStyleSection;}

        /** ����������ɫ
        @version NIIEngine 3.0.0
        */
        inline void setColour(const ColourArea & colour)    { mColour = colour;}

        /** ��ȡ������ɫ
        @version NIIEngine 3.0.0
        */
        inline const ColourArea & getColour() const         { return mColour;}

        /** ����������ɫ(��Դ����)
        @version NIIEngine 3.0.0
        */
        inline void setColourID(PropertyID pid)             { mColourID = pid;}

        /** ��ȡ������ɫ(��Դ����)
        @version NIIEngine 3.0.0
        */
        inline PropertyID getColourID() const               { return mColourID; }

        /** ���ÿɼ���������
        @version NIIEngine 3.0.0
        */
        inline void setVisibleCtrlProperty(PropertyID pid)  { mCtlProperty = pid;}

        /** ��ȡ�ɼ���������
        @version NIIEngine 3.0.0
        */
        inline PropertyID getVisibleCtrlProperty() const    { return mCtlProperty; }

        /** ��ȡ�ɼ���������
        @version NIIEngine 3.0.0
        */
        inline void setVisibleCtrlWidget(WidgetID id)       { mCtlWidget = id; }
        
        /** ��ȡ�ɼ���������
        @version NIIEngine 3.0.0
        */
        inline WidgetID getVisibleCtrlWidget() const        { return mCtlWidget;}
        
        /** ��Ⱦ����
        @version NIIEngine 3.0.0
        */
        void render(const Widget * dest, const ColourArea * mc = 0, const Rectf * clip = 0) const;

        /** ��Ⱦ����
        @version NIIEngine 3.0.0
        */
        void render(const Widget * dest, const Rectf & rect, const ColourArea * mc = 0, const Rectf * clip = 0) const;

        /** д�뵽Xml
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * out) const;
    private:
        StyleID mStyle;
        Nid mStyleSection;
        ColourArea mColour;
        PropertyID mColourID;
        WidgetID mCtlWidget;
        PropertyID mCtlProperty;
    };
    
    /** UI�����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Layer : public UIAlloc
    {
    public:
        Layer(NCount level = 0);

        /** �ȽϺ���
        @version NIIEngine 3.0.0
        */
        inline bool operator<(const Layer & o) const{ return mLevel < o.mLevel;}

        /** ���ò�
        @version NIIEngine 3.0.0
        */
        inline void setLevel(NCount level)      { mLevel = level;}

        /** ��ȡ��
        @version NIIEngine 3.0.0
        */
        inline NCount getLevel() const          { return mLevel;}

        /** ���UI��������
        @version NIIEngine 3.0.0
        */
        inline void add(const Section & obj)    { mSections.push_back(obj); }

        /** ��ȥ���в�
        @version NIIEngine 3.0.0
        */
        inline void removeAll()                 { mSections.clear(); }

        /** ��Ⱦ��
        @version NIIEngine 3.0.0
        */
        void render(const Widget * dest, const ColourArea * colour = 0, const Rectf * clip = 0) const;

        /** ��Ⱦ��
        @version NIIEngine 3.0.0
        */
        void render(const Widget * dest, const Rectf & rect, const ColourArea * colour = 0, const Rectf * clip = 0) const;

        /** д�뵽Xml����
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * out) const;
    protected:
        typedef vector<Section>::type Sections;
    protected:
        NCount mLevel;
        Sections mSections;
    };
    
    /** UI���Ʒ�񲼾�
    @version NIIEngine 3.0.0
    */
    class _EngineAPI StyleLayout : public UIAlloc
    {
    public:
        StyleLayout();
        StyleLayout(Nid id);
        StyleLayout(const StyleLayout & o);

        /** ����ID
        @version NIIEngine 3.0.0
        */
        inline void setID(Nid id){ mID = id;}

        /** ��ȡID
        @version NIIEngine 3.0.0
        */
        inline Nid getID() const{ return mID;}

        /** �����Ƿ�ü�����
        @version NIIEngine 3.0.0
        */
        inline void setClip(bool set){ mClip = set;}

        /** ��ȡ�Ƿ�ü�����
        @version NIIEngine 3.0.0
        */
        inline bool isClip() const{ return mClip;}

        /** ���
        @version NIIEngine 3.0.0
        */
        inline void add(const Layer & layer){ mLayers.insert(layer);}

        /** �ƶ�
        @version NIIEngine 3.0.0
        */
        inline void removeAll(){ mLayers.clear();}

        /** ��Ⱦ״̬
        @version NIIEngine 3.0.0
        */
        void render(const Widget * dest, const ColourArea * cv = 0, const Rectf * clip = 0) const;

        /** ��Ⱦ״̬
        @version NIIEngine 3.0.0
        */
        void render(const Widget * dest, const Rectf & rect, const ColourArea * cv = 0, const Rectf * clip = 0) const;

        /** д��
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * out) const;
    protected:
        typedef multiset<Layer>::type Layers;
    protected:
        Nid mID;
        Layers mLayers;
        Nmark mMark;
        bool mClip;
    };
}
}
#endif