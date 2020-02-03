/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-5-7

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
      ��άӦ�����
        �������������������(CAD)
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
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
    /** �¼��ص�ִ�еĺ�����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Functor : public EventAlloc
    {
    public:
        virtual ~Functor();

        /** ���к���
        @param[in] args ����
        @version NIIEngine 3.0.0
        */
        inline bool operator()(const EventArgs * args)
        {
            return execute(args);
        }

        /** ���к���
        @param[in] args ����
        @note ������Ҫ��д�������
        @version NIIEngine 3.0.0
        */
        virtual bool execute(const EventArgs * args);
        
        /** ������������
        @remark 
            �����һ������,��ʹ�� N_new ����������,����ʹ�������Ҫ�� N_delete ����
        @note ������Ҫ��д�������
        @version NIIEngine 3.0.0
        */
        virtual Functor * clone() const;
    };

    /// ����
    class _EngineInner DummyFunctor : public Functor
    {
    public:
        DummyFunctor();
        ~DummyFunctor();
        
        ///@copydetails Functor()
        bool execute(const EventArgs * args);
    };

    /** �����ͺ���
    @remark
    @version NIIEngine 3.0.0
    */
    class RefFunctor : public Functor
    {
    public:
        RefFunctor(Functor & func);

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

        /// @copydetails Functor::execute
        bool execute(const EventArgs * args);

        /// @copydetails Functor::clone
        Functor * clone() const;
    private:
        Functor * mReal;
    };

    typedef void(Event::*EventMethod)(const EventArgs *);

    /** ��ͨ�ĺ���
    @remark
        ӵ�� bool ���غ� const EventArgs * ��Ϊ��������ͨ����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MethodFunctor : public Functor
    {
    public:
        MethodFunctor(Event * obj, EventMethod m);

        /// @copydetails Functor::execute
        bool execute(const EventArgs * args);

        /// @copydetails Funcotr::clone
        Functor * clone() const;
    protected:
        Event * mObj;
        EventMethod mMethod;
    };

    /** �����ͺ���
    @remark
    @note Functor �е� clone ��Ϊ����������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CopyFunctor : public Functor
    {
    public:
        CopyFunctor(const Functor & func);
        ~CopyFunctor();

        /// @copydetails Functor::operator()
        bool execute(const EventArgs * args);

        /// @copydetails Functor::clone
        Functor * clone() const;
    private:
        Functor * mReal;
    };

    typedef void(CommandObj::*CmdObjMemberFunc)(const EventArgs *);

    /** �������ĳ�Ա����
    @remark
        �����¼������е����������еĳ�Ա����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CommandObjFunctor : public Functor
    {
    public:
        CommandObjFunctor(CommandObj * obj, CmdObjMemberFunc mf);

        /// @copydetails Functor::execute
        bool execute(const EventArgs * args);

        /// @copydetails Functor::clone
        Functor * clone() const;
    private:
        CommandObj * mObj;
        CmdObjMemberFunc mMethod;
    };
}
}

#endif