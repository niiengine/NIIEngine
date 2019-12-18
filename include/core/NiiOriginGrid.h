/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-5-7

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

#ifndef _NII_ORIGIN_GRID_H_
#define _NII_ORIGIN_GRID_H_

#include "NiiPreInclude.h"
#include "NiiRenderObj.h"

namespace NII
{
    /** 原点网格
    @remark 这东西很基础,也很常见,所有的三维编辑器都有的.一个平行与x轴,y坐标为0的网格
    @version NIIEngine 3.0.0
    */
    class _EngineAPI OriginGrid : public RenderObj
    {
    public:
        OriginGrid(NCount x, NCount y, Nmark bufmark);
        OriginGrid(SpaceID sid, NCount x, NCount y, Nmark bufmark);
        ~OriginGrid();

        /// @copydetails GeometryObj::distanceSquared
        NIIf distanceSquared(const Camera * cam) const;

        /// @copydetails SpaceObj::getCenterRange
        NIIf getCenterRange() const;
    protected:
        /// @copydetails GeometryObj::getMatrix
        void getMatrix(Matrix4f * out, NCount & count) const;
    private:
        void buildGeo(NCount x, NCount y, Nmark vBufUsage);
    protected:
        NCount mXGrid;
        NCount mYGrid;
    };
}
#endif