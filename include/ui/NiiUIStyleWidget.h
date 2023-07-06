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

#ifndef _NII_UI_StyleWidget_H_
#define _NII_UI_StyleWidget_H_

#include "NiiUIPreInclude.h"
#include "NiiAssignProperty.h"
#include "NiiUIStyleSpecial.h"

namespace NII
{
namespace UI
{
    /** �������Ԫ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI StyleWidget : public UIAlloc
    {
    public:
        typedef vector<AssignProperty>::type PropertyList;
    public:
        StyleWidget(StyleViewID sid, WidgetID wid, bool autowin);

        /** ���ӵ���Ԫ��
        @version NIIEngine 3.0.0
        */
        void attach(Widget * obj) const;

        /** �ӵ�Ԫ�н��
        @version NIIEngine 3.0.0
        */
        void detach(Widget * obj) const;

        /** ����ID
        @version NIIEngine 3.0.0
        */
        inline void setID(WidgetID id)              { mID = id; }

        /** ��ȡID
        @version NIIEngine 3.0.0
        */
        inline WidgetID getID() const               { return mID; }

        /** �Ű�
        @version NIIEngine 3.0.0
        */
        void layout(Widget * dst) const;

        /** ��������
        @version NIIEngine 3.0.0
        */
        inline void setArea(const StyleArea & area) { mArea = area; }

        /** ��ȡ����
        @version NIIEngine 3.0.0
        */
        inline const StyleArea & getArea() const    { return mArea; }

        /** ���õ�Ԫ������
        @version NIIEngine 3.0.0
        */
        inline void setViewID(StyleViewID id)       { mViewID = id; }

        /** ��ȡ��Ԫ������
        @version NIIEngine 3.0.0
        */
        inline StyleViewID getViewID() const        { return mViewID; }

        /** ����x�������
        @version NIIEngine 3.0.0
        */
        inline void setHAlign(HAlign align)         { mHAlign = align; }

        /** ����x�������
        @version NIIEngine 3.0.0
        */
        inline HAlign getHAlign() const             { return mHAlign; }

        /** ����y�������
        @version NIIEngine 3.0.0
        */
        inline void setVAlign(VAlign align)         { mVAlign = align; }

        /** ��ȡy�������
        @version NIIEngine 3.0.0
        */
        inline VAlign getVAlign() const             { return mVAlign; }
        
        /** ���Ԥ��ֵ����
        @version NIIEngine 3.0.0
        */
        void add(const AssignProperty & property);

        /** ��ȡԤ��ֵ����
        @version NIIEngine 3.0.0
        */
        const AssignProperty * getAssignProperty(PropertyID pid) const;
        
        /** ��ȥԤ��ֵ����
        @version NIIEngine 3.0.0
        */
        void removeAssignProperty(PropertyID pid);

        /** ��������Ԥ��ֵ����
        @version NIIEngine 3.0.0
        */
        void removeAllAssignProperty();

        /** ��ȡԤ��ֵ�����б�
        @version NIIEngine 3.0.0
        */
        const PropertyList & getPropertyList() const{ return mPropertyList; }
        
        /** �����Ƿ��Զ�����
        @version NIIEngine 3.0.0
        */
        inline void setAutoWindow(bool b)           { mAutoGen = b; }

        /** ��ȡ�Ƿ��Զ�����
        @version NIIEngine 3.0.0
        */
        inline bool isAutoWindow() const            { return mAutoGen; }

        /**
        @version NIIEngine 3.0.0
        */
        bool notifyFont(Widget * window, const Font * font) const { return false; }

        /** д�뵽 XML
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * out) const;
    protected:
        StyleWidget();
    private:
        WidgetID mID;
        StyleViewID mViewID;
        VAlign mVAlign;
        HAlign mHAlign;
        StyleArea mArea;
        PropertyList mPropertyList;
        bool mAutoGen;
    };
}
}
#endif