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

#ifndef _NII_UI_StyleSpecial_H_
#define _NII_UI_StyleSpecial_H_

#include "NiiUIPreInclude.h"
#include "NiiSizeDefine.h"
#include "NiiUIStyleArea.h"

namespace NII
{
namespace UI
{
    /** ����ض�
    @version NIIEngine 3.0.0
    */
    class _EngineAPI StyleSpecial : public UIAlloc
    {
    public:
        StyleSpecial();
        StyleSpecial(Nid id);

        /** ����ID
        @version NIIEngine 3.0.0
        */
        inline void setID(Nid id)                  {  mID = id; }

        /** ��ȡID
        @version NIIEngine 3.0.0
        */
        inline Nid getID() const                   { return mID; }

        /** ��������
        @version NIIEngine 3.0.0
        */
        inline void setArea(const StyleArea & area){ mArea = area; }

        /** ��ȡ����
        @version NIIEngine 3.0.0
        */
        inline const StyleArea & getArea() const   { return mArea; }

        /** д�뵽XML
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * out) const;

        /** ����ı�ʱ����
        @version NIIEngine 3.0.0
        */
        bool notifyFont(Widget * obj, const Font * font) const;
    private:
        Nid mID;
        StyleArea mArea;
    };
}
}
#endif