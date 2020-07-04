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

#ifndef _NII_SPACEOBJ_LISTENER_H_
#define _NII_SPACEOBJ_LISTENER_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** �¼�����
    @remark ��������������/��Ӱ��Ⱦ������
    @version NIIEngine 3.0.0 �߼�api
    */
    class _EngineAPI SpaceObjListener
    {
    public:
        SpaceObjListener();
        virtual ~SpaceObjListener();

        /** ������ʱ
        @version NIIEngine 3.0.0
        */
        virtual void create(SpaceObj * obj);
        
        /** ɾ��ʱ
        @version NIIEngine 3.0.0
        */
        virtual void destroy(SpaceObj * obj);
        
        /** λ�÷����ı�
        @version NIIEngine 3.0.0
        */
        virtual void notify(SpaceObj * obj);

        /** ���ӵ��ڵ�ʱ
        @note ���ڲ���2��0ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void attach(SpaceObj * obj, const PosNode * np);

        /** �ӽڵ��з���ʱ
        @note ���ڲ���2��0ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void detach(SpaceObj * obj, const PosNode * op);

        /** ʹ��ָ����������ƿռ�
        @return �Ƿ���ʵ�ʻ���Ч��,û��Ч�������
        @version NIIEngine 3.0.0
        */
        virtual bool render(SpaceObj * obj, const Camera * view);

        /** ��ѯʵ��Ӱ��ռ����ĵƹ�
        @version NIIEngine 3.0.0
        */
        virtual void query(const SpaceObj * obj, LightList & out);
    };
}

#endif