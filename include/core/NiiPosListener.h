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

#ifndef _NII_PosListener_H_
#define _NII_PosListener_H_

#include "NiiPreProcess.h"

namespace NII
{
    /** λ�ýڼ���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PosListener
    {
    public:
        PosListener();
        virtual ~PosListener();

        /** ����ʱ
        @version NIIEngine 3.0.0
        */
        virtual void create(PosNode * o);

        /** ɾ��ʱ
        @version NIIEngine 3.0.0
        */
        virtual void destroy(PosNode * o);

        /** ����Ӷ���ʱ
        @version NIIEngine 3.0.0
        */
        virtual void add(PosNode * obj, PosNode * child);

        /** ��ȥ�Ӷ���ʱ
        @version NIIEngine 3.0.0
        */
        virtual void remove(PosNode * o, PosNode * child);

        /** ���ӵ���������ʱ
        @version NIIEngine 3.0.0
        */
        virtual void attach(PosNode * o);

        /** ������������ʱ
        @version NIIEngine 3.0.0
        */
        virtual void detach(PosNode * o);

        /** ����ʱ
        @version NIIEngine 3.0.0
        */
        virtual void update(PosNode * o);
    };
}
#endif