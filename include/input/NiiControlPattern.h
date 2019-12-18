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

#ifndef _NII_CONTROL_PATTERN_H_
#define _NII_CONTROL_PATTERN_H_

#include "NiiPreInclude.h"
#include "NiiPattern.h"
#include "NiiControlDevType.h"

namespace NII
{
namespace NII_MEDIA
{
    /** ������
    @remark ���������������Ŀ��Ƶ�Ԫʵ���鷽��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ControlPattern : public Pattern
    {
        friend class ControlPatternManager;
    public:
        typedef vector<ControlItem *>::type ItemList;
    public:
        /// ���������豸��������
        virtual ControlDevType getType() const = 0;
    protected:
        /// ��ֹ����
        ControlPattern();

        /** ʹ��ControlPatternManger,����ֱ��ʹ���������
        @param[in] id ͬ�����豸�б�����Ψһ��
        @param[in] own �������ڵĶ���
        @param[in] type �����豸��������
        */
        ControlPattern(Nid id, const ViewWindow * own);

        virtual ~ControlPattern();

        /** �����豸�Ĺ�Ӧ��
        @param[in] v ��Ӧ�̵�����
        @version NIIEngine 3.0.0
        */
        void setVendor(const String & v);

        /** �󶨿��ƴ���
        @remark �ڲ�����
        @param[in] win ��Ҫ�󶨵Ŀ��ƴ���
        @version NIIEngine 3.0.0
        */
        virtual void bind(const ViewWindow * win);

        /** Ӧ�ÿ��Ƶ�Ԫ
        @remark �ڲ�����
        @param[in] item ��ҪӦ�õĿ��Ƶ�Ԫ
        @version NIIEngine 3.0.0
        */
        virtual void apply(ControlItem * item) = 0;

        /** �����ǰ���Ƶ�Ԫ�İ�
        @remark �ڲ�����
        @version NIIEngine 3.0.0
        */
        virtual void detach() = 0;

        /** ����������
        @remark ���̳߳��е�
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void _reset() = 0;

        /** ��ʼ���п�����
        @remark ���̳߳��е�
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void _start() = 0;

        /** ��ͣ���п�����
        @remark ���̳߳��е�
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void _pause() = 0;

        /** ֹͣ���п�����
        @remark ���̳߳��е�
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void _stop() = 0;
    protected:
        const ViewWindow * mOwn;///< �������ڵĶ���
        Nid mID;                ///< �ж��ͬ���͵������豸��ʱ������
        String mVendor;         ///< ��Ӧ��(�豸������)��Ϣ
        bool mActive;           ///< �豸���Ƿ�
        
    };
}
}
#endif