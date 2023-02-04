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

#ifndef _NII_CODEGEN_H_
#define _NII_CODEGEN_H_

#include "NiiPreInclude.h"
#include "NiiCommon.h"

namespace NII
{
    /** 编码生成器
    @remark
        编码合成与生成
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CodeGen : public VFSAlloc
    {
    public:
        /** 合成类型
        @version NIIEngine 3.0.0
        */
        enum Type
        {
            T_Unknow            = 0,
            T_Replace           = 1,
            T_Framework         = 2,
            T_Plugin            = 3
        };
    public:
        CodeGen(Nid codeid, Type type);
        CodeGen(const String * name, Nid codeid, Type type);
        virtual ~CodeGen();
        
        /** 返回这个档案代码类型
        @version NIIEngine 3.0.0
        */
        Nid getCodeID() const              { return mCodeID; }

        /** 返回这个档案代码类型
        @version NIIEngine 3.0.0
        */
        Type getGenType() const             { return mGenType; }

        /** 获取原代码
        @version NIIEngine 3.0.0
        */
        void setName(const String * in)     { mIn = in; }
        
        /** 获取原代码
        @version NIIEngine 3.0.0
        */
        const String * getName() const      { return mIn; }

        /** 合成代码
        @version NIIEngine 3.0.0
        */
        virtual void compound(const String * in, String * out);
        
        /** 合成代码
        @version NIIEngine 3.0.0
        */
        virtual void compound(String * out);
    protected:
        const String * mIn;
        Type mGenType;
        Nid mCodeID;
    };
}
#endif