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

#ifndef _NII_GAME_OBJECT_FUNCTOR_H_
#define _NII_GAME_OBJECT_FUNCTOR_H_

#include "NiiPreInclude.h"
#include "NiiFunctor.h"

namespace NII
{
namespace NII_COMMAND
{
    /// 游戏对象事件的处理函子
    class GameObjFunctor : public Functor
    {
    public:
        GameObjFunctor();
        virtual ~GameObjectFunctor();
    };

    /** 调整对象事件的处理函子,调整的对象均是接收命令的对象
    @version NIIEngine 3.0.0
    */
    class AdjustObjFunctor : public GameObjFunctor
    {
    public:
        /// @copydetails Functor::execute
        bool execute(const EventArgs * args);

        /// @copydetails Functor::clone
        Funcotr * clone() const;
    protected:
        void Adjust(EventObj * target);
    };

    /// 删除对象事件的处理函子,删除的对象均是接收命令的对象
    class DeleteObjFunctor : public GameObjFunctor
    {
    public:
        /// @copydetails Functor::execute
        bool execute(const EventArgs * args);

        /// @copydetails Functor::clone
        Functor * clone() const;
    protected:
        void Delete(EventObj * target);
    };

    /// 生成对象事件的处理函子,生成的对象均是接收命令的对象的类型
    class GenerateObjFunctor : public GameObjFunctor
    {
    public:
        /// @copydetails Functor::execute
        bool execute(const EventArgs * args);

        /// @copydetails Functor::clone
        Functor * clone() const;
    protected:
        void Generate(EventObj * target);
    };
}
}
#endif