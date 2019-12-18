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
        �������������������(CAD)
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/

#ifndef _NII_UI_CURSOR_H_
#define _NII_UI_CURSOR_H_

#include "NiiUIPreInclude.h"
#include "NiiCommandObj.h"
#include "NiiUICursorEventArgs.h"
#include "NiiRelPos.h"

using namespace NII::NII_COMMAND;

namespace NII
{
namespace UI
{
    /** ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Cursor : public CommandObj, public UIAlloc
    {
    public:
        Cursor(const Rectf & area);
        ~Cursor();

        /** ����
        @version NIIEngine 3.0.0
        */
        void draw();

        /** ˢ��
        @note һ����ͼ��/λ�ñ仯��Ҫˢ��
        @version NIIEngine 3.0.0
        */
        void refresh();

        /** ����
        @version NIIEngine 3.0.0
        */
        void hide();

        /** ��ʾ
        @version NIIEngine 3.0.0
        */
        void show();

        /** �����Ƿ�ɼ�
        @version NIIEngine 3.0.0
        */
        void setVisible(bool b);

        /** ��ȡ�Ƿ�ɼ�
        @version NIIEngine 3.0.0
        */
        bool isVisible() const;

        /** ���ô�С
        @version NIIEngine 3.0.0
        */
        void setRenderSize(const PlaneSizef & size);

        /** ��ȡ��С
        @version NIIEngine 3.0.0
        */
        const PlaneSizef & getRenderSize() const;

        /** ����λ��
        @version NIIEngine 3.0.0
        */
        void setPos(const Vector2f & pos);

        /** ��ȡλ��
        @version NIIEngine 3.0.0
        */
        const Vector2f & getPos() const;

        /** ���õ�ǰͼ��
        @version NIIEngine 3.0.0
        */
        void setImage(PixelBuffer * pb);

        /** ��ȡ��ǰͼ��
        @version NIIEngine 3.0.0
        */
        PixelBuffer * getImage() const;

        /** ���û���ͼ��
        @version NIIEngine 3.0.0
        */
        void setDefaultImage(PixelBuffer * pb);

        /** ��ȡ����ͼ��
        @version NIIEngine 3.0.0
        */
        PixelBuffer * getDefaultImage() const;

        /** �����ƶ�����
        @version NIIEngine 3.0.0
        */
        void setMoveArea(const Rectf * area);

        /** ��ȡ�ƶ�����
        @version NIIEngine 3.0.0
        */
        Rectf getMoveArea() const;

        /** �����ƶ�����
        @version NIIEngine 3.0.0
        */
        void setEvnMoveArea(const RelRect * area);

        /** ��ȡ�ƶ�����
        @version NIIEngine 3.0.0
        */
        const RelRect & getEvnMoveArea() const;

        /** ֪ͨ���̴�С
        @version NIIEngine 3.0.0
        */
        void notifyView(const PlaneSizef & size);
    protected:
        Cursor();

        /** ��ǰͼ���ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onImageChange(const CursorViewArgs * arg);

        /** ����ͼ���ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onDefaultImageChange(const CursorViewArgs * arg);

        void clampMoveArea();
    public:
        /** ��ǰͼ���ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID ImageChangeEvent;

        /** ����ͼ���ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID DefaultImageChangeEvent;

        /// �¼�����
        static const EventID EventCount;
    private:
        GeometryPixel * mBuffer;
        PixelBuffer * mView;
        PixelBuffer * mNormalView;
        Vector2f mPos;
        PlaneSizef mSize;
        Rectf mArea;
        RelRect mMoveArea;
        bool mVisible;
        bool mBufferValid;
    };
}
}
#endif