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

#ifndef _NII_APU_PROGRAM_H_
#define _NII_APU_PROGRAM_H_

#include "NiiPreInclude.h"
#include "NiiResource.h"
#include "NiiApuBuffer.h"
#include "NiiApuProgramParam.h"

namespace NII
{
    /** 执行监听器
    @version NIIEngine 4.0.0
    */
    class _EngineAPI ApuProgramListener
    {
    public:
        virtual ~ApuProgramListener();

        /** 当处理开始时触发
        @version NIIEngine 4.0.0
        */
        virtual void onProcessBengin() {}

        /** 当处理完成时触发
        @version NIIEngine 4.0.0
        */
        virtual void onProcessInterrupt() {}

        /** 当处理完成时触发
        @version NIIEngine 4.0.0
        */
        virtual void onProcessEnd() {}
    };

    /** 加速程序
    @version NIIEngine 4.0.0
    */
    class _EngineAPI ApuProgram : public Resource
    {
    public:
        /** 构造函数
        @version NIIEngine 4.0.0 高级api
        */
        ApuProgram(ResourceID rid, GroupID gid,
            ResLoadScheme * ls = 0, ResResultScheme * rs = 0, 
            ScriptTypeID stid = N_CmdObj_ApuProgram, LangID lid = N_PrimaryLang);

        virtual ~ApuProgram();

        /** 是否有效
        @remark 一般着色程序都涉及渲染系统和硬件支持
        @version NIIEngine 4.0.0
        */
        virtual bool isValid() const;

        /** 执行程序
        @version NIIEngine 4.0.0
        */
        bool run();

        /** 获取错误代码
        @version NIIEngine 4.0.0
        */
        inline Nui32 getErrorCode() const               { return mErrorCode; }

        /** 设置语法类型
        @version NIIEngine 4.0.0
        */
        inline void setSyntaxType(ApuLanguage sl)       { mSyntax = sl; }

        /** 获取语法类型
        @version NIIEngine 4.0.0
        */
        inline ApuLanguage getSyntaxType() const        { return mSyntax; }

        /** 为这个着色程序,设置资源汇编的来源文件
        @remark 设置这个没有效果直到(重新)加载这个程序
        @version NIIEngine 4.0.0
        */
        void setProgramSrc(const String & file);

        /** 设置代码串
        @version NIIEngine 4.0.0
        */
        void setProgramStr(const VString & code);

        /** 获取代码串
        @version NIIEngine 4.0.0
        */
        inline const VString & getProgramStr() const    { return mSource; }
        
        /** 获取这个加速程序使用的资源文件名
        @version NIIEngine 4.0.0
        */
        inline const String & getProgramSrc() const     { return mFile; }

        /** 程序入口
        @version NIIEngine 4.0.0
        */
        inline void setKernel(const VString & kernel)   { mKernel = kernel; }

        /** 程序入口
        @version NIIEngine 4.0.0
        */
        inline const VString & getKernel() const        { return mKernel; }
    protected:
        /// @copydoc Resource::loadImpl
        void loadImpl();

        /// @copydoc Resource::calcSize
        NCount calcSize() const;

        /// @copydetails PropertyCmdObj::init
        bool initCmd(PropertyCmdSet * dest);

        /** 加载代码
        @version NIIEngine 4.0.0
        */
        virtual void loadCodeImpl(const VString & code) = 0;

        /** 系统是否支持
        @version NIIEngine 4.0.0
        */
        bool isSysSupport() const;
    protected:
        ApuProgramID mID;
        ApuLanguage mSyntax;
        String mFile;
        VString mKernel;
        VString mSource;
        Nui32 mErrorCode;
        ApuProgramParam * mParams;
        bool mParamValid;
        bool mProgramSrcValid;
    };
}
#endif