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

#ifndef _NII_DATA_TYPE_H_
#define _NII_DATA_TYPE_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ����Ԫ������,����ȷ���������ݵĻ�������
    @version NIIEngine 3.0.0
    */
    enum DataType
    {
        DT_Float1x      = 0,         ///< ��1������Ϊ��λ��
        DT_Float2x      = 1,         ///< ��2������Ϊ��λ��
        DT_Float3x      = 2,         ///< ��3������Ϊ��λ��
        DT_Float4x      = 3,         ///< ��4������Ϊ��λ��
        DT_Double1x     = 4,
        DT_Double2x     = 5,
        DT_Double3x     = 6,
        DT_Double4x     = 7,
        DT_Int1x        = 8,
        DT_Int2x        = 9,
        DT_Int3x        = 10,
        DT_Int4x        = 11,
        DT_UInt1x       = 12,
        DT_UInt2x       = 13,
        DT_UInt3x       = 14,
        DT_UInt4x       = 15,
        DT_Colour       = 16,       ///< ��ɫ��ʽ(һ����32λ)
        DT_Colour_ARGB  = 17,       ///< ��ɫ��ʽ(D3D����)
        DT_Colour_ABGR  = 18,       ///< ��ɫ��ʽ(GL����)
        DT_Short2x      = 20,       ///< ��2��������Ϊ��λ��
        DT_Short4x      = 22,       ///< ��4��������Ϊ��λ��
        DT_UShort2x     = 24,       ///< ��2���޷��Ŷ�����Ϊ��λ��
        DT_UShort4x     = 26,       ///< ��4���޷��Ŷ�����Ϊ��λ��
        DT_ShortUnit2x  = 27,
        DT_ShortUnit4x  = 28,
        DT_UShortUnit2x = 29,   
        DT_UShortUnit4x = 30,
        DT_Vector2f     = 31,       ///< ��2������Ϊ��λ��
        DT_Quaternion   = 32,       ///< ��4������Ϊ��λ��
        DT_Vector4f     = 33,       ///< ��4������Ϊ��λ��
        DT_Matrix4f     = 35,       ///< ��16������Ϊ��λ��
        DT_UByte4x      = 36,       ///< �޷���(32λ)
        DT_Byte4x       = 37,
        DT_UByteUnit4x  = 38,  
        DT_ByteUnit4x   = 39,
        DT_HalfFloat2x  = 40,
        DT_HalfFloat4x  = 41,
        DT_Sampler      = 42,
        DT_Subroutine   = 43,
        DT_Unkonw       = 44
    };
    
    /** �������͸���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DataTypeUtil
    {
    public:
        /**
        @version NIIEngine 5.0.0
        */
        bool isUnit(DataType type);
    
        /** ��ȡ�ɷִ�С
        @param[in] type ���ݳɷ�
        @version NIIEngine 3.0.0
        */
        static NCount getSize(DataType type);

        /** ��ȡ�ɷֻ�������
        @param[in] type ���ݳɷ�
        @version NIIEngine 3.0.0
        */
        static NCount getCount(DataType type);

        /** �򵥵�ת��������,��
        @param[in] type ���ϻ����ɷ�
        @param[out] count �����ɷ�����
        @version NIIEngine 3.0.0
        */
        static DataType getBaseType(DataType type, NCount & count);

        /** ��ȡ�ϳ�����
        @param[in] type �����ɷ�
        @param[in] count �����ɷ�����
        @version NIIEngine 3.0.0
        */
        static DataType getMultiType(DataType type, NCount count);
    };
}

#endif