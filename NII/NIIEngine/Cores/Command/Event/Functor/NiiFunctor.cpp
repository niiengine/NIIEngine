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

#include "NiiPreProcess.h"
#include "NiiFunctor.h"

namespace NII
{
namespace NII_COMMAND
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Functor
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    Functor::~Functor(){}
	//------------------------------------------------------------------------
	bool Functor::operator ==(const Functor & o) const
	{
		return false;
	}
    //------------------------------------------------------------------------
    bool Functor::execute(const EventArgs * args)
    {
        return false;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // DummyFunctor
    //------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool DummyFunctor::operator ==(const Functor & o) const
	{
		return false;
	}
    //------------------------------------------------------------------------
    bool DummyFunctor::execute(const EventArgs *)
    {
        return false;
    }
	//------------------------------------------------------------------------
	Functor * DummyFunctor::clone() const
	{
		return 0;
	}
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // RefFunctor
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    RefFunctor::RefFunctor(Functor & func) :
        mReal(func)
    {

    }
    //------------------------------------------------------------------------
    bool RefFunctor::operator ==(const Functor & o) const
    { 
		const RefFunctor * temp = dynamic_cast<const RefFunctor *>(&o);
		if (temp)
			return mReal == temp->mReal;
        return false; 
    }
    //------------------------------------------------------------------------
    bool RefFunctor::execute(const EventArgs * args)
    {
        return mReal(args);
    }
    //------------------------------------------------------------------------
    Functor * RefFunctor::clone() const
    {
        return N_new RefFunctor(mReal);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // PtrFunctor
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    PtrFunctor::PtrFunctor(Functor * func) :
        mReal(func)
    {
    }
    //------------------------------------------------------------------------
    bool PtrFunctor::operator ==(const Functor & o) const
    { 
		const PtrFunctor * temp = dynamic_cast<const PtrFunctor *>(&o);
		if (temp)
			return *mReal == *temp->mReal;
        return false; 
    }
    //------------------------------------------------------------------------
    bool PtrFunctor::execute(const EventArgs * args)
    {
        return (*mReal)(args);
    }
    //------------------------------------------------------------------------
    Functor * PtrFunctor::clone() const
    {
        return N_new PtrFunctor(mReal);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // MethodFunctor
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    MethodFunctor::MethodFunctor(Event * obj, EventMethod m) :
        mObj(obj),
        mMethod(m)
    {
    }
    //------------------------------------------------------------------------
    bool MethodFunctor::operator ==(const Functor & o) const
    { 
		const MethodFunctor * temp = dynamic_cast<const MethodFunctor *>(&o);
		if (temp)
			return mObj == temp->mObj && mMethod == temp->mMethod;
        return false; 
    }
    //------------------------------------------------------------------------
    bool MethodFunctor::execute(const EventArgs * args)
    {
        (mObj->*mMethod)(args);
        return true;
    }
    //------------------------------------------------------------------------
    Functor * MethodFunctor::clone() const
    {
        return N_new MethodFunctor(mObj, mMethod);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // CopyFunctor
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    CopyFunctor::CopyFunctor(const Functor & func)
    {
        mReal = func.clone();
    }
    //------------------------------------------------------------------------
    CopyFunctor::~CopyFunctor()
    {
        if (mReal)
        {
            N_delete mReal;
            mReal = 0;
        }
    }
    //------------------------------------------------------------------------
    bool CopyFunctor::operator ==(const Functor & o) const
    { 
		const CopyFunctor * temp = dynamic_cast<const CopyFunctor *>(&o);
		if (temp)
			return *mReal == *temp->mReal;
        return false; 
    }
    //------------------------------------------------------------------------
    bool CopyFunctor::execute(const EventArgs * args)
    {
        return (*mReal)(args);
    }
    //------------------------------------------------------------------------
    Functor * CopyFunctor::clone() const
    {
        return N_new CopyFunctor(*mReal);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // CommandFunctor
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    CommandFunctor::CommandFunctor(CommandObj * obj, CommandObjFunctor mf) :
        mObj(obj),
        mMethod(mf)
    {
    }
    //------------------------------------------------------------------------
    bool CommandFunctor::operator ==(const Functor & o) const
    {
		const CommandFunctor * temp = dynamic_cast<const CommandFunctor *>(&o);
		if(temp)
			return mObj == temp->mObj && mMethod == temp->mMethod;
        return false; 
    }
    //------------------------------------------------------------------------
    bool CommandFunctor::execute(const EventArgs * args)
    {
        (mObj->*mMethod)(args);
        return true;
    }
    //------------------------------------------------------------------------
    Functor * CommandFunctor::clone() const
    {
        return N_new CommandFunctor(mObj, mMethod);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // GameObjFunctor
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    GameObjFunctor::GameObjFunctor()
    {

    }
    //------------------------------------------------------------------------
    GameObjFunctor::~GameObjFunctor()
    {

    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // GenerateObjFunctor
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    bool GenerateObjFunctor::operator ==(const Functor & o) const
    {
        return false;
    }
    //------------------------------------------------------------------------
    bool GenerateObjFunctor::execute(const EventArgs * args)
    {
        return false;
    }
    //------------------------------------------------------------------------
    Functor * GenerateObjFunctor::clone() const
    {
        return N_new GenerateObjFunctor();
    }
    //------------------------------------------------------------------------
    void GenerateObjFunctor::Generate(EventObj * target)
    {

    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // DeleteObjFunctor
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    bool DeleteObjFunctor::operator ==(const Functor & o) const
    {
        return false;
    }
    //------------------------------------------------------------------------
    bool DeleteObjFunctor::execute(const EventArgs * args)
    {
        return false;
    }
    //------------------------------------------------------------------------
    Functor * DeleteObjFunctor::clone() const
    {
        return N_new DeleteObjFunctor();
    }
    //------------------------------------------------------------------------
    void DeleteObjFunctor::Delete(EventObj * target)
    {

    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // AdjustObjFunctor
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    bool AdjustObjFunctor::operator ==(const Functor & o) const
    {
        return false;
    }
    //------------------------------------------------------------------------
    bool AdjustObjFunctor::execute(const EventArgs * args)
    {
        return false;
    }
    //------------------------------------------------------------------------
    Functor * AdjustObjFunctor::clone() const
    {
        return N_new AdjustObjFunctor();
    }
    //------------------------------------------------------------------------
    void AdjustObjFunctor::Adjust(EventObj * target)
    {

    }
    //------------------------------------------------------------------------
}
}