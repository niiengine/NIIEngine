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

#include "OgreCgProgramFactory.h"
#include "OgreString.h"
#include "OgreCgProgram.h"

namespace NII 
{
    //-----------------------------------------------------------------------
    CgProgramFactory::CgProgramFactory()
    {
        mCgContext = cgCreateContext();
        // Check for errors
        checkForCgError("CgProgramFactory::CgProgramFactory", 
            "Unable to create initial Cg context: ", mCgContext);
    }
    //-----------------------------------------------------------------------
    CgProgramFactory::~CgProgramFactory()
    {
        cgDestroyContext(mCgContext);
        // Check for errors
        checkForCgError("CgProgramFactory::~CgProgramFactory", 
            "Unable to destroy Cg context: ", mCgContext);
    }
    //-----------------------------------------------------------------------
    FactoryID CgProgramFactory::getID() const
    {
        return N_FACTORY_CGPROGRAM;
    }
    //-----------------------------------------------------------------------
    ShaderLanguage CgProgramFactory::getLanguage() const
    {
        return SL_CG;
    }
    //-----------------------------------------------------------------------
    HighLevelGpuProgram * CgProgramFactory::create(ResourceID rid, GroupID gid, PrcID pid)
    {
        return N_new CgProgram(mManager, rid, gid, pid, mCgContext);
    }
    //-----------------------------------------------------------------------
	void CgProgramFactory::destroy(void * prog)
    {
        CgProgram * temp = static_cast<CgProgram *>(prog);
        N_delete temp;
    }
    //-----------------------------------------------------------------------

}
