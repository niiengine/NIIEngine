/*
-----------------------------------------------------------------------------
面向游戏设计引擎

时间: 2012-1-15 

作者: zelda_ok

格式: open it use utf-8

主页: www.nii-zelda.tk

授权方式:商业授权(www.nii-zelda.tk/license)
------------------------------------------------------------------------------
*/

#ifndef NII_DESIGN_WINDOW_H
#define NII_DESIGN_WINDOW_H

#include "NII.h"

namespace NII
{
    /** 描述一个编辑器总界面
    @remark 一般的都是由主窗体控制全局,所以有必要的时候需要加入鼠标,键
        盘还有相关的事件更新机制
    */
    class _NIIExport DesignWindow
    {
    public :
        DesignWindow(const String & title);
        virtual ~DesignWindow();

        /// 加入渲染事件
        virtual void joinRenderEvent()=0;

        /// 加入NII的根事件更新
        virtual void joinRootEvent()=0;

        /// 加入设备输入更新
        virtual void joinDeviceEvent()=0;

        /// 加入自定义事件更新
        virtual void joinDefineEvent()=0;

        /// 加入本窗体的一些格外事件
        virtual void joinWindowEvent()=0;

        /** 创建主窗体的右工具栏
        @returns bool 返回是否创建成功
        */
        virtual bool generateRightSide()=0;

        /** 创建主窗体的左工具栏
        @returns bool 返回是否创建成功        
        */
        virtual bool generateLeftSide()=0;

        /** 创建主窗体的上工具栏
        @returns bool 返回是否创建成功        
        */
        virtual bool generateTopSide()=0;

        /** 创建主窗体的下工具栏
        @returns bool 返回是否创建成功        
        */
        virtual bool generateBottomSide()=0;

        /// 获取描述
        virtual String Description() = 0;
    protected:
        /// 生成工具栏
        virtual bool generateToolsBar()=0;
        /// 生成菜单栏
        virtual bool generateMenus()=0;
    protected:
        /// 主窗体窗体ID
        NIIui mWindowId;
        /// 窗体标题
        String mTitle;
        /// 窗体左上角坐标
        Vector<NIIi> mStart;
        /// 窗体宽度
        NIIui mWidth;
        /// 窗体高度
        NIIui mHeight;
        /// 有无窗框
        bool mFrame;
        /// 最大化
        bool mMax;
        /// 最小化
        bool mMin;
    }
}
#endif