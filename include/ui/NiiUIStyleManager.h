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

#ifndef _NII_UI_STYLEMANAGER_H_
#define _NII_UI_STYLEMANAGER_H_

#include "NiiUIPreInclude.h"
#include "NiiSingleton.h"
#include "NiiUIException.h"
#include "NiiUIStyle.h"
#include "NiiUIEffectTextView.h"
#include "NiiUINormalTextView.h"

namespace NII
{
    /** UI风格管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UIStyleManager : public Singleton<UIStyleManager>, public UIAlloc
    {
    public:
        typedef map<StyleID, Style *>::type StyleList;
    public:
        UIStyleManager();
        ~UIStyleManager();

        /** 加载文件
        @version NIIEngine 3.0.0
        */
        void load(const String & file, GroupID gid = GroupUnknow);
        
        /** 加载内存流
        @version NIIEngine 3.0.0
        */
        void load(const MemDataStream * data);
        
        /** 添加风格
        @version NIIEngine 3.0.0
        */
        void add(Style * style);

        /** 移去风格
        @version NIIEngine 3.0.0
        */
        void remove(StyleID style);

        /** 移去所有风格
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** 是否存在风格
        @version NIIEngine 3.0.0
        */
        bool isExist(StyleID style) const;

        /** 获取风格
        @version NIIEngine 3.0.0
        */
        Style * get(StyleID style) const;

        /** 
        @param[out]
        */
        void write(StyleID style, Nostream & out) const;

        /** 
        @param[out] out 
        */
        void exportSeries(Nui16 Ser, Nostream & out) const;

        /** 
        @version NIIEngine 3.0.0
        */
        static inline void setGroup(GroupID gid)
        {
            ResourceGroup = gid;
        }

        /** 
        @version NIIEngine 3.0.0
        */
        static inline GroupID getGroup()
        {
            return ResourceGroup;
        }
        
        const StyleList & getWidgetList() const;

           /// @copydetails Singleton::geOnly
        static UIStyleManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static UIStyleManager * getOnlyPtr();
    public:
        static const Colour DefaultTextColour;
        static const Colour DefaultSelectionColour;
                
        static EffectTextView gDefaultTextView;
        static NormalTextView gNormalTextView;
    private:
        static GroupID ResourceGroup;
        StyleList mStyleList;
    };
}
#endif