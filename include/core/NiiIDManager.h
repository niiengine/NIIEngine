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

#ifndef _NII_ID_MANAGER_H_
#define _NII_ID_MANAGER_H_

#include "NiiPreInclude.h"

#include "NiiString.h"
namespace NII
{
    /** 记录某一个类的成员的容器管理器基类
    @remarks
        我们知道一个类的成员的句柄是可以从1开始的,当我们需要使用它的另外一个象征时,
        如用字符串表示这个句柄的含义,但在程序中字符串的象征是多余的,计算机并不需要
        知道你的字符串具体含义它只需要知道你具体需要操作哪一个对象,在调试中时后你可
        能需要知道更多的相关信息,而程序运行时我们注重的是效率,这个类一般是配合Manager
        级别或FactoryObj级别再或者含有父对象的类一起使用,前两者一般有单独实例性质
        (即使不标明),而且有共享性质.后者则只有局部共享性质,这个ID管理器管理的类必须
        是含有顶级对象的性质,也就是说不含有同性质的父对象
    @par
        对象是独立于对象本声的,而不是应用程序,所以不是惟独实例
    */
    class _EngineAPI IDManager : public ManagerAlloc
    {
    public:
        IDManager(const String & autoname);
        virtual ~IDManager();

        /** 生成一个实例对象
        @param[in] nane 对象别名
        @return 一个实例对象,拥有一个惟独ID
        */
        void * generate(const String & name);

        /** 生成一个实例对象
        @return 一个实例对象,拥有一个惟独ID
        */
        void * generate();

        /** 添加一个已经存在的对象
        @remark
            这个机制会自动帮助这个对象生成一个ID,也就是说如果原来这个对象已经有个
            ID,而且与其他对象有ID性质的映射将会导致不可预料的结果,所以在添加到这
            个机制器前不要存在与其他对象的ID映射
        @return 一个实例对象,拥有一个惟独ID
        */
        void add(const void * target);

        /// 移去指定ID的项
        void remove(ObjID id);

        /// 亿去指定的对象实例
        void remove(void * target);

        /** 清空所有对象
        @remark 尽量在引擎结束的时候才调用这个函数,不然会导致不可以预料的结果
        */
        void clear();
    protected:
        /** 创建一个有名字的对象
        @remark 通过工厂类去操作
        @param[in] name
        */
        virtual void * create(const String & name) = 0;

        /** 创建一个对象
        @remark 通过工厂类去操作
        */
        virtual void * create() = 0;

        /** 删除一个对象
        @remark 通过工厂类去操作
        */
        virtual void destroy(void * target) = 0;

        /** 重新命名一个对象
        @remark 通过具体对象的类去操作
        @param[in] target 需要改ID的对象
        @param[in] id 新ID
        */
        virtual void reset(const void * target, ObjID id) = 0;
    public:
        typedef map<Nid, Nvoid>::type PoolList;
        typedef map<String, Nid>::type AliasPoolList;
        typedef vector<Nid>::type RemainList;
    protected:
        N_mutex1
        ObjID mCurrent;
        PoolList mList;
        AliasPoolList mAList;
        RemainList mRList;
    };
}
#endif