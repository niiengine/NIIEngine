/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-5

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

#ifndef _NII_CLIPBOARD_H_
#define _NII_CLIPBOARD_H_

#include "NiiPreInclude.h"
#include "NiiString.h"

namespace NII
{
    /** 剪切数据
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ClipboardData : public DataAlloc
    {
    public:
        virtual ~ClipboardData();

        /** 设置内容
        @version NIIEngine 3.0.0
        */
        virtual void set(const String & mime, void * data, NCount size) = 0;

        /** 获取内容
        @version NIIEngine 3.0.0
        */
        virtual void get(String & mime, void *& data, NCount & size) = 0;
    };

    /** 剪切器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Clipboard : public DataAlloc
    {
    public:
        Clipboard();
        ~Clipboard();

        /** 设置数据
        @param[in] size 字节大小
        @version NIIEngine 3.0.0
        */
        void setData(const String & mime, const Nui8 * buffer, NCount size);

        /** 获取数据
        @param[out] size 字节大小
        @version NIIEngine 3.0.0
        */
        void getData(String & mime, const Nui8 *& buffer, NCount & size);

        /** 设置剪切内容
        @version NIIEngine 3.0.0
        */
        void set(ClipboardData * src);

        /** 获取剪切内容
        @version NIIEngine 3.0.0
        */
        void get(ClipboardData *& src) const;

        /** 设置文本
        @remark 简易方式 text/plain 数据
        @version NIIEngine 3.0.0
        */
        void setText(const String & text);

        /** 获取文本
        @remark 简易方式 text/plain 数据
        @version NIIEngine 3.0.0
        */
        String getText();
    private:
        Nui8 * mBuffer; 
        String mMimeType;
        NCount mCount;
        ClipboardData * mData;
    };
}
#endif