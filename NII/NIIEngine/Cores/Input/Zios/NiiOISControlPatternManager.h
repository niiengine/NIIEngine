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

#ifndef _NII_OIS_CONTROLPATTERN_MANAGER_H_
#define _NII_OIS_CONTROLPATTERN_MANAGER_H_

#include "NiiOISControlPatternPrereq.h"
#include "NiiInputManager.h"

namespace NII
{
    /** ����һ��ר��ΪNII�����װ��OIS
    @author �ӵõ�
    @mail zelda_ok@163.com
    */
    class OISControlPatternManager : public InputManager
    {
    public:
        OISControlPatternManager();
        ~OISControlPatternManager();

        /// @copydetails InputManager::set
        void set(const PropertyData * misc);

        /// @copydetails InputManager::getCount
        NCount getCount(const ViewWindow * main, ControlDevType type) const;
    protected:
        /// @copydetails InputManager::create
        ControlPattern * create(const ViewWindow * main, ControlDevType type);
    private:
    };
}
#endif