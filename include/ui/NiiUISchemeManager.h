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
#ifndef _NII_UI_SchemeManager_H_
#define _NII_UI_SchemeManager_H_

#include "NiiUIPreInclude.h"
#include "NiiSingleton.h"
#include "NiiUIScheme.h"

using namespace NII::UI;

namespace NII
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UISchemeManager : public Singleton<UISchemeManager>, public UIAlloc
    {
    public:
        typedef map<Nid, Scheme *>::type ObjectList;
    public:
        UISchemeManager();
        ~UISchemeManager();

        /**
        @version NIIEngine 3.0.0
        */
        inline void setAutoLoad(bool set)       { mAutoLoad = set; }

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isAutoLoad() const       { return mAutoLoad; }
        
        /**
        @version NIIEngine 3.0.0
        */
        void createAllDefine(const String & pattern, GroupID gid);

        /**
        @version NIIEngine 3.0.0
        */
        Scheme * create(const String & file, GroupID gid);

        /**
        @version NIIEngine 3.0.0
        */
        Scheme * get(SchemeID id) const;

        /**
        @version NIIEngine 3.0.0
        */
        bool isExist(Nid id) const;

        /**
        @version NIIEngine 3.0.0
        */
        void destroy(Nid id);

        /**
        @version NIIEngine 3.0.0
        */
        void destroyAll();

        /**
        @version NIIEngine 3.0.0 ¸ß¼¶api
        */
        const ObjectList & getList() const;

        /**
        @version NIIEngine 3.0.0
        */
        static void setGroup(GroupID gid);

        /**
        @version NIIEngine 3.0.0
        */
        static GroupID getGroup();
    protected:
        ObjectList mObjectList;
        bool mAutoLoad;
        static GroupID ResourceGroup;
    };
}
#endif