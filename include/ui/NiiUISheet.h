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
#ifndef _NII_UI_SHEET_H_
#define _NII_UI_SHEET_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidgetEventArgs.h"
#include "NiiUICursor.h"
#include "NiiGeometryPixel.h"
#include "NiiCommandObj.h"
#include "NiiKeyBoardControlItem.h"
#include "NiiMatrix4.h"

using namespace NII::NII_COMMAND;

namespace NII
{
namespace UI
{
    /** UI����������
    @version NIIEngine 3.0.0
    */
    struct SheetContext : public UIAlloc
    {
        const Window * mRoot;   ///<     
        UISheet * mSheet;       ///< 
        UIInstance * mFace;     ///< 
        GeometryLevel mQueue;   ///<         
        Vector2f mPosOffset;    ///< 
    };

    /** ����ͼ���¼�����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GeometryLevelEventArgs : public EventArgs
    {
    public:
        GeometryLevelEventArgs(GeometryLevel gl);

        GeometryLevel mGL;
    };
    
    /** UI������Ⱦ������
    @remark ������ȾUI����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UISheet : public CommandObj, public UIAlloc
    {
        friend class UIManager;
    public:
        UISheet(RenderSys * rs, ViewWindow * win);

        ~UISheet();

        /** Ӧ��״̬ģʽ
        @note Ӧ��״̬ģʽ��,�����ƵĶ�����λ��/ͶӰ���òŷ���UI���Ʊ���
        @version NIIEngine 3.0.0 �߼�api
        */
        void apply();

        /** ��ˢ����
        @version NIIEngine 3.0.0
        */
        void flush();

        /** ���»���
        @version NIIEngine 3.0.0
        */
        void redraw();

        /** �Ƿ����»���
        @version NIIEngine 3.0.0
        */
        bool isRedraw() const;

        /** ����״̬
        @param[in] cost �ϴε�������ε��öɹ���ʱ��
        @version NIIEngine 3.0.0
        */
        void update(TimeDurMS cost);

        /** ��ȡ������������
        @version NIIEngine 3.0.0
        */
        Nmark getWinComKey() const;

        /** ����UI����
        @version NIIEngine 3.0.0
        */
        void create(UIInstance *& out);

        /** ɾ��UI����
        @version NIIEngine 3.0.0
        */
        void destroy(UIInstance * obj);

        /** ����UI����
        @note �ڴ治��������������
        @version NIIEngine 3.0.0
        */
        void attach(UIInstance * obj);

        /** ���UI����
        @note �ڴ治��������������
        @version NIIEngine 3.0.0
        */
        void detach(UIInstance * obj);

        /** ��Ӽ�������
        @param[in] gl ���β�
        @param[in] obj ��������
        @version NIIEngine 3.0.0
        */
        void add(GeometryLevel gl, GeometryPixel * obj);

        /** ��ȥ��������
        @param[in] gl ���β�
        @param[in] obj ��������
        @version NIIEngine 3.0.0
        */
        void remove(GeometryLevel gl, GeometryPixel * obj);

        /** ��ȥָ�����β�ļ�������
        @param[in] gl ���β�
        @version NIIEngine 3.0.0
        */
        void clear(GeometryLevel gl);

        /** ��ȥ���м�������
        @version NIIEngine 3.0.0
        */
        void clear();

        /** ��Ӵ���
        @note �ڴ治��������������
        @version NIIEngine 3.0.0
        */
        void add(Window * obj);

        /** ��ȥ����
        @note �ڴ治��������������
        @verison NIIEngine 3.0.0
        */
        void remove(Window * ob);

        /** ������ʾ����
        @version NIIEngine 3.0.0
        */
        void setArea(const Recti & area);

        /** ��ȡ��ʾ����
        @version NIIEngine 3.0.0
        */
        const Recti & getArea() const;

        /** ���ص�ǰ�ȴ��رմ���
        @version NIIEngine 3.0.0 �߼�api
        */
        void setModal(Window * obj);

        /** ���ص�ǰ�ȴ��رմ���
        @version NIIEngine 3.0.0
        */
        Window * getModal() const;

        /** ���õ�ǰ��Ĵ���
        @version NIIEngine 3.0.0 �߼�api
        */
        void setActive(Window * obj);

        /** ��ȡ��ǰ��Ĵ���
        @version NIIEngine 3.0.0
        */
        Window * getActive() const;

        /** ���õ�ǰ�Խ���Ԫ
        @note getActive ��һ����ϵ
        @version NIIEngine 3.0.0 �߼�api
        */
        void setFocus(Widget * obj);

        /** ��ȡ��ǰ�Խ���Ԫ
        @note getActive ��һ����ϵ
        @version NIIEngine 3.0.0
        */
        Widget * getFocus() const;

        /** �����α�����
        @version NIIEngine 3.0.0
        */
        void setCursorArea(const PlaneSizef & sz);

        /** ��ȡ�α�����
        @version NIIEngine 3.0.0
        */
        const PlaneSizef & getCursorArea() const;

        /** �Ƿ������α���ص��
        @version NIIEngine 3.0.0
        */
        void setCursorMultiClick(bool b);

        /** ��ȡ�Ƿ��α���ص��
        @version NIIEngine 3.0.0
        */
        bool getCursorMultiClick() const;

        /** �α��ƶ�ϵ��
        @version NIIEngine 3.0.0
        */
        void setCursorMoveFactor(NIIf f);

        /** �α��ƶ�ϵ��
        @version NIIEngine 3.0.0
        */
        NIIf getCursorMoveFactor() const;

        /** �����α���ʱ�䷧ֵ
        @note ������ֵ�򲻻��������¼�
        @version NIIEngine 3.0.0
        */
        void setCursorClickFactor(TimeDurMS time);

        /** ��ȡ�α���ʱ�䷧ֵ
        @note ������ֵ�򲻻��������¼�
        @version NIIEngine 3.0.0
        */
        TimeDurMS getCursorClickFactor() const;

        /** �����α���ص��ʱ�䷧ֵ
        @note ������ֵ�򲻻��������¼�
        @version NIIEngine 3.0.0
        */
        void setCursorMultiClickFactor(TimeDurMS time);

        /** ��ȡ�α���ص��ʱ�䷧ֵ
        @note ������ֵ�򲻻��������¼�
        @version NIIEngine 3.0.0
        */
        TimeDurMS getCursorMultiClickFactor() const;

        /** ����Ĭ���������
        @remark
            Ĭ������.���UI��Ԫ��û���Լ�����������,��ʹ���������.
        @param[in] �������
        @version NIIEngine 3.0.0 �߼�api
        */
        void setFont(Font * obj);

        /** ��ȡĬ���������
        @remark
            Ĭ������.���UI��Ԫ��û���Լ�����������,��ʹ���������.
        @param[out] �������
        @version NIIEngine 3.0.0 �߼�api
        */
        Font * getFont() const;

        /** ����Ĭ�ϵ���ʾ�����
        @remark
            ���UI��Ԫ��û���Լ�����ʾ������,��ʹ�������ʾ��.
        @param[in] obj ��ʾ�����
        @version NIIEngine 3.0.0 �߼�api
        */
        void setTooltip(Tooltip * obj, bool autoDestroy = true);

        /** ��ȡĬ�ϵ���ʾ�����
        @remark
            ���UI��Ԫ��û���Լ�����ʾ������,��ʹ�������ʾ��.
        @remark[out] obj ��ʾ�����
        @version NIIEngine 3.0.0 �߼�api
        */
        Tooltip * getTooltip() const;

        /** ��ȡ�α����
        @note �����豸һ�㲻�����������
        @version NIIEngine 3.0.0
        */
        Cursor * getCursor() const;

        /** ˢ�¸���
        @version NIIEngine 3.0.0
        */
        void resetCursorOver();

        /** ��ȡ��긲�ǵĴ���
        @note �α��ƶ�ʱ���ܻ��б仯
        @version NIIEngine 3.0.0
        */
        Widget * getCursorOver();

        /** ������ͼ����
        @version NIIEngine 3.0.0 �߼�api
        */
        void setWindow(ViewWindow * obj);

        /** ��ȡ��ͼ����
        @version NIIEngine 3.0.0 �߼�api
        */
        ViewWindow * getWindow() const;

        /** ��ȡ���߹�ͬ����������
        @version NIIEngine 3.0.0
        */
        Container * getAncestor(Widget * w1, Widget * w2) const;

        /** ����任
        @param[in] m ʵ�ʿռ�
        @param[in] in ����
        @param[in] out ���
        @version NIIEngine 3.0.0
        */
        void affine(const Matrix4f * m, const Vector2f & in, Vector2f & out) const;
    public:
        /** ��������ʱ
        @version NIIEngine 3.0.0
        */
        bool onButtonDown(NII_MEDIA::MouseButton button);

        /** �����ͷ�ʱ
        @version NIIEngine 3.0.0
        */
        bool onButtonUp(NII_MEDIA::MouseButton button);

        /** �α��ƶ�ʱ
        @version NIIEngine 3.0.0
        */
        bool onCursorMove(NIIi dx, NIIi dy);

        /** �α���קʱ
        @version NIIEngine 3.0.0
        */
        bool onCursorDrag(NIIi delta);

        /** �α갴��ʱ
        @version NIIEngine 3.0.0
        */
        bool onCursorTouch(NII_MEDIA::MouseButton button);

        /** �α갴��ʱ
        @version NIIEngine 3.0.0
        */
        bool onCursorMultiTouch(NII_MEDIA::MouseButton button, NCount count);

        /** ���̰���ʱ
        @version NIIEngine 3.0.0
        */
        bool onKeyDown(NII_MEDIA::KeyValue key);

        /** �����ͷŰ���ʱ
        @version NIIEngine 3.0.0
        */
        bool onKeyUp(NII_MEDIA::KeyValue key);

        /** �ı�����ʱ
        @version NIIEngine 3.0.0
        */
        bool onChar(Nui32 chr);

        /** ִ�и���ʱ
        @version NIIEngine 3.0.0
        */
        bool onObjCopy();

        /** ִ�м���ʱ
        @version NIIEngine 3.0.0
        */
        bool onObjCut();

        /** ִ��ճ��ʱ
        @version NIIEngine 3.0.0
        */
        bool onObjPaste();
    protected:
        /** ��ȡ��ǰλ����Ч��UI��Ԫ
        @param[in] pos λ��
        @param[in] disable ��ʹ����Ҳ��Ч
        @version NIIEngine 3.0.0
        */
        Widget * getDest(const Vector2f & pos, bool disable) const;

        /** ��ȡ��ǰ�Խ�
        @version NIIEngine 3.0.0
        */
        Widget * getForce() const;

        /** ����
        @version NIIEngine 3.0.0
        */
        void updateMatrix();

        /** �ӿ�
        @version NIIEngine 3.0.0
        */
        void updateViewport();

        /** ������ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onFont(const FontEventArgs * args);

        /** ��ͼ����ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onWindowChange(const RenderSheetEventArgs * args);

        /** �α�����ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onCursorAreaChange(const SheetEventArgs * args);

        /** �α��ƶ�ϵ���ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onCursorMoveFactorChange(const SheetEventArgs * args);

        /** �α굥��ʱ�䷧ֵ�ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onCursorClickFactorChange(const SheetEventArgs * args);

        /** �α���ص��ʱ�䷧ֵ�ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onCursorMultiClickFactorChange(const SheetEventArgs * args);
    protected:
        /** �����������
        @version NIIEngine 3.0.0
        */
        void FontChangeCB(Widget * obj) const;

        /** ɾ����Ԫ����
        @version NIIEngine 3.0.0
        */
        void DestroyCB(const Widget * obj);
    public:
        /** ��Ⱦÿ������ǰ�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID PreRenderEvent;

        /** ��Ⱦÿ�����к��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID EndRenderEvent;

        /** ����ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID FontEvent;

        /** ��ͼ����ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID WindowChangeEvent;

        /** �α�����ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID CursorAreaChangeEvent;

        /** �α��ƶ����Ӹı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID CursorMoveFactorChangeEvent;

        /** �α굥��ʱ�䷧ֵ�ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID CursorClickFactorChangeEvent;

        /** �α���ص��ʱ�䷧ֵ�ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID CursorMultiClickFactorChangeEvent;

        /** �¼�����
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        typedef map<GeometryLevel, GeometryQueue>::type Queues;
        typedef vector<UIInstance *>::type Childs;
        typedef vector<Window *>::type WinList;
    protected:
        RenderSys * mSys;
        Viewport * mView;
        ViewWindow * mTarget;
        Window * mModal;                    ///< ģ̬����
        Window * mActive;                   ///< �����
        Widget * mFocus;                    ///< �������ϵ
        Font * mFont;
        Tooltip * mTooltip;
        Widget * mOverlay;
        Queues mRenderQueue;
        WinList mWinList;
        Childs mChild;
        Cursor * mCursor;
        CursorMultiTouch * mCursorTouch;
        Nui32 mWinComKey;
        Matrix4f mProjectMatrix;
        PlaneSizef mCursorArea;
        NIIf mViewExtent;
        Recti mArea;

        NIIf mCursorMoveFactor;
        TimeDurMS mCursorClickFactor;
        TimeDurMS mCursorMultiClickFactor;

        bool mRedraw;
        bool mValidView;
        bool mAutoTooltip;
        bool mValidOverlay;
        bool mValidProjectMatrix;
        bool mGenCursorMultiClick;
    };
}
}
#endif