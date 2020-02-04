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

#ifndef _NII_NET_UNIQUE_ADDRESS_H_
#define _NII_NET_UNIQUE_ADDRESS_H_

#include "NiiNetPreInclude.h"

namespace NII
{
namespace NII_NET
{
        /** 网络唯一标识
    @remark 即使是使用同个IP同个端口, 在运行机制中同一个socket可能包含多个处理机制
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UniqueID : public NetAlloc
    {
	public:
        UniqueID();
        UniqueID(Nui64 g);

        UniqueID & operator = (const UniqueID & o);

        bool operator == (const UniqueID & o) const;
        bool operator != (const UniqueID & o) const;
        bool operator > (const UniqueID & o) const;
        bool operator < (const UniqueID & o) const;

		/**
		*/
        void write(String & out) const;

		/**
		*/
        bool read(const String & in);

		/**
		*/
        void write(NetSerializer * out) const;

		/**
		*/
        void read(const NetSerializer * in);

		/**
		*/
        static NCount size();

		/**
		*/
        static Nul toNum(const UniqueID & g);

        static const UniqueID INVALID;
	public:
        Nindex mIndex;
        Nui64 g;
    };
    
    class _EngineAPI UniqueAddress : public NetAlloc
    {
    public:
        UniqueAddress();

        UniqueAddress(Message * msg);

        UniqueAddress(const UniqueID & o);

        UniqueAddress(const Address * o);

        UniqueAddress(const UniqueAddress & o);

        ~UniqueAddress();
        
        UniqueAddress & operator = (const UniqueID & o);

        UniqueAddress & operator = (const Address & o);

        UniqueAddress & operator = (const UniqueAddress & o);

        bool operator == (const UniqueAddress & o) const;

		/**
		*/
		void write(String & out, bool port = true) const;

		/**
		*/
		void read(const String & in, bool port = true);

		/**
		*/
		bool isValid() const;

		/**
		*/
        static Nul toNum(const UniqueAddress & obj);
	public:
        UniqueID mID;
        Address * mAddress;
    };
}
}
#endif