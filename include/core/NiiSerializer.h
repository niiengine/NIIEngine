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
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_SERIALIZER_H_
#define _NII_SERIALIZER_H_

#include "NiiPreInclude.h"
#include "NiiCommon.h"
#include "NiiQuaternion.h"
#include "NiiRay.h"
#include "NiiSphere.h"

namespace NII
{
    /** 文件的字节序
    @version NIIEngine 3.0.0
	*/
	enum Endian
    {
        E_Native,   ///< 使用平台自身
        E_B,        ///< 使用大字节序 (0x1000 为 0x1000)
        E_L         ///< 使用小字节序 (0x1000 为 0x0010)
    };

    /** 序列化数据读/写二进制流文件,默认对象只能执行或读或写,不能操作一起执行
    @remark
        每读/写操作实际的读/写位置已经发生了变化,如果是不同的对象或者跨度使用同一个数
        据流必须要明确当前的读/写位置.虽然它可以通过子类继承去实现同一个数据流实例同时
        进行读写操作操作,这个类设计并不期望这么去做.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Serializer : public SerializerAlloc
    {
    public:
        Serializer(const DataStream * src, bool flip = false);

        virtual ~Serializer();
    public:
		/** 写入一个布尔型
		@remark 一般使用Nui8数据类型去描述一个bool
		@param[in] in 输入数据
		@version NIIEngine 3.0.0
		*/
		void write(bool in);

        /** 写入一个或多个布尔
        @remark 一般使用Nui8数据类型去描述一个bool
        @param[in] in 输入数据
        @param[in] cnt 数据类型个数
        @version NIIEngine 3.0.0
        */
        void write(const bool * in, NCount cnt = 1);

		/** 写入一个Ni8
		@param[in] in 输入数据
		@version NIIEngine 3.0.0
		*/
		void write(Ni8 in);

        /** 写入一个或多个Ni8
        @param[in] in 输入数据
        @param[in] cnt 数据类型个数
        @version NIIEngine 3.0.0
        */
        void write(const Ni8 * in, NCount cnt = 1);

		/** 写入一个Nui8
		@param[in] in 输入数据
		@version NIIEngine 3.0.0
		*/
		void write(Nui8 in);

        /** 写入一个或多个Nui8
        @param[in] in 输入数据
        @param[in] cnt 数据类型个数
        @version NIIEngine 3.0.0
        */
        void write(const Nui8 * in, NCount cnt = 1);

		/** 写入一个Ni16
		@param[in] in 输入数据
		@version NIIEngine 3.0.0
		*/
		void write(Ni16 in);

        /** 写入一个或多个Ni16
        @param[in] in 输入数据
        @param[in] cnt 数据类型个数
        @version NIIEngine 3.0.0
        */
        void write(const Ni16 * in, NCount cnt = 1);

		/** 写入一个Nui16
		@param[in] in 输入数据
		@version NIIEngine 3.0.0
		*/
		void write(Nui16 in);

        /** 写入一个或多个Nui16
        @param[in] in 输入数据
        @param[in] cnt 数据类型个数
        @version NIIEngine 3.0.0
        */
        void write(const Nui16 * in, NCount cnt = 1);

		/** 写入一个Ni32
		@param[in] in 输入数据
		@version NIIEngine 3.0.0
		*/
		void write(Ni32 in);

        /** 写入一个或多个Ni32
        @param[in] in 输入数据
        @param[in] cnt 数据类型个数
        @version NIIEngine 3.0.0
        */
        void write(const Ni32 * in, NCount cnt = 1);

		/** 写入一个Nui32
		@param[in] in 输入数据
		@version NIIEngine 3.0.0
		*/
		void write(Nui32 in);

        /** 写入一个或多个Nui32
        @param[in] in 输入数据
        @param[in] cnt 数据类型个数
        @version NIIEngine 3.0.0
        */
        void write(const Nui32 * in, NCount cnt = 1);

		/** 写入一个NIIf
		@param[in] in 输入数据
		@version NIIEngine 3.0.0
		*/
		void write(NIIf in);

        /** 写入一个或多个NIIf
        @param[in] in 输入数据
        @param[in] cnt 数据类型个数
        @version NIIEngine 3.0.0
        */
        void write(const NIIf * in, NCount cnt = 1);

		/** 写入一个NIId
		@param[in] in 输入数据
		@version NIIEngine 3.0.0
		*/
		void write(NIId in);

        /** 写入一个或多个NIId
        @param[in] in 输入数据
        @param[in] cnt 数据类型个数
        @version NIIEngine 3.0.0
        */
        void write(const NIId * in, NCount cnt = 1);

        /** 写入一个字符串
        @param[in] in 输入数据
        @version NIIEngine 3.0.0
        */
        void write(const String & in);

        /** 写入一个射线
        @param[in] in 输入数据
        @version NIIEngine 3.0.0
        */
        void write(const Ray & in);

        /** 写入一个球
        @param[in] in 输入数据
        @version NIIEngine 3.0.0
        */
        void write(const Sphere & in);

        /** 写入一个方向角
        @param[in] in 输入数据
        @version NIIEngine 3.0.0
        */
        void write(const Quaternion & in);

		/** 写入一个方向角
		@param[in] in 输入数据
		@param[in] cnt 数据类型个数
		@version NIIEngine 3.0.0
		*/
		void write(const Quaternion * in, NCount cnt = 1);

		/** 写入一个任意类型的Vector2
		@param[in] in 输入数据
		@version NIIEngine 3.0.0
		*/
		template <typename t> void write(const Vector2<t> & in);

        /** 写入一个或多个任意类型的Vector2
        @param[in] in 输入数据
        @param[in] cnt 写入的个数,这个与数组的大小必须匹配
        @version NIIEngine 3.0.0
        */
        template <typename t> void write(const Vector2<t> * in, NCount cnt = 1);

		/** 写入一个任意类型的Vector3
		@param[in] in 输入数据
		@version NIIEngine 3.0.0
		*/
		template <typename t> void write(const Vector3<t> & in);

        /** 写入一个或多个任意类型的Vector3
        @param[in] in 输入数据
        @param[in] cnt 写入的个数,这个与数组的大小必须匹配
        @version NIIEngine 3.0.0
        */
        template <typename t> void write(const Vector3<t> * in, NCount cnt = 1);

		/** 写入一个任意类型的Vector4
		@param[in] in 输入数据
		@version NIIEngine 3.0.0
		*/
		template <typename t> void write(const Vector4<t> & in);

        /** 写入一个或多个任意类型的Vector4
        @param[in] in 输入数据
        @param[in] cnt 写入的个数,这个与数组的大小必须匹配
        @version NIIEngine 3.0.0
        */
        template <typename t> void write(const Vector4<t> * in, NCount cnt = 1);

		/** 写入一个任意类型的Matrix3
		@param[in] in 输入数据
		@version NIIEngine 3.0.0
		*/
		template <typename t> void write(const Matrix3<t> & in);

        /** 写入一个或多个任意类型的Matrix3
        @param[in] in 输入数据
        @param[in] cnt 写入的个数,这个与数组的大小必须匹配
        @version NIIEngine 3.0.0
        */
        template <typename t> void write(const Matrix3<t> * in, NCount cnt = 1);

		/** 写入一个任意类型的Matrix4
		@param[in] in 输入数据
		@version NIIEngine 3.0.0
		*/
		template <typename t> void write(const Matrix4<t> & in);

        /** 写入一个或多个任意类型的Matrix4
        @param[in] in 输入数据
        @param[in] cnt 写入的个数,这个与数组的大小必须匹配
        @version NIIEngine 3.0.0
        */
        template <typename t> void write(const Matrix4<t> * in, NCount cnt = 1);

        /** 写入一个指定长度的字符串
        @remark  这是写入核心,重写它可以获得更多的变化
        @param[in] in 输入数据
        @param[in] size 结构体大小
        @param[in] cnt 数据类型个数
        @param[in] flip 是否启用翻转字节序
        @version NIIEngine 3.0.0
        */
        virtual NCount write(const void * in, NCount size, NCount cnt, bool flip = true);

        /** 读取一个或多个布尔
        @param[out] out 输出数据
        @param[in] cnt 数据类型个数
        @version NIIEngine 3.0.0
        */
        NCount read(bool * out, NCount cnt = 1) const;

        /** 读取一个或多个Nui8
        @param[out] out 输出数据
        @param[in] cnt 数据类型个数
        @version NIIEngine 3.0.0
        */
        NCount read(Ni8 * out, NCount cnt = 1) const;

        /** 读取一个或多个Nui8
        @param[out] out 输出数据
        @param[in] cnt 数据类型个数
        @version NIIEngine 3.0.0
        */
        NCount read(Nui8 * out, NCount cnt = 1) const;

        /** 读取一个或多个Ni16
        @param[out] out 输出数据
        @param[in] cnt 数据类型个数
        @version NIIEngine 3.0.0
        */
        NCount read(Ni16 * out, NCount cnt = 1) const;

        /** 读取一个或多个Nui16
        @param[out] out 输出数据
        @param[in] cnt 数据类型个数
        @version NIIEngine 3.0.0
        */
        NCount read(Nui16 * out, NCount cnt = 1) const;

        /** 读取一个或多个Ni32
        @param[out] out 输出数据
        @param[in] cnt 数据类型个数
        @version NIIEngine 3.0.0
        */
        NCount read(Ni32 * out, NCount cnt = 1) const;

        /** 读取一个或多个Nui32
        @param[out] out 输出数据
        @param[in] cnt 数据类型个数
        @version NIIEngine 3.0.0
        */
        NCount read(Nui32 * out, NCount cnt = 1) const;

        /** 读取一个或多个NIIf
        @param[out] out 输出数据
        @param[in] cnt 数据类型个数
        @version NIIEngine 3.0.0
        */
        NCount read(NIIf * out, NCount cnt = 1) const;

        /** 读取一个或多个NIId
        @param[out] out 输出数据
        @param[in] cnt 数据类型个数
        @version NIIEngine 3.0.0
        */
        NCount read(NIId * out, NCount cnt = 1) const;

        /** 读取一个指定长度的字符串
        @param[in] 一个字符串
        @version NIIEngine 3.0.0
        */
        NCount read(String & out) const;

        /** 读取一个或多个射线
        @param[out] out 输出数据
        @version NIIEngine 3.0.0
        */
        NCount read(Ray & out) const;

        /** 读取一个或多个球
        @param[out] out 输出数据
        @version NIIEngine 3.0.0
        */
        NCount read(Sphere & out) const;

        /** 读取一个或多个轴对称盒
        @param[out] out 输出数据
        @version NIIEngine 3.0.0
        */
        NCount read(AABox & out) const;

		/** 读取一个或多个方向角
		@param[out] out 输出数据
		@param[in] cnt 数据类型个数
		@version NIIEngine 3.0.0
		*/
		NCount read(Quaternion * out, NCount cnt = 1) const;

        /** 读取一个或多个
        @param[out] out 输出数据
        @param[in] cnt 数据类型个数
        @version NIIEngine 3.0.0
        */
        template <typename t> NCount read(Vector2<t> * out, NCount cnt = 1) const;

        /** 读取一个或多个
        @param[out] out 输出数据
        @param[in] cnt 数据类型个数
        @version NIIEngine 3.0.0
        */
        template <typename t> NCount read(Vector3<t> * out, NCount cnt = 1) const;

        /** 读取一个或多个
        @param[out] out 输出数据
        @param[in] cnt 数据类型个数
        @version NIIEngine 3.0.0
        */
        template <typename t> NCount read(Vector4<t> * out, NCount cnt = 1) const;

        /** 读取一个或多个
        @param[out] out 输出数据
        @param[in] cnt 数据类型个数
        @version NIIEngine 3.0.0
        */
        template <typename t> NCount read(Matrix3<t> * out, NCount cnt = 1) const;

        /** 读取一个或多个
        @param[out] out 输出数据
        @param[in] cnt 数据类型个数
        */
        template <typename t> NCount read(Matrix4<t> * out, NCount cnt = 1) const;

        /** 读取一个或多个结构型
        @remakr 这是读取核心,重写它可以获得更多的变化
        @param[out] out 输出数据
        @param[in] size 结构体大小
        @param[in] cnt 结构体个数
        @param[in] flip 是否启用翻转字节序
        @version NIIEngine 3.0.0
        */
        virtual NCount read(void * out, NCount size, NCount cnt, bool flip = true) const;

        /** 获取字符串序列化中的大小
        @version NIIEngine 3.0.0
        */
        NCount getSize(const String & str);

        /** 翻转字节(仅字节型)
        @param[in] in 输入数据
        @param[out] out 输出数据
        @param[in] size 翻转字节数量
        @version NIIEngine 3.0.0
        */
        static void mirror(const Nui8 * in, Nui8 * out, NCount cnt);

        /** 翻转字节(仅字节型)
        @param data 输入和输出数据
        @param[in] size 翻转字节数量
        @version NIIEngine 3.0.0
        */
        static void mirror(Nui8 * data, NCount cnt);

        /** 翻转字节序
        @param data 输入和输出数据
        @param[in] size 翻转字节大小
        @param[in] cnt 翻转数量
        @version NIIEngine 3.0.0
        */
        static void flip(Nui8 * data, NCount size, NCount cnt);

        /** 翻转字节序
        @param[in] in 输入数据(已经分配内存)
        @param[out] out 输出数据(已经分配内存)
        @param[in] size 翻转字节大小
        @param[in] cnt 翻转数量
        @version NIIEngine 3.0.0
        */
        static void flip(const Nui8 * in, Nui8 * out, NCount size, NCount cnt);

        /** 翻转数据
        @remark 扩展,期望是小字节序时使用
        @version NIIEngine 3.0.0
        */
        static void flipL(Nui8 * data, NCount size, NCount cnt);

        /** 翻转数据
        @remark 扩展,期望是大字节序时使用
        @version NIIEngine 3.0.0
        */
        static void flipB(Nui8 * data, NCount size, NCount cnt);
    protected:
        Serializer();

        /** 确定传入的流匹配的原字节序
        @param[in] dest 期望的字节序与这个参数(流指针)使用的相同
        @param[in] oft 效验位置
        @version NIIEngine 3.0.0
        */
        virtual void check(DataStream * dest, NCount oft);

        /** 基于相对确定写入的字节序
        @param[in] dest 期望的字节序
        @version NIIEngine 3.0.0
        */
        virtual void check(Endian dest);
    protected:
        static const NIIl ChunkHeaderSize;
        static const Nui16 NorEndian;
        static const Nui16 RevEndian;
    protected:
        mutable DataStream * mStream;   ///< 写入/读取流(读/写)
        bool mFlipEndian;               ///< (读/写)
    };
#include "NiiSerializer.inl"
}
#endif