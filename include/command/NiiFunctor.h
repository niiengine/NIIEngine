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

#ifndef _NII_FUNCTOR_H_
#define _NII_FUNCTOR_H_

#include "NiiPreInclude.h"
#include "NiiEventArgs.h"

namespace NII
{
namespace NII_COMMAND
{
    /** 事件回调执行的函子类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Functor : public EventAlloc
    {
    public:
        virtual ~Functor();

		/**
		@version NIIEngine 3.0.0
		*/
		virtual bool operator ==(const Functor & o) const = 0;

		/**
		@version NIIEngine 3.0.0
		*/
		inline bool operator !=(const Functor & o) const
		{
			return !operator ==(o);
		}

        /** 运行函子
        @param[in] args 因子
        @version NIIEngine 3.0.0
        */
        inline bool operator()(const EventArgs * args)
        {
            return execute(args);
        }

        /** 运行函子
        @param[in] args 因子
        @note 子类需要重写这个函数
        @version NIIEngine 3.0.0
        */
        virtual bool execute(const EventArgs * args);
        
        /** 产生副本对象
        @remark 
            会产生一个副本,是使用 N_new 创建出来的,所以使用完后需要用 N_delete 销毁
        @note 子类需要重写这个函数
        @version NIIEngine 3.0.0
        */
		virtual Functor * clone() const = 0;
    };

    /// 傀儡
    class _EngineInner DummyFunctor : public Functor
    {
    public:
        DummyFunctor();
        ~DummyFunctor();
        
		///@copydetails Functor::operator ==
		bool operator ==(const Functor & o) const;

        ///@copydetails Functor::execute
        bool execute(const EventArgs * args);

		///@copydetails Functor::clone
		virtual Functor * clone() const;
    };

    /** 引用型函数
    @remark
    @version NIIEngine 3.0.0
    */
    class RefFunctor : public Functor
    {
    public:
        RefFunctor(Functor & func);

		///@copydetails Functor::operator ==
		bool operator ==(const Functor & o) const;

        /// @copydetails Functor::execute
        bool execute(const EventArgs * args);

        /// @copydetails Functor::clone
        Functor * clone() const;
    private:
        Functor & mReal;
    };

    class PtrFunctor : public Functor
    {
    public:
        PtrFunctor(Functor * func);

		///@copydetails Functor::operator ==
		bool operator ==(const Functor & o) const;

        /// @copydetails Functor::execute
        bool execute(const EventArgs * args);

        /// @copydetails Functor::clone
        Functor * clone() const;
    private:
        Functor * mReal;
    };

    typedef void(Event::*EventMethod)(const EventArgs *);

    /** 普通的函数
    @remark
        拥有 bool 返回和 const EventArgs * 作为参数的普通函数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MethodFunctor : public Functor
    {
    public:
        MethodFunctor(Event * event, EventMethod m);

		///@copydetails Functor::operator ==
		bool operator ==(const Functor & o) const;

        /// @copydetails Functor::execute
        bool execute(const EventArgs * args);

        /// @copydetails Funcotr::clone
        Functor * clone() const;
    protected:
		Event * mObj;
		EventMethod mMethod;
    };

    /** 副本型函数
    @remark
    @note Functor 中的 clone 是为这个类而生的
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CopyFunctor : public Functor
    {
    public:
        CopyFunctor(const Functor & func);
        ~CopyFunctor();

		///@copydetails Functor::operator ==
		bool operator ==(const Functor & o) const;

        /// @copydetails Functor::operator()
        bool execute(const EventArgs * args);

        /// @copydetails Functor::clone
        Functor * clone() const;
    private:
        Functor * mReal;
    };

    typedef void(CommandObj::*COFunc)(const EventArgs *);
	typedef COFunc CommandObjFunctor;
	template <typename _class, typename _earg> COFunc convCmdObjFunc(void (_class::*f)(const _earg *))
	{
		return static_cast<COFunc>(reinterpret_cast<void (_class::*)(const EventArgs *)>(f));
	}

    /** 命令对象的成员函数
    @remark
        引擎事件机制中的命令对象独有的成员函数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CommandFunctor : public Functor
    {
    public:
        CommandFunctor(CommandObj * obj, CommandObjFunctor mf);

		///@copydetails Functor::operator ==
		bool operator ==(const Functor & o) const;

        /// @copydetails Functor::execute
        bool execute(const EventArgs * args);

        /// @copydetails Functor::clone
        Functor * clone() const;
    private:
        CommandObj * mObj;
        CommandObjFunctor mMethod;
    };

	template <typename _class, typename _earg>
		class ObjFunctor : public Functor
	{
	public:
		ObjFunctor(_class * obj, void (_class::*mf)(const _earg *)) :
			mObj(obj),
			mMethod(mf)
		{
		}

		///@copydetails Functor::operator ==
		bool operator ==(const Functor & o) const
		{
			const ObjFunctor<_class, _earg> * temp = dynamic_cast<const ObjFunctor<_class, _earg> *>(&o);
			if (temp)
				return mObj == temp->mObj && mMethod == temp->mMethod;
			return false;
		}

		///@copydetails Functor::execute
		bool execute(const EventArgs * args)
		{
			(mObj->*mMethod)(static_cast<const _earg *>(args));
			return true;
		}

		///@copydetails Functor::clone
		Functor * clone() const
		{
			return N_new ObjFunctor(mObj, mMethod);
		}
	private:
		_class * mObj;
		void (_class::*mMethod)(const _earg *);
	};
}
}

#endif