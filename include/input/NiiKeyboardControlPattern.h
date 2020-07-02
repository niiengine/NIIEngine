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

#ifndef _NII_KEYBOARD_CONTROLPATTERN_H_
#define _NII_KEYBOARD_CONTROLPATTERN_H_

#include "NiiPreInclude.h"
#include "NiiControlPattern.h"
#include "NiiKeyBoardControlItem.h"

namespace NII
{
namespace NII_MEDIA
{
    /** �ⲿ�豸���ƴ�������(����)
    @remark ʹ�õ���104��׼�ļ������
    @note
        ���ڴ��ϵͳ�ں˶�ȡ���̼�¼���Զ��л����,����Ϊ�˸��õ���Ʊ��ģ�鲻Ӧ��
        ��ʹ�ö��л���ģʽ
    @version NIIEngine 3.0.0 �߼�api
    */
    class _EngineAPI KeyboardControlPattern : public ControlPattern
    {
    public:
        /** ��ָ��������
        @remark �����ڲ���������,��ȷ���û�ı����״̬
        @param[in] type ��������
        @version NIIEngine 3.0.0
        */
        void on(KeyboardAlterLockType type);

        /** �ر�ָ��������
        @remark �����ڲ���������,��ȷ���û�ı����״̬
        @paramp[in] type ��������
        @version NIIEngine 3.0.0
        */
        void off(KeyboardAlterLockType type);

        /** ͬ��Ŀ�����״̬
        @remark ͬ�� capslock/scrolllock/numlock ��״̬
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void syn(KeyboardAlterLockType type);

        /** ָ�������Ƿ񱻰���
        @remark �첽״̬
        @param[in] key ����(ʹ�ñ�ʶ)
        @version NIIEngine 3.0.0
        */
        bool isPress(KeyValue key);

        /** ָ�������Ƿ����ͷ�
        @remark �첽״̬
        @param[in] key ����(ʹ�ñ�ʶ)
        @version NIIEngine 3.0.0
        */
        bool isRelease(KeyValue key);

        /** ��ȡʵ�ʰ����ַ��ı�
        @remark Unicode����
        @param[in] key ����(ʹ�ñ�ʶ)
        @version NIIEngine 3.0.0
        */
        virtual Nui32 getKeyCode(KeyValue key);

        /** ��ȡʵ�ʰ����ַ�����
        @remark
        @param[in] key ����(ʹ�ñ�ʶ)
        @version NIIEngine 3.0.0
        */
        virtual String getKeyText(KeyValue key);

        ///@copedetails ControlPattern::getType
        ControlDevType getType() const;
    protected:
        /** Ĭ�Ϲ��캯��
        @remark ʹ��protected����,��ֹ����
        */
        KeyboardControlPattern();

        /** Ĭ�Ϲ��캯��
        @remark ʹ��protected����,��ֹ����
        @param[in] id
            ��������Ψһ��ʶ,���п�������Ψһ(����ͬ����,�Ͳ�ͬ���͵ĸ���)
        @param[in] own ������������ĸ������
        */
        KeyboardControlPattern(Nid id, const ViewWindow * own);

        virtual ~KeyboardControlPattern();

        /** ����±���
        @remark ����������������¼�,
        @param[in] key ����(ʹ�ñ�ʶ)
        @version NIIEngine 3.0.0 �߼�api
        */
        void press(const KeyValue key);

        /** �ͷŰ��±���
        @remark ����������������¼�
        @param[in] key ����(ʹ�ñ�ʶ)
        @version NIIEngine 3.0.0 �߼�api
        */
        void release(const KeyValue key);

        /// @copydetails ControlPattern::apply
        void apply(ControlItem * item);

        /// @copydetails ControlPattern::detach
        void detach();
    protected:
        KeyboardControlItem * mTarget;              ///< ��ǰ�Խ��Ŀ��Ƶ�Ԫ
        Nui32 mBuffer[8];                           ///< ��������
        Nui8 mModifiers;                            ///< ����ģʽ
    };

    /** ����
    @version NIIEngine 3.0.0
    */
    class DummyKeyboardControlPattern : public KeyboardControlPattern
    {
    public:
        DummyKeyboardControlPattern() {}

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