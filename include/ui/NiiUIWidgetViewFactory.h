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

#ifndef _NII_WidgetModelFactory_H_
#define _NII_WidgetModelFactory_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidget.h"

#define N_WidgetModeFactory(c, id) TWidgetViewFactory<c>(id)

namespace NII
{
namespace UI
{
    /** 单元风格工厂类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI WidgetViewFactory : public FactoryAlloc
    {
    public:
        WidgetViewFactory(){}
        virtual ~WidgetViewFactory() {}

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        virtual FactoryID getID() const = 0;

        /** 创建单元风格
        @version NIIEngine 3.0.0
        */
        virtual WidgetView * create() = 0;

        /** 删除单元风格
        @version NIIEngine 3.0.0
        */
        virtual void destroy(WidgetView * obj) = 0;

    };
    
    /** 预定制UI风格工厂类
    @version NIIEngine 3.0.0
    */
    template <typename T> class TWidgetViewFactory : public WidgetViewFactory
    {
    public:
        TWidgetViewFactory(FactoryID fid);

        ///@copydetails WidgetViewFactory::getID
        FactoryID getID() const { return mID; }

        ///@copydetails WidgetViewFactory::create
        WidgetView * create();

        ///@copydetails WidgetViewFactory::destroy
        void destroy(WidgetView * wr);

        /** 注入到系统中
        @version NIIEngine 3.0.0
        */
        void plugin();

        /** 从系统中移出
        @version NIIEngine 3.0.0
        */
        void unplugin();
    protected:
        FactoryID mID;
    };
    //-------------------------------------------------------------------------
    template <typename T> TWidgetViewFactory<T>::TWidgetViewFactory(FactoryID fid) :
        mID(fid){}
    //-------------------------------------------------------------------------
    template <typename T> WidgetView * TWidgetViewFactory<T>::create()
    {
        return N_new T(mID);
    }
    //-------------------------------------------------------------------------
    template <typename T> void TWidgetViewFactory<T>::destroy(WidgetView * obj)
    {
        N_delete obj;
    }
    //-------------------------------------------------------------------------
    template <typename T> void TWidgetViewFactory<T>::plugin()
    {
        WidgetViewFactory * factory = N_new TWidgetViewFactory<T>(mID);

        if(N_OnlyPtr(UIWidget))
        {
            if (N_Only(UIWidget).addModelFactory(factory) != true)
            {
                N_delete factory;
            }
        }
    }
    //-------------------------------------------------------------------------
    template <typename T> void TWidgetViewFactory<T>::unplugin()
    {
        if(N_OnlyPtr(UIWidget))
        {
            WidgetViewFactory * factory = N_Only(UIWidget).getModelFactory(mID);
            if(factory != 0)
            {
                N_Only(UIWidget).removeModelFactory(mID);
                N_delete factory;
            }
        }
    }
    //------------------------------------------------------------------------
}
}
#endif