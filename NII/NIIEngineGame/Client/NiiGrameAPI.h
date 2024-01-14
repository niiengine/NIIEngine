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

        /// ��ʼ�����
        void init();

        /// NII�������
        void main();
    public:
        /** �������
        @version NIIEngine 3.0.0
        */
        void nii();
    protected:
        WarSceneManager * mSceneManager;    ///< ս������
        GameObjectManager * mObjectManager; ///< ս����������
        Root * mCore;                       ///< ���Ĳ���
        Camera * mCamera;                   ///< ����ͷ
        ViewWindow * mWindow;               ///< ������
    };
}
#endif