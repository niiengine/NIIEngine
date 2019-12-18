/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-25

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

#ifndef _NII_Fusion_H_
#define _NII_Fusion_H_

#include "NiiPreInclude.h"
#include "NiiTexture.h"

namespace NII
{
    /** 帧合成对象
    @version NIIEngine 3.0.0
     */
    class _EngineAPI Fusion : public Resource
    {
    public:
        typedef vector<FrameFusionShader *>::type FrameList;
    public:
        Fusion(ResourceID rid, GroupID gid, 
            ResourceLoadScheme * ls = 0, ResourceResultScheme * rs = 0, LangID lid = N_PrimaryLang);
        ~Fusion();

        /** 创建合成帧
        @param[out] obj 创建合成帧
        @version NIIEngine 3.0.0
        */
        FrameFusionShader * create();

        /** 删除指定索引合成帧
        @param[in] index 指定索引
        @version NIIEngine 3.0.0
        */
        void destroy(Nidx index);

        /** 删除所有合成帧
        @version NIIEngine 3.0.0
        */
        void destroyAll();

        /** 获取合成帧
        @version NIIEngine 3.0.0
        */
        FrameFusionShader * get(Nidx index) const;

        /** 合成帧数量
        @version NIIEngine 3.0.0
        */
        NCount getCount() const;

        /** 获取合成帧数量
        @version NIIEngine 3.0.0
        */
        const FrameList & getList() const;

        /** 获取支持的合成帧数量
        @version NIIEngine 3.0.0
        */
        NCount getValidCount() const;

        /** 获取当前支持的混合
        @version NIIEngine 3.0.0
        */
        const FrameList & geValidList() const;

        /** 获取支持的合成帧.
        @version NIIEngine 3.0.0
        */
        FrameFusionShader * getValid(Nidx idx) const;

        /** 获取支持合成帧
        @param[in] sid 方案ID
        @version NIIEngine 3.0.0
        */
        FrameFusionShader * getValid(SchemeID sid = 0) const;

        /** 获取渲染纹理
        @param[in] name
        @param[in] multiIdx
        */
        Texture * getTexture(const String & name, Nidx multiIdx) const;

        /** 获取渲染对象
        @version NIIEngine 3.0.0
        */
        FrameObj * getFrameObj(const String & name) const;
    protected:
        /// @see Resource::loadImpl
        void loadImpl();

        /// @see Resource::unloadImpl
        void unloadImpl();

        /// @see Resource::calcSize
        NCount calcSize() const;
    private:
        typedef map<String, Texture *>::type TextureList;
        typedef map<String, MultiTextureFrame *>::type MultiTexFrameList;

        FrameList mFusions;
        FrameList mValidFusions;
        TextureList mTextureList;
        MultiTexFrameList mMultiTexFrameList;
        bool mCompilationRequired;
    };
}
#endif