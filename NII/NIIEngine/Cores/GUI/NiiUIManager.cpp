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

#include "NiiPreProcess.h"
#include "NiiUIManager.h"
#include "NiiUIPixel.h"
#include "NiiUIWidgetManager.h"
#include "NiiUIException.h"
#include "NiiUICombobox.h"
#include "NiiUIDragContainer.h"
#include "NiiUIEditbox.h"
#include "NiiUIListEntry.h"
#include "NiiUIItemListbox.h"
#include "NiiUIListbox.h"
#include "NiiUIListbox.h"
#include "NiiUIListHeader.h"
#include "NiiUIMenubar.h"
#include "NiiUIMultiColumnList.h"
#include "NiiUIMultiLineEditbox.h"
#include "NiiUIWindow.h"
#include "NiiUIProgressBar.h"
#include "NiiUIButton.h"
#include "NiiUILayoutContainer.h"
#include "NiiUIScrollContainer.h"
#include "NiiUIScrollbar.h"
#include "NiiUISizeContainer.h"
#include "NiiUISlider.h"
#include "NiiUINumSpinner.h"
#include "NiiUITabControl.h"
#include "NiiUITitlebar.h"
#include "NiiUITooltip.h"
#include "NiiUITree.h"
#include "NiiUIConfigXmlAnalyze.h"
#include "NiiUICursor.h"
#include "NiiUISchemeManager.h"

#include "NiiPixelManager.h"
#include "NiiFontManager.h"
#include "NiiViewWindow.h"
#include "NiiGpuProgramManager.h"
#include "NiiVFSManager.h"
#include "NiiRenderEffectManager.h"
#include "NiiAnimationManager.h"
#include "NiiEngine.h"
#include "NiiCodecList.h"
#include "NiiScriptModule.h"
#include "NiiDefaultStream.h"
#include "NiiXmlPattern.h"
#include "NiiRegexManager.h"
#include "NiiImageCodec.h"

#include <ctime>
#include <clocale>

#define S_(X) #X
#define STRINGIZE(X) S_(X)

using namespace NII::UI;

namespace NII
{
    //------------------------------------------------------------------------
    class _EngineAPI InnerCodec : public ImageCodec
    {
    public:
        InnerCodec():
            ImageCodec(N_IMAGE_INNER1, _T("InnerCodec")){}

        void setImageFileDataType(const String & type)
        {
            mDataTypeID = type;
        }

        const String & getImageFileDataType() const
        {
            return mDataTypeID;
        }

        Texture * load(const MemStream & data, Texture * result)
        {
            // wrap the buffer of the MemStream with an NII::MemStream.
            DataStream * stream = N_new MemStream(data.getData(), data.size(), false);

            // load the image
            NII::Image image;
            image.load(stream, mDataTypeID.c_str());

            N_delete stream;

            PixelFormat pf = image.getFormat();

            // discover the pixel format and number of pixel components
            int components;
            bool rbswap;
            switch(pf)
            {
            case PF_R8G8B8UN:
                rbswap = true;
                components = 3;
                break;
            case PF_A8R8G8B8:
                rbswap = true;
                components = 4;
                break;
            default:
                rbswap = false;
                break;
            }

            // do the old switcharoo on R and B if needed
            if(rbswap)
            {
                Nui8 * dat = image.getData();
                for(NCount j = 0; j < image.getHeight(); ++j)
                {
                    for(NCount i = 0; i < image.getWidth(); ++i)
                    {
                        Nui8 tmp = dat[i * components + 0];
                        dat[i * components + 0] = dat[i * components + 2];
                        dat[i * components + 2] = tmp;
                    }
                    dat += (image.getWidth() * image.getUnitSize());
                }
            }

            // load the resulting image into the texture
            result->load(image.getData(), image.getWidth(), image.getHeight(), pf);

            return result;
        }
    protected:
        String mDataTypeID;
    };
    //------------------------------------------------------------------------
    template<> UIManager * Singleton<UIManager>::mOnly    = 0;
    //------------------------------------------------------------------------
    static VString gpu_hlsl_vs(
        "uniform float4x4 worldViewProjMatrix;"
        "struct VS_OUT "
        "{"
        "   float4 position : POSITION;"
        "   float2 uv : TEXCOORD0;"
        "   float4 colour : COLOR;"
        "};"
        "VS_OUT main(float4 position : POSITION,"
        "            float2 uv : TEXCOORD0,"
        "            float4 colour : COLOR)"
        "{"
        "    VS_OUT o;"
        "    o.uv = uv;"
        "    o.colour = colour;"
        "    o.position = mul(worldViewProjMatrix, position);"
        "    return o;"
        "}");

    static VString gpu_hlsl_ps(
        "float4 main(float4 colour : COLOR,"
        "   float2 texCoord : TEXCOORD0,"
        "   uniform sampler2D texture : TEXUNIT0) : COLOR"
        "{"
        "    return tex2D(texture, texCoord) * colour;"
        "}");

    static VString gpu_glsl_vs(
        "void main(void)"
        "{"
        "    gl_TexCoord[0] = gl_MultiTexCoord0;"
        "    gl_FrontColor = gl_Color;"
        "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
        "}");

    static VString gpu_glsl_ps(
        "uniform sampler2D texture;"
        "void main(void)"
        "{"
        "    gl_FragColor = texture2D(texture, gl_TexCoord[0].st) * gl_Color;"
        "}");
    //------------------------------------------------------------------------
    N_EVENT(CommandObj, UIManager, ViewSizeChangeEvent, 0);
    N_EVENT(CommandObj, UIManager, TextViewChangeEvent, 1);
    N_EVENT(CommandObj, UIManager, EventCount,          2);
    //------------------------------------------------------------------------
    UIManager::UIManager(RenderSys * rsys, ViewWindow * primary):
        mModelMatrix(Matrix4f::IDENTITY),
        mViewMatrix(Matrix4f::IDENTITY),
        mProjMatrix(Matrix4f::IDENTITY),
        mSpaceMatrix(Matrix4f::IDENTITY),
        mBlendMode(PBM_Unknow),
        mTextView(0),
        mPreVP(0),
        mVP(0),
        mFP(0),
        mVPParams(0),
        mFPParams(0),
        mValidSpaceMatrix(false),
        mShaderEnable(false),
        mAloneFrame(true),
        mEnable(true)
    {
        notifySignal(UIManager::EventCount);
        Engine * root = N_EnginePtr();
        ConfigXmlAnalyze config;
        String configFile = _T("NiiUIConfig.xsd");
        XmlPattern * xprc = root->getXmlPattern();
        DataStream * data = N_Only(ResourceScheme).open(configFile, GroupUnknow);
        if(data)
        {
            try
            {
                xprc->parse(&config, data);
            }
            catch(...)
            {
            }
            N_delete data;            
        }

        config.initModule();

        // beginning main init
        N_Only(Log).log(_I18n("---- Begining NII UI SYSTEM ----"));

        N_new WidgetManager();
        N_new SchemeManager();
        N_new RenderEffectManager();

        N_WidgetFactory(DefaultWindow,                              N_WIDGET_DefaultWindow).plugin();
        N_WidgetFactory(DragContainer,                              N_WIDGET_DragContainer).plugin();
        N_WidgetFactory(SizeContainer,                              N_WIDGET_SizeContainer).plugin();
        N_WidgetFactory(PushButton,                                 N_WIDGET_PushButton).plugin();
        N_WidgetFactory(RadioButton,                                N_WIDGET_RadioButton).plugin();
        N_WidgetFactory(Combobox,                                   N_WIDGET_Combobox).plugin();
        N_WidgetFactory(Container,                                  N_WIDGET_Container).plugin();
        N_WidgetFactory(ComboDropList,                              N_WIDGET_ComboDropList).plugin();
        N_WidgetFactory(Editbox,                                    N_WIDGET_Editbox).plugin();
        N_WidgetFactory(FrameWindow,                                N_WIDGET_FrameWindow).plugin();
        N_WidgetFactory(ListItem,                                   N_WIDGET_ListEntry).plugin();
        N_WidgetFactory(Listbox,                                    N_WIDGET_Listbox).plugin();
        N_WidgetFactory(ListHeader,                                 N_WIDGET_ListHeader).plugin();
        N_WidgetFactory(ListHeaderItem,                             N_WIDGET_ListHeaderSegment).plugin();
        N_WidgetFactory(Menubar,                                    N_WIDGET_Menubar).plugin();
        N_WidgetFactory(PopupMenu,                                  N_WIDGET_PopupMenu).plugin();
        N_WidgetFactory(MenuItem,                                   N_WIDGET_MenuItem).plugin();
        N_WidgetFactory(MultiColumnList,                            N_WIDGET_MultiColumnList);
        N_WidgetFactory(MultiEditbox,                               N_WIDGET_MultiLineEditbox).plugin();
        N_WidgetFactory(ProgressBar,                                N_WIDGET_ProgressBar).plugin();
        N_WidgetFactory(ScrollContainer,                            N_WIDGET_ScrollContainer).plugin();
        N_WidgetFactory(Scrollbar,                                  N_WIDGET_Scrollbar).plugin();
        N_WidgetFactory(Slider,                                     N_WIDGET_Slider).plugin();
        N_WidgetFactory(NumSpinner,                                 N_WIDGET_NumSpinner).plugin();
        N_WidgetFactory(TabButton,                                  N_WIDGET_TabButton).plugin();
        N_WidgetFactory(TabControl,                                 N_WIDGET_TabControl).plugin();
        N_WidgetFactory(PosButton,                                  N_WIDGET_PosButton).plugin();
        N_WidgetFactory(Titlebar,                                   N_WIDGET_Titlebar).plugin();
        N_WidgetFactory(ToggleButton,                               N_WIDGET_ToggleButton).plugin();
        N_WidgetFactory(Tooltip,                                    N_WIDGET_Tooltip).plugin();
        N_WidgetFactory(ItemListbox,                                N_WIDGET_ItemListbox).plugin();
        N_WidgetFactory(Tree,                                       N_WIDGET_Tree).plugin();
        N_WidgetFactory(LayoutCell,                                 N_WIDGET_LayoutCell).plugin();
        N_WidgetFactory(HorizontalLayoutContainer,                  N_WIDGET_HorizontalLayoutContainer).plugin();
        N_WidgetFactory(VerticalLayoutContainer,                    N_WIDGET_VerticalLayoutContainer).plugin();
        N_WidgetFactory(GridLayoutContainer,                        N_WIDGET_GridLayoutContainer).plugin();

        N_WidgetModeFactory(ButtonView,                             N_VIEW_Button).plugin();
        N_WidgetModeFactory(DefaultView,                            N_VIEW_Default).plugin();
        N_WidgetModeFactory(EditboxView,                            N_VIEW_Editbox).plugin();
        N_WidgetModeFactory(FrameWindowView,                        N_VIEW_FrameWindow).plugin();
        N_WidgetModeFactory(DefaultListItemView,                    N_VIEW_ListEntry).plugin();
        N_WidgetModeFactory(ListHeaderView,                         N_VIEW_ListHeader).plugin();
        N_WidgetModeFactory(ListHeaderItemView,                     N_VIEW_ListHeaderSegment).plugin();
        N_WidgetModeFactory(ListboxView,                            N_VIEW_Listbox).plugin();
        N_WidgetModeFactory(DefaultListContainerView,               N_VIEW_Menubar).plugin();
        N_WidgetModeFactory(MenuItemView,                           N_VIEW_MenuItem).plugin();
        N_WidgetModeFactory(MultiColumnListView,                    N_VIEW_MultiColumnList).plugin();
        N_WidgetModeFactory(MultiEditboxView,                       N_VIEW_MultiLineEditbox).plugin();
        N_WidgetModeFactory(PopupMenuView,                          N_VIEW_PopupMenu).plugin();
        N_WidgetModeFactory(ProgressBarView,                        N_VIEW_ProgressBar).plugin();
        N_WidgetModeFactory(ScrollContainerView,                    N_VIEW_ScrollContainer).plugin();
        N_WidgetModeFactory(ScrollbarView,                          N_VIEW_Scrollbar).plugin();
        N_WidgetModeFactory(SliderView,                             N_VIEW_Slider).plugin();
        N_WidgetModeFactory(StaticView,                             N_VIEW_Static).plugin();
        N_WidgetModeFactory(StaticImageView,                        N_VIEW_StaticImage).plugin();
        N_WidgetModeFactory(StaticTextView,                         N_VIEW_StaticText).plugin();
        N_WidgetModeFactory(TabButtonView,                          N_VIEW_TabButton).plugin();
        N_WidgetModeFactory(TabControlView,                         N_VIEW_TabControl).plugin();
        N_WidgetModeFactory(TitlebarView,                           N_VIEW_Titlebar).plugin();
        N_WidgetModeFactory(ToggleButtonView,                       N_VIEW_ToggleButton).plugin();
        N_WidgetModeFactory(TooltipView,                            N_VIEW_Tooltip).plugin();
        N_WidgetModeFactory(ItemListboxView,                        N_VIEW_ItemListbox).plugin();
        N_WidgetModeFactory(TreeView,                               N_VIEW_Tree).plugin();

        //ViewWindow * target = root->getView();
        mSys = rsys;

        /*#if defined RTSHADER_SYSTEM_BUILD_CORE_SHADERS
            // default to using shaders when that is the sane thing to do.
            if(!mSys->getFixedPipelineEnabled())
                setShaderEnable(true);
        #endif*/
        mMainSheet = create(mSys, primary);
        mCursorInitPos = Vector2f(primary->getWidth() / 2.0f, primary->getHeight() / 2.0f);
        // autoload resources specified in config
        config.loadResource();

        // set up defaults
        config.initResource();

        // scripting available?
        if(root->getScriptModule(0))
        {
            config.execInitExec();
            config.getTerminateExec();
        }
    }
    //------------------------------------------------------------------------
    UIManager::~UIManager()
    {
        N_Only(Log).log(_I18n("---- Begining NII UI SYSTEM destroy ----"));

        if(mFPParams)
        {
            N_delete mFPParams;
            mFPParams = 0;
        }
        if(mVPParams)
        {
            N_delete mVPParams;
            mVPParams = 0;
        }
        if(mFP)
        {
            N_Only(HighLevelGpuProgram).destroy(mFP->getOriginID());
            mFP = 0;
        }
        if(mVP)
        {
            N_Only(HighLevelGpuProgram).destroy(mVP->getOriginID());
            mVP = 0;
        }

        N_Only(Widget).recover();
        N_Only(Widget).destroyAll();

        N_WidgetFactory(DefaultWindow,                          N_WIDGET_DefaultWindow).unplugin();
        N_WidgetFactory(DragContainer,                          N_WIDGET_DragContainer).unplugin();
        N_WidgetFactory(SizeContainer,                          N_WIDGET_SizeContainer).unplugin();
        N_WidgetFactory(PushButton,                             N_WIDGET_PushButton).unplugin();
        N_WidgetFactory(RadioButton,                            N_WIDGET_RadioButton).unplugin();
        N_WidgetFactory(Combobox,                               N_WIDGET_Combobox).unplugin();
        N_WidgetFactory(Container,                              N_WIDGET_Container).unplugin();
        N_WidgetFactory(ComboDropList,                          N_WIDGET_ComboDropList).unplugin();
        N_WidgetFactory(Editbox,                                N_WIDGET_Editbox).unplugin();
        N_WidgetFactory(FrameWindow,                            N_WIDGET_FrameWindow).unplugin();
        N_WidgetFactory(ListItem,                               N_WIDGET_ListEntry).unplugin();
        N_WidgetFactory(Listbox,                                N_WIDGET_Listbox).unplugin();
        N_WidgetFactory(ListHeader,                             N_WIDGET_ListHeader).unplugin();
        N_WidgetFactory(ListHeaderItem,                         N_WIDGET_ListHeaderSegment).unplugin();
        N_WidgetFactory(Menubar,                                N_WIDGET_Menubar).unplugin();
        N_WidgetFactory(PopupMenu,                              N_WIDGET_PopupMenu).unplugin();
        N_WidgetFactory(MenuItem,                               N_WIDGET_MenuItem).unplugin();
        N_WidgetFactory(MultiColumnList,                        N_WIDGET_MultiColumnList);
        N_WidgetFactory(MultiEditbox,                           N_WIDGET_MultiLineEditbox).unplugin();
        N_WidgetFactory(ProgressBar,                            N_WIDGET_ProgressBar).unplugin();
        N_WidgetFactory(ScrollContainer,                        N_WIDGET_ScrollContainer).unplugin();
        N_WidgetFactory(Scrollbar,                              N_WIDGET_Scrollbar).unplugin();
        N_WidgetFactory(Slider,                                 N_WIDGET_Slider).unplugin();
        N_WidgetFactory(NumSpinner,                             N_WIDGET_NumSpinner).unplugin();
        N_WidgetFactory(TabButton,                              N_WIDGET_TabButton).unplugin();
        N_WidgetFactory(TabControl,                             N_WIDGET_TabControl).unplugin();
        N_WidgetFactory(PosButton,                              N_WIDGET_PosButton).unplugin();
        N_WidgetFactory(Titlebar,                               N_WIDGET_Titlebar).unplugin();
        N_WidgetFactory(ToggleButton,                           N_WIDGET_ToggleButton).unplugin();
        N_WidgetFactory(Tooltip,                                N_WIDGET_Tooltip).unplugin();
        N_WidgetFactory(ItemListbox,                            N_WIDGET_ItemListbox).unplugin();
        N_WidgetFactory(Tree,                                   N_WIDGET_Tree).unplugin();
        N_WidgetFactory(LayoutCell,                             N_WIDGET_LayoutCell).unplugin();
        N_WidgetFactory(HorizontalLayoutContainer,              N_WIDGET_HorizontalLayoutContainer).unplugin();
        N_WidgetFactory(VerticalLayoutContainer,                N_WIDGET_VerticalLayoutContainer).unplugin();
        N_WidgetFactory(GridLayoutContainer,                    N_WIDGET_GridLayoutContainer).unplugin();

        N_WidgetModeFactory(ButtonView,                         N_VIEW_Button).unplugin();
        N_WidgetModeFactory(DefaultView,                        N_VIEW_Default).unplugin();
        N_WidgetModeFactory(EditboxView,                        N_VIEW_Editbox).unplugin();
        N_WidgetModeFactory(FrameWindowView,                    N_VIEW_FrameWindow).unplugin();
        N_WidgetModeFactory(DefaultListItemView,                N_VIEW_ListEntry).unplugin();
        N_WidgetModeFactory(ListHeaderView,                     N_VIEW_ListHeader).unplugin();
        N_WidgetModeFactory(ListHeaderItemView,                 N_VIEW_ListHeaderSegment).unplugin();
        N_WidgetModeFactory(ListboxView,                        N_VIEW_Listbox).unplugin();
        N_WidgetModeFactory(DefaultListContainerView,           N_VIEW_Menubar).unplugin();
        N_WidgetModeFactory(MenuItemView,                       N_VIEW_MenuItem).unplugin();
        N_WidgetModeFactory(MultiColumnListView,                N_VIEW_MultiColumnList).unplugin();
        N_WidgetModeFactory(MultiEditboxView,                   N_VIEW_MultiLineEditbox).unplugin();
        N_WidgetModeFactory(PopupMenuView,                      N_VIEW_PopupMenu).unplugin();
        N_WidgetModeFactory(ProgressBarView,                    N_VIEW_ProgressBar).unplugin();
        N_WidgetModeFactory(ScrollContainerView,                N_VIEW_ScrollContainer).unplugin();
        N_WidgetModeFactory(ScrollbarView,                      N_VIEW_Scrollbar).unplugin();
        N_WidgetModeFactory(SliderView,                         N_VIEW_Slider).unplugin();
        N_WidgetModeFactory(StaticView,                         N_VIEW_Static).unplugin();
        N_WidgetModeFactory(StaticImageView,                    N_VIEW_StaticImage).unplugin();
        N_WidgetModeFactory(StaticTextView,                     N_VIEW_StaticText).unplugin();
        N_WidgetModeFactory(TabButtonView,                      N_VIEW_TabButton).unplugin();
        N_WidgetModeFactory(TabControlView,                     N_VIEW_TabControl).unplugin();
        N_WidgetModeFactory(TitlebarView,                       N_VIEW_Titlebar).unplugin();
        N_WidgetModeFactory(ToggleButtonView,                   N_VIEW_ToggleButton).unplugin();
        N_WidgetModeFactory(TooltipView,                        N_VIEW_Tooltip).unplugin();
        N_WidgetModeFactory(ItemListboxView,                    N_VIEW_ItemListbox).unplugin();
        N_WidgetModeFactory(TreeView,                           N_VIEW_Tree).unplugin();

        N_Only(Widget).removeAllFactory();

        N_delete SchemeManager::getOnlyPtr();
        N_delete WidgetManager::getOnlyPtr();
        N_delete RenderEffectManager::getOnlyPtr();

        SheetList::iterator z, zend = mSheetList.end();
        for(z = mSheetList.begin(); z != zend; ++z)
        {
            N_delete z->first;
        }
        mSheetList.clear();
    }
    //------------------------------------------------------------------------
    void UIManager::render()
    {
        SheetList::iterator i, iend = mSheetList.end();
        for(i = mSheetList.begin(); i != iend; ++i)
        {
            UISheet * sheet = i->first;
            if(!mPreVP)
            {
                mPreVP = mSys->getViewports();
                if(mPreVP && mPreVP->getCamera())
                    mPreProjMatrix = mPreVP->getCamera()->getSysProjMatrix();
            }

            // 进入状态
            sheet->apply();

            mSys->setLight(false);
            mSys->setStencil(ShaderChStencil(false, CPM_LESS_EQUAL, false, 0, 0), SB_Depth);
            mSys->_setCullingMode(CM_None);
            mSys->setFog(ShaderChFog::None);
            mSys->setColourWrite(ColourMark::CM_RGBA);
            mSys->setShadingType(SM_GOURAUD);
            mSys->_setPolygonMode(PM_SOLID);

            if(mShaderEnable)
            {
                GpuProgram * prog;

                if(prog = mVP->getMain())
                    mSys->on(prog);

                if(prog = mFP->getMain())
                    mSys->on(prog);
            }
            else
            {
                mSys->off(GpuProgram::ST_VS);
                mSys->off(GpuProgram::ST_FS);
                mSys->off(GpuProgram::ST_GS);
            }

            // set alpha blending to known state
            apply(PBM_Normal, true);

            if(mAloneFrame)
                mSys->_beginFrame();

            sheet->flush();

            if(mAloneFrame)
                mSys->_endFrame();

            if(mPreVP)
            {
                mSys->_setViewport(mPreVP);
                if(mPreVP->getCamera())
                {
                    mSys->_setProj(mPreProjMatrix);
                    mSys->_setView(mPreVP->getCamera()->getViewMatrix());
                }
                mPreVP = 0;
                mPreProjMatrix = Matrix4f::IDENTITY;
            }
        }

        N_Only(Widget).recover();
    }
    //------------------------------------------------------------------------
    void UIManager::setViewSize(ViewWindow * dst, const PlaneSizei & size)
    {
        // notify other components of the display size change
        N_Only(Pixel).notifyView(size);
        N_Only(Font).notifyView(size);

        SheetList::iterator i, iend = mSheetList.end();
        for(i = mSheetList.begin(); i != iend; ++i)
        {
            if (i->second == dst)
            {
                UISheet * sheet = i->first;
                if (size != sheet->getArea().getSize())
                {
                    sheet->setAreaSize(size);
                }
                UISheet::WinList::iterator j, jend = i->first->mWinList.end();
                for (j = i->first->mWinList.begin(); j != jend; ++j)
                {
                    Window * win = *j;
                    win->refresh();
                    UISubSheet * rs = win->getRender();
                    if (rs)
                    {
                        rs->rebuild();
                    }
                }
            }
        }

        DisplayEventArgs arg(size);
        signal(ViewSizeChangeEvent, &arg);
    }
    //------------------------------------------------------------------------
    void UIManager::onCreate(const Widget * obj)
    {
        (void)obj;
    }
    //------------------------------------------------------------------------
    void UIManager::onDestroy(const Widget * obj)
    {
        SheetList::iterator i, iend = mSheetList.end();
        for (i = mSheetList.begin(); i != iend; ++i)
        {
            i->first->DestroyCB(obj);
        }
    }
    //------------------------------------------------------------------------
    void UIManager::setView(TextView * obj)
    {
        if(obj != mTextView)
        {
            mTextView = obj;

            EventArgs arg;
            signal(TextViewChangeEvent, &arg);
        }
    }
    //------------------------------------------------------------------------
    UISheet * UIManager::create(RenderSys * rs, ViewWindow * win)
    {
        UISheet * re = N_new UISheet(rs, win);
        mSheetList.insert(Npair(re, win));
        return re;
    }
    //------------------------------------------------------------------------
    void UIManager::destroy(UISheet * obj)
    {
        SheetList::iterator i = mSheetList.find(obj);
        if(i != mSheetList.end())
        {
            N_delete i->second;
            mSheetList.erase(i);
        }
    }
    //------------------------------------------------------------------------
    UISheet * UIManager::getSheet(ViewWindow * dst) const
    {
        SheetList::const_iterator i, iend = mSheetList.end();
        for(i = mSheetList.begin(); i != iend; ++i)
        {
            if(i->second == dst)
            return i->first;
        }
        return 0;
    }
    //------------------------------------------------------------------------
    UISheet * UIManager::getMainSheet() const
    {
        return mSheetList.begin()->first;
    }
    //------------------------------------------------------------------------
    void UIManager::setDisplaySize(ViewWindow * dst, const PlaneSizei & sz)
    {
        SheetList::iterator i, iend = mSheetList.end();
        for(i = mSheetList.begin(); i != iend; ++i)
        {
            if (i->second == dst)
            {
                UISheet * sheet = i->first;
                if (sz != sheet->getArea().getSize())
                {
                    Recti temp = sheet->getArea();
                    temp.setSize(sz);
                    sheet->setArea(temp);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    PlaneSizei UIManager::getDisplaySize(Nid id) const
    {
        SheetList::const_iterator i, iend = mSheetList.end();
        for (i = mSheetList.begin(); i != iend; ++i)
        {
            if (i->second->getID() == id)
                return i->first->getArea().getSize();
        }
        return mMainSheet->getArea().getSize();
    }
    //------------------------------------------------------------------------
    PlaneSizei UIManager::getDisplaySize(ViewWindow * dst) const
    {
        SheetList::const_iterator i, iend = mSheetList.end();
        for(i = mSheetList.begin(); i != iend; ++i)
        {
            if(i->second == dst)
                return i->first->getArea().getSize();
        }
        return mMainSheet->getArea().getSize();
    }
    //------------------------------------------------------------------------
    void UIManager::apply(PixelBlendMode mode, bool force)
    {
        static ShaderChBlend s1(CF_ONE, CF_INV_SRC_ALPHA, CBM_ADD);
        static ShaderChBlend s2(CF_SRC_ALPHA, CF_INV_SRC_ALPHA,
            CF_INV_DST_ALPHA, CF_ONE, CBM_ADD, CBM_ADD);

        if((mBlendMode == mode) && !force)
            return;

        mBlendMode = mode;

        if(mBlendMode == PBM_Average)
            mSys->setBlend(s1);
        else
            mSys->setBlend(s2);
    }
    //------------------------------------------------------------------------
    void UIManager::setWindow(UISheet * dest, ViewWindow * obj)
    {
        SheetList::const_iterator i = mSheetList.find(dest);
        if(i != mSheetList.end())
        {
            i->first->setWindow(obj);
        }
    }
    //------------------------------------------------------------------------
    void UIManager::getWindow(UISheet * dest, ViewWindow *& obj)
    {
        SheetList::const_iterator i = mSheetList.find(dest);
        if (i != mSheetList.end())
        {
            obj = i->second;
        }
        obj = 0;
    }
    //------------------------------------------------------------------------
    void UIManager::setShaderEnable(bool set)
    {
        if(mShaderEnable == set)
            return;

        if(set)
        {
            mGLSLEnable = N_Only(HighLevelGpuProgram).isShaderSupport(SL_GLSL);

            // create vertex shader
            mVP = N_Only(HighLevelGpuProgram).create(N_Only(HighLevelGpuProgram).genValidID(),
                GroupDefault, mGLSLEnable ? SL_GLSL : SL_HLSL, GpuProgram::ST_VS);

            mVP->set(_T("entry_point"), "main");

            if(mGLSLEnable)
                mVP->set(_T("target"), "arbvp1");
            else if(N_Only(GpuProgram).isShaderSupport(SL_VS_4_0))
                mVP->set(_T("target"), "vs_4_0");
            else if(N_Only(GpuProgram).isShaderSupport(SL_VS_2_0))
                mVP->set(_T("target"), "vs_2_0");
            else
            {
                N_Only(HighLevelGpuProgram).destroy(mVP->getOriginID());
                mVP = 0;
                N_EXCEPT(RenderingAPI, _I18n("不支持顶点着色语法,不能编译'gui_vs'着色程序"));
            }

            mVP->setProgramCode(mGLSLEnable ? gpu_glsl_vs : gpu_hlsl_vs);
            mVP->load();

            // create pixel shader
            mFP = N_Only(HighLevelGpuProgram).create(N_Only(HighLevelGpuProgram).genValidID(),
                GroupDefault, mGLSLEnable ? SL_GLSL : SL_HLSL, GpuProgram::ST_FS);

            mFP->set(_T("entry_point"), "main");

            if(mGLSLEnable)
            {
                mFP->set(_T("target"), "arbfp1");
            }
            else if(N_Only(GpuProgram).isShaderSupport(SL_PS_4_0))
            {
                mFP->set(_T("target"), "ps_4_0");
            }
            else if(N_Only(GpuProgram).isShaderSupport(SL_PS_2_0))
            {
                mFP->set(_T("target"), "ps_2_0");
            }
            else
            {
                N_Only(HighLevelGpuProgram).destroy(mVP->getOriginID());
                N_Only(HighLevelGpuProgram).destroy(mFP->getOriginID());
                mVP = 0;
                mFP = 0;

                N_EXCEPT(RenderingAPI, _I18n("不支持片段着色语法,不能编译'gui_ps'着色程序"));
            }

            mFP->setProgramCode(mGLSLEnable ? gpu_glsl_ps : gpu_hlsl_ps);
            mFP->load();

            mVPParams = mVP->createParam();
            mFPParams = mFP->createParam();
        }
        else
        {
            if(mFPParams != 0)
            {
                N_delete mFPParams;
                mFPParams = 0;
            }
            if(mVPParams != 0)
            {
                N_delete mVPParams;
                mVPParams = 0;
            }
            if(mFP != 0)
            {
                N_Only(HighLevelGpuProgram).destroy(mFP->getOriginID());
                mFP = 0;
            }
            if(mVP != 0)
            {
                N_Only(HighLevelGpuProgram).destroy(mVP->getOriginID());
                mVP = 0;
            }
        }

        mShaderEnable = set;
    }
    //------------------------------------------------------------------------
    void UIManager::syn() const
    {
        if (mShaderEnable)
        {

            if (mGLSLEnable)
            {
                mFPParams->set("texture", 0, 0);

                mSys->on(GpuProgram::ST_FS, mFPParams, GPT_Unknow);
            }
            else
            {
                mVPParams->set("worldViewProjMatrix", 0, getSpaceMatrix());

                mSys->on(GpuProgram::ST_VS, mVPParams, GPT_Unknow);
            }
        }
    }
    //------------------------------------------------------------------------
    void UIManager::setModelMatrix(const Matrix4f & m)
    {
        mSys->_setModel(m);

        mModelMatrix = m;
        mValidSpaceMatrix = false;
    }
    //------------------------------------------------------------------------
    void UIManager::setViewMatrix(const Matrix4f & m)
    {
        mSys->_setView(m);

        mViewMatrix = m;
        mValidSpaceMatrix = false;
    }
    //------------------------------------------------------------------------
    void UIManager::setProjMatrix(const Matrix4f & m)
    {
        mSys->_setProj(m);

        mProjMatrix = m;
        mValidSpaceMatrix = false;
    }
    //------------------------------------------------------------------------
    const Matrix4f & UIManager::getSpaceMatrix() const
    {
        if(!mValidSpaceMatrix)
        {
            Matrix4f fprj(mProjMatrix);

            if(mSys->getViewports()->getBuffer()->isFlipping())
            {
                fprj[1][0] = -fprj[1][0];
                fprj[1][1] = -fprj[1][1];
                fprj[1][2] = -fprj[1][2];
                fprj[1][3] = -fprj[1][3];
            }

            const_cast<UIManager *>(this)->mSpaceMatrix = fprj * mViewMatrix * mModelMatrix;

            const_cast<UIManager *>(this)->mValidSpaceMatrix = true;
        }

        return mSpaceMatrix;
    }
    //------------------------------------------------------------------------
}