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
///@warnning 不要手动修改此文件
///@warnning Do not modify this file manually
#ifndef _NII_PROPERTYLIST_H_
#define _NII_PROPERTYLIST_H_


/////////////////////////////////////////////////////
// ScriptType
/////////////////////////////////////////////////////

#define N_Script_Font                   1
#define N_Script_Material               2
#define N_Script_GpuProgram             3
#define N_Script_GpuProgramParam        4
#define N_Script_ParticleSpace          5
#define N_Script_SpreadParticle         6
#define N_Script_ParticleEffect         7
#define N_Script_FrameFusion            8
#define N_Script_Widget                 9
#define N_Script_PixelBuffer            10
#define N_Script_AnimationCollector     11
#define N_Script_Animation              12
#define N_Script_UIConfig               13
#define N_Script_UILayout               14
#define N_Script_UIScheme               15
#define N_Script_UIStyle                16

#define N_Script_Custom                 1000000
#define N_Script_CustomMax              9000000

/////////////////////////////////////////////////////
// Language
/////////////////////////////////////////////////////

#define N_Lang_English      0
#define N_Lang_French       1
#define N_Lang_Hebrew       2
#define N_Lang_Japanese     3
#define N_Lang_German       4
#define N_Lang_China_TD     5
#define N_Lang_Russian      6

/////////////////////////////////////////////////////
// Error Type
/////////////////////////////////////////////////////

#define N_OK                    0
#define N_ERROR_IO              1
#define N_ERROR_Internal        2
#define N_ERROR_Runtime         3
#define N_ERROR_UnImpl          4
#define N_ERROR_InvalidFile     5
#define N_ERROR_InvalidParam    6
#define N_ERROR_InvalidState    7
#define N_ERROR_UniqueRepeat    8
#define N_ERROR_EXT10           9
#define N_ERROR_EXT9            10
#define N_ERROR_EXT8            11
#define N_ERROR_EXT7            12
#define N_ERROR_EXT6            13
#define N_ERROR_EXT5            14
#define N_ERROR_EXT4            15
#define N_ERROR_EXT3            16
#define N_ERROR_EXT2            17
#define N_ERROR_EXT1            18
#define N_ERROR_NET_1           19 

/////////////////////////////////////////////////////
// CodeGen
/////////////////////////////////////////////////////
#define N_CodeGen_Render        0
#define N_CodeGen_Compute       200

/////////////////////////////////////////////////////
// FACTORY
/////////////////////////////////////////////////////

#define N_FACTORY_Camera                0
#define N_FACTORY_Light                 1
#define N_FACTORY_GeoSpace              2
#define N_FACTORY_CustomSpace           3
#define N_FACTORY_PixelOrbit            4
#define N_FACTORY_ParticleSpace         5
#define N_FACTORY_PixelChain            6
#define N_FACTORY_Frustum               7
#define N_FACTORY_RenderObj             8
#define N_FACTORY_MovablePlane          9
#define N_FACTORY_SpaceObj              10
#define N_FACTORY_HLSLProgram           11
#define N_FACTORY_GLSLProgram           12
#define N_FACTORY_CGProgram             13
#define N_FACTORY_ProxyProgram          14
#define N_FACTORY_GLSLESProgram         15
#define N_FACTORY_DummyProgram          16
#define N_FACTORY_InstanceSpace         17
#define N_FACTORY_NormalSpaceType       18
#define N_FACTORY_StaticSpace           19
#define N_FACTORY_InstanceBatch         20
#define N_FACTORY_InstanceGeo           21
#define N_FACTORY_ParticleCtrl          22

/////////////////////////////////////////////////////
// PropertyObj
/////////////////////////////////////////////////////
#define N_CmdObj_Null                              0
#define N_CmdObj_Texture                           1
#define N_CmdObj_ItemListbox                       2
#define N_CmdObj_FreeTypeFont                      3
#define N_CmdObj_PixmapFont                        4
#define N_CmdObj_Font                              5
#define N_CmdObj_Spinner                           6
#define N_CmdObj_ScrollContainer                   7
#define N_CmdObj_RadioButton                       8
#define N_CmdObj_TextOverlayGeo                    9
#define N_CmdObj_SpreadParticle                    10
#define N_CmdObj_Editbox                           21
#define N_CmdObj_Combobox                          22
#define N_CmdObj_ProgressBar                       23
#define N_CmdObj_Tooltip                           24
#define N_CmdObj_PopupMenu                         25
#define N_CmdObj_Widget                            26
#define N_CmdObj_Listbox                           27
#define N_CmdObj_GridLayoutContainer               28
#define N_CmdObj_ListContainer                     29
#define N_CmdObj_TabControl                        30
#define N_CmdObj_Tree                              31
#define N_CmdObj_ListHeader                        32
#define N_CmdObj_DragContainer                     33
#define N_CmdObj_ListHeaderSegment                 34
#define N_CmdObj_FrameWindow_ID                    35
#define N_CmdObj_FrameOverlayGeo                   36
#define N_CmdObj_ParticleSpace                     37
#define N_CmdObj_ParticleCtrlGeo                   38
#define N_CmdObj_RectOverlayGeo                    39
#define N_CmdObj_Skeleton                          40
#define N_CmdObj_Sound                             41
#define N_CmdObj_Material                          42
#define N_CmdObj_ScrolledListContainer             43
#define N_CmdObj_MultiColumnList                   44
#define N_CmdObj_MultiLineEditbox                  45
#define N_CmdObj_Scrollbar                         46
#define N_CmdObj_Menubar                           47
#define N_CmdObj_Titlebar                          48
#define N_CmdObj_ExternalTexture                   49
#define N_CmdObj_ParticleEffect                    50
#define N_CmdObj_ParticleGeo                       51
#define N_CmdObj_OverlayView                       52
#define N_CmdObj_ContainerOverlayView              53
#define N_CmdObj_FrameOverlayView                  54
#define N_CmdObj_Compositor                        55
#define N_CmdObj_Mesh                              56
#define N_CmdObj_PatchMesh                         57

#define N_CmdObj_Lable                             60
#define N_CmdObj_DragDropWindow                    61
#define N_CmdObj_LayoutCell                        62
#define N_CmdObj_VerticalLayoutContainer           63
#define N_CmdObj_HorizontalLayoutContainer         64
#define N_CmdObj_TabButton                         65
#define N_CmdObj_Button                            66
#define N_CmdObj_ToggleButton                      67
#define N_CmdObj_PushButton                        68
#define N_CmdObj_PosButton                         69
#define N_CmdObj_ComboDropList                     70
#define N_CmdObj_Slider                            71
#define N_CmdObj_SizeContainer                     72
#define N_CmdObj_ListEntry                         73
#define N_CmdObj_MenuItem                          74
#define N_CmdObj_Window                            75
#define N_CmdObj_FrameWindow                       76
#define N_CmdObj_DefaultWindow                     77
#define N_CmdObj_Container                         78

#define N_CmdObj_GpuProgram                        100
#define N_CmdObj_GLGpuProgram                      101
#define N_CmdObj_HighLevelGpuProgram               102
#define N_CmdObj_GLSLProgram                       103
#define N_CmdObj_ApuProgram                        104
#define N_CmdObj_CLProgram                         106
#define N_CmdObj_CudaProgram                       107
#define N_CmdObj_ProxyGpuProgram                   108

#define N_CmdObj_PixelBuffer                       200
#define N_CmdObj_UIPixelGrid                       201

#define N_CmdObj_UIWidgetAide_ID                   5001
#define N_CmdObj_UITooltipWidgetAide               5002
#define N_CmdObj_UICursorWidgetAide                5003

#define N_CmdObj_Custom                            1000000
#define N_CmdObj_CustomMax                         9000000

///////////////////////////////////////////////////////////////////
/////////////////////////////////////////////
///////////////////////////////
///////////////////
////////
// Property

#define N_PropertyDefine                                    1                
#define N_PropertyID                                        2
#define N_PropertyName                                      3
#define N_PropertyGroup                                     4
#define N_PropertyType                                      5
#define N_PropertySource                                    6
#define N_PropertyValue                                     7
#define N_PropertyVersion                                   8
#define N_PropertyTrue                                      9
#define N_PropertyFalse                                     10

#define N_PropertyResource_Group                            51
#define N_PropertyResource_Src                              52
#define N_PropertyResource_Size                             53
#define N_PropertyResource_CSize                            54
#define N_PropertyResource_PID                              55
#define N_PropertyResource_RID                              56

#define N_PropertyMesh_ID                                   101
#define N_PropertyMesh_PrefabType                           102

#define N_PropertyTexture_ID                                201

#define N_PropertySkeleton_ID                               301

#define N_PropertyGpuProgram_ID                             401
#define N_PropertyGpuProgram_Language                       402
#define N_PropertyGpuProgram_Type                           403
#define N_PropertyGpuProgram_Syntax                         404
#define N_PropertyGpuProgram_Main                           405
#define N_PropertyGpuProgram_Adjacency                      406
#define N_PropertyGpuProgram_Src                            407
#define N_PropertyGpuProgram_Param                          408
#define N_PropertyGpuProgram_ParamIndex                     409
#define N_PropertyGpuProgram_ParamName                      410
#define N_PropertyGpuProgram_SParamIndex                    411
#define N_PropertyGpuProgram_SParamName                     412
#define N_PropertyGpuProgram_ParamValue                     413
#define N_PropertyGpuProgram_ASM                            414
#define N_PropertyGpuProgram_Micro                          415
#define N_PropertyGpuProgram_ExtMicro                       416
#define N_PropertyGpuProgram_Skeletal                       417
#define N_PropertyGpuProgram_VertexUnit                     418
#define N_PropertyGpuProgram_VertexOffset                   419
#define N_PropertyGpuProgram_VerTexFetch                    420
#define N_PropertyGpuProgram_ManualNamedConstant            421

#define N_PropertyGpuProgramGLSL_Attach                     450
#define N_PropertyGpuProgramGLSL_ColumnMajor                451
#define N_PropertyGpuProgramGLSL_PreDefines                 452
#define N_PropertyGpuProgramGLSL_InputOpType                453
#define N_PropertyGpuProgramGLSL_OutOpType                  454
#define N_PropertyGpuProgramGLSL_MaxOutputVertices          455

#define N_PropertyMatrial_ID                                501

#define N_PropertyFont_ID                                   601
#define N_PropertyFont_Src                                  602
#define N_PropertyFont_Type                                 603
#define N_PropertyFont_FreeType                             604
#define N_PropertyFont_Pixmap                               605
#define N_PropertyFont_NativeRes                            606
#define N_PropertyFont_AutoRes                              607
#define N_PropertyFont_Size                                 608
#define N_PropertyFont_LineSpace                            609
#define N_PropertyFont_AntiAlias                            610
#define N_PropertyFont_PixelMap                             611
#define N_PropertyFont_CharDefine                           612
#define N_PropertyFont_CharTextureIndx                      613
#define N_PropertyFont_CharCodeValue                        614
#define N_PropertyFont_CharPos                              615
#define N_PropertyFont_CharSize                             616
#define N_PropertyFont_CharPosOft                           617
#define N_PropertyFont_CharXOft                             618

#define N_PropertyLight_Type                                701
#define N_PropertyLight_Spot                                702
#define N_PropertyLight_Point                               703
#define N_PropertyLight_Directional                         704
#define N_PropertyLight_Visible                             705
#define N_PropertyLight_PowerScale                          706
#define N_PropertyLight_Diffuse                             707
#define N_PropertyLight_Specular                            708
#define N_PropertyLight_Attenuation                         709
#define N_PropertyLight_Position                            710
#define N_PropertyLight_Direction                           711
#define N_PropertyLight_CastShadows                         712
#define N_PropertyLight_ShadowFarDistance                   713
#define N_PropertyLight_SpotlightInner                      714
#define N_PropertyLight_SpotlightOuter                      715
#define N_PropertyLight_SpotlightFalloff                    716

#define N_PropertyExtTexture_ShaderFusion                   801
#define N_PropertyExtTexture_ShaderCh                       802
#define N_PropertyExtTexture_RenderLevel                    803
#define N_PropertyExtTexture_FileName                       804
#define N_PropertyExtTexture_FPS                            805
#define N_PropertyExtTexture_PlayMode                       806

#define N_PropertyPixelOrbit_MaxElement                     901
#define N_PropertyPixelOrbit_NumOfChain                     902
#define N_PropertyPixelOrbit_UseTexCoord                    903
#define N_PropertyPixelOrbit_UseVerColour                   904

#define N_PropertyPixelChain_MaxElement                     1001
#define N_PropertyPixelChain_NumOfChain                     1002
#define N_PropertyPixelChain_UseTexCoord                    1003
#define N_PropertyPixelChain_UseVerColour                   1004

#define N_PropertyParticleSpace_Template                    1101
#define N_PropertyParticleSpace_PoolSize                    1102
#define N_PropertyParticleSpace_SpreadMax                   1102
#define N_PropertyParticleSpace_MaterialID                  1102
#define N_PropertyParticleSpace_CullItself                  1102
#define N_PropertyParticleSpace_ParticleWidth               1102
#define N_PropertyParticleSpace_ParticleHeight              1102
#define N_PropertyParticleSpace_Geo                         1102
#define N_PropertyParticleSpace_SortEnable                  1102
#define N_PropertyParticleSpace_LocalSpace                  1102
#define N_PropertyParticleSpace_TimeInterval                1102
#define N_PropertyParticleSpace_TimeThreshold               1102

#define N_PropertyParticleCtrl_PoolSize                     1201
#define N_PropertyParticleCtrl_ExtData                      1202

#define N_PropertyAlterGpuProgram_Alter                     1401

#define N_PropertyGpuProgramParam_ParamName                 1501

#define N_PropertyParticleSpace_Material                    1601   

#define N_PropertyContainerOverlayView_SelfView             1701
#define N_PropertyContainerOverlayView_UVCoords             1702

#define N_PropertyFrameOverlayView_BorderSize               1801
#define N_PropertyFrameOverlayView_LeftUV                   1802
#define N_PropertyFrameOverlayView_TopUV                    1803
#define N_PropertyFrameOverlayView_RightUV                  1804
#define N_PropertyFrameOverlayView_BorderUV                 1805
#define N_PropertyFrameOverlayView_CentreUV                 1806
#define N_PropertyFrameOverlayView_TopLeftUV                1807
#define N_PropertyFrameOverlayView_BottomLeftUV             1808
#define N_PropertyFrameOverlayView_BottomRightUV            1809
#define N_PropertyFrameOverlayView_TopRightUV               1810

#define N_PropertyOverlayGeo_Left                           1901
#define N_PropertyOverlayGeo_Top                            1902
#define N_PropertyOverlayGeo_Width                          1903
#define N_PropertyOverlayGeo_Height                         1904
#define N_PropertyOverlayGeo_Material                       1905
#define N_PropertyOverlayGeo_Caption                        1906
#define N_PropertyOverlayGeo_MetricsMode                    1907
#define N_PropertyOverlayGeo_HorizontalAlign                1908
#define N_PropertyOverlayGeo_VerticalAlign                  1909
#define N_PropertyOverlayGeo_Visible                        1910

#define N_PropertyParticleCtrlGeo_CtrlMode                  2001
#define N_PropertyParticleCtrlGeo_OriginType                2002
#define N_PropertyParticleCtrlGeo_RotationMode              2003
#define N_PropertyParticleCtrlGeo_CtrlDirection             2004
#define N_PropertyParticleCtrlGeo_CtrlUp                    2005
#define N_PropertyParticleCtrlGeo_PointSprites              2006
#define N_PropertyParticleCtrlGeo_ParallelView              2007

#define N_PropertyRectOverlayGeo_TexRepeat                  2101
#define N_PropertyRectOverlayGeo_UVCoords                   2102

#define N_PropertySpreadParticle_Angle                      2201
#define N_PropertySpreadParticle_Colour                     2202
#define N_PropertySpreadParticle_MinColour                  2203
#define N_PropertySpreadParticle_MaxColour                  2204
#define N_PropertySpreadParticle_Direction                  2205
#define N_PropertySpreadParticle_SpreadRate                 2206
#define N_PropertySpreadParticle_SpreadSpeed                2207
#define N_PropertySpreadParticle_MinSpreadSpeed             2208
#define N_PropertySpreadParticle_MaxSpreadSpeed             2209
#define N_PropertySpreadParticle_MinTTL                     2210
#define N_PropertySpreadParticle_MaxTTL                     2211
#define N_PropertySpreadParticle_Position                   2212
#define N_PropertySpreadParticle_Duration                   2213
#define N_PropertySpreadParticle_MinDuration                2214
#define N_PropertySpreadParticle_MaxDuration                2215
#define N_PropertySpreadParticle_RepeatDelay                2216
#define N_PropertySpreadParticle_MinRepeatDelay             2217
#define N_PropertySpreadParticle_MaxRepeatDelay             2218
#define N_PropertySpreadParticle_Name                       2219
#define N_PropertySpreadParticle_SpreadName                 2220
#define N_PropertySpreadParticle_Up                         2221

#define N_PropertyTextCharGeo_GlyphHeight                   2301
#define N_PropertyTextCharGeo_SpaceWidth                    2302
#define N_PropertyTextCharGeo_FontID                        2303
#define N_PropertyTextCharGeo_Colour                        2304
#define N_PropertyTextCharGeo_AlignMode                     2305

#define N_PropertyUIButton_Select                           2401

#define N_PropertyUICombobox_ReadOnly                       2401
#define N_PropertyUICombobox_ValidationString               2402
#define N_PropertyUICombobox_CaretIndex                     2403
#define N_PropertyUICombobox_SelectionStartIndex            2404
#define N_PropertyUICombobox_SelectionLength                2405
#define N_PropertyUICombobox_MaxTextLength                  2406
#define N_PropertyUICombobox_SortEnable                     2407
#define N_PropertyUICombobox_VScrollAlwaysShow              2408
#define N_PropertyUICombobox_HScrollAlwaysShow              2409
#define N_PropertyUICombobox_SingleClickEnable              2410
#define N_PropertyUICombobox_AutoSizeListHeight             2411
#define N_PropertyUICombobox_AutoSizeListWidth              2412

#define N_PropertyUIContainer_TransmitCursor                2501
#define N_PropertyUIContainer_BroadcastInput                2502

#define N_PropertyUICursorWidgetAide_MouseCursor            2601

#define N_PropertyUIText_HLayout                            2802
#define N_PropertyUIText_VLayout                            2803
#define N_PropertyUIText_VScrollbarEnable                   2804
#define N_PropertyUIText_HScrollbarEnable                   2805
#define N_PropertyUIText_HTextExtent                        2806
#define N_PropertyUIText_VTextExtent                        2807

#define N_PropertyUIStatic_FrameEnable                      2901
#define N_PropertyUIStatic_BgEnable                         2902

#define N_PropertyUIDragContainer_DragEnable                2901
#define N_PropertyUIDragContainer_DragAlpha                 2902
#define N_PropertyUIDragContainer_DragThreshold             2903
#define N_PropertyUIDragContainer_DragCursor                2904
#define N_PropertyUIDragContainer_StickyMode                2905

#define N_PropertyUIDragDropWindow_DragDropEnable           3001

#define N_PropertyUIEditbox_ReadOnly                        3101
#define N_PropertyUIEditbox_MarkMode                        3102
#define N_PropertyUIEditbox_MarkChar                        3103
#define N_PropertyUIEditbox_RegexString                     3104
#define N_PropertyUIEditbox_Input                           3105
#define N_PropertyUIEditbox_SelectBegin                     3106
#define N_PropertyUIEditbox_SelectLength                    3107
#define N_PropertyUIEditbox_MaxLength                       3108
#define N_PropertyUIEditbox_BlinkEnable                     3109
#define N_PropertyUIEditbox_BlinkTimeout                    3110
#define N_PropertyUIEditbox_TextAlign                       3111
#define N_PropertyUIEditbox_SelectTextColour                3112
#define N_PropertyUIEditbox_UnselectTextColour				3113

#define N_PropertyUIMultiEditbox_SelectTextColour           3201
#define N_PropertyUIMultiEditbox_UnselectTextColour         3202
#define N_PropertyUIMultiEditbox_ActiveSelectionColour      3203
#define N_PropertyUIMultiEditbox_InactiveSelectionColour    3204

#define N_PropertyUIFrameWindow_SizingEnable                3301
#define N_PropertyUIFrameWindow_FrameEnable                 3302
#define N_PropertyUIFrameWindow_TitleBarEnable              3303
#define N_PropertyUIFrameWindow_CloseButtonEnable           3304
#define N_PropertyUIFrameWindow_RollUpEnable                3305
#define N_PropertyUIFrameWindow_RollUp                      3306
#define N_PropertyUIFrameWindow_DragTitleBar                3307
#define N_PropertyUIFrameWindow_BorderSize                  3308
#define N_PropertyUIFrameWindow_TBSizingCursor              3309
#define N_PropertyUIFrameWindow_LRSizingCursor              3310
#define N_PropertyUIFrameWindow_BRSizingCursor              3311
#define N_PropertyUIFrameWindow_BLSizingCursor              3312

#define N_PropertyUIGridLayoutContainer_GridSize            3401
#define N_PropertyUIGridLayoutContainer_LayoutType          3402

#define N_PropertyUIItemListbox_MultiSelectEnable           3501

#define N_PropertyUIListbox_SortEnable                      3601
#define N_PropertyUIListbox_MultiselectEnable               3602
#define N_PropertyUIListbox_VScrollAlwaysShow               3603
#define N_PropertyUIListbox_HScrollAlwaysShow               3604
#define N_PropertyUIListbox_TooltipsEnable                  3605

#define N_PropertyUIListContainer_AutoResize                3701
#define N_PropertyUIListContainer_SortMode                  3702

#define N_PropertyUIListEntry_SelectEnable                  3801
#define N_PropertyUIListEntry_Select                        3802

#define N_PropertyUIListHeader_SortingEnable                3901
#define N_PropertyUIListHeader_ColumnSizingEnable           3902
#define N_PropertyUIListHeader_ColumnDraggingEnable         3903
#define N_PropertyUIListHeader_SortSegmentID                3904
#define N_PropertyUIListHeader_SortDirection                3905
#define N_PropertyUIListHeader_SegmentWidgetType            3906

#define N_PropertyUIListHeaderSegment_SizingEnable          4101
#define N_PropertyUIListHeaderSegment_ClickEnable           4102
#define N_PropertyUIListHeaderSegment_DragMovingEnable      4103
#define N_PropertyUIListHeaderSegment_SortDirection         4104
#define N_PropertyUIListHeaderSegment_SizingCursorImage     4105
#define N_PropertyUIListHeaderSegment_MovingCursorImage     4106

#define N_PropertyUIMenubar_ItemSpace                       4201
#define N_PropertyUIMenubar_MultiPopup                      4202
#define N_PropertyUIMenubar_AutoClosePopup                  4203

#define N_PropertyUIMenuItem_PopupOffset                    4301
#define N_PropertyUIMenuItem_AutoPopupTimeout               4302

#define N_PropertyUIMouseControlItem_AutoEvent              4401
#define N_PropertyUIMouseControlItem_Rate                   4402
#define N_PropertyUIMouseControlItem_Threshold              4403

#define N_PropertyUIMultiColumnList_ColumnSizingEnable      4501
#define N_PropertyUIMultiColumnList_ColumnDraggingEnable    4502
#define N_PropertyUIMultiColumnList_SortEnable              4503
#define N_PropertyUIMultiColumnList_SortDirection           4504
#define N_PropertyUIMultiColumnList_VScrollAlwaysShow       4505
#define N_PropertyUIMultiColumnList_HScrollAlwaysShow       4506
#define N_PropertyUIMultiColumnList_DefaultSelectionColumn  4507
#define N_PropertyUIMultiColumnList_DefaultSelectionRow     4508
#define N_PropertyUIMultiColumnList_RowCount                4509
#define N_PropertyUIMultiColumnList_SelectionMode           4510
#define N_PropertyUIMultiColumnList_ColumnAutoSize          4511
#define N_PropertyUIMultiColumnList_addColumn               4512

#define N_PropertyUIMultiLineEditbox_ReadOnly               4601
#define N_PropertyUIMultiLineEditbox_CaretIndex             4602
#define N_PropertyUIMultiLineEditbox_SelectionStartIndex    4603
#define N_PropertyUIMultiLineEditbox_SelectionLength        4604
#define N_PropertyUIMultiLineEditbox_MaxTextLength          4605
#define N_PropertyUIMultiLineEditbox_WordWrapped            4606
#define N_PropertyUIMultiLineEditbox_SelectionImage         4607
#define N_PropertyUIMultiLineEditbox_VScrollAlwaysShow      4608
#define N_PropertyUIMultiLineEditbox_BlinkCaret             4609
#define N_PropertyUIMultiLineEditbox_CaretBlinkTimeout      4610

#define N_PropertyUINumSpinner_Current                      4701
#define N_PropertyUINumSpinner_StepValue                    4702
#define N_PropertyUINumSpinner_MinValue                     4703
#define N_PropertyUINumSpinner_MaxValue                     4704
#define N_PropertyUINumSpinner_NumMode                      4705

#define N_PropertyUIFadeInTime                              4801
#define N_PropertyUIFadeOutTime                             4802

#define N_PropertyUIPosButton_PosEvent                      4901
#define N_PropertyUIPosButton_YRange                        4902
#define N_PropertyUIPosButton_XRange                        4903
#define N_PropertyUIPosButton_YMoveEnable                   4904
#define N_PropertyUIPosButton_XMoveEnable                   4905

#define N_PropertyUIProgressBar_Progress                    5001
#define N_PropertyUIProgressBar_Step                        5002
#define N_PropertyUIProgressBar_Vertical                    5003
#define N_PropertyUIProgressBar_Reverse                     5004

#define N_PropertyUIRadioButton_Group                       5101

#define N_PropertyUIScrollbar_DocSize                       5201
#define N_PropertyUIScrollbar_PageSize                      5202
#define N_PropertyUIScrollbar_StepSize                      5203
#define N_PropertyUIScrollbar_OverlapSize                   5204
#define N_PropertyUIScrollbar_Current                       5205
#define N_PropertyUIScrollbar_CurrentUnit                   5206
#define N_PropertyUIScrollbar_AutoEndPos                    5207
#define N_PropertyUIScrollbar_Vertical                      5208

#define N_PropertyUIForceVScrollbar_ForceVScrollbar         5301
#define N_PropertyUIForceVScrollbar_ForceHScrollbar         5302
#define N_PropertyUIForceVScrollbar_HStepSize               5303
#define N_PropertyUIForceVScrollbar_HPageRemain             5304
#define N_PropertyUIForceVScrollbar_HScrollPos              5305
#define N_PropertyUIForceVScrollbar_VStepSize               5306
#define N_PropertyUIForceVScrollbar_VPageRemain             5307
#define N_PropertyUIForceVScrollbar_VScrollPos              5308
#define N_PropertyUIForceVScrollbar_ContentAutoSize         5310
#define N_PropertyUIForceVScrollbar_ContentArea             5311

#define N_PropertyUIScrolledListContainer_VScrollAlwaysShow 5401
#define N_PropertyUIScrolledListContainer_HScrollAlwaysShow 5402

#define N_PropertyUISizeContainer_AutoSize                  5501
#define N_PropertyUISizeContainer_ContentArea               5502
#define N_PropertyUISizeContainer_AutoContentArea           5503

#define N_PropertyUISlider_Current                          5601
#define N_PropertyUISlider_MaxValue                         5602
#define N_PropertyUISlider_StepValue                        5603
#define N_PropertyUISlider_Vertical                         5604
#define N_PropertyUISlider_Reverse                          5605

#define N_PropertyUITabControl_TabHeight                    5701
#define N_PropertyUITabControl_TabTextPadding               5702
#define N_PropertyUITabControl_TabType                      5703
#define N_PropertyUITabControl_ButtonType                   5704

#define N_PropertyUITitlebar_DragEnable                     5801

#define N_PropertyUIToggleButton_On                         5901

#define N_PropertyUITooltip_HoverTime                       6001
#define N_PropertyUITooltip_DisplayTime                     6002

#define N_PropertyUITooltipWidgetAide_TooltipType           6101
#define N_PropertyUITooltipWidgetAide_TooltipText           6102
#define N_PropertyUITooltipWidgetAide_ParentTooltipText     6103

#define N_PropertyUITree_SortEnable                         6201
#define N_PropertyUITree_MultiselectEnable                  6202
#define N_PropertyUITree_VScrollAlwaysShow                  6203
#define N_PropertyUITree_HScrollAlwaysShow                  6204
#define N_PropertyUITree_ItemTooltipsEnable                 6205

#define N_PropertyUIWidget_ID                               6301
#define N_PropertyUIWidget_Name                             6302
#define N_PropertyUIWidget_RelArea                          6303
#define N_PropertyUIWidget_Pos                              6304
#define N_PropertyUIWidget_VAlign                           6305
#define N_PropertyUIWidget_HAlign                           6306
#define N_PropertyUIWidget_Size                             6307
#define N_PropertyUIWidget_MinSize                          6308
#define N_PropertyUIWidget_MaxSize                          6309
#define N_PropertyUIWidget_ScaleMode                        6310
#define N_PropertyUIWidget_ScaleFactor                      6311
#define N_PropertyUIWidget_PixelAlign                       6312
#define N_PropertyUIWidget_NamePath                         6313
#define N_PropertyUIWidget_Alpha                            6314
#define N_PropertyUIWidget_AutoClip                         6315
#define N_PropertyUIWidget_AutoDestroy                      6316
#define N_PropertyUIWidget_EnvAlpha                         6317
#define N_PropertyUIWidget_Visible                          6318
#define N_PropertyUIWidget_Disable                          6319
#define N_PropertyUIWidget_Margin                           6320
#define N_PropertyUIWidget_RestorePreFocus                  6321
#define N_PropertyUIWidget_CursorMultiClick                 6322
#define N_PropertyUIWidget_View                             6323
#define N_PropertyUIWidget_Style                            6324
#define N_PropertyUIWidget_UpdateMode                       6325
#define N_PropertyUIWidget_SubmitCursorEvent                6326
#define N_PropertyUIWidget_AutoWindow                       6327
#define N_PropertyUIWidget_TextLayout                       6328
#define N_PropertyUIWidget_Text                             6329
#define N_PropertyUIWidget_TextColour                       6330
#define N_PropertyUIWidget_Font                             6331
#define N_PropertyUIWidget_ZOrderClick                      6332
#define N_PropertyUIWidget_Top                              6333
#define N_PropertyUIWidget_ZOrder                           6334
#define N_PropertyUIWidget_Image                            6335
#define N_PropertyUIWidget_ExtData                          6336

#define N_PropertyUIWindow_Overgo                           6501
#define N_PropertyUIWindow_AutoRender                       6502
#define N_PropertyUIWindow_Rotation                         6503

#define N_PropertyUIScheme_ViewUnit                         7501
#define N_PropertyUIScheme_PixelGroup                       7502
#define N_PropertyUIScheme_FontUnit                         7503
#define N_PropertyUIScheme_WidgetModule                     7504
#define N_PropertyUIScheme_WidgetFactory                    7505
#define N_PropertyUIScheme_WidgetAlias                      7506
#define N_PropertyUIScheme_WidgetView                       7507
#define N_PropertyUIScheme_Style                            7508
#define N_PropertyUIScheme_WidgetViewModule                 7509
#define N_PropertyUIScheme_WidgetViewFactory                7510
#define N_PropertyUIScheme_WidgetAliasName                  7511
#define N_PropertyUIScheme_WidgetAliasTarget                7512
#define N_PropertyUIScheme_WidgetViewID                     7513
#define N_PropertyUIScheme_WidgetViewStyleID                7514
#define N_PropertyUIScheme_WidgetViewEffectID               7515
#define N_PropertyUIScheme_WidgetViewModelID                7516

#define N_PropertyUILayout_Define                           8001
#define N_PropertyUILayout_Event                            8002
#define N_PropertyUILayout_FuncName                         8003

#define N_PropertyUIPixelGrid_Colour                        9001
#define N_PropertyUIPixelGrid_Font                          9002
#define N_PropertyUIPixelGrid_Image                         9003
#define N_PropertyUIPixelGrid_Window                        9004
#define N_PropertyUIPixelGrid_VertAlignment                 9005
#define N_PropertyUIPixelGrid_Padding                       9006
#define N_PropertyUIPixelGrid_TopPadding                    9007
#define N_PropertyUIPixelGrid_BottomPadding                 9008
#define N_PropertyUIPixelGrid_LeftPadding                   9010
#define N_PropertyUIPixelGrid_RightPadding                  9011
#define N_PropertyUIPixelGrid_AspectLock                    9012
#define N_PropertyUIPixelGrid_ImageSize                     9013
#define N_PropertyUIPixelGrid_ImageWidth                    9014
#define N_PropertyUIPixelGrid_ImageHeight                   9015
#define N_PropertyUIPixelGrid_TopAlignedValue               9016
#define N_PropertyUIPixelGrid_BottomAlignedValue            9017
#define N_PropertyUIPixelGrid_CentreAlignedValue            9018
#define N_PropertyUIPixelGrid_StretchAlignedValue           9019

#define N_PropertyUIConfig_Tooltip                          10001
#define N_PropertyUIConfig_Font                             10002
#define N_PropertyUIConfig_AutoLoadRes                      10003
#define N_PropertyUIConfig_Log                              10004
#define N_PropertyUIConfig_ResGroup                         10005
#define N_PropertyUIConfig_Script                           10006
#define N_PropertyUIConfig_XmlPattern                       10007
#define N_PropertyUIConfig_ImageCodec                       10008
#define N_PropertyUIConfig_CursorView                       10009
#define N_PropertyUIConfig_LogLevel                         10010
#define N_PropertyUIConfig_ResPattern                       10011
#define N_PropertyUIConfig_ResPath                          10012
#define N_PropertyUIConfig_InitExec                         10013
#define N_PropertyUIConfig_TerminateExec                    10014
#define N_PropertyUIConfig_CursorImage                      10015

#define N_PropertyApuProgram_ID                             11001
#define N_PropertyApuProgram_Syntax                         11002
#define N_PropertyApuProgram_Type                           11003
#define N_PropertyApuProgram_Kernel                         11004
#define N_PropertyApuProgram_Src                            11005
#define N_PropertyApuProgram_PlatformIndex                  11006
#define N_PropertyApuProgram_DeviceIndex                    11007
#define N_PropertyApuProgram_ContextIndex                   11008
#define N_PropertyApuProgram_ASM                            11009

#define N_PropertyPixelBuffer_Pixel                         100101
#define N_PropertyPixelBuffer_TextureSrc                    100102
#define N_PropertyPixelBuffer_PosX                          100103
#define N_PropertyPixelBuffer_PosY                          100104
#define N_PropertyPixelBuffer_PosXOffset                    100105
#define N_PropertyPixelBuffer_PosYOffset                    100106
#define N_PropertyPixelBuffer_Width                         100107
#define N_PropertyPixelBuffer_Height                        100108
#define N_PropertyPixelBuffer_NativeResX                    100109
#define N_PropertyPixelBuffer_NativeResY                    100110
#define N_PropertyPixelBuffer_AutoRes                       100111

#define N_PropertyWindow_KeyBoard                           200501
#define N_PropertyWindow_Mouse                              200502
#define N_PropertyWindow_Joystick                           200503
#define N_PropertyWindow_Title                              200504
#define N_PropertyWindow_Border                             200505
#define N_PropertyWindow_Left                               200506
#define N_PropertyWindow_Top                                200507
#define N_PropertyWindow_DepthBuffer                        200508
#define N_PropertyWindow_ColourDepth                        200509
#define N_PropertyWindow_Width                              200510
#define N_PropertyWindow_Height                             200511
#define N_PropertyWindow_FullWindow                         200512
#define N_PropertyWindow_AdjustSize                         200513
#define N_PropertyWindow_PixelWidth                         200514
#define N_PropertyWindow_PixelHeight                        200515
#define N_PropertyWindow_sRGB                               200516
#define N_PropertyWindow_Stereo                             200517
#define N_PropertyWindow_Vsync                              200518
#define N_PropertyWindow_VsyncInterval                      200519
#define N_PropertyWindow_Frequency                          200520
#define N_PropertyWindow_VideoMode                          200521
#define N_PropertyWindow_Hide                               200522
#define N_PropertyWindow_FSAA                               200523
#define N_PropertyWindow_FSAAHint                           200524
#define N_PropertyWindow_TBOMode                            200525
#define N_PropertyWindow_Gamma                              200526
#define N_PropertyWindow_ExtGLContext                       200527
#define N_PropertyWindow_GLContext                          200528
#define N_PropertyWindow_CurrentGLContext                   200529
#define N_PropertyWindow_WindowPrc                          200530
#define N_PropertyWindow_Handle                             200531
#define N_PropertyWindow_ParentHandle                       200532
#define N_PropertyWindow_MonitorHandle                      200533
#define N_PropertyWindow_MonitorIndex                       200534
#define N_PropertyWindow_EnableDoubleClick                  200535

#define N_PropertyWindow_GTKContainer                       200536 
#define N_PropertyWindow_wxWidgetPanel                      200537 
#define N_PropertyWindow_wxWidgetMoveX                      200538
#define N_PropertyWindow_wxWidgetMoveY                      200539

#define N_PropertySound_Type
#define N_PropertySound_Gain                                200601
#define N_PropertySound_Pitch                               200602
#define N_PropertySound_Param                               200603
#define N_PropertySound_File                                200604
#define N_PropertySound_Fade                                200605
#define N_PropertySound_FadeIn                              200606
#define N_PropertySound_FadeOut                             200607
#define N_PropertySound_FadeInEnable                        200608
#define N_PropertySound_FadeOutEnable                       200609
#define N_PropertySound_ReplayDelay                         200610
#define N_PropertySound_MaxDistance                         200611
#define N_PropertySound_MinDistance                         200612
#define N_PropertySound_Alpha                               200613
#define N_PropertySound_Sub                                 200614
#define N_PropertySound_SubMark                             200615
#define N_PropertySound_TimeFactor                          200616
#define N_PropertySound_FusionType                          200617
#define N_PropertySound_SubIndex                            200618
#define N_PropertySound_TimeIndex                           200619
#define N_PropertyValueSound_Order                          200620
#define N_PropertyValueSound_TimeOrder                      200621
#define N_PropertyValueSound_Fusion                         200622

#define N_Property_Custom                                   1000000
#define N_Property_CustomMax                                9000000

#endif