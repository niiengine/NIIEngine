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

#ifndef _NII_GEOMETRY_QUERY_H_
#define _NII_GEOMETRY_QUERY_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 几何查询器
    @remark 这个类可以保存查询结果
    @version NIIEngine 3.0.0 高级api
    */
    class GeometryQuery
    {
    public:
        GeometryQuery();
        virtual ~GeometryQuery();

        /** 设置模型调试模式
        @version NIIEngine 3.0.0
        */
        void setDebugMode(bool b);
        
        /** 获取调试模式模式
        @version NIIEngine 3.0.0
        */
        bool isDebugMode() const;
        
        /** 执行查询
        @version NIIEngine 3.0.0 高级api
        */
        virtual void query(GeometryObj * obj) = 0;
    protected:
        bool mDebug;
    };
}
#endif