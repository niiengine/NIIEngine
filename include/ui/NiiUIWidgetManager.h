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
#include "NiiUIWidgetView.h"
#include "NiiUIWidgetFactory.h"
#include "NiiUIWidgetModelFactory.h"

using namespace NII::UI;

namespace NII
{
    /** UI��Ԫ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UIWidgetManager : public Singleton<UIWidgetManager>, public UIAlloc
    {
        friend class UIManager;
    public:
        /** �����
        @version NIIEngine 3.0.0
        */
        struct _EngineAPI WidgetModelDefine
        {
            StyleID mStyle;
            EffectID mEffect;
            FactoryID mFactory;
            WidgetModelID mWidgetModel;
        };

        typedef map<FactoryID, WidgetModelFactory *>::type WidgetModelList;
        typedef    map<FactoryID, WidgetFactory *>::type WidgetFactoryList;
        typedef map<FactoryID, WidgetModelDefine>::type StyleDefineList;
    public:
        UIWidgetManager();
        ~UIWidgetManager();

        /** ������Դ��
        @version NIIEngine 3.0.0
        */
        static void setGroup(GroupID gid);

        /** ��ȡ��Դ��
        @version NIIEngine 3.0.0
        */
        static GroupID getGroup();

        /** ����UI��Ԫ
        @version NIIEngine 3.0.0
        */
        Widget * create(FactoryID sid, WidgetID wid = 0, Container * parent = 0);
        
        /** �Ƿ����UI��Ԫ
        @version NIIEngine 3.0.0
        */
        bool isExist(const Widget * obj) const;

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
        const WidgetFactoryList & getWidgetFactoryList() const;

        /** ��ȡ����б�
        @version NIIEngine 3.0.0
        */
        const StyleDefineList & getStyleDefineList() const;

        /** ���UIģ�͹���
        @version NIIEngine 3.0.0
        */
        bool addModelFactory(WidgetModelFactory * obj);

        /** ��ȥUIģ�͹���
        @version NIIEngine 3.0.0
        */
        void removeModelFactory(FactoryID fid);

        /** ��ȡUIģ�͹���
        @version NIIEngine 3.0.0
        */
        WidgetModelFactory * getModelFactory(FactoryID fid) const;

        /** �Ƿ����UIģ�͹���
        @version NIIEngine 3.0.0
        */
        bool isModelFactoryExist(FactoryID fid) const;

        /** ����UIģ��
        @version NIIEngine 3.0.0
        */
        WidgetView * createModel(FactoryID wid);

        /** ɾ��UIģ��
        @version NIIEngine 3.0.0
        */
        void destroyModel(WidgetView * obj);

        /** ��ӷ����
        @version NIIEngine 3.0.0
        */
        void addStyle(FactoryID fid, StyleID sid, WidgetModelID wsid, EffectID eid);

        /** ������Ƿ����
        @version NIIEngine 3.0.0
        */
        bool isStyleExist(FactoryID fid) const;

        /** ��ȡ�����
        @version NIIEngine 3.0.0
        */
        const WidgetModelDefine & getStyle(FactoryID fid) const;

        /** ��ȥ�����
        @version NIIEngine 3.0.0
        */
        void removeStyle(FactoryID fid);

        /** ��ȥ���з����
        @version NIIEngine 3.0.0
        */
        void removeAllStyle();

        /** ����
        @note xml��
        @version NIIEngine 3.0.0
        */
        Widget * load(const String & file, GroupID gid = GroupUnknow);

        /** ����
        @note xml��
        @version NIIEngine 3.0.0
        */
        Widget * load(const MemDataStream * source);

        /** ����
        @param[in] out �����
        @note xml��
        @version NIIEngine 3.0.0
        */
        void save(const Widget & obj, Nostream & out) const;

        /** ����
        @param[in] file �ļ���
        @note xml��
        @version NIIEngine 3.0.0
        */
        void save(const Widget & obj, const String & file) const;

        /** ����
        @version NIIEngine 3.0.0
        */
        void recover();

        /** ��ȡδʹ�õ�ID
        @version NIIEngine 3.0.0 �߼�api
        */
        WidgetID genValidID();

        /// @copydetails Singleton::getOnly
        static UIWidgetManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static UIWidgetManager * getOnlyPtr();
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
    private:
        Widgets mCreateWidgets;
        Widgets mDestroyWidgets;
        WidgetFactoryList mWidgetFactoryList;
        StyleDefineList mStyleDefineList;
        WidgetModelList mWidgetModelList;
        WidgetID mValidWidgetID;
        static GroupID ResourceGroup;
    };
}
#endif