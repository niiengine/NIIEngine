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

#include "NiiPreProcess.h"
#include "NiiDataType.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    bool DataTypeUtil::isUnit(DataType type)
    {
        switch( baseType )
        {
            case DT_ShortUnit2x:
            case DT_UShortUnit2x:
            case DT_ShortUnit4x:
            case DT_UShortUnit4x:
            case DT_ByteUnit4x:
            case DT_UByteUnit4x:
                return true;
            default:
                return false;
        }

        return false;
    }
    //------------------------------------------------------------------------
    NCount DataTypeUtil::getSize(DataType type)
    {
        switch(type)
        {
        case DT_Colour:
        case DT_Colour_ABGR:
        case DT_Colour_ARGB:
            return sizeof(RGBA);
        case DT_Float1x:
            return sizeof(NIIf);
        case DT_Float2x:
            return sizeof(NIIf) * 2;
        case DT_Float3x:
            return sizeof(NIIf) * 3;
        case DT_Float4x:
            return sizeof(NIIf) * 4;
        case DT_Short2x:
        case DT_UShort2x:
        case DT_ShortUnit2x:
        case DT_UShortUnit2x:
            return sizeof(NIIs) * 2;
        case DT_Short4x:
        case DT_UShort4x:
        case DT_ShortUnit4x:
        case DT_UShortUnit4x:
            return sizeof(NIIs) * 4;
        case DT_Double1x:
            return sizeof(NIId);
        case DT_Double2x:
            return sizeof(NIId) * 2;
        case DT_Double3x:
            return sizeof(NIId) * 3;
        case DT_Double4x:
            return sizeof(NIId) * 4;
        case DT_Int1x:
        case DT_UInt1x:
            return sizeof(NIIi);
        case DT_Int2x:
        case DT_UInt2x:
            return sizeof(NIIi) * 2;
        case DT_Int3x:
        case DT_UInt3x:
            return sizeof(NIIi) * 3;
        case DT_Int4x:
        case DT_UInt4x:
            return sizeof(NIIi) * 4;
        case DT_Vector2f:
            return sizeof(NIIf) * 2;
        case DT_Quaternion:
            return sizeof(NIIf) * 4;
        case DT_Vector4f:
            return sizeof(NIIf) * 4;
        case DT_Matrix4f:
            return sizeof(NIIf) * 16;
        case DT_UByte4x:
        case DT_Byte4x:
        case DT_UByteUnit4x:
        case DT_ByteUnit4x:
            return sizeof(Nui8) * 4;
        case DT_HalfFloat2x:
            return sizeof(NIIf);
        case DT_HalfFloat4x:
            return sizeof(NIIf) * 2;
        default:
            return 0;
        }
        return 0;
    }
    //------------------------------------------------------------------------
    NCount DataTypeUtil::getCount(DataType type)
    {
        switch(type)
        {
        case DT_Colour:
        case DT_Colour_ABGR:
        case DT_Colour_ARGB:
        case DT_Int1x:
        case DT_UInt1x:
        case DT_Float1x:
        case DT_Double1x:
            return 1;
        case DT_UInt2x:
        case DT_Int2x:
        case DT_Float2x:
        case DT_Double2x:
        case DT_Short2x:
        case DT_UShort2x:
        case DT_ShortUnit2x:
        case DT_UShortUnit2x:
        case DT_Vector2f:
        case DT_HalfFloat2x:
            return 2;
        case DT_UInt3x:
        case DT_Int3x:
        case DT_Float3x:
        case DT_Double3x:
            return 3;
        case DT_Int4x:
        case DT_UInt4x:
        case DT_Float4x:
        case DT_Double4x:
        case DT_Short4x:
        case DT_UShort4x:
        case DT_UByte4x:
        case DT_ShortUnit4x:
        case DT_UShortUnit4x:
        case DT_Quaternion:
        case DT_Vector4f:
        case DT_Byte4x:
        case DT_UByteUnit4x:
        case DT_ByteUnit4x:
        case DT_HalfFloat4x:
            return 4;
        case DT_Matrix4f:
            return 16;
        }
        N_EXCEPT(InvalidParam, _I18n("Invalid type"));
    }
    //-----------------------------------------------------------------------
    DataType DataTypeUtil::getBaseType(DataType type, NCount & count)
    {
        switch(type)
        {
        case DT_Float1x:
            count = 1;
            return DT_Float1x;
        case DT_Float2x:
            count = 2;
            return DT_Float1x;
        case DT_Float3x:
            count = 3;
            return DT_Float1x;
        case DT_Float4x:
            count = 4;
            return DT_Float1x;
        case DT_Colour:
            count = 1;
            return DT_Colour;
        case DT_Colour_ABGR:
            count = 1;
            return DT_Colour_ABGR;
        case DT_Colour_ARGB:
            count = 1;
            return DT_Colour_ARGB;
        case DT_Short2x:
            count = 1;
            return DT_Short2x;
        case DT_Short4x:
            count = 2;
            return DT_Short2x;
        case DT_UByte4x:
            count = 4;
            return DT_UByte4x;
        case DT_Byte4x:
            count = 4;
            return DT_Byte4x;
        case DT_UByteUnit4x:
            count = 4;
            return DT_UByteUnit4x;
        case DT_ByteUnit4x:
            count = 4;
            return DT_ByteUnit4x;
        case DT_Double1x:
            count = 1;
            return DT_Double1x;
        case DT_Double2x:
            count = 2;
            return DT_Double1x;
        case DT_Double3x:
            count = 3;
            return DT_Double1x;
        case DT_Double4x:
            count = 4;
            return DT_Double1x;
        case DT_Int1x:
            count = 1;
            return DT_Int1x;
        case DT_Int2x:
            count = 2;
            return DT_Int1x;
        case DT_Int3x:
            count = 3;
            return DT_Int1x;
        case DT_Int4x:
            count = 4;
            return DT_Int1x;
        case DT_UInt1x:
            count = 1;
            return DT_UInt1x;
        case DT_UInt2x:
            count = 2;
            return DT_UInt1x;
        case DT_UInt3x:
            count = 3;
            return DT_UInt1x;
        case DT_UInt4x:
            count = 4;
            return DT_UInt1x;
        case DT_UShort2x:
            count = 1;
            return DT_UShort2x;
        case DT_UShort4x:
            count = 2;
            return DT_UShort2x;
        case DT_ShortUnit2x:
            count = 1;
            return DT_ShortUnit2x;
        case DT_ShortUnit4x:
            count = 2;
            return DT_ShortUnit2x;
        case DT_UShortUnit2x:
            count = 1;
            return DT_UShortUnit2x;
        case DT_UShortUnit4x:
            count = 2;
            return DT_UShortUnit2x;
        case DT_Vector2f:
            count = 2;
            return DT_Float1x;
        case DT_Quaternion:
            count = 4;
            return DT_Float1x;
        case DT_Vector4f:
            count = 4;
            return DT_Float1x;
        case DT_Matrix4f:
            count = 16;
            return DT_Float1x;
        case DT_HalfFloat2x:
            count = 1;
            return DT_HalfFloat2x;
        case DT_HalfFloat4x:
            count = 2;
            return DT_HalfFloat2x;
        };
        // µÙπ˝±‡“Î∆˜Œ Ã‚
        count = 1;
        return DT_Float1x;
    }
    //------------------------------------------------------------------------
    DataType DataTypeUtil::getMultiType(DataType type, NCount count)
    {
        switch(type)
        {
        case DT_Float1x:
            switch(count)
            {
            case 1:
                return DT_Float1x;
            case 2:
                return DT_Float2x;
            case 3:
                return DT_Float3x;
            case 4:
                return DT_Float4x;
            default:
                break;
            }
            break;
        case DT_Short2x:
            switch(count)
            {
            case 1:
                return DT_Short2x;
            case 2:
                return DT_Short4x;
            default:
                break;
            }
            break;
        case DT_Double1x:
            switch (count)
            {
            case 1:
                return DT_Double1x;
            case 2:
                return DT_Double2x;
            case 3:
                return DT_Double3x;
            case 4:
                return DT_Double4x;
            default:
                break;
            }
            break;
        case DT_Int1x:
            switch (count)
            {
            case 1:
                return DT_Int1x;
            case 2:
                return DT_Int2x;
            case 3:
                return DT_Int3x;
            case 4:
                return DT_Int4x;
            default:
                break;
            }
            break;
        case DT_UInt1x:
            switch (count)
            {
            case 1:
                return DT_UInt1x;
            case 2:
                return DT_UInt2x;
            case 3:
                return DT_UInt3x;
            case 4:
                return DT_UInt4x;
            default:
                break;
            }
            break;
        case DT_UShort2x:
            switch (count)
            {
            case 1:
                return DT_UShort2x;
            case 2:
                return DT_UShort4x;
            default:
                break;
            }
            break;
        case DT_ShortUnit2x:
            switch (count)
            {
            case 1:
                return DT_ShortUnit2x;
            case 2:
                return DT_ShortUnit4x;
            default:
                break;
            }
            break;
        case DT_UShortUnit2x:
            switch (count)
            {
            case 1:
                return DT_UShortUnit2x;
            case 2:
                return DT_UShortUnit4x;
            default:
                break;
            }
            break;
        case DT_HalfFloat2x:
            switch (count)
            {
            case 1:
                return DT_HalfFloat2x;
            case 2:
                return DT_HalfFloat4x;
            default:
                break;
            }
            break;
        default:
            break;
        }
        return type;
    }
    //------------------------------------------------------------------------
}