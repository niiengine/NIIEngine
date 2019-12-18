/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-7

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

#ifndef _NII_FRAMEOBJ_H_
#define _NII_FRAMEOBJ_H_

#include "NiiPreInclude.h"
#include "NiiViewport.h"
#include "NiiFramesPerSecond.h"
#include "NiiRenderSysData.h"
#include "NiiPixelFormat.h"

namespace NII
{
    /** 设备结果(视口中的)帧
    @remark 一个冲刷结果,和摄像机,视口,帧缓存有关系
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FrameObj : public RenderSysData, public FrameAlloc
    {
        friend class RenderSys;
    public:
        /** 运行模式
        @version NIIEngine 3.0.0
        */
        enum Mode
        {
            M_SRGB = 0x01,
            M_AUTO = 0x02,
            M_VSYNC = 0x04
        };

        /** 面类型
        @remark 和顶点绕向有关系
        @version NIIEngine 3.0.0
        */
        enum FaceType
        {
            FT_Unknow,
            FT_Front,
            FT_Back
        };
    public:
        FrameObj(Nid id, const String & name = N_StrBlank);
        virtual ~FrameObj();

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        Nid getID() const;

        /** 冲刷缓存
        @param[in] swap 交换帧缓存
        @version NIIEngine 3.0.0
        */
        virtual void flush(bool swap = true);

        /** 交换帧缓存
        @remark 当前帧和处理中的帧缓存交换
        @param[in] vsync 同步垂直
        @version NIIEngine 3.0.0
        */
        virtual void swap(bool vsync = true);

        /** 复制当前渲染目标内容到一个像素盒
        @version NIIEngine 3.0.0
        */
        virtual void fill(FaceType type, PixelFormat pf, PixelBlock & out) = 0;

        /** 重置帧速率统计
        @version NIIEngine 3.0.0
        */
        void resetFPS();

        /** 添加监听器
        @version NIIEngine 3.0.0
        */
        virtual void add(FrameObjListener * obj);

        /** 移去监听器
        @version NIIEngine 3.0.0
        */
        virtual void remove(FrameObjListener * obj);

        /** 移去这个实例所有的监听
        @version NIIEngine 3.0.0
        */
        virtual void removeAllListener();

        /** 是否自动冲刷缓存
        @version NIIEngine 3.0.0
        */
        void setAutoFlush(bool b);

        /** 是否自动冲刷缓存
        @version NIIEngine 3.0.0
        */
        bool isAutoFlush() const;

        /** 设置冲刷优先级
        @version NIIEngine 3.0.0
        */
        void setFlushLevel(Nui16 fl);

        /** 获取冲刷优先级
        @version NIIEngine 3.0.0
        */
        Nui16 getFlushLevel() const;

        /** 设置光栅组
        @version NIIEngine 3.0.0
        */
        void setRasterGroup(GroupID gid);

        /** 获取光栅组
        @version NIIEngine 3.0.0
        */
        GroupID getRasterGroup() const;

        /** 用于设置渲染目标活动状态
        @version NIIEngine 3.0.0
        */
        virtual void setActive(bool b);

        /** 用于检索或设置渲染目标活动状态
        @version NIIEngine 3.0.0
        */
        virtual bool isActive() const;

        /** 第一个渲染窗体
        @remark 第一个渲染窗体可能包含了一个主的设备器,主设备器共享渲染系统资源
        @version NIIEngine 3.0.0
        */
        virtual bool isFirstWindow() const;

        /** 是否硬件sRGB
        @version NIIEngine 3.0.0
        */
        virtual bool isHardwareSRGB() const;

        /** 是否翻转
        @remark D3D用了左上角作为原点，而OpenGL用了左下角
        @version NIIEngine 3.0.0
        */
        virtual bool isTextureFlipping() const;

        /** 获取渲染对象的宽度
        @version NIIEngine 3.0.0
        */
        virtual Nui32 getWidth() const;

        /** 获取渲染对象的高度
        @version NIIEngine 3.0.0
        */
        virtual Nui32 getHeight() const;

        /** 获取渲染颜色深度
        @version NIIEngine 3.0.0
        */
        virtual Nui32 getColourDepth() const;

        /** 输出缓存格式
        @version NIIEngine 3.0.0
        */
        virtual PixelFormat getFormat() const;

        /** 使用扩展帧缓存
        @version NIIEngine 3.0.0
        */
        virtual bool attachExt(ExtFrameBuffer * ext);

        /** 分离扩展帧缓存
        @version NIIEngine 3.0.0
        */
        virtual void detachExt();

        /** 获取当前使用的帧缓存
        @version NIIEngine 3.0.0
        */
        ExtFrameBuffer * getExtBuffer() const;

        /** 创建视口
        @param[in] cam 视口使用的摄象机
        @param[in] x 视口在渲染窗体的x坐标
        @param[in] y 视口在渲染窗体的y坐标
        @version NIIEngine 3.0.0
        */
        virtual Viewport * createViewport(ViewportID id, Camera * cam, NIIf x = 0.0f, 
            NIIf y = 0.0f, NIIf width = 1.0f, NIIf height = 1.0f, NIIi z = 0);

        /** 获取视口
        @param[in] index 下标
        @version NIIEngine 3.0.0
        */
        virtual Viewport * getViewport(Nidx index);

        /** 移去视口
        @param[in] index 下标
        @version NIIEngine 3.0.0
        */
        void destroyViewport(Nidx index);

        /** 移去视口
        @version NIIEngine 3.0.0
        */
        void destroyViewport(Viewport * v);

        /** 移去这个目标的所有视口.
        @version NIIEngine 3.0.0
        */
        void destroyAllViewport();

        /** 返回视口总数
        @version NIIEngine 3.0.0
        */
        NCount getViewportCount() const;

        /** 获取秒帧数
        @version NIIEngine 3.0.0
        */
        const FramesPerSecond & getFPS() const;

        /** 获取三角形数量
        @version NIIEngine 3.0.0
        */
        NCount getTriangleCount() const;

        /** 获取渲染的批次数量
        @version NIIEngine 3.0.0
        */
        NCount getBatchCount() const;

        /** 获取多重采样设置
        @version NIIEngine 3.0.0
        */
        virtual Nui32 getFSAA() const;

        /** 获取多重采样设置
        @version NIIEngine 3.0.0
        */
        const String & getFSAAExt() const;

        /** 获取辅助名字
        @version NIIEngine 3.0.0
        */
        const String & getName() const;
    public:
        /** 通知对象已经不存在了
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _notifyDestroy(const Camera * obj);

        /** 通知对象已经不存在了
        @versioin NIIEngine  高级api
        */
        virtual void _notifyDestroy(const Viewport * obj);
        
        /** 通知对象已经不存在了
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _notifyDestroy(const ExtFrameBuffer * obj);
    protected:
        /** 更新所有视口
        @version NIIEngine 3.0.0 高级api
        */
        virtual void updateImpl();

        /** 冲刷视口前
        @version NIIEngine 3.0.0 高级api
        */
        virtual void preFlush();

        /** 刷新视口
        @version NIIEngine 3.0.0 高级api
        */
        virtual void flushImpl(Viewport * obj);

        /** 刷新所有自动视口
        @version NIIEngine 3.0.0 高级api
        */
        virtual void flushAutoImpl();

        /** 冲刷视口后
        @version NIIEngine 3.0.0 高级api
        */
        virtual void endFlush();
    protected:
		static Nid NextID;
        typedef map<NIIi, Viewport *>::type Views;
        typedef vector<FrameObjListener *>::type Listeners;
    protected:
        Nid mID;                    ///< ID
        String mName;               ///< 辅助名字
        Nui32 mWidth;               ///< 宽度
        Nui32 mHeight;              ///< 高度
        Nui32 mColourDepth;         ///< 深度
        Nui32 mFSAA;
        GroupID mRasterGroup;       
        FramesPerSecond * mStats;
        ExtFrameBuffer * mExtBuffer;
        Views mViews;
        Listeners mListeners;
        NCount mTriCount;
        NCount mBatchCount;
        Nmark mMark;
        String mFSAAExt;
        Nui16 mFlushLevel;          ///< 渲染目标的优先级
        bool mActive;               ///< 是否活动
    };
}
#endif