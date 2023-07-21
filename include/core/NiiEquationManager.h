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

#ifndef _NII_EquationManager_H_
#define _NII_EquationManager_H_

#include "NiiPreInclude.h"
#include "NiiCommon.h"
#include "NiiSingleton.h"
#include "NiiDataEquation.h"

namespace NII
{
    /** 等式管理器类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI EquationManager : public Singleton<EquationManager>, public ControlAlloc
    {
    public:
        EquationManager();
        ~EquationManager();

        /** 更新所有等式
        @version NIIEngine 3.0.0
        */
        void update(Nmark frameMark);

        /** 创建等式
        @version NIIEngine 3.0.0
        */
        template <typename in, typename out> inline DataEquation<in, out> * create(const DataValue<in> * src,
            DataValue<out> * dest, DataFunc<in, out> * func)
        {
            DataEquation<in, out> * c = N_new DataEquation<in, out>(src, dest, func);
            mEquationList.insert(c);
            return c;
        }

        /** 删除指定等式
        @version NIIEngine 3.0.0
        */
        void destroy(DataEquationBase * base);

        /** 删除所有等式
        @version NIIEngine 3.0.0
        */
        void destroyAll();
    protected:
        typedef set<DataEquationBase *>::type EquationList;
        EquationList mEquationList;
        Nmark mLastFrame;
    };
}
#endif