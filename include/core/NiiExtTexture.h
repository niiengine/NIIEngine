/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

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

#ifndef _NII_ExternalTextureSource_H
#define _NII_ExternalTextureSource_H

#include "NiiPropertyCmdObj.h"
#include "NiiResourceLoadManager.h"
#include "NiiPlugin.h"

namespace NII
{
    /** 来自视频的纹理
    @version NIIEngien 3.0.0
    */
    class _EngineAPI ExtTexture : public PropertyCmdObj, public Plugin
    {
    public:
        enum PlayState
        {
            PS_Play = 0,
            PS_Loop = 1,
            PS_Pause = 2,
            PS_Stop = 3
        };
    public:
        ExtTexture(PluginID pid, LangID lid = N_PrimaryLang);
        virtual ~ExtTexture() {}

        /** 
        @version NIIEngine 3.0.0
        */
        virtual void createTexture(ResourceID rid, GroupID gid = GroupDefault) = 0;

        /** 
        @version NIIEngine 3.0.0
        */
        virtual void destroyTexture(const String & rid, GroupID gid = GroupDefault) = 0;

        /** 设置输入文件
        @remark 输入文件
        @version NIIEngine 3.0.0
        */
        void setInputName(String in) { mInputFileName = in; }

        /** 获取输入文件
        @version NIIEngine 3.0.0
        */
        const String & getInputName() const { return mInputFileName; }

        /** 设置帧每秒
        @version NIIEngine 3.0.0
        */
        void setFPS(NIIi fps) { mFramesPerSecond = fps; }

        /** 获取帧每秒
        @version NIIEngine 3.0.0
        */
        const NIIi getFPS() const { return mFramesPerSecond; }

        /** 设置播放模式
        @version NIIEngine 3.0.0
        */
        void setPlayMode(PlayState mode) { mMode = mode; }

        /** 获取播放模式
        @version NIIEngine 3.0.0
        */
        PlayState getPlayMode() const { return mMode; }

        /** 设置渲染混合
        @version NIIEngine 3.0.0
        */
        void setShaderFusion(NIIi fusion)
        { 
            mShaderFusion = fusion;
        }

        /** 设置渲染通道
        @version NIIEngine 3.0.0
        */
        void setShaderCh(Nid ch)
        {
            mShaderCh = ch;
        }

        /** 设置渲染等级
        @version NIIEngine 3.0.0
        */
        void setRenderLevel(Nidx level)
        {
            mRenderLevel = level;
        }

        /// 获取渲染混合
        Nid getShaderFusion() const
        { 
            return mShaderFusion;
        }

        /// 获取渲染通道
        Nid getShaderCh() const
        {
            return mShaderCh;
        }

        /// 获取渲染等级
        Nidx getRenderLevel() const
        {
            return mRenderLevel;
        }
    protected:
        bool init(PropertyCmdSet * dest);
    protected:
        PlayState mMode;
        String mInputFileName;
        NIIi mFramesPerSecond;
        Nid mShaderFusion;
        Nid mShaderCh;
        Nidx mRenderLevel;
        bool mPreFrame;
    };
}
#endif