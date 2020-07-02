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

#ifndef _NII_UI_INSTANCE_H_
#define _NII_UI_INSTANCE_H_

#include "NiiUIPreInclude.h"
#include "NiiGeometryPixel.h"
#include "NiiCommandObj.h"
#include "NiiQuaternion.h"
#include "NiiMatrix4.h"
#include "NiiRect.h"

namespace NII
{
namespace UI
{
    /** 独立渲染画盘
    @remark 独立渲染画盘,可以让UI元素产生独立效果
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UIInstance : public NII_COMMAND::CommandObj, public UIAlloc
    {
        friend class UISheet;
    public:
        /** 构造函数
        @param[in] rs 必要
        @param[in] own 必要
        @param[in] oft Sheet坐标中的偏移, 可以设置成 Vector2f::ZERO
        @version NIIEngine 3.0.0 高级api
        */
        UIInstance(RenderSys * rs, UISheet * own, const Vector2f & oft);

        ~UIInstance();

        /** 创建子画盘
        @param[out] obj 创建出来的子画盘
        @version NIIEngine 3.0.0
        */
        void create(UIInstance *& obj);

        /** 删除子画盘
        @param[in] obj 要删除的子画盘
        @version NIIEngine 3.0.0
        */
        void destroy(UIInstance * obj);

        /** 附加子画盘
        @note 内存外部管理
        @param[in] obj 子画盘
        @version NIIEngine 3.0.0
        */
        void attach(UIInstance * obj);

        /** 解除子画盘
        @note 内存外部管理
        @param[in] obj 子画盘
        @version NIIEngine 3.0.0
        */
        void detach(UIInstance * obj);

        /** 添加几何对象
        @param[in] gl 几何层
        @param[in] obj 几何对象
        @version NIIEngine 3.0.0
        */
        void add(GeometryLevel gl, GeometryPixel * obj);

        /** 移去几何对象
        @param[in] gl 几何层
        @param[in] obj 几何对象
        @version NIIEngine 3.0.0
        */
        void remove(GeometryLevel gl, GeometryPixel * obj);

        /** 移去所有几何对象
        @param[in] gl 几何层
        @version NIIEngine 3.0.0
        */
        void clear(GeometryLevel gl);

        /** 移去所有几何对象
        @version NIIEngine 3.0.0
        */
        void clear();

        /** 更新时间机制
        @remark 时间机制总与当前状态有关
        @param cost 上次与当前调用渡过的时间
        @version NIIEngine 3.0.0
        */
        void update(TimeDurMS cost);

        /** 执行渲染机制
        @version NIIEngine 3.0.0
        */
        void flush();

        /** 刷新渲染机制
        @version NIIEngine 3.0.0
        */
        void refresh();

        /** 重构渲染机制
        @version NIIEngine 3.0.0
        */
        void rebuild();

        /** 是否刷新渲染机制
        @version NIIEngine 3.0.0
        */
        inline bool isRefresh() const { return mRefresh;  }

        /** 设置裁减区域
        @param[in] area 区域
        @version NIIEngine 3.0.0
        */
        void setClip(const Recti & area);

        /** 设置显示区域
        @param[in] area 区域
        @version NIIEngine 3.0.0
        */
        void setArea(const Rectf & area);

        /** 获取显示区域
        @version NIIEngine 3.0.0
        */
        inline const Rectf & getArea() const { return mArea;  }

        /** 设置位置
        @version NIIEngine 3.0.0
        */
        void setPos(const Vector2f & pos);

        /** 获取位置
        @version NIIEngine 3.0.0
        */
        const Vector2f & getPos() const;

        /** 设置相对尺寸
        @note 相对于界面画盘
        @version NIIEngine 3.0.0
        */
        void setSize(const PlaneSizef & size);

        /** 获取相对尺寸
        @version NIIEngine 3.0.0
        */
        inline const PlaneSizef & getSize() const { return mSize; }

        /** 设置原始尺寸
        @version NIIEngine 3.0.0
        */
        void setNativeSize(const PlaneSizef & size);

        /** 设置旋转原点
        @version NIIEngine 3.0.0
        */
        void setOrigin(const Vector3f & org);

        /** 获取旋转原点
        @version NIIEngine 3.0.0
        */
        inline const Vector3f & getOrigin() const { return mOrigin; }

        /** 旋转角度
        @version NIIEngine 3.0.0
        */
        void setRotation(const Quaternion & rot);

        /** 旋转角度
        @version NIIEngine 3.0.0
        */
        inline const Quaternion & getRotation()const { return mRot;  }

        /** 应用渲染效果
        @version NIIEngine 3.0.0
        */
        void setEffect(RenderEffect * obj) { mBuffer->setEffect(obj);  }

        /** 获取所应用的效果
        @version NIIEngine 3.0.0
        */
        inline RenderEffect * getEffect() const { return mBuffer->getEffect();  }

        /** 主UI盘
        @version NIIEngine 3.0.0
        */
        UISheet * getOwner() const;

        /** 主画盘
        @note 如果不存在附属关系,可能是 NULL 对象
        @version NIIEngine 3.0.0
        */
        UIInstance * getRel() const;

        /** 纹理目标
        @version NIIEngine 3.0.0
        */
        Texture * getTexture() const;

        /** 仿射位置
        @param[in] in 原位置
        @param[out] out 实际位置
        @version NIIEngine 3.0.0
        */
        void affine(const Vector2f & in, Vector2f & out);
    public:
        /** 附加子画盘时触发
        @version NIIEngine 3.0.0
        */
        static const EventID AttachEvent;

        /** 移除子画盘时触发
        @version NIIEngine 3.0.0
        */
        static const EventID DetachEvent;

        /** 渲染队列前触发
        @version NIIEngine 3.0.0
        */
        static const EventID PreRenderEvent;

        /** 渲染队列后触发
        @version NIIEngine 3.0.0
        */
        static const EventID EndRenderEvent;

        static const EventID EventCount;
    protected:
        /** 附加事件
        @version NIIEngine 3.0.0
        */
        void onAttach(const EventArgs * args);

        /** 移除事件
        @version NIIEngine 3.0.0
        */
        void onDetach(const EventArgs * args);

        /** 更新矩阵
        @version NIIEngine 内部api
        */
        void updateMatrix();

        /** 更新视口
        @version NIIEngine 内部api
        */
        void updateViewport();

        /** 仿射空间
        @param[in] m 实际空间位置
        @param[in] in 原位置
        @param[out] out 实际位置
        @version NIIEngine 内部api
        */
        void affine(const Matrix4f * m, const Vector2f & in, Vector2f & out) const;
    protected:
        typedef map<GeometryLevel, GeometryQueue *>::type GeoQueues;
        typedef vector<UIInstance *>::type ChildList;
    protected:
        RenderSys * mSys;
        Viewport * mView;
        UISheet * mParent;
        UIInstance * mRel;
        Texture * mDst;
        GeometryPixel * mBuffer;
        ChildList mChild;
        GeoQueues mRenderQueue;
        Matrix4f mProjectMatrix;
        NIIf mViewExtent;
        Rectf mArea;
        Vector2f mPos;
        Vector2f mOft;
        Quaternion mRot;
        PlaneSizef mSize;
        Vector3f mOrigin;
        bool mValidProjectMatrix;
        bool mValidView;
        bool mValidBuffer;
        bool mRefresh;
    };
}
}
#endif