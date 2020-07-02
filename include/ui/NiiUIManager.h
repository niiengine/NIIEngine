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
#ifndef _NII_UI_MANAGER_H_
#define _NII_UI_MANAGER_H_

#include "NiiUIPreInclude.h"
#include "NiiCommandObj.h"
#include "NiiGeometryPixel.h"
#include "NiiMatrix4.h"
#include "NiiSingleton.h"

using namespace NII::UI;

namespace NII
{
    /** UI单元管理器
    @remark 负责应用程序所有UI单元对象
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UIManager : public NII_COMMAND::CommandObj, public Singleton<UIManager>,
        public UIAlloc
    {
        friend class Engine;
    public:
        /** 同步渲染系统状态
        @version NIIEngine 3.0.0
        */
        void syn() const;

        /** 执行渲染
        @remark 渲染所有存在于机制里的UI单元对象
        @version NIIEngine 3.0.0
        */
        void render();

        /** 设置混合模式
        @remark UI图层一般是可见的
        @param[in] mode 模式
        @param[in] force 是否强制
        @version NIIEngine 3.0.0
        */
        void apply(PixelBlendMode mode, bool force = false);

        /** 设置是否启动UI渲染
        @version NIIEngine 3.0.0
        */
        void setEnable(bool b);

        /** 获取是否启动UI渲染
        @version NIIEngine 3.0.0
        */
        bool isEnable() const;

        /** 设置游标初始位置
        @version NIIEngine 3.0.0
        */
        void setCursorInitPos(const Vector2f & pos);  
        
        /** 获取游标初始位置
        @version NIIEngine 3.0.0
        */
        const Vector2f & getCursorInitPos() const;
        
        /** 创建UI画盘
        @param[out] out 所创建出的
        @version NIIEngine 3.0.0
        */
        UISheet * create(RenderSys * rsys, ViewWindow * win);

        /** 删除UI画盘
        @param[in] out 需要删除的
        @version NIIEngine 3.0.0
        */
        void destroy(UISheet * obj);

        /** 设置渲染体
        @param[in] obj 渲染视窗
        @version NIIEngine 3.0.0
        */
        void setWindow(UISheet * dest, ViewWindow * obj);

        /** 获取渲染体
        @param[in] obj 渲染视窗
        @version NIIEngine 3.0.0
        */
        void getWindow(UISheet * dest, ViewWindow *& obj);

        /** 获取窗体的画盘
        @param[in] dst
        @version NIIEngine 3.0.0
        */
        UISheet * getSheet(ViewWindow * dst) const;

        /** 获取主窗体画盘
        @return
            很可能只创建一个窗体,这时候这个窗体就是使用这个窗体画盘的,在独立的游戏设备
            或独立的游戏应用程序中很常用,如果是编辑级别的应用程序建议使用 getSheet 函
            数
        @version NIIEngine 3.0.0
        */
        UISheet * getMainSheet() const;

        /** 设置显示像素大小
        @version NIIEngine 3.0.0
        */
        void setDisplaySize(ViewWindow * dst, const PlaneSizei & size);

        /** 获取视窗显示像素大小
        @param[in] dst 视窗
        @version NIIEngine 3.0.0
        */
        PlaneSizei getDisplaySize(Nid id) const;

        /** 获取视窗显示像素大小
        @param[in] dst 视窗
        @version NIIEngine 3.0.0
        */
        PlaneSizei getDisplaySize(ViewWindow * dst) const;

        /** 着色程序模式
        @version NIIEngine 3.0.0
        */
        void setShaderEnable(bool b);

        /** 着色程序模式
        @version NIIEngine 3.0.0
        */
        bool isShaderEnable() const;

        /** 设置投影矩阵
        @version NIIEngine 3.0.0 高级api
        */
        void setProjMatrix(const Matrix4f & m);

        /** 获取投影矩阵
        @version NIIEngine 3.0.0
        */
        const Matrix4f & getProjMatrix() const;

        /** 设置视图矩阵
        @version NIIEngine 3.0.0 高级api
        */
        void setViewMatrix(const Matrix4f & m);

        /** 获取视图矩阵
        @version NIIEngine 3.0.0
        */
        const Matrix4f & getViewMatrix() const;

        /** 获取模型矩阵
        @version NIIEngine 3.0.0 高级api
        */
        void setModelMatrix(const Matrix4f & m);

        /** 获取模型矩阵
        @version NIIEngine 3.0.0
        */
        const Matrix4f & getModelMatrix() const;

        /** 获取空间矩阵
        @version NIIEngine 3.0.0
        */
        const Matrix4f & getSpaceMatrix() const;

        /** 设置文本视图
        @version NIIEngine 3.0.0
        */
        void setView(TextView * obj);

        /** 获取文本视图
        @version NIIEngine 3.0.0
        */
        TextView * getView() const;

        /// @copydetails Singleton::getOnly
        static UIManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static UIManager * getOnlyPtr();
    public:
        /** 当视图尺寸改变时
        @version NIIEngine 3.0.0
        */
        void onViewSize(ViewWindow * dst, const PlaneSizei & size);
    protected:
        UIManager(RenderSys * sys, ViewWindow * primary);
        ~UIManager();
    public:
        /** 创建单元时
        @version NIIEngine 3.0.0
        */
        void onCreate(const Widget * obj);

        /** 删除单元时
        @version NIIEngine 3.0.0
        */
        void onDestroy(const Widget * obj);

        /** 视图尺寸改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID ViewSizeChangeEvent;

        /** 文本视图改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID TextViewChangeEvent;

        /** 事件总数
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;

        NormalTextView * TextView0;        

        EffectTextView * TextView1;
    protected:
        typedef map<UISheet *, ViewWindow *>::type SheetList;
    protected:
        RenderSys * mSys;
        SheetList mSheetList;
        UISheet * mMainSheet;
        Viewport * mPreVP;
        Matrix4f mPreProjMatrix;
        TextView * mTextView;
        PixelBlendMode mBlendMode;
        Vector2f mCursorInitPos;
        Matrix4f mViewMatrix;           ///< 视图矩阵
        Matrix4f mProjMatrix;           ///< 投影矩阵
        Matrix4f mModelMatrix;          ///< 模型矩阵
        Matrix4f mSpaceMatrix;          ///< 空间矩阵
        HighLevelGpuProgram * mVP;
        HighLevelGpuProgram * mFP;
        GpuProgramParam * mVPParams;
        GpuProgramParam * mFPParams;

        bool mValidSpaceMatrix;
        bool mAloneFrame;
        bool mShaderEnable;
        bool mGLSLEnable;
        bool mEnable;
    };
}
#endif