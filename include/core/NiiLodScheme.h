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

#ifndef _NII_LOD_SCHEME_H_
#define _NII_LOD_SCHEME_H_

#include "NiiPreInclude.h"
#include "NiiCamera.h"
#include "NiiSpaceObj.h"

namespace NII
{
    /** 层次细节等级策略(空间对象)
    @remark 被动策略
    @version NIIEngine 3.0.0
    */
    class _EngineAPI LodScheme : public LodAlloc
    {
    public:
        typedef vector<NIIf>::type SrcList;
    public:
        LodScheme(SchemeID sid);
        virtual ~LodScheme();

        /** 获取这个策略的名字
        @version NIIEngine 3.0.0
        */
        SchemeID getTypeID() const;

        /** 添加原始值
        @param[in] src 原始基数
        @version NIIEngine 3.0.0
        */
        virtual void add(NIIf src) = 0;

        /** 获取原始详细程度值.
        @remark 原基本细节状态
        @version NIIEngine 3.0.0
        */
        virtual NIIf getOrigin() const;

        /** LOD的系数函子
        @version NIIEngine 3.0.0
        */
        virtual NIIf getBias(NIIf f) const;

        /** 获取映射值
        @remark 原始基数 到 目标基数的 映射
        @version NIIEngine 3.0.0
        */
        virtual Nui32 getDest(NIIf src) const;

        /** 设置因子
        @version NIIEngine 3.0.0
        */
        virtual void setFactor(Nui32 f);

        /** 获取因子
        @version NIIEngine 3.0.0
        */
        virtual Nui32 getFactor() const;

        /** 设置阀值
        @version NIIEngine 3.0.0
        */
        virtual void setThreshold(Nui32 t);

        /** 获取阀值
        @version NIIEngine 3.0.0
        */
        virtual Nui32 getThreshold() const;

        /** 给定空间对象和摄象机计算LOD值.
        @remark 因为摄象机含盖的信息最丰富,所以使用它作为切换LOD的变量
        @version NIIEngine 3.0.0
        */
        NIIf getValue(const SpaceObj * obj, const Camera * camera) const;

        /** 获取原始LOD列表
        @version NIIEngine 3.0.0
        */
        virtual void getLodList(SrcList & list) const = 0;

        /** 获取LOD层
        @param[in] value
        @version NIIEngine 3.0.0
        */
        virtual Nidx get(Nui32 value) const = 0;

        /** 排序所有映射
        @version NIIEngine 3.0.0
        */
        virtual void sort() = 0;

        /** 执行映射公式
        @remark 和 add(NIIf, Nui32) 不同,所有的 src->dst 同符合同一个线性函数
        @version NIIEngine 3.0.0
        */
        virtual void map() = 0;

        /** 设置策略是否有效
        @version NIIEngine 3.0.0 高级api
        */
        virtual void setValid(bool s) = 0;

        /** 获取策略是否有效
        @version NIIEngine 3.0.0
        */
        virtual bool isValid() const = 0;

        /** 移去所有LOD层
        @version NIIEngine 3.0.0
        */
        virtual void removeAll() = 0;

        /** 副本对象
        @version NIIEngine 3.0.0
        */
        virtual LodScheme * clone() const = 0;
    protected:
        LodScheme() {}

        /** 获取实际LOD值的实现
        @remark 因为摄象机含盖的信息最丰富,所以使用它作为切换LOD的变量
        @version NIIEngine 3.0.0
        */
        virtual NIIf getValueImpl(const SpaceObj * obj, const Camera * camera) const = 0;
    protected:
        SchemeID mSid;      ///<
        Nui32 mThreshold;   ///<
        Nui32 mFactor;      ///<
    };
}
#endif