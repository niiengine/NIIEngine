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

#ifndef _NII_ADDITIVE_STENCILSHADOW_PATTERN_H
#define _NII_ADDITIVE_STENCILSHADOW_PATTERN_H

#include "NiiPreInclude.h"
#include "NiiStencilRenderPattern.h"

namespace NII
{
    class AdditiveStencilRenderPattern : public StencilRenderPattern
    {
    public:
        AdditiveStencilRenderPattern(SpaceManager * sm, RenderSys * rsy, SysSyncParam * syncParam);

        /// @copydetails RenderPattern::prepare
        void prepare(RenderQueue * queue);

        /// @copydetails RenderPattern::prepare
        void prepare(RenderGroup * group);

        /// @copydetails Pattern::getQuality
        Nui8 getQuality() const { return 0; }

        /// @copydetails Pattern::getPerformance
		Nui8 getPerformance() const { return 0; }
    protected:
        /// @copydetails RenderPattern::renderImpl
        void renderImpl(RenderGroup * rg);

        /// @copydetails Pattern::_init
		bool _init() { return true; }

        /// @copydetails Pattern::_update
        void _update() {}

        /// @copydetails Pattern::_end
        bool _end(){ return true; }
        
        /// @copydetails Pattern::getPID
        PatternID getPID() const { return 0; }        
    };
}
#endif