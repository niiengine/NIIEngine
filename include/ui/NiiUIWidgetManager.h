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

#ifndef _NII_UI_WIDGET_MANAGER_H_
#define _NII_UI_WIDGET_MANAGER_H_

#include "NiiUIPreInclude.h"
#include "NiiSingleton.h"
#include "NiiUIWidget.h"
#include "NiiUIException.h"
#include "NiiUIStyle.h"
#include "NiiUIPixelUnitGrid.h"

using namespace NII::UI;

namespace NII
{
    /** UI��Ԫ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI WidgetManager : public Singleton<WidgetManager>, public UIAlloc
    {
        friend class UIManager;
    public:
        /** �����
        @version NIIEngine 3.0.0
        */
        struct _EngineAPI StyleView
        {
            String mName;
            StyleViewID mID;
            GroupID mGroup;
            FactoryID mModel;
            StyleID mStyle;
            WidgetViewlID mView;
            EffectID mEffect;
        };

        typedef map<FactoryID, WidgetViewFactory *>::type WidgetViewList;
        typedef map<FactoryID, WidgetFactory *>::type WidgetFactoryList;
        typedef map<StyleViewID, StyleView>::type StyleViewList;
        typedef map<StyleID, Style *>::type StyleList;
    public:
        WidgetManager();
        ~WidgetManager();

        /** ������Դ��
        @version NIIEngine 3.0.0
        */
        static inline void setGroup(GroupID gid)    { ResourceGroup = gid; }

        /** ��ȡ��Դ��
        @version NIIEngine 3.0.0
        */
        static inline GroupID getGroup()            { return ResourceGroup; }

        /**
        @version NIIEngine 3.0.0
        */
        static inline void setStyleGroup(GroupID gid) { StyleResourceGroup = gid; }

        /**
        @version NIIEngine 3.0.0
        */
        static inline GroupID getStyleGroup()       { return StyleResourceGroup; }

        /** ����UI��Ԫ
        @version NIIEngine 3.0.0
        */
        Widget * create(StyleViewID id, WidgetID wid = 0, Container * parent = 0, FactoryID modelhint = 0);

        /** ����UI��Ԫ
        @version NIIEngine 3.0.0
        */
        Widget * create(GroupID gid, FactoryID mid, WidgetViewlID vid, WidgetID wid = 0, Container * parent = 0);
        
        /** �Ƿ����UI��Ԫ
        @version NIIEngine 3.0.0
        */
        bool isExist(const Widget * obj) const;

        /** �Ƿ����UI��Ԫ
        @version NIIEngine 3.0.0
        */
        bool isExist(const String & name, bool path = false) const;

        /** ɾ��UI��Ԫ
        @version NIIEngine 3.0.0
        */
        void destroy(Widget * obj);

        /** ɾ��UI��Ԫ
        @version NIIEngine 3.0.0
        */
        void destroyAll();

        /** ��ӹ�����
        @version NIIEngine 3.0.0
        */
        bool addFactory(WidgetFactory * obj);

        /** ��ȥ������
        @version NIIEngine 3.0.0
        */
        void removeFactory(FactoryID fid);

        /** ��ȡ������
        @version NIIEngine 3.0.0
        */
        WidgetFactory * getFactory(FactoryID fid) const;

        /** �������Ƿ����
        @version NIIEngine 3.0.0
        */
        bool isFactoryExist(FactoryID fid) const;

        /** ��ȥ���й�����
        @version NIIEngine 3.0.0
        */
        void removeAllFactory();

        /** ��ȡ�����б�
        @version NIIEngine 3.0.0 �߼� api
        */
        const WidgetFactoryList & getWidgetFactoryList() const  { return mWidgetFactoryList;  }

        /** ���UIģ�͹���
        @version NIIEngine 3.0.0
        */
        bool addViewFactory(WidgetViewFactory * obj);

        /** ��ȥUIģ�͹���
        @version NIIEngine 3.0.0
        */
        void removeViewFactory(FactoryID fid);

        /** ��ȡUIģ�͹���
        @version NIIEngine 3.0.0
        */
        WidgetViewFactory * getViewFactory(FactoryID fid) const;

        /** �Ƿ����UIģ�͹���
        @version NIIEngine 3.0.0
        */
        bool isViewFactoryExist(FactoryID fid) const;

        /** �����ļ�
        @version NIIEngine 3.0.0
        */
        void loadStyle(const String & file, GroupID gid = GroupUnknow);

        /** �����ڴ���
        @version NIIEngine 3.0.0
        */
        void loadStyle(const MemStream * data);

        /** ��ӷ��
        @version NIIEngine 3.0.0
        */
        void addStyle(Style * style);

        /** ��ȥ���
        @version NIIEngine 3.0.0
        */
        void removeStyle(StyleID style);

        /** ��ȥ���з��
        @version NIIEngine 3.0.0
        */
        void removeAllStyle();

        /** �Ƿ���ڷ��
        @version NIIEngine 3.0.0
        */
        bool isStyleExist(StyleID style) const;

        /** ��ȡ���
        @version NIIEngine 3.0.0
        */
        Style * getStyle(StyleID style) const;

        /** ��ȡ����б�
        @version NIIEngine 3.0.0
        */
        inline const StyleList & getStyleList() const           { return mStyleList; }

        /** ����UI��ͼ
        @version NIIEngine 3.0.0
        */
        WidgetView * createView(FactoryID wid);

        /** ɾ��UI��ͼ
        @version NIIEngine 3.0.0
        */
        void destroyView(WidgetView * obj);

        /** ��ӷ����
        @version NIIEngine 3.0.0
        */
        void addStyleView(StyleViewID id, FactoryID mid, WidgetViewlID vid, StyleID sid, EffectID eid, const String & name = N_StrBlank);

        /** ������Ƿ����
        @version NIIEngine 3.0.0
        */
        bool isStyleViewExist(FactoryID mid, WidgetViewlID vid, StyleID sid) const;

        /** ������Ƿ����
        @version NIIEngine 3.0.0
        */
        bool isStyleViewExist(StyleViewID fid) const;

        /** ��ȡ�����
        @version NIIEngine 3.0.0
        */
        const StyleView * getStyleView(FactoryID mid, WidgetViewlID vid, StyleID sid) const;

        /** ��ȡ�����
        @version NIIEngine 3.0.0
        */
        const StyleView * getStyleView(StyleViewID fid) const;

        /** ��ȥ�����
        @version NIIEngine 3.0.0
        */
        void removeStyleView(StyleViewID fid);

        /** ��ȥ���з����
        @version NIIEngine 3.0.0
        */
        void removeAllStyleView();

        /** ��ȡ����б�
        @version NIIEngine 3.0.0
        */
        const StyleViewList & getStyleViewList() const          { return mStyleViewList; }

        /** ����
        @note xml��
        @version NIIEngine 3.0.0
        */
        Widget * load(const String & file, GroupID gid = GroupUnknow);

        /** ����
        @note xml��
        @version NIIEngine 3.0.0
        */
        Widget * load(const MemStream * source);

        /** ��ȡ�б�
        @version NIIEngine 3.0.0 �߼�api
        */
        const Widgets & getList() const                         { return mCreateWidgets; }

        /** д��UI��Ԫ
        @param[in] out �����
        @note xml��
        @version NIIEngine 3.0.0
        */
        void writeWidget(const Widget * obj, Nostream & out) const;

        /** д��UI��Ԫ
        @param[in] file �ļ���
        @note xml��
        @version NIIEngine 3.0.0
        */
        void writeWidget(const Widget * obj, const String & file) const;

        /** д��UI���
        @param[out]
        @version NIIEngine 3.0.0
        */
        void writeStyle(StyleID style, Nostream & out) const;

        /** д��UI���
        @param[out] out
        @version NIIEngine 3.0.0
        */
        void writeStyleList(Nui16 Ser, Nostream & out) const;

        /** ����
        @version NIIEngine 3.0.0
        */
        void recover();

        /** ��ȡδʹ�õ�ID
        @version NIIEngine 3.0.0 �߼�api
        */
        WidgetID genValidWidgetID();

        /** ��ȡδʹ�õ�ID
        @version NIIEngine 3.0.0 �߼�api
        */
        StyleViewID genValidStyleViewID();
    public:
        /** ����Tooltip
        @version NIIEngine 3.0.0
        */
        static TooltipWidgetAide * mTooltip;
        
        /** ����Cursor
        @version NIIEngine 3.0.0
        */
        static CursorWidgetAide * mCursor;
    protected:
        /** ������Ԫʱ
        @version NIIEngine 3.0.0 �߼�api
        */
        void onCreate(const Widget * obj);
        
        /** ɾ����Ԫʱ
        @version NIIEngine 3.0.0 �߼�api
        */
        void onDestroy(const Widget * obj);
    public:
        static const Colour DefaultTextColour;
        static const Colour DefaultSelectionColour;

        static EffectTextView * gDefaultTextView;
        static TextView * gNormalTextView;
    private:
        static GroupID StyleResourceGroup;
        StyleList mStyleList;
        Widgets mCreateWidgets;
        Widgets mDestroyWidgets;
        WidgetFactoryList mWidgetFactoryList;
        StyleViewList mStyleViewList;
        WidgetViewList mWidgetViewList;
        WidgetID mValidWidgetID;
        WidgetID mValidStyleViewID;
        static GroupID ResourceGroup;
    };
}
#endif