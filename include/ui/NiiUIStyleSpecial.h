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
    /** 风格特定
    @version NIIEngine 3.0.0
    */
    class _EngineAPI StyleSpecial : public UIAlloc
    {
    public:
        StyleSpecial();
        StyleSpecial(Nid id);

        /** 设置ID
        @version NIIEngine 3.0.0
        */
        inline void setID(Nid id)                  {  mID = id; }

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        inline Nid getID() const                   { return mID; }

        /** 设置区域
        @version NIIEngine 3.0.0
        */
        inline void setArea(const StyleArea & area){ mArea = area; }

        /** 获取区域
        @version NIIEngine 3.0.0
        */
        inline const StyleArea & getArea() const   { return mArea; }

        /** 写入到XML
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * out) const;

        /** 字体改变时触发
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