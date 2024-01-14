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
#ifndef NAVIGATIONMANAGER_H
#define NAVIGATIONMANAGER_H

#include "gsgsPreInclude.h"
#include "gsgsCommon.h"
#include "ieditor.h"
#include "imanager.h"

namespace gsgs
{
    /**
     * @class NavMgr
     * @brief a manager class that remembers navigation position in a queue
     * @author Eran
     * @date 08/10/07
     */
    class _gsgsAPI NavMgr
    {
        std::vector<BrowseRecord> m_jumps;
        size_t m_cur;

    private:
        NavMgr();
        virtual ~NavMgr();

    public:
        static NavMgr* Get();

        /**
         * @brief check if a browse record is actually a valid jump location (source or target)
         */
        bool ValidLocation(const BrowseRecord& rec) const;

        /**
         * @brief add new jump record to the manager, this new record become the new top of the list
         * @param from the starting point of the jump
         * @param to the ending point of the jump
         */
        void AddJump(const BrowseRecord& from, const BrowseRecord& to);

        /**
         * @brief return the next place to visit (destination)
         * @return next browsing record
         */
        BrowseRecord GetNext();

        /**
         * @brief return the previous place we visited (source)
         * @return previous browsing record
         */
        BrowseRecord GetPrev();

        /**
         * @brief return true if manager has more next items
         * @return
         */
        bool CanNext() const;

        /**
         * @brief return true if manager has more previous items
         * @return
         */
        bool CanPrev() const;

        bool NavigateBackward(gsgs::IManager* mgr);
        bool NavigateForward(gsgs::IManager* mgr);
        void Clear();
    };
}
#endif // NAVIGATIONMANAGER_H
