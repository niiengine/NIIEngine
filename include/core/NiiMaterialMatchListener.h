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

#ifndef _NII_MaterialMatchListener_H_
#define _NII_MaterialMatchListener_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ����ƥ�����
    @version NIIEngine 3.0.0 �߼�api
    */
    class MaterialMatchListener
    {
    public:
        virtual ~MaterialMatchListener();

        /** ƥ��
        @version NIIEngine 3.0.0
        */
        virtual ShaderFusion * find(SchemeID sid, const Material * mat, 
            const GeometryObj * geo, Nidx lod);
            
        /** ����
        @version NIIEngine 3.0.0
        */
        virtual ShaderFusion * miss(SchemeID sid, const Material * mat, 
            const GeometryObj * geo, Nidx lod);
    };
}

#endif