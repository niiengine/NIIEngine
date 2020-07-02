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
        inline bool isPress(MouseButton button) const { return (mBuffer & button) != 0; }

        /** ָ����갴���Ƿ��ͷ�
        @remark ʵʱ״̬
        @param[in] button ����(ʹ�ñ�ʶ)
        @version NIIEngine 3.0.0
        */
        inline bool isRelease(MouseButton button) const { return (mBuffer & button) == 0;  }

        /** ��ȡ��ǰ���X����
        @remark �������Ⱦ����(�豸������)
        @version NIIEngine 3.0.0
        */
        inline Nui32 getX() const { return mX; }

        /** ��ȡ��ǰ���Y����
        @remark �������Ⱦ����(�豸������)
        @version NIIEngine 3.0.0
        */
        inline Nui32 getY() const { return mY; }
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
        Nui32 mX;                   ///< ��ǰ���X����(�������Ⱦ����)
        Nui32 mY;                   ///< ��ǰ���Y����(�������Ⱦ����)
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