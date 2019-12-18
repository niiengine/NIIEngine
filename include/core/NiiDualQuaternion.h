/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_DUALQuaternion_H_
#define _NII_DUALQuaternion_H_

#include "NiiPreInclude.h"
#include "NiiMath.h"

namespace NII
{
	/** 双四元数
    @note 用于骨骼蒙皮
    @version NIIEngine 3.0.0
	*/
	class _EngineAPI DualQuaternion
	{
	public:
        DualQuaternion();
        DualQuaternion(NIIf * data);
        DualQuaternion(NIIf w, NIIf x, NIIf y, NIIf z, NIIf dw, NIIf dx, NIIf dy, NIIf dz);
        DualQuaternion(const Quaternion & rot, const Vector3f & oft);
        DualQuaternion(const Matrix4f & rotspace);

        /** 获取值
        @version NIIEngine 3.0.0
        */
		inline NIIf operator [](NCount i) const
		{
			return *(&w+i);
		}

        /** 获取值
        @version NIIEngine 3.0.0
        */
		inline NIIf & operator [](NCount i)
		{
			return *(&w+i);
		}

        /** 赋值
        @version NIIEngine 3.0.0
        */
        DualQuaternion & operator= (const DualQuaternion & o);

        /** 是否相等
        @version NIIEngine 3.0.0
        */
        bool operator== (const DualQuaternion & o) const;

        /** 是否不等
        @version NIIEngine 3.0.0
        */
        bool operator!= (const DualQuaternion & o) const;

        /** 交换值
        @version NIIEngine 3.0.0
        */
        void swap(DualQuaternion & o);

		/** 从四元组和平移中构建
        @version NIIEngine 3.0.0
        */
		void from(const Quaternion & q, const Vector3f & oft);

		/** 解析出四元组和平移
        @version NIIEngine 3.0.0
        */
		void to(Quaternion & q, Vector3f & oft) const;

		/** 通过空间矩阵构建
        @version NIIEngine 3.0.0
        */
		void from(const Matrix4f & rotspace);

		/** 解析出空间矩阵
        @version NIIEngine 3.0.0
        */
		void to(Matrix4f & rotspace) const;

		/** 是否有效
        @version NIIEngine 3.0.0
        */
        bool isNaN() const;
	public:
		NIIf w, x, y, z, dw, dx, dy, dz;
	};

    inline _EngineAPI std::ostream & operator << (std::ostream & o, const DualQuaternion & q)
    {
        o << "DualQuaternion(" << q.w << "," << q.x << "," << q.y << "," << q.z << ","
            << q.dw << "," << q.dx << "," << q.dy << "," << q.dz << ")";
        return o;
    }
}
#endif
