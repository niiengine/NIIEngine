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

#ifndef _NII_WIN32_CONTROLPATTERN_MANAGER_H_
#define _NII_WIN32_CONTROLPATTERN_MANAGER_H_

#include "NiiWin32ControlPatternPrereq.h"
#include "NiiInputManager.h"

namespace NII
{
    /** Win32������������
    @remark win32ƽ̨ר��
    @version NIIEngine 3.0.0
    */
    class Win32ControlPatternManager : public InputManager
    {
    public:
        Win32ControlPatternManager();
        ~Win32ControlPatternManager();

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
        /// ö���豸�ص�����
        static BOOL CALLBACK enumDevCB(LPCDIDEVICEINSTANCE dev, LPVOID p);
    private:
        ControlList mDev;                   ///< ����ʵ���е��豸
        WindowControlList mIdleDev;         ///< ���嵽�����豸��ӳ��
        IDirectInput8 * mDirectInput;       ///< ����ʵ�����豸�ӿ�
        Nid mMark;
    };
}
#endif