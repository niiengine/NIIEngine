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

#ifndef _NII_UI_WIDGET_H_
#define _NII_UI_WIDGET_H_

#include "NiiUIPreInclude.h"
#include "NiiCommandObj.h"
#include "NiiPropertyCmdObj.h"
#include "NiiExtData.h"
#include "NiiUIWidgetView.h"
#include "NiiUIPixelUnitGrid.h"
#include "NiiUIWidgetAide.h"
#include "NiiUIWidgetList.h"
#include "NiiUIWidgetEventArgs.h"
#include "NiiUISheet.h"
#include "NiiUICalcRect.h"
#include "NiiClipboard.h"
#include "NiiQuaternion.h"

using namespace NII::NII_MEDIA;
using namespace NII::NII_COMMAND;

namespace NII
{
namespace UI
{
    typedef Rectf(Widget::*CalcMethod)(bool) const;
    typedef bool(Widget::*intersect)(const Vector2f &, bool) const;
    /** UI��Ԫ������
    @remark
        UI��Ԫ�����ܶ����ڴ��弶���UI��Ԫ����,Ҳ����˵UI��Ԫ����������һ������,��
        ������û��Ч��.��������Ϊ�˱����ظ����ж��ص�λ�ù�ϵ,��ֱ��ͨ���ϼ�����ȷ��
    @note ��������ಢû����Ⱦ����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Widget : public CommandObj, public PropertyCmdObj, 
        public ExtData, public UIAlloc
    {
        friend class UIWidgetManager;
        friend class WidgetView;
        friend class UIInstance;
        friend class UISheet;
        friend class Window;
        friend class Container;
        friend class MouseControlItem;
    public:
        Widget(ScriptTypeID stid, WidgetID wid, FactoryID sid, Container * own,
            const String & name = N_StrBlank, LangID lid = N_PrimaryLang);
        virtual ~Widget();

        /** ����
        @param r �����¼�����
        @return ��������
        @version NIIEngine 3.0.0
        */
        Widget * clone(bool r = true) const;

        /** ����
        @param[in] cost ���ϴε��ú�ɹ���ʱ��,���ڸ��¿��ܳ��ֵĶ���
        @version NIIEngine 3.0.0
        */
        virtual void update(TimeDurMS cost);

        /** �״̬
        @version NIIEngine 3.0.0
        */
        virtual void activate();

        /** �ǻ״̬
        @version NIIEngine 3.0.0
        */
        virtual void deactivate();

        /** ��ʾ
        @version NIIEngine 3.0.0
        */
        void show();

        /** ����
        @version NIIEngine 3.0.0
        */
        void hide();

        /** ����״̬
        @version NIIEngine 3.0.0
        */
        void enable();

        /** ����״̬
        @version NIIEngine 3.0.0
        */
        void disable();

        /** ɾ���ڲ��ṹ
        @version NIIEngine 3.0.0
        */
        virtual void destroy();

        /** �Ƿ�����������
        @note ָ���� NII::UI::Container ��
        @version NIIEngine 3.0.0
        */
        virtual bool isContainer() const;

        /** ����������������
        @version NIIEngine 3.0.0
        */
        inline FactoryID getType() const{ return mType;}

        /** ����UI��ԪID
        @version NIIEngine 3.0.0
        */
        void setID(WidgetID id);

        /** ���������������ID
        @version NIIEngine 3.0.0
        */
        inline WidgetID getID() const{ return mID; }

        /** ��������
        @version NIIEngine 3.0.0
        */
        void setName(const String & n);

        /** ��ȡ����
        @version NIIEngine 3.0.0
        */
        inline const String & getName() const{ return mName; }

        /** ��ȡ����·��
        @remark ����������·��������
        @version NIIEngine 3.0.0
        */
        String getNamePath() const;

        /** ������������
        @param c ��������
        @version NIIEngine 3.0.0
        */
        void setParent(Container * c);

        /** ��ȡ��������
        @version NIIEngine 3.0.0
        */
        inline Container * getParent() const { return mParent; }

        /** ��ȡUI��Ԫ���ڵĻ���
        @version NIIEngine 3.0.0
        */
        inline UISheet * getSheet() const { return mSheet; }

        /** �Ƿ�����UI��Ԫ
        @version NIIEngine 3.0.0
        */
        void setEnable(bool b);

        /** ����UI��Ԫ�ĸ���ģʽ
        @remark ����UI��Ԫ���ݷ����仯ʱ�����
        @version NIIEngine 3.0.0
        */
        inline void setUpdateMode(RefreshMode m){ mRefreshMode = m; }

        /** ����UI��Ԫ�ĸ���ģʽ
        @remark ����UI��Ԫ���ݷ����仯ʱ�����
        @version NIIEngine 3.0.0
        */
        inline RefreshMode getUpdateMode() const{  return mRefreshMode;}

        /** ����UI��Ԫ�Ƿ�ɼ�
        @version NIIEngine 3.0.0
        */
        void setVisible(bool b);

        /** �������Ԫ�ص�ǰ�Ƿ�ɼ�
        @version NIIEngine 3.0.0
        */
        inline bool isVisible() const{ return mVisible; }

        /** ���ò��Ƿ���Ըı�
        @version NIIEngine 3.0.0
        */
        inline void setZOrder(bool b) { mZOrder = b; }

        /** ��ȡ���Ƿ���Ըı�
        @version NIIEngine 3.0.0
        */
        inline bool isZOrder() const { return mZOrder; }

        /** �����α����Ƿ����������
        @version NIIEngine 3.0.0
        */
        inline void setZOrderClick(bool b) { mZOrderClick = b; }

        /** ��ȡ�α����Ƿ����������
        @version NIIEngine 3.0.0
        */
        inline bool isZOrderClick() const{ return mZOrderClick; }
        
        /** ���ñ���
        @version NIIEngine 3.0.0
        */
        virtual void setMargin(const RelRectf & rect);

        /** ��ȡ����
        @version NIIEngine 3.0.0
        */
        inline const RelRectf & getMargin() const{ return mMargin; }

        /** ��ȡ����ƫ��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual RelVector2f getMarginOft() const;

        /** ��ȡ���ϱ����Ԫ�ش�С
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual RelVector2f getMarginSize() const;

        /** ���÷��
        @param sid ��Ʒ��
        @param wsid ��Ԫ���
        @version NIIEngine 3.0.0
        */
        void setStyle(StyleID sid, WidgetViewlID wsid);

        /** ������Ʒ��
        @version NIIEngine 3.0.0
        */
        virtual void setStyle(StyleID sid);

        /** ��ȡ��Ʒ��
        @version NIIEngine 3.0.0
        */
        inline StyleID getStyle() const{return mStyleID;}

        /** ���õ�Ԫ���
        @version NIIEngine 3.0.0
        */
        void setWidgetModel(WidgetViewlID wsid);

        /** ��ȡ��Ԫ���
        @version NIIEngine 3.0.0
        */
        inline WidgetView * getWidgetModel() const{return mWidgetModel;}

        /** ������ʾ�ı�
        @param[in] txt ��ʾ�ı�
        @version NIIEngine 3.0.0
        */
        void setTooltipText(const String & txt);

        /** ��ȡ��ʾ�ı�
        @version NIIEngine 3.0.0
        */
        const String & getTooltipText() const;

        /** �����Ƿ�ʹ�ø�������ʾ�ı�
        @version NIIEngine 3.0.0
        */
        inline void setParentTipsText(bool b){mParentTipsText = b;}

        /** ��ȡ�Ƿ�ʹ�ø�������ʾ�ı�
        @return NIIEngine 3.0.0
        */
        inline bool isParentTooltipText() const {return mParentTipsText;}

        /** ����ˮƽ���뷽ʽ
        @remark ���λ������ڸ�����
        @param[in] align ˮƽ���뷽ʽ
        @version NIIEngine 3.0.0
        */
        void setXAlign(HAlign align);

        /** ��ȡˮƽ���뷽ʽ
        @remark ���λ������ڸ�����
        @return ˮƽ���뷽ʽ
        @version NIIEngine 3.0.0
        */
        inline HAlign getXAlign() const{return mHAlign;}

        /*** ���ô�ֱ���뷽ʽ
        @remark ���λ������ڸ�����
        @param align ��ֱ���뷽ʽ
        @version NIIEngine 3.0.0
        */
        void setYAlign(VAlign align);

        /** ��ȡ��ֱ���뷽ʽ
        @remark ���λ������ڸ�����
        @return ��ֱ���뷽ʽ
        @version NIIEngine 3.0.0
        */
        inline VAlign getYAlign() const{return mVAlign;}

        /** ����͸����
        @param a ͸����[0.0-1.0]
        @version NIIEngine 3.0.0
        */
        void setAlpha(NIIf a);

        /** ����������������õ�͸����
        @version NIIEngine 3.0.0
        */
        inline NIIf getAlpha() const{ return mAlpha;}

        /** �����Ƿ�ʹ�û���͸����
        @remark һ����ָͬ����ϵ�е�͸����
        @version NIIEngine 3.0.0
        */
        void setEnvAlpha(bool b);

        /** ��ȡ�Ƿ�ʹ�û���͸����
        @remark һ����ָͬ����ϵ�е�͸����
        @version NIIEngine 3.0.0
        */
        inline bool isEnvAlpha() const { return mEnvAlpha; }

        /** ��ȡ���յ�͸����
        @version NIIEngine 3.0.0
        */
        NIIf getAlphaEffect() const;

        /** ���������С����ʾ������ģʽ
        @param mode ģʽ
        @version NIIEngine 3.0.0
        */
        void setScaleMode(AspectMode mode);

        /** ��ȡ�����С����ʾ������ģʽ
        @version NIIEngine 3.0.0
        */
        inline AspectMode getScaleMode() const{ return mScaleMode; }

        /** ���������С����ʾ����������
        @param ratio ��/�ߵı���
        @note AspectMode �� AM_IGNORE ʱ��Ч.
        @version NIIEngine 3.0.0
        */
        void setScaleFactor(NIIf ratio);

        /** ��ȡ�����С����ʾ����������
        @version NIIEngine 3.0.0
        */
        inline NIIf getScaleFactor() const { return mScaleFactor;}

        /** �����Ƿ���������
        @remark
            �������2��UI��������ͬ�Ĳ������,ϵͳ���ᰴʵ����ʾ�Ⱥ���в�����
        @version NIIEngine 3.0.0
        */
        void setTop(bool b);

        /** ��ȡ�Ƿ���������
        @remark
            �������2��UI��������ͬ�Ĳ������,ϵͳ���ᰴʵ����ʾ�Ⱥ���в�����
        @version NIIEngine 3.0.0
        */
        inline bool isTop() const { return mTop; }

        /** ����Ԫ�ص�λ��
        @param pos ���λ��
        @version NIIEngine 3.0.0
        */
        void setPos(const RelVector2f & pos);

        /** ��ȡԪ��λ��
        @return ���λ��
        @version NIIEngine 3.0.0
        */
        inline RelVector2f getPos() const { return mArea.getPos(); }

        /**
        @version NIIEngine 3.0.0
        */
        NIIf getActualPosX() const;

        /**
        @version NIIEngine 3.0.0
        */
        NIIf getActualPosY() const;

        /** ����Ԫ�صĴ�С
        @param size Ԫ�ش�С
        @version NIIEngine 3.0.0
        */
        void setSize(const RelPlaneSizef & size);

        /** ��ȡԪ�صĴ�С
        @return Ԫ�ش�С
        @version NIIEngine 3.0.0
        */
        inline RelPlaneSizef getSize() const { return mArea.getSize(); }

        /** ����λ������.
        @param pos ���λ��
        @param size ��Դ�С
        @version NIIEngine 3.0.0
        */
        virtual void setRelArea(const RelVector2f & pos, const RelPlaneSizef & size);

        /** ��ȡλ������
        @version NIIEngine 3.0.0
        */
        inline const RelRectf & getRelArea() const { return mArea; }

        /** ���UIԪ���Ƿ���ָ����λ��
        @param[in] pos ָ��λ��
        @param[in] disable ���Դ��ڽ���״̬
        @version NIIEngine 3.0.0
        */
        virtual bool isPos(const Vector2f & pos, bool disable) const;

        /** �Ƿ����λ��
        @version NIIEngine 3.0.0
        */
        bool isTopPos() const;

        /** ��ȡ��λ��
        @version NIIEngine 3.0.0
        */
        Nidx getPosLevel() const;

        /** �Ƿ�����һ��Ԫ���ϲ�
        @version NIIEngine 3.0.0
        */
        bool isFront(const Widget * obj) const;

        /** �Ƿ�����һ��Ԫ���²�
        @version NIIEngine 3.0.0
        */
        bool isBehind(const Widget * obj) const;

        /** �������Ԫ�Ƶ���һ��ǰ��
        @version NIIEngine 3.0.0
        */
        void moveFront(const Widget * obj);

        /** �������Ԫ�Ƶ���һ������
        @version NIIEngine 3.0.0
        */
        void moveBack(const Widget * obj);

        /** �������Ԫ�Ƶ�����
        @version NIIEngine 3.0.0
        */
        void moveFront();

        /** �������Ԫ�Ƶ���ײ�
        @version NIIEngine 3.0.0
        */
        void moveBack();

        /** ���ÿ�
        @version NIIEngine 3.0.0
        */
        void setWidth(const RelPosf & w);

        /** ��ȡ��
        @version NIIEngine 3.0.0
        */
        inline RelPosf getWidth() const { return getSize().mWidth;}

        /** ���ø�
        @version NIIEngine 3.0.0
        */
        void setHeight(const RelPosf & h);

        /** ��ȡ��
        @version NIIEngine 3.0.0
        */
        inline RelPosf getHeight() const{ return getSize().mHeight;}

        /** ����Ԫ�ص���С��С
        @version NIIEngine 3.0.0
        */
        void setMinSize(const RelPlaneSizef & size);

        /** ��ȡԪ�ص���С��С
        @version NIIEngine 3.0.0
        */
        inline const RelPlaneSizef & getMinSize() const { return mMinSize; }

        /** ����Ԫ������С
        @version NIIEngine 3.0.0
        */
        void setMaxSize(const RelPlaneSizef & size);

        /** ��ȡԪ������С
        @version NIIEngine 3.0.0
        */
        inline const RelPlaneSizef & getMaxSize() const { return mMaxSize;}

        /** �����Ƿ��������
        @version NIIEngine 3.0.0
        */
        void setPixelAlign(bool b);

        /** ��ȡ�Ƿ��������
        @version NIIEngine 3.0.0
        */
        inline bool getPixelAlign() const { return mPixelAlign;}

        /** ��ȡԪ�صľ���λ��(��λ:����)
        @return Vector2f Ԫ��ʵ�ʵ�λ��(��λ:����)
        @version NIIEngine 3.0.0
        */
        Vector2f getPixelPos() const;

        /** ��ȡԪ�صĴ�С(��λ:����)
        @return Ԫ�صĴ�С(��λ:����)
        @version NIIEngine 3.0.0
        */
        inline const PlaneSizef & getPixelSize() const{ return mPixelSize; }

        /** �����Ƿ��Զ��ü�
        @note һ���Ǳ����ü�/������ü�
        @version NIIEngine 3.0.0
        */
        void setAutoClip(bool b);

        /** ��ȡ�Ƿ��Զ��ü�
        @version NIIEngine 3.0.0
        */
        inline bool isAutoClip() const{ return mAutoClip; }

        /** �Ƿ��Զ�ɾ��
        @version NIIEngine 3.0.0
        */
        void setAutoDestroy(bool b);

        /** �Ƿ��Զ�ɾ��
        @version NIIEngine 3.0.0
        */
        inline bool isAutoDestroy() const{ return mAutoDestroy;}

        /** �������Ԫ���Ƿ�
        @version NIIEngine 3.0.0
        */
        inline bool isActive() const { return mActive; }

        /** �������Ԫ���Ƿ�
        @remark ��ζ�����Ԫ�ؽ����û������¼�
        @version NIIEngine 3.0.0
        */
        bool isFinalActive() const;

        /** �������Ԫ�ص�ǰ�Ƿ����
        @return true������ֹ��,false���������
        @version NIIEngine 3.0.0
        */
        inline bool isDisable() const{ return !mEnable;}

        /** �������Ԫ�ص�ǰ�Ƿ��ֹ(����Ч��)
        @return true ������ֹ��, false���������
        @version NIIEngine 3.0.0
        */
        bool isFinalDisable() const;

        /** �������Ԫ�ص�ǰ�Ƿ�ɼ�(����Ч��)
        @version NIIEngine 3.0.0
        */
        bool isFinalVisible() const;

        /** ָ�������Ƿ����ϼ�����
        @param id �ϼ�����ID
        @version NIIEngine 3.0.0
        */
        bool isAncestor(WidgetID id) const;

        /** ָ�������Ƿ����ϼ�����
        @param e �ϼ�����
        @version NIIEngine 3.0.0
        */
        bool isAncestor(const Widget * con) const;

        /** ָ�������Ƿ����ϼ�����
        @param name �ϼ���������
        @version NIIEngine 3.0.0
        */
        bool isAncestor(const String & name) const;

        /** ��ȡ��ͬ���ϼ�����
        @note ��һ��������ϼ���������������й�ͬ�ϼ������򷵻����������ϼ�����
        @param[in] o ��һ������
        @version NIIEngine 3.0.0
        */
        const Widget * getCommonAncestor(const Widget * o) const;

        /** �����������Ƿ�����UI��Ԫ�ཻ
        @param pos ����λ��
        @param disable true UI��Ԫ���Դ��ڽ���״̬;false UI��Ԫ���ܴ��ڽ���״̬
        @return �Ƿ�����ཻ
        @version NIIEngine 3.0.0
        */
        virtual bool intersects(const Vector2f & pos, bool disable = false) const;

        /** ����ͬ����еĶ���
        @version NIIEngine 3.0.0
        */
        Widget * getActiveSibling();

        /** �������UIԪ�صĴ�������
        @remark Ҳ���ǰ������(�߿������)������
        @note ��Ԫ������Ļ�ռ�������λ����ͬ
        @return ��������
        @version NIIEngine 3.0.0
        */
        inline const CalcRect & getArea() const{  return mRect; }

        /** �������UIԪ�ص���Ч����
        @remark ���������(�߿������)������(��������)
        @remark �������򲻰����Ӷ����޷����������
        @return �ڲ�����
        @version NIIEngine 3.0.0
        */
        inline const CalcRect & getInnerArea() const { return mInnerRect; }

        /** �������UIԪ�ص�ָ������
        @param inner
            true �������UIԪ�ص���Ч����;false �������UIԪ�صĴ�������
        @version NIIEngine 3.0.0
        */
        inline const CalcRect & getArea(bool inner) const { return inner ? mInnerRect : mRect; }

        /** �������Ԫ�صĴ���ü�����
        @remark
            Ҳ���ǰ������(�߿������)������,���������ᱻ������(��Ⱦ��)ʵ�ʵ���Ⱦ
            �������ü�
        @version NIIEngine 3.0.0
        */
        const Rectf & getClipArea() const;

        /** �������UIԪ�ص���Ч�ü�����
        @remark
            ���������(�߿������)������(��������),�����������ᱻ������(��Ⱦ��)ʵ
            �ʵ���Ⱦ�������ü�
        @version NIIEngine 3.0.0
        */
        const Rectf & getInnerClipArea() const;

        /** �������UIԪ�ص�ָ���ü�����
        @param content
            true �������UIԪ�ص���Ч�ü�����;false �������Ԫ�ص�����ü�����
        @version NIIEngine 3.0.0
        */
        inline const Rectf & getClipArea(bool content) const{ return content ? getInnerClipArea() : getClipArea(); }

        /** ��ȡ��Ч�ļ������
        @version NIIEngine 3.0.0
        */
        const Rectf & getIntersectArea() const;

        /** ֪ͨλ���Ѿ��ı���
        @remark
            ����λ�øı�˵����������仯��,��Ⱦͼ�ε�λ�û���ҲӦ��Ҫ�仯
        @note
            �����ݸı�(��)�ŵ������������
        @version NIIEngine 3.0.0
        */
        virtual void notifyPosChange();

        /** �Ƿ��ϴ��α��¼�
        @version NIIEngine 3.0.0
        */
        inline void setSubmitCursorEvent(bool b){ mSubmitCursorEvent = b; }

        /** �Ƿ��ϴ��α��¼�
        @return NIIEngine 3.0.0
        */
        inline bool isSubmitCursorEvent() const{ return mSubmitCursorEvent; }

        /** �����Զ��ظ�����"click"�¼�
        @version NIIEngine 3.0.0
        */
        inline void setCursorMultiClick(bool b){ mMultiClickEventEnable = b;}

        /** �Ƿ��Զ��ظ�����"click"�¼�
        @version NIIEngine 3.0.0
        */
        inline bool isCursorMultiClick() const{ return mMultiClickEventEnable;}

        /** �����Ƿ�洢��һ�ζԽ��Ķ���
        @version NIIEngine 3.0.0
        */
        virtual void setRestorePreFocus(bool b);

        /** ��ȡ�Ƿ�洢��һ�ζԽ��Ķ���
        @version NIIEngine 3.0.0
        */
        virtual bool isRestorePreFocus() const;

        /** �����Ƿ��Զ������Ķ���
        @remark ��һ���Ի���,����Ĭ�ϰ���һЩ��ť,����Щ��ť�����Զ�����
        @version NIIEngine 3.0.0
        */
        void setAutoWindow(bool b);

        /** �����Ƿ��Զ������Ķ���
        @remark ��һ���Ի���,����Ĭ�ϰ���һЩ��ť,����Щ��ť�����Զ�����
        @version NIIEngine 3.0.0
        */
        inline bool isAutoWindow() const{ return mAutoGen; }

        /** ������������Ƿ񱻶Խ�
        @version NIIEngine 3.0.0
        */
        inline bool isFocus() const{ return mSheet->getFocus() == this; }

        /** �������������ϼ��Ƿ񱻶Խ�
        @version NIIEngine 3.0.0
        */
        inline bool isAncestorFocus() const{ return isAncestor(mSheet->getFocus()); }

        /** ����ʵ�ʵĿ�����Ⱦ�Ĵ�С
        @version NIIEngine 3.0.0
        */
        inline PlaneSizei getRenderSize() const{ return mSheet->getArea().getSize(); }

        /** ��������
        @version NIIEngine 3.0.0
        */
        void setFont(Font * obj);

        /** ����ʹ�õ�����
        @version NIIEngine 3.0.0
        */
        Font * getFont(FontID fid = 0) const;

        /** ���û����ı�
        @version NIIEngine 3.0.0
        */
        void setText(const String & t);

        /** ��ȡ�����ı�
        @version NIIEngine 3.0.0
        */
        inline const String & getText() const{ return mText; }

        /** ��������ı�
        @version NIIEngine 3.0.0
        */
        void insertText(const String & text, Nidx pos);

        /** ��������ı�
        @version NIIEngine 3.0.0
        */
        void appendText(const String & text);

        /** ��ȡ�Ӿ��ı�
        @version NIIEngine 3.0.0
        */
        const String & getVisualText() const;

        /** ��ȡ��Ⱦ�ı�
        @version NIIEngine 3.0.0
        */
        const PixelUnitGrid * getRenderText() const;

        /** �����Ƿ�ʹ���ı��Ű���
        @version NIIEngine 3.0.0
        */
        void setTextLayout(bool b);

        /** ��ȡ�Ƿ�ʹ���ı��Ű���
        @version NIIEngine 3.0.0
        */
        inline bool isTextLayout() const{ return mTextLayout;}

        /** ����˫���ı�
        @version NIIEngine 3.0.0
        */
        inline const BidiText * getBidiText() const { return mBidiText; }

        /** �����ı��Ű���
        @version NIIEngine 3.0.0
        */
        void setTextView(TextView * obj);

        /** ��ȡ�ı��Ű���
        @version NIIEngine 3.0.0
        */
        inline TextView * getTextView() const{ return mTextView; }

        /** ��ȡʵ��ʹ�õ��Ű���
        @version NIIEngine 3.0.0
        */
        virtual TextView * getRenderTextView() const;

        /**
        @version NIIEngine 3.0.0
        */
        inline KeyboardControlItem * getKeyboardItem() const { return mC0; }

        /**
        @version NIIEngine 3.0.0
        */
        inline MouseControlItem * getMouseItem() const { return mC1; }
        
        /**
        @version NIIEngine 3.0.0
        */
        inline TouchControlItem * getTouchItem() const { return mC2; }

        /** ���������Ƿ�����л�
        @version NIIEngine 3.0.0
        */
        inline void setDataWrite(bool set) { mDataWrite = set;}

        /** ���������Ƿ�����л�
        @version NIIEngine 3.0.0
        */
        inline bool isDataWrite() const{ return mDataWrite;}

        /** ���л���������
        @param[in] out ������
        @version NIIEngine 3.0.0
        */
        virtual void write(XmlSerializer * out) const;

        /** �������л�����
        @version NIIEngine 3.0.0
        */
        void setWrite(PropertyID pid, bool b);

        /** �Ƿ����л���������
        @version NIIEngine 3.0.0
        */
        bool isWrite(PropertyID pid) const;

        /** �����Ƿ���Ĭ��ֵ
        @version NIIEngine 3.0.0
        */
        bool isDefault(const PropertyCmd * obj) const;
    public:
        /** �Խ�UI��Ԫ
        @version NIIEngine 3.0.0
        */
        bool focus();

        /** UI��Ԫʧ��
        @version NIIEngine 3.0.0
        */
        void lost();

        /** �������ڵĴ���
        @return
            UI��Ԫ���ϼ�����������,��������һ���Ǵ������,��������������ض�������
            ĳ������.
        @version NIIEngine 3.0.0
        */
        Window * getRoot() const;

        /** ��ȡ��������
        @version NIIEngine 3.0.0
        */
        inline GeometryPixel * getPixelData() const{ return mPixelData; }

        /** ����Ԫ�ص����ش�С
        @param pixelAlign �Ƿ����ͻ���С
        @version NIIEngine 3.0.0
        */
        PlaneSizef calcArea(bool pixelAlign = true) const;

        /** ��ȡ������Ԫ�ص����ش�С
        @param pixelAlign �Ƿ����ͻ���С
        @version NIIEngine 3.0.0
        */
        PlaneSizef calcParentArea(bool pixelAlign = true) const;

        /** ˢ����Ⱦ
        @param[in] r �Ƿ񺬸��¼�
        @version NIIEngine 3.0.0
        */
        void refresh(bool r = false);

        /** ִ����ʽ�Ű�
        @version NIIEngine 3.0.0
        */
        virtual void layout();

        /** ִ�и���
        @version NIIEngine 3.0.0
        */
        virtual bool onCopyData(Clipboard & clipboard);

        /** ִ�м���
        @version NIIEngine 3.0.0
        */
        virtual bool onCutData(Clipboard & clipboard);

        /** ִ��ճ��
        @version NIIEngine 3.0.0
        */
        virtual bool onPasteData(Clipboard & clipboard);
    public:
        /** id�仯�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID IDEvent;

        /** ���ֱ仯�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID NameEvent;

        /** �ߴ�仯�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID SizeEvent;

        /** ��������
        @version NIIEngine 3.0.0
        */
        static const EventID MarginModeEvent;

        /** �ϼ��ߴ�仯�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID ParentSizeEvent;

        /** �ƶ��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID MoveEvent;

        /** ͸���ȱ仯�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID AlphaEvent;

        /** ����͸���ȱ仯�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID EnvAlphaEvent;

        /** ����ת�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID RotateEvent;

        /** �����¼�
        @version NIIEngine 3.0.0
        */
        static const EventID UpdataEvent;

        /** �״̬�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID ActivateEvent;

        /** ���״̬�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID DeactivateEvent;

        /** ��ʾ״̬�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID ShowEvent;

        /** ����ʾ״̬�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID HideEvent;

        /** �����¼�
        @version NIIEngine 3.0.0
        */
        static const EventID EnableEvent;

        /** �����¼�
        @version NIIEngine 3.0.0
        */
        static const EventID DisableEvent;

        /** ˮƽ����ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID HAlignModeEvent;

        /** ��ֱ����ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID VAlignModeEvent;

        /** �Խ��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID FocusEvent;

        /** ʧ���¼�
        @version NIIEngine 3.0.0
        */
        static const EventID LostEvent;

        /** �Զ��ü��ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID AutoClipModeEvent;

        /** �Զ�ɾ���ı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID AutoDestroyModeEvent;

        /** ��Ⱦ��ʼ�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID RenderBeginEvent;

        /** ��Ⱦ�����¼�
        @version NIIEngine 3.0.0
        */
        static const EventID RenderEndEvent;

        /** ʵ�ʵ���Ⱦ��ı�(��)����
        @version NIIEngine 3.0.0
        */
        static const EventID RenderCtxEvent;

        /** �ö����øı��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID TopModeEvent;

        /** ��仯�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID ZOrderEvent;

        /** ɾ����ʼ�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID DestroyStartEvent;

        /** һ��UI��Ԫ���Ӧ�õ����UI����(��)����
        @version NIIEngine 3.0.0
        */
        static const EventID ViewAttachEvent;

        /** һ��UI��Ԫ�������UI������ʧЧ(��)����
        @version NIIEngine 3.0.0
        */
        static const EventID ViewDetachEvent;

        /** ���Ӧ�õ����UI����(��)����
        @version NIIEngine 3.0.0
        */
        static const EventID StyleAttachEvent;

        /** �������UI������ʧЧ(��)����
        @version NIIEngine 3.0.0
        */
        static const EventID StyleDetachEvent;

        /** ��Ч״̬�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID InvalidEvent;

        /** ����ı�ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID FontEvent;

        /** �ı��ı�ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID TextEvent;

        /** �ı��Ű�ı�ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID TextLayoutModeEvent;

        /** �α�����¼�
        @version NIIEngine 3.0.0
        */
        static const EventID CursorEnterEvent;

        /** �α��Ƴ��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID CursorLeaveEvent;

        /** �α��ƶ��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID CursorMoveEvent;

        /** �α���ק�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID CursorDragEvent;

        /** �α괥���¼�
        @version NIIEngine 3.0.0
        */
        static const EventID CursorTouchEvent;

        /** �α���ش����¼�
        @version NIIEngine 3.0.0
        */
        static const EventID CursorMultiTouchEvent;

        /** ���������¼�
        @version NIIEngine 3.0.0
        */
        static const EventID ButtonDownEvent;

        /** �����ͷ��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID ButtonUpEvent;

        /** ���̰����¼�
        @version NIIEngine 3.0.0
        */
        static const EventID KeyDownEvent;

        /** �����ͷ��¼�
        @version NIIEngine 3.0.0
        */
        static const EventID KeyUpEvent;

        /** �ı������¼�
        @version NIIEngine 3.0.0
        */
        static const EventID CharEvent;

        /** �¼�����
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;

        /** �ڲ��¼�
        @version NIIEngine 3.0.0
        */
        static const PropertyID InnerPropertyLimit;
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dest);

        /** �Ƿ�����
        @remark һ����ָ�Ƿ�ƥ��
        @version NIIEngine 3.0.0
        */
        virtual bool validImpl(const WidgetView * view) const;

        /** ��ȡʵ��UI����
        @param[out] ctx UI����
        @version NIIEngine 3.0.0
        */
        void getContext(SheetContext & ctx) const;

        /** ������Ⱦ
        @version NIIEngine 3.0.0 �߼�API
        */
        virtual void queuePixelImpl();

        /** ����ʵ��
        @version NIIEngine 3.0.0
        */
        virtual void cloneImpl(Widget * dest, bool r) const;

        /** ����ʵ��
        @param[in] pos ���λ��
        @param[in] size ��Դ�С
        @param[in] event �Ƿ񴥷��¼�
        @version NIIEngine 3.0.0
        */
        virtual void setAreaImpl(const RelVector2f & pos, const RelPlaneSizef & size, bool event = true);

        /** ִ�и���
        @remark ��ʽ/λ�ö����ȸ���
        @param cost �ϴε��öɹ���ʱ��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void updateImpl(TimeDurMS cost);

        /** ������Ⱦ����
        @remark λ��/�ü��������
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void updateImpl(SheetContext * rc);

        /** �����Ⱦ����
        @remark һ�����ڲ��ṹ�����仯��Ҫ���
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void renderImpl();

        /** ���ƶ�ʵ��
        @version NIIEngine 3.0.0
        */
        virtual void ZOrderImpl();

        /** �Ƶ�ͬ��Χ����ǰ��
        @return ����ͬ��Χ
        @param[in] click
        @version NIIEngine 3.0.0
        */
        virtual bool moveFrontImpl(bool click);

        /** ��ȡ�ڲ�����ʵ��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual Rectf getAreaImpl(bool pixelAlign) const;

        /** ��ȡ��������ʵ��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual Rectf getInnerAreaImpl(bool pixelAlign) const;

        /** �ü��������仯
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void notifyClipChange();

        /** ��ȡ�ڲ��ü�����ʵ��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual Rectf getClipAreaImpl() const;

        /** ��ȡ�ڲ��ü�����ʵ��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual Rectf getClipInnerAreaImpl() const;

        /** ��Ч�Ľ�������ʵ��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual Rectf getIntersectAreaImpl() const;

        /** ��ȡ��ȡʵ��UI����ʵ��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void getContextImpl(SheetContext & out) const;

        /** ˢ��ʵ��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void refreshImpl(bool r);

        /** ˢ����Ⱦ��
        @version NIIEngine 3.0.0 �߼�API
        */
        virtual void refreshFaceImpl();

        /** �����л�����
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void disableWrite();

        /** д�뵽������
        @param[in] out ������
        */
        virtual NCount writeImpl(XmlSerializer * out) const;
    public:
        /** ���UI�������ⲿ��������ʱ����
        @remark �ⲿ�¼���
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void onBeginInit();

        /** ���UI�������ⲿ���ݽ�������ʱ����
        @remark �ⲿ�¼���
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void onEndInit();
    protected:
        /** ID�ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onID(const WidgetEventArgs * arg);

        /** ���ָı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onName(const WidgetEventArgs * arg);

        /** �ߴ�ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onSize(const WidgetEventArgs * arg);

        /** ������ߴ�ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onParentSize(const WidgetEventArgs * arg);

        /** ��Ԫ�ص�λ�øı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onMove(const WidgetEventArgs * arg);

        /** ˮƽ���뷽ʽ�ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onHAlignMode(const WidgetEventArgs * arg);

        /** ��ֱ���뷽ʽ�ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onVAlignMode(const WidgetEventArgs * arg);

        /** ����ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onMargin(const WidgetEventArgs * arg);

        /** ͸���ȸı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onAlpha(const WidgetEventArgs * arg);

        /** ����͸���ȸı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onEnvAlpha(const WidgetEventArgs * arg);

        /** ��ʾ״̬�ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onShow(const WidgetEventArgs * arg);

        /** ����״̬�ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onHide(const WidgetEventArgs * arg);

        /** ����״̬�ı䴥��
        @version NIIEngine 3.0.0
        */
        virtual void onEnable(const WidgetEventArgs * arg);

        /** ����״̬�ı䴥��
        @version NIIEngine 3.0.0
        */
        virtual void onDisable(const WidgetEventArgs * arg);

        /** �ü��ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onClipMode(const WidgetEventArgs * arg);

        /** �ö����øı�ʱ����
        @versioin NIIEngine 3.0.0
        */
        virtual void onTopMode(const WidgetEventArgs * arg);

        /** ��ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onZOrder(const WidgetEventArgs * arg);

        /** �Զ�ɾ���ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onAutoDestroyMode(const WidgetEventArgs * arg);

        /** �Խ�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onFocus(const WidgetEventArgs * arg);

        /** ʧ��ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onLost(const WidgetEventArgs * arg);

        /** ˢ��ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onRefresh(const WidgetEventArgs * arg);

        /** ��Ⱦǰ����
        @version NIIEngine 3.0.0
        */
        virtual void onPreRender(const WidgetEventArgs * arg);

        /** ��Ⱦ�󴥷�
        @version NIIEngine 3.0.0
        */
        virtual void onRenderEnd(const WidgetEventArgs * arg);

        /** ɾ��ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onDestroy(const WidgetEventArgs * arg);

        /** ���ӵ�Ԫ���ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onViewAttach(const WidgetEventArgs * arg);

        /** �����Ԫ���ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onViewDetach(const WidgetEventArgs * arg);

        /** ���̴���
        @version NIIEngine 3.0.0
        */
        virtual void onSheet(const SheetEventArgs * arg);

        /** �״̬ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onActivate(const ActivationEventArgs * arg);

        /** �ǻ״̬ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onInactivate(const ActivationEventArgs * arg);

        /** ���ӷ��ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onStyleAttach(const StyleEventArgs * arg);

        /** ������ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onStyleDetach(const StyleEventArgs * arg);

        /** ���巢���ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onFont(const FontEventArgs * arg);

        /** �ı������ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onText(const WidgetEventArgs * arg);

        /** Bidi�����ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onTextLayoutMode(const WidgetEventArgs * arg);

        /** �α����ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onCursorEnter(const CursorEventArgs * arg);

        /** �α��뿪�󴥷�
        @version NIIEngine 3.0.0
        */
        virtual void onCursorLeave(const CursorEventArgs * arg);

        /** �α��ƶ�ʱ����
        @remark �α����ʱ
        @version NIIEngine 3.0.0
        */
        virtual void onCursorMove(const CursorEventArgs * arg);

        /** �α���ק����
        @rmark ����״̬/����״̬
        @version NIIEngine 3.0.0
        */
        virtual void onCursorDrag(const CursorEventArgs * arg);

        /** ��������ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onButtonDown(const CursorEventArgs * arg);

        /** �����ͷ�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onButtonUp(const CursorEventArgs * arg);

        /** �α갴��ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onCursorTouch(const CursorEventArgs * arg);

        /** �α���ذ���ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onCursorMultiTouch(const CursorEventArgs * arg);

        /** ���̰���ʱ����
        @param arg
        @version NIIEngine 3.0.0
        */
        virtual void onKeyDown(const KeyEventArgs * arg);

        /** �����ͷ�ʱ����
        @param arg
        @version NIIEngine 3.0.0
        */
        virtual void onKeyUp(const KeyEventArgs * arg);

        /** �ı�����ʱ����
        @param arg
        @version NIIEngine 3.0.0
        */
        virtual void onChar(const KeyEventArgs * arg);
    protected:
        Widget(const Widget &);

        Widget & operator=(const Widget &);
    protected:
        typedef NII::set<PropertyID>::type NonSerlProperty;
    protected:
        Container * mParent;
        String mText;
        WidgetID mID;
        String mName;
        FactoryID mType;
        StyleID mStyleID;                   ///< UI���ID
        WidgetView * mWidgetModel;
        GeometryPixel * mPixelData;
        RelRectf mArea;
        NIIf mAlpha;
        RefreshMode mRefreshMode;
        Font * mFont;
        BidiText * mBidiText;
        TextView * mTextView;
        Widget * mPreFocus;
        HAlign mHAlign;
        VAlign mVAlign;
        RelRectf mMargin;
        RelPlaneSizef mMinSize;
        RelPlaneSizef mMaxSize;
        AspectMode mScaleMode;              ///< ��ƥ���Сʱ������ģʽ
        NIIf mScaleFactor;                  ///< ��ƥ���Сʱ����������
        PlaneSizef mPixelSize;              ///< ���ش�С
        Quaternion mRot;
        CalcRect mRect;                     ///< �����С(��Ļλ��)
        CalcRect mInnerRect;                ///< �������С(��Ļλ��)
        mutable PixelUnitGrid mRenderText;
        NonSerlProperty mNonSerlProperty;
        CursorWidgetAide * mAide1;
        TooltipWidgetAide * mAide2;
        KeyboardControlItem * mC0;
        MouseControlItem * mC1;
        TouchControlItem * mC2;
        UISheet * mSheet;                   ///< ����UI���廭��
        Nmark mSpaceMark;
        mutable Rectf mClipRect;            ///< �ü������С(��Ļλ��)
        mutable Rectf mClipInnerRect;       ///< �ڲü������С(��Ļλ��)
        mutable Rectf mIntersectRect;       ///< �ཻ��������(��Ļλ��)
        String mTipsText;
        bool mParentTipsText;
        bool mInit;
        bool mDestroy;
        bool mRedraw;                       ///< �Ƿ���Ҫ���¼���/���Ƽ���
        bool mMultiClickEventEnable;        ///< �Ƿ����˫������¼�
        bool mSubmitCursorEvent;            ///< �ϴ��α��¼�
        mutable bool mValidClipRect;        ///< �ü������С(�Ƿ�)��Ч
        mutable bool mValidClipInnerRect;   ///< �ڲü������С(�Ƿ�)��Ч
        mutable bool mValidIntersectRect;   ///< �ཻ��������(�Ƿ�)��Ч
        bool mZOrder;                       ///< ��ο��Ըı�
        bool mZOrderClick;                  ///< ����¼��ܸı��������Ĳ��
        bool mRestorePreFocus;              ///< �Ƿ�洢ǰһ�����Խ��Ķ���
        bool mTop;                          ///< �����
        bool mPixelAlign;                   ///< ����������
        bool mAutoGen;                      ///< ϵͳ�Զ�������Ԫ��
        bool mEnable;                       ///< �Ƿ�����
        bool mVisible;                      ///< �Ƿ�ɼ�
        bool mActive;                       ///< �Ƿ���
        bool mEnvAlpha;                     ///< �Ƿ�Ӹ������м̳�͸����
        bool mDataWrite;                    ///< ���л�����
        bool mAutoDestroy;                  ///< �Ƿ��Զ�ɾ��
        bool mAutoClip;                     ///< �Զ��ü�
        bool mTextLayout;
        mutable bool mValidText;
        mutable bool mValidBidi;
    };
    typedef vector<Widget *>::type Widgets;
}
}
#endif