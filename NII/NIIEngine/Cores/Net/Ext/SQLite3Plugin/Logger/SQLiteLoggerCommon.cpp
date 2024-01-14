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
#include "SQLiteLoggerCommon.h"
#include "NiiNetSerializer.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    static const char *sqlDataTypeNames[SQLLPDT_COUNT] =
    {
        "INTEGER",
        "INTEGER",
        "NUMERIC",
        "TEXT",
        "BLOB",
        "BLOB",
    };
    //------------------------------------------------------------------------
    extern "C" const char *GetSqlDataTypeName(SQLLoggerPrimaryDataType idx)
    {
        return sqlDataTypeNames[(int)idx];
    }
    //------------------------------------------------------------------------
    void LogParameter::read(NetSerializer * out) const
    {
        unsigned char c = type;
        out->write(c);
        out->write(size);
        switch (type)
        {
            case SQLLPDT_POINTER:
            case SQLLPDT_BLOB:
            case SQLLPDT_TEXT:
                out->write(data.ucptr, size);
                break;
            case SQLLPDT_IMAGE:
                out->write(data.ucptr, size);
                out->write(imageWidth);
                out->write(imageHeight);
                out->write(linePitch);
                out->write(input_components);
                out->write(compressionMode);
                break;
            case SQLLPDT_REAL:
            case SQLLPDT_INTEGER:
                out->write((const Nui8 *)&data, size);
                if(out->isFlipData())
                {
                    out->mirror(out->mDatab + out->mWriteb - size, size);
                }
                break;
        }
    }
    //------------------------------------------------------------------------
    bool LogParameter::write(NetSerializer * in)
    {
        bool b;
        unsigned char c;
        in->read(c);
        type = (SQLLoggerPrimaryDataType)c;
        b = in->read(size);
        if(size == 0)
        {
            data.vptr = 0;
            return b;
        }
        switch (type)
        {
        case SQLLPDT_POINTER:
        case SQLLPDT_BLOB:
        case SQLLPDT_TEXT:
            data.vptr = rakMalloc_Ex(size);
            b = in->read(data.ucptr, size);
            break;
        case SQLLPDT_IMAGE:
            data.vptr = rakMalloc_Ex(size);
            in->read(data.ucptr, size);
            in->read(imageWidth);
            in->read(imageHeight);
            in->read(linePitch);
            in->read(input_components);
            b = in->read(compressionMode);
            break;
        case SQLLPDT_REAL:
        case SQLLPDT_INTEGER:
            {
                b = in->read((unsigned char *)&data, size);
                if(in->isFlipData())
                    in->mirror((unsigned char *)&data, size);
            }
            break;
        }
        return b;
    }
    //------------------------------------------------------------------------
    void LogParameter::DoNotFree(void)
    {
        type=SQLLPDT_COUNT;
    }
    //------------------------------------------------------------------------
    void LogParameter::Free(void)
    {
        if (type == SQLLPDT_BLOB || type == SQLLPDT_TEXT || type == SQLLPDT_IMAGE ||
            type == SQLLPDT_POINTER)
        {
            Free(data.vptr);
        }
    }
    //------------------------------------------------------------------------
    void LogParameter::Free(void * v)
    {
        rakFree_Ex(v);
    }
    //------------------------------------------------------------------------
    #pragma pack(push)
    #pragma pack(1)
    // 18 bytes
    struct TGAHEADER {
        char  idlength;
        char  colourmaptype;
        char  datatypecode;
        short int colourmaporigin;
        short int colourmaplength;
        char  colourmapdepth;
        short int x_origin;
        short int y_origin;
        short width;
        short height;
        char  bitsperpixel;
        char  imagedescriptor;
    };
    #pragma pack(pop)
    //------------------------------------------------------------------------
    void RGBImageBlob::SaveToTGA(const char *filename)
    {
    // 	DirectX Color format in memory is BGRA, and written as such to disk.
    // 	Written to disk is the correct side up (point of triangle facing up, as it should). However, TGA displays this incorrectly (upside down)
    // 	TGA color format, on disk, is BGRA.
    // 	DXT compressor input format is ARGB.

        // http://local.wasp.uwa.edu.au/~pbourke/dataformats/tga/
        FILE *fptr = fopen(filename, "wb");
        TGAHEADER h;
        memset(&h,0,sizeof(h));
        h.datatypecode=2;
        h.width = imageWidth;
        if(NetSerializer::isNetEndian() == true)
            Serializer::mirror((Nui8 *) &h.width, sizeof(h.width));
        h.height = imageHeight;
        if(NetSerializer::isNetEndian() == true)
            Serializer::mirror((Nui8 *) &h.height, sizeof(h.height));
        h.bitsperpixel = input_components * 8;

        // TGAs have a flag indicating if they are upside down or right side up
        // Be sure to set right side up.
        // http://www.gamedev.net/community/forums/topic.asp?topic_id=42001
        h.imagedescriptor=(1<<5);

        fwrite(&h,1,sizeof(h),fptr);

        for (int row=0; row < imageHeight; ++row)
        {
            fwrite((char*) data+row*linePitch, input_components, imageWidth, fptr);
        }
        fclose(fptr);
    }
    //------------------------------------------------------------------------
}
}