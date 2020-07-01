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

#ifndef _NII_EVENT_ARGS_H_
#define _NII_EVENT_ARGS_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ִ�к���,�ຯ��,��������,��Ա����ʱ��Ҫ�Ĳ�����
    @remark
        ���������������Ϊ�������,����ı�һ��������,Ϊ��Ч�ʺܴ���ռ����ĵ�����,��
        Ҫ������ʹ��const &�����γ�Ա����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI EventArgs : public EventAlloc
    {
    public:
        EventArgs();
        virtual ~EventArgs();
    };

    template <typename NT> class ValueEventArgs : public EventArgs
    {
    public:
        ValueEventArgs(const NT & value) : mValue(value) {}
        ~ValueEventArgs() {}

        const NT & mValue;
    };
}
#endif