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

#ifndef _NII_FUSION_NUM_H_
#define _NII_FUSION_NUM_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ��ʽ��������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FusionNum : public DataAlloc
    {
    public:
        FusionNum();
        virtual ~FusionNum();

        /** ���������
        @param[out] out ���,�������Ѿ�ʵ����
        @version NIIEngine 3.0.0
        */
        virtual void Add(const FusionNum * in, FusionNum * out) = 0;

        /** ���������
        @param[out] out ���,�������Ѿ�ʵ����
        @version NIIEngine 3.0.0
        */
        virtual void Sub(const FusionNum * in, FusionNum * out) = 0;

        /** ���������
        @param[out] out ���,�������Ѿ�ʵ����
        @version NIIEngine 3.0.0
        */
        virtual void Mul(const FusionNum * in, FusionNum * out) = 0;

        /** ���������
        @param[out] out ���,�������Ѿ�ʵ����
        @version NIIEngine 3.0.0
        */
        virtual void Div(const FusionNum * in, FusionNum * out) = 0;

        /** ���ϸ���
        @param[out] out ���,�������Ѿ�ʵ����
        @version NIIEngine 3.0.0
        */
        virtual void Add(NIIf in, FusionNum * out) = 0;

        /** ��ȥ����
        @param[out] out ���,�������Ѿ�ʵ����
        @version NIIEngine 3.0.0
        */
        virtual void Sub(NIIf in, FusionNum * out) = 0;

        /** ���Ը���
        @param[out] out ���,�������Ѿ�ʵ����
        @version NIIEngine 3.0.0
        */
        virtual void Mul(NIIf in, FusionNum * out) = 0;

        /** ���ڸ���
        @param[out] out ���,�������Ѿ�ʵ����
        @version NIIEngine 3.0.0
        */
        virtual void Div(NIIf in, FusionNum * out) = 0;

        /** ���
        @version NIIEngine 3.0.0
        */
        virtual void Fusion(FusionNum * k1, FusionNum * k2, NIIf t) = 0;

        /** ���
        @version NIIEngine 3.0.0
        */
        virtual void RelFusion(FusionNum * bas, FusionNum * k1, FusionNum * k2, NIIf t) = 0;

        /** ���
        @version NIIEngine 3.0.0
        */
        virtual void RelMulFusion(FusionNum * bas, FusionNum * k1, FusionNum * k2, NIIf t) = 0;

        /** �������ݵ��ַ���
        @version NIIEngine 3.0.0
        */
        virtual void to(String & out) const = 0;

        /** ���ַ�����������
        @version NIIEngine 3.0.0
        */
        virtual void from(const String & in) = 0;

        /** ����ָ��
        @version NIIEngine 3.0.0
        */
        virtual void setData(const void * data) = 0;

        /** ����ָ��
        @version NIIEngine 3.0.0
        */
        virtual const void * getDataPtr() const = 0;

        /** ͬ���͸���
        @note ���������ݸ�ֵ
        @version NIIEngine 3.0.0
        */
        virtual FusionNum * cloneType() const = 0;

        /** ����
        @note �������ݸ�ֵ
        @version NIIEngine 3.0.0
        */
        virtual FusionNum * clone() const = 0;
    };
}
#endif