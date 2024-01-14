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
        void write(const WString & in);

        /** 写入一个字符串
        @param[in] in 输入数据
        @version NIIEngine 3.0.0
        */
        void write(const VString & in);

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
        NCount read(WString & out) const;

        /** 读取一个指定长度的字符串
        @param[in] 一个字符串
        @version NIIEngine 3.0.0
        */
        NCount read(VString & out) const;

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
        NCount getSize(const WString & str);

        /** 获取字符串序列化中的大小
        @version NIIEngine 3.0.0
        */
        NCount getSize(const VString & str);

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

    //------------------------------------------------------------------------
    inline void Serializer::write(Ni8 in)
    {
        write(&in, sizeof(Ni8), 1, false);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(const Ni8 * in, NCount count)
    {
        write(in, sizeof(Ni8), count, false);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(Nui8 in)
    {
        write(&in, sizeof(Nui8), 1, false);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(const Nui8 * in, NCount count)
    {
        write(in, sizeof(Nui8), count, false);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(Ni16 in)
    {
        write(&in, sizeof(Ni16), 1);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(const Ni16 * in, NCount count)
    {
        write(in, sizeof(Ni16), count);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(Nui16 in)
    {
        write(&in, sizeof(Nui16), 1);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(const Nui16 * in, NCount count)
    {
        write(in, sizeof(Nui16), count);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(Ni32 in)
    {
        write(&in, sizeof(Ni32), 1);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(const Ni32 * in, NCount count)
    {
        write(in, sizeof(Ni32), count);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(Nui32 in)
    {
        write(&in, sizeof(Nui32), 1);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(const Nui32 * in, NCount count)
    {
        write(in, sizeof(Nui32), count);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(NIIf in)
    {
        write(&in, sizeof(NIIf), 1);
    }
    //------------------------------------------------------------------------
    inline void Serializer::write(const NIIf * in, NCount count)
    {
        write(in, sizeof(NIIf), count);
    }
    //-----------------------------------------------------------------------
    inline void Serializer::write(const Ray & in)
    {
        write(&in.mOrigin);
        write(&in.mDirection);
    }
    //-----------------------------------------------------------------------
    inline void Serializer::write(const Sphere & in)
    {
        write(&in.mCenter);
        write(&in.mRadius);
    }
    //-----------------------------------------------------------------------
    inline void Serializer::write(const Quaternion & in)
    {
        write(&in.w, sizeof(NIIf), 4);
    }
    //-----------------------------------------------------------------------
    inline void Serializer::write(const Quaternion * in, NCount count)
    {
        write(&in->w, sizeof(NIIf), 4 * count);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline void Serializer::write(const Vector2<t> & in)
    {
        write(&in.x, sizeof(t), 2);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline void Serializer::write(const Vector2<t> * in, NCount count)
    {
        write(&in->x, sizeof(t), 2 * count);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline void Serializer::write(const Vector3<t> & in)
    {
        write(&in.x, sizeof(t), 3);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline void Serializer::write(const Vector3<t> * in, NCount count)
    {
        write(&in->x, sizeof(t), 3 * count);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline void Serializer::write(const Vector4<t> & in)
    {
        write(&in.x, sizeof(t), 4);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline void Serializer::write(const Vector4<t> * in, NCount count)
    {
        write(&in->x, sizeof(t), 4 * count);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline void Serializer::write(const Matrix3<t> & in)
    {
        write(in._m, sizeof(t), 9);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline void Serializer::write(const Matrix3<t> * in, NCount count)
    {
        write(in->_m, sizeof(t), 9 * count);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline void Serializer::write(const Matrix4<t> & in)
    {
        write(in._m, sizeof(t), 16);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline void Serializer::write(const Matrix4<t> * in, NCount count)
    {
        write(in->_m, sizeof(t), 16 * count);
    }
    //------------------------------------------------------------------------
    inline NCount Serializer::read(Ni8 * out, NCount count) const
    {
        return read(out, sizeof(Ni8), count, false);
    }
    //------------------------------------------------------------------------
    inline NCount Serializer::read(Nui8 * out, NCount count) const
    {
        return read(out, sizeof(Nui8), count, false);
    }
    //------------------------------------------------------------------------
    inline NCount Serializer::read(Ni16 * out, NCount count) const
    {
        return read(out, sizeof(Ni16), count);
    }
    //------------------------------------------------------------------------
    inline NCount Serializer::read(Nui16 * out, NCount count) const
    {
        return read(out, sizeof(Nui16), count);
    }
    //------------------------------------------------------------------------
    inline NCount Serializer::read(Ni32 * out, NCount count) const
    {
        return read(out, sizeof(Ni32), count);
    }
    //------------------------------------------------------------------------
    inline NCount Serializer::read(Nui32 * out, NCount count) const
    {
        return read(out, sizeof(Nui32), count);
    }
    //------------------------------------------------------------------------
    inline NCount Serializer::read(NIIf * out, NCount count) const
    {
        return read(out, sizeof(NIIf), count);
    }
    //-----------------------------------------------------------------------
    inline NCount Serializer::read(Ray & out) const
    {
        return read(&out.mOrigin) + read(&out.mDirection);
    }
    //-----------------------------------------------------------------------
    inline NCount Serializer::read(Sphere & out) const
    {
        return read(&out.mCenter) + read(&out.mRadius);
    }
    //-----------------------------------------------------------------------
    inline NCount Serializer::read(Quaternion * out, NCount count) const
    {
        return read(&out->w, sizeof(NIIf), 4 * count);
    }
    //-----------------------------------------------------------------------
    inline NCount Serializer::read(AABox & out) const
    {
        return read(&out.mMinimum) + read(&out.mMaximum);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline NCount Serializer::read(Vector2<t> * out, NCount count) const
    {
        return read(&out->x, sizeof(t), 2 * count);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline NCount Serializer::read(Vector3<t> * out, NCount count) const
    {
        return read(&out->x, sizeof(t), 3 * count);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline NCount Serializer::read(Vector4<t> * out, NCount count) const
    {
        return read(&out->x, sizeof(t), 4 * count);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline NCount Serializer::read(Matrix3<t> * out, NCount count) const
    {
        return read(out->_m, sizeof(t), 9 * count);
    }
    //-----------------------------------------------------------------------
    template <typename t> inline NCount Serializer::read(Matrix4<t> * out, NCount count) const
    {
        return read(out->_m, sizeof(t), 16 * count);
    }
    //-----------------------------------------------------------------------
}
#endif