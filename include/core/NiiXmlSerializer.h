/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-8-26

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
#ifndef _NII_XMLSerializer_H_
#define _NII_XMLSerializer_H_

#include "NiiPreInclude.h"
#include "NiiStrConv.h"
#include "NiiXmlAnalyzeList.h"

namespace NII
{
    /** 创建Xml文件.
    @code
    #include <iostream>

    int main()
    {
        XmlSerializer xml(&std::cout, 4);
        xml.begin("Object")
            .attribute("Type", "objecttype")
            .attribute("Name", "Objectname")
                .begin("Property")
                    .attribute("Name", "Object_property_name")
                    .text("This is a test")
                .end()
                .begin("Property2")
                    .attribute("Name", "Object_property2_name")
                    .attribute("Type", "Object_property2_type")
                    .begin("Property21")
                        .attribute("Name", "Object_property21_name")
                        .attribute("Value", "Object_property21_type")
                    .end()
                .end()
          .end();

       if (xml.isValid())
       {
           std::cout << "success" << std::endl;
       }
       return 0;
    }
    @endcode
    */
    class _EngineAPI XmlSerializer : public ScriptAlloc
    {
    public:
        XmlSerializer(Ntostream * out, NCount indentSpace = 4);
		XmlSerializer(Ntostream * out, const ScriptProperty * obj, NCount indent = 4);
		~XmlSerializer();

		/** 开始新的Tag节点
		@param name Tag节点单元ID
		@return 当前操作的Tag节点
		*/
		XmlSerializer & begin(XmlUnitID uid);

        /** 开始新的Tag节点
        @param name Tag节点名字
		@return 当前操作的Tag节点
        */
        XmlSerializer & begin(const String & name);

        /** 结束当前Tag节点
        @return 当前操作的Tag节点
        */
        XmlSerializer & end();

		/** 添加属性
		@param name 属性ID
		@param value 属性值
		@return 当前操作的Tag节点
		*/
		XmlSerializer & attribute(PropertyID pid, const String & value);

        /** 添加属性
        @param name 属性名字
        @param value 属性值
        @return 当前操作的Tag节点
        */
        XmlSerializer & attribute(const String & name, const String & value);

		/** 添加属性
		@param name 属性ID
		@param value 属性值
		@return 当前操作的Tag节点
		*/
		XmlSerializer & attribute(PropertyID pid, bool value)
		{
			return attribute(pid, StrConv::conv(value));
		}

		/** 添加属性
		@param name 属性名字
		@param value 属性值
		@return 当前操作的Tag节点
		*/
		inline XmlSerializer & attribute(const String & name, bool value)
		{
			return attribute(name, StrConv::conv(value));
		}

		/** 添加属性
		@param name 属性ID
		@param value 属性值
		@return 当前操作的Tag节点
		*/
		inline XmlSerializer & attribute(PropertyID pid, Nui16 value)
		{
			return attribute(pid, StrConv::conv(value));
		}

		/** 添加属性
		@param name 属性名字
		@param value 属性值
		@return 当前操作的Tag节点
		*/
		inline XmlSerializer & attribute(const String & name, Nui16 value)
		{
			return attribute(name, StrConv::conv(value));
		}

		/** 添加属性
		@param name 属性ID
		@param value 属性值
		@return 当前操作的Tag节点
		*/
		inline XmlSerializer & attribute(PropertyID pid, Ni16 value)
		{
			return attribute(pid, StrConv::conv(value));
		}

		/** 添加属性
		@param name 属性名字
		@param value 属性值
		@return 当前操作的Tag节点
		*/
		inline XmlSerializer & attribute(const String & name, Ni16 value)
		{
			return attribute(name, StrConv::conv(value));
		}

		/** 添加属性
		@param name 属性ID
		@param value 属性值
		@return 当前操作的Tag节点
		*/
		inline XmlSerializer & attribute(PropertyID pid, Nui32 value)
		{
			return attribute(pid, StrConv::conv(value));
		}

		/** 添加属性
		@param name 属性名字
		@param value 属性值
		@return 当前操作的Tag节点
		*/
		inline XmlSerializer & attribute(const String & name, Nui32 value)
		{
			return attribute(name, StrConv::conv(value));
		}

		/** 添加属性
		@param name 属性ID
		@param value 属性值
		@version NIIEngine 3.0.0
		*/
		inline XmlSerializer & attribute(PropertyID pid, Ni32 value)
		{
			return attribute(pid, StrConv::conv(value));
		}

		/** 添加属性
		@param name 属性名字
		@param value 属性值
		@version NIIEngine 3.0.0
		*/
		inline XmlSerializer & attribute(const String & name, Ni32 value)
		{
			return attribute(name, StrConv::conv(value));
		}

		/** 添加属性
		@param name ID
		@param value 属性值
		@version NIIEngine 3.0.0
		*/
		inline XmlSerializer & attribute(PropertyID pid, NIIf value)
		{
			return attribute(pid, StrConv::conv(value));
		}

		/** 属性节点
		@param name 属性名字
		@param value 属性值
		@version NIIEngine 3.0.0
		*/
		inline XmlSerializer & attribute(const String & name, NIIf value)
		{
			return attribute(name, StrConv::conv(value));
		}

        /** 添加文本节点
        @param text 节点的文本
        @return 当前操作的Tag节点
		@version NIIEngine 3.0.0
        */
        XmlSerializer & text(const String & text);

        /** 获取当前Tag节点数量
		@version NIIEngine 3.0.0
        */
        NCount getTagCount() const;

        /** 状态是否正常
        @version NIIEngine 3.0.0
        */
        inline bool isValid() const
        {
            return false == mError;
        }

        /** 状态是否异常
        @version NIIEngine 3.0.0
        */
		inline bool isUnvalid() const
        {
            return false != mError;
        }
    protected:
        XmlSerializer(const XmlSerializer & o);

        XmlSerializer & operator=(const XmlSerializer & o);
    private:
        /** 缩行
		@version NIIEngine 3.0.0
        */
        void indentLine();
    private:
		const ScriptProperty * mCmdObj;
        Ntostream * mStream;
        StringList mTagList;
        NCount mTagCount;
        NCount mCurrentDepth;
        NCount mIndentSpace;
        bool mCloseLast;
        bool mLastText;
        bool mError;
    };
}
#endif