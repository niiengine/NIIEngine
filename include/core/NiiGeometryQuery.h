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

#ifndef _NII_GEOMETRY_QUERY_H_
#define _NII_GEOMETRY_QUERY_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ���β�ѯ��
    @remark �������Ա����ѯ���
    @version NIIEngine 3.0.0 �߼�api
    */
    class GeometryQuery
    {
    public:
        GeometryQuery();
        virtual ~GeometryQuery();

        /** ����ģ�͵���ģʽ
        @version NIIEngine 3.0.0
        */
        void setDebugMode(bool b);
        
        /** ��ȡ����ģʽģʽ
        @version NIIEngine 3.0.0
        */
        bool isDebugMode() const;
        
        /** ִ�в�ѯ
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void query(GeometryObj * obj) = 0;
    protected:
        bool mDebug;
    };
}
#endif