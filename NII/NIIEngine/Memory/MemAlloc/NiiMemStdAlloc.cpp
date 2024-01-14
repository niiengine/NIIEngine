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
#include "NiiPlatInfo.h"
#include "NiiBitwise.h"

/**
*
* |___2___|3|_________5__________|__6__|
* ^         ^
* 1         4
*
* 1 -> ָ��new��������Ŀռ�
* 2 -> Gap used to get 4 aligned on given alignment
* 3 -> �洢1��4֮���ƫ����(8λ,1�ֽ�)
* 4 -> ָ�����ݿ�Ŀ�ʼ��
* 5 -> ���ݿ�.
* 6 -> ���ݿ����˷ѵ��Ŀռ�(�����ڶ�����)
*/
#if NII_MemAllocType == NII_StdAllocType
namespace NII
{
    //---------------------------------------------------------------------
    void * AlignedMemory::alloc(size_t s, size_t align)
    {
        assert(0 < align && align <= 128 && Bitwise::isPO2(align));

        unsigned char * p = new unsigned char[s + align];
        size_t offset = align - (size_t(p) & (align-1));

        unsigned char * result = p + offset;
        result[-1] = (unsigned char)offset;

        return result;
    }
    //---------------------------------------------------------------------
    void * AlignedMemory::alloc(size_t s)
    {
        return alloc(s, N_SIMD_ALIGNMENT);
    }
    //---------------------------------------------------------------------
    void * AlignedMemory::realloc(void * p, size_t os, size_t ns, size_t align)
    {
        unsigned char * np = new unsigned char[ns + align];
        size_t offset = align - (size_t(np) & (align-1));

        unsigned char * result = np + offset;
        result[-1] = (unsigned char)offset;

        memcpy(np, p, N_MIN(size_t, os, ns));
        dealloc(p);
        return result;
    }
    //---------------------------------------------------------------------
    void * AlignedMemory::realloc(void * p, size_t os, size_t ns)
    {
        return realloc(p, os, ns, N_SIMD_ALIGNMENT);
    }
    //---------------------------------------------------------------------
    void AlignedMemory::dealloc(void * p)
    {
        if (p)
        {
            unsigned char * mem = (unsigned char *)p;
            mem = mem - mem[-1];
            delete [] mem;
        }
    }
    //--------------------------------------------------------------------
}
#endif