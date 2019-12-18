/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2017-12-24

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

#ifndef _NII_PROPERTYCMD_PREDEFINE_H
#define _NII_PROPERTYCMD_PREDEFINE_H

#define N_PropertyCmdi(Parent, Name, Propid, Verbose, Conv, Set, Get, _Default, B1, B2, B3) \
    class _EngineInner Name : public PropertyCmd                        \
    {                                                                       \
    public:                                                                 \
        Name() : PropertyCmd(Propid, _T(Verbose)) {}                    \
        void get(const PropertyCmdObj * own, String & out) const        \
        {                                                               \
            NIIi t1 = static_cast<const Parent *>(own)->Get();                   \
            Conv::conv(t1, out);                                            \
        }                                                               \
        void set(PropertyCmdObj * own, const String & val)                  \
        {                                                               \
            NIIi t1;                                                    \
            Conv::conv(val, t1);                                            \
            static_cast<Parent *>(own)->Set(t1);                            \
        }                                                                    \
        void get(const PropertyCmdObj * own, void * out)                    \
        {                                                                    \
            NIIi * temp = (NIIi *)out;                                      \
            *temp = static_cast<const Parent *>(own)->Get();                    \
        }                                                                       \
        void set(PropertyCmdObj * own, const void * in)                 \
        {                                                                   \
            NIIi * temp = (NIIf *)in;                                       \
            static_cast<Parent *>(own)->Set(*temp);                          \
        }                                                                   \
        bool isReadable() const{ return B1; }                               \
        bool isWritable() const { return B2; }                              \
        bool isSerialize() const { return B3;}                                   \
        PropertyCmd * clone() const { return N_new Name();   }           \
        void getDefault(String & out) const{ out = (_Default);}};

#define N_PropertyCmdf(Parent, Name, Propid, Verbose, Conv, Set, Get, _Default, B1, B2, B3) \
    class _EngineInner Name : public PropertyCmd                            \
    {                                                                   \
    public:                                                             \
        Name() : PropertyCmd(Propid, _T(Verbose)) {}                        \
        void get(const PropertyCmdObj * own, String & out) const            \
        {                                                                   \
            NIIf t1 = static_cast<const Parent *>(own)->Get();                   \
            Conv::conv(t1, out);                                               \
        }                                                                   \
        void set(PropertyCmdObj * own, const String & val)                  \
        {                                                               \
            NIIf t1;                                                    \
            Conv::conv(val, t1);                                        \
            static_cast<Parent *>(own)->Set(t1);                            \
        }                                                                   \
        void get(const PropertyCmdObj * own, void * out)                    \
        {                                                               \
            NIIf * temp = (NIIf *)out;                                      \
            *temp = static_cast<const Parent *>(own)->Get();                    \
        }                                                                   \
        void set(PropertyCmdObj * own, const void * in)                      \
        {                                                                   \
            NIIf * temp = (NIIf *)in;                                       \
            static_cast<Parent *>(own)->Set(*temp);                          \
        }                                                                   \
        bool isReadable() const{ return B1; }                               \
        bool isWritable() const { return B2; }                              \
        bool isSerialize() const { return B3;}                                  \
        PropertyCmd * clone() const { return N_new Name();   }              \
        void getDefault(String & out) const{ out = (_Default);}};

#define N_PropertyCmd4i(Parent, Name, Propid, Verbose, Conv, Set, Get, _Default, B1, B2, B3)            \
    class _EngineInner Name : public PropertyCmd                            \
    {                                                                       \
    public:                                                                 \
        Name() : PropertyCmd(Propid, _T(Verbose)) {}                        \
        void get(const PropertyCmdObj * own, String & out) const            \
        {                                                                   \
            NIIi u1, v1, u2, v2;                                            \
            static_cast<const Parent *>(own)->Get(u1, v1, u2, v2);          \
            Conv::conv(Vector4f(u1, v1, u2, v2), out);                      \
        }                                                                   \
        void set(PropertyCmdObj * own, const String & val)                  \
        {                                                                   \
            Vector4i temp;                                                  \
            Conv::conv(val, temp);                                          \
            static_cast<Parent *>(own)->Set(temp.x, temp.y, temp.z, temp.w);    \
        }                                                                   \
        void get(const PropertyCmdObj * own, void * out)                    \
        {                                                                   \
            NIIi * temp = (NIIi *)out;                                      \
            static_cast<const Parent *>(own)->Get(*temp, *++temp, *++temp, *++temp);    \
        }                                                                   \
        void set(PropertyCmdObj * own, const void * in)                     \
        {                                                                   \
            NIIi * temp = (NIIi *)in;                                           \
            static_cast<Parent *>(own)->Set(*temp, *++temp, *++temp, *++temp);      \
        }                                                                   \
        bool isReadable() const{ return B1; }                               \
        bool isWritable() const { return B2; }                              \
        bool isSerialize() const { return B3;}                              \
        PropertyCmd * clone() const { return N_new Name();   }              \
        void getDefault(String & out) const{ out = (_Default);}};

#define N_PropertyCmd4f(Parent, Name, Propid, Verbose, Conv, Set, Get, _Default, B1, B2, B3)            \
    class _EngineInner Name : public PropertyCmd                            \
    {                                                                           \
    public:                                                                     \
        Name() : PropertyCmd(Propid, _T(Verbose)) {}                        \
        void get(const PropertyCmdObj * own, String & out) const            \
        {                                                                   \
            NIIf u1, v1, u2, v2;                                                \
            static_cast<const Parent *>(own)->Get(u1, v1, u2, v2);              \
            Conv::conv(Vector4f(u1, v1, u2, v2), out);                      \
        }                                                                       \
        void set(PropertyCmdObj * own, const String & val)                      \
        {                                                                       \
            Vector4f temp;                                                      \
            Conv::conv(val, temp);                                              \
            static_cast<Parent *>(own)->Set(temp.x, temp.y, temp.z, temp.w);        \
        }                                                                           \
        void get(const PropertyCmdObj * own, void * out)                            \
        {                                                                           \
            NIIf * temp = (NIIf *)out;                                                  \
            static_cast<const Parent *>(own)->Get(*temp, *++temp, *++temp, *++temp);    \
        }                                                                               \
        void set(PropertyCmdObj * own, const void * in)                         \
        {                                                                       \
            NIIf * temp = (NIIf *)in;                                           \
            static_cast<Parent *>(own)->Set(*temp, *++temp, *++temp, *++temp);  \
        }                                                                       \
        bool isReadable() const{ return B1; }                                   \
        bool isWritable() const { return B2; }                                  \
        bool isSerialize() const { return B3;}                                  \
        PropertyCmd * clone() const { return N_new Name();   }                  \
        void getDefault(String & out) const{ out = (_Default);}};

#endif