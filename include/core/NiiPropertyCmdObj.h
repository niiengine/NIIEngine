/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-12-16

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

#ifndef _NII_PropertyCmdObj_H_
#define _NII_PropertyCmdObj_H_

#include "NiiPreInclude.h"
#include "NiiPropertyCmd.h"
#include "NiiPropertyData.h"
#include "NiiScriptProperty.h"
#include "NiiString.h"

namespace NII
{
    /** 属性控制集
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PropertyCmdSet
    {
        friend class PropertyCmdObj;
    public:
        PropertyCmdSet(PropertyCmdObj * own);
        ~PropertyCmdSet();

        /** 添加属性控制
        @param[in] cmd 属性控制
        @note 参数的内存将由这个类来管理, 使用 N_new 创建
        @version NIIEngine 3.0.0
        */
        void add(PropertyCmd * cmd);

        /** 移去属性
        @param[in] pid 属性控制的ID
        @version NIIEngine 3.0.0
        */
        void remove(PropertyID pid);

        /** 是否存在属性控制
        @version NIIEngine 3.0.0
        */
        bool isExist(PropertyID pid)const;

        /** 获取属性控制
        @version NIIEngine 3.0.0
        */
        PropertyCmd * get(PropertyID pid) const;

        /** 获取属性控制
        @version NIIEngine 3.0.0
        */
        PropertyCmd * get(const String & pid) const;

        /** 获取列表
        @version NIIEngine 3.0.0 高级api
        */
        PropertyCmdMap & getList();

        /** 获取列表
        @version NIIEngine 3.0.0 高级api
        */
        const PropertyCmdMap & getList() const;

        /** 移去所有属性控制
        @version NIIEngine 3.0.0
        */
        void removeAll();
    protected:
        PropertyCmdSet();
    protected:
        PropertyCmdObj * mOwn;
        PropertyCmdMap mCmdMap;
    };
    typedef map<ClassID, PropertyCmdSet>::type CmdCollectMap;

    /** 字符串控制对象
    @remark
        这个类比较合适于驱动配置脚本,不适合用于 EventObj, CommandObj 里的机制驱动,
        因为任意类型/字符串操作并非高效的
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PropertyCmdObj : public ScriptProperty
    {
    public:
        PropertyCmdObj(ScriptTypeID stid, LangID lid = N_PrimaryLang);
        virtual ~PropertyCmdObj();

        /** 通用多参数设置方法
        @param params 参数集
        @version NIIEngine 3.0.0
        */
        virtual void set(const PropertyData & params);

        /** 设置属性值
        @note 必须明确知道所设置的属性是什么类型
        @note 此函数比PropertyID慢
        @version NIIEngine 3.0.0
        */
        virtual bool set(const String & pname, const String & value);

        /** 设置属性值
        @note 必须明确知道所设置的属性是什么类型,输入参数是类型实体的指针
        @note 此函数比PropertyID慢
        @version NIIEngine 3.0.0
        */
        virtual bool set(const String & pname, const void * out);

        /** 设置属性值
        @note 必须明确知道所设置的属性是什么类型
        @version NIIEngine 3.0.0
        */
        virtual bool set(PropertyID pid, const String & value);

        /** 设置属性值
        @note 必须明确知道所设置的属性是什么类型,输入参数是类型实体的指针
        @version NIIEngine 3.0.0
        */
        virtual bool set(PropertyID pid, const void * out);

        /** 获取属性值
        @note 必须明确知道所获取的属性是什么类型
        @version NIIEngine 3.0.0
        */
        virtual bool get(PropertyID pid, String & value) const;

        /** 获取属性值
        @note 必须明确知道所获取的属性是什么类型,输出参数是类型实体的指针
        @version NIIEngine 3.0.0
        */
        virtual bool get(PropertyID pid, void * out) const;

        /** 创建属性值混合
        @note 必须明确知道所获取的属性是什么类型,输出参数是新构建类型实体的指针
        @version NIIEngine 3.0.0
        */
        virtual FusionNum * create(PropertyID pid);

        /** 复制属性
        @param[in] src 属性来源
        @version NIIEngine 3.0.0
        */
        virtual void copyProperty(PropertyCmdObj * src);

        /** 为这个类,检索参数目录
        @version NIIEngine 3.0.0
        */
        PropertyCmdSet * getCmdObj() const;

        /** 注入属性控制
        @version NIIEngine 3.0.0
        */
        void login();

        /** 注销属性控制
        @version NIIEngine 3.0.0
        */
        void logout();

        /** 进入应用模式
        @version NIIEngine 3.0.0
        */
        static void enterApp();

        /** 进入设计模式
        @version NIIEngine 3.0.0
        */
        static void enterDesign();

        /** 清除所有属性控制对象
        @version NIIEngine 3.0.0
        */
        static void cleanAll();
    protected:
        PropertyCmdObj();

        /** 建立字符串控制
        @version NIIEngine 3.0.0 高级api
        */
        virtual bool setupCmd(ClassID cid);

        /** 初始化参数命令
        @version NIIEngine 3.0.0 高级api
        */
        virtual bool init(PropertyCmdSet * dest);
    protected:
        ClassID mCmdObjID;
        PropertyCmdSet * mCmdCollecter;
        N_static_mutex(mCmdCollectMapMutex);
        static CmdCollectMap mCmdCollectMap;
    };
}
#endif