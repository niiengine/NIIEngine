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

#ifndef _NII_API_H_
#define _NII_API_H_

#include "NiiPreInclude.h"

namespace NIIGame
{
    class _NIIExport NII
    {
    public:
        NII();
        ~NII();

        /// 初始化框架
        void init();

        /// NII程序进口
        void main();
    public:
        /** 主题代码
        @version NIIEngine 3.0.0
        */
        void nii();
    protected:
        WarSceneManager * mSceneManager;    ///< 战争场景
        GameObjectManager * mObjectManager; ///< 战争场景对象
        Root * mCore;                       ///< 核心部分
        Camera * mCamera;                   ///< 可视头
        ViewWindow * mWindow;               ///< 主窗体
    };
}
#endif