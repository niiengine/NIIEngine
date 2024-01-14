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

#ifndef _NII_MODULATIVE_SHADOWMAPPING_PATTERN_H
#define _NII_MODULATIVE_SHADOWMAPPING_PATTERN_H

#include "NiiPreInclude.h"

#include "NiiShadowMappingPattern.h"

namespace NII
{
    class IntegrateShadowMapping : public ShadowMappingPattern
    {
    public:
        IntegrateShadowMapping(SpaceManager * parent, RenderSys * rsys, SysSyncParam * param);

        /// @copydetails RenderPattern::prepare
        virtual void prepare(RenderQueue * queue);

        /// @copydetails RenderPattern::prepare
        virtual void prepare(RenderGroup * group);

        /// @copydetails Pattern::getQuality
        Nui8 getQuality() const { return 0; }

        /// @copydetails Pattern::getPerformance
        Nui8 getPerformance() const { return 0; }
    protected:
        /// @copydetails RenderPattern::renderImpl
        void renderImpl(RenderGroup * rg);

        /// @copydetails Pattern::_init
        bool _init() { return true; }

        /// @copydetails Pattern::_update
        void _update() {}

        /// @copydetails Pattern::_end
        bool _end(){ return true; }
        
        /// @copydetails Pattern::getPID
        PatternID getPID() const { return 0; }
    };
    
    class AdditiveShadowMappingPattern : public ShadowMappingPattern
    {
    public:
        AdditiveShadowMappingPattern(SpaceManager * parent, RenderSys * rsys, SysSyncParam * param);

        /// @copydetails RenderPattern::prepare
        virtual void prepare(RenderQueue * queue);

        /// @copydetails RenderPattern::prepare
        virtual void prepare(RenderGroup * group);
        
        /// @copydetails RenderPattern::setShadowColour
        void setShadowColour(const Colour & c);

        /// @copydetails Pattern::getQuality
        Nui8 getQuality() const { return 0; }

        /// @copydetails Pattern::getPerformance
        Nui8 getPerformance() const { return 0; }
    protected:
        /// @copydetails RenderPattern::renderImpl
        void renderImpl(RenderGroup * rg);

        /// @copydetails RenderPattern::deriveReceiver
        ShaderCh * deriveReceiver(const ShaderCh * src);

        /// @copydetails RenderPattern::setTextureFade
        void setTextureFade(NIIf begin, NIIf end);

        /// @copydetails Pattern::_init
        bool _init() { return true; }

        /// @copydetails Pattern::_update
        void _update() {}

        /// @copydetails Pattern::_end
        bool _end(){ return true; }

        /// @copydetails Pattern::getPID
        PatternID getPID() const { return 0; }
    };

    /**
    @version NIIEngine 3.0.0
    */
    class ModulativeShadowMappingPattern : public ShadowMappingPattern
    {
    public:
        ModulativeShadowMappingPattern(SpaceManager * parent, RenderSys * rsys, SysSyncParam * param);

        /// @copydetails RenderPattern::test
        bool testRender(const ShaderCh * ch);

        /// @copydetails RenderPattern::test
        bool testRender(const ShaderCh * ch, const GeometryObj * obj);

        /// @copydetails RenderPattern::prepare
        virtual void prepare(RenderQueue * queue);

        /// @copydetails RenderPattern::prepare
        virtual void prepare(RenderGroup * rg);

        /// @copydetails Pattern::getQuality
        Nui8 getQuality() const { return 0; }

        /// @copydetails Pattern::getPerformance
        Nui8 getPerformance() const { return 0; }
    protected:
        /// @copydetails RenderPattern::renderImpl
        void renderImpl(RenderGroup * rg);

        /// @copydetails Pattern::_init
        bool _init() { return true; }

        /// @copydetails Pattern::_update
        void _update() {}

        /// @copydetails Pattern::_end
        bool _end() { return true; }
        
        /// @copydetails Pattern::getPID
        PatternID getPID() const { return 0; }
    private:
        void renderSolidsBasic(RenderGroup * pGroup);
    };
}
#endif