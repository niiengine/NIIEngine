/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-8-26

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
	  ��άӦ�����
		������Ϣϵͳ���(GIS)
		��Ӱ���������ع����
		������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
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
        XmlSerializer(Ntostream * out, NCount indentSpace = 4);
		XmlSerializer(Ntostream * out, const ScriptProperty * obj, NCount indent = 4);
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
			return attribute(pid, StrConv::conv(value));
		}

		/** �������
		@param name ��������
		@param value ����ֵ
		@return ��ǰ������Tag�ڵ�
		*/
		inline XmlSerializer & attribute(const String & name, bool value)
		{
			return attribute(name, StrConv::conv(value));
		}

		/** �������
		@param name ����ID
		@param value ����ֵ
		@return ��ǰ������Tag�ڵ�
		*/
		inline XmlSerializer & attribute(PropertyID pid, Nui16 value)
		{
			return attribute(pid, StrConv::conv(value));
		}

		/** �������
		@param name ��������
		@param value ����ֵ
		@return ��ǰ������Tag�ڵ�
		*/
		inline XmlSerializer & attribute(const String & name, Nui16 value)
		{
			return attribute(name, StrConv::conv(value));
		}

		/** �������
		@param name ����ID
		@param value ����ֵ
		@return ��ǰ������Tag�ڵ�
		*/
		inline XmlSerializer & attribute(PropertyID pid, Ni16 value)
		{
			return attribute(pid, StrConv::conv(value));
		}

		/** �������
		@param name ��������
		@param value ����ֵ
		@return ��ǰ������Tag�ڵ�
		*/
		inline XmlSerializer & attribute(const String & name, Ni16 value)
		{
			return attribute(name, StrConv::conv(value));
		}

		/** �������
		@param name ����ID
		@param value ����ֵ
		@return ��ǰ������Tag�ڵ�
		*/
		inline XmlSerializer & attribute(PropertyID pid, Nui32 value)
		{
			return attribute(pid, StrConv::conv(value));
		}

		/** �������
		@param name ��������
		@param value ����ֵ
		@return ��ǰ������Tag�ڵ�
		*/
		inline XmlSerializer & attribute(const String & name, Nui32 value)
		{
			return attribute(name, StrConv::conv(value));
		}

		/** �������
		@param name ����ID
		@param value ����ֵ
		@version NIIEngine 3.0.0
		*/
		inline XmlSerializer & attribute(PropertyID pid, Ni32 value)
		{
			return attribute(pid, StrConv::conv(value));
		}

		/** �������
		@param name ��������
		@param value ����ֵ
		@version NIIEngine 3.0.0
		*/
		inline XmlSerializer & attribute(const String & name, Ni32 value)
		{
			return attribute(name, StrConv::conv(value));
		}

		/** �������
		@param name ID
		@param value ����ֵ
		@version NIIEngine 3.0.0
		*/
		inline XmlSerializer & attribute(PropertyID pid, NIIf value)
		{
			return attribute(pid, StrConv::conv(value));
		}

		/** ���Խڵ�
		@param name ��������
		@param value ����ֵ
		@version NIIEngine 3.0.0
		*/
		inline XmlSerializer & attribute(const String & name, NIIf value)
		{
			return attribute(name, StrConv::conv(value));
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