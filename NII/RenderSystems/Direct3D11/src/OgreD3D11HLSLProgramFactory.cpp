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

#include "OgreD3D11HLSLProgramFactory.h"
#include "OgreD3D11HLSLProgram.h"
#include "OgreD3D11Device.h"

namespace Ogre {
    //-----------------------------------------------------------------------
    String D3D11HLSLProgramFactory::sLanguageName = "hlsl";
    //-----------------------------------------------------------------------
    D3D11HLSLProgramFactory::D3D11HLSLProgramFactory(D3D11Device & device) :  mDevice(device)
    {
    }
    //-----------------------------------------------------------------------
    D3D11HLSLProgramFactory::~D3D11HLSLProgramFactory()
    {
    }
    //-----------------------------------------------------------------------
    const String& D3D11HLSLProgramFactory::getLanguage(void) const
    {
        return sLanguageName;
    }
    //-----------------------------------------------------------------------
    HighLevelGpuProgram* D3D11HLSLProgramFactory::create(ResourceManager* creator, 
        const String& name, ResourceHandle handle,
        const String& group, bool isManual, ManualResourceLoader* loader)
    {
        return new D3D11HLSLProgram(creator, name, handle, group, isManual, loader, mDevice);
    }
    //-----------------------------------------------------------------------
    void D3D11HLSLProgramFactory::destroy(HighLevelGpuProgram* prog)
    {
        delete prog;
    }
    //-----------------------------------------------------------------------

}
