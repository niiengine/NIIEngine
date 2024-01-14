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

#ifndef _NII_GLGpuProgramManager_H_
#define _NII_GLGpuProgramManager_H_

#include "NiiGLPreInclude.h"
#include "NiiGpuProgramManager.h"
#include "NiiGLGpuProgram.h"

namespace NII
{
    class _EngineGLAPI GLGpuProgramManager : public GpuProgramManager
    {
    public:
        typedef GpuProgram * (*CreateGpuProgramCallback)(ResourceID rid, GroupID gid,
            ResLoadScheme * ls, ResResultScheme * rs, GpuProgram::ShaderType gptype, ShaderLanguage lang);
    public:
        GLGpuProgramManager();
        ~GLGpuProgramManager();
        bool addCreateFunc(ShaderLanguage lang, CreateGpuProgramCallback cb);
        bool removeCreateFunc(ShaderLanguage lang);
        
        virtual const char * getToken(PixelFormat pf) const;

        virtual const char * getToken(PixelFormat pf, uint32 textureType, bool isMsaa, Buffer::OpType access) const;
    protected:
		/// @copydetails ResourceManager::init
		void init() {}

        /// @copydetails GpuProgramManager::createImpl
        Resource * createImpl(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs,
            GpuProgram::ShaderType type, ShaderLanguage sl);
    private:
        typedef map<ShaderLanguage, CreateGpuProgramCallback>::type ProgramMap;
        ProgramMap mProgramMap;
    };
}
#endif