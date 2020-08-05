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

#ifndef _NII_APU_PROGRAMPARAM_H_
#define _NII_APU_PROGRAMPARAM_H_

#include "NiiPreInclude.h"
#include "NiiApuParamMap.h"

namespace NII
{
    /** 着色程序参数
    @version NIIEngine 4.0.0
    */
    class _EngineAPI ApuProgramParam : public GpuParamsAlloc
    {
    public:
        ApuProgramParam();
        ApuProgramParam(const ApuProgramParam & o);
        ~ApuProgramParam();

        ApuProgramParam & operator=(const ApuProgramParam & o);

        /** 当前参数是否有效
        @version NIIEngine 4.0.0
        */
        bool isValid() const;

        /** 设置参数绑定
        @version NIIEngine 4.0.0 高级api
        */
        inline void setMap(ApuParamMap * map)       { mParamMap = map; }

        /** 获取参数绑定
        @version NIIEngine 4.0.0 高级api
        */
        inline const ApuParamMap * getMap() const   { return mParamMap; }

        /** 默认参数是否存在
        @version NIIEngine 4.0.0
        */
        bool addDefault(Nidx idx, Nmark tmark, void * src, NCount size) const;

        /** 默认参数是否存在
        @version NIIEngine 4.0.0
        */
        bool isDefaultExist(Nidx idx) const;

        /** 移去默认参数
        @version NIIEngine 4.0.0
        */
        void removeDefault(Nidx idx);

        /** 移去所有默认参数
        @version NIIEngine 4.0.0
        */
        void removeAllDefault();

        /** 复制所有默认参数
        @version NIIEngine 4.0.0
        */
        void copyDefault(const ApuProgramParam & src);
    protected:
        ApuParamMap * mParamMap;
        ApuParamMap::UnitList mShareSyncList;
    };
}
#endif
