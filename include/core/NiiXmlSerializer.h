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
        XmlSerializer(Nostream * out, NCount indentSpace = 4);
        XmlSerializer(Nostream * out, const ScriptProperty * obj, NCount indent = 4);
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
            return attribute(pid, N_conv(value));
        }

        /** 添加属性
        @param name 属性名字
        @param value 属性值
        @return 当前操作的Tag节点
        */
        inline XmlSerializer & attribute(const String & name, bool value)
        {
            return attribute(name, N_conv(value));
        }

        /** 添加属性
        @param name 属性ID
        @param value 属性值
        @return 当前操作的Tag节点
        */
        inline XmlSerializer & attribute(PropertyID pid, Nui16 value)
        {
            return attribute(pid, N_conv(value));
        }

        /** 添加属性
        @param name 属性名字
        @param value 属性值
        @return 当前操作的Tag节点
        */
        inline XmlSerializer & attribute(const String & name, Nui16 value)
        {
            return attribute(name, N_conv(value));
        }

        /** 添加属性
        @param name 属性ID
        @param value 属性值
        @return 当前操作的Tag节点
        */
        inline XmlSerializer & attribute(PropertyID pid, Ni16 value)
        {
            return attribute(pid, N_conv(value));
        }

        /** 添加属性
        @param name 属性名字
        @param value 属性值
        @return 当前操作的Tag节点
        */
        inline XmlSerializer & attribute(const String & name, Ni16 value)
        {
            return attribute(name, N_conv(value));
        }

        /** 添加属性
        @param name 属性ID
        @param value 属性值
        @return 当前操作的Tag节点
        */
        inline XmlSerializer & attribute(PropertyID pid, Nui32 value)
        {
            return attribute(pid, N_conv(value));
        }

        /** 添加属性
        @param name 属性名字
        @param value 属性值
        @return 当前操作的Tag节点
        */
        inline XmlSerializer & attribute(const String & name, Nui32 value)
        {
            return attribute(name, N_conv(value));
        }

        /** 添加属性
        @param name 属性ID
        @param value 属性值
        @version NIIEngine 3.0.0
        */
        inline XmlSerializer & attribute(PropertyID pid, Ni32 value)
        {
            return attribute(pid, N_conv(value));
        }

        /** 添加属性
        @param name 属性名字
        @param value 属性值
        @version NIIEngine 3.0.0
        */
        inline XmlSerializer & attribute(const String & name, Ni32 value)
        {
            return attribute(name, N_conv(value));
        }

        /** 添加属性
        @param name ID
        @param value 属性值
        @version NIIEngine 3.0.0
        */
        inline XmlSerializer & attribute(PropertyID pid, NIIf value)
        {
            return attribute(pid, N_conv(value));
        }

        /** 属性节点
        @param name 属性名字
        @param value 属性值
        @version NIIEngine 3.0.0
        */
        inline XmlSerializer & attribute(const String & name, NIIf value)
        {
            return attribute(name, N_conv(value));
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
        Nostream * mStream;
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