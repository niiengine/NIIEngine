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

#ifndef _NII_SERIALIZER_MANAGER_H_
#define _NII_SERIALIZER_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSerializerFactoryObj.h"

namespace NII
{
    /** �������н�����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SerializerManager : public Singleton<SerializerManager>,
        public SerializerAlloc
    {
    public:
        SerializerManager();
        ~SerializerManager();

        /** ����ļ����н�����
        @remark ������ͷŵ�ʱ�򲻻�ɾ��ʵ��������,�����ǰ�ɾ����¼
        @param[in] serial
        @version NIIEngine 3.0.0
        */
        void add(SerializerFactoryObj * serial);

        /** ��ȥ�ļ����н�����
        @remark ������ͷŵ�ʱ�򲻻�ɾ��ʵ��������,�����ǰ�ɾ����¼
        @param[in] serial
        @version NIIEngine 3.0.0
        */
        void remove(SerializerFactoryObj * serial);

        /** ��ȡ�ļ����н�����
        @param[in] id �ڲ���ı�ʶID,�������е��඼�б�ʶID��
        @version NIIEngine 3.0.0
        */
        SerializerFactoryObj * getFactory(Nui32 id);

        /** ��ȥ���е��ļ����н�����
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /// @copydetails Singleton::getOnly
        static SerializerManager & getOnly();
        
        /// @copydetails Singleton::getOnlyPtr
        static SerializerManager * getOnlyPtr();
    private:
        typedef map<FactoryID, SerializerFactoryObj *>::type SerializerList;
        SerializerList mSerializers;
    };
}

#endif