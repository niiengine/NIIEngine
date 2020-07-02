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

#ifndef _NII_UI_INDEX_LayoutContainer_H_
#define _NII_UI_INDEX_LayoutContainer_H_

#include "NiiUIPreInclude.h"
#include "NiiUILayoutContainer.h"

namespace NII
{
namespace UI
{
    /** �����Ű�����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI IndexLayoutContainer : public LayoutContainer
    {
    public:
        IndexLayoutContainer(ScriptTypeID stid, WidgetID wid, FactoryID fid, Container * own, 
            const String & name = N_StrBlank, LangID lid = N_PrimaryLang);

        virtual ~IndexLayoutContainer();

        using Container::add;

        using Container::remove;


        /** ����Ԫ��
        @version NIIEngine 3.0.0
        */
        void swap(Nidx sub1, Nidx sub2);

        /** ����Ԫ��
        @version NIIEngine 3.0.0
        */
        void swap(Widget * sub1, Widget * sub2);

        /** ����Ԫ��
        @version NIIEngine 3.0.0
        */
        void move(Widget * sub, Nidx index);

        /** ��ǰԪ��
        @version NIIEngine 3.0.0
        */
        void forward(Widget * sub, NCount count = 1);

        /** ���
        @version NIIEngine 3.0.0
        */
        void add(Widget * sub, Nidx index);

        /** ��ȥ
        @version NIIEngine 3.0.0
        */
        void remove(Nidx index);
    public:
        /** ��Ԫ������仯�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID ChildOrderChangeEvent;

        static const EventID EventCount;
    protected:
        /** ��Ԫ������仯ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onChildOrderChange(const WidgetEventArgs * arg);
    };
}
}
#endif