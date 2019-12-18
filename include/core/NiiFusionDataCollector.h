/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-6-29

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