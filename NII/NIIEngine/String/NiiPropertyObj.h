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

#ifndef _NII_PropertyObj_H_
#define _NII_PropertyObj_H_

#include "NiiPreInclude.h"
#include "NiiScriptProperty.h"
#include "NiiString.h"
#include "NiiAny.h"

namespace NII
{
    struct _EngineAPI PropertyValue
    {
        PropertyValue(PropertyID key, const String & value) : mKey(key), mValue(value) {}

        bool operator == (const PropertyValue & o) const
        {
            return mKey == o.mKey && mValue == o.mValue;
        }
        
        PropertyID mKey;
        String mValue;
    };

    typedef vector<PropertyValue>::type PropertyValueList;
    
	inline bool PropertyValueLess(const PropertyValue & o1, const PropertyValue & o2)
	{
		return o1.mKey < o2.mKey;
	}
	
	class _EngineAPI PropertyValueData : public DataAlloc
	{
	public :
        bool operator == (const PropertyValueData & o) const
        {
            return mData == o.mData;
        }
	
		inline void push_back(PropertyID key, const String & value)
		{
			mData.emplace_back(PropertyValue(key, value));
		}

		void add(PropertyID key, const String & value);

		const String & get(PropertyID key, const String & init = N_StrBlank);
		
		PropertyValueList mData;
	};
	
	class _EngineAPI StringIdData : public DataAlloc
	{
	public :
        bool operator == (const StringIdData & o) const
        {
            return mData == o.mData;
        }
		
		bool isExist(Nid key, String & defauleOut);
		
		StringIdMap mData;
	};
	
    inline bool StringIdLess(const std::pair<Nid, String> & o1, const std::pair<Nid, String> & o2)
    {
        return o1.first < o2.first;
    }
	
    /** 属性数据结构
    @note 一般由 XML JSON YAML 等可描述脚本序列化程序使用
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PropertyData : public DataAlloc
    {
    public:
        typedef map<PropertyID, PropertyData>::type SubPropertyList;
        typedef map<PropertyID, Any>::type ExtPropertyList;
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
        @note 不管理(数据指针)的有效性,没有副本产生
        @version NIIEngine 3.0.0 adv api
        */
        void addData(PropertyID pid, const Any & value);

        /** 添加属性(补充)
        @param pid 属性ID
        @param value 值(数据指针)
        @note 不管理(数据指针)的有效性,没有副本产生
        @version NIIEngine 3.0.0 adv api
        */
        void addSub(PropertyID pid, const PropertyData & value);

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
        
        /** 移去所有属性
        @param pid 属性ID
        @version NIIEngine 3.0.0
        */
        void removeAll();

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
        @param[in] idx 属性ID
        @version NIIEngine 3.0.0 adv api
        */
        const Any & getData(Nidx idx) const;

        /** 获取值
        @param[in] idx 属性ID
        @version NIIEngine 3.0.0 adv api
        */
        const PropertyData * getSub(Nidx idx) const;

        /** 获取值
        @param pid 属性ID
        @param init 属性默认值
        @version NIIEngine 3.0.0
        */
        String getDefault(PropertyID pid, const String & init = _T("")) const;

        /** 获取值
        @param pid 属性ID
        @param init 属性默认值
        @version NIIEngine 3.0.0
        */
        const String & get(PropertyID pid) const;

        /** 获取值
        @param[in] pid 属性ID
        @param[out] out 输出
        @param[in] init 属性默认值
        @version NIIEngine 3.0.0
        */
        bool get(PropertyID pid, PropertyData & out) const;

        /** 获取值
        @param pid 属性ID
        @param init 属性默认值
        @version NIIEngine 3.0.0
        */
        bool get(PropertyID pid, String & out, const String & init = _T("")) const;

        /** 获取值
        @param[in] pid 属性ID
        @param[out] out 输出
        @version NIIEngine 3.0.0
        */
        bool get(PropertyID pid, StringList & out) const;

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
        @version NIIEngine 3.0.0
        */
        bool get(PropertyID pid, IntArray & out) const;

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
        @version NIIEngine 3.0.0
        */
        bool get(PropertyID pid, UIntArray & out) const;

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

        /** 获取值
        @param[in] pid 属性ID
        @param[out] out 输出
        @version NIIEngine 3.0.0
        */
        bool get(PropertyID pid, FloatArray & out) const;

        /** 获取值
        @param[in] pid 属性ID
        @param[out] out 输出
        @param[in] init 属性默认值
        @version NIIEngine 3.0.0
        */
        bool get(PropertyID pid, NIId & out, NIId init = 0.0) const;

        /** 获取值
        @param[in] pid 属性ID
        @param[out] out 输出
        @param[in] cnt 输出个数
        @version NIIEngine 3.0.0
        */
        bool get(PropertyID pid, NIId * out, NCount cnt) const;

        /** 获取值
        @param[in] pid 属性ID
        @param[out] out 输出
        @version NIIEngine 3.0.0
        */
        bool get(PropertyID pid, DoubleArray & out) const;

        /** 获取值
        @param[in] pid 属性ID
        @return 输出
        @version NIIEngine 3.0.0
        */
        template<typename T> N_FORCEINLINE T getValue(PropertyID pid)
        {
            T re;
            get(pid, re);
            return re;
        }

        /** 获取值
        @param[in] pid 属性ID
        @return 输出
        @version NIIEngine 3.0.0
        */
        template<typename T> N_FORCEINLINE T getValue(PropertyID pid, const T & init)
        {
            T re;
            get(pid, re, init);
            return re;
        }

        /** 获取值(补充)
        @param[in] pid 属性ID
        @param[out] out 输出
        @param[in] init 属性默认值
        @version NIIEngine 3.0.0 adv
        */
        bool getData(PropertyID pid, Any & out, const Any & init = 0) const;

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
        inline NCount getCount() const                      { return mPropertys.size() + mExtPropertys.size(); }

        /** 获取属性值列表
        @version NIIEngine 3.0.0 adv api
        */
        inline const PropertyList & getList() const         { return mPropertys; }

        /** 获取属性值列表
        @version NIIEngine 3.0.0 adv api
        */
        inline const ExtPropertyList & getExtList() const   { return mExtPropertys; }
        
        /** 获取子属性值列表
        @version NIIEngine 3.0.0 adv api
        */
        inline const SubPropertyList & getSubList() const   { return mSubPropertys; }

        /** 复制对象
        @note 返回的使用完后使用 N_delete 删除
        @version NIIEngine 3.0.0
        */
        PropertyData * clone() const;
    protected:
        PropertyList mPropertys;
        ExtPropertyList mExtPropertys;
        SubPropertyList mSubPropertys;
    };

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

        inline const String & getName() const       { return mName; }

        /** 获取属性ID
        @version NIIEngine 3.0.0
        */
        inline PropertyID getID() const             { return mID; }

        /** 初始化控制对象
        @version NIIEngine 3.0.0
        */
        virtual void init(PropertyCmdObj * own) const;

        /** 获取值(字符串)
        @version NIIEngine 3.0.0
        */
        virtual void get(const PropertyCmdObj * own, String & out) const;

        /** 获取值(数据指针)
        @param[out] out 已经实例化的指针
        @version NIIEngine 3.0.0 高级api
        */
        virtual void get(const PropertyCmdObj * own, Any & out) const;

        /** 设置值(字符串)
        @version NIIEngine 3.0.0
        */
        virtual void set(PropertyCmdObj * own, const String & in);

        /** 设置值(数据指针)
        @param[in] in 已经实例化的指针
        @version NIIEngine 3.0.0 高级api
        */
        virtual void set(PropertyCmdObj * own, const Any & in);

        /** 创建数值混合对象
        @version NIIEngine 3.0.0 高级api
        */
        virtual void create(PropertyCmdObj * own, FusionNum *& out) const;

        /** 获取值(类型)
        @version NIIEngine 3.0.0
        */
        template<typename T, typename STRConv> N_FORCEINLINE T getValue(PropertyID pid, T & out)
        {
            String re;
            get(pid, re);
            STRConv::conv(re, out);
        }

        /** 获取默认值(字符串)
        @version NIIEngine 3.0.0
        */
        virtual void getDefault(String & out) const = 0;

        /** 获取描述
        @version NIIEngine 3.0.0
        */
        const String & getVerbose() const           { return mVerbose; }

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
        bool isExist(PropertyID pid)const               { return mCmdMap.find(pid) != mCmdMap.end();}

        /** 获取属性控制
        @version NIIEngine 3.0.0
        */
        PropertyCmd * get(PropertyID pid) const;

        /** 获取属性控制
        @version NIIEngine 3.0.0
        */
        PropertyCmd * get(const String & pid) const;

        /** 获取属性名字
        @version NIIEngine 3.0.0
        */
        const String & getName(PropertyID pid) const    { return get(pid)->getName(); }

        /** 获取列表
        @version NIIEngine 3.0.0 高级api
        */
        PropertyCmdMap & getList()                      { return mCmdMap; }

        /** 获取列表
        @version NIIEngine 3.0.0 高级api
        */
        const PropertyCmdMap & getList() const          { return mCmdMap; }

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
        virtual bool set(const String & pname, const Any & out);

        /** 设置属性值
        @note 必须明确知道所设置的属性是什么类型
        @version NIIEngine 3.0.0
        */
        virtual bool set(PropertyID pid, const String & value);

        /** 设置属性值
        @note 必须明确知道所设置的属性是什么类型,输入参数是类型实体的指针
        @version NIIEngine 3.0.0
        */
        virtual bool set(PropertyID pid, const Any & out);

        /** 获取属性值
        @note 必须明确知道所获取的属性是什么类型
        @version NIIEngine 3.0.0
        */
        virtual bool get(PropertyID pid, String & value) const;

        /** 获取属性值
        @note 必须明确知道所获取的属性是什么类型,输出参数是类型实体的指针
        @version NIIEngine 3.0.0
        */
        virtual bool get(PropertyID pid, Any & out) const;

        /** 获取属性名字
        @version NIIEngine 3.0.0
        */
        const String & getPropertyName(PropertyID pid) const    { return mProperty->get(pid)->getName(); }

        /** 获取属性字符串值
        @version NIIEngine 3.0.0
        */
        String getPropertyValue(PropertyID pid) const           { String re; mProperty->get(pid)->get(this, re); return re; }

        /** 设置属性字符串值
        @version NIIEngine 3.0.0
        */
        void setPropertyValue(PropertyID pid, const String & value) { mProperty->get(pid)->set(this, value); }

        /** 创建属性值混合
        @note 必须明确知道所获取的属性是什么类型,输出参数是新构建类型实体的指针
        @version NIIEngine 3.0.0
        */
        virtual FusionNum * createNum(PropertyID pid);

        /** 复制属性
        @param[in] src 属性来源
        @version NIIEngine 3.0.0
        */
        virtual void copyProperty(PropertyCmdObj * src);

        /** 目录
        @version NIIEngine 3.0.0
        */
        PropertyCmdSet * getProperty() const        { return mProperty; }

        /** 是否存在属性
        @version NIIEngine 3.0.0
        */
        bool isPropertyExist(PropertyID pid) const  { return mProperty->isExist(pid); }

        /** 是否默认值
        @version NIIEngine 3.0.0
        */
        bool isPropertyDefault(PropertyID pid) const { return mProperty->get(pid)->isDefault(this); }

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
        virtual bool initCmd(PropertyCmdSet * dest);
    protected:
        ClassID mCmdObjID;
        PropertyCmdSet * mProperty;
        N_static_mutex(mCmdCollectMapMutex);
        static CmdCollectMap mCmdCollectMap;
    };
    
#define N_PropertyCmdb(OWN, PNAME, PID, VERBOSE, SetFun, GetFun, _default, _rable, _wable, _sable) \
    class _EngineInner PNAME : public PropertyCmd                       \
    {                                                                   \
    public:                                                             \
        PNAME() : PropertyCmd(PID, _T(VERBOSE)) {}                      \
        void get(const PropertyCmdObj * own, String & out) const        \
        {                                                               \
            bool t1 = static_cast<const OWN *>(own)->GetFun();          \
            StrConv::conv(t1, out);                                     \
        }                                                               \
        void set(PropertyCmdObj * own, const String & val)              \
        {                                                               \
            bool t1;                                                    \
            StrConv::conv(val, t1);                                     \
            static_cast<OWN *>(own)->SetFun(t1);                        \
        }                                                               \
        void get(const PropertyCmdObj * own, Any & out)                 \
        {                                                               \
            out = Any(static_cast<const OWN *>(own)->GetFun());         \
        }                                                               \
        void set(PropertyCmdObj * own, const Any & in)                  \
        {                                                               \
            static_cast<OWN *>(own)->SetFun(NII::any_cast<bool>(in));   \
        }                                                               \
        bool isReadable() const{ return _rable; }                       \
        bool isWritable() const { return _wable; }                      \
        bool isSerialize() const { return _sable;}                      \
        PropertyCmd * clone() const { return N_new PNAME();   }         \
        void getDefault(String & out) const{ out = _T(_default);}};

#define N_PropertyCmdi(OWN, PNAME, PID, VERBOSE, SetFun, GetFun, _default, _rable, _wable, _sable) \
    class _EngineInner PNAME : public PropertyCmd                       \
    {                                                                   \
    public:                                                             \
        PNAME() : PropertyCmd(PID, _T(VERBOSE)) {}                      \
        void get(const PropertyCmdObj * own, String & out) const        \
        {                                                               \
            NIIi t1 = static_cast<const OWN *>(own)->GetFun();          \
            StrConv::conv(t1, out);                                     \
        }                                                               \
        void set(PropertyCmdObj * own, const String & val)              \
        {                                                               \
            NIIi t1;                                                    \
            StrConv::conv(val, t1);                                     \
            static_cast<OWN *>(own)->SetFun(t1);                        \
        }                                                               \
        void get(const PropertyCmdObj * own, Any & out)                 \
        {                                                               \
            out = Any(static_cast<const OWN *>(own)->GetFun());         \
        }                                                               \
        void set(PropertyCmdObj * own, const Any & in)                  \
        {                                                               \
            static_cast<OWN *>(own)->SetFun(NII::any_cast<NIIi>(in));   \
        }                                                               \
        bool isReadable() const{ return _rable; }                       \
        bool isWritable() const { return _wable; }                      \
        bool isSerialize() const { return _sable;}                      \
        PropertyCmd * clone() const { return N_new PNAME();   }         \
        void getDefault(String & out) const{ out = _T(_default);}};

#define N_PropertyCmdf(OWN, PNAME, PID, VERBOSE, SetFun, GetFun, _default, _rable, _wable, _sable) \
    class _EngineInner PNAME : public PropertyCmd                       \
    {                                                                   \
    public:                                                             \
        PNAME() : PropertyCmd(PID, _T(VERBOSE)) {}                      \
        void get(const PropertyCmdObj * own, String & out) const        \
        {                                                               \
            NIIf t1 = static_cast<const OWN *>(own)->GetFun();          \
            StrConv::conv(t1, out);                                     \
        }                                                               \
        void set(PropertyCmdObj * own, const String & val)              \
        {                                                               \
            NIIf t1;                                                    \
            StrConv::conv(val, t1);                                     \
            static_cast<OWN *>(own)->SetFun(t1);                        \
        }                                                               \
        void get(const PropertyCmdObj * own, Any & out)                 \
        {                                                               \
            out = Any(static_cast<const OWN *>(own)->GetFun());         \
        }                                                               \
        void set(PropertyCmdObj * own, const Any & in)                  \
        {                                                               \
            static_cast<OWN *>(own)->SetFun(NII::any_cast<NIIf>(in));   \
        }                                                               \
        bool isReadable() const{ return _rable; }                       \
        bool isWritable() const { return _wable; }                      \
        bool isSerialize() const { return _sable;}                      \
        PropertyCmd * clone() const { return N_new PNAME();   }         \
        void getDefault(String & out) const{ out = _T(_default);}};

#define N_PropertyCmd4i(OWN, PNAME, PID, VERBOSE, CONV, SetFun, GetFun, _default, _rable, _wable, _sable)            \
    class _EngineInner PNAME : public PropertyCmd                       \
    {                                                                   \
    public:                                                             \
        PNAME() : PropertyCmd(PID, _T(VERBOSE)) {}                      \
        void get(const PropertyCmdObj * own, String & out) const        \
        {                                                               \
            NIIi u1, v1, u2, v2;                                        \
            static_cast<const OWN *>(own)->GetFun(u1, v1, u2, v2);      \
            CONV::conv(Vector4i(u1, v1, u2, v2), out);                  \
        }                                                               \
        void set(PropertyCmdObj * own, const String & val)              \
        {                                                               \
            Vector4i temp;                                              \
            CONV::conv(val, temp);                                      \
            static_cast<OWN *>(own)->SetFun(temp.x, temp.y, temp.z, temp.w);    \
        }                                                               \
        void get(const PropertyCmdObj * own, Any & out)                 \
        {                                                               \
            NIIi u1, v1, u2, v2;                                        \
            static_cast<const OWN *>(own)->GetFun(u1, v1, u2, v2);      \
            out = Any(Vector4i(u1, v1, u2, v2));                        \
        }                                                               \
        void set(PropertyCmdObj * own, const Any & in)                  \
        {                                                               \
            const Vector4i & temp = NII::any_cast<Vector4i>(in);        \
            static_cast<OWN *>(own)->SetFun(temp.x, temp.y, temp.z, temp.w);    \
        }                                                               \
        bool isReadable() const{ return _rable; }                       \
        bool isWritable() const { return _wable; }                      \
        bool isSerialize() const { return _sable;}                      \
        PropertyCmd * clone() const { return N_new PNAME();   }         \
        void getDefault(String & out) const{ out = _T(_default);}};

#define N_PropertyCmd4f(OWN, PNAME, PID, VERBOSE, SetFun, GetFun, _default, _rable, _wable, _sable) \
    class _EngineInner PNAME : public PropertyCmd                               \
    {                                                                           \
    public:                                                                     \
        PNAME() : PropertyCmd(PID, _T(VERBOSE)) {}                              \
        void get(const PropertyCmdObj * own, String & out) const                \
        {                                                                       \
            NIIf u1, v1, u2, v2;                                                \
            static_cast<const OWN *>(own)->GetFun(u1, v1, u2, v2);              \
            StrConv::conv(Vector4f(u1, v1, u2, v2), out);                       \
        }                                                                       \
        void set(PropertyCmdObj * own, const String & val)                      \
        {                                                                       \
            Vector4f temp;                                                      \
            StrConv::conv(val, temp);                                           \
            static_cast<OWN *>(own)->SetFun(temp.x, temp.y, temp.z, temp.w);    \
        }                                                                       \
        void get(const PropertyCmdObj * own, Any & out)                         \
        {                                                                       \
            NIIf u1, v1, u2, v2;                                                \
            static_cast<const OWN *>(own)->GetFun(u1, v1, u2, v2);              \
            out = Any(Vector4f(u1, v1, u2, v2));                                \
        }                                                                       \
        void set(PropertyCmdObj * own, const Any & in)                          \
        {                                                                       \
            const Vector4f & temp = NII::any_cast<Vector4f>(in);                \
            static_cast<OWN *>(own)->SetFun(temp.x, temp.y, temp.z, temp.w);    \
        }                                                                       \
        bool isReadable() const{ return _rable; }                               \
        bool isWritable() const { return _wable; }                              \
        bool isSerialize() const { return _sable;}                              \
        PropertyCmd * clone() const { return N_new PNAME();   }                 \
        void getDefault(String & out) const{ out = _T(_default);}};

#define N_PropertyCmdT(T, OWN, PNAME, PID, VERBOSE, TCONV, SetFun, GetFun, _default, _rable, _wable, _sable) \
    class _EngineInner PNAME : public PropertyCmd                               \
    {                                                                           \
    public:                                                                     \
        PNAME() : PropertyCmd(PID, _T(VERBOSE)) {}                              \
        void get(const PropertyCmdObj * own, String & out) const                \
        {                                                                       \
            T value;                                                            \
            value = static_cast<const OWN *>(own)->GetFun();                    \
            TCONV::conv(value, out);                                            \
        }                                                                       \
        void set(PropertyCmdObj * own, const String & val)                      \
        {                                                                       \
            T value;                                                            \
            TCONV::conv(value, temp);                                           \
            static_cast<OWN *>(own)->SetFun(value);                             \
        }                                                                       \
        void get(const PropertyCmdObj * own, Any & out)                         \
        {                                                                       \
            out = Any(static_cast<const OWN *>(own)->GetFun());                 \
        }                                                                       \
        void set(PropertyCmdObj * own, const Any & in)                          \
        {                                                                       \
            static_cast<OWN *>(own)->SetFun(NII::any_cast<T>(in));              \
        }                                                                       \
        bool isReadable() const{ return _rable; }                               \
        bool isWritable() const { return _wable; }                              \
        bool isSerialize() const { return _sable;}                              \
        PropertyCmd * clone() const { return N_new PNAME();   }                 \
        void getDefault(String & out) const{ out = _T(_default);}};
        
#define N_PropertyCmd4T(OWN, PNAME, PID, VERBOSE, TCONV, SetFun, GetFun, _default, _rable, _wable, _sable) \
    class _EngineInner PNAME : public PropertyCmd                               \
    {                                                                           \
    public:                                                                     \
        PNAME() : PropertyCmd(PID, _T(VERBOSE)) {}                              \
        void get(const PropertyCmdObj * own, String & out) const                \
        {                                                                       \
            T u1, v1, u2, v2;                                                   \
            static_cast<const OWN *>(own)->GetFun(u1, v1, u2, v2);              \
            TCONV::conv(Vector4<T>(u1, v1, u2, v2), out);                       \
        }                                                                       \
        void set(PropertyCmdObj * own, const String & val)                      \
        {                                                                       \
            Vector4<T> temp;                                                    \
            TCONV::conv(val, temp);                                             \
            static_cast<OWN *>(own)->SetFun(temp.x, temp.y, temp.z, temp.w);    \
        }                                                                       \
        void get(const PropertyCmdObj * own, Any & out)                         \
        {                                                                       \
            T u1, v1, u2, v2;                                                   \
            static_cast<const OWN *>(own)->GetFun(u1, v1, u2, v2);              \
            out = Any(Vector4<T>(u1, v1, u2, v2));                              \
        }                                                                       \
        void set(PropertyCmdObj * own, const Any & in)                          \
        {                                                                       \
            const Vector4<T> & temp = NII::any_cast<Vector4<T> >(in);           \
            static_cast<OWN *>(own)->SetFun(temp.x, temp.y, temp.z, temp.w);    \
        }                                                                       \
        bool isReadable() const{ return _rable; }                               \
        bool isWritable() const { return _wable; }                              \
        bool isSerialize() const { return _sable;}                              \
        PropertyCmd * clone() const { return N_new PNAME();   }                 \
        void getDefault(String & out) const{ out = _T(_default);}};
}
#endif