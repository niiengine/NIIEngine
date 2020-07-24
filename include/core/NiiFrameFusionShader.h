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

#ifndef _NII_FrameFusionShader_H_
#define _NII_FrameFusionShader_H_

#include "NiiPreInclude.h"
#include "NiiPixelFormat.h"

namespace NII
{
    /** 基本组成技术
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FrameFusionShader : public FrameAlloc
    {
    public:
        /** 合成器的纹理来源
        @version NIIEngine 3.0.0
        */
        enum SourceType
        {
            ST_Temp,
            ST_Fusion,
            ST_Pool
        };

        /** 缓存帧定义
        @version NIIEngine 3.0.0
        */
        class FrameDefine : public FrameAlloc
        {
        public:
            FrameDefine() :
                mWidth(0),
                mHeight(0),
                mWidthFactor(1.0f),
                mHeightFactor(1.0f),
                mFusionSource(0),
                mRasterGroup(1),
                mSourceType(ST_Temp),
                mPool(false),
                mFSAA(true),
                mHWGamma(false) {}
        public:
            String mName;
            SourceType mSourceType;
            ResourceID mFusionSource;
            String mTextureSource;
            NCount mWidth;
            NCount mHeight;
            NIIf mWidthFactor;
            NIIf mHeightFactor;
            PixelFormatList mFormatList;
            Nui16 mRasterGroup;
            bool mPool;
            bool mFSAA;
            bool mHWGamma;
        };
        typedef vector<FrameShaderCh *>::type TargetList;
        typedef vector<FrameDefine *>::type DefineList;
    public:
        FrameFusionShader(Fusion * parent);
        virtual ~FrameFusionShader();

        /** 获取父对象
        @version NIIEngine 3.0.0
        */
        inline Fusion * getParent() const { return mParent; }

        /** 获取渲染器是否支持
        @version NIIEngine 3.0.0
        */
        bool isSupport(bool degenerate) const;

        /** 设置名字
        @version NIIEngine 3.0.0
        */
        void setName(const String & name);

        /** 获取名字
        @version NIIEngine 3.0.0
        */
        const String & getName() const;

        /** 设置方案ID
        @version NIIEngine 3.0.0
        */
        void setSchemeID(SchemeID sid);

        /** 获取方案ID
        @version NIIEngine 3.0.0
        */
        SchemeID getSchemeID() const;

        /** 创建帧定义
        @param[in] name 名字
        @version NIIEngine 3.0.0
        */
        FrameDefine * createDefine(const String & name);

        /** 移去帧定义
        @version NIIEngine 3.0.0
        */
        void removeDefine(Nidx idx);

        /** 获取帧定义
        @version NIIEngine 3.0.0
        */
        FrameDefine * getDefine(Nidx idx) const;

        /** 获取帧定义
        @version NIIEngine 3.0.0
        */
        FrameDefine * getDefine(const String & name) const;

        /** 获取帧定义数量
        @version NIIEngine 3.0.0
        */
        NCount getDefineCount() const;

        /** 移去所有帧定义
        @version NIIEngine 3.0.0
        */
        void removeAllDefine();

        /** 获取帧定义列表
        @version NIIEngine 3.0.0
        */
        const DefineList & getDefineList() const;

        /** 创建合成通道
        @version NIIEngine 3.0.0
        */
        FrameShaderCh * createCh();

        /** 移去合成通道
        @version NIIEngine 3.0.0
        */
        void removeCh(Nidx idx);

        /** 获取合成通道
        @version NIIEngine 3.0.0
        */
        FrameShaderCh * getCh(Nidx idx) const;

        /** 获取合成通道数量
        @version NIIEngine 3.0.0
        */
        NCount getChCount() const;

        /** 移去全部目标通道
        @version NIIEngine 3.0.0
        */
        void removeAllCh();

        /** 获取目标渲染列表
        @version NIIEngine 3.0.0
        */
        const TargetList & getTargetList() const;

        /** 获取输出混合通道
        @version NIIEngine 3.0.0
        */
        FrameShaderCh * getOutputCh() const;
    private:
        String mName;
        SchemeID mSchemeID;
        Fusion * mParent;
        FrameShaderCh * mOutputCh;
        DefineList mDefineList;
        TargetList mTargetList;
    };
}
#endif