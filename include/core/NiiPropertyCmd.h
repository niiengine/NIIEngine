/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-12-24

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

#ifndef _NII_PROPERTY_CMD_H_
#define _NII_PROPERTY_CMD_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 属性控制
    @remark
        可用于属性定义型脚本,也就是 json, xml, css 类型的脚本,不含概解析型编程语言
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PropertyCmd : public StreamAlloc
    {
        friend class PropertyCmdObj;
        friend class PropertyCmdSet;
    public:
        /** 构造函数
        @param[in] id 属性ID,在引擎内部使用
        @param[in] verbose 细节描述,在设计模式下可以看见的辅助描述
        @version NIIEngine 3.0.0
        */
        PropertyCmd(PropertyID id, const String & verbose);

        /** 构造函数
        @param[in] id 属性ID,在引擎内部使用
        @param[in] name 属性名字,在脚本上出现的属性标识
        @param[in] verbose 细节描述,在设计模式下可以看见的辅助描述
        @version NIIEngine 3.0.0
        */
        PropertyCmd(PropertyID id, const String & name, const String & verbose);

        virtual ~PropertyCmd();

        /** 获取属性ID
        @version NIIEngine 3.0.0
        */
        PropertyID getID() const;

        /** 初始化控制对象
        @version NIIEngine 3.0.0
        */
        virtual void init(PropertyCmdObj * own) const;

        /** 获取值(字符串)
        @version NIIEngine 3.0.0
        */
        virtual void get(const PropertyCmdObj * own, String & out) const = 0;

        /** 获取值(数据指针)
        @param[out] out 已经实例化的指针
        @version NIIEngine 3.0.0 高级api
        */
        virtual void get(const PropertyCmdObj * own, void * out) const;

        /** 设置值(字符串)
        @version NIIEngine 3.0.0
        */
        virtual void set(PropertyCmdObj * own, const String & in) = 0;

        /** 设置值(数据指针)
        @param[in] in 已经实例化的指针
        @version NIIEngine 3.0.0 高级api
        */
        virtual void set(PropertyCmdObj * own, const void * in);

        /** 创建数值混合对象
        @version NIIEngine 3.0.0 高级api
        */
        virtual void create(PropertyCmdObj * own, FusionNum *& out) const;

        /** 获取默认值(字符串)
        @version NIIEngine 3.0.0
        */
        virtual void getDefault(String & out) const = 0;

        /** 获取描述
        @version NIIEngine 3.0.0
        */
        const String & getVerbose() const;

        /** 是否处于默认状态
        @version NIIEngine 3.0.0
        */
        bool isDefault(const PropertyCmdObj * own) const;

        /** 是否可读取
        @version NIIEngine 3.0.0
        */
        virtual bool isReadable() const;

        /** 是否可写入
        @version NIIEngine 3.0.0
        */
        virtual bool isWritable() const;

        /** 数据存储化
        @version NIIEngine 3.0.0
        */
        virtual bool isSerialize() const;

        /** 复制
        @version NIIEngine 3.0.0
        */
        virtual PropertyCmd * clone() const = 0;

        /** 导出到XML存储数据
        @version NIIEngine 3.0.0
        */
        virtual void write(const PropertyCmdObj * obj, XmlSerializer * dst) const;
    public:
        static const String PropertyAttribute;
        static const String NameAttribute;
        static const String ValueAttribute;
    protected:
        PropertyID mID;
        String mName;
        String mVerbose;
    };
    typedef map<PropertyID, PropertyCmd *>::type PropertyCmdMap;
}
#endif