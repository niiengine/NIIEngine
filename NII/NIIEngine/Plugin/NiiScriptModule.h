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

#ifndef _NII_ScriptModule_H_
#define _NII_ScriptModule_H_

#include "NiiPreInclude.h"
#include "NiiCommandObj.h"

namespace NII
{
    /** �ű�ģ��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScriptModule : public UIAlloc
    {
    public:
        ScriptModule();
        virtual ~ScriptModule();

        /** ���ؽű�ģ��
        @version NIIEngine 3.0.0
        */
        virtual void load();

        /** ж�ؽű�ģ��
        @version NIIEngine 3.0.0
        */
        virtual void unload();

        /** �ű�ģ��ID
        @version NIIEngine 3.0.0
        */
        ScriptModuleID getID() const;

        /** ִ�нű��ļ�
        @version NIIEngine 3.0.0
        */
        virtual void executeFile(const String & file, GroupID gid = 0) = 0;

        /** ִ�нű�����
        @version NIIEngine 3.0.0
        */
        virtual int executeFunc(const String & func) = 0;

        /** ִ�нű�����
        @param func ����
        @param arg ����
        @version NIIEngine 3.0.0
        */
        virtual bool executeFunc(const String & func, const EventArgs & arg) = 0;

        /** ִ�нű�
        @version NIIEngine 3.0.0
        */
        virtual void executeScript(const String & script) = 0;

        /** ����ű����������������
        @param obj �������
        @param eid �¼�ID
        @param func �ű�����
        @version NIIEngine 3.0.0
        */
        virtual SignalPtr add(NII_COMMAND::CommandObj * obj, EventID eid, const String & func) = 0;

        /** �ű�ģ��
        @version NIIEngine 3.0.0
        */
        static void setGroup(GroupID gid);

        /** �ű�ģ����Դ��
        @version NIIEngine 3.0.0
        */
        static GroupID getGroup();
    protected:
        ScriptModuleID mID;
        static GroupID mResourceGroup;
    };

    class _EngineAPI ScriptFunctor : public SysAlloc
    {
    public:
        ScriptFunctor(ScriptModuleID mid, const String & fc) :mModuleID(mid), mFuncName(fc) {}
        ScriptFunctor(const ScriptFunctor & o) : mFuncName(o.mFuncName) {}
        bool operator()(const EventArgs & e) const;
    private:
        ScriptFunctor & operator=(const ScriptFunctor & o);

        String mFuncName;
        ScriptModuleID mModuleID;
    };
}
#endif