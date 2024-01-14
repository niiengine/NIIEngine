//===- llvm/ADT/SmallVector.cpp - 'Normally small' vectors ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License.
// ==============================================================================
// LLVM Release License
// ==============================================================================
// University of Illinois/NCSA
// Open Source License
//
// Copyright (c) 2003-2010 University of Illinois at Urbana-Champaign.
// All rights reserved.
//
// Developed by:
//
// LLVM Team
//
// University of Illinois at Urbana-Champaign
//
// http://llvm.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal with
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// * Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimers.
//
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimers in the
// documentation and/or other materials provided with the distribution.
//
// * Neither the names of the LLVM Team, University of Illinois at
// Urbana-Champaign, nor the names of its contributors may be used to
// endorse or promote products derived from this Software without specific
// prior written permission.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE
// SOFTWARE.
//
//===----------------------------------------------------------------------===//
//
// This file implements the SmallVector class.
//
//===----------------------------------------------------------------------===//
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
#include "NiiSmallVector.h"

namespace NII
{

    /* grow_pod - This is an implementation of the grow() method which only works
        on POD-like datatypes and is out of line to reduce code duplication.
    */
    void SmallVectorBase::grow_pod(NCount MinSizeInBytes, NCount TSize)
    {
        NCount CurSizeBytes = size_in_bytes();
        NCount NewCapacityInBytes = 2 * capacity_in_bytes() + TSize; // Always grow.
        if (NewCapacityInBytes < MinSizeInBytes)
            NewCapacityInBytes = MinSizeInBytes;

        void *NewElts;
        if (this->isSmall())
        {
            NewElts = malloc(NewCapacityInBytes);

            // Copy the elements over.  No need to run dtors on PODs.
            memcpy(NewElts, this->BeginX, CurSizeBytes);
        }
        else
        {
            // If this wasn't grown from the inline copy, grow the allocated space.
            NewElts = realloc(this->BeginX, NewCapacityInBytes);
        }

        this->EndX = (NIIb*)NewElts+CurSizeBytes;
        this->BeginX = NewElts;
        this->CapacityX = (NIIb*)this->BeginX + NewCapacityInBytes;
    }
}
