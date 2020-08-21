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

#ifndef _NII_FRAMEOBJ_H_
#define _NII_FRAMEOBJ_H_

#include "NiiPreInclude.h"
#include "NiiViewport.h"
#include "NiiFrameObjListener.h"
#include "NiiRenderSysData.h"
#include "NiiPixelFormat.h"
#include "NiiPixelBlock.h"

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
            M_SRGB  = 0x01,
            M_AUTO  = 0x02,
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
        FrameObj(FrameObjID id, const String & name = N_StrBlank);
        virtual ~FrameObj();

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        inline Nid getID() const                    { return mID;  }

        /** 获取辅助名字
        @version NIIEngine 3.0.0
        */
        inline const String & getName() const       { return mName; }

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
        virtual void fill(FaceType type, const Box & src, PixelBlock & out) = 0;

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
        inline void setFlushLevel(Nui16 level)          { mFlushLevel = level;  }

        /** 获取冲刷优先级
        @version NIIEngine 3.0.0
        */
        inline Nui16 getFlushLevel() const              { return mFlushLevel; }

        /** 设置光栅组
        @version NIIEngine 3.0.0
        */
        void setRasterGroup(GroupID gid);

        /** 获取光栅组
        @version NIIEngine 3.0.0
        */
        inline GroupID getRasterGroup() const           { return mRasterGroup; }

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

        /** 是否立体模式
        @version NIIEngine 3.0.0
        */
        virtual bool isStereo() const;

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
        inline ExtFrameBuffer * getExtBuffer() const    { return mExtBuffer;  }

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
        inline NCount getViewportCount() const          { return mViews.size(); }

        /** 获取秒帧数
        @version NIIEngine 3.0.0
        */
        inline const FramesPerSecond & getFPS() const   { return *mStats; }

        /** 获取三角形数量
        @version NIIEngine 3.0.0
        */
        inline NCount getTriangleCount() const          { return mTriCount;  }

        /** 获取渲染的批次数量
        @version NIIEngine 3.0.0
        */
        inline NCount getBatchCount() const             { return mBatchCount; }

        /** 获取多重采样设置
        @version NIIEngine 3.0.0
        */
        virtual Nui32 getFSAA() const;

        /** 获取多重采样设置
        @version NIIEngine 3.0.0
        */
        inline const String & getFSAAExt() const        { return mFSAAExt; }
    public:
        /** 通知对象已经不存在了
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _notifyDestroy(const Camera * obj);

        /** 通知对象已经不存在了
        @versioin NIIEngine  高级api
        */
        virtual void _notifyDestroy(const Viewport * obj);
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
        FrameObjID mID;
        String mName;
        Nui32 mWidth;
        Nui32 mHeight;
        Nui32 mColourDepth;
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
        bool mStereo;
    };
}
#endif