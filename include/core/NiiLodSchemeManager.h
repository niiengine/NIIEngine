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

#ifndef _NII_LOD_SCHEME_MANAGER_H_
#define _NII_LOD_SCHEME_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiLodScheme.h"
#include "NiiSingleton.h"
#include "NiiLodUnitList.h"

namespace NII
{
    /** LOD管理器
    @note src 0.0f 代表原始层,而且LodScheme需要一个原始层
    @version NIIEngine 3.0.0
    */
    class _EngineAPI LodSchemeManager : public Singleton<LodSchemeManager>, public LodAlloc
    {
    public:
        typedef map<ClassID, LodScheme *>::type DefaultList;
        typedef map<SchemeID, LodScheme *>::type SchemeList;
        typedef map<ClassID, SchemeList>::type ClassSchemeList;
    public:
        LodSchemeManager();
        ~LodSchemeManager();

        /** 添加策略
        @version NIIEngine 3.0.0
        */
        void add(ClassID cid, LodScheme * obj);

        /** 移去策略
        @version NIIEngine 3.0.0
        */
        void remove(ClassID cid, SchemeID sid);

        /** 获取策略
        @version NIIEngine 3.0.0
        */
        LodScheme * get(ClassID cid, SchemeID sid);

        /** 创建策略
        @note 使用完后调用 N_delete
        @version NIIEngine 3.0.0
        */
        LodScheme * create(ClassID cid, SchemeID sid);

        /** 移去所有策略
        @version NIIEngine 3.0.0
        */
        void removeAll(ClassID cid);

        /** 移去所有策略
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** 设置默认策略
        @version NIIEngine 3.0.0
        */
        void setDefault(ClassID cid, SchemeID sid);

        /** 创建默认策略
        @note 使用完后调用 N_delete
        @version NIIEngine 3.0.0
        */
        LodScheme * createDefault(ClassID cid);

        /** 获取列表
        @version NIIEngine 3.0.0
        */
        const SchemeList * getList(ClassID cid);

        /// @copydetails Singleton::getOnly
        static LodSchemeManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static LodSchemeManager * getOnlyPtr();
    private:
        DefaultList mDefaultList;
        ClassSchemeList mSchemeList;
    };
}
#endif