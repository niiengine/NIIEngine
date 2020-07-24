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

#ifndef _NII_FRAMESHADERCH_H_
#define _NII_FRAMESHADERCH_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 合成的FrameObj的渲染
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FrameShaderCh : public FrameAlloc
    {
    public:
        typedef vector<FrameShaderOp *>::type OpList;
    public:
        FrameShaderCh(FrameFusionShader * fusion);
        ~FrameShaderCh();

        /** 获取父对象
        @version NIIEngine 3.0.0
        */
        FrameFusionShader * getFusion() const;

        /** 获取渲染器是否支持.
        @version NIIEngine 3.0.0
        */
        bool isValid() const;

        /** 设置只绘制一次
        @version NIIEngine 3.0.0
        */
        void setOnce(bool value);

        /** 获取只绘制一次
        @version NIIEngine 3.0.0
        */
        bool isOnce() const;

        /** 设置是否启用阴影.
        @version NIIEngine 3.0.0
        */
        void setShadowEnable(bool b);

        /** 获取是否启用阴影.
        @version NIIEngine 3.0.0
        */
        bool isShadowEnable() const;
        
        /** 设置可见掩码
        @version NIIEngine 3.0.0
        */
        void setVisibleMask(Nmark mark);

        /** 获取可见掩码
        @version NIIEngine 3.0.0
        */
        Nmark getVisibleMask();
        
        /** 设置是否使用前个结果
        @version NIEngine 3.0.0
        */
        void setUsePreResult(bool set);

        /** 获取是否使用前个结果
        @version NIEngine 3.0.0
        */
        bool isUsePreResult() const;

        /** 设置输出局部纹理名字
        @version NIIEngine 3.0.0
        */
        void setOutputName(const String & out);

        /** 获取输出局部纹理名字
        @version NIIEngine 3.0.0
        */
        const String & getOutputName() const;

        /** 设置目标通道的材质方案.
        @version NIIEngine 3.0.0
        */
        void setMaterialScheme(SchemeID schemeName);

        /** 获取目标通道的材质方案.
        @see ShaderFusion::setScheme.
        */
        SchemeID getMaterialScheme() const;

        /** 设置使用场景LOD偏量
        @version NIIEngine 3.0.0
        */
        void setLodBias(NIIf bias);

        /** 获取使用场景LOD偏量
        @version NIIEngine 3.0.0
        */
        NIIf getLodBias() const;

        /** 创建混合操作.
        @version NIIEngine 3.0.0
        */
        FrameShaderOp * create();

        /** 移去混合操作.
        @version NIIEngine 3.0.0
        */
        void destroy(Nidx idx);

        /** 获取混合操作.
        @version NIIEngine 3.0.0
        */
        FrameShaderOp * get(Nidx idx);

        /** 获取混合操作数量.
        @version NIIEngine 3.0.0
        */
        NCount getCount();

        /** 移去所有混合操作
        @version NIIEngine 3.0.0
        */
        void destroyAll();

        /** 获取操作列表
        @version NIIEngine 3.0.0
        */
        const OpList & getList() const;
    private:
        FrameFusionShader * mFusion;
        OpList mPasses;
        String mOutputName;
        Nui32 mVisibleMask;
        NIIf mLodBias;
        SchemeID mMaterialScheme;
        bool mOnce;
        bool mInputMode;
        bool mShadowsEnable;
    };
}
#endif