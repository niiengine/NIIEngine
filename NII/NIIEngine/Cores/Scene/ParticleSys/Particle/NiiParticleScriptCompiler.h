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

#ifndef _NII_PARTICLESCRIPTCOMPILER_H_
#define _NII_PARTICLESCRIPTCOMPILER_H_

#include "NiiPreInclude.h"
#include "NiiScriptCompiler.h"

namespace NII
{
    class _EngineAPI ParticleScriptCompiler : public ScriptCompiler
    {
    public:
        ParticleScriptCompiler();

        /// Returns the particle system currently being compiled
        ParticleSpace * getParticleSpace() const;
    protected:
        /// This begins the compilation of the particle system from the final transformed AST
        bool compileImpl(ScriptNodeList * nodes);

        /// Delegates to the listener if it can, otherwise returns false. If it returns true, then some input was consumed.
        bool processNode(ScriptNodeList::iterator & i, ScriptNodeList::iterator & end);

        /// Allows a listener to override error handling in the compiler
        bool errorRaised(const ScriptCompilerError * error);
    private: // Handlers for compiling script elements
        void compileParticleSpace(const ScriptNode * node);

        void compileEmitter(const ScriptNode * node);

        void compileAffector(const ScriptNode * node);

        String getParameterValue(ScriptNodeList::iterator & i, ScriptNodeList::iterator & end);
    private: // Listener and context data
        ParticleSpace * mSystem;
    };
}
#endif