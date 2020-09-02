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
    @remark 在ApuParamMap的基础上再涵盖共享/公共参数
    @version NIIEngine 4.0.0
    */
    class _EngineAPI ApuProgramParam : public GpuParamsAlloc
    {
    public:
        ApuProgramParam();
        ApuProgramParam(const ApuProgramParam & o);
        ~ApuProgramParam();

        ApuProgramParam & operator=(const ApuProgramParam & o);

        /** 设置参数总数
        @version NIIEngine 4.0.0 adv
        */
        inline void setCount(NCount cnt)            { mParamCount = cnt; }

        /** 获取参数总数
        @version NIIEngine 4.0.0 adv
        */
        inline NCount getCount() const              { return mParamCount; }

        /** 当前参数是否有效
        @version NIIEngine 4.0.0
        */
        bool isValid() const;

        /** 设置参数绑定(基础)
        @version NIIEngine 4.0.0 高级api
        */
        inline void setMap(ApuParamMap * map)       { mParamMap = map; }

        /** 获取参数绑定(基础)
        @version NIIEngine 4.0.0 高级api
        */
        inline const ApuParamMap * getMap() const   { return mParamMap; }

        /** 添加参数（公共）
        @version NIIEngine 4.0.0
        */
        bool addDefault(Nidx idx, Nmark tmark, void * src, NCount unitSize, NCount unitCnt);

        /** 参数是否存在
        @version NIIEngine 4.0.0
        */
        inline bool isDefaultExist(Nidx idx) const  { return mShareSyncList.find(idx) != mShareSyncList.end(); }

        /** 获取参数数量
        @version NIIEngine 4.0.0
        */
        inline NCount getDefaultCount() const       { return mShareSyncList.size(); }

        /** 映射缓存（公共）
        @note 可能当bind函数含有T_Ref非T_Copy,并且属于同一操作体系平台,则可能与源指针相同
        @version NIIEngine 4.0.0
        */
        void * lockDefault(Nidx idx, Nmark opmark, NCount oft, NCount size);

        /** 解除映射（公共）
        @version NIIEngine 4.0.0
        */
        void unlockDefault(Nidx idx);

        /** 读取（公共）
        @version NIIEngine 4.0.0
        */
        void readDefault(Nidx idx, void * out, NCount oft, NCount size) const;

        /** 写入（公共）
        @version NIIEngine 4.0.0
        */
        void writeDefault(Nidx idx, const void * in, NCount oft, NCount size);

        /** 移去参数（公共）
        @version NIIEngine 4.0.0
        */
        void removeDefault(Nidx idx);

        /** 移去所有参数（公共）
        @version NIIEngine 4.0.0
        */
        void removeAllDefault();

        /** 获取列表
        @version NIIEngine 4.0.0
        */
        inline const ApuParamMap::UnitList & getDefaultUnitList() const { return mShareSyncList; }

        /** 复制所有参数（公共）
        @version NIIEngine 4.0.0
        */
        void copyDefault(const ApuProgramParam & src);
    protected:
        NCount mParamCount;
        ApuParamMap * mParamMap;
        ApuParamMap::UnitList mShareSyncList;
    };
}
#endif
