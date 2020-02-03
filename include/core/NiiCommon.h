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
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
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
    static const Nui32  NiiOrMark[32];
    static const Nui32  NiiAndMark[32];
    static const Nui32  NiiNotMark[32];
    static const EventID EventCount;
    static const PropertyID PropertyCount;

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
        CPM_EQUAL = 1,                      ///< 当等于时通过
        CPM_NOT_EQUAL = 2,                  ///< 当不等于时通过
        CPM_ALWAYS_FAIL = 3,                ///< 总是不通过
        CPM_ALWAYS_PASS = 4,                ///< 总是通过
        CPM_GREATER_EQUAL = 5,              ///< 当不小于时通过
        CPM_GREATER = 6,                    ///< 当大于时通过
        CPM_LESS = 7,                       ///< 当小于时通过
        CPM_LESS_EQUAL = 8,                 ///< 当不大于时通过
        CPM_NONE = 9                        ///< 任何情况都通过
    };

    /** 缩放形式
    @version NIIEngine 3.0.0
    */
    enum ScaleMode
    {
        SM_Unknow,                            ///<
        SM_Y,                                ///<
        SM_X,                                ///<
        SM_Min,                                ///<
        SM_Max,                                ///<
        SM_ALL                                ///<
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
        CBT_BACK        = 0,
        CBT_BACK_LEFT,
        CBT_BACK_RIGHT
    };

    /** 处理优先级
    @version NIIEngine 4.0.0
    */
    enum PrcLevel
    {
        PL_High,
        PL_Mid_High,
        PL_Mid,
        PL_Mid_Low,
        PL_Low
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
        void reset()
        {
            N_mutex1_lock
            mNext = 1;
        }

        /** 手动设置内部记数器(使用警告)
        @version NIIEngine 3.0.0
        */
        void setNext(Nui32 val)
        {
            N_mutex1_lock
            mNext = val;
        }

        /** 获取内部记数器
        @version nIIEngine 3.0.0
        */
        Nui32 getNext() const
        {
            N_mutex1_lock
            return mNext;
        }
    protected:
        String mPrefix;        ///< 命名前缀
        Nui32 mNext;        ///< 下一个记数
        N_mutex1            ///< 锁
    };

    /** 描述一个简单的事物池的模板类
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
#endif
