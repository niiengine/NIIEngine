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

#ifndef _NII_CodeGen_MANAGER_H_
#define _NII_CodeGen_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiCodeGen.h"

namespace NII
{
    /** 编码生成器工厂类
    @note CodeGen 类是实例对象,所以需要这个类辅助
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CodeGenFactory : public VFSAlloc
    {
    public:
        CodeGenFactory(Nid codeid) : mCodeID(codeid) {}
        virtual ~CodeGenFactory() {}
        
        /** 类型
        @version NIIEngine 3.0.0
        */
        Nid getCodeID() const              { return mCodeID; }

        /** 类型
        @version NIIEngine 3.0.0
        */
        virtual CodeGen::Type getType() const = 0;

        /** 创建实例
        @version NIIEngine 3.0.0
        */
        virtual CodeGen * create(const String * rawcode) = 0;

        /** 删除实例
        @version NIIEngine 3.0.0
        */
        virtual void destroy(CodeGen * obj) = 0;
    protected:
        Nid mCodeID;
    };

    /** 编码生成器管理器
    @remark 用先进的工具生成更先进的工具
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CodeGenManager : public Singleton<CodeGenManager>, public VFSAlloc
    {
    public:
        typedef map<Nid, CodeGenFactory *>::type FactoryList;
    public:
        CodeGenManager();
        virtual ~CodeGenManager();

        /** 添加工厂
        @version NIIEngine 3.0.0
        */
        void add(CodeGenFactory * factory);

        /** 移去工厂
        @version NIIEngine 3.0.0
        */
        void remove(CodeGenFactory * factory);

        /** 加载编码生成器
        @param[in] rawcode 原代码(实例)
        @param[in] codeid 类型
        @version NIIEngine 3.0.0
        */
        CodeGen * load(const String * rawcode, Nid codeid);

        /** 卸载编码生成器
        @param[in] rawcode 原代码(实例)
        @note 你必须确保在移去它的时候没有正在使用
        @version NIIEngine 3.0.0
        */
        void unload(const String * rawcode);
    protected:
        typedef map<const String *, CodeGen *>::type CodeGenInsList;

        FactoryList mFactoryList;
        CodeGenInsList mCodeGenInsList;
    };
}
#endif