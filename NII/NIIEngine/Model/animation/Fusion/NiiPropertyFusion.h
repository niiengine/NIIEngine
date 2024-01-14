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

#ifndef _NII_PropertyFusion_H_
#define _NII_PropertyFusion_H_

#include "NiiPreInclude.h"
#include "NiiAnimationFusion.h"
#include "NiiFusionData.h"

namespace NII
{
    /** �������Զ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PropertyFusion : public AnimationFusion
    {
    public:
        PropertyFusion(PropertyCmdObj * dst, AnimationObj * obj, Animation * ani, 
            NIIf weight = 1.0, bool enable = false);

        PropertyFusion(PropertyCmdObj * dst, AnimationObj * obj,
            Animation * ani, TimeDurMS pos, TimeDurMS dur, NIIf weight = 1.0, bool enable = false);

        PropertyFusion(AnimationFusionObj * own, const PropertyFusion & o);
        
        ~PropertyFusion();

        /** Ӧ��
        @version NIIEngine 3.0.0
        */
        void apply(NIIf factor = 1.0);

        /** �������Ե�ǰֵ
        @remark ���Ե�ֵ�����ڶ�����
        @version NIIEngine 3.0.0
        */
        void setBaseValue(PropertyID pid);

        /** ��ȡ���Ա���ֵ
        @remark ���Ե�ֵ�����ڶ�����
        @version NIIEngine 3.0.0
        */
        const FusionData & getBaseValue(PropertyID pid);

        /** ����Ŀ�����
        @version NIIEngine 3.0.0
        */
        void setTarget(PropertyCmdObj * dst);

        /** ��ȡĿ�����
        @version NIIEngine 3.0.0
        */
        PropertyCmdObj * getTarget() const;
        
        /// @copydetails AnimationFusion::clone
        AnimationFusion * clone(AnimationFusionObj * own) const;
    protected:
        /// @copydetails AnimationFusion::onStart
        void onStart();
    private:
        typedef map<PropertyID, FusionData>::type PropertyValueMap;

        PropertyCmdObj * mDst;
        PropertyValueMap mBaseValue;
    };
}
#endif