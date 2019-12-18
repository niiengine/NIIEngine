/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-7-8

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

#ifndef _NII_TEMPBUFFER_H_
#define _NII_TEMPBUFFER_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 静态缓存
    @version NIIEngine 3.0.0
    */
    template <NCount size> class TempBuffer
    {
    public:
        TempBuffer()
        {
            mValid = 0;
            mPos = 0;
        }

        /** 缓存数据
        @note 只保留不大于缓存大小的数据。
        @version NIIEngine 3.0.0
        */
        NCount cacheData(const void * data, NCount cnt)
        {
            if(cnt < size)
            {
                if(cnt + mValid <= size)
                {
                    memcpy(mBuffer + mValid, data, cnt);
                    mValid += cnt;
                }
                else
                {
                    NCount begOff = cnt - (size - mValid);
                    memmove(mBuffer, mBuffer + begOff, mValid - begOff);
                    memcpy(mBuffer + size - cnt, data, cnt);
                    mValid = size;
                }
                mPos = mValid;
                return cnt;
            }
            else
            {
                memcpy(mBuffer, (const Nui8 *)data + cnt - size, size);
                mValid = mPos = size;
                return size;
            }
        }
        /** 从缓存中读取数据
        @version NIIEngine 330.0
        */
        NCount read(void * data, NCount cnt) const
        {
            NCount rb = avail();
            rb = (rb < cnt) ? rb : cnt;
            memcpy(data, mBuffer + mPos, rb);
            mPos += rb;
            return rb;
        }

        /** 倒回
        @version NIIEngine 3.0.0
        */
        bool rewind(NCount cnt)
        {
            if (mPos < cnt)
            {
                clear();
                return false;
            }
            else
            {
                mPos -= cnt;
                return true;
            }
        }

        /** 跳转
        @version NIIEngine 3.0.0
        */
        bool skip(NCount cnt)
        {
            if (avail() < cnt)
            {
                clear();
                return false;
            }
            else
            {
                mPos += cnt;
                return true;
            }
        }

        /** 剩余缓存大小
        @version NIIEngine 3.0.0
        */
        NCount avail() const
        {
            return mValid - mPos;
        }

        /** 清除缓存
        @version NIIEngine 3.0.0
        */
        void clear()
        {
            mValid = 0;
            mPos = 0;
        }
    protected:
        Nui8 mBuffer[size];
        NCount mValid;
        mutable NCount mPos;
    };
}
#endif