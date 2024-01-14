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
    /** 这是一个专门为NII引擎包装的OSX系统管理器
    @author 冇得弹
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
        /// 枚举设备
        void enumDev();
        
        /** 枚举设备
        @remark 枚举同种设备
        @param[in] iterator 同类设备链
        */
        void enumDev(io_iterator_t iterator);
        
        /** 枚举设备属性
        @remark 只输出部分供应商信息
        @param[out] re 设备结构输出
        @param[in] propertyMap 设备属性
        */
        void enumDevProperties(ControlPattern * re, CFMutableDictionaryRef propertyMap);
        
        /// 分析设备属性
        void analysisProperties(CFDictionaryRef properties);
        
        /// 分析设备属性组
        void analysisPropertiesGroup(CFDictionaryRef properties);

        /// 寻找设备
        io_iterator_t lookUp(int usage, int page);
    private:
        ControlList mDev;                   ///< 程序实例中的设备
        WindowControlList mIdleDev;         ///< 窗体到空闲设备的映射
        Nid mMark;                          ///< 内部掩码
    };
}
#endif