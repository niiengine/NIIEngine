/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-8

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

#ifndef _NII_NET_SECURE_H_
#define _NII_NET_SECURE_H_

#include "NiiPreInclude.h"

namespace NII
{
namespace NII_NET
{
	/** 链接限制
	@version NIIEngine 3.0.0
	*/
    class _EngineAPI Ban : public NetAlloc
    {
        class Data;
    public:
        Ban();
        ~Ban();
        
        /** 添加
        @version NIIEngine 3.0.0
        */
        void add(const String & ip, TimeDurMS time = 0);
        
        /** 移去
        @version NIIEngine 3.0.0
        */
        void remove(const String & ip);
        
        /** 是否存在
        @version NIIEngine 3.0.0
        */
        bool isExist(const String & ip);
        
        /** 移去所有
        @version NIIEngine 3.0.0
        */
        void removeAll();
    private:
        vector<Data *>::type Bans;
	private:
        ThreadMutex mMutex;
        Bans mBans;
    };
    
    /** 链接安全
	@note 表示层概念
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Secure : public NetAlloc
    {
        class Data;
    public:
        Secure();
        ~Secure();

        /** 设置是否启动安全
        @version NIIEngine 3.0.0
        */
        void setSecure(bool b);

        /** 获取是否启动安全
        @version NIIEngine 3.0.0
        */
        bool isSecure() const;

        /** 初始化
        @param[in] publicKey
        @param[in] privateKey
        @param[in] clientKey
        @version NIIEngine 3.0.0
        */
        bool init(const Nui8 * publicKey, const Nui8 * privateKey, bool clientKey = false);
        
        /** 添加
        @version NIIEngine 3.0.0
        */
        void add(const String & ip);
        
        /** 删除
        @version NIIEngine 3.0.0
        */
        void remove(const String & ip);
        
        /** 是否存在
        @version NIIEngine 3.0.0
        */
        bool isExist(const String & ip);
        
        /** 移去所有
        @version NIIEngine 3.0.0
        */
        void removeAll();
    private:
        typedef list<String>::type IPList;
    private:
        Data * mData;
        ThreadRWMutex mMutex;
        IPList mIPList;
    };
}
}
#endif