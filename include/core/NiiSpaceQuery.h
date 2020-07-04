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

#ifndef _NII_SpaceQuery_H_
#define _NII_SpaceQuery_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 执行在一个场景查询的类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SpaceQuery : public SpaceAlloc
    {
    public:
        /** 片元类型
        @version NIIEngine 3.0.0
        */
        enum PrimitivesType
        {
            PT_None,
            PT_Point,
            PT_PlaneList,
            PT_SpaceObj,
            PT_GeoRaw,
            PT_Collision,
            PT_Intersect
        };
    public:
        SpaceQuery();
        virtual ~SpaceQuery();

        /** 设置查询掩码
        @remark 主要用于匹配对象的相交类型
        @version NIIEngine 3.0.0
        */
        virtual void setQueryMask(Nmark mask);

        /** 获取查询掩码
        @version NIIEngine 3.0.0
        */
        virtual Nmark getQueryMask() const;

        /** 设置查询片元码
        @version NIIEngine 3.0.0
        */
        virtual void setPrimitivesMark(Nmark mark);

        /** 获取查询片元码
        @version NIIEngine 3.0.0
        */
        virtual Nmark getPrimitivesMark() const;

        /** 设置类型掩码
        @version NIIEngine 3.0.0
        */
        virtual void setQueryTypeMask(Nmark mask);

        /** 获取类型掩码
        @version NIIEngine 3.0.0
        */
        virtual Nmark getQueryTypeMask() const;

        /** 添加支持类型
        @version NIIEngine 3.0.0
        */
        virtual void addSupportPrimitives(PrimitivesType type);

        /** 获取支持类型码
        @version NIIEngine 3.0.0
        */
        virtual Nmark getSupportPrimitivesMark() const;
        
        /** 执行查询
        @versioin NIIEngine 3.0.0
        */
        virtual void execute(SpaceQueryCmd * cmd) = 0;
    protected:
        Nmark mQueryMask;
        Nmark mQueryTypeMask;
        Nmark mPrimitivesMark;
        Nmark mSupportPrimitivesMark;
    };
}
#endif