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

#ifndef _NII_AssignProperty_H_
#define _NII_AssignProperty_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 属性赋值
    @version NIIEngine 3.0.0
    */
    class _EngineAPI AssignProperty : public StreamAlloc
    {
    public:
        AssignProperty();
        AssignProperty(PropertyID pid, const String & value);

        /** 应用到属性控制对象中
        @version NIIEngine 3.0.0
        */
        void apply(PropertyCmdObj * obj) const;

        /** 设置属性
        @version NIIEngine 3.0.0
        */
        inline void setProperty(PropertyID pid)         { mProperty = pid; }

        /** 获取属性
        @version NIIEngine 3.0.0
        */
        inline PropertyID getProperty() const           { return mProperty; }

        /** 设置值
        @version NIIEngine 3.0.0
        */
        inline void setValue(const String & value)      { mValue = value; }

        /** 获取值
        @version NIIEngine 3.0.0
        */
        inline const String & getValue() const          { return mValue;}

        /** 写入到 XML
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * out) const;
    private:
        PropertyID mProperty;
        String mValue;
    };
}
#endif