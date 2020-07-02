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

#ifndef _NII_CONTROLITEM_H_
#define _NII_CONTROLITEM_H_

#include "NiiPreInclude.h"
#include "NiiControlDevType.h"
#include "NiiSerialItem.h"

namespace NII
{
namespace NII_MEDIA
{
    /** ���в���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ControlSerialArgs : public EventArgs
    {
    public:
        ControlSerialArgs(Nid id);
        virtual ~ControlSerialArgs();

        Nid mSerialID;      ///< ��ǰ���������ID
    };
    
    /** �����豸�Ŀ��Ƶ�Ԫ��
    @remark ������л�Ӱ����������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ControlItem
    {
        friend class ControlItemManager;
        friend class ControlPatternManager;
        friend class ControlPattern;
    public:
        /// ����״̬ӳ��
        typedef map<Nui32, SerialItem *>::type SerialStatusMap;
    public:
        ControlItem(bool includeRelease = false);

        virtual ~ControlItem();

        /** �����Ƿ��������м���
        @param[in] b һ������
        @version NIIEngine 3.0.0
        */
        inline void setSerial(bool b) { mSerial = b; }

        /** �Ƿ��������м���
        @return һ������
        @version NIIEngine 3.0.0
        */
        inline bool getSerial() const { return mSerial; }

        /** ���ÿ�����
        @version NIIEngine 3.0.0
        */
        inline void setInfluence(ControlPattern * cp) { mInfluence = cp; }
        
        /** ��ȡ������
        @version NIIEngine 3.0.0
        */
        inline ControlPattern * getInfluence() const { return mInfluence; }

        /** ���һ����������
        @remark
            �����еİ����Ƿְ��º��ͷ�,�������ڷ�����������е�ʱ��Ƚϸ���,����
            �����ǲ�Ӧ�úͲ��ܰ����ͷŰ��������е�Ԫ
        @param[in] id ��������ID
        @param[in] item ����
        @param[in] func ִ�еĺ��Ӳ�
        */
        void add(Nui32 id, SerialItem * item);

        /** ��ȥһ����������
        @param[in] id ��������ID
        */
        void remove(Nui32 id);

        /// ���ص�ǰ�Ŀ��Ƶ�Ԫ����
        virtual ControlDevType getType() const = 0;
    protected:
        /** ��������¼
        @param[in] arg �����������¼�����
        @return ����false���ж����µĻ��洦��
        */
        void raise(const EventArgs * arg);

        /** ���������г�����ʱ�򴥷�
        @remark ������д�������ȥʵ�����벶׽
        @param[in] arg
        */
        virtual void onSerial(const ControlSerialArgs * arg);
    protected:
        Nid mID;                            ///< ������Ƶ�Ԫ���Թ���
        SerialStatusMap mSerialStatuses;    ///< ����״̬
        ControlPattern * mInfluence;        ///< ��ǰӰ���������Ƶ�Ԫ�Ŀ�����
        bool mSerial;                       ///< �Ƿ���������
    };
    
    class _EngineAPI QueueControlItem : public ControlItem
    {
    public:
        QueueControlItem(bool includeRelease = false);
        ~QueueControlItem();
    protected:
        /** ��������¼
        @param[in] arg �����������¼�����
        @return ����false���ж����µĻ��洦��
        */
        void raise(const EventArgs * arg);

        /** ���������г�����ʱ�򴥷�
        @remark ������д�������ȥʵ�����벶׽
        @param[in] arg
        */
        virtual void onSerial(const ControlSerialArgs * arg);
    };
}
}
#endif