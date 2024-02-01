﻿/*
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

#ifndef _NII_COMMON_H_
#define _NII_COMMON_H_

#include "NiiString.h"
#include "NiiHash.h"

#if defined (NII_GCC_VISIBILITY)
    #pragma GCC visibility push(default)
#endif

#include <utility>
#include <sstream>

#if defined (NII_GCC_VISIBILITY)
    #pragma GCC visibility pop
#endif

namespace NII
{
    // Deprecated
    _EngineAPI extern const Nui32  N32_1OrMark[32];     ///< { 0x01, 0x02, 0x04, 0x08, 0x10, ... }
    _EngineAPI extern const Nui32  N32_1AndMark[32];    ///< { 0x01, 0x03, 0x07, 0x0F, 0x1F, ... }
    _EngineAPI extern const Nui32  N32_1NotMark[32];    ///< { 0xFFFFFFFE, 0xFFFFFFFD, 0xFFFFFFFB, 0xFFFFFFF7, 0xFFFFFFEF, ... }
    
    // 64位类型对64位cpu的寄存器帮助大
    _EngineAPI extern const Nui64  N64_1OrMark[64];     ///< { 0x01, 0x02, 0x04, 0x08, 0x10, ... }
    _EngineAPI extern const Nui64  N64_1AndMark[64];    ///< { 0x01, 0x03, 0x07, 0x0F, 0x1F, ... }
    _EngineAPI extern const Nui64  N64_1NotMark[64];    ///< { 0xFFFFFFFFFFFFFFE, 0xFFFFFFFFFFFFFFD, 0xFFFFFFFFFFFFFFB, ... }
    _EngineAPI extern const Nui64  N64_16OrMark[4];     ///< { 0xFFFF, 0xFFFF0000, 0xFFFF00000000, 0xFFFF000000000000 }
    _EngineAPI extern const Nui64  N64_16AndMark[4];    ///< { 0xFFFF, 0xFFFFFFFF, 0xFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF }
    _EngineAPI extern const Nui64  N64_16NotMark[4];    ///< { 0xFFFFFFFFFFFF0000, 0xFFFFFFFF0000FFFF, 0xFFFF0000FFFFFFFF, 0xFFFFFFFFFFFF }
    
    _EngineAPI extern const EventID     EventCount;
    _EngineAPI extern const PropertyID  PropertyCount;
    _EngineAPI extern const StateID     StateCount;
    enum TextEncodeType
    {
        TET_ASCII,
        TET_UCS_2,
        TET_UCS_4,
        TET_UTF_8,  ///< 引擎默认使用
        TET_UTF_16
    };

    /** 阴影特征
    @version NIIEngine 3.0.0
    */
    enum ShadowFeture
    {
        SF_None = 0x00,                     ///<没有阴影
        SF_Additive = 0x01,                 ///<叠加类阴影
        SF_Modulate = 0x02,                 ///<调制类阴影
        SF_Fusion = 0x04,                   ///<混合类阴影
        SF_Stencil = 0x10,                  ///<模版类阴影
        SF_Texture = 0x20,                  ///<纹理阴影标志
        SF_Modulate_Stencil = 0x12,         ///<基于模版阴影技术,渲染所有阴影体积到一个调制结果
        SF_Additive_Stencil = 0x11,         ///<基于模版阴影技术,独立渲染每个阴影体积附加到一个结果中
        SF_Modulate_Texture = 0x22,         ///<基于纹理阴影技术,渲染投射纹理到一个调制通道
        SF_Additive_Texture = 0x21,         ///<基于纹理阴影技术,独立渲染每个阴影体积附加到一个结果中
        SF_Additive_Fusion_Texture = 0x25,  ///<基于SF_Additive_Texture的混合
        SF_Modulate_Fusion_Texture = 0x26   ///<基础SF_Modulate_Texture的混合
    };

    /** 比较函数,用于深度/模板缓存操作和其他
    @version NIIEngine 3.0.0
    */
    enum CmpMode
    {
        CPM_NONE = 0,                       ///< 不启动
        CPM_ALWAYS_FAIL = 1,                ///< 总是不通过
        CPM_LESS = 2,                       ///< 当小于时通过
        CPM_LESS_EQUAL = 3,                 ///< 当不大于时通过
        CPM_EQUAL = 4,                      ///< 当等于时通过
        CPM_NOT_EQUAL = 5,                  ///< 当不等于时通过
        CPM_GREATER = 6,                    ///< 当大于时通过
        CPM_GREATER_EQUAL = 7,              ///< 当不小于时通过
        CPM_ALWAYS_PASS = 8                 ///< 总是通过
    };
    
    _EngineAPI CmpMode reverse(CmpMode mode);

    /** 缩放形式
    @version NIIEngine 3.0.0
    */
    enum ScaleMode
    {
        SM_Unknow,                          ///<
        SM_Y,                               ///<
        SM_X,                               ///<
        SM_Min,                             ///<
        SM_Max,                             ///<
        SM_ALL                              ///<
    };

    /** 伸缩模式
    @version NIIEngine 3.0.0
    */
    enum AspectMode
    {
        AM_Stretch,
        AM_Shrink,
        AM_Expand,
        AM_None
    };

    /** 立体模式
    @version NIIEngine 4.0.0
    */
    enum StereoModeType
    {
        SMT_NONE        = 0,
        SMT_Stereo
    };

    /** 视口类型
    @version NIIEngine 4.0.0
    */
    enum ViewportType
    {
        VT_Back        = 0,
        VT_BackLeft,
        VT_BackRight
    };
    
    /** 面向类型
    @version NIIEngine 3.0.0
    */
    enum FaceSideType
    {
        FST_Front,
        FST_Back,
        FST_Both
    };

    /** 处理优先级
    @version NIIEngine 4.0.0
    */
    enum PrcLevel
    {
        PL_High         = 0,
        PL_Mid_High     = 1,
        PL_Mid          = 2,
        PL_Mid_Low      = 3,
        PL_Low          = 4,
        PL_Count        = 5
    };
    
    /** 文件路径结构
    @version NIIEngine 3.0.0
    */
    struct FileInfo
    {
        VFS * mSrc;             ///< 来源
        String mFileName;       ///< 文件名字
        String mPath;
        String mFullName;
        NCount mSize;           ///< 文件实际大小
        NCount mCompressSize;
        bool mMatchCase;        ///< 大小写匹配
    };
    typedef vector<FileInfo>::type FileInfoList;
    
    /** 平台特定数据
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI RenderSysData
    {
    public:
        enum Type
        {
            T_Unknow        = 0,
            T_DX9_0         = 0x01,
            T_DX10_0        = 0x02,
            T_DX11_0        = 0x04,
            T_DX12_0        = 0x08,
            T_OPENGL1_1     = 0x10,
            T_OPENGL2_1     = 0x20,
            T_OPENGL3_3     = 0x30,
            T_OPENGL4_3     = 0x40,
            T_OPENGL4_6     = 0x80,
            T_OPENGL_ES1_1  = 0x100,
            T_OPENGL_ES2_1  = 0x200,
            T_OPENGL_ES3_1  = 0x400,
            T_DIRECT_FB     = 0x800,

            T_WIN32         = 0x100000,
            T_WINRT         = 0x200000,     ///< win8编程框架
            T_WIN8_PHONE    = 0x400000,
            T_LINUX         = 0x800000,     ///< linux内核
            T_LINUX_X       = 0x1000000,    ///< x窗体级别的linux系统
            T_OSX           = 0x2000000,    ///< 苹果mac os x(darwin)
            T_IOS           = 0x4000000,    ///< 苹果iphone(darwin)
            T_ANDROID       = 0x8000000,    ///< 安桌系统(linux内核级)
            T_EMSCRIPTEN    = 0x10000000,   ///< web编程框架
            T_FREEBSD       = 0x20000000,   ///< freeBSD
        };
    public:
        /** 获取平台额外信息
        @param[in] name 需要获取的属性名字
        @param[in] data 指向存储获取内容的内存指针
        @return 是否获取成功
        @note 一般在最底端接口重写这个函数
        @version NIIEngien 3.0.0 高级api
        */
        virtual bool getPlatformData(const String & name, void * data) const;

        /** 获取平台额外信息
        @param[in] name 需要获取的属性名字
        @param[in] data 指向存储获取内容的内存指针
        @return 组合类型
        @note 一般在最底端接口重写这个函数
        @version NIIEngien 3.0.0 高级api
        */
        virtual Nmark getPlatformType() const;
    };

    /** 使用方法创建一系列名称顺序编号
    @version NIIEngine 3.0.0
    */
    class _EngineAPI NameGenerator
    {
    public:
        NameGenerator(const String & prefix) : mPrefix(prefix), mNext(1) {}
        NameGenerator(const NameGenerator & o) : mPrefix(o.mPrefix), mNext(o.mNext) {}

        /** 生成一个新名字
        @version NIIEngine 3.0.0
        */
        String gen()
        {
            N_mutex1_lock
            StringStream s;
            s << mPrefix << mNext++;
            return s.str();
        }

        /** 重制内部记数器
        @version NIIEngine 3.0.0
        */
        inline void reset()
        {
            N_mutex1_lock
            mNext = 1;
        }

        /** 手动设置内部记数器(使用警告)
        @version NIIEngine 3.0.0
        */
        inline void setNext(Nui32 val)
        {
            N_mutex1_lock
            mNext = val;
        }

        /** 获取内部记数器
        @version nIIEngine 3.0.0
        */
        inline Nui32 getNext() const
        {
            N_mutex1_lock
            return mNext;
        }
    protected:
        String mPrefix;
        Nui32 mNext;
        N_mutex1
    };

    /** 描述池的模板类
    @version NIIEngine 3.0.0
    */
    template <typename T> class Pool
    {
    public:
        Pool(){}
        virtual ~Pool(){}

        /**获取这个池的下个项目.
        @return pair 标识是个是否是一个自由项
            @version NIIEngine 3.0.0
        */
        virtual std::pair<bool, T> removeItem()
        {
            N_mutex1_lock
            std::pair<bool, T> ret;
            if (mItems.empty())
            {
                ret.first = false;
            }
            else
            {
                ret.first = true;
                ret.second = mItems.front();
                mItems.pop_front();
            }
            return ret;
        }

        /** 添加一个项目到池.
        @version NIIEngine 3.0.0
        */
        virtual void addItem(const T & i)
        {
            N_mutex1_lock
            mItems.push_front(i);
        }

        /** 清空池
        @version NIIEngine 3.0.0
        */
        virtual void clear()
        {
            N_mutex1_lock
            mItems.clear();
        }
    protected:
        typedef typename list<T>::type ItemList;
        ItemList mItems;
        N_mutex1
    };

    typedef map<String, bool>::type SingleArgList;
    typedef map<String, String>::type ComplexArgList;

    /** 获取命令行操作中单一"-b"和复合"-b 1.txt"参数,
    @param[in] args, argv 方法的参数
    @param[in] arg1 单一选项映射
    @param[in] arg2 复合选项映射
    */
    NIIi _EngineAPI findCmdArgs(NIIi args, Ntchar ** argv, SingleArgList & arg1, ComplexArgList & arg2);
}

#if (N_PLAT != N_PLAT_WIN32) && !defined(__GNUC__)  &&!defined(__GCCXML__)

#include <sys/time.h>
struct timezone
{
	int tz_minuteswest;
	int tz_dsttime;
};
#if defined(N_PLAT_WINRT)
struct timeval 
{
	long tv_sec;
	long tv_usec;
};
#endif
int gettimeofday(struct timeval *tv, struct timezone *tz);

#else
#include <time.h>

#endif
#endif