/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-7

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

#ifndef _NII_OverlayGeo_H_
#define _NII_OverlayGeo_H_

#include "NiiPreInclude.h"
#include "NiiGeometryObj.h"
#include "NiiPropertyCmdObj.h"
#include "NiiColour.h"
#include "NiiRect.h"

namespace NII
{
    /** 覆盖层几何
    @version NIIEngine 3.0.0
    */
    class _EngineAPI OverlayGeo : public GeometryObj, public PropertyCmdObj,
        public OverlayAlloc
    {
    public:
        OverlayGeo(OverlayGeoID id, ScriptTypeID stid, LangID lid = N_PrimaryLang);
        virtual ~OverlayGeo();

        /** 设置父容器
        @version NIIEngine 3.0.0
        */
        void setParent(OverlayView * parent);

        /** 获取父容器
        @version NIIEngine 3.0.0
        */
        OverlayView * getParent();

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        OverlayGeoID getID() const;

        /** 类型
        @version NIIEngine 3.0.0
        */
        virtual OverlayGeoType getTypeID() const = 0;

        /** 初始化
        @version NIIEngine 3.0.0
        */
        virtual void init() = 0;

        /** 显示
        @version NIIEngine 3.0.0
        */
        virtual void show();

        /** 隐藏
        @version NIIEngine 3.0.0
        */
        virtual void hide();

        /** 是否显示
        @version NIIEngine 3.0.0
        */
        bool isVisible() const;

        /** 设置是否启用
        @version NIIEngine 3.0.0
        */
        virtual void setEnabled(bool b);

        /** 获取是否启用
        @version NIIEngine 3.0.0
        */
        bool isEnabled() const;

        /** 设置标题
        @version NIIEngine 3.0.0
        */
        virtual void setCaption(const ViewString & text);

        /** 获取标题
        @version NIIEngine 3.0.0
        */
        virtual const ViewString & getCaption() const;

        /** 设置颜色
        @version NIIEngine 3.0.0
        */
        virtual void setColour(const Colour & col);

        /** 获取颜色
        @version NIIEngine 3.0.0
        */
        virtual const Colour & getColour() const;

        /** 设置尺寸模式
        @version NIIEngine 3.0.0
        */
        virtual void setSizeMode(SizeMode gmm);

        /** 获取尺寸模式
        @version NIIEngine 3.0.0
        */
        virtual SizeMode getSizeMode() const;

        /** 设置x对齐方式
        @version NIIEngine 3.0.0
        */
        void setXAlign(HAlign align);

        /** 获取x对齐方式
        @version NIIEngine 3.0.0
        */
        HAlign getXAlign() const;

        /** 设置y对齐方式
        @version NIIEngine 3.0.0
        */
        void setYAlign(VAlign align);

        /** 获取y对齐方式
        @version NIIEngine 3.0.0
        */
        VAlign getYAlign() const;

        /** 设置层等级
        @version NIIEngine 3.0.0
        */
        void setZOrder(Nidx idx);

        /** 获取层等级
        @version NIIEngine 3.0.0
        */
        Nidx getZOrder() const;

        /** 设置左上角位置
        @version NIIEngine 3.0.0
        */
        void setPos(NIIf left, NIIf top);

        /** 设置左上角x轴位置
        @version NIIEngine 3.0.0
        */
        void setLeft(NIIf left);

        /** 获取左上角x轴位置
        @version NIIEngine 3.0.0
        */
        NIIf getLeft() const;

        /** 设置左上角y轴位置
        @version NIIEngine 3.0.0
        */
        void setTop(NIIf Top);

        /** 获取左上角y轴位置
        @version NIIEngine 3.0.0
        */
        NIIf getTop() const;

        /** 设置大小
        @version NIIEngine 3.0.0
        */
        void setSize(NIIf width, NIIf height);

        /** 设置宽度
        @version NIIEngine 3.0.0
        */
        void setWidth(NIIf w);

        /** 获取宽度
        @version NIIEngine 3.0.0
        */
        NIIf getWidth() const;

        /** 设置高度
        @version NIIEngine 3.0.0
        */
        void setHeight(NIIf h);

        /** 获取高度
        @version NIIEngine 3.0.0
        */
        NIIf getHeight() const;

        /** 设置左上角位置(单位化)
        @param[in] left [0,1.0]
        @param[in] top [0,1.0]
        @version NIIEngine 3.0.0
        */
        void setUnitPos(NIIf left, NIIf top);

        /** 设置大小(单位化)
        @param[in] width [0,1.0]
        @param[in] height [0,1.0]
        @version NIIEngine 3.0.0
        */
        void setUnitSize(NIIf width, NIIf height);

        /** 设置左上角x轴位置(单位化)
        @param[in] left [0,1.0]
        @version NIIEngine 3.0.0
        */
        void setUnitLeft(NIIf left);

        /** 获取左上角x轴位置(单位化)
        @return [0,1.0]
        @version NIIEngine 3.0.0
        */
        NIIf getUnitLeft() const;

        /** 设置左上角y轴位置(单位化)
        @param[in] top [0,1.0]
        @version NIIEngine 3.0.0
        */
        void setUnitTop(NIIf top);

        /** 获取左上角y轴位置(单位化)
        @return [0,1.0]
        @version NIIEngine 3.0.0
        */
        NIIf getUnitTop() const;

        /** 设置宽度(单位化)
        @param[in] width [0,1.0]
        @version NIIEngine 3.0.0
        */
        void setUnitWidth(NIIf width);

        /** 获取宽度(单位化)
        @return [0,1.0]
        @version NIIEngine 3.0.0
        */
        NIIf getUnitWidth() const;

        /** 设置高度(单位化)
        @param[in] height [0,1.0]
        @version NIIEngine 3.0.0
        */
        void setUnitHeight(NIIf height);

        /** 获取高度(单位化)
        @return [0,1.0]
        @version NIIEngine 3.0.0
        */
        NIIf getUnitHeight() const;

        /** 获取左上角x轴实际位置
        @version NIIEngine 3.0.0
        */
        NIIf getEnvLeft();

        /** 获取左上角y轴实际位置
        @version NIIEngine 3.0.0
        */
        NIIf getEnvTop();

        /** 获取实际宽度
        @version NIIEngine 3.0.0
        */
        NIIf getEnvWidth();

        /** 获取实际高度
        @version NIIEngine 3.0.0
        */
        NIIf getEnvHeight();

        /** 获取裁减区域
        @version NIIEngine 3.0.0
        */
        void getClipArea(Rectf & rect);

        /** 设置纹理
        @version NIIEngine 3.0.0
        */
        void setMaterial(ResourceID rid);

        /** 获取纹理
        @version NIIEngine 3.0.0
        */
        ResourceID getMaterialID() const;

        /** 更新
        @version NIIEngine 3.0.0
        */
        virtual void _update();

        /** 位置改变
        @version NIIEngine 3.0.0
        */
        virtual void _notifyPos();

        /** 父对象改变
        @version NIIEngine 3.0.0
        */
        virtual void _notifyParent(OverlayView * parent, Overlay * overlay);

        /** 层级改变
        @version NIIEngine 3.0.0
        */
        virtual Nidx _notifyZOrder(Nidx order);

        /** 视口改变
        @version NIIEngine 3.0.0
        */
        virtual void _notifyViewport();

        /** 排列层到渲染队列
        @version NIIEngine 3.0.0
        */
        virtual void queue(RenderQueue * queue);

        /** 获取覆盖层几何
        @version NIIEngine 3.0.0
        */
        virtual OverlayGeo * getGeo(NIIf x, NIIf y) const;

        /** 获取屏幕宽度
        @version NIIEngine 3.0.0
        */
        virtual NIIi getScreenWidth() const;

        /** 获取屏幕高度
        @version NIIEngine 3.0.0
        */
        virtual NIIi getScreenHeight() const;

        /// @copydetails GeometryObj::getMaterial
        const Material * getMaterial() const;

        /// @copydetails GeometryObj::getMatrix
        void getMatrix(Matrix4f * out, NCount & count) const;

        /// @copydetails GeometryObj::distanceSquared
        NIIf distanceSquared(const Camera * cam) const;

        /// @copydetails GeometryObj::getLight
        void getLight(LightList & ll) const;

        /** 设置是否可模版化
        @version NIIEngine 3.0.0
        */
        void setTemplate(bool c);

        /** 获取是否可模版化
        @version NIIEngine 3.0.0
        */
        bool isTemplate() const;

        /** 复制模版
        @version NIIEngine 3.0.0
        */
        virtual void setTemplate(OverlayGeo * obj);

        /** 获取复制模版
        @version NIIEngine 3.0.0
        */
        const OverlayGeo * getTemplate() const;

        /** 副本
        @version NIIEngine 3.0.0
        */
        virtual OverlayGeo * clone(OverlayGeoID newid);
    protected:
        /// @copydetails PropertyCmdObj::init
        virtual bool init(PropertyCmdSet * dest);

        /** 更新
        @version NIIEngine 3.0.0 高级api
        */
        virtual void updateImpl();

        /** 更新几何位置
        @version NIIEngine 3.0.0 高级api
        */
        virtual void updateGeoPos() = 0;

        /** 更新几何纹理
        @version NIIEngine 3.0.0 高级api
        */
        virtual void updateGeoTex() = 0;
    protected:
        OverlayGeoID mID;
        ResourceID mMaterialID;
        Material * mMaterial;
        Overlay * mOverlay;
        OverlayView * mParent;
        OverlayGeo * mSrcTemplate;
        Viewport * mViewport;
        SizeMode mMetricsMode;
        HAlign mHorzAlign;
        VAlign mVertAlign;
        Nidx mZOrder;
        NIIf mLeft;
        NIIf mTop;
        NIIf mWidth;
        NIIf mHeight;
        NIIf mPixelTop;
        NIIf mPixelLeft;
        NIIf mPixelWidth;
        NIIf mPixelHeight;
        NIIf mPixelScaleX;
        NIIf mPixelScaleY;
        Rectf mClipArea;
        ViewString mCaption;
        Colour mColour;
        NIIf mEnvLeft;
        NIIf mEnvTop;
        bool mInit;
        bool mVisible;
        bool mEnabled;
        bool mNeedUpdate;
        bool mGeoPosDirty;
        bool mGeoTexDirty;
        bool mTemplate;
    };
}
#endif