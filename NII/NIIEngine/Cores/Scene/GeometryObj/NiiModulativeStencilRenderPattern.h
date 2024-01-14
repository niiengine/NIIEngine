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

#ifndef NII_MODULATIVE_STENCILSHADOW_PATTERN_H
#define NII_MODULATIVE_STENCILSHADOW_PATTERN_H

#include "NiiPreInclude.h"
#include "NiiStencilRenderPattern.h"

namespace NII
{
    /**
    @remark
    */
    class AdditiveStencilRenderPattern : public StencilRenderPattern
    {
    public:
        AdditiveStencilRenderPattern(SpaceManager * sm, RenderSys * rsy, SysSyncParam * param);

        /// @copydetails RenderPattern::prepare
        void prepare(RenderQueue * queue);

        /// @copydetails RenderPattern::prepare
        void prepare(RenderGroup * group);

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
    
    /**
    @remark
    */
    class ModulativeStencilRenderPattern : public StencilRenderPattern
    {
    public:
        ModulativeStencilRenderPattern(SpaceManager * sm, RenderSys * rsys, SysSyncParam * param);

        /// @copydetails RenderPattern::prepare
        void prepare(RenderQueue * queue);

        /// @copydetails RenderPattern::prepare
        void prepare(RenderGroup * group);
            
        /// @copydetails Pattern::getQuality
        Nui8 getQuality() const { return 0; }

        /// @copydetails Pattern::getPerformance
        Nui8 getPerformance() const { return 0; }
    protected:
        /// @copydetails RenderPattern::renderImpl
        void renderImpl(RenderGroup * rg);

        /// @copydetails RenderPattern::applyDarkCap
        void applyDarkCap(const Light * lig, const Camera * camera,
            ShadowObj * caster, NIIf extrude, Nmark & flags);
            
        /// @copydetails Pattern::_init
        bool _init() { return true; }

        /// @copydetails Pattern::_update
        void _update() {}

        /// @copydetails Pattern::_end
        bool _end() { return true; }
        
        /// @copydetails Pattern::getPID
        PatternID getPID() const { return 0; }            
    };
}
#endif