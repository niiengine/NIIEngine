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

#ifndef _NII_EXTDATA_H_
#define _NII_EXTDATA_H_

#include "NiiPreInclude.h"
#include "NiiString.h"

namespace NII
{
    /** 扩展数据
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ExtData
    {
    public:
        ExtData();
        virtual ~ExtData();

        /** 设置在这个类实例的任何种类用户对象
        @param[in] data 数据指针(仅支持结构型,不支持对象型数据)
        @param[in] autodestroy 是否自动释放 data 数据
        @param[in] type 内存构造类型
        @version NIIEngine 3.0.0
        */
        void setExtData(void * data, bool autodestroy = true);

        /** 获取在这个类实例的无关键的任何种类用户对象
        @version NIIEngine 3.0.0
        */
        void * getExtData() const;

        /** 设置在这个类实例的任何种类用户对象
        @param[in] key 映射ID
        @param[in] data 使用 N_alooc 创建的数据
        @param[in] autodestroy 是否自动释放 data 数据
        @param[in] type 内存构造类型
        @version NIIEngine 3.0.0
        */
        void setExtData(Nid key, void * data, bool autodestroy = true);

        /** 获取在这个类实例的有关键的任何种类用户对象
        @param[in] key 映射ID
        @version NIIEngine 3.0.0
        */
        void * getExtData(Nid key) const;

        /** 设置在这个类实例的任何种类用户对象
        @param[in] key 映射ID
        @param[in] value 输入字符串
        @version NIIEngine 3.0.0
        */
        void setExtData(Nid key, const String & value);

        /** 获取在这个类实例的有关键的任何种类用户对象
        @param[in] key 映射ID
        @param[out] value 输出字符串
        @version NIIEngine 3.0.0
        */
        void getExtData(Nid key, String & value) const;

        /** 获取指定下标数据
        @version NIIEngine 3.0.0
        */
        void * getIndexData(Nidx idx);

        /** 清除所有数据
        @version NIIEngine 3.0.0
        */
        void clear();
    protected:
        typedef map<Nid, std::pair<void *, bool> >::type DataList;
        typedef map<Nid, String>::type DataStrList;
    private:
        DataList mDataList;
        DataStrList mStringList;
    };
}
#endif