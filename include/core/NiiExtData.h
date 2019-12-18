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

#ifndef _NII_EXTDATA_H_
#define _NII_EXTDATA_H_

#include "NiiPreInclude.h"
#include "NiiString.h"

namespace NII
{
    /** 扩展数据
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ExtData
    {
    public:
        ExtData();
        virtual ~ExtData();

        /** 设置在这个类实例的任何种类用户对象
        @param[in] data 数据指针(仅支持结构型,不支持对象型数据)
        @param[in] autodestroy 是否自动释放 data 数据
        @param[in] type 内存构造类型
        @version NIIEngine 3.0.0
        */
        void setExtData(void * data, bool autodestroy = true);

        /** 获取在这个类实例的无关键的任何种类用户对象
        @version NIIEngine 3.0.0
        */
        void * getExtData() const;

        /** 设置在这个类实例的任何种类用户对象
        @param[in] key 映射ID
        @param[in] data 使用 N_alooc 创建的数据
        @param[in] autodestroy 是否自动释放 data 数据
        @param[in] type 内存构造类型
        @version NIIEngine 3.0.0
        */
        void setExtData(Nid key, void * data, bool autodestroy = true);

        /** 获取在这个类实例的有关键的任何种类用户对象
        @param[in] key 映射ID
        @version NIIEngine 3.0.0
        */
        void * getExtData(Nid key) const;

        /** 设置在这个类实例的任何种类用户对象
        @param[in] key 映射ID
        @param[in] value 输入字符串
        @version NIIEngine 3.0.0
        */
        void setExtData(Nid key, const String & value);

        /** 获取在这个类实例的有关键的任何种类用户对象
        @param[in] key 映射ID
        @param[out] value 输出字符串
        @version NIIEngine 3.0.0
        */
        void getExtData(Nid key, String & value) const;

        /** 获取第一个数据
        @version NIIEngine 3.0.0
        */
        void * getFirst();

        /** 获取第二个数据
        @version NIIEngine 3.0.0
        */
        void * getSecond();

        /** 获取第三个数据
        @version NIIEngine 3.0.0
        */
        void * getThird();

        /** 清除所有数据
        @version NIIEngine 3.0.0
        */
        void clear();
    protected:
        typedef map<Nid, std::pair<void *, bool> >::type DataList;
        typedef map<Nid, String>::type DataStrList;
    private:
        DataList mDataList;
        DataStrList mStringList;
    };
}
#endif