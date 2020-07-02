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
#ifndef _NII_UI_SCROLLBAR_H_
#define _NII_UI_SCROLLBAR_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"
#include "NiiUIWidgetView.h"

namespace NII
{
namespace UI
{
    /** ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Scrollbar : public Container
    {
    public:
        Scrollbar(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);
        ~Scrollbar();

        /** ������ҳ��С
        @version NIIEngine 3.0.0
        */
        void setDocSize(NIIf size);

        /** ��ȡ��ҳ��С
        @version NIIEngine 3.0.0
        */
        inline NIIf getDocSize() const{ return mDocSize; }

        /** ���ð�������С
        @version NIIEngine 3.0.0
        */
        void setStepSize(NIIf size);
        
        /** ��ȡ��������С
        @version NIIEngine 3.0.0
        */
        inline NIIf getStepSize() const{ return mStepSize; }

        /** ����ҳ��С
        @version NIIEngine 3.0.0
        */
        void setPageSize(NIIf size);

        /** ��ȡҳ��С
        @version NIIEngine 3.0.0
        */
        inline NIIf getPageSize() const{ return mPageSize; }
        
        /** ����ҳ���С
        @version NIIEngine 3.0.0
        */
        void setOverlapSize(NIIf size);
        
        /** ��ȡҳ���С
        @version NIIEngine 3.0.0
        */
        inline NIIf getOverlapSize() const { return mPageRemian; }

        /** ���õ�ǰλ��
        @param[in] pos [0 <= pos <= doc - page]
        @version NIIEngine 3.0.0
        */
        void setCurrent(NIIf pos);
        
        /** ��ȡ��ǰλ��
        @param[in] pos [0 <= pos <= doc - page]
        @version NIIEngine 3.0.0
        */
        inline NIIf getCurrent() const{ return mCurrentPos;}

        /** ���õ�λ����ǰλ��
        @param[in] upos [0, 1]
        @version NIIEngine 3.0.0
        */
        void setCurrentUnit(NIIf upos);        
        
        /** ��ȡ��λ����ǰλ��
        @param[in] upos [0, 1]
        @version NIIEngine 3.0.0
        */
        NIIf getCurrentUnit() const;

        /** ���򲽽�
        @version NIIEngine 3.0.0
        */
        void negStep();
        
        /** ���򲽽�
        @version NIIEngine 3.0.0
        */
        void posStep();

        /** ����ҳ��
        @version NIIEngine 3.0.0
        */
        void negPage();
        
        /** ����ҳ��
        @version NIIEngine 3.0.0
        */
        void posPage();
        
        /** ��ȡλ��ָ��
        @version NIIEngine 3.0.0
        */
        inline PosButton * getThumb() const { return mThumb; }

        /** ��ȡ����Ŧ
        @version NIIEngine 3.0.0
        */
        inline PushButton * getNegative() const{ return mNegative; }
        
        /** ��ȡ����Ŧ
        @version NIIEngine 3.0.0
        */
        inline PushButton * getPositive() const { return mPositive;}

        /** �����Ƿ��Զ�ĩβ
        @version NIIEngine 3.0.0
        */
        inline void setAutoEndPos(bool b){ mAutoEnd = b;}

        /** ��ȡ�Ƿ��Զ�ĩβ
        @version NIIEngine 3.0.0
        */
        inline bool isAutoEndPos() const{ return mAutoEnd; }
        
        /** �ۺ�����
        @version NIIEngine 3.0.0
        */
        void setConfig(NIIf doc, NIIf page, NIIf step, NIIf pageremain, NIIf pos);
    protected:
        /// @copydetails Container::initChild
        void initChild();

        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

        /** ���õ�ǰλ��
        @version NIIEngine 3.0.0
         */
        bool setCurrentImpl(NIIf pos);

        /** ��ȡ���λ��
        @version NIIEngine 3.0.0
        */
        NIIf getCurrentMax() const;
        
        /** �Ƿ���ĩβλ��
        @version NIIEngine 3.0.0
        */
        bool isCurrentEnd() const;

        /** λ��ָ�꺯��
        @version NIIEngine 3.0.0 �߼�api
        */
        void ThumbMoveMFunc(const EventArgs * e);

        /** λ��ָ�꺯��
        @version NIIEngine 3.0.0 �߼�api
        */
        void PositiveMFunc(const EventArgs * e);

        /** λ��ָ�꺯��
        @version NIIEngine 3.0.0 �߼�api
        */
        void NegativeMFunc(const EventArgs * e);

        /** λ��ָ�꺯��
        @version NIIEngine 3.0.0 �߼�api
        */
        void ThumbBeginMFunc(const EventArgs * e);

        /** λ��ָ�꺯��
        @version NIIEngine 3.0.0 �߼�api
        */
        void ThumbEndMFunc(const EventArgs * e);

        /// @copydetail Widget::validImpl
        virtual bool validImpl(const WidgetView * view) const;

        /** ��ǰλ�ñ仯ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onCurrentChange(const WidgetEventArgs * arg);

        /** λ��ָ�꿪ʼ�ƶ�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onTumbMoveBegin(const WidgetEventArgs * arg);

        /** λ��ָ������ƶ�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onThumbMoveEnd(const WidgetEventArgs * arg);

        /** ���÷����仯ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onConfigChange(const WidgetEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorDrag
        virtual void onCursorDrag(const CursorEventArgs * arg);

        /// @copydetails Widget::disableWrite
        void disableWrite();
    public:
        /** ��ǰλ�ñ仯ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID CurrentChageEvent;

        /** λ��ָ�꿪ʼ�ƶ�ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID ThumbMoveBeginEvent;

        /** λ��ָ������ƶ�ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID ThumbMoveEndEvent;

        /** ���÷����仯ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID ConfigChangeEvent;

        /** �¼�����
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        PosButton * mThumb;
        PushButton * mNegative;
        PushButton * mPositive;
        NIIf mDocSize;
        NIIf mPageSize;
        NIIf mStepSize;
        NIIf mPageRemian;
        NIIf mCurrentPos;
        bool mAutoEnd;
    };

    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScrollbarView : public WidgetView
    {
        friend class Scrollbar;
    public:
        ScrollbarView(WidgetModelID wsid);

        bool isVertical() const;

        void setVertical(bool b);

        virtual NIIf getThumbValue() const;

        virtual NIIf getDirection(const Vector2f & pt) const;

        /// @copydetails WidgetView::flush
        void flush();

        /// @copydetails WidgetView::layout
        void layout();
    protected:
        virtual void updateThumb();
    protected:
        bool mYDirection;
    };
}
}
#endif