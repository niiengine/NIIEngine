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
#include "NiiKeyboardControlPattern.h"
#include "NiiMouseControlPattern.h"
#include "NiiJoyPadControlPattern.h"
#include "NiiTouchControlPattern.h"
#include "NiiMatrix4.h"
#include "NiiRect.h"

using namespace NII::NII_COMMAND;

namespace NII
{
namespace UI
{
    class _EngineAPI SheetKeyboardControlItem : public NII_MEDIA::KeyboardControlItem
    {
    public:
        SheetKeyboardControlItem();
    };

    class _EngineAPI SheetMouseControlItem : public NII_MEDIA::MouseControlItem
    {
    public:
        SheetMouseControlItem();
    };

    class _EngineAPI SheetJoyPadControlItem : public NII_MEDIA::JoyPadControlItem
    {
    public:
        SheetJoyPadControlItem();
    };

    /** ������Ⱦ����
    @remark ������Ⱦ����,������UIԪ�ز�������Ч��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UISubSheet : public NII_COMMAND::CommandObj, public UIAlloc
    {
        friend class UISheet;
    public:
        /** ���캯��
        @param[in] rs ��Ҫ
        @param[in] own ��Ҫ
        @param[in] oft Sheet�����е�ƫ��, �������ó� Vector2f::ZERO
        @version NIIEngine 3.0.0 �߼�api
        */
        UISubSheet(RenderSys * rs, UISheet * own, const Vector2f & oft);

        ~UISubSheet();

        /** �����ӻ���
        @param[out] obj �����������ӻ���
        @version NIIEngine 3.0.0
        */
        void create(UISubSheet *& obj);

        /** ɾ���ӻ���
        @param[in] obj Ҫɾ�����ӻ���
        @version NIIEngine 3.0.0
        */
        void destroy(UISubSheet * obj);

        /** �����ӻ���
        @note �ڴ��ⲿ����
        @param[in] obj �ӻ���
        @version NIIEngine 3.0.0
        */
        void attach(UISubSheet * obj);

        /** ����ӻ���
        @note �ڴ��ⲿ����
        @param[in] obj �ӻ���
        @version NIIEngine 3.0.0
        */
        void detach(UISubSheet * obj);

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
        inline bool isRefresh() const { return mRefresh; }

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

        /** ������ʾ����
        @param[in] area ����
        @version NIIEngine 3.0.0
        */
        void setAreaSize(const PlaneSizef & size)   { setArea(Rectf(mArea.getPos(), size)); }

        /** ��ȡ��ʾ����
        @version NIIEngine 3.0.0
        */
        inline const Rectf & getArea() const        { return mArea; }

        /** ����λ��
        @version NIIEngine 3.0.0
        */
        void setPos(const Vector2f & pos);

        /** ��ȡλ��
        @version NIIEngine 3.0.0
        */
        const Vector2f & getPos() const { return mPos; }

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
        inline const Quaternion & getRotation()const { return mRot; }

        /** Ӧ����ȾЧ��
        @version NIIEngine 3.0.0
        */
        void setEffect(RenderEffect * obj) { mBuffer->setEffect(obj); }

        /** ��ȡ��Ӧ�õ�Ч��
        @version NIIEngine 3.0.0
        */
        inline RenderEffect * getEffect() const { return mBuffer->getEffect(); }

        /** ��UI��
        @version NIIEngine 3.0.0
        */
        UISheet * getSheet() const { return mParent; }

        /** ������
        @note ��������ڸ�����ϵ,������ NULL ����
        @version NIIEngine 3.0.0
        */
        UISubSheet * getRel() const { return mRel; }

        /** ����Ŀ��
        @version NIIEngine 3.0.0
        */
        Texture * getTexture() const { return mDst; }

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
        typedef vector<UISubSheet *>::type ChildList;
    protected:
        RenderSys * mSys;
        Viewport * mView;
        UISheet * mParent;
        UISubSheet * mRel;
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

    /** UI����������
    @version NIIEngine 3.0.0
    */
    struct SheetContext : public UIAlloc
    {
        const Window * mRoot;   ///<     
        UISheet * mSheet;       ///< 
        UISubSheet * mFace;     ///< 
        GeometryLevel mQueue;   ///<         
        Vector2f mPosOffset;    ///< 
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
        void redraw()                   { mRedraw = true; }

        /** �Ƿ����»���
        @version NIIEngine 3.0.0
        */
        bool isRedraw() const           { return mRedraw; }

        /** ����״̬
        @param[in] cost �ϴε�������ε��öɹ���ʱ��
        @version NIIEngine 3.0.0
        */
        void update(TimeDurMS cost);

        /** ��ȡ������������
        @version NIIEngine 3.0.0
        */
        Nmark getWinComKey() const      { return mWinComKey; }

        /** ����UI����
        @version NIIEngine 3.0.0
        */
        void create(UISubSheet *& out);

        /** ɾ��UI����
        @version NIIEngine 3.0.0
        */
        void destroy(UISubSheet * obj);

        /** ����UI����
        @note �ڴ治��������������
        @version NIIEngine 3.0.0
        */
        void attach(UISubSheet * obj);

        /** ���UI����
        @note �ڴ治��������������
        @version NIIEngine 3.0.0
        */
        void detach(UISubSheet * obj);

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

        /** ������ʾ����
        @param[in] area ����
        @version NIIEngine 3.0.0
        */
        void setAreaSize(const PlaneSizei & size)   { setArea(Recti(mArea.getPos(), size)); }

        /** ��ȡ��ʾ����
        @version NIIEngine 3.0.0
        */
        const Recti & getArea() const               { return mArea; }

        /** ���ص�ǰ�ȴ��رմ���
        @version NIIEngine 3.0.0 �߼�api
        */
        void setModal(Window * obj)                 { mModal = obj; }

        /** ���ص�ǰ�ȴ��رմ���
        @version NIIEngine 3.0.0
        */
        Window * getModal() const                   { return mModal; }

        /** ���õ�ǰ��Ĵ���
        @version NIIEngine 3.0.0 �߼�api
        */
        void setActive(Window * obj)                { mActive = obj; }

        /** ��ȡ��ǰ��Ĵ���
        @version NIIEngine 3.0.0
        */
        Window * getActive() const                  { return mActive; }

        /** ���õ�ǰ�Խ���Ԫ
        @note getActive ��һ����ϵ
        @version NIIEngine 3.0.0 �߼�api
        */
        void setFocus(Widget * obj)                 { mFocus = obj; }

        /** ��ȡ��ǰ�Խ���Ԫ
        @note getActive ��һ����ϵ
        @version NIIEngine 3.0.0
        */
        Widget * getFocus() const                   { return mFocus; }

        /** �����α�����
        @version NIIEngine 3.0.0
        */
        void setCursorArea(const PlaneSizef & sz);

        /** ��ȡ�α�����
        @version NIIEngine 3.0.0
        */
        const PlaneSizef & getCursorArea() const    { return mCursorArea; }

        /** �Ƿ������α���ص��
        @version NIIEngine 3.0.0
        */
        void setCursorMultiClick(bool b)            { mGenCursorMultiClick = b; }

        /** ��ȡ�Ƿ��α���ص��
        @version NIIEngine 3.0.0
        */
        bool getCursorMultiClick() const            { return mGenCursorMultiClick; }

        /** �α��ƶ�ϵ��
        @version NIIEngine 3.0.0
        */
        void setCursorMoveFactor(NIIf f);

        /** �α��ƶ�ϵ��
        @version NIIEngine 3.0.0
        */
        NIIf getCursorMoveFactor() const            { return mCursorMoveFactor; }

        /** �����α���ʱ�䷧ֵ
        @note ������ֵ�򲻻��������¼�
        @version NIIEngine 3.0.0
        */
        void setCursorClickFactor(TimeDurMS time);

        /** ��ȡ�α���ʱ�䷧ֵ
        @note ������ֵ�򲻻��������¼�
        @version NIIEngine 3.0.0
        */
        TimeDurMS getCursorClickFactor() const      { return mCursorClickFactor; }

        /** �����α���ص��ʱ�䷧ֵ
        @note ������ֵ�򲻻��������¼�
        @version NIIEngine 3.0.0
        */
        void setCursorMultiClickFactor(TimeDurMS time);

        /** ��ȡ�α���ص��ʱ�䷧ֵ
        @note ������ֵ�򲻻��������¼�
        @version NIIEngine 3.0.0
        */
        TimeDurMS getCursorMultiClickFactor() const { return mCursorMultiClickFactor; }

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
        Tooltip * getTooltip() const                { return mTooltip; }

        /** ��ȡ�α����
        @note �����豸һ�㲻�����������
        @version NIIEngine 3.0.0
        */
        Cursor * getCursor() const                  { return mCursor; }

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
        ViewWindow * getWindow() const              { return mTarget; }

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
        typedef vector<UISubSheet *>::type Childs;
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