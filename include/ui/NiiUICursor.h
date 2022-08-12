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

#ifndef _NII_UI_CURSOR_H_
#define _NII_UI_CURSOR_H_

#include "NiiUIPreInclude.h"
#include "NiiCommandObj.h"
#include "NiiUICursorEventArgs.h"
#include "NiiPlaneSize.h"

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
        Cursor(const Recti & area);
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
        void hide()                         { mVisible = false; }

        /** ��ʾ
        @version NIIEngine 3.0.0
        */
        void show()                         { mVisible = true; }

        /** �����Ƿ�ɼ�
        @version NIIEngine 3.0.0
        */
        void setVisible(bool b)             { mVisible = b; }

        /** ��ȡ�Ƿ�ɼ�
        @version NIIEngine 3.0.0
        */
        bool isVisible() const              { return mVisible; }

        /** ���ô�С
        @version NIIEngine 3.0.0
        */
        void setRenderSize(const PlaneSizef & size);

        /** ��ȡ��С
        @version NIIEngine 3.0.0
        */
        const PlaneSizef & getRenderSize() const { return mSize; }

        /** ����λ��
        @version NIIEngine 3.0.0
        */
        void setPos(const Vector2f & pos);

        /** ��ȡλ��
        @version NIIEngine 3.0.0
        */
        const Vector2f & getPos() const         { return mPos; }

        /** ���õ�ǰͼ��
        @version NIIEngine 3.0.0
        */
        void setImage(PixelBuffer * pb);

        /** ��ȡ��ǰͼ��
        @version NIIEngine 3.0.0
        */
        PixelBuffer * getImage() const          { return mView; }

        /** ���û���ͼ��
        @version NIIEngine 3.0.0
        */
        void setDefaultImage(PixelBuffer * pb);

        /** ��ȡ����ͼ��
        @version NIIEngine 3.0.0
        */
        PixelBuffer * getDefaultImage() const   { return mNormalView; }

        /** �����ƶ�����
        @version NIIEngine 3.0.0
        */
        void setMoveArea(const Recti * area);

        /** ��ȡ�ƶ�����
        @version NIIEngine 3.0.0
        */
        Recti getMoveArea() const;

        /** �����ƶ�����
        @version NIIEngine 3.0.0
        */
        void setEvnMoveArea(const RelRecti * area);

        /** ��ȡ�ƶ�����
        @version NIIEngine 3.0.0
        */
        const RelRecti & getEvnMoveArea() const { return mMoveArea; }

        /** ֪ͨ���̴�С
        @version NIIEngine 3.0.0
        */
        void notifyView(const PlaneSizei & size);
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
        Recti mArea;
        RelRecti mMoveArea;
        bool mVisible;
        bool mBufferValid;
    };
}
}
#endif