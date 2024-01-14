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
#include "NiiPreInclude.h"
#include "NiiMemNedPooling.h"

#if NII_MemAllocType == NII_NedPoolAllocType

namespace NII
{
    namespace _NedPoolingIntern
    {
        const size_t s_poolCount = 14; // Needs to be greater than 4
        void * s_poolFootprint = reinterpret_cast<void *>(0xBB1AA45A);
        nedalloc::nedpool * s_pools[s_poolCount + 1] = { 0 };
        nedalloc::nedpool * s_poolsAligned[s_poolCount + 1] = { 0 };
        //--------------------------------------------------------------------
        size_t poolIDFromSize(size_t s)
        {
            // Requests size 16 or smaller are allocated at a 4 byte granularity.
            // Requests size 17 or larger are allocated at a 16 byte granularity.
            // With a s_poolCount of 14, requests size 177 or larger go in the default pool.

            // spreadsheet style =IF(B35<=16; FLOOR((B35-1)/4;1); MIN(FLOOR((B35-1)/16; 1) + 3; 14))

            size_t poolID = 0;

            if (s > 0)
            {
                if (s <= 16)
                {
                    poolID = (s - 1) >> 2;
                }
                else
                {
                    poolID = std::min<size_t>(((s - 1) >> 4) + 3, s_poolCount);
                }
            }

            return poolID;
        }
    }
    using namespace _NedPoolingIntern;
        //--------------------------------------------------------------------
        void * NedPoolingImpl::internalAlloc(size_t s)
        {
            size_t poolid = _NedPoolingIntern::poolIDFromSize(s);
            nedalloc::nedpool * pool(0); // A pool pointer of 0 means the default pool.

            if (poolid < _NedPoolingIntern::s_poolCount)
            {
                if (_NedPoolingIntern::s_pools[poolid] == 0)
                {
                    // Init pool if first use
                    s_pools[poolid] = nedalloc::nedcreatepool(0, 8);
                    // All pools are stamped with a footprint
                    nedalloc::nedpsetvalue(s_pools[poolid], s_poolFootprint);
                }

                pool = s_pools[poolid];
            }

            return nedalloc::nedpmalloc(pool, s);
        }
        //--------------------------------------------------------------------
        void * NedPoolingImpl::internalRealloc(void * p, size_t os, size_t ns)
        {
            size_t npoolid = poolIDFromSize(ns);
            // A pool pointer of 0 means the default pool.
            nedalloc::nedpool * npool(0);

            if (npoolid < s_poolCount)
            {
                if (s_pools[npoolid] == 0)
                {
                    // Init pool if first use
                    s_pools[npoolid] = nedalloc::nedcreatepool(0, 8);
                    // All pools are stamped with a footprint
                    nedalloc::nedpsetvalue(s_pools[npoolid], s_poolFootprint);
                }

                npool = s_pools[npoolid];
            }

            void * np = nedalloc::nedpmalloc(npool, ns);

            if (p)
            {
                memcpy(np, p, N_MIN(size_t, os, ns));
                nedalloc::nedpool * opool(0);

                // nedalloc lets us get the pool pointer from the memory pointer
                void * footprint = nedalloc::nedgetvalue(&opool, p);

                // Check footprint
                if (footprint == s_poolFootprint)
                {
                    // If we allocated the pool, deallocate from this pool...
                    nedalloc::nedpfree(opool, p);
                }
                else
                {
                    // ...otherwise let nedalloc handle it.
                    nedalloc::nedfree(p);
                }
            }
            return np;
        }
        //--------------------------------------------------------------------
        void * NedPoolingImpl::internalAllocAlign(size_t align, size_t s)
        {
            size_t poolid = poolIDFromSize(s);
            // A pool pointer of 0 means the default pool.
            nedalloc::nedpool * pool(0);

            if (poolid < s_poolCount)
            {
                if (s_poolsAligned[poolid] == 0)
                {
                    // Init pool if first use
                    s_poolsAligned[poolid] = nedalloc::nedcreatepool(0, 8);
                    // All pools are stamped with a footprint
                    nedalloc::nedpsetvalue(s_poolsAligned[poolid], s_poolFootprint);
                }

                pool = s_poolsAligned[poolid];
            }

            return nedalloc::nedpmemalign(pool, align, s);
        }
        //--------------------------------------------------------------------
        void * NedPoolingImpl::internalReallocAlign(void * p, size_t align, size_t os, size_t ns)
        {
            size_t npoolid = poolIDFromSize(ns);
            // A pool pointer of 0 means the default pool.
            nedalloc::nedpool * npool(0);

            if (npoolid < s_poolCount)
            {
                if (s_poolsAligned[npoolid] == 0)
                {
                    // Init pool if first use
                    s_poolsAligned[npoolid] = nedalloc::nedcreatepool(0, 8);
                    // All pools are stamped with a footprint
                    nedalloc::nedpsetvalue(s_poolsAligned[npoolid], s_poolFootprint);
                }

                npool = s_poolsAligned[npoolid];
            }

            void * np =  nedalloc::nedpmemalign(npool, align, ns);

            if (p)
            {
                memcpy(np, p, N_MIN(size_t, os, ns));
                nedalloc::nedpool * opool(0);

                // nedalloc lets us get the pool pointer from the memory pointer
                void * footprint = nedalloc::nedgetvalue(&opool, p);

                // Check footprint
                if (footprint == s_poolFootprint)
                {
                    // If we allocated the pool, deallocate from this pool...
                    nedalloc::nedpfree(opool, p);
                }
                else
                {
                    // ...otherwise let nedalloc handle it.
                    nedalloc::nedfree(p);
                }
            }

            return np;
        }
        //--------------------------------------------------------------------
        void NedPoolingImpl::internalFree(void * p)
        {
            if (p)
            {
                nedalloc::nedpool * pool(0);

                // nedalloc lets us get the pool pointer from the memory pointer
                void * footprint = nedalloc::nedgetvalue(&pool, p);

                // Check footprint
                if (footprint == s_poolFootprint)
                {
                    // If we allocated the pool, deallocate from this pool...
                    nedalloc::nedpfree(pool, p);
                }
                else
                {
                    // ...otherwise let nedalloc handle it.
                    nedalloc::nedfree(p);
                }
            }
        }
    //}
    //------------------------------------------------------------------------
}
#endif