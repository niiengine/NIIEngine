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

#ifndef _NII_LINUX_CONTROLPATTERN_MANAGER_H_
#define _NII_LINUX_CONTROLPATTERN_MANAGER_H_

#include "NiiLinuxControlPatternPrereq.h"
#include "NiiInputManager.h"

#define test_bit(array, bit)  (array[(bit)/8] & (1 << ((bit)%8)))

namespace NII
{
namespace NII_MEDIA
{
    /** ����һ��ר��ΪNII�����װ��Linux������
    @version NIIEngine 3.0.0
    */
    class LinuxControlPatternManager : public InputManager
    {
    public:
        LinuxControlPatternManager();
        ~LinuxControlPatternManager();

        /// @copydetails InputManager::init
        void init();

        /// @copydetails InputManager::retrieve
        void retrieve(const ViewWindow * main, ControlPattern * dest);

        /// @copydetails InputManager::getCount
        NCount getCount(ControlDevType type) const;
    protected:
        /// @copydetails InputManager::createImpl
        ControlPattern * createImpl(const ViewWindow * main, ControlDevType type);
    private:
        /// ö���豸
        void enumDev();
    private:
        ControlList mDev;                   ///< ����ʵ���е��豸
        WindowControlList mIdleDev;         ///< ���嵽�����豸��ӳ��
        Nid mMark;                          ///< �ڲ�����
    };
}
}
#endif