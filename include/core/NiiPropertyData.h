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

#ifndef _NII_PROPERTYDATA_H_
#define _NII_PROPERTYDATA_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 属性数据结构
    @note 一般由 XML JSON YAML 等可描述脚本序列化程序使用
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PropertyData : public DataAlloc
    {
    public:
        typedef map<PropertyID, PropertyData>::type SubPropertyList;
        typedef map<PropertyID, void *>::type ExtPropertyList;
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
        void addVoid(PropertyID pid, void * value);

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
		const void * getData(Nidx idx) const;

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
        const String & get(PropertyID pid) const;

        /** 获取值
        @param pid 属性ID
        @param init 属性默认值
        @version NIIEngine 3.0.0
        */
        String getDefault(PropertyID pid, const String & init = _T("")) const;

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

        /** 获取值(补充)
        @param[in] pid 属性ID
        @param[out] out 输出
        @param[in] init 属性默认值
        @version NIIEngine 3.0.0 adv
        */
        bool getData(PropertyID pid, void *& out, void * init = 0) const;

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
        inline NCount getCount() const                          { return mPropertys.size() + mExtPropertys.size(); }

        /** 获取属性值列表
        @version NIIEngine 3.0.0 adv api
        */
        inline const PropertyList & getList() const             { return mPropertys; }
        
        /** 获取属性值列表
        @version NIIEngine 3.0.0 adv api
        */
        inline const ExtPropertyList & getExtList() const       {return mExtPropertys;}

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
}
#endif