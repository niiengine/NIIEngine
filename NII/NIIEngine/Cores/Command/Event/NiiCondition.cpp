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
#include "NiiCondition.h"

namespace NII
{
namespace NII_COMMAND
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Command
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    Command::Command() {}
    //------------------------------------------------------------------------
    Command::~Command()
    {
        Funcs::iterator i, iend = mExecutes.end();
        for (i = mExecutes.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mExecutes.clear();
    }
    //------------------------------------------------------------------------
    void Command::add(EventFunctor * func)
    {
        Funcs::iterator i, iend = mExecutes.end();
        for (i = mExecutes.begin(); i != iend; ++i)
        {
            if (**i == *func)
                return;
        }
        mExecutes.push_back(func);
    }
    //------------------------------------------------------------------------
    void Command::remove(EventFunctor * func)
    {
        Funcs::iterator i, iend = mExecutes.end();
        for (i = mExecutes.begin(); i != iend; ++i)
        {
            if (**i == *func)
            {
                N_delete *i;
                mExecutes.erase(i);
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    void Command::operator()(const EventArgs * arg)
    {
        Funcs::iterator i, iend = mExecutes.end();
        for (i = mExecutes.begin(); i != iend; ++i)
        {
            (**i)(arg);
        }

        main();

        onDone(arg);
    }
    //------------------------------------------------------------------------
    void Command::main()
    {
    }
    //------------------------------------------------------------------------
    void Command::onDone(const EventArgs * arg)
    {
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // CreateCommand
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    CreateCommand::CreateCommand()
    {

    }
    //------------------------------------------------------------------------
    CreateCommand::~CreateCommand()
    {

    }
    //------------------------------------------------------------------------
    void CreateCommand::main()
    {

    }
    //------------------------------------------------------------------------
    void CreateCommand::onCreated(const GenerateObjEventArgs & arg)
    {

    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // DeleteCommand
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    DeleteCommand::DeleteCommand()
    {

    }
    //------------------------------------------------------------------------
    DeleteCommand::~DeleteCommand()
    {

    }
    //------------------------------------------------------------------------
    void DeleteCommand::main()
    {

    }
    //------------------------------------------------------------------------
    void DeleteCommand::onDeleted(const DeleteObjEventArgs & arg)
    {

    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ModifyCommand
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ModifyCommand::ModifyCommand()
    {

    }
    //------------------------------------------------------------------------
    ModifyCommand::~ModifyCommand()
    {

    }
    //------------------------------------------------------------------------
    void ModifyCommand::main()
    {

    }
    //------------------------------------------------------------------------
    void ModifyCommand::onSet(const AdjustObjEventArgs & arg)
    {

    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ConditionParam
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    Condition::ConditionParam::ConditionParam(Condition * own, CmpMode op, EventArgs * ex) :
        mIndex(0),
        mOwn(own),
        mOperator(op),
        mExpect(ex)
    {
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Condition
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    Condition::Condition() :
        mExec(0),
        mAutoReset(false)
    {

    }
    //------------------------------------------------------------------------
    Condition::Condition(Command * cmd) : 
        mExec(cmd),
        mAutoReset(false)
    {
    
    }
    //------------------------------------------------------------------------
    Condition::~Condition()
    {
        mParams.clear();
    }
    //------------------------------------------------------------------------
    void Condition::add(EventID id, CmpMode op, EventArgs * ex)
    {
        ConditionParams::iterator i = mParams.find(id);
        if(i != mParams.end())
        {
            ConditionParam * temp = i->second;
            temp->mOperator = op;
            N_delete temp->mExpect;
            temp->mExpect = ex;
        }
        else
        {
            ConditionParam * temp = N_new ConditionParam(this, op, ex);
            mParams.insert(Npair(id, temp));
        }
    }
    //------------------------------------------------------------------------
    void Condition::remove(EventID mid)
    {
        ConditionParams::iterator i = mParams.find(mid);
        if(i != mParams.end())
        {
            N_delete i->second->mExpect;
            N_delete i->second;
            mParams.erase(i);
        }
    }
    //------------------------------------------------------------------------
    const Condition::ConditionParam * Condition::find(EventID mid) const
    {
        ConditionParams::const_iterator i = mParams.find(mid);
        if(i != mParams.end())
            return i->second;
        else
            return NULL;
    }
    //------------------------------------------------------------------------
    void Condition::exec(const EventArgs * args)
    {
        if (mExec)
            (*mExec)(args);
    }
    //------------------------------------------------------------------------
    void Condition::setExec(Command * cmd)
    {
        mExec = cmd;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // DefaultCondition
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    DefaultCondition::DefaultCondition() :
        mComparesMark(0) {}
    //------------------------------------------------------------------------
    DefaultCondition::DefaultCondition(Command * cmd) :
        Condition(cmd),
        mComparesMark(0) {}
    //------------------------------------------------------------------------
    DefaultCondition::~DefaultCondition() {}
    //------------------------------------------------------------------------
    void DefaultCondition::compile()
    {
        Nui8 index = 0;
        ConditionParams::iterator it, itend = mParams.end();
        for (it = mParams.begin(); it != itend; ++it, ++index)
        {
            it->second->mIndex = index;
        }
        mComparesMark = 0;
    }
    //------------------------------------------------------------------------
    void DefaultCondition::reach()
    {
        mComparesMark = 0xffffffff;
    }
    //------------------------------------------------------------------------
    void DefaultCondition::fail()
    {
        mComparesMark = 0;
    }
    //------------------------------------------------------------------------
    void DefaultCondition::reach(const ConditionParam * cp)
    {
        mComparesMark |= N32_1OrMark[cp->mIndex];
    }
    //------------------------------------------------------------------------
    void DefaultCondition::fail(const ConditionParam * cp)
    {
        mComparesMark &= N32_1NotMark[cp->mIndex];
    }
    //------------------------------------------------------------------------
    bool DefaultCondition::reached()
    {
        if (mParams.size() == 0)
            return true;
        if (N32_1AndMark[mParams.size() - 1] == mComparesMark)
            return true;
        return false;
    }
    //------------------------------------------------------------------------
}
}