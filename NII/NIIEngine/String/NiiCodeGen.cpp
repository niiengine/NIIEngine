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
#include "NiiCodeGen.h"
#include "NiiDataStream.h"
#include "NiiException.h"

namespace NII
{
    //------------------------------------------------------------------------
    CodeGen::CodeGen(Nid cid, CodeGen::Type type):
        mIn(0),
        mGenType(type),
        mCodeID(cid)
    {
    }
    //------------------------------------------------------------------------
    CodeGen::CodeGen(const String * name, Nid cid, CodeGen::Type type):
        mIn(name),
        mGenType(type),
        mCodeID(cid)
    {
    }
    //------------------------------------------------------------------------
    CodeGen::~CodeGen()
    {
    }
    //------------------------------------------------------------------------
    void CodeGen::compound(const String *, String *)
    {
        
    }
    //------------------------------------------------------------------------
    void CodeGen::compound(String *)
    {
        
    }
    //------------------------------------------------------------------------
}