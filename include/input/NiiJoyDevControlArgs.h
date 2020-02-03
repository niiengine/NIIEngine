/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-5-8

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

#ifndef _NII_JOYDEV_CONTROL_ARGS_H_
#define _NII_JOYDEV_CONTROL_ARGS_H_

#include "NiiPreInclude.h"
#include "NiiEventArgs.h"
#include "NiiPovDirection.h"

namespace NII
{
namespace NII_MEDIA
{
    enum JoyDevControlType
    {
        JDCT_Button,
        JDCT_Slider,
        JDCT_Pov,
        JDCT_Vector,
        JDCT_Axis,
        JDCT_Wheel
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyDevControlArgs : public EventArgs
    {
    public:
        JoyDevControlArgs(JoyDevControlType t) : mType(t) {}
        virtual ~JoyDevControlArgs() {}
        
        JoyDevControlType mType;   ///< ��������,��Ҫֱ���޸���
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyHandleControlVectorArgs : public JoyDevControlArgs
    {
    public:
        JoyHandleControlVectorArgs(NIIi v, NIIf x, NIIf y, NIIf z) :
            JoyDevControlArgs(JDCT_Vector),
            mVector(v),
            mX(x),
            mY(y),
            mZ(z) {}

        NIIi mVector;   ///< vector����
        NIIf mX;        ///< x����
        NIIf mY;        ///< y����
        NIIf mZ;        ///< z����
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyPadControlAxisArgs : public JoyDevControlArgs
    {
    public:
        JoyPadControlAxisArgs(Nui8 a, NIIi rel) :
            JoyDevControlArgs(JDCT_Axis),
            mAxis(a),
            mRel(rel) {}

        Nui8 mAxis;                         ///< ҡ�˵�����
        NIIi mRel;                          ///< ҡ��������
    };

    /** �ֱ����Ʋ���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyPadControlButtonArgs : public JoyDevControlArgs
    {
    public:
        JoyPadControlButtonArgs(Nui8 b) :
            JoyDevControlArgs(JDCT_Button),
            mButton(b) {}

        Nui8 mButton;                       ///< ����������
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyStickControlPovArgs : public JoyDevControlArgs
    {
    public:
        JoyStickControlPovArgs(Nui8 pov, PovType dir) :
            JoyDevControlArgs(JDCT_Pov),
            mPov(pov),
            mDirection(dir) {}

        Nui8 mPov;                      ///< ����
        PovType mDirection;             ///< pov����
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyStickControlSliderArgs : public JoyDevControlArgs
    {
    public:
        JoyStickControlSliderArgs(Nui8 s, const Vector2<NIIi> & data) :
            JoyDevControlArgs(JDCT_Slider),
            mSlider(s),
            mData(data) {}

        const Vector2<NIIi> & mData;        ///< ����������
        Nui8 mSlider;                       ///< slider����
    };

    /** ҡ��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyWheelControlArgs : public JoyDevControlArgs
    {
    public:
        JoyWheelControlArgs(NIIf w) :
            JoyDevControlArgs(JDCT_Wheel),
            mWheel(w) {}

        NIIf mWheel;
    };
}
}
#endif