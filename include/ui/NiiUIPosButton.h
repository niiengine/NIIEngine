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

#ifndef _NII_UI_POSBUTTON_H_
#define _NII_UI_POSBUTTON_H_

#include "NiiUIPreInclude.h"
#include "NiiUIPushButton.h"

namespace NII
{
namespace UI
{
    /** ��¼��Ŧ
    @remark ��¼λ��״̬�İ�Ŧ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PosButton : public PushButton
    {
    public:
        PosButton(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        virtual ~PosButton();

        /** �����Ƿ�ͬ��λ���¼�
        @remark 
        @version NIIEngine 3.0.0
        */
        void setPosEvent(bool b);

        /** ��ȡ�Ƿ�ͬ��λ���¼�
        @version NIIEngine 3.0.0
        */
        bool isPosEvent() const;

        /** ����y�����ƶ�
        @version NIIEngine 3.0.0
        */
        void setYMove(bool b);

        /** ��ȡy�����ƶ�
        @version NIIEngine 3.0.0
        */
        bool isYMove() const;

        /** ����x�����ƶ�
        @version NIIEngine 3.0.0
        */
        void setXMove(bool b);

        /** ��ȡx�����ƶ�
        @version NIIEngine 3.0.0
        */
        bool isXMove() const;

        /** ����y����Χ
        @version NIIEngine 3.0.0
        */
        void setYRange(NIIf min, NIIf max);

        /** ��ȡy����Χ
        @version NIIEngine 3.0.0
        */
        void getYRange(NIIf & min, NIIf & max) const;

        /** ����x����Χ
        @version NIIEngine 3.0.0
        */
        void setXRange(NIIf min, NIIf max);

        /** ��ȡx����Χ
        @version NIIEngine 3.0.0
        */
        void getXRange(NIIf & min, NIIf & max) const;
    protected:
        /** ���UI����ʼ��קʱ����
        @remark
            �����ק����һ����ָ�α������UI�����λ��������,���Ұ��´�����ק�İ���
        @version NIIEngine 3.0.0
        */
        virtual void onMoveBegin(const WidgetEventArgs * arg);

        /** ���UI���������קʱ����
        @remark
            �����ק������,�ִ������ק�����������󴥷�,һ����ָ�ͷ����α�İ���
        @version NIIEngine 3.0.0
        */
        virtual void onMoveEnd(const WidgetEventArgs * arg);

        /** ���UI�����λ�÷����仯ʱ����
        @remark
            ������Ӧ���Ƿ�ͬ��λ�ñ仯�¼������,���ͬ����ʵʱ����(���ڶ��),����
            ������ÿ�ο�ʼ��ק�¼���������ק�¼��󴥷�һ��.
        @version NIIEngine 3.0.0
        */
        virtual void onPosChange(const WidgetEventArgs * arg);

        /// @copydetails Widget::onCursorMove
        virtual void onCursorMove(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onLost
        virtual void onLost(const WidgetEventArgs * arg);
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dest);

        /// @copydetails Widget::disableWrite
        void disableWrite();
    public:
        /** �ƶ���ʼ�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID MoveBeginEvent;

        /** �ƶ������¼�
        @version NIIEngine 3.0.0
        */
        static const EventID MoveEndEvent;

        /** λ�øı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID PosChangeEvent;
        
        /** �¼�����
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        Vector2f mDragPos;
        NIIf mVMin;
        NIIf mVMax;
        NIIf mHMin;
        NIIf mHMax;
        bool mSynEvent;
        bool mDrag;
        bool mYMove;
        bool mXMove;
    };
}
}
#endif