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

#ifndef _NII_GPU_CUSTOM_PARAM_H_
#define _NII_GPU_CUSTOM_PARAM_H_

#include "NiiPreInclude.h"
#include "NiiGpuProgramParam.h"

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
        virtual void updateCustom(GpuProgramParam * dst, const GpuSyncParamIndex & bind) const;
        
        /** 更新自定义参数到实际的着色程序参数中
        @param[in] dst 需要更新的着色程序参数集
        @param[in] index 参数使用的索引
        @param[in] bind 需要更新着色程序参数
        @version NIIEngine 3.0.0
        */
        virtual void updateCustom(GpuProgramParam * dst, Nidx index, const GpuSyncParamIndex & entry) const;
    protected:
        typedef map<Nidx, Vector4f>::type CustomParameterMap;
    protected:
        CustomParameterMap mCustomParameters;
    };
}
#endif