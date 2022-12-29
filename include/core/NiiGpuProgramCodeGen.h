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

#ifndef _NII_GpuProgramCODEGEN_H_
#define _NII_GpuProgramCODEGEN_H_

#include "NiiPreInclude.h"
#include "NiiCodeGen.h"

namespace NII
{
    /** 编码生成器
    @remark
        编码合成与生成
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuProgramCodeGen : public CodeGen
    {
    public:

    public:
        GpuProgramCodeGen(const String * name, Nid codeid);
        virtual ~GpuProgramCodeGen();

        /// @copydetails CodeGen::compound
        void compound(const String * in, String * out);
        
        /// @copydetails CodeGen::compound
        void compound(String * out);
        
        virtual void setRender(RenderSys * rsys);
        
        RenderSys * getRender() const                      { return mRenderSystem; }
        
        virtual void clearShaderCache();
    protected:
        RenderStateCacheList mShaderCache;
    };
}
#endif