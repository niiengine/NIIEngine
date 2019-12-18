/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-7-7

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

#ifndef _NII_GPU_CUSTOM_PARAM_H_
#define _NII_GPU_CUSTOM_PARAM_H_

#include "NiiPreInclude.h"
#include "NiiGpuParamBind.h"

namespace NII
{
    /** 着色程序自定义参数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuCustomParam
    {
    public:
        GpuCustomParam();
        ~GpuCustomParam();

        /** 为这个可渲染物设置一个自定义参数,用于推倒计算这个指定的可渲染物,
            像GPU着色程序参数那样
        @param[in] index
        @param[in] value
        @version NIIEngine 3.0.0
        */
        void setCustonParam(Nidx index, const Vector4f & value);

        /** 给定索引获取关联这个可渲染物的自定义值
        @param[in] index 参数使用的索引
        @version NIIEngine 3.0.0
        */
        const Vector4f & getCustomParam(Nidx index) const;

        /** 移去自定义参数
        @param[in] index 参数使用的索引
        @version NIIEngine 3.0.0
        */
        void removeCustomParam(Nidx index);

        /** 是否存在自定义参数
        @param[in] index 参数使用的索引
        @version NIIEngine 3.0.0
        */
        bool isCustomParamExist(Nidx index) const;

        /** 更新自定义参数到实际的着色程序参数中
        @note 参数使用的索引 bind 里的 mInputInt
        @param[in] dst 需要更新的着色程序参数集
        @param[in] bind 需要更新着色程序参数
        @version NIIEngine 3.0.0
        */
        virtual void updateCustom(GpuProgramParam * dst, const GpuSyncParamBind & bind) const;
        
        /** 更新自定义参数到实际的着色程序参数中
        @param[in] dst 需要更新的着色程序参数集
        @param[in] index 参数使用的索引
        @param[in] bind 需要更新着色程序参数
        @version NIIEngine 3.0.0
        */
        virtual void updateCustom(GpuProgramParam * dst, Nidx index, const GpuSyncParamBind & entry) const;
    protected:
        typedef map<Nidx, Vector4f>::type CustomParameterMap;
    protected:
        CustomParameterMap mCustomParameters;
    };
}
#endif