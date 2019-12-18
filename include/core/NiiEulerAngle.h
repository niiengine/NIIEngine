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

#include "NiiPreInclude.h"

namespace NII
{
    /* 正交矩阵,yaw * pitch * roll. yaw饶向上量旋转.pitch饶右轴旋转,roll饶方向轴旋转.
	@version NIIEngine 3.0.0
    */
	extern _EngineAPI bool eulerXYZ(const Matrix3f & src, Radian & Y, Radian & P, Radian & R);

	extern _EngineAPI bool eulerXZY(const Matrix3f & src, Radian & Y, Radian & P, Radian & R);

	extern _EngineAPI bool eulerYXZ(const Matrix3f & src, Radian & Y, Radian & P, Radian & R);

	extern _EngineAPI bool eulerYZX(const Matrix3f & src, Radian & Y, Radian & P, Radian & R);

	extern _EngineAPI bool eulerZXY(const Matrix3f & src, Radian & Y, Radian & P, Radian & R);

	extern _EngineAPI bool eulerZYX(const Matrix3f & src, Radian & Y, Radian & P, Radian & R);

	extern _EngineAPI void eulerXYZ(Matrix3f * dst, const Radian & Y, const Radian & P, const Radian & R);

	extern _EngineAPI void eulerXZY(Matrix3f * dst, const Radian & Y, const Radian & P, const Radian & R);

	extern _EngineAPI void eulerYXZ(Matrix3f * dst, const Radian & Y, const Radian & P, const Radian & R);

	extern _EngineAPI void eulerYZX(Matrix3f * dst, const Radian & Y, const Radian & P, const Radian & R);

	extern _EngineAPI void eulerZXY(Matrix3f * dst, const Radian & Y, const Radian & P, const Radian & R);

	extern _EngineAPI void eulerZYX(Matrix3f * dst, const Radian & Y, const Radian & P, const Radian & R);
}