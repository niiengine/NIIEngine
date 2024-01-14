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

#ifndef _NII_ID_HELPER_H_
#define _NII_ID_HELPER_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 辅助脚本级别数据
    @remark
    */
    class _EngineAPI IDHelper : public IDAlloc
    {
    public:
        typedef map<String, ChildID>::type IDMaps;
    public:
        IDHelper();
        ~IDHelper();

        void add(const String & name, ChildID id, const String & help);
    protected:
        ClassID mPID;
        ClassID mID;
        IDMaps mIDMaps;
    };
};

#ifndef N_IDHelp
#define N_IDHelp IDHelper mIDHelper
#endif

#ifndef N_IDHelp_Def
#define N_IDHelp_Def(name, id, help) mIDHelper.add(name, id, help)
#endif

#endif