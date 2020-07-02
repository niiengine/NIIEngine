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

#ifndef _NII_PROPERTY_CMD_H_
#define _NII_PROPERTY_CMD_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ���Կ���
    @remark
        ���������Զ����ͽű�,Ҳ���� json, xml, css ���͵Ľű�,�����Ž����ͱ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PropertyCmd : public StreamAlloc
    {
        friend class PropertyCmdObj;
        friend class PropertyCmdSet;
    public:
        /** ���캯��
        @param[in] id ����ID,�������ڲ�ʹ��
        @param[in] verbose ϸ������,�����ģʽ�¿��Կ����ĸ�������
        @version NIIEngine 3.0.0
        */
        PropertyCmd(PropertyID id, const String & verbose);

        /** ���캯��
        @param[in] id ����ID,�������ڲ�ʹ��
        @param[in] name ��������,�ڽű��ϳ��ֵ����Ա�ʶ
        @param[in] verbose ϸ������,�����ģʽ�¿��Կ����ĸ�������
        @version NIIEngine 3.0.0
        */
        PropertyCmd(PropertyID id, const String & name, const String & verbose);

        virtual ~PropertyCmd();

        /** ��ȡ����ID
        @version NIIEngine 3.0.0
        */
        PropertyID getID() const;

        /** ��ʼ�����ƶ���
        @version NIIEngine 3.0.0
        */
        virtual void init(PropertyCmdObj * own) const;

        /** ��ȡֵ(�ַ���)
        @version NIIEngine 3.0.0
        */
        virtual void get(const PropertyCmdObj * own, String & out) const = 0;

        /** ��ȡֵ(����ָ��)
        @param[out] out �Ѿ�ʵ������ָ��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void get(const PropertyCmdObj * own, void * out) const;

        /** ����ֵ(�ַ���)
        @version NIIEngine 3.0.0
        */
        virtual void set(PropertyCmdObj * own, const String & in) = 0;

        /** ����ֵ(����ָ��)
        @param[in] in �Ѿ�ʵ������ָ��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void set(PropertyCmdObj * own, const void * in);

        /** ������ֵ��϶���
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void create(PropertyCmdObj * own, FusionNum *& out) const;

        /** ��ȡĬ��ֵ(�ַ���)
        @version NIIEngine 3.0.0
        */
        virtual void getDefault(String & out) const = 0;

        /** ��ȡ����
        @version NIIEngine 3.0.0
        */
        const String & getVerbose() const;

        /** �Ƿ���Ĭ��״̬
        @version NIIEngine 3.0.0
        */
        bool isDefault(const PropertyCmdObj * own) const;

        /** �Ƿ�ɶ�ȡ
        @version NIIEngine 3.0.0
        */
        virtual bool isReadable() const;

        /** �Ƿ��д��
        @version NIIEngine 3.0.0
        */
        virtual bool isWritable() const;

        /** ���ݴ洢��
        @version NIIEngine 3.0.0
        */
        virtual bool isSerialize() const;

        /** ����
        @version NIIEngine 3.0.0
        */
        virtual PropertyCmd * clone() const = 0;

        /** ������XML�洢����
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