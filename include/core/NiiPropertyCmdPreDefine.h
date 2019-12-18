/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2017-12-24

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