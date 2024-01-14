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
#ifndef _NII_VERTEXFUSION_H_
#define _NII_VERTEXFUSION_H_

#include "NiiPreInclude.h"
#include "NiiAnimationFusion.h"

namespace NII
{
    /** ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexFusion : public AnimationFusion
    {
    public:
        VertexFusion(AnimationObj * obj, Animation * ani, TimeDurMS pos, TimeDurMS dur,
            NIIf weight = 1.0, bool enable = false);
            
        VertexFusion(AnimationFusionObj * own, const VertexFusion & o);
        
        virtual ~VertexFusion();

        /** Ӧ�õ�Ŀ�����
        @remark Ӳ����ϲ��ܻ�ȡ��ǰ״̬,���ֲ����������Ӱ�����Ҫ������
        @param[in] sw ִ�������������
        @param[in] hw ִ��Ӳ����������
        @version NIIEngine 3.0.0
        */
        virtual void apply(GeometrySpace * dst, bool sw, bool hw) = 0;
        
        /** Ӧ�õ���������
        @version NIIEngine 3.0.0
        */
        virtual void apply(VertexData * data) = 0;
    };
}
#endif