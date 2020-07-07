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

#ifndef _NII_UI_WIDGET_AIDE_H_
#define _NII_UI_WIDGET_AIDE_H_

#include "NiiUIPreInclude.h"

namespace NII
{
namespace UI
{
    /** UI单元辅助
    @version NIIEngine 3.0.0
    */
    class _EngineAPI WidgetAide : public PropertyCmdObj
    {
    public:
        WidgetAide(Widget * dst, ScriptTypeID stid, LangID lid = N_PrimaryLang) :
            PropertyCmdObj(stid, lid),
            mCurrent(dst){}
        virtual ~WidgetAide() {}
    protected:
        /** 当游标进入时触发
        @remark 仅在状态交换时触发
        @param[in] arg
        @version NIIEngine 3.0.0
        */
        virtual void onCursorEnter(const CursorEventArgs * arg) {}
        
        /** 当游标离开时触发
        @remark 仅在状态交换时触发
        @remark 仅在
        @param[in] arg
        @version NIIEngine 3.0.0
        */
        virtual void onCursorLeave(const CursorEventArgs * arg) {}
        
        /** 当游标移动时触发(进入状态下)
        @param[in] arg
        @version NIIEngine 3.0.0
        */
        virtual void onCursorMove(const CursorEventArgs * arg) {}
        
        /** 当游标拖拽时触发(按下状态)
        @param[in] arg
        @version NIIEngine 3.0.0
        */
        virtual void onCursorDrag(const CursorEventArgs * arg) {}
        
        /** 当游标按键释放时触发(进入状态下)
        @remark 仅在状态交换时触发(另一状态则为按键按下)
        @param[in] arg
        @version NIIEngine 3.0.0
        */
        virtual void onButtonDown(const CursorEventArgs * arg) {}
        
        /** 当游标按键按下时触发(进入状态下)
        @remark 仅在状态交换时触发(另一状态则为按键释放)
        @param[in] arg
        @version NIIEngine 3.0.0
        */
        virtual void onButtonUp(const CursorEventArgs * arg) {}
    protected:
        WidgetAide() : PropertyCmdObj(5001, N_PrimaryLang){}
    protected:
        UISheet * mSheet;
        Widget * mCurrent;
    };
    
    /** 游标辅助对象
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CursorWidgetAide : public WidgetAide
    {
    public:
        CursorWidgetAide(Widget * dst, LangID lid = N_PrimaryLang);
        ~CursorWidgetAide();

        /** 鼠标图片像素
        @param[in] pb 对象
        @version NIIEngine 3.0.0
        */
        void setMouseCursor(PixelBuffer * pb);
        
        /** 获取实际的像素缓存
        @param _default
        @vesion NIIEngine 3.0.0
        */
        PixelBuffer * getMouseCursor(bool _default = true) const;
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dest);

        /// @copydetails WidgetAide::onCursorEnter
        void onCursorEnter(const CursorEventArgs * arg);
    protected:
        PixelBuffer * mImage;        ///< 鼠标指针图片
    };
    
    /** UI提示辅助
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TooltipWidgetAide : public WidgetAide
    {
    public:
        TooltipWidgetAide(Widget * dst, LangID lid = N_PrimaryLang);
        ~TooltipWidgetAide();

        /** 设置提示文本
        @param[in] txt 提示文本
        @version NIIEngine 3.0.0
        */
        void setTooltipText(const String & txt);

        /** 获取提示文本
        @version NIIEngine 3.0.0
        */
        const String & getTooltipText() const;

        /** 设置提示对象类型
        @param type 已经注册的Tooltip类类型
        @version NIIEngine 3.0.0
        */
        void setTooltipType(StyleID type);

        /** 获取提示对象类型
        @return 已经注册的Tooltip类类型
        @version NIIEngine 3.0.0
        */
        StyleID getTooltipType() const;

        /** 设置提示对象
        @param obj Tooltip对象
        @version NIIEngine 3.0.0
        */
        void setTooltip(Tooltip * obj);

        /** 获取提示对象
        @return Tooltip对象
        @version NIIEngine 3.0.0
        */
        Tooltip * getTooltip() const;

        /** 是否使用默认的提示对象
        @version NIIEngine 3.0.0
        */
        bool isDefault() const;

        /** 设置是否使用父对象提示文本
        @version NIIEngine 3.0.0
        */
        inline void setParentTipsText(bool b){ mParentTipsText = b; }

        /** 获取是否使用父对象提示文本
        @return NIIEngine 3.0.0
        */
        inline bool isParentTooltipText() const{return mParentTipsText;}
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dst);
    protected:
        /// @copydetails WidgetAide::onCursorEnter
        void onCursorEnter(const CursorEventArgs * arg);

        /// @copydetails WidgetAide::onCursorLeave
        void onCursorLeave(const CursorEventArgs * arg);

        /// @copydetails WidgetAide::onCursorMove
        void onCursorMove(const CursorEventArgs * arg);

        /// @copydetails WidgetAide::onButtonDown
        void onButtonDown(const CursorEventArgs * arg);
    protected:
        String mTipsText;
        mutable StyleID mType;  ///< Tooltip类型
        Tooltip * mCustomTip;
        bool mGenTip;           ///< 自定义对象
        bool mParentTipsText;
    };
}
}
#endif