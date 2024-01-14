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
#include "NiiSoundUnitState.h"

namespace NII
{
    //------------------------------------------------------------------------
    SoundUnitState::SoundUnitState() :
        mEffect(0),
        mDirect(true),
        mDirectFactor(1.0f),
        mDirectFilter(false),
        mDirectFilterPass(SoundUnitState::FP_low),
        mEffectFactor(1.0f),
        mEffectFilter(false),
        mEffectFilterPass(SoundUnitState::FP_low),
        mDirectTran(1.0f),
        mEffectTran(1.0f)
    {
        
    }
    //------------------------------------------------------------------------
    SoundUnitState::~SoundUnitState(){}
    //------------------------------------------------------------------------
}