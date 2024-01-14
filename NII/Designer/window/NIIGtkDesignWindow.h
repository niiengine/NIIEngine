/*
-----------------------------------------------------------------------------
面向游戏设计引擎

时间: 2012-1-15 

作者: zelda_ok

格式: open it use utf-8

主页: www.nii-zelda.tk

授权方式:商业授权(www.nii-zelda.tk/license)
------------------------------------------------------------------------------
*/

#ifndef NII_GTK_DESIGN_WINDOW
#define NII_GTK_DESIGN_WINDOW

namespace NII
{
    class _NIIExport GTKDesignWindow : public DesignWindow
    {
    public:
        GTKDesignWindow();
        ~GTKDesignWindow();
    public:
        /// @copydetails 
        virtual void joinRenderEvent()=0;

        /// @copydetails 
        virtual void joinRootEvent()=0;

        /// @copydetails 
        virtual void joinDeviceEvent()=0;

        /// @copydetails
        virtual void joinDefineEvent()=0;

        /// @copydetails
        virtual void joinWindowEvent()=0;

        /// @copydetails
        virtual bool generateRightSide()=0;

        /// @copydetails
        virtual bool generateLeftSide()=0;

        /// @copydetails
        virtual bool generateTopSide()=0;

        /// @copydetails
        virtual bool generateBottomSide()=0;

        /// @copydetails
        virtual String Description() = 0;
    protected:
        /// @copydetails
        virtual bool generateToolsBar()=0;
        /// @copydetails
        virtual bool generateMenus()=0;
    private:
        
    };
}

#endif