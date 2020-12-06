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

#ifndef _NII_IO_VALUE_MAP_H_
#define _NII_IO_VALUE_MAP_H_

#include "NiiPreInclude.h"
#include "NiiCommon.h"

#if N_ARCH == N_ARCH_32
    #define IOTemp      Nui16
    #define IOMark      5
    #define IOValid     32
    #define IOByteCount 4
#elif N_ARCH == N_ARCH_64
    #define IOTemp      Nui16
    #define IOMark      6
    #define IOValid     64
    #define IOByteCount 8
#elif N_ARCH == N_ARCH_128
    #define IOTemp      Nui32
    #define IOMark      7
    #define IOValid     128
    #define IOByteCount 16
#elif N_ARCH == N_ARCH_256
    #define IOTemp      Nui32
    #define IOMark      8
    #define IOValid     256
    #define IOByteCount 32
#elif N_ARCH == N_ARCH_512
    #define IOTemp      Nui32
    #define IOMark      9
    #define IOValid     512
    #define IOByteCount 64
#elif N_ARCH == N_ARCH_1024
    #define IOTemp      Nui32
    #define IOMark      10
    #define IOValid     1024
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
        typedef struct _IOCell
        {
            IOType data;
            IOTemp mark;
        }IOCell;
    public:
        BitSet();
        BitSet(NCount count);
        ~BitSet();

        /** 分配大小
        @remark
            每次大小改变的时候都必须调用这个方法,每次调用这个方法都将清空现有内容,
            这个对象不包含容量溢出检测,所以操作的时候必须确保操作范围在容量范围内
        @param[in] count IO个数
        */
        void resize(NCount count);

        /** 设置指定io位置的值为真
        @param[in] io位置(0开始)
        */
        void setTrue(Nidx io);
        
        /** 设置指定io位置的值为真
        @param[in] r 位置码
        @param[in] sr 范围
        */
        void setTrue(Nidx r, Nidx sr);

        /** 设置指定io位置的值为假
        @param[in] io位置(0开始)
        */
        void setFalse(Nidx io);
        
        /** 设置指定io位置的值为假
        @param[in] r 位置码
        @param[in] sr 范围
        */
        void setFalse(Nidx r, Nidx sr);
        
        /** 检测指定io位置的值是否为真
        @param[in] io位置(0开始)
        @return 真值状态
        */
        inline bool isTrue(Nidx io)
        {
            return (mData[io >> IOMark].data & NiiOrMark[io % IOValid]) != 0;
        }

        /** 检测指定io位置的值是否为真
        @param[in] r 位置码
        @param[in] sr 范围
        @return 真值状态
        */
        inline bool isTrue(Nidx r, Nidx sr)
        {
            return (mData[r].data & NiiOrMark[sr]) != 0;
        }

        /** 获取指定[0-io]位置所处的范围内最高有效位(0开始)
        @param[in] io位置(0开始)
        @return 如果不存在，则返回 -1 值
        */
        NCount getMostSignificant(Nidx io);
        
        /** 获取指定范围内的IO最高有效位(0开始)
        @param[in] r 位置码
        @param[in] sr 范围
        @return 如果不存在，则返回 -1 值
        */
        NCount getMostSignificant(Nidx r, Nidx sr);
        
        /** 获取指定[0-io]位置所处的范围内的IO真值个数
        @param[in] io位置(结束)
        @return 此位置前的所有真值个数
        */
        NCount getCount(Nidx io);

        /** 获取指定范围内的IO真值个数
        @param[in] r 位置码
        @param[in] sr 范围
        @return 此位置前的所有真值个数
        */
        NCount getCount(Nidx r, Nidx sr);
    private:
        IOCell * mData;                    ///< 主内存
        NCount mCount;
    };
}
#endif