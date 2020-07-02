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

#ifndef _NII_WidgetModelFactory_H_
#define _NII_WidgetModelFactory_H_

#include "NiiUIPreInclude.h"

namespace NII
{
namespace UI
{
    /** ��Ԫ��񹤳���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI WidgetModelFactory : public FactoryAlloc
    {
    public:
        WidgetModelFactory(){}
        virtual ~WidgetModelFactory() {}

        /** ��ȡID
        @version NIIEngine 3.0.0
        */
        virtual FactoryID getID() const = 0;

        /** ������Ԫ���
        @version NIIEngine 3.0.0
        */
        virtual WidgetView * create() = 0;

        /** ɾ����Ԫ���
        @version NIIEngine 3.0.0
        */
        virtual void destroy(WidgetView * obj) = 0;

    };
}
}
#endif