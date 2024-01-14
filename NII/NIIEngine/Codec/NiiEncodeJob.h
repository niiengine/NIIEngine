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

#ifndef _NII_ENCODE_JOB_H_
#define _NII_ENCODE_JOB_H_

#include "NiiPreInclude.h"
#include "NiiCodecJob.h"

namespace NII
{
    /** 解码工作
    @remark 
        高级接口,继承时必须和相应的对象和流结合在一起,否则会内存泄露,或非法指针
    @version NIIEngine 3.0.0
    */
    class _EngineAPI EncodeJob : public CodecJob
    {
        friend class CodecJob;
    public:
        /** 从内存中加码数据到目标位置
        @param[in] in 还没加码的数据
        @param[in] type 目标类型
        @param[in] any 目标位置
        */
        EncodeJob(NCount total, DataType type, const Any & any);
        virtual ~EncodeJob();
        
        /// 获取数据源
        MemStream * getIn();
    protected:
        EncodeJob();

        /// @copydetails CodecJob::onRequest
        void onRequest(NCount count);
    protected:
        MemStream * mIn;  ///< 输入流
    };
}
#endif