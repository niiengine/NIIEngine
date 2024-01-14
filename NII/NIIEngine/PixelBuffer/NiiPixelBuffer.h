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
#ifndef _NII_PIXELBUFFER_H_
#define _NII_PIXELBUFFER_H_

#include "NiiPreInclude.h"
#include "NiiPlaneSize.h"
#include "NiiRect.h"

namespace NII
{
    /** ���ػ���
    @remark
        ���ظ������/Ƭ���ӵ�.���ص�ʵ����ȾЧ�����������ŷ�ʽ,ʵ����Ⱦ��С�Ȳ���
        ȷ����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelBuffer : public ResourceAlloc
    {
        friend class GeometryPixel;
        friend class PixelManager;
    public:
        PixelBuffer(PixelBufferID pxid, PixelBufferGroup * pbg, const Rectf & area = Rectf::ZERO);
        PixelBuffer(PixelBufferID pxid, Texture * obj, const Rectf & area = Rectf::ZERO, ScaleMode mode = SM_Unknow,
            const Vector2f & oft = Vector2f::ZERO, const Vector2f & pixeloft = Vector2f::ZERO,  
                const PlaneSizef & srcRes = PlaneSizef(640.0f, 480.0f));
        ~PixelBuffer();

        /** ��ȡID
        @version NIIEngine 3.0.0
        */
        inline PixelBufferID getID() const { return mID;  }

        /** ������������
        @param[in] area ������(���ؼ�)
        @version NIIEngine 3.0.0
        */
        void setArea(const Rectf & area);

        /** ��������ģʽ
        @param[in] mode ģʽ
        @version NIIEngine 3.0.0
        */
        void setMode(ScaleMode mode);

        /** ��ȡ��Դԭ�ֱ���
        @note (���ؼ�)
        @version NIIEngine 3.0.0
        */
        void setNativeRes(const PlaneSizef & area);

        /** ����������Դ
        @param[in] obj ����
        @version NIIEngine 3.0.0
        */
        void setTexture(Texture * obj);

        /** ����ʵ��ƫ��
        @return ʵ����Ⱦ����Ļ��λ��ƫ��(��Ļ��)
        @version NIIEngine 3.0.0
        */
        void setOffset(const Vector2f & oft);

        /** ������Ⱦƫ��
        @param[in] oft ����λ��ƫ��(���ؼ�)
        @version NIIEngine 3.0.0
        */
        void setPixelOffset(const Vector2f & oft);

        /** ��ȡʵ�ʳߴ�
        @return ʵ����Ⱦ�ߴ�(��Ļ��)
        @version NIIEngine 3.0.0
        */
        inline const PlaneSizef & getSize() const { return mSize;  }

        /** ��������ģʽ
        @param[in] mode ģʽ
        @version NIIEngine 3.0.0
        */
        ScaleMode getMode() const;

        /** ����������Դ
        @param[in] obj ����
        @version NIIEngine 3.0.0
        */
        Texture * getTexture() const;

        /** ��ȡȺ��
        @version NIIEngine 3.0.0
        */
        GroupID getGroupID() const;

        /** ��ȡʵ��ƫ��
        @return ʵ����Ⱦƫ��(��Ļ��)
        @version NIIEngine 3.0.0
        */
        const Vector2f & getOffset() const;

        /** ������Ⱦƫ��
        @param[in] oft ����λ��ƫ��(���ؼ�)
        @version NIIEngine 3.0.0
        */
        const Vector2f & getPixelOffset() const;

        /** ��ȡ��Դԭ�ֱ���
        @note (���ؼ�)
        @version NIIEngine 3.0.0
        */
        const PlaneSizef & getNativeRes() const;

        /** ȷ����ͼ��ʵ�ʴ�С
        @version NIIEngine 3.0.0
        */
        void notifyRatio(const Vector2f & aspect);

        /** �������ű���
        @param[in] out ���ű���
        @version NIIEngine 3.0.0
        */
        static void calcAspectRatio(ScaleMode mode, const PlaneSizei & dest, const PlaneSizef & src, Vector2f & out);
    protected:
        PixelBufferID mID;
        PixelBufferGroup * mGroup;
        Rectf mArea;
        PlaneSizef mSize;
    };

    /** ���ػ���Ⱥ��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelBufferGroup : public ResourceAlloc
    {
        friend class PixelManager;
    public:
        typedef map<PixelBufferID, PixelBuffer *>::type PBList;
    public:
        bool operator == (PixelBufferGroup & o);

        /** ������ص�Ԫ
        @version NIIEngine 3.0.0
        */
        inline void add(PixelBuffer * pb)                   { mPBList.insert(Npair(pb->getID(), pb)); }

        /** ��ȡָ�������ص�Ԫ
        @param[in] id ���ر�ʶ
        @version NIIEngine 3.0.0
        */
        inline PixelBuffer * get(PixelBufferID pid) const   { PBList::const_iterator i = mPBList.find(pid); return (i != mPBList.end()) ? i->second : 0; }

        /** �Ƿ���ڶ�������ص�Ԫ
        @param[in] pid ���ر�ʶ
        @version NIIEngine 3.0.0
        */
        inline bool isExist(PixelBufferID pid) const        { return mPBList.find(pid) != mPBList.end(); }

        /** ��ȥ���ص�Ԫ
        @param[in] pid ���ر�ʶ
        @note ��ȥ��,���಻��������ڴ�
        @version NIIEngine 3.0.0
        */
        inline PixelBuffer * remove(PixelBufferID pid);

        /** ��ȡ���ص�Ԫ����
        @version NIIEngine 3.0.0
        */
        inline NCount getCount() const                      { return mPBList.size(); }

        /** ��ȡȺ��
        @version NIIEngine 3.0.0
        */
        inline GroupID getGroupID() const                   { return mGroupID; }

        /** ����������Դ
        @param[in] obj ����
        @version NIIEngine 3.0.0
        */
        inline void setTexture(Texture * obj)               { mTexture = obj; }

        /** ��ȡ������Դ
        @param[in] obj ����
        @version NIIEngine 3.0.0
        */
        inline Texture * getTexture() const                 { return mTexture; }

        /** ��������ģʽ
        @param[in] mode ģʽ
        @version NIIEngine 3.0.0
        */
        inline void setMode(ScaleMode mode)                 { mMode = mode; }

        /** ��ȡ����ģʽ
        @param[in] mode ģʽ
        @version NIIEngine 3.0.0
        */
        inline ScaleMode getMode() const                    { return mMode; }

        /** ������Դԭ�ֱ���
        @note (���ؼ�)
        @version NIIEngine 3.0.0
        */
        inline void setNativeRes(const PlaneSizef & area)   { mNativeRes = area; }

        /** ��ȡ��Դԭ�ֱ���
        @note (���ؼ�)
        @version NIIEngine 3.0.0
        */
        inline const PlaneSizef & getNativeRes() const      { return mNativeRes; }

        /** ������Ⱦƫ��
        @param[in] oft �����������ϵ�λ��ƫ��(���ؼ�)
        @version NIIEngine 3.0.0
        */
        inline void setPixelOffset(const Vector2f & oft)    { mPixelOffset = oft; }

        /** ��ȡ��Ⱦƫ��
        @param[in] oft �����������ϵ�λ��ƫ��(���ؼ�)
        @version NIIEngine 3.0.0
        */
        inline const Vector2f & getPixelOffset() const      { return mPixelOffset; }

        /** ����ʵ��ƫ��
        @return ʵ����Ⱦ����Ļ��λ��ƫ��(��Ļ��)
        @version NIIEngine 3.0.0
        */
        inline void setOffset(const Vector2f & oft)         { mOffset = oft; }

        /** ��ȡʵ��ƫ��
        @return ʵ����Ⱦ����Ļ��λ��ƫ��(��Ļ��)
        @version NIIEngine 3.0.0
        */
        inline const Vector2f & getOffset() const           { return mOffset; }

        /** ȷ����ͼ��ʵ�ʴ�С
        @version NIIEngine 3.0.0
        */
        void notifyView(const PlaneSizei & size);

        const PBList & getList() const                      { return mPBList; }
    protected:
        PixelBufferGroup();
        PixelBufferGroup(GroupID gid);
        PixelBufferGroup(GroupID gid, Texture * obj, ScaleMode mode = SM_Unknow, const Vector2f & oft = Vector2f::ZERO,
            const Vector2f & pixeloft = Vector2f::ZERO, const PlaneSizef & srcRes = PlaneSizef(640.0f, 480.0f));
        ~PixelBufferGroup();
    protected:
        PBList mPBList;
        GroupID mGroupID;
        Texture * mTexture;
        ScaleMode mMode;
        Vector2f mPixelOffset;
        Vector2f mOffset;
        PlaneSizef mNativeRes;
    };

    inline ScaleMode PixelBuffer::getMode() const { return mGroup->getMode(); }
    inline Texture * PixelBuffer::getTexture() const { return mGroup->getTexture(); }
    inline GroupID PixelBuffer::getGroupID() const { return mGroup->getGroupID(); }
    inline const Vector2f & PixelBuffer::getOffset() const { return mGroup->getOffset(); }
    inline const Vector2f & PixelBuffer::getPixelOffset() const { return mGroup->getPixelOffset(); }
    inline const PlaneSizef & PixelBuffer::getNativeRes() const { return mGroup->getNativeRes(); }

    typedef vector<PixelBufferGroup *>::type PixelBufferGroupList;
}
#endif