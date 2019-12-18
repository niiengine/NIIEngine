/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-5-7

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
	  ��άӦ�����
		������Ϣϵͳ���(GIS)
		��Ӱ���������ع����
		������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/
#ifndef _PIXEL_BUFFER_MANAGER_H_
#define _PIXEL_BUFFER_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiPixelBuffer.h"
#include "NiiException.h"

namespace NII
{
    /** ���ػ��������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelBufferManager : public Singleton<PixelBufferManager>, public BufferAlloc
    {
    public:
        PixelBufferManager();
        ~PixelBufferManager();

        /** ���ش������ػ���
        @param[in] file �ű��ļ�����
        @version NIIEngine 3.0.0
        */
        void load(const String & file, GroupID gid = GroupUnknow);

        /** ���ش������ػ���
        @param[in] id ���ر�ʶ
        @param[in] file �ļ�����
        @param[in] rgid ��Դ���ʶ
        @version NIIEngine 3.0.0
        */
        void load(PixelBufferID id, const String & file, GroupID rgid = GroupUnknow);

        /** �������ػ���
        @param[in] id ���ر�ʶ
        @param[in] rid ��������Դ
        @version NIIEngine 3.0.0
        */
        PixelBuffer * create(ResourceID rid, PixelBufferID id);

		/** ��ȡָ�������ػ���
		@param[in] pid ���ر�ʶ
		@version NIIEngine 3.0.0
		*/
		PixelBuffer * get(PixelBufferID pid) const;

        /** ��ȡָ�������ػ���
        @param[in] rid ��������Դ
        @param[in] id ���ر�ʶ
        @version NIIEngine 3.0.0
        */
        PixelBuffer * get(ResourceID rid, PixelBufferID id) const;

		/** �Ƿ���ڶ�������ػ���
		@param[in] pid ���ر�ʶ
		@version NIIEngine 3.0.0
		*/
		bool isExist(PixelBufferID id) const;

        /** �Ƿ���ڶ�������ػ���
        @param[in] id ���ر�ʶ
        @param[in] rid ��������Դ
        @version NIIEngine 3.0.0
        */
        bool isExist(ResourceID rid, PixelBufferID id) const;

		/** ɾ��������
		@param[in] rid ��������Դ
		@param[in] deltex ɾ������
		*/
		void destroy(ResourceID rid, PixelBufferID id, bool deltex);

        /** ɾ��������
        @param[in] rid ��������Դ
        @param[in] deltex ɾ������
        */
        void destroy(ResourceID rid, bool deltex);

        /** ɾ������
        @version NIIEngine 3.0.0
        */
        void destroyAll();

        /** ��ȡ�������е���������
        @param[in] rid ��������Դ
        @version NIIEngine 3.0.0
        */
        NCount getCount(ResourceID rid) const;

        /** ��ͼ��С
        @param size �ߴ��С
        @version NIIEngine 3.0.0
        */
        void notifyView(const PlaneSizef & size);

        /** ������Դ��
        @version NIIEngine 3.0.0
        */
        static void setGroup(GroupID rgid);

        /** ��ȡ��Դ��
        @version NIIEngine 3.0.0
        */
		static GroupID getGroup();

        /// @copydetails Singleton::getOnly
        static PixelBufferManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static PixelBufferManager * getOnlyPtr();
    protected:
        /** �������
        @version NIIEngine 3.0.0
        */
        void addImpl(PixelBuffer * pb);
    private:
        typedef map<PixelBufferID, PixelBuffer *>::type PixelBuffers;
        typedef map<ResourceID, PixelBuffers>::type PixelList;
        PixelList mPixelBuffers;
		static GroupID ResourceGroup;
    };
}
#endif