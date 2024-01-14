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
#include "NiiDisplayBuffer.h"
#include "NiiMaterialManager.h"
#include "NiiMaterial.h"

namespace NII
{
    //-----------------------------------------------------------------------
    DisplayBuffer::DisplayBuffer() :
        mGeoOp(GeometryRaw::OT_TRIANGLE_LIST),
        mMaterial(0),
        mMaxVertexCount(1000),
        mAutoFlush(false),
        mRequestFlush(true)
    {
        mVertexData = N_new VertexData;
    }
    //-----------------------------------------------------------------------
    DisplayBuffer::~DisplayBuffer()
    {
        N_delete mVertexData;
        
        if(mMaterial)
        {
            mMaterial->unref();
            mMaterial = 0;
        }
    }
    //-----------------------------------------------------------------------
    void DisplayBuffer::setMaterialID(ResourceID rid)
    {
        mMaterial = static_cast<Material *>(N_Only(Material).get(rid));

        if(mMaterial == 0)
            N_EXCEPT(NotExist, _I18n("Could not find material ") + rid);

        /* Ensure that the new material was loaded (will not load again if
           already loaded anyway)
        */
        mMaterial->load();
        mMaterial->ref(false);
    }
    //-----------------------------------------------------------------------
    void DisplayBuffer::reflush() 
    { 
        mRequestFlush = true; 
    }
    //-----------------------------------------------------------------------
}
