/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-28

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
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