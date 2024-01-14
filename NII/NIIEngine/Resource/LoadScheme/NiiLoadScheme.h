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

#ifndef _NII_LOAD_SCHEME_H_
#define _NII_LOAD_SCHEME_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 加载策略
    @remark
        主要是为了协调资源和引擎关系,还有引擎和多线程的关系,普通的开启多个线程去等侯
        加载队列并非是一个很好的办法,每个资源最好也只能被一个线程操作,
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI LoadScheme : public PatternAlloc
    {
    public:
        LoadScheme(SchemeID sid);
        virtual ~LoadScheme();

        /**
        @remark
        @version NIIEngine 3.0.0
        */
        void setPendingMode(bool s);

        /**
        @remark
        @version NIIEngine 3.0.0
        */
        bool isPendingMode() const;

        /** 请求加载策略达成指定状态
        @remark
        @return 请求是否得到了响应
        @version NIIEngine 3.0.0 高级api
        */
        virtual bool request(LoadingState ls) = 0;

        /** 在开始工作的时候触发
        @remark
        @param[in] ls 触发时的状态
        @version NIIEngine 3.0.0
        */
        virtual void onStart(LoadingState ls);

        /** 在重至工作的时候触发
        @remark
        @param[in] ls 触发时的状态
        @version NIIEngine 3.0.0
        */
        virtual void onRestart(LoadingState ls);

        /** 在处理每一工作节的时候触发
        @remark
        @param[in] ls 触发时的状态
        @version NIIEngine 3.0.0
        */
        virtual void onProcess(LoadingState ls);

        /** 在请求加载时候触发
        @remark
        @param[in] ls 触发时的状态
        @version NIIEngine 3.0.0
        */
        virtual void onRequest(LoadingState ls);

        /** 在请求停止的时候触发
        @remark
        @param[in] ls 触发时的状态
        @version NIIEngine 3.0.0
        */
        virtual void onStop(LoadingState ls);

        /** 在请求停止的时候触发
        @remark
        @param[in] ls 触发时的状态
        @version NIIEngine 3.0.0
        */
        virtual void onPause(LoadingState ls);

        /** 在异常停止的时候触发
        @remark
        @param[in] ls 触发时的状态
        @version NIIEngine 3.0.0
        */
        virtual void onSuspend(LoadingState ls);

    protected:
        SchemeID mSID;
    };
}
#endif