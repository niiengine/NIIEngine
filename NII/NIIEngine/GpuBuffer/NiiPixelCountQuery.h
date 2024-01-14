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

#ifndef _NII_PixelCountQuery_H_
#define _NII_PixelCountQuery_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 像素数量查询
    @version NIIEngine 3.0.0 高级api 
    */
    class _EngineAPI PixelCountQuery : public RenderSysAlloc
    {
    public:
        PixelCountQuery();

        virtual ~PixelCountQuery();

        /** 开始渲染
        @version NIIEngine 3.0.0
        */
        virtual void beginRender() = 0;

        /** 结束渲染
        @version NIIEngine 3.0.0
        */
        virtual void endRender() = 0;

        /** 执行像素查询
        @param[out] count 获取片段数量的结果
        @return 如果查询成功返回true
        @version NIIEngine 3.0.0
        */
        virtual bool query(NCount & count) = 0;

        /** 是否仍然在执行查询
        @version NIIEngine 3.0.0
        */
        virtual bool isRun() = 0;
    protected :
        bool mRun;
    };
}
#endif