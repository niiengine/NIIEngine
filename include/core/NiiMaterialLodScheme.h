/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-22

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

#ifndef _NII_MATERIAL_LOD_SCHEME_H_
#define _NII_MATERIAL_LOD_SCHEME_H_

#include "NiiPreInclude.h"
#include "NiiLodScheme.h"

namespace NII
{
    /** 材质LOD
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MaterialLodScheme : public LodScheme
    {
    public:
        typedef vector<std::pair<NIIf, Nui32> >::type LodList;
    public:
        MaterialLodScheme(SchemeID sid);
        virtual ~MaterialLodScheme();

        /// @copydetails LodScheme::getLodList
        void getLodList(SrcList & list) const;

        /// @copydetails LodScheme::add
        void add(NIIf src);

        /// @copydetails LodScheme::removeAll
        void removeAll();
        
        /// @copydetails LodScheme::map
        void map();

        /// @copydetails LodScheme::setValid
        void setValid(bool s) {}

        /// @copydetails LodScheme::isValid
        bool isValid() const { return true; }

        /** 获取列表
        @version NIIEngine 3.0.0
        */
        const LodList & getList() const;
    protected:
        LodList mLods;
    };

    /** 材质视图LOD策略
    @version NIIEngine 3.0.0
    */
    class _EngineInner MaterialViewLodScheme : public MaterialLodScheme
    {
    public:
        MaterialViewLodScheme(SchemeID sid);

        /// @copydetails LodScheme::getOrigin
        virtual NIIf getOrigin() const;

        /// @copydetails LodScheme::get
        Nidx get(Nui32 value) const;

        /// @copydetails LodScheme::sort
        void sort();

        /// @copydetails LodScheme::valid
        bool valid() const;

        /// @copydetails LodScheme::clone
        LodScheme * clone() const;
    protected:
        /// @copydetails LodScheme::getValueImpl
        NIIf getValueImpl(const SpaceObj * obj, const Camera * camera) const;
    };

    /** 摄象机距离材质LOD策略
    @version NIIEngine 3.0.0
    */
    class _EngineInner MaterialCameraLodScheme : public MaterialLodScheme
    {
    public:
        MaterialCameraLodScheme(SchemeID sid);

        /** 设置对比区域
        @param[in] w 视口的宽度
        @param[in] h 视口的高度
        @version NIIEngine 3.0.0
        */
        void setOriArea(NIIf w, NIIf h, Radian fovY);

        /** 设置是否对比区域
        @version NIIEngine 3.0.0
        */
        void setRefArea(bool b);

        /** 获取是否对比区域
        @version NIIEngine 3.0.0
        */
        bool isRefArea() const;

        /// @copydetails LodScheme::getDest
        Nui32 getDest(NIIf src) const;

        /// @copydetails LodScheme::getBias
        NIIf getBias(NIIf f) const;

        /// @copydetails LodScheme::get
        Nidx get(Nui32 value) const;

        /// @copydetails LodScheme::sort
        void sort();

        /// @copydetails LodScheme::valid
        bool valid() const;

        /// @copydetails LodScheme::clone
        LodScheme * clone() const;
    protected:
        /// @copydetails LodScheme::getValueImpl
        NIIf getValueImpl(const SpaceObj * obj, const Camera * camera) const;
    private:
        bool mRelation;
        NIIf mRefArea;
    };
}
#endif