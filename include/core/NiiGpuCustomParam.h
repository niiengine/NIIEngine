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

#ifndef _NII_GPU_CUSTOM_PARAM_H_
#define _NII_GPU_CUSTOM_PARAM_H_

#include "NiiPreInclude.h"
#include "NiiGpuProgramParam.h"

namespace NII
{
    /** ��ɫ�����Զ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuCustomParam
    {
    public:
        GpuCustomParam();
        ~GpuCustomParam();

        /** Ϊ�������Ⱦ������һ���Զ������,�����Ƶ��������ָ���Ŀ���Ⱦ��,
            ��GPU��ɫ�����������
        @param[in] index
        @param[in] value
        @version NIIEngine 3.0.0
        */
        void setCustonParam(Nidx index, const Vector4f & value);

        /** ����������ȡ�����������Ⱦ����Զ���ֵ
        @param[in] index ����ʹ�õ�����
        @version NIIEngine 3.0.0
        */
        const Vector4f & getCustomParam(Nidx index) const;

        /** ��ȥ�Զ������
        @param[in] index ����ʹ�õ�����
        @version NIIEngine 3.0.0
        */
        void removeCustomParam(Nidx index);

        /** �Ƿ�����Զ������
        @param[in] index ����ʹ�õ�����
        @version NIIEngine 3.0.0
        */
        bool isCustomParamExist(Nidx index) const;

        /** �����Զ��������ʵ�ʵ���ɫ���������
        @note ����ʹ�õ����� bind ��� mInputInt
        @param[in] dst ��Ҫ���µ���ɫ���������
        @param[in] bind ��Ҫ������ɫ�������
        @version NIIEngine 3.0.0
        */
        virtual void updateCustom(GpuProgramParam * dst, const GpuSyncParamIndex & bind) const;
        
        /** �����Զ��������ʵ�ʵ���ɫ���������
        @param[in] dst ��Ҫ���µ���ɫ���������
        @param[in] index ����ʹ�õ�����
        @param[in] bind ��Ҫ������ɫ�������
        @version NIIEngine 3.0.0
        */
        virtual void updateCustom(GpuProgramParam * dst, Nidx index, const GpuSyncParamIndex & entry) const;
    protected:
        typedef map<Nidx, Vector4f>::type CustomParameterMap;
    protected:
        CustomParameterMap mCustomParameters;
    };
}
#endif