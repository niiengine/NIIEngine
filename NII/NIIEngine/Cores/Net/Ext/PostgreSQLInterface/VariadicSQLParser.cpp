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

#include "VariadicSQLParser.h"
#include "NiiNetSerializer.h"
#include <stdarg.h>

namespace NII
{
namespace NII_NET
{
	using namespace VariadicSQLParser;
    //------------------------------------------------------------------------
    struct TypeMapping
    {
        char inputType;
        const char * type;
    };
    //------------------------------------------------------------------------
    const int NUM_TYPE_MAPPINGS = 7;
    TypeMapping typeMappings[NUM_TYPE_MAPPINGS] =
    {
        {'i', "int"},
        {'d', "int"},
        {'s', "text"},
        {'b', "bool"},
        {'f', "numeric"},
        {'g', "double precision"},
        {'a', "bytea"},
    };
    //------------------------------------------------------------------------
    Nui32 GetTypeMappingIndex(char c)
    {
        Nui32 i;
        for (i=0; i < (Nui32) NUM_TYPE_MAPPINGS; i++ )
            if (typeMappings[i].inputType==c)
                return i;
        return (Nui32)-1;
    }
    //------------------------------------------------------------------------
    const char * VariadicSQLParser::GetTypeMappingAtIndex(int i)
    {
        return typeMappings[i].type;
    }
    //------------------------------------------------------------------------
    void VariadicSQLParser::GetTypeMappingIndices(const char * format,
        DataStructures::List<IndexAndType> & indices)
    {
        bool previousCharWasPercentSign;
        Nui32 i;
        Nui32 typeMappingIndex;
        indices.Clear(false);
        Nui32 len = (Nui32) strlen(format);
        previousCharWasPercentSign=false;
        for (i=0; i < len; i++)
        {
            if (previousCharWasPercentSign==true )
            {
                typeMappingIndex = GetTypeMappingIndex(format[i]);
                if (typeMappingIndex!=(Nui32) -1)
                {
                    IndexAndType iat;
                    iat.strIndex=i-1;
                    iat.typeMappingIndex=typeMappingIndex;
                    indices.Insert(iat);
                }
            }

            previousCharWasPercentSign=format[i]=='%';
        }
    }
    //------------------------------------------------------------------------
    void VariadicSQLParser::ExtractArguments(va_list argptr, const DataStructures::List<IndexAndType> &indices, char ***argumentBinary, int **argumentLengths )
    {
        if (indices.Size()==0)
            return;

        Nui32 i;
        *argumentBinary = N_alloc_t(char *, indices.Size());
        *argumentLengths = N_alloc_t(int, indices.Size());

        char **paramData=*argumentBinary;
        int *paramLength=*argumentLengths;

        int variadicArgIndex;
        for (variadicArgIndex = 0, i = 0; i < indices.Size(); i++, variadicArgIndex++)
        {
            switch (typeMappings[indices[i].typeMappingIndex].inputType)
            {
            case 'i':
            case 'd':
                {
                    int val = va_arg( argptr, int );
                    paramLength[i]=sizeof(val);
                    paramData[i] = (char *)N_alloc(paramLength[i]);
                    memcpy(paramData[i], &val, paramLength[i]);
                    if(NetSerializer::isNetEndian()==false) 
                        Serializer::mirror((Nui8 *)paramData[i], paramLength[i]);
                }
                break;
            case 's':
                {
                    char* val = va_arg( argptr, char* );
                    paramLength[i]=(int) strlen(val);
                    paramData[i] = (char *)N_alloc(paramLength[i] + 1);
                    memcpy(paramData[i], val, paramLength[i]+1);
                }
                break;
            case 'b':
                {
                    bool val = (va_arg( argptr, int )!=0);
                    paramLength[i] = sizeof(val);
                    paramData[i] = (char *)N_alloc(paramLength[i]);
                    memcpy(paramData[i], &val, paramLength[i]);
                    if(NetSerializer::isNetEndian()==false)
                        Serializer::mirror((Nui8 *)paramData[i], paramLength[i]);
                }
                break;
                /*
            case 'f':
                {
                    // On MSVC at least, this only works with NIId as the 2nd param
                    NIIf val = (NIIf) va_arg( argptr, NIId );
                    //NIIf val = va_arg( argptr, NIIf );
                    paramLength[i]=sizeof(val);
                    paramData[i]=(char*) N_alloc(paramLength[i]);
                    memcpy(paramData[i], &val, paramLength[i]);
                    if (NetSerializer::isNetEndian()==false)
                        Serializer::mirror((Nui8 *)paramData[i], paramLength[i]);
                }
                break;
                */
            // On MSVC at least, this only works with NIId as the 2nd param
            case 'f':
            case 'g':
                {
                    NIId val = va_arg( argptr, NIId );
                    paramLength[i] = sizeof(val);
                    paramData[i] = (char *)N_alloc(paramLength[i]);
                    memcpy(paramData[i], &val, paramLength[i]);
                    if(NetSerializer::isNetEndian() == false)
                        Serializer::mirror((Nui8 *)paramData[i], paramLength[i]);
                }
                break;
            case 'a':
                {
                    char* val = va_arg(argptr, char *);
                    paramLength[i] = va_arg(argptr, Nui32);
                    paramData[i] = (char *)N_alloc(paramLength[i]);
                    memcpy(paramData[i], val, paramLength[i]);
                }
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    void VariadicSQLParser::FreeArguments(const DataStructures::List<IndexAndType> & indices, char **argumentBinary, int *argumentLengths)
    {
        if (indices.Size() == 0)
            return;

        NCount i;
        for (i = 0; i < indices.Size(); ++i)
            N_free(argumentBinary[i]);
        N_free(argumentBinary);
        N_free(argumentLengths);
    }
    //------------------------------------------------------------------------
}
}