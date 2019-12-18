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
        void setDisplaySize(ViewWindow * dst, const PlaneSizef & size);

        /** 获取视窗显示像素大小
        @param[in] dst 视窗
        @version NIIEngine 3.0.0
        */
        PlaneSizef getDisplaySize(Nid id) const;

        /** 获取视窗显示像素大小
        @param[in] dst 视窗
        @version NIIEngine 3.0.0
        */
        PlaneSizef getDisplaySize(ViewWindow * dst) const;

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
        void onViewSize(ViewWindow * dst, const PlaneSizef & size);
    protected:
        UIManager();
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
        typedef map<UISheet *, ViewWindow *>::type Sheets;
    protected:
        RenderSys * mSys;
        Sheets mSheets;
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