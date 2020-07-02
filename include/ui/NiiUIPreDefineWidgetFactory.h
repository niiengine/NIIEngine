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

#ifndef _NII_UI_PreDefineWidgetFactory_H_
#define _NII_UI_PreDefineWidgetFactory_H_

#include "NiiUIWidgetFactory.h"

#define N_WidgetFactory(c, id) PreDefineWidgetFactory<c>(id)

namespace NII
{
namespace UI
{
    /** 预定制UI单元工厂类
    @version NIIEngine 3.0.0
    */
    template <typename T> class PreDefineWidgetFactory : public WidgetFactory
    {
    public:
        PreDefineWidgetFactory(FactoryID fid);

        /// @copydetails WidgetFactory::getID
        FactoryID getID() const;

        /// @copydetails WidgetFactory::create
        Widget * create(WidgetID wid, Container * own);

        /// @copydetails WidgetFactory::destroy
        void destroy(Widget * obj);

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
    //------------------------------------------------------------------------
    template <typename T> PreDefineWidgetFactory<T>::PreDefineWidgetFactory(FactoryID fid) :
        mID(fid)
    {
    }
    //------------------------------------------------------------------------
    template <typename T> FactoryID PreDefineWidgetFactory<T>::getID() const
    {
        return mID;
    }
    //------------------------------------------------------------------------
    template <typename T> Widget * PreDefineWidgetFactory<T>::create(WidgetID wid, Container * own)
    {
        return N_new T(wid, mID, own);
    }
    //------------------------------------------------------------------------
    template <typename T> void PreDefineWidgetFactory<T>::destroy(Widget * obj)
    {
        N_delete obj;
    }
    //------------------------------------------------------------------------
    template <typename T> void PreDefineWidgetFactory<T>::plugin()
    {
        WidgetFactory * factory = N_new PreDefineWidgetFactory<T>(mID);
        if(N_OnlyPtr(UIWidget))
        {
            if (N_Only(UIWidget).addFactory(factory) != true)
            {
                N_delete factory;
            }
        }
    }
    //------------------------------------------------------------------------
    template <typename T> void PreDefineWidgetFactory<T>::unplugin()
    {
        if(N_OnlyPtr(UIWidget))
        {
            WidgetFactory * factory = N_Only(UIWidget).getFactory(mID);
            if(factory != 0)
            {
                N_Only(UIWidget).removeFactory(mID);
                N_delete factory;
            }
        }
    }
    //------------------------------------------------------------------------
}
}
#endif