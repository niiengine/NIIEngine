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

#ifndef _NII_UI_DRAG_DROP_WINDOW_H_
#define _NII_UI_DRAG_DROP_WINDOW_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"

namespace NII
{
namespace UI
{
    /** �ɷ���ק������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DragDropWindow : public Container
    {
    public:
        DragDropWindow(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        virtual ~DragDropWindow();

        /** �����Ƿ����ÿ��Ϸ�
        @version NIIEngine 3.0.0
        */
        void setDragDropEnable(bool b);

        /** ��ȡ�Ƿ����ÿ��Ϸ�
        @version NIIEngine 3.0.0
        */
        bool isDragDropEnable() const;

        /** ֪ͨ����������
        @version NIIEngine 3.0.0
        */
        void notifyEnter(DragContainer * c);

        /** ֪ͨ����������
        @version NIIEngine 3.0.0
        */
        void notifyDrop(DragContainer * c);

        /** ֪ͨ�������Ƴ�
        @version NIIEngine 3.0.0
        */
        void notifyLeave(DragContainer * c);
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dest);
    protected:
        /** ����������ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onEnter(const DragDropEventArgs * arg);

        /** ����������ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onDrop(const DragDropEventArgs * arg);

        /** �������Ƴ�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onLeave(const DragDropEventArgs * arg);
    public:
        /** �����������¼�
        @version NIIEngine 3.0.0
        */
        static const EventID EnterEvent;

        /** �����������¼�
        @version NIIEngine 3.0.0
        */
        static const EventID DropEvent;

        /** �������Ƴ��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID LeaveEvent;

        /// �¼�����
        static const EventID EventCount;
    protected:
        bool mDragDropEnable;
    };
}
}
#endif