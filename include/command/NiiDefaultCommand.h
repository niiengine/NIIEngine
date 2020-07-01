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

#ifndef _NII_DELETE_COMMAND_
#define _NII_DELETE_COMMAND_

#include "NiiPreInclude.h"
#include "NiiCommand.h"
#include "NiiDefaultEventArgs.h"

namespace NII
{
namespace NII_COMMAND
{
    /** (�ɽ�������Ķ���)�Ĵ�������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CreateCommand : public Command
    {
    public:
        CreateCommand();
        ~CreateCommand();

        ///@copydetails Command::main
        void main();
    protected:
        /** ����������ִ�к��ͬʱ�����������
        @version NIIEngine 3.0.0
        */
        void onCreated(const GenerateObjEventArgs & arg);
    private:
        GenerateObjEventArgs * mArg;
    };
    
    /** (�ɽ�������Ķ���)��ɾ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DeleteCommand : public Command
    {
    public:
        DeleteCommand();
        ~DeleteCommand();

        ///@copydetails Command::main
        void main();
    protected:
        /** ��ɾ������ִ�к��ͬʱ�����������
        @version NIIEngine 3.0.0
        */
        void onDeleted(const DeleteObjEventArgs & arg);
    private:
        DeleteObjEventArgs * mArg;
    };
    
    /** (�ɽ�������Ķ���)�ĵ�������������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SetCommand : public Command
    {
    public:
        SetCommand();
        ~SetCommand();

        ///@copydetails Command::main
        void main();
    protected:
        /** ��������������ִ�к��ͬʱ�����������
        @version NIIEngine 3.0.0
        */
        void onSet(const AdjustObjEventArgs & arg);
    protected:
        AdjustObjEventArgs * mArg;
    };
}
}

#endif