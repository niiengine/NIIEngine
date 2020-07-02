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

#ifndef _NII_DATA_VALUE_H_
#define _NII_DATA_VALUE_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 参子.
    @version NIIEngine 3.0.0
    */
    template <typename T> class DataValue : public DataAlloc
    {
    public:
        DataValue(bool autoDsy) :
            mAutoDestroy(autoDsy)
        {
        }

        virtual ~DataValue() {}

        /** 获取值
        @remark version NIIEngine 3.0.0
        */
        virtual const T getValue() const = 0;

        /** 设置值
        @remark version NIIEngine 3.0.0
        */
        virtual void setValue(const T & v) = 0;

        /** 设置是否自动删除数据
        @version NIIEngine 3.0.0
        */
        void setAutoDestroy(bool b){ mAutoDestroy = b; }

        /** 获取是否自动删除数据
        @version NIIEngine 3.0.0
        */
        bool isAutoDestroy() const{ return mAutoDestroy; }
    protected:
        bool mAutoDestroy;
    };
}
#endif