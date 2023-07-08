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
#ifndef _PIXEL_MANAGER_H_
#define _PIXEL_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiPixelBuffer.h"
#include "NiiException.h"

namespace NII
{
    /** ���ػ��������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelManager : public Singleton<PixelManager>, public BufferAlloc
    {
    public:
        PixelManager();
        ~PixelManager();

        /** ���ش������ػ���
        @param[in] file �ű��ļ�����
        @version NIIEngine 3.0.0
        */
        PixelBufferGroupList create(const String & file, GroupID gid = GroupUnknow);

        /** ���ش������ػ���
        @param[in] id ���ر�ʶ
        @param[in] file �ļ�����
        @param[in] rgid ��Դ���ʶ
        @version NIIEngine 3.0.0
        */
        PixelBufferGroup * create(PixelBufferID id, const String & file, GroupID rgid = GroupUnknow);

        /** ����(���ȡ)������
        @version NIIEngine 3.0.0
        */
        PixelBufferGroup * create(Texture * obj, ScaleMode mode = SM_Unknow, const Vector2f & oft = Vector2f::ZERO,
            const Vector2f & pixeloft = Vector2f::ZERO, const PlaneSizef & srcRes = PlaneSizef(640.0f, 480.0f));

        /** ��ȡ(�򴴽�)������
        @version NIIEngine 3.0.0
        */
        PixelBufferGroup * getGroup(GroupID gid);

        /** ��ȡ(�򴴽�)������
        @version NIIEngine 3.0.0
        */
        bool isGroupExist(GroupID gid);

        /** ɾ��������
        @version NIIEngine 3.0.0
        */
        void destroyGroup(GroupID gid);

        /** �������ػ���
        @param[in] id ���ر�ʶ
        @param[in] rid ��������Դ
        @version NIIEngine 3.0.0
        */
        PixelBuffer * create(PixelBufferID id, PixelBufferGroup * pbg, const Rectf & area);

        /** ��ȡָ�������ػ���
        @param[in] pid ���ر�ʶ
        @version NIIEngine 3.0.0
        */
        PixelBuffer * get(PixelBufferComID id) const;

        /** ��ȡָ�������ػ���
        @param[in] rid ��������Դ
        @param[in] id ���ر�ʶ
        @version NIIEngine 3.0.0
        */
        PixelBuffer * get(GroupID rid, PixelBufferID pid) const;

        /** �Ƿ���ڶ�������ػ���
        @param[in] pid ���ر�ʶ
        @version NIIEngine 3.0.0
        */
        bool isExist(PixelBufferComID id) const;

        /** �Ƿ���ڶ�������ػ���
        @param[in] id ���ر�ʶ
        @param[in] rid ��������Դ
        @version NIIEngine 3.0.0
        */
        bool isExist(GroupID rid, PixelBufferID pid) const;

        /** ɾ��������
        @param[in] rid ��������Դ
        @param[in] deltex ɾ������
        */
        void destroy(GroupID rid, PixelBufferID pid, bool deltex = true);

        /** ɾ��������
        @param[in] rid ��������Դ
        @param[in] deltex ɾ������
        */
        void destroy(GroupID rid, bool deltex = true);

        /** ɾ������
        @version NIIEngine 3.0.0
        */
        void destroyAll(bool deltex = true);

        /** ��ȡ�������е���������
        @param[in] rid ��������Դ
        @version NIIEngine 3.0.0
        */
        NCount getCount(GroupID rid) const;

        /** ��ͼ��С
        @param size �ߴ��С
        @version NIIEngine 3.0.0
        */
        void notifyView(const PlaneSizei & size);

        /** ������Դ��
        @version NIIEngine 3.0.0
        */
        static inline void setGroup(GroupID rgid)   { ResourceGroup = rgid;  }

        /** ��ȡ��Դ��
        @version NIIEngine 3.0.0
        */
        static inline GroupID getGroup()            { return ResourceGroup; }
    protected:
        GroupID genValidID() const;
    private:
        typedef map<GroupID, PixelBufferGroup *>::type PixelGroupList;
        typedef multimap<ResourceID, PixelBufferGroup *>::type ResourceGroupList;
        PixelGroupList mPBGroupList;
        ResourceGroupList mPBRList;
        mutable GroupID mValidRID;
        N_mutex(mValidGIDMutex)
        static GroupID ResourceGroup;
    };
}
#endif