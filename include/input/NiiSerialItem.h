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