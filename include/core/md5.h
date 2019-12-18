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

#ifndef _NII_MD5_H_
#define _NII_MD5_H_

#include "NiiPreInclude.h"
#include "NiiString.h"

namespace NII
{
    struct MD5Context 
    {
        Nui32 buf[4];
        Nui32 bytes[2];
        Nui32 in[16];
    };

    class Md5
    {
    public:
        Md5(void);
        ~Md5(void);
        void append(const Nui8 * inBuf, size_t inLen);
        void append(const String & str);
        void getDigest(Nui8 digest[16]);
        void getDigest(String & digest);
    
        /*! \brief Get the MD5 digest of the given text
        \param text text to compute the MD5 for
        \return MD5 digest
        */
        static String GetMD5(const String &text);
	private:
        MD5Context m_ctx;
    };
}

#endif
