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
#include "NiiHash.h"

#include <cstddef>

//-------------------------------------------------------------------
/** General hash function, derived from here
http://www.azillionmonkeys.com/qed/hash.html
Original by Paul Hsieh
*/
#if N_ENDIAN == N_ENDIAN_L
#define NII_GET16BITS(d) (*((const Nui16 *) (d)))
#else
// Cast to Nui16 in little endian means first byte is least significant
// replicate that here
#define NII_GET16BITS(d) (*((const Nui8 *) (d)) + (*((const Nui8 *) (d+1))<<8))
#endif

// Microsoft Visual Studio

#if defined(_MSC_VER)
    #include <stdlib.h>

    #define ROTL32(x,y) _rotl(x,y)
    #define ROTL64(x,y) _rotl64(x,y)

    #define BIG_CONSTANT(x) (x)
#else   // Other compilers
    inline Nui32 rotl32 ( Nui32 x, int8_t r )
    {
      return (x << r) | (x >> (32 - r));
    }

    inline Nui64 rotl64 ( Nui64 x, int8_t r )
    {
      return (x << r) | (x >> (64 - r));
    }

    #define ROTL32(x,y) rotl32(x,y)
    #define ROTL64(x,y) rotl64(x,y)

    #define BIG_CONSTANT(x) (x##LLU)

#endif // defined(_MSC_VER)

namespace NII
{
    //------------------------------------------------------------------------
    Nui32 _EngineAPI FastHash(const Nui8 * data, NCount len, Nui32 hashSoFar)
    {
        Nui32 ret;
        MurmurHash_x86_32(data, len, hashSoFar, &ret);
        return ret;
#if 0
        Nui32 hash;
        Nui32 tmp;
        NIIi rem;

        if (hashSoFar)
            hash = hashSoFar;
        else
            hash = len;

        if (len <= 0 || data == NULL) return 0;

        rem = len & 3;
        len >>= 2;

        /* Main loop */
        for(;len > 0; --len)
        {
            hash  += NII_GET16BITS (data);
            tmp    = (NII_GET16BITS (data+2) << 11) ^ hash;
            hash   = (hash << 16) ^ tmp;
            data  += 2*sizeof (Nui16);
            hash  += hash >> 11;
        }

        /* Handle end cases */
        switch(rem)
        {
        case 3: hash += NII_GET16BITS (data);
            hash ^= hash << 16;
            hash ^= data[sizeof (Nui16)] << 18;
            hash += hash >> 11;
            break;
        case 2: hash += NII_GET16BITS (data);
            hash ^= hash << 11;
            hash += hash >> 17;
            break;
        case 1: hash += *data;
            hash ^= hash << 10;
            hash += hash >> 1;
        }

        /* Force "avalanching" of final 127 bits */
        hash ^= hash << 3;
        hash += hash >> 5;
        hash ^= hash << 4;
        hash += hash >> 17;
        hash ^= hash << 25;
        hash += hash >> 6;
        return hash;
#endif
    }
    //------------------------------------------------------------------------
    Nui32 _EngineAPI FastHash(FILE * fp)
    {
        fseek(fp, 0, SEEK_END);
		int bytesRemaining = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        Nui32 lastHash = 0;
		Nui8 rblock[8192];
        while (bytesRemaining>=(int) sizeof(rblock))
        {
            fread(rblock, sizeof(rblock), 1, fp);
            lastHash = FastHash (rblock, sizeof(rblock), lastHash );
            bytesRemaining-=(int) sizeof(rblock);
        }
        if (bytesRemaining>0)
        {
            fread(rblock, bytesRemaining, 1, fp);
            lastHash = FastHash (rblock, bytesRemaining, lastHash );
        }
        return lastHash;
    }
    //------------------------------------------------------------------------
    
    //-----------------------------------------------------------------------------
    // MurmurHash3 was written by Austin Appleby, and is placed in the public
    // domain. The author hereby disclaims copyright to this source code.

    // Note - The x86 and x64 versions do _not_ produce the same results, as the
    // algorithms are optimized for their respective platforms. You can still
    // compile and run any of them on any platform, but your performance with the
    // non-native version will be less than optimal.
    //-----------------------------------------------------------------------------
    // Block read - if your platform needs to do endian-swapping or can only
    // handle aligned reads, do the conversion here

    N_FORCEINLINE Nui32 getblock32 ( const Nui32 * p, int i )
    {
        return p[i];
    }

    N_FORCEINLINE Nui64 getblock64 ( const Nui64 * p, int i )
    {
        return p[i];
    }

    //-----------------------------------------------------------------------------
    // Finalization mix - force all bits of a hash block to avalanche

    N_FORCEINLINE Nui32 fmix32 ( Nui32 h )
    {
        h ^= h >> 16;
        h *= 0x85ebca6b;
        h ^= h >> 13;
        h *= 0xc2b2ae35;
        h ^= h >> 16;

        return h;
    }

    //----------

    N_FORCEINLINE Nui64 fmix64 ( Nui64 k )
    {
        k ^= k >> 33;
        k *= BIG_CONSTANT(0xff51afd7ed558ccd);
        k ^= k >> 33;
        k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
        k ^= k >> 33;

        return k;
    }

    //-----------------------------------------------------------------------------
    // MurmurHash2, 64-bit versions, by Austin Appleby

    // The same caveats as 32-bit MurmurHash2 apply here - beware of alignment 
    // and endian-ness issues if used across multiple platforms.

    // 64-bit hash for 64-bit platforms

    Nui64 _EngineAPI MurmurHash64A(const void * key, NCount len, Nui64 seed)
    {
        const Nui64 m = BIG_CONSTANT(0xc6a4a7935bd1e995);
        const int r = 47;

        Nui64 h = seed ^ (len * m);

        const Nui64 * data = (const Nui64 *)key;
        const Nui64 * end = data + (len / 8);

        while (data != end)
        {
            Nui64 k = *data++;

            k *= m;
            k ^= k >> r;
            k *= m;

            h ^= k;
            h *= m;
        }

        const unsigned char * data2 = (const unsigned char*)data;

        switch (len & 7)
        {
        case 7: h ^= Nui64(data2[6]) << 48;
        case 6: h ^= Nui64(data2[5]) << 40;
        case 5: h ^= Nui64(data2[4]) << 32;
        case 4: h ^= Nui64(data2[3]) << 24;
        case 3: h ^= Nui64(data2[2]) << 16;
        case 2: h ^= Nui64(data2[1]) << 8;
        case 1: h ^= Nui64(data2[0]);
            h *= m;
        };

        h ^= h >> r;
        h *= m;
        h ^= h >> r;

        return h;
    }

    // 64-bit hash for 32-bit platforms

    Nui64 _EngineAPI MurmurHash64B(const void * key, NCount len, Nui64 seed)
    {
        const uint32_t m = 0x5bd1e995;
        const int r = 24;

        uint32_t h1 = uint32_t(seed) ^ len;
        uint32_t h2 = uint32_t(seed >> 32);

        const uint32_t * data = (const uint32_t *)key;

        while (len >= 8)
        {
            uint32_t k1 = *data++;
            k1 *= m; k1 ^= k1 >> r; k1 *= m;
            h1 *= m; h1 ^= k1;
            len -= 4;

            uint32_t k2 = *data++;
            k2 *= m; k2 ^= k2 >> r; k2 *= m;
            h2 *= m; h2 ^= k2;
            len -= 4;
        }

        if (len >= 4)
        {
            uint32_t k1 = *data++;
            k1 *= m; k1 ^= k1 >> r; k1 *= m;
            h1 *= m; h1 ^= k1;
            len -= 4;
        }

        switch (len)
        {
        case 3: h2 ^= ((unsigned char*)data)[2] << 16;
        case 2: h2 ^= ((unsigned char*)data)[1] << 8;
        case 1: h2 ^= ((unsigned char*)data)[0];
            h2 *= m;
        };

        h1 ^= h2 >> 18; h1 *= m;
        h2 ^= h1 >> 22; h2 *= m;
        h1 ^= h2 >> 17; h1 *= m;
        h2 ^= h1 >> 19; h2 *= m;

        Nui64 h = h1;

        h = (h << 32) | h2;

        return h;
    }
    //-----------------------------------------------------------------------------
    void _EngineAPI MurmurHash_x86_32(const void * key, NCount len, Nui32 seed, Nui32 & out)
    {
        const uint8_t * data = (const uint8_t*)key;
        const int nblocks = len / 4;

        Nui32 h1 = seed;

        const Nui32 c1 = 0xcc9e2d51;
        const Nui32 c2 = 0x1b873593;

        //----------
        // body

        const Nui32 * blocks = (const Nui32 *)(data + nblocks * 4);

        for(int i = -nblocks; i; i++)
        {
            Nui32 k1 = getblock32(blocks,i);

            k1 *= c1;
            k1 = ROTL32(k1,15);
            k1 *= c2;
    
            h1 ^= k1;
            h1 = ROTL32(h1,13); 
            h1 = h1*5+0xe6546b64;
        }

        //----------
        // tail

        const uint8_t * tail = (const uint8_t*)(data + nblocks*4);

        Nui32 k1 = 0;

        switch(len & 3)
        {
        case 3: k1 ^= tail[2] << 16; N_FALLTHROUGH;
        case 2: k1 ^= tail[1] << 8; N_FALLTHROUGH;
        case 1: k1 ^= tail[0];
              k1 *= c1; k1 = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
        };

        //----------
        // finalization

        h1 ^= len;

        h1 = fmix32(h1);

        out = h1;
    } 

    //-----------------------------------------------------------------------------

    void _EngineAPI MurmurHash_x86_128(const void * key, NCount len, const Nui32 & seed[4], Nui32 & out[4])
    {
        const uint8_t * data = (const uint8_t*)key;
        const int nblocks = len / 16;

        Nui32 h1 = seed[0];
        Nui32 h2 = seed[1];
        Nui32 h3 = seed[2];
        Nui32 h4 = seed[3];

        const Nui32 c1 = 0x239b961b; 
        const Nui32 c2 = 0xab0e9789;
        const Nui32 c3 = 0x38b34ae5; 
        const Nui32 c4 = 0xa1e38b93;

        //----------
        // body

        const Nui32 * blocks = (const Nui32 *)(data + nblocks*16);

        for(int i = -nblocks; i; i++)
        {
            Nui32 k1 = getblock32(blocks,i*4+0);
            Nui32 k2 = getblock32(blocks,i*4+1);
            Nui32 k3 = getblock32(blocks,i*4+2);
            Nui32 k4 = getblock32(blocks,i*4+3);

            k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;

            h1 = ROTL32(h1,19); h1 += h2; h1 = h1*5+0x561ccd1b;

            k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;

            h2 = ROTL32(h2,17); h2 += h3; h2 = h2*5+0x0bcaa747;

            k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;

            h3 = ROTL32(h3,15); h3 += h4; h3 = h3*5+0x96cd1c35;

            k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;

            h4 = ROTL32(h4,13); h4 += h1; h4 = h4*5+0x32ac3b17;
        }

        //----------
        // tail

        const uint8_t * tail = (const uint8_t*)(data + nblocks*16);

        Nui32 k1 = 0;
        Nui32 k2 = 0;
        Nui32 k3 = 0;
        Nui32 k4 = 0;

        switch(len & 15)
        {
        case 15: k4 ^= tail[14] << 16; N_FALLTHROUGH;
        case 14: k4 ^= tail[13] << 8; N_FALLTHROUGH;
        case 13: k4 ^= tail[12] << 0;
               k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4; N_FALLTHROUGH;

        case 12: k3 ^= tail[11] << 24; N_FALLTHROUGH;
        case 11: k3 ^= tail[10] << 16; N_FALLTHROUGH;
        case 10: k3 ^= tail[ 9] << 8; N_FALLTHROUGH;
        case  9: k3 ^= tail[ 8] << 0;
               k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3; N_FALLTHROUGH;

        case  8: k2 ^= tail[ 7] << 24; N_FALLTHROUGH;
        case  7: k2 ^= tail[ 6] << 16; N_FALLTHROUGH;
        case  6: k2 ^= tail[ 5] << 8; N_FALLTHROUGH;
        case  5: k2 ^= tail[ 4] << 0;
               k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2; N_FALLTHROUGH;

        case  4: k1 ^= tail[ 3] << 24; N_FALLTHROUGH;
        case  3: k1 ^= tail[ 2] << 16; N_FALLTHROUGH;
        case  2: k1 ^= tail[ 1] << 8; N_FALLTHROUGH;
        case  1: k1 ^= tail[ 0] << 0;
               k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
        };

        //----------
        // finalization

        h1 ^= len; h2 ^= len; h3 ^= len; h4 ^= len;

        h1 += h2; h1 += h3; h1 += h4;
        h2 += h1; h3 += h1; h4 += h1;

        h1 = fmix32(h1);
        h2 = fmix32(h2);
        h3 = fmix32(h3);
        h4 = fmix32(h4);

        h1 += h2; h1 += h3; h1 += h4;
        h2 += h1; h3 += h1; h4 += h1;

        out[0] = h1;
        out[1] = h2;
        out[2] = h3;
        out[3] = h4;
    }
    //-----------------------------------------------------------------------------
    void _EngineAPI MurmurHash_x64_128 (const void * key, NCount len, const Nui64 & seed[2], Nui64 & out[2])
    {
        const uint8_t * data = (const uint8_t*)key;
        const int nblocks = len / 16;

        Nui64 h1 = seed[0];
        Nui64 h2 = seed[1];

        const Nui64 c1 = BIG_CONSTANT(0x87c37b91114253d5);
        const Nui64 c2 = BIG_CONSTANT(0x4cf5ad432745937f);

        //----------
        // body

        const Nui64 * blocks = (const Nui64 *)(data);

        for(int i = 0; i < nblocks; i++)
        {
            Nui64 k1 = getblock64(blocks,i*2+0);
            Nui64 k2 = getblock64(blocks,i*2+1);

            k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;

            h1 = ROTL64(h1,27); h1 += h2; h1 = h1*5+0x52dce729;

            k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;

            h2 = ROTL64(h2,31); h2 += h1; h2 = h2*5+0x38495ab5;
        }

        //----------
        // tail

        const uint8_t * tail = (const uint8_t*)(data + nblocks*16);

        Nui64 k1 = 0;
        Nui64 k2 = 0;

        switch(len & 15)
        {
        case 15: k2 ^= ((Nui64)tail[14]) << 48; N_FALLTHROUGH;
        case 14: k2 ^= ((Nui64)tail[13]) << 40; N_FALLTHROUGH;
        case 13: k2 ^= ((Nui64)tail[12]) << 32; N_FALLTHROUGH;
        case 12: k2 ^= ((Nui64)tail[11]) << 24; N_FALLTHROUGH;
        case 11: k2 ^= ((Nui64)tail[10]) << 16; N_FALLTHROUGH;
        case 10: k2 ^= ((Nui64)tail[ 9]) << 8;  N_FALLTHROUGH;
        case  9: k2 ^= ((Nui64)tail[ 8]) << 0;
               k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2; N_FALLTHROUGH;

        case  8: k1 ^= ((Nui64)tail[ 7]) << 56; N_FALLTHROUGH;
        case  7: k1 ^= ((Nui64)tail[ 6]) << 48; N_FALLTHROUGH;
        case  6: k1 ^= ((Nui64)tail[ 5]) << 40; N_FALLTHROUGH;
        case  5: k1 ^= ((Nui64)tail[ 4]) << 32; N_FALLTHROUGH;
        case  4: k1 ^= ((Nui64)tail[ 3]) << 24; N_FALLTHROUGH;
        case  3: k1 ^= ((Nui64)tail[ 2]) << 16; N_FALLTHROUGH;
        case  2: k1 ^= ((Nui64)tail[ 1]) << 8;  N_FALLTHROUGH;
        case  1: k1 ^= ((Nui64)tail[ 0]) << 0;
               k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;
        };

        //----------
        // finalization

        h1 ^= len; h2 ^= len;

        h1 += h2;
        h2 += h1;

        h1 = fmix64(h1);
        h2 = fmix64(h2);

        h1 += h2;
        h2 += h1;

        out[0] = h1;
        out[1] = h2;
    }
    //-----------------------------------------------------------------------------
}