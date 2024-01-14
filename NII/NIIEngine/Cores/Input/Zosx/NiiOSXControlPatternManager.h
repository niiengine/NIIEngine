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

#ifndef _NII_OSX_CONTROLPATTERN_MANAGER_H_
#define _NII_OSX_CONTROLPATTERN_MANAGER_H_

#include "NiiOSXControlPatternPrereq.h"
#include "NiiInputManager.h"

namespace NII
{
    /** ����һ��ר��ΪNII�����װ��OSXϵͳ������
    @author �ӵõ�
    @mail zelda_ok@163.com
    */
    class OSXControlPatternManager : public InputManager
    {
    public:
        OSXControlPatternManager();
        ~OSXControlPatternManager();

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
        
        /** ö���豸
        @remark ö��ͬ���豸
        @param[in] iterator ͬ���豸��
        */
        void enumDev(io_iterator_t iterator);
        
        /** ö���豸����
        @remark ֻ������ֹ�Ӧ����Ϣ
        @param[out] re �豸�ṹ���
        @param[in] propertyMap �豸����
        */
        void enumDevProperties(ControlPattern * re, CFMutableDictionaryRef propertyMap);
        
        /// �����豸����
        void analysisProperties(CFDictionaryRef properties);
        
        /// �����豸������
        void analysisPropertiesGroup(CFDictionaryRef properties);

        /// Ѱ���豸
        io_iterator_t lookUp(int usage, int page);
    private:
        ControlList mDev;                   ///< ����ʵ���е��豸
        WindowControlList mIdleDev;         ///< ���嵽�����豸��ӳ��
        Nid mMark;                          ///< �ڲ�����
    };
}
#endif