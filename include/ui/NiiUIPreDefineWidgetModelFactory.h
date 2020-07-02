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

#ifndef _NII_PreDefineWidgetModelFactory_H_
#define _NII_PreDefineWidgetModelFactory_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidgetModelFactory.h"
#include "NiiUIWidget.h"

#define N_WidgetModeFactory(c, id) PreDefineWidgetModelFactory<c>(id)

namespace NII
{
namespace UI
{
    /** 预定制UI风格工厂类
    @version NIIEngine 3.0.0
    */
    template <typename T> class PreDefineWidgetModelFactory : public WidgetModelFactory
    {
    public:
        PreDefineWidgetModelFactory(FactoryID fid);

        ///@copydetails WidgetModelFactory::getID
        FactoryID getID() const { return mID; }

        ///@copydetails WidgetModelFactory::create
        WidgetView * create();

        ///@copydetails WidgetModelFactory::destroy
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
    template <typename T> PreDefineWidgetModelFactory<T>::PreDefineWidgetModelFactory(FactoryID fid) :
        mID(fid){}
    //-------------------------------------------------------------------------
    template <typename T> WidgetView * PreDefineWidgetModelFactory<T>::create()
    {
        return N_new T(mID);
    }
    //-------------------------------------------------------------------------
    template <typename T> void PreDefineWidgetModelFactory<T>::destroy(WidgetView * obj)
    {
        N_delete obj;
    }
    //-------------------------------------------------------------------------
    template <typename T> void PreDefineWidgetModelFactory<T>::plugin()
    {
        WidgetModelFactory * factory = N_new PreDefineWidgetModelFactory<T>(mID);

        if(N_OnlyPtr(UIWidget))
        {
            if (N_Only(UIWidget).addModelFactory(factory) != true)
            {
                N_delete factory;
            }
        }
    }
    //-------------------------------------------------------------------------
    template <typename T> void PreDefineWidgetModelFactory<T>::unplugin()
    {
        if(N_OnlyPtr(UIWidget))
        {
            WidgetModelFactory * factory = N_Only(UIWidget).getModelFactory(mID);
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
