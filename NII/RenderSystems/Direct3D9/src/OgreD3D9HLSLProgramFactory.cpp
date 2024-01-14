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

#include "OgreD3D9HLSLProgramFactory.h"
#include "OgreString.h"
#include "OgreD3D9HLSLProgram.h"

namespace Ogre {
    //-----------------------------------------------------------------------
    String D3D9HLSLProgramFactory::sLanguageName = "hlsl";
    //-----------------------------------------------------------------------
    D3D9HLSLProgramFactory::D3D9HLSLProgramFactory()
    {
    }
    //-----------------------------------------------------------------------
    D3D9HLSLProgramFactory::~D3D9HLSLProgramFactory()
    {
    }
    //-----------------------------------------------------------------------
    const String& D3D9HLSLProgramFactory::getLanguage(void) const
    {
        return sLanguageName;
    }
    //-----------------------------------------------------------------------
    HighLevelGpuProgram* D3D9HLSLProgramFactory::create(ResourceManager* creator, 
        const String& name, ResourceHandle handle,
        const String& group, bool isManual, ManualResourceLoader* loader)
    {
        return N_new D3D9HLSLProgram(creator, name, handle, group, isManual, loader);
    }
    //-----------------------------------------------------------------------
    void D3D9HLSLProgramFactory::destroy(HighLevelGpuProgram* prog)
    {
        N_delete prog;
    }
    //-----------------------------------------------------------------------

}
