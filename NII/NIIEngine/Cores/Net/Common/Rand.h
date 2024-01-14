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

#ifndef __RAND_H
#define __RAND_H

#include "NiiNetPreInclude.h"
#include "Rand.h"
#include "DS_Queue.h"
    
/// Initialise seed for Random Generator
/// \note not threadSafe, use an instance of RakNetRandom if necessary per thread
/// \param[in] seed The seed value for the random number generator.
extern void _EngineAPI seedMT(Nui32 seed);

/// \internal
/// \note not threadSafe, use an instance of RakNetRandom if necessary per thread
extern Nui32 _EngineAPI reloadMT(void);

/// Gets a random Nui32
/// \note not threadSafe, use an instance of RakNetRandom if necessary per thread
/// \return an integer random value.
extern Nui32 _EngineAPI randomMT(void);

/// Gets a random NIIf
/// \note not threadSafe, use an instance of RakNetRandom if necessary per thread
/// \return 0 to 1.0f, inclusive
extern NIIf _EngineAPI frandomMT(void);

/// Randomizes a buffer
/// \note not threadSafe, use an instance of RakNetRandom if necessary per thread
extern void _EngineAPI fillBufferMT(void * buffer, Nui32 bytes);

namespace NII
{
namespace NII_NET
{
    class _EngineAPI RakNetRandomSync
    {
    public:
        RakNetRandomSync();
        virtual ~RakNetRandomSync();
        void SeedMT(Nui32 _seed);
        void SeedMT(Nui32 _seed, Nui32 skipValues);
        NIIf FrandomMT(void);
        Nui32 RandomMT(void);
        Nui32 GetSeed(void) const;
        Nui32 GetCallCount(void) const;
        void SetCallCount(Nui32 i);

        virtual void SerializeConstruction(NetSerializer * constructionBitstream);
        virtual bool DeserializeConstruction(NetSerializer * constructionBitstream);
        virtual void read(NetSerializer * out);
        virtual void import(NetSerializer * in);
    protected:
        void Skip(Nui32 count);
        DataStructures::Queue<Nui32> usedValues;
        Nui32 seed;
        Nui32 callCount;
        Nui32 usedValueBufferCount;
        RakNetRandom rnr;
    };
    
    // Same thing as above functions, but not global
    class _EngineAPI RakNetRandom
    {
    public:
        RakNetRandom();
        ~RakNetRandom();
        void SeedMT(Nui32 seed);
        Nui32 ReloadMT(void);
        Nui32 RandomMT(void);
        NIIf FrandomMT(void);
        void FillBufferMT(void * buffer, Nui32 bytes);
    protected:
        Nui32 state[ 624 + 1 ];
        Nui32 *next;
        int left;
    };
}
}
#endif