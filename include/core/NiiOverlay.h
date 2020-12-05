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

#ifndef _NII_OVERLAY_H_
#define _NII_OVERLAY_H_

#include "NiiPreInclude.h"
#include "NiiGeometryObj.h"
#include "NiiPropertyCmdObj.h"
#include "NiiSpaceNode.h"
#include "NiiColour.h"
#include "NiiRect.h"

namespace NII
{
    /** 表层面
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Overlay : public OverlayAlloc
    {
        friend class OverlayManager;
    public:
        typedef list<OverlayView *>::type ViewList;
    public:
        Overlay(OverlayID oid, Viewport * vp);
        virtual ~Overlay();

        /** 获取
        @version NIIEngine 3.0.0
        */
        inline OverlayID getID() const                 { return mID;}

        /** 获取视口
        @version NIIEngine 3.0.0
        */
        inline Viewport * getViewport() const          { return mViewport; }
        
        /** 设置数据来源
        @version NIIEngine 3.0.0
        */
        inline void setSrc(const String & data)        { mSrc = data;}

        /** 获取数据来源
        @version NIIEngine 3.0.0
        */
        inline const String & getSrc() const           { return mSrc; }

        /** 设置层
        @version NIIEngine 3.0.0
        */
        void setZOrder(Nidx16 level);

        /** 获取层
        @version NIIEngine 3.0.0
        */
        inline Nidx16 getZOrder() const                 { return mZOrder; }

        /** 实现
        @version NIIEngine 3.0.0
        */
        void show();

        /** 隐藏
        @version NIIEngine 3.0.0
        */
        inline void hide()                              { mVisible = false; }

        /** 是否显示
        @version NIIEngine 3.0.0
        */
        inline bool isVisible() const                   { return mVisible; }

        /** 是否已经初始化
        @version NIIEngine 3.0.0
        */
        inline bool isInit() const                      { return mInit; }

        /** 添加节点
        @version NIIEngine 3.0.0
        */
        inline void add(SpaceNode * node)               { mRootNode->add(node); }

        /** 移去节点
        @version NIIEngine 3.0.0
        */
        inline void remove(SpaceNode * node)            { mRootNode->remove(node->getID()); }

        /** 添加图元视图
        @version NIIEngine 3.0.0
        */
        void add(OverlayView * obj);

        /** 移去图元视图
        @version NIIEngine 3.0.0
        */
        void remove(OverlayView * obj);

        /** 获取图元视图
        @version NIIEngine 3.0.0
        */
        OverlayView * get(OverlayGeoID id);

        /** 清除所有图元/节点
        @version NIIEngine 3.0.0
        */
        void clearAll();

        /** 设置平移因子
        @note 平面概念
        @version NIIEngine 3.0.0
        */
        void setOffset(NIIf x, NIIf y);

        /** 滚动
        @note 平面概念
        @version NIIEngine 3.0.0
        */
        void offset(NIIf xoft, NIIf yoft);

        /** 获取平移因子
        @note 平面概念
        @version NIIEngine 3.0.0
        */
        inline NIIf getXOffset() const                  { return mScrollX; }

        /** 获取平移因子
        @note 平面概念
        @version NIIEngine 3.0.0
        */
        inline NIIf getYOffset() const                  { return mScrollY; }

        /** 设置旋转
        @note 平面概念
        @version NIIEngine 3.0.0
        */
        void setRotate(const Radian & angle);

        /** 旋转偏移
        @note 平面概念
        @version NIIEngine 3.0.0
        */
        inline void rotate(const Radian & angleoft)     { setRotate(mRotate + angleoft); }

        /** 获取旋转
        @note 平面概念
        @version NIIEngine 3.0.0
        */
        inline const Radian & getRotate() const         { return mRotate; }

        /** 设置缩放因子.
        @note 平面概念
        @version NIIEngine 3.0.0
        */
        void setScale(NIIf x, NIIf y);

        /** 获取x缩放值
        @note 平面概念
        @version NIIEngine 3.0.0
        */
        inline NIIf getXScale() const                   { return mScaleX; }

        /** 获取y缩放值
        @note 平面概念
        @version NIIEngine 3.0.0
        */
        inline NIIf getYScale() const                   { return mScaleY; }

        /** 获取空间矩阵
        @version NIIEngine 3.0.0 高级api
        */
        void getSpaceMatrix(Matrix4f * xform) const;

        /** 获取层元
        @version NIIEngine 3.0.0
        */
        virtual OverlayGeo * getGeo(NIIf x, NIIf y);

        /** 层视图列表
        @version NIIEngine 3.0.0
        */
        inline const ViewList & getViewList() const     { return mViewList; }
    protected:
        /** 初始化
        @version NIIEngine 3.0.0
        */
        void init();

        /** 渲染队列
        @version NIIEngine 3.0.0
        */
        void renderImpl(Camera * cam, RenderQueue * queue);
    protected:
        OverlayID mID;
        String mSrc;
        ViewList mViewList;
        Viewport * mViewport;
        SpaceNode * mRootNode;
        Radian mRotate;
        NIIf mScrollX;
        NIIf mScrollY;
        NIIf mScaleX;
        NIIf mScaleY;        
        Matrix4f mSpace;
        Nidx16 mZOrder;
        bool mSpaceValid;
        bool mVisible;
        bool mInit;
    };
    
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
        OverlayView * getParent()               { return mParent;  }

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        OverlayGeoID getID() const              { return mID;  }

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
        bool isVisible() const                  { return mVisible;  }

        /** 设置是否启用
        @version NIIEngine 3.0.0
        */
        virtual void setEnable(bool b);

        /** 获取是否启用
        @version NIIEngine 3.0.0
        */
        bool isEnabled() const                  { return mEnabled;  }

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
        HAlign getXAlign() const                { return mHorzAlign; }

        /** 设置y对齐方式
        @version NIIEngine 3.0.0
        */
        void setYAlign(VAlign align);

        /** 获取y对齐方式
        @version NIIEngine 3.0.0
        */
        VAlign getYAlign() const                { return mVertAlign;  }

        /** 设置层等级
        @version NIIEngine 3.0.0
        */
        void setZOrder(Nui16 idx)               { mZOrder = idx;  }

        /** 获取层等级
        @version NIIEngine 3.0.0
        */
        Nui16 getZOrder() const                 { return mZOrder; }

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
        NIIf getUnitLeft() const                { return mLeft; }

        /** 设置左上角y轴位置(单位化)
        @param[in] top [0,1.0]
        @version NIIEngine 3.0.0
        */
        void setUnitTop(NIIf top);

        /** 获取左上角y轴位置(单位化)
        @return [0,1.0]
        @version NIIEngine 3.0.0
        */
        NIIf getUnitTop() const                 { return mTop;  }

        /** 设置宽度(单位化)
        @param[in] width [0,1.0]
        @version NIIEngine 3.0.0
        */
        void setUnitWidth(NIIf width);

        /** 获取宽度(单位化)
        @return [0,1.0]
        @version NIIEngine 3.0.0
        */
        NIIf getUnitWidth() const               { return mWidth; }

        /** 设置高度(单位化)
        @param[in] height [0,1.0]
        @version NIIEngine 3.0.0
        */
        void setUnitHeight(NIIf height);

        /** 获取高度(单位化)
        @return [0,1.0]
        @version NIIEngine 3.0.0
        */
        NIIf getUnitHeight() const              { return mHeight;  }

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
        NIIf getEnvWidth()                  { return mWidth; }

        /** 获取实际高度
        @version NIIEngine 3.0.0
        */
        NIIf getEnvHeight()                 { return mHeight;  }

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
        ResourceID getMaterialID() const    { return mMaterialID;  }

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
        virtual Nidx16 _notifyZOrder(Nidx16 order);

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
        void setTemplate(bool c)                {  mTemplate = c;  }

        /** 获取是否可模版化
        @version NIIEngine 3.0.0
        */
        bool isTemplate() const                 {  return mTemplate;  }

        /** 复制模版
        @version NIIEngine 3.0.0
        */
        virtual void setTemplate(OverlayGeo * obj);

        /** 获取复制模版
        @version NIIEngine 3.0.0
        */
        const OverlayGeo * getTemplate() const  { return mSrcTemplate; }

        /** 副本
        @version NIIEngine 3.0.0
        */
        virtual OverlayGeo * clone(OverlayGeoID newid);
    protected:
        /// @copydetails PropertyCmdObj::init
        virtual bool initCmd(PropertyCmdSet * dest);

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
        Nidx16 mZOrder;
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
    
    /** 表层面视图
    @version NIIEngine 3.0.0
    */
    class _EngineAPI OverlayView : public OverlayGeo
    {
    public:
        typedef map<OverlayGeoID, OverlayGeo *>::type ChildMap;
    public:
        OverlayView(OverlayGeoID id, ScriptTypeID stid = N_CmdObj_OverlayView, LangID lid = N_PrimaryLang);
        virtual ~OverlayView();

        /** 添加覆盖层几何
        @version NIIEngine 3.0.0
        */
        void add(OverlayGeo * obj);

        /** 移去覆盖层几何
        @version NIIEngine 3.0.0
        */
        void remove(OverlayGeoID id);

        /** 获取覆盖层几何
        @version NIIEngine 3.0.0
        */
        OverlayGeo * get(OverlayGeoID id);

        /** 获取覆盖层几何列表
        @version NIIEngine 3.0.0
        */
        inline const ChildMap & getList()           { return mChildren; }

        /** 设置是否检测子对象
        @version NIIEngine 3.0.0
        */
        inline void setChildPosCheck(bool b)        { mChildPosCheck = b; }

        /** 获取是否检测子对象
        @version NIIEngine 3.0.0
        */
        inline bool isChildPosCheck() const         { return mChildPosCheck; }

        using OverlayGeo::init;

        /// @copydetails OverlayGeo::init
        virtual void init();

        /// @copydetails OverlayGeo::_update
        virtual void _update();

        /// @copydetails OverlayGeo::_notifyPos
        virtual void _notifyPos();

        /// @copydetails OverlayGeo::_notifyZOrder
        virtual Nidx16 _notifyZOrder(Nidx16 level);

        /// @copydetails OverlayGeo::_notifyViewport
        virtual void _notifyViewport();

        /// @copydetails OverlayGeo::_notifyParent
        virtual void _notifyParent(OverlayView * parent, Overlay * overlay);

        /// @copydetails OverlayGeo::queue
        virtual void queue(RenderQueue * queue);

        /// @copydetails OverlayGeo::getGeo
        virtual OverlayGeo * getGeo(NIIf x, NIIf y) const;

        /// @copydetails OverlayGeo::setTemplate
        virtual void setTemplate(OverlayGeo * obj);

        /// @copydetails OverlayGeo::clone
        virtual OverlayGeo * clone(OverlayGeoID newid);
    protected:
        ChildMap mChildren;
        bool mChildPosCheck;
    };
}
#endif