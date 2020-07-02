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

#ifndef _NII_UI_INSTANCE_H_
#define _NII_UI_INSTANCE_H_

#include "NiiUIPreInclude.h"
#include "NiiGeometryPixel.h"
#include "NiiCommandObj.h"
#include "NiiQuaternion.h"
#include "NiiMatrix4.h"
#include "NiiRect.h"

namespace NII
{
namespace UI
{
    /** ������Ⱦ����
    @remark ������Ⱦ����,������UIԪ�ز�������Ч��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UIInstance : public NII_COMMAND::CommandObj, public UIAlloc
    {
        friend class UISheet;
    public:
        /** ���캯��
        @param[in] rs ��Ҫ
        @param[in] own ��Ҫ
        @param[in] oft Sheet�����е�ƫ��, �������ó� Vector2f::ZERO
        @version NIIEngine 3.0.0 �߼�api
        */
        UIInstance(RenderSys * rs, UISheet * own, const Vector2f & oft);

        ~UIInstance();

        /** �����ӻ���
        @param[out] obj �����������ӻ���
        @version NIIEngine 3.0.0
        */
        void create(UIInstance *& obj);

        /** ɾ���ӻ���
        @param[in] obj Ҫɾ�����ӻ���
        @version NIIEngine 3.0.0
        */
        void destroy(UIInstance * obj);

        /** �����ӻ���
        @note �ڴ��ⲿ����
        @param[in] obj �ӻ���
        @version NIIEngine 3.0.0
        */
        void attach(UIInstance * obj);

        /** ����ӻ���
        @note �ڴ��ⲿ����
        @param[in] obj �ӻ���
        @version NIIEngine 3.0.0
        */
        void detach(UIInstance * obj);

        /** ��Ӽ��ζ���
        @param[in] gl ���β�
        @param[in] obj ���ζ���
        @version NIIEngine 3.0.0
        */
        void add(GeometryLevel gl, GeometryPixel * obj);

        /** ��ȥ���ζ���
        @param[in] gl ���β�
        @param[in] obj ���ζ���
        @version NIIEngine 3.0.0
        */
        void remove(GeometryLevel gl, GeometryPixel * obj);

        /** ��ȥ���м��ζ���
        @param[in] gl ���β�
        @version NIIEngine 3.0.0
        */
        void clear(GeometryLevel gl);

        /** ��ȥ���м��ζ���
        @version NIIEngine 3.0.0
        */
        void clear();

        /** ����ʱ�����
        @remark ʱ��������뵱ǰ״̬�й�
        @param cost �ϴ��뵱ǰ���öɹ���ʱ��
        @version NIIEngine 3.0.0
        */
        void update(TimeDurMS cost);

        /** ִ����Ⱦ����
        @version NIIEngine 3.0.0
        */
        void flush();

        /** ˢ����Ⱦ����
        @version NIIEngine 3.0.0
        */
        void refresh();

        /** �ع���Ⱦ����
        @version NIIEngine 3.0.0
        */
        void rebuild();

        /** �Ƿ�ˢ����Ⱦ����
        @version NIIEngine 3.0.0
        */
        inline bool isRefresh() const { return mRefresh;  }

        /** ���òü�����
        @param[in] area ����
        @version NIIEngine 3.0.0
        */
        void setClip(const Recti & area);

        /** ������ʾ����
        @param[in] area ����
        @version NIIEngine 3.0.0
        */
        void setArea(const Rectf & area);

        /** ��ȡ��ʾ����
        @version NIIEngine 3.0.0
        */
        inline const Rectf & getArea() const { return mArea;  }

        /** ����λ��
        @version NIIEngine 3.0.0
        */
        void setPos(const Vector2f & pos);

        /** ��ȡλ��
        @version NIIEngine 3.0.0
        */
        const Vector2f & getPos() const;

        /** ������Գߴ�
        @note ����ڽ��滭��
        @version NIIEngine 3.0.0
        */
        void setSize(const PlaneSizef & size);

        /** ��ȡ��Գߴ�
        @version NIIEngine 3.0.0
        */
        inline const PlaneSizef & getSize() const { return mSize; }

        /** ����ԭʼ�ߴ�
        @version NIIEngine 3.0.0
        */
        void setNativeSize(const PlaneSizef & size);

        /** ������תԭ��
        @version NIIEngine 3.0.0
        */
        void setOrigin(const Vector3f & org);

        /** ��ȡ��תԭ��
        @version NIIEngine 3.0.0
        */
        inline const Vector3f & getOrigin() const { return mOrigin; }

        /** ��ת�Ƕ�
        @version NIIEngine 3.0.0
        */
        void setRotation(const Quaternion & rot);

        /** ��ת�Ƕ�
        @version NIIEngine 3.0.0
        */
        inline const Quaternion & getRotation()const { return mRot;  }

        /** Ӧ����ȾЧ��
        @version NIIEngine 3.0.0
        */
        void setEffect(RenderEffect * obj) { mBuffer->setEffect(obj);  }

        /** ��ȡ��Ӧ�õ�Ч��
        @version NIIEngine 3.0.0
        */
        inline RenderEffect * getEffect() const { return mBuffer->getEffect();  }

        /** ��UI��
        @version NIIEngine 3.0.0
        */
        UISheet * getOwner() const;

        /** ������
        @note ��������ڸ�����ϵ,������ NULL ����
        @version NIIEngine 3.0.0
        */
        UIInstance * getRel() const;

        /** ����Ŀ��
        @version NIIEngine 3.0.0
        */
        Texture * getTexture() const;

        /** ����λ��
        @param[in] in ԭλ��
        @param[out] out ʵ��λ��
        @version NIIEngine 3.0.0
        */
        void affine(const Vector2f & in, Vector2f & out);
    public:
        /** �����ӻ���ʱ����
        @version NIIEngine 3.0.0
        */
        static const EventID AttachEvent;

        /** �Ƴ��ӻ���ʱ����
        @version NIIEngine 3.0.0
        */
        static const EventID DetachEvent;

        /** ��Ⱦ����ǰ����
        @version NIIEngine 3.0.0
        */
        static const EventID PreRenderEvent;

        /** ��Ⱦ���к󴥷�
        @version NIIEngine 3.0.0
        */
        static const EventID EndRenderEvent;

        static const EventID EventCount;
    protected:
        /** �����¼�
        @version NIIEngine 3.0.0
        */
        void onAttach(const EventArgs * args);

        /** �Ƴ��¼�
        @version NIIEngine 3.0.0
        */
        void onDetach(const EventArgs * args);

        /** ���¾���
        @version NIIEngine �ڲ�api
        */
        void updateMatrix();

        /** �����ӿ�
        @version NIIEngine �ڲ�api
        */
        void updateViewport();

        /** ����ռ�
        @param[in] m ʵ�ʿռ�λ��
        @param[in] in ԭλ��
        @param[out] out ʵ��λ��
        @version NIIEngine �ڲ�api
        */
        void affine(const Matrix4f * m, const Vector2f & in, Vector2f & out) const;
    protected:
        typedef map<GeometryLevel, GeometryQueue *>::type GeoQueues;
        typedef vector<UIInstance *>::type ChildList;
    protected:
        RenderSys * mSys;
        Viewport * mView;
        UISheet * mParent;
        UIInstance * mRel;
        Texture * mDst;
        GeometryPixel * mBuffer;
        ChildList mChild;
        GeoQueues mRenderQueue;
        Matrix4f mProjectMatrix;
        NIIf mViewExtent;
        Rectf mArea;
        Vector2f mPos;
        Vector2f mOft;
        Quaternion mRot;
        PlaneSizef mSize;
        Vector3f mOrigin;
        bool mValidProjectMatrix;
        bool mValidView;
        bool mValidBuffer;
        bool mRefresh;
    };
}
}
#endif