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

#include "NiiGameActionScene.h"

namespace NIIGame
{
    //------------------------------------------------------------------------
    GameObj * ActionScene::createGameObject()
    {
        GameObj * object = 
        object->onCreate(this);
    }
    //------------------------------------------------------------------------
    void ActionScene::destroy(GameObj * dest)
    {
        object->onDestroy(this);
    }
    //------------------------------------------------------------------------
    ActionScene * ActionScene::operator +(const GameObj * src)
    {
        src->onDetach(this)
    }
    //------------------------------------------------------------------------
    ActionScene * ActionScene::operator -(const GameObj * src)
    {
        src->onAttach(this);
    }
    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
}