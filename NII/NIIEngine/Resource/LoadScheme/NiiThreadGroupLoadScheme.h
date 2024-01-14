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

#ifndef _NII_THREAD_GROUPLOADSCHEME_H_
#define _NII_THREAD_GROUPLOADSCHEME_H_

#include "NiiPreInclude.h"
#include "NiiGroupLoadScheme.h"
#include "NiiJob.h"
#include "NiiThread.h"

namespace NII
{
    /** 多线程加载策略
    @remark 使用多线程协调计算机硬件系统实际性能加载资源
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineInner ThreadGroupLoadScheme : public GroupLoadScheme,
        public Job, public ThreadMain
    {
    public:
        ThreadGroupLoadScheme(SchemeID sid, GroupID gid, NCount wl);
        ~ThreadGroupLoadScheme();

        /// @copydetails LoadScheme::request
        bool request(LoadingState ls);

        /// @copydetails Job::run
        bool run();

        /// @copydetails Job::pause
        void pause();

        /// @copydetails Job::stop
        void stop();

        /// @copydetails ThreadMain::main
        void main();
    protected:
        /** 添加一个需要加载的对象
        @remark
        @param[in] r 需要添加的对象
        */
        void add(Resource * r);

        /** 添加一个需要加载的对象
        @remark
        @param[in] r 需要移去的对象
        */
        void remove(Resource * r);
    };
}
#endif