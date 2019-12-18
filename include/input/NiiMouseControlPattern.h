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

#ifndef _NII_MOUSE_CONTROLPATTERN_H_
#define _NII_MOUSE_CONTROLPATTERN_H_

#include "NiiPreInclude.h"
#include "NiiControlPattern.h"
#include "NiiMouseControlItem.h"
#include "NiiMouseControlItem.h"

namespace NII
{
namespace NII_MEDIA
{
    /** ��������
    @remark ʹ���й��������׼���
    @note 
        ���ڴ��ϵͳ�ں˶�ȡ����¼���Զ��л����,����Ϊ�˸��õ���Ʊ��ģ�鲻Ӧ��
        ��ʹ�ö��л���ģʽ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MouseControlPattern : public ControlPattern
    {
    public:
        /** ָ����갴���Ƿ���
        @remark ʵʱ״̬
        @param[in] button ����(ʹ�ñ�ʶ)
        @version NIIEngine 3.0.0
        */
        bool isPress(MouseButton button) const;

        /** ָ����갴���Ƿ��ͷ�
        @remark ʵʱ״̬
        @param[in] button ����(ʹ�ñ�ʶ)
        @version NIIEngine 3.0.0
        */
        bool isRelease(MouseButton button) const;

        /** ��ȡ��ǰ���X����
        @remark �������Ⱦ����(�豸������)
        @version NIIEngine 3.0.0
        */
        Ni32 getX() const;

        /** ��ȡ��ǰ���Y����
        @remark �������Ⱦ����(�豸������)
        @version NIIEngine 3.0.0
        */
        Ni32 getY() const;
    protected:
        /** Ĭ�Ϲ��캯��
        @version NIIEngine 3.0.0 �߼�api
        */
        MouseControlPattern();

        /** Ĭ�Ϲ��캯��
        @param[in] id ��������Ψһ��ʶ,���п�������Ψһ(����ͬ����,�Ͳ�ͬ���͵ĸ���)
        @param[in] own ������������ĸ������
        @version NIIEngine 3.0.0 �߼�api
        */
        MouseControlPattern(Nid id, const ViewWindow * own);

        virtual ~MouseControlPattern();

        /** ������갴��
        @remark �������������갴�������¼�
        @param[in] button ����(ʹ�ñ�ʶ)
        @version NIIEngine 3.0.0 �߼�api
        */
        void press(MouseButton button);

        /** �ɿ���갴��
        @remark �������������갴���ͷ��¼�
        @param[in] button ����(ʹ�ñ�ʶ)
        @version NIIEngine 3.0.0 �߼�api
        */
        void release(MouseButton button);

        /** �ƶ����
        @remark ���������������ƶ��¼�
        @param[in] x x�����ƶ���
        @param[in] y y�����ƶ���
        @param[in] z (����)�ƶ���
        @version NIIEngine 3.0.0 �߼�api
        */
        void move(Ni32 x, Ni32 y, Ni32 z);

        /// @copydetails ControlPattern::apply
        void apply(ControlItem * item);

        /// @copydetails ControlPattern::detach
        void detach();

        ///@copedetails ControlPattern::getType
        ControlDevType getType() const;
    protected:
        Nui32 mWindowSizeX;         ///< �����ΧX��С
        Nui32 mWindowSizeY;         ///< �����ΧY��С
        Ni32 mX;                    ///< ��ǰ���X����(�������Ⱦ����)
        Ni32 mY;                    ///< ��ǰ���Y����(�������Ⱦ����)
        Ni32 mBuffer;               ///< ��������
        MouseControlItem * mTarget; ///< ��ǰ�Խ��Ŀ��Ƶ�Ԫ
    };

    /** ����
    @version NIIEngine 3.0.0
    */
    class DummyMouseControlPattern : public MouseControlPattern
    {
    public:
        DummyMouseControlPattern() {}
        ///@copydetails ControlPattern::_init
        bool _init() { return true; }

        ///@copydetails ControlPattern::_end
        bool _end() { return true; }

        ///@copydetails ControlPattern::_update
        void _update() {}

        ///@copydetails KeyboardControlPattern::_reset
        void _reset() {}

        ///@copydetails KeyboardControlPattern::_start
        void _start() {}

        ///@copydetails KeyboardControlPattern::_pause
        void _pause() {}

        ///@copydetails KeyboardControlPattern::_stop
        void _stop() {}
    };
}
}
#endif