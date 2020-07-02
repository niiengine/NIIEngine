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
#ifndef _NII_UI_SHEET_H_
#define _NII_UI_SHEET_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidgetEventArgs.h"
#include "NiiUICursor.h"
#include "NiiGeometryPixel.h"
#include "NiiCommandObj.h"
#include "NiiKeyBoardControlItem.h"
#include "NiiMatrix4.h"

using namespace NII::NII_COMMAND;

namespace NII
{
namespace UI
{
    /** UI画盘上下文
    @version NIIEngine 3.0.0
    */
    struct SheetContext : public UIAlloc
    {
        const Window * mRoot;   ///<     
        UISheet * mSheet;       ///< 
        UIInstance * mFace;     ///< 
        GeometryLevel mQueue;   ///<         
        Vector2f mPosOffset;    ///< 
    };

    /** 几何图层事件函数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GeometryLevelEventArgs : public EventArgs
    {
    public:
        GeometryLevelEventArgs(GeometryLevel gl);

        GeometryLevel mGL;
    };
    
    /** UI窗体渲染界面盘
    @remark 负责渲染UI窗体
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UISheet : public CommandObj, public UIAlloc
    {
        friend class UIManager;
    public:
        UISheet(RenderSys * rs, ViewWindow * win);

        ~UISheet();

        /** 应用状态模式
        @note 应用状态模式后,所绘制的东西的位置/投影设置才符合UI机制本生
        @version NIIEngine 3.0.0 高级api
        */
        void apply();

        /** 冲刷缓存
        @version NIIEngine 3.0.0
        */
        void flush();

        /** 重新绘制
        @version NIIEngine 3.0.0
        */
        void redraw();

        /** 是否重新绘制
        @version NIIEngine 3.0.0
        */
        bool isRedraw() const;

        /** 更新状态
        @param[in] cost 上次调用与这次调用渡过的时间
        @version NIIEngine 3.0.0
        */
        void update(TimeDurMS cost);

        /** 获取监听到的掩码
        @version NIIEngine 3.0.0
        */
        Nmark getWinComKey() const;

        /** 创建UI画盘
        @version NIIEngine 3.0.0
        */
        void create(UIInstance *& out);

        /** 删除UI画盘
        @version NIIEngine 3.0.0
        */
        void destroy(UIInstance * obj);

        /** 附加UI画盘
        @note 内存不属于这个对象管理
        @version NIIEngine 3.0.0
        */
        void attach(UIInstance * obj);

        /** 解除UI画盘
        @note 内存不属于这个对象管理
        @version NIIEngine 3.0.0
        */
        void detach(UIInstance * obj);

        /** 添加几何像素
        @param[in] gl 几何层
        @param[in] obj 几何像素
        @version NIIEngine 3.0.0
        */
        void add(GeometryLevel gl, GeometryPixel * obj);

        /** 移去几何像素
        @param[in] gl 几何层
        @param[in] obj 几何像素
        @version NIIEngine 3.0.0
        */
        void remove(GeometryLevel gl, GeometryPixel * obj);

        /** 移去指定几何层的几何像素
        @param[in] gl 几何层
        @version NIIEngine 3.0.0
        */
        void clear(GeometryLevel gl);

        /** 移去所有几何像素
        @version NIIEngine 3.0.0
        */
        void clear();

        /** 添加窗体
        @note 内存不属于这个对象管理
        @version NIIEngine 3.0.0
        */
        void add(Window * obj);

        /** 移去窗体
        @note 内存不属于这个对象管理
        @verison NIIEngine 3.0.0
        */
        void remove(Window * ob);

        /** 设置显示区域
        @version NIIEngine 3.0.0
        */
        void setArea(const Recti & area);

        /** 获取显示区域
        @version NIIEngine 3.0.0
        */
        const Recti & getArea() const;

        /** 返回当前等待关闭窗体
        @version NIIEngine 3.0.0 高级api
        */
        void setModal(Window * obj);

        /** 返回当前等待关闭窗体
        @version NIIEngine 3.0.0
        */
        Window * getModal() const;

        /** 设置当前活动的窗体
        @version NIIEngine 3.0.0 高级api
        */
        void setActive(Window * obj);

        /** 获取当前活动的窗体
        @version NIIEngine 3.0.0
        */
        Window * getActive() const;

        /** 设置当前对焦单元
        @note getActive 的一个子系
        @version NIIEngine 3.0.0 高级api
        */
        void setFocus(Widget * obj);

        /** 获取当前对焦单元
        @note getActive 的一个子系
        @version NIIEngine 3.0.0
        */
        Widget * getFocus() const;

        /** 设置游标区域
        @version NIIEngine 3.0.0
        */
        void setCursorArea(const PlaneSizef & sz);

        /** 获取游标区域
        @version NIIEngine 3.0.0
        */
        const PlaneSizef & getCursorArea() const;

        /** 是否启用游标多重点击
        @version NIIEngine 3.0.0
        */
        void setCursorMultiClick(bool b);

        /** 获取是否游标多重点击
        @version NIIEngine 3.0.0
        */
        bool getCursorMultiClick() const;

        /** 游标移动系数
        @version NIIEngine 3.0.0
        */
        void setCursorMoveFactor(NIIf f);

        /** 游标移动系数
        @version NIIEngine 3.0.0
        */
        NIIf getCursorMoveFactor() const;

        /** 设置游标点击时间阀值
        @note 超过阀值则不会产生点击事件
        @version NIIEngine 3.0.0
        */
        void setCursorClickFactor(TimeDurMS time);

        /** 获取游标点击时间阀值
        @note 超过阀值则不会产生点击事件
        @version NIIEngine 3.0.0
        */
        TimeDurMS getCursorClickFactor() const;

        /** 设置游标多重点击时间阀值
        @note 超过阀值则不会产生点击事件
        @version NIIEngine 3.0.0
        */
        void setCursorMultiClickFactor(TimeDurMS time);

        /** 获取游标多重点击时间阀值
        @note 超过阀值则不会产生点击事件
        @version NIIEngine 3.0.0
        */
        TimeDurMS getCursorMultiClickFactor() const;

        /** 设置默认字体对象
        @remark
            默认字体.如果UI单元并没有自己的字体属性,则使用这个字体.
        @param[in] 字体对象
        @version NIIEngine 3.0.0 高级api
        */
        void setFont(Font * obj);

        /** 获取默认字体对象
        @remark
            默认字体.如果UI单元并没有自己的字体属性,则使用这个字体.
        @param[out] 字体对象
        @version NIIEngine 3.0.0 高级api
        */
        Font * getFont() const;

        /** 设置默认的提示框对象
        @remark
            如果UI单元并没有自己的提示框属性,则使用这个提示框.
        @param[in] obj 提示框对象
        @version NIIEngine 3.0.0 高级api
        */
        void setTooltip(Tooltip * obj, bool autoDestroy = true);

        /** 获取默认的提示框对象
        @remark
            如果UI单元并没有自己的提示框属性,则使用这个提示框.
        @remark[out] obj 提示框对象
        @version NIIEngine 3.0.0 高级api
        */
        Tooltip * getTooltip() const;

        /** 获取游标对象
        @note 触屏设备一般不存在这个概念
        @version NIIEngine 3.0.0
        */
        Cursor * getCursor() const;

        /** 刷新覆盖
        @version NIIEngine 3.0.0
        */
        void resetCursorOver();

        /** 获取光标覆盖的窗体
        @note 游标移动时可能会有变化
        @version NIIEngine 3.0.0
        */
        Widget * getCursorOver();

        /** 设置视图窗体
        @version NIIEngine 3.0.0 高级api
        */
        void setWindow(ViewWindow * obj);

        /** 获取视图窗体
        @version NIIEngine 3.0.0 高级api
        */
        ViewWindow * getWindow() const;

        /** 获取两者共同的容器对象
        @version NIIEngine 3.0.0
        */
        Container * getAncestor(Widget * w1, Widget * w2) const;

        /** 仿射变换
        @param[in] m 实际空间
        @param[in] in 输入
        @param[in] out 输出
        @version NIIEngine 3.0.0
        */
        void affine(const Matrix4f * m, const Vector2f & in, Vector2f & out) const;
    public:
        /** 按键按下时
        @version NIIEngine 3.0.0
        */
        bool onButtonDown(NII_MEDIA::MouseButton button);

        /** 按键释放时
        @version NIIEngine 3.0.0
        */
        bool onButtonUp(NII_MEDIA::MouseButton button);

        /** 游标移动时
        @version NIIEngine 3.0.0
        */
        bool onCursorMove(NIIi dx, NIIi dy);

        /** 游标拖拽时
        @version NIIEngine 3.0.0
        */
        bool onCursorDrag(NIIi delta);

        /** 游标按下时
        @version NIIEngine 3.0.0
        */
        bool onCursorTouch(NII_MEDIA::MouseButton button);

        /** 游标按下时
        @version NIIEngine 3.0.0
        */
        bool onCursorMultiTouch(NII_MEDIA::MouseButton button, NCount count);

        /** 键盘按下时
        @version NIIEngine 3.0.0
        */
        bool onKeyDown(NII_MEDIA::KeyValue key);

        /** 键盘释放按下时
        @version NIIEngine 3.0.0
        */
        bool onKeyUp(NII_MEDIA::KeyValue key);

        /** 文本输入时
        @version NIIEngine 3.0.0
        */
        bool onChar(Nui32 chr);

        /** 执行复制时
        @version NIIEngine 3.0.0
        */
        bool onObjCopy();

        /** 执行剪切时
        @version NIIEngine 3.0.0
        */
        bool onObjCut();

        /** 执行粘贴时
        @version NIIEngine 3.0.0
        */
        bool onObjPaste();
    protected:
        /** 获取当前位置有效的UI单元
        @param[in] pos 位置
        @param[in] disable 即使禁用也有效
        @version NIIEngine 3.0.0
        */
        Widget * getDest(const Vector2f & pos, bool disable) const;

        /** 获取当前对焦
        @version NIIEngine 3.0.0
        */
        Widget * getForce() const;

        /** 矩阵
        @version NIIEngine 3.0.0
        */
        void updateMatrix();

        /** 视口
        @version NIIEngine 3.0.0
        */
        void updateViewport();

        /** 主字体改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onFont(const FontEventArgs * args);

        /** 视图窗体改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onWindowChange(const RenderSheetEventArgs * args);

        /** 游标区域改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onCursorAreaChange(const SheetEventArgs * args);

        /** 游标移动系数改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onCursorMoveFactorChange(const SheetEventArgs * args);

        /** 游标单击时间阀值改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onCursorClickFactorChange(const SheetEventArgs * args);

        /** 游标多重点击时间阀值改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onCursorMultiClickFactorChange(const SheetEventArgs * args);
    protected:
        /** 字体迭代函数
        @version NIIEngine 3.0.0
        */
        void FontChangeCB(Widget * obj) const;

        /** 删除单元函数
        @version NIIEngine 3.0.0
        */
        void DestroyCB(const Widget * obj);
    public:
        /** 渲染每个队列前事件
        @version NIIEngine 3.0.0
        */
        static const EventID PreRenderEvent;

        /** 渲染每个队列后事件
        @version NIIEngine 3.0.0
        */
        static const EventID EndRenderEvent;

        /** 字体改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID FontEvent;

        /** 视图窗体改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID WindowChangeEvent;

        /** 游标区域改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID CursorAreaChangeEvent;

        /** 游标移动因子改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID CursorMoveFactorChangeEvent;

        /** 游标单击时间阀值改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID CursorClickFactorChangeEvent;

        /** 游标多重点击时间阀值改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID CursorMultiClickFactorChangeEvent;

        /** 事件总数
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        typedef map<GeometryLevel, GeometryQueue>::type Queues;
        typedef vector<UIInstance *>::type Childs;
        typedef vector<Window *>::type WinList;
    protected:
        RenderSys * mSys;
        Viewport * mView;
        ViewWindow * mTarget;
        Window * mModal;                    ///< 模态窗体
        Window * mActive;                   ///< 活动窗体
        Widget * mFocus;                    ///< 活动窗体子系
        Font * mFont;
        Tooltip * mTooltip;
        Widget * mOverlay;
        Queues mRenderQueue;
        WinList mWinList;
        Childs mChild;
        Cursor * mCursor;
        CursorMultiTouch * mCursorTouch;
        Nui32 mWinComKey;
        Matrix4f mProjectMatrix;
        PlaneSizef mCursorArea;
        NIIf mViewExtent;
        Recti mArea;

        NIIf mCursorMoveFactor;
        TimeDurMS mCursorClickFactor;
        TimeDurMS mCursorMultiClickFactor;

        bool mRedraw;
        bool mValidView;
        bool mAutoTooltip;
        bool mValidOverlay;
        bool mValidProjectMatrix;
        bool mGenCursorMultiClick;
    };
}
}
#endif