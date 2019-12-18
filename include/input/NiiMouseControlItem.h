/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-6-23

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

#ifndef _NII_MOUSE_CONTROLITEM_H_
#define _NII_MOUSE_CONTROLITEM_H_

#include "NiiPreInclude.h"
#include "NiiControlItem.h"
#include "NiiEventArgs.h"

namespace NII
{
namespace NII_MEDIA
{
    enum MouseButton
    {
        MB_Left = 0,
        MB_Right = 1,
        MB_Middle = 2,
        MB_Button3 = 3,
        MB_Button4 = 4,
        MB_Button5 = 5,
        MB_Button6 = 6,
        MB_Button7 = 7,
        MB_Unknow = 8
    };

    /** ����¼�����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MouseMoveBaseArgs : public EventAlloc
    {
    public:
        MouseMoveBaseArgs();
        MouseMoveBaseArgs(const MouseMoveBaseArgs & o);
        MouseMoveBaseArgs(const MouseMoveArgs * o);
        MouseMoveBaseArgs(const MouseControlPattern * src);
        MouseMoveBaseArgs(const MouseControlPattern * src, Ni32 x, Ni32 y, Ni32 scroll);

        const MouseControlPattern * mSrc;   ///< ������Դ
        Ni32 mX;                            ///< x�����ƶ�����
        Ni32 mY;                            ///< y�����ƶ�����
        Ni32 mScroll;                       ///< wheel��������
    };
    /** ����¼�����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MouseMoveArgs : public EventArgs
    {
    public:
        MouseMoveArgs();
        MouseMoveArgs(const MouseMoveBaseArgs * o);
        MouseMoveArgs(const MouseMoveArgs & o);
        MouseMoveArgs(const MouseControlPattern * src);
        MouseMoveArgs(const MouseControlPattern * src, Ni32 x, Ni32 y, Ni32 scroll);

        const MouseControlPattern * mSrc;   ///< ������Դ
        Ni32 mX;                            ///< x�����ƶ�����
        Ni32 mY;                            ///< y�����ƶ�����
        Ni32 mScroll;                       ///< wheel��������
    };

    /**
    @verison NIIEngine 3.0.0
    */
    class _EngineAPI MousePressBaseArgs : public EventAlloc
    {
    public:
        MousePressBaseArgs();
        MousePressBaseArgs(const MousePressBaseArgs & o);
        MousePressBaseArgs(const MousePressArgs * o);
        MousePressBaseArgs(const MouseControlPattern * src);
        MousePressBaseArgs(const MouseControlPattern * src, MouseButton b);

        const MouseControlPattern * mSrc;   ///< ������Դ
        MouseButton mButton;                ///< ��������
    };
    /**
    @verison NIIEngine 3.0.0
    */
    class _EngineAPI MousePressArgs : public EventArgs
    {
    public:
        MousePressArgs();
        MousePressArgs(const MousePressBaseArgs * o);
        MousePressArgs(const MousePressArgs & o);
        MousePressArgs(const MouseControlPattern * src);
        MousePressArgs(const MouseControlPattern * src, MouseButton b);

        const MouseControlPattern * mSrc;   ///< ������Դ
        MouseButton mButton;                ///< ��������
    };

    typedef void(MouseControlItem::*MouseMethod)(const MousePressArgs *);
    /** �����Ƶ�Ԫ
    @remark ��Ϸ�����������ת�����Ƶ�Ԫ�Ӷ���ͬ��Ч��,�Ѳ����¼�д������ø���
        �Ĵ�����������.�����Ҫʹ����,��Ҫ�̳�������д���ֻ�ȫ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MouseControlItem : public ControlItem
    {
        friend class MouseControlPattern;
    public:
        MouseControlItem();

        virtual ~MouseControlItem();

        /** ������굥Ԫ����������
        @remark ʹ������������Լӿ�����ƶ�
        @par ���Ӽ���,������ϵ
        @version NIIEngine 3.0.0
        */
        void setGain(NIIf factor);

        /** ��ȡ��굥Ԫ����������
        @remark ʹ������������Լӿ�����ƶ�
        @par ���Ӽ���,������ϵ
        @version NIIEngine 3.0.0
        */
        NIIf getGain() const;

        ///@copydetails ControlItem::getType
        ControlDevType getType() const;
    public:
        /** ��������������ʱ����
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onPressLeft(const MousePressArgs * arg);

        /** �������������º��ͷ�ʱ����
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseLeft(const MousePressArgs * arg);

        /** ������Ҽ�������ʱ����
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onPressRight(const MousePressArgs * arg);

        /** ������Ҽ������º��ͷ�ʱ����
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseRight(const MousePressArgs * arg);

        /** ������м�������ʱ����
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onPressMiddle(const MousePressArgs * arg);

        /** ������м������º��ͷ�ʱ����
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMiddle(const MousePressArgs * arg);

        /** ������м�������ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onPressButton3(const MousePressArgs * arg);

        /** ������м������º��ͷ�ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseButton3(const MousePressArgs * arg);

        /** ������м�������ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onPressButton4(const MousePressArgs * arg);

        /** ������м������º��ͷ�ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseButton4(const MousePressArgs * arg);

        /** ������м�������ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onPressButton5(const MousePressArgs * arg);

        /** ������м������º��ͷ�ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseButton5(const MousePressArgs * arg);

        /** ������м�������ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onPressButton6(const MousePressArgs * arg);

        /** ������м�������ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseButton6(const MousePressArgs * arg);

        /** ������м�������ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onPressButton7(const MousePressArgs * arg);

        /** ������м�������ʱ����,���Ķ�����չ(һ�㲻Ҫ��)
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseButton7(const MousePressArgs * arg);

        /** ������ƶ���ʱ�򴥷�
        @param[in] arg ����¼�����
        @version NIIEngine 3.0.0
        */
        virtual void onMove(const MouseMoveArgs * arg);
    protected:
        /** ��ʼ��ӳ���
        @remark ���ٵ���һ��
        */
        static void init();

        /** ����갴������ʱ����
        @param[in] arg
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void onPress(const MousePressArgs * arg);

        /** ����갴���ͷ�ʱ����
        @param[in] arg
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void onRelease(const MousePressArgs * arg);
    protected:
        NIIf mFactor;                        ///< ��������
    };

    /**
    @version NIIEngine 3.0.0
    */
    class DummyMouseControlItem : public MouseControlItem, public ControlAlloc
    {
    public:
        DummyMouseControlItem() {}
    };
}
}
#endif