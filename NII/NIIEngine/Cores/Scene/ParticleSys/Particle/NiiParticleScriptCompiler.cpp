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
#include "NiiParticleScriptCompiler.h"
#include "NiiParticleSpaceManager.h"
#include "NiiParticleGeo.h"
#include "NiiSpreadParticle.h"
#include "NiiParticleEffect.h"

namespace NII
{
    //----------------------------------------------------------------------------
    ParticleScriptCompiler::ParticleScriptCompiler() :
        mSystem(0)
    {
        //mAllowNontypedObjects = true;
    }
    //----------------------------------------------------------------------------
    ParticleSpace * ParticleScriptCompiler::getParticleSpace() const
    {
        return mSystem;
    }
    //----------------------------------------------------------------------------
    bool ParticleScriptCompiler::compileImpl(ScriptNodeList * nodes)
    {
        ScriptNodeList::iterator i = nodes->begin();
        while(i != nodes->end())
        {
            // Delegate some processing to the listener
            if(!processNode(i, nodes->end()))
            {
                // The first just the name of the particle system, but ignore "Base"
                if((*i)->token != "Base")
                {
                    compileParticleSpace(*i);
                }
                ++i;
            }
        }
        return mErrors.empty();
    }
    //----------------------------------------------------------------------------
    bool ParticleScriptCompiler::processNode(ScriptNodeList::iterator & i, ScriptNodeList::iterator & end)
    {
        return false;
    }
    //----------------------------------------------------------------------------
    bool ParticleScriptCompiler::errorRaised(const ScriptCompilerError * error)
    {
        return true;
    }
    //----------------------------------------------------------------------------
    void ParticleScriptCompiler::compileParticleSpace(const ScriptNode * node)
    {
        // Use the listener to get the particle system object
        mSystem = N_Only(ParticleSpace).createTemplate(node->token, mGroup);

        // The particle system does not support extra option after its name, so skip ahead to the '{'
        ScriptNodeList::iterator i = findNode(node->children.begin(), node->children.end(), SNT_LBRACE);
        ScriptNodeList::iterator j = (*i)->children.begin();
        while(j != (*i)->children.end())
        {
            if(!processNode(j, (*i)->children.end()))
            {
                // Each property in the particle system has only 1 value associated with it
                String name = (*j)->token, value;

                if(name == "emitter")
                {
                    compileEmitter(*j);
                }
                else if(name == "affector")
                {
                    compileAffector(*j);
                }
                else
                {
                    // Construct the parameter values from the children of the property
                    value = getParameterValue((*j)->children.begin(), (*j)->children.end());
                    if(!mSystem->set(name, value))
                    {
                        if(mSystem->getRenderer())
                        {
                            if(!mSystem->getRenderer()->set(name, value))
                                error(*j, CE_INVALIDPROPERTY, (*j)->column);
                        }
                    }
                }
                ++j;
            }
        }

        // Reset the pointer to the system
        mSystem = 0;
    }
    //----------------------------------------------------------------------------
    void ParticleScriptCompiler::compileEmitter(const ScriptNode * node)
    {
        if(node->children.empty() || node->children.front()->type != SNT_WORD)
            return;

        // Create the emitter based on the first child
        SpreadParticle *emitter = 0;
        String type = node->children.front()->token;
        try
        {
            emitter = mSystem->addSpread(type);
        }
        catch(...)
        {
            error(node->children.front(), CE_OBJECTALLOCATIONERROR, node->children.front()->column);
            return;
        }

        // Jump ahead now to the '{' as the emitter does not support other parameters in the header
        ScriptNodeList::iterator i = findNode(node->children.begin(), node->children.end(), SNT_LBRACE);
        if(i == node->children.end())
            return;

        ScriptNodeList::iterator j = (*i)->children.begin();
        while(j != (*i)->children.end())
        {
            if(!processNode(j, (*i)->children.end()))
            {
                String name = (*j)->token,
                    value = getParameterValue((*j)->children.begin(), (*j)->children.end());
                if(!emitter->set(name, value))
                    error(*j, CE_INVALIDPROPERTY, (*j)->column);
                ++j;
            }
        }
    }
    //----------------------------------------------------------------------------
    void ParticleScriptCompiler::compileAffector(const ScriptNode * node)
    {
        if(node->children.empty() || node->children.front()->type != SNT_WORD)
            return;

        // Create the emitter based on the first child
        ParticleEffect * affector = 0;
        String type = node->children.front()->token;
        try
        {
            affector = mSystem->addEffect(type);
        }
        catch(...)
        {
            error(node->children.front(), CE_OBJECTALLOCATIONERROR, node->children.front()->column);
            return;
        }

        // Jump ahead now to the '{' as the emitter does not support other parameters in the header
        ScriptNodeList::iterator i = findNode(node->children.begin(), node->children.end(), SNT_LBRACE);
        if(i == node->children.end())
            return;

        ScriptNodeList::iterator j = (*i)->children.begin();
        while(j != (*i)->children.end())
        {
            if(!processNode(j, (*i)->children.end()))
            {
                String name = (*j)->token,
                    value = getParameterValue((*j)->children.begin(), (*j)->children.end());
                if(!affector->set(name, value))
                    error(*j, CE_INVALIDPROPERTY, (*j)->column);
                ++j;
            }
        }
    }
    //----------------------------------------------------------------------------
    String ParticleScriptCompiler::getParameterValue(ScriptNodeList::iterator & i,
        ScriptNodeList::iterator & end)
    {
        String retval;
        if(i != end)
        {
            if((*i)->type == SNT_WORD || (*i)->type == SNT_QUOTE)
                retval = (*i)->token;
            else if((*i)->type == SNT_NUMBER)
                retval = StrConv::conv((*i)->data);
        }

        ++i;
        while(i != end)
        {
            if((*i)->type == SNT_WORD || (*i)->type == SNT_QUOTE)
                retval = retval + " " + (*i)->token;
            else if((*i)->type == SNT_NUMBER)
                retval = retval + " " + StrConv::conv((*i)->data);
            ++i;
        }

        return retval;
    }
    //----------------------------------------------------------------------------
}