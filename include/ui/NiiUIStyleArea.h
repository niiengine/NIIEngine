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
#ifndef _NII_StyleArea_H_
#define _NII_StyleArea_H_

#include "NiiUIPreInclude.h"
#include "NiiSizeDefine.h"

namespace NII
{
namespace UI
{
    /** ����UI��Ԫ��ͼ��ɷ�
    @version NIIEngine 3.0.0
    */
    class _EngineAPI StyleArea : public UIAlloc
    {
    public:
        StyleArea();

        /** ��ȡ�������������
        @param w
        @version NIIEngine 3.0.0
        */
        Rectf getPixelRect(const Widget & w) const;

        /** ��ȡ�������������
        @param w
        @param container
        @version NIIEngine 3.0.0
        */
        Rectf getPixelRect(const Widget & w, const Rectf & container) const;

        /**����
        @version NIIEngine 3.0.0
        */
        void setSrcFetch(StyleID sid, Nid style_special);

        /**
        @version NIIEngine 3.0.0
        */
        void setPropertyFetch(PropertyID pid);

        /**
        @version NIIEngine 3.0.0
        */
        PropertyID getPropertyFetch() const;

        /** ͨ�����Ի�ȡ
        @version NIIEngine 3.0.0
        */
        bool isPropertyFetch() const;

        /**
        @version NIIEngine 3.0.0
        */
        bool isSpecialFetch() const;

        /**
        @version NIIEngine 3.0.0
        */
        bool notifyFont(Widget * widget, const Font * font) const;

        /** 
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * out) const;
    public:
        SizeDefine mLeft;
        SizeDefine mTop;
        SizeDefine mXDim;
        SizeDefine mYDim;
    private:
        PropertyID mPropertyID;
        StyleID mStyleID;
    };
}
}
#endif