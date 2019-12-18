/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

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

#ifndef _NII_IO_VALUE_MAP_H_
#define _NII_IO_VALUE_MAP_H_

#include "NiiPreInclude.h"

#if NII_ARCH_TYPE == N_ARCH_32
    #define IOTemp      Nui16
    #define IOMark      5
    #define IOValid     31
    #define IOByteCount 4
#elif NII_ARCH_TYPE == N_ARCH_64
    #define IOTemp      Nui16
    #define IOMark      6
    #define IOValid     63
    #define IOByteCount 8
#elif NII_ARCH_TYPE == N_ARCH_128
    #define IOTemp      Nui16
    #define IOMark      7
    #define IOValid     127
    #define IOByteCount 16
#elif NII_ARCH_TYPE == N_ARCH_256
    #define IOTemp      Nui16
    #define IOMark      8
    #define IOValid     255
    #define IOByteCount 32
#elif NII_ARCH_TYPE == N_ARCH_512
    #define IOTemp      Nui16
    #define IOMark      9
    #define IOValid     511
    #define IOByteCount 64
#elif NII_ARCH_TYPE == N_ARCH_1024
    #define IOTemp      Nui16
    #define IOMark      10
    #define IOValid     1023
    #define IOByteCount 128
#else
    #error 出现错误了
#endif

namespace NII
{
    /** 值映射
    @remark 用于表达事物是否存在,是否有效
    @version NIIEngine 3.0.0
    */
    class _EngineInner BitSet : public StreamAlloc
    {
    public:
        typedef struct _Com
        {
            IOType data;
            IOTemp mark;
        }Com;
    public:
        BitSet();
        BitSet(NCount count);
        ~BitSet();

        /** 分配大小
        @remark
            每次大小改变的时候都必须调用这个方法,每次调用这个方法都将清空现有内容没,
            这个对象不包含容量溢出检测,所以操作的时候必须确保操作范围在容量范围内
        @param[in] count IO个数
        */
        void resize(NCount count);

        /** 设置指定io位置的值为真
        @param[in] r 位置码
        @param[in] sr 范围
        */
        void add(NCount r, NCount sr);

        /** 设置指定io位置的值为假
        @param[in] r 位置码
        @param[in] sr 范围
        */
        void remove(NCount r, NCount sr);

        /** 获取指定io位置所处的范围内的IO真值个数
        @param[in] io位置
        @return 此位置前的所有真值个数
        */
        NCount get(NCount io);

        /** 获取指定范围内的IO真值个数
        @param[in] r 位置码
        @param[in] sr 范围
        @return 此位置前的所有真值个数
        */
        NCount get(NCount r, NCount sr);

        /** 检测指定io位置的值是否为真
        @param[in] io位置
        @return 真值状态
        */
        inline bool check(NCount io)
        {
            return (mData[io >> IOMark].data & NiiOrMark[io % IOValid]) != 0;
        }

        /** 检测指定io位置的值是否为真
        @param[in] r 位置码
        @param[in] sr 范围
        @return 真值状态
        */
        inline bool check(NCount r, NCount sr)
        {
            return (mData[r].data & NiiOrMark[sr]) != 0;
        }

        /** 设置指定io位置的值为真
        @param[in] io位置
        */
        inline void setTrue(NCount io)
        {
            mData[io >> IOMark].data |= NiiOrMark[io % IOValid];
        }

        /** 设置指定io位置的值为假
        @param[in] io位置
        */
        inline void setFalse(NCount io)
        {
            mData[io >> IOMark].data &= NiiNotMark[io % IOValid];
        }
    private:
        Com * mData;                    ///< 主内存
        NCount mCount;
    };
}
#endif