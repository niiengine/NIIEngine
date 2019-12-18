/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-4

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

#ifndef _NII_PROPERTYDATA_H_
#define _NII_PROPERTYDATA_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 属性数据结构
    @note 一般由 XML JSON YAML 等可描述脚本序列化程序使用
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PropertyData : public DataAlloc
    {
    public:
        typedef map<PropertyID, void *>::type ExtPropertyList;
        typedef map<PropertyID, String>::type PropertyList;
    public:
        PropertyData();
        PropertyData(const PropertyData & o);
        virtual ~PropertyData();

        /** 添加属性
        @param pid 属性ID
        @param value 值(字符串)
        @version NIIEngine 3.0.0
        */
        void add(PropertyID pid, const String & value);

        /** 添加属性
        @param pid 属性ID
        @param value 值(字符串)
        @version NIIEngine 3.0.0
        */
        void add(PropertyID pid, const Nutf8 * value);

        /** 添加属性(补充)
        @param pid 属性ID
        @param value 值(数据指针)
        @note 不管理数据指针的有效性,没有副本产生
        @version NIIEngine 3.0.0 adv
        */
        void addVoid(PropertyID pid, void * value);

        /** 添加属性
        @param[in] o 另一个对象
        @version NIIEngine 3.0.0
        */
        void add(const PropertyData & o);

        /** 移去属性
        @param pid 属性ID
        @version NIIEngine 3.0.0
        */
        void remove(PropertyID pid);

        /** 是否存在属性
        @param pid 属性ID
        @version NIIEngine 3.0.0
        */
        bool isExist(PropertyID pid) const;

        /** 获取属性
        @param[in] idx 属性ID
        @version NIIEngine 3.0.0
        */
        PropertyID getID(Nidx idx) const;

        /** 获取值
        @param[in] idx 属性ID
        @version NIIEngine 3.0.0
        */
        const String & getStr(Nidx idx) const;

        /** 获取值
        @param pid 属性ID
        @param init 属性默认值
        @version NIIEngine 3.0.0
        */
        const String & get(PropertyID pid, const String & init = _T("")) const;

        /** 获取值
        @param[in] pid 属性ID
        @param[out] out 输出
        @param[in] init 属性默认值
        @version NIIEngine 3.0.0
        */
        bool get(PropertyID pid, bool & out, bool init = false) const;

        /** 获取值
        @param[in] pid 属性ID
        @param[out] out 输出
        @param[in] init 属性默认值
        @version NIIEngine 3.0.0
        */
        bool get(PropertyID pid, Ni32 & out, Ni32 init = 0) const;

        /** 获取值
        @param[in] pid 属性ID
        @param[out] out 输出
        @param[in] cnt 输出个数
        @version NIIEngine 3.0.0
        */
        bool get(PropertyID pid, Ni32 * out, NCount cnt) const;

        /** 获取值
        @param[in] pid 属性ID
        @param[out] out 输出
        @param[in] init 属性默认值
        @version NIIEngine 3.0.0
        */
        bool get(PropertyID pid, Nui32 & out, Nui32 init = 0) const;

        /** 获取值
        @param[in] pid 属性ID
        @param[out] out 输出
        @param[in] cnt 输出个数
        @version NIIEngine 3.0.0
        */
        bool get(PropertyID pid, Nui32 * out, NCount cnt) const;

        /** 获取值
        @param[in] pid 属性ID
        @param[out] out 输出
        @param[in] init 属性默认值
        @version NIIEngine 3.0.0
        */
        bool get(PropertyID pid, NIIf & out, NIIf init = 0.0f) const;

        /** 获取值
        @param[in] pid 属性ID
        @param[out] out 输出
        @param[in] cnt 输出个数
        @version NIIEngine 3.0.0
        */
        bool get(PropertyID pid, NIIf * out, NCount cnt) const;

        /** 获取值(补充)
        @param[in] pid 属性ID
        @param[out] out 输出
        @param[in] init 属性默认值
        @version NIIEngine 3.0.0 adv
        */
        bool getVoid(PropertyID pid, void *& out, void * init = 0) const;

        /** 获取值
        @param[in] pid 属性ID
        @param[out] out 输出
        @param[in] init 属性默认值
        @version NIIEngine 3.0.0
        */
        bool get(LangID lid, ScriptTypeID tid, PropertyID pid, PropertyID & out, PropertyID init = 0) const;

        /** 属性数量
        @version NIIEngine 3.0.0
        */
        NCount getCount() const;

        /** 获取属性值列表
        @version NIIEngine 3.0.0 高级api
        */
        const PropertyList & getList() const;
    protected:
        PropertyList mPropertys;
        ExtPropertyList mExtPropertys;
    };
}
#endif