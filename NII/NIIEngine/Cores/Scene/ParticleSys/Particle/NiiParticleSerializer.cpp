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
#include "NiiParticleSerializer.h"

namespace NII
{
    //------------------------------------------------------------------------
    ParticleSerializer::ParticleSerializer()
    {

    }
    //------------------------------------------------------------------------
    ParticleSerializer::~ParticleSerializer()
    {

    }
    //------------------------------------------------------------------------
    void ParticleSerializer::write(const ParticleSpace * in, const String & file,
        SerialVer ver, SerialFormat sf, Endian emode)
    {

    }
    ///-----------------------------------------------------------------------
    void ParticleSerializer::write(const ParticleSpace * in, DataStream * stream,
            SerialVer ver, SerialFormat sf, Endian emode)
    {

    }
    //------------------------------------------------------------------------
    void ParticleSerializer::read(DataStream * stream, ParticleSpace * out)
    {

    }
    //------------------------------------------------------------------------
    void ParticleSerializer::readUnit(void * out)
    {

    }
    //------------------------------------------------------------------------
    void ParticleSerializer::writeUnit(const void * in, SerialVer ver, SerialFormat sf, Endian emode)
    {

    }
    //------------------------------------------------------------------------
}