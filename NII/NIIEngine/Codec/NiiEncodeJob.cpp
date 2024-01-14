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
#include "NiiEncodeJob.h"

namespace NII
{
    //------------------------------------------------------------------------
    EncodeJob::EncodeJob() :
        CodecJob(0, DT_None, Any()),
        mIn(NULL)
    {
    }
    //------------------------------------------------------------------------
    EncodeJob::EncodeJob(NCount total, DataType type, const Any & any) : 
        CodecJob(total, type, any),
        mIn(NULL)
    {
    }
    //------------------------------------------------------------------------
    EncodeJob::~EncodeJob()
    {
    }
    //------------------------------------------------------------------------
    MemStream * EncodeJob::getIn()
    {
        return mIn;
    }
    //------------------------------------------------------------------------
    void EncodeJob::onRequest(NCount count)
    {

    }
    //------------------------------------------------------------------------
}