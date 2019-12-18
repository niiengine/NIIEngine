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

#ifndef _NII_SERIAL_ITEM_H_
#define _NII_SERIAL_ITEM_H_

#include "NiiPreInclude.h"
#include "NiiFunctor.h"

namespace NII
{
namespace NII_MEDIA
{
    /** 输入设备连续事件
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SerialItemArgs : public EventArgs
    {
    public:
        virtual ~SerialItemArgs() {}

        /** 达成比较
        @remark 子类必须重写这个函数
        */
        virtual bool operator == (const SerialItemArgs * o) const = 0;

        /** 不达成比较
        @remark 子类必须重写这个函数
        */
        virtual bool operator != (const SerialItemArgs * o) const = 0;
    };
    
    /** 序列单元
    @version NIIEngine 3.0.0
    */
    class SerialItem : public ControlAlloc
    {
    public:
        typedef vector<const SerialItemArgs *>::type ItemSerial;
    public:
        SerialItem(NII_COMMAND::Functor * func);
        ~SerialItem() {}

        /** 添加一个序列单元
        @remark 这个对象不负责参数一的内存释放
        @param[in] item 要添加入列的序列单元
        */
        void add(const SerialItemArgs * item);

        /**  添加一个序列单元
        @remark 
            这个对象不负责参数二的内存释放,如果参数一的位置上已经有元素,将会覆盖,且依
            然是不对原对象使用的内存释放
        @param[in] pos 要添加入列的位置,标量从0开始
        @param[in] item 要添加入列的序列单元
        */
        void add(NCount pos, const SerialItemArgs * item);

        /** 移去添加一个序列单元
        @remark 这个对象不负责移去对象的内存释放
        @param[in] pos 需要移去的序列单元的位置
        */
        void remove(NCount pos);

        /** 建立起结构
        @remark 使用完add和remove函数后调用这个函数,建立起正确的序列
        */
        void compile();

        /** 检测
        @remark
        @param[in] arg 参数
        */
        bool check(const EventArgs * arg);
    private:
        ItemSerial mItemSerial;
        ItemSerial::iterator mBegin;
        ItemSerial::iterator mCurrent;
        ItemSerial::iterator mEnd;
        NII_COMMAND::Functor * mFunc;
    };
}
}
#endif