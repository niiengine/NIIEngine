/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_DataEquation_H_
#define _NII_DataEquation_H_

#include "NiiPreInclude.h"
#include "NiiDataFunc.h"
#include "NiiDataValue.h"

namespace NII
{
	class DataEquationBase : public DataAlloc
	{
	public:
		DataEquationBase() : mValid(true) {}
		virtual ~DataEquationBase() {}
	public:
		/** 执行等式
		@remark 确保参子/函子,输入/输出相关联
		@note 这个等式输出 fn(src) = dst, 不可逆
		@version NIIEngine 3.0.0
		*/
		virtual void exec() = 0;

		/** 是否有效
		@version NIIEngine 3.0.0
		*/
		bool isEnable() const
		{
			return mValid;
		}

		/** 设置是否有效
		@version NIIEngine 3.0.0
		*/
		void setEnable(bool b)
		{
			mValid = b;
		}
	protected:
		bool mValid;
	};
    /** 函数/参子等式
    @version NIIEngine 3.0.0
    */
    template <typename in, typename out> class DataEquation : public DataEquationBase
    {
    public:
        DataEquation(const DataValue<in> * src, DataValue<out> * dst, DataFunc<in, out> * func):
            mSrc(src),
            mDst(dst),
            mFunc(func)
        {
        }

        virtual ~DataEquation()
        {
			if (mSrc != 0 && mSrc->isAutoDestroy())
			{
				N_delete mSrc;
				mSrc = 0;
			}
			if (mDst != 0 && mDst->isAutoDestroy())
			{
				N_delete mDst;
				mDst = 0;
			}
			if (mFunc != 0 && mFunc->isAutoDestroy())
			{
				N_delete mFunc;
				mFunc = 0;
			}
        }

        /// @copydetails DataEquationBase::exec
        void exec()
        {
            if(mValid)
            {
                mDst->setValue(mFunc->func(mSrc->getValue()));
            }
        }

        /** 设置输入
        @version NIIEngine 3.0.0
        */
        void setInput(const DataValue<in> * src)
        {
            mSrc = src;
        }

        /** 获取输入
        @version NIIEngine 3.0.0
        */
        const DataValue<in> * getInput() const
        {
            return mSrc;
        }

        /** 输出
        @version NIIEngine 3.0.0
        */
        void setOutput(DataValue<out> * dst)
        {
            mDst = dst;
        }

        /** 输出
        @version NIIEngine 3.0.0
        */
        const DataValue<out> * getOutput() const
        {
            return mDst;
        }

        /** 设置这个控制使用的函数对象.
        @version NIIEngine 3.0.0
        */
        void setFunc(const DataFunc<in, out> * func)
        {
            mFunc = func;
        }

        /** 返回这个控制使用的函数对象的一个指针.
        @version NIIEngine 3.0.0
        */
        const DataFunc<in, out> * getFunc() const
        {
            return mFunc;
        }
    protected:
        const DataValue<in> * mSrc;
        DataValue<out> * mDst;
        DataFunc<in, out> * mFunc;
    };
}
#endif