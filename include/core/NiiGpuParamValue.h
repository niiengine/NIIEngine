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

#ifndef _NII_GPU_ParamValue_H_
#define _NII_GPU_ParamValue_H_

#include "NiiPreInclude.h"
#include "NiiGpuParamDefine.h"

namespace NII
{
    /** Ԥ������ɫ����ֵ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuParamValue : public GpuParamsAlloc
    {
    public:
        GpuParamValue(const String & name);
        virtual ~GpuParamValue();

        /** ��������
        @version NIIEngine 3.0.0
        */
        inline const String & getName(){ return mName; }

        /** ��ӱ���
        @version NIIEngine 3.0.0
        */
        void add(const VString & name, GpuDataType type, NCount count = 1);

        /** ��ȡ����
        @version NIIEngine 3.0.0
        */
        const GpuParamUnit & get(const VString & name) const;

        /** ��ȥ����
        @version NIIEngine 3.0.0
        */
        void remove(const VString & name);

        /** ��ȥ���б���
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** ��������
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, NIIi in){ set(name, &in, 1); }

        /** ��������
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, NIIf in){ set(name, &in, 1); }

        /** ��������
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, const Colour & in){ set(name, &in.r, 4); }

        /** ��������
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, const Vector4f & in){ set(name, &in.x, 4); }

        /** ��������
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, const Vector3f & in){ set(name, &in.x, 3); }
        
        /** ��������
        @version NIIEngine 3.0.0
        */
        inline void set(const VString & name, const Matrix4f & in){ set(name, in[0], 16); }

        /** ��������
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const NIIi * in, NCount count);

        /** ��������
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const NIIf * in, NCount count);

        /** ��������
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const NIId * in, NCount count);

        /** ��������
        @version NIIEngine 3.0.0
        */
        void set(const VString & name, const Matrix4f * in, NCount count);

        /** ��ȡ�����б�
        @version NIIEngine 3.0.0 �߼�api
        */
        inline const GpuParamUnitList & getDefList() const{ return mDefines; }

        /** ��ȡ״̬����
        @version NIIEngine 3.0.0
        */
        inline Nul getStateMark() const{ return mDirtyMark; }

        /** ��ȡ���㲿������
        @version NIIEngine 3.0.0
        */
        inline NIIf * getFloatData(NCount pos) const{ return &mFloatData[pos];}

        /** ��ȡ���β�������
        @version NIIEngine 3.0.0
        */
        inline NIId * getDoubleData(NCount pos) const{ return &mDoubleData[pos]; }

        /** ��ȡ���β�������
        @version NIIEngine 3.0.0
        */
        inline NIIi * getIntData(NCount pos) const { return &mIntData[pos]; }
    protected:
        String mName;
        GpuParamUnitList mDefines;
        mutable IntArray mIntData;
        mutable FloatArray mFloatData; 
        mutable DoubleArray mDoubleData;
        NCount mIntSize;
        NCount mFloatSize;
        NCount mDoubleSize;
        Nmark mDirtyMark;
    };
}

#endif