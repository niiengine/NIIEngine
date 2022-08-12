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

#ifndef _NII_CONTROL_PATTERN_H_
#define _NII_CONTROL_PATTERN_H_

#include "NiiPreInclude.h"
#include "NiiPattern.h"
#include "NiiSerialItem.h"

namespace NII
{
    /** ����������
    @version NIIEngine 3.0.0
    */
        enum ControlDevType
        {
            CDT_Keyboard = 1,
            CDT_Mouse = 2,
            CDT_JoyPad = 3,
            CDT_JoyHandle = 4,
            CDT_JoyWheel = 5,
            CDT_Touch = 6
        };
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
        friend class InputManager;
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

    /** ������
    @remark ���������������Ŀ��Ƶ�Ԫʵ���鷽��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ControlPattern : public Pattern
    {
        friend class InputManager;
    public:
        typedef vector<ControlItem *>::type ItemList;
    public:
        /// ���������豸��������
        virtual ControlDevType getType() const = 0;
    protected:
        ControlPattern();

        /** ʹ��ControlPatternManger,����ֱ��ʹ���������
        @param[in] id ͬ�����豸�б�����Ψһ��
        @param[in] own �������ڵĶ���
        @param[in] type �����豸��������
        */
        ControlPattern(Nid id, const ViewWindow * own);

        virtual ~ControlPattern();

        /** �����豸�Ĺ�Ӧ��
        @param[in] v ��Ӧ�̵�����
        @version NIIEngine 3.0.0
        */
        void setVendor(const String & v);

        /** �󶨿��ƴ���
        @remark �ڲ�����
        @param[in] win ��Ҫ�󶨵Ŀ��ƴ���
        @version NIIEngine 3.0.0
        */
        virtual void bind(const ViewWindow * win);

        /** Ӧ�ÿ��Ƶ�Ԫ
        @remark �ڲ�����
        @param[in] item ��ҪӦ�õĿ��Ƶ�Ԫ
        @version NIIEngine 3.0.0
        */
        virtual void apply(ControlItem * item) = 0;

        /** �����ǰ���Ƶ�Ԫ�İ�
        @remark �ڲ�����
        @version NIIEngine 3.0.0
        */
        virtual void detach() = 0;

        /** ����������
        @remark ���̳߳��е�
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void _reset() = 0;

        /** ��ʼ���п�����
        @remark ���̳߳��е�
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void _start() = 0;

        /** ��ͣ���п�����
        @remark ���̳߳��е�
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void _pause() = 0;

        /** ֹͣ���п�����
        @remark ���̳߳��е�
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void _stop() = 0;
    protected:
        const ViewWindow * mOwn;///< �������ڵĶ���
        Nid mID;                ///< �ж��ͬ���͵������豸��ʱ������
        String mVendor;         ///< ��Ӧ��(�豸������)��Ϣ
        bool mActive;           ///< �豸���Ƿ�
    };
    
    class _EngineAPI QueueControlPattern : public ControlPattern
    {
    protected:
        QueueControlPattern();
        ~QueueControlPattern();
    };
}
}
#endif