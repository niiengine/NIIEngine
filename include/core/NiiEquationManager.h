/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-27

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
		void update();

        /** 创建等式
		@version NIIEngine 3.0.0
		*/
		template <typename in, typename out> DataEquation<in, out> * create(const DataValue<in> * src,
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

        /// @copydetails Singleton::getOnly
        static EquationManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static EquationManager * getOnlyPtr();
    protected:
        typedef set<DataEquationBase *>::type EquationList;
        EquationList mEquationList;
    };
}
#endif