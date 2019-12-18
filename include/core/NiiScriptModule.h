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
		virtual	void executeFile(const String & file, GroupID gid = 0) = 0;

		/** ִ�нű�����
		@version NIIEngine 3.0.0
		*/
		virtual int	executeFunc(const String & func) = 0;

		/** ִ�нű�����
		@param func ����
		@param arg ����
		@version NIIEngine 3.0.0
		*/
		virtual	bool executeFunc(const String & func, const EventArgs & arg) = 0;

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