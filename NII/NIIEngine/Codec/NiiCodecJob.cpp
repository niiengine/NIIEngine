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
#include "NiiCodecJob.h"

namespace NII
{
    //------------------------------------------------------------------------
    CodecJob::CodecJob(NCount total, DataType type, const Any & any) :
        mTotal(total),
        mDataType(type),
        mData(any),
        mComplete(0)
    {
    }
    //------------------------------------------------------------------------
    CodecJob::~CodecJob()
    {

    }
    //------------------------------------------------------------------------
    void CodecJob::setData(const Any & data)
    {
        mData = data;
    }
    //------------------------------------------------------------------------
    const Any & CodecJob::getData() const
    {
        return mData;
    }
    //------------------------------------------------------------------------
    void CodecJob::setDataType(DataType type)
    {
        mDataType = type;
    }
    //------------------------------------------------------------------------
    CodecJob::DataType CodecJob::getDataType() const
    {
        return mDataType;
    }
    //------------------------------------------------------------------------
    bool CodecJob::run()
    {
        return false;
    }
    //------------------------------------------------------------------------
    void CodecJob::onRequest(NCount count)
    {

    }
    //------------------------------------------------------------------------
}