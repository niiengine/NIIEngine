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
    /** �����豸�����¼�
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SerialItemArgs : public EventArgs
    {
    public:
        virtual ~SerialItemArgs() {}

        /** ��ɱȽ�
        @remark ���������д�������
        */
        virtual bool operator == (const SerialItemArgs * o) const = 0;

        /** ����ɱȽ�
        @remark ���������д�������
        */
        virtual bool operator != (const SerialItemArgs * o) const = 0;
    };
    
    /** ���е�Ԫ
    @version NIIEngine 3.0.0
    */
    class SerialItem : public ControlAlloc
    {
    public:
        typedef vector<const SerialItemArgs *>::type ItemSerial;
    public:
        SerialItem(NII_COMMAND::Functor * func);
        ~SerialItem() {}

        /** ���һ�����е�Ԫ
        @remark ������󲻸������һ���ڴ��ͷ�
        @param[in] item Ҫ������е����е�Ԫ
        */
        void add(const SerialItemArgs * item);

        /**  ���һ�����е�Ԫ
        @remark 
            ������󲻸�����������ڴ��ͷ�,�������һ��λ�����Ѿ���Ԫ��,���Ḳ��,����
            Ȼ�ǲ���ԭ����ʹ�õ��ڴ��ͷ�
        @param[in] pos Ҫ������е�λ��,������0��ʼ
        @param[in] item Ҫ������е����е�Ԫ
        */
        void add(NCount pos, const SerialItemArgs * item);

        /** ��ȥ���һ�����е�Ԫ
        @remark ������󲻸�����ȥ������ڴ��ͷ�
        @param[in] pos ��Ҫ��ȥ�����е�Ԫ��λ��
        */
        void remove(NCount pos);

        /** ������ṹ
        @remark ʹ����add��remove����������������,��������ȷ������
        */
        void compile();

        /** ���
        @remark
        @param[in] arg ����
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