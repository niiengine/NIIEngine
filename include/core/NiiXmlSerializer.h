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
    /** ����Xml�ļ�.
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

        /** ��ʼ�µ�Tag�ڵ�
        @param name Tag�ڵ㵥ԪID
        @return ��ǰ������Tag�ڵ�
        */
        XmlSerializer & begin(XmlUnitID uid);

        /** ��ʼ�µ�Tag�ڵ�
        @param name Tag�ڵ�����
        @return ��ǰ������Tag�ڵ�
        */
        XmlSerializer & begin(const String & name);

        /** ������ǰTag�ڵ�
        @return ��ǰ������Tag�ڵ�
        */
        XmlSerializer & end();

        /** �������
        @param name ����ID
        @param value ����ֵ
        @return ��ǰ������Tag�ڵ�
        */
        XmlSerializer & attribute(PropertyID pid, const String & value);

        /** �������
        @param name ��������
        @param value ����ֵ
        @return ��ǰ������Tag�ڵ�
        */
        XmlSerializer & attribute(const String & name, const String & value);

        /** �������
        @param name ����ID
        @param value ����ֵ
        @return ��ǰ������Tag�ڵ�
        */
        XmlSerializer & attribute(PropertyID pid, bool value)
        {
            return attribute(pid, N_conv(value));
        }

        /** �������
        @param name ��������
        @param value ����ֵ
        @return ��ǰ������Tag�ڵ�
        */
        inline XmlSerializer & attribute(const String & name, bool value)
        {
            return attribute(name, N_conv(value));
        }

        /** �������
        @param name ����ID
        @param value ����ֵ
        @return ��ǰ������Tag�ڵ�
        */
        inline XmlSerializer & attribute(PropertyID pid, Nui16 value)
        {
            return attribute(pid, N_conv(value));
        }

        /** �������
        @param name ��������
        @param value ����ֵ
        @return ��ǰ������Tag�ڵ�
        */
        inline XmlSerializer & attribute(const String & name, Nui16 value)
        {
            return attribute(name, N_conv(value));
        }

        /** �������
        @param name ����ID
        @param value ����ֵ
        @return ��ǰ������Tag�ڵ�
        */
        inline XmlSerializer & attribute(PropertyID pid, Ni16 value)
        {
            return attribute(pid, N_conv(value));
        }

        /** �������
        @param name ��������
        @param value ����ֵ
        @return ��ǰ������Tag�ڵ�
        */
        inline XmlSerializer & attribute(const String & name, Ni16 value)
        {
            return attribute(name, N_conv(value));
        }

        /** �������
        @param name ����ID
        @param value ����ֵ
        @return ��ǰ������Tag�ڵ�
        */
        inline XmlSerializer & attribute(PropertyID pid, Nui32 value)
        {
            return attribute(pid, N_conv(value));
        }

        /** �������
        @param name ��������
        @param value ����ֵ
        @return ��ǰ������Tag�ڵ�
        */
        inline XmlSerializer & attribute(const String & name, Nui32 value)
        {
            return attribute(name, N_conv(value));
        }

        /** �������
        @param name ����ID
        @param value ����ֵ
        @version NIIEngine 3.0.0
        */
        inline XmlSerializer & attribute(PropertyID pid, Ni32 value)
        {
            return attribute(pid, N_conv(value));
        }

        /** �������
        @param name ��������
        @param value ����ֵ
        @version NIIEngine 3.0.0
        */
        inline XmlSerializer & attribute(const String & name, Ni32 value)
        {
            return attribute(name, N_conv(value));
        }

        /** �������
        @param name ID
        @param value ����ֵ
        @version NIIEngine 3.0.0
        */
        inline XmlSerializer & attribute(PropertyID pid, NIIf value)
        {
            return attribute(pid, N_conv(value));
        }

        /** ���Խڵ�
        @param name ��������
        @param value ����ֵ
        @version NIIEngine 3.0.0
        */
        inline XmlSerializer & attribute(const String & name, NIIf value)
        {
            return attribute(name, N_conv(value));
        }

        /** ����ı��ڵ�
        @param text �ڵ���ı�
        @return ��ǰ������Tag�ڵ�
        @version NIIEngine 3.0.0
        */
        XmlSerializer & text(const String & text);

        /** ��ȡ��ǰTag�ڵ�����
        @version NIIEngine 3.0.0
        */
        NCount getTagCount() const;

        /** ״̬�Ƿ�����
        @version NIIEngine 3.0.0
        */
        inline bool isValid() const
        {
            return false == mError;
        }

        /** ״̬�Ƿ��쳣
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
        /** ����
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