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

#ifndef _NII_EXT_FRAMEBUFFER_H_
#define _NII_EXT_FRAMEBUFFER_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ֡����չ����
    @version NIIEngine 3.0.0 �߼�api
    */
    class _EngineAPI ExtFrameBuffer : public BufferAlloc
    {
    public:
        ExtFrameBuffer(GroupID gid, NCount w, NCount h, NCount bit, Nui32 fsaa, 
            const String & hint, bool manual);

        virtual ~ExtFrameBuffer();

        /** ��ȡ���
        @version NIIEngine 3.0.0
        */
        inline NCount getWidth() const { return mWidth; }

        /** ��ȡ���
        @version NIIEngine 3.0.0
        */
        inline NCount getHeight() const { return mHeight; }

        /** ��ȡ���
        @version NIIEngine 3.0.0
        */
        virtual NCount getBitDepth() const { return mBitDepth; }

        /** ��ȡ����������
        @version NIIEngine 3.0.0
        */
        inline NCount getFsaa() const { return mFsaa; }

        /** ��ȡ������������ʾ
        @version NIIEngine 3.0.0
        */
        inline const String & getFsaaHint() const { return mFsaaHint; }

        /** �Ƿ�������֡�������
        @version NIIEngine 3.0.0
        */
        virtual bool isSuitable(FrameObj * fo) const;

        /** ���ӵ�֡���������
        @version NIIEngine 3.0.0
        */
        virtual void attach(FrameObj * fo);

        /** ��֡��������н��
        @version NIIEngine 3.0.0
        */
        virtual void detach(FrameObj * fo);
        
        /** ������
        @version NIIEngine 3.0.0 �߼�api
        */
        void setGroup(GroupID gid);

        /** ��ȡ��
        @version NIIEngine 3.0.0 �߼�api
        */
        inline GroupID getGroup() const { return mPoolId; }

        /** ��������
        @version NIIEngine 3.0.0 �߼�api
        */
        inline void setManual(bool b) { mManual = b; }
        
        /** ��������
        @version NIIEngine 3.0.0 �߼�api
        */
        inline bool isManual() const { return mManual; }
    protected:
        typedef set<FrameObj *>::type UserList;
    protected:
        GroupID mPoolId;
        NCount mWidth;
        NCount mHeight;
        NCount mBitDepth;
        NCount mFsaa;
        String mFsaaHint;
        UserList mUserList;
        bool mManual;
    };
    typedef vector<ExtFrameBuffer *>::type ExtFrameBufferList;
}
#endif
