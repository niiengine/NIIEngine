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

#ifndef _NII_FusionDataCollector_H_
#define _NII_FusionDataCollector_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 混合算术数据集
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FusionDataCollector
    {
    public:
        FusionDataCollector();
        virtual ~FusionDataCollector();

        /** 创建混合数据
        @version NIIEngine 3.0.0
        */
        virtual FusionData * createFusionData(Nid id);

        /** 获取混合集合类型
        @version NIIEngine 3.0.0
        */
        virtual TypeID getFusionCollectorType() const;
    protected:
        /** 建立混合数据列表
        @version NIIEngine 3.0.0
        */
        void setupFusionData() const;

        /** 初始化混合数据列表
        @version NIIEngine 3.0.0
        */
        virtual void initFusionCollector(IDList * ids) const = 0;
    protected:
        typedef map<TypeID, IDList>::type FusionCollectorList;
        static FusionCollectorList mFusionCollectorList;
    };
}
#endif