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

#ifndef _NII_GAME_EFFECT_H_
#define _NII_GAME_EFFECT_H_

#include "Nii.h"

namespace NIIGame
{
    /** 游戏对象技术_阴影级别
    @version NIIEngine 3.0.0
    */
    enum GameTech_Shadow
    {
        /** 仅仅对地面产生黑点,无论下方是否被任何东西遮挡,都会把阴影投向地面,这种阴影性能
            最好(模版缓存型)
        */
        GT_S_SPOT = 0x01,

        /** 就仅仅把影子影射到指定地面,物体上(模版缓存型)
        */
        GT_S_PlanarShadow = 0x02,

        /** 计算物体边缘衍生出阴影覆盖像素,如果摄像机在阴影里可能会有问题(模版缓存型)
        */
        GT_S_ShadowVolume = 0x02,

        /** 这种阴影技术继承ShadowVolume,做了大范围性能优化,但降低了部分精确度,如果对象本身
            是静态的可以选择和GE_L_Static共用,可加大性能(模版缓存型)
        */
        GT_S_ShadowVolume_X = 0x04,

        /** ShadowVolume能做出不含糊的阴影,这种技术需要额外的纹理空间,而且纹理越大越好,可以
            使用虚拟摄象机把阴影做得更好(纹理型)
        */
        GT_S_ShadowMapping = 0x08,

        /** 继承ShadowMapping,但大大降低了运行时的性能,全方位阴影,不像普通的ShadowMapping
            每个灯光需要一个深度纹理,而是每个灯光需要6+1个纹理,这种技术,可以实现那种被物体几
            乎全包围的灯光情况,建议尽量不使用这个技术(纹理型)
        */
        GT_S_ShadowMapping_Omnidirectional = 0x10,

        /// 强制关闭对象的阴影,无任何阴影
        GT_S_None = 0x20,

        /// 默认的状态
        GT_S_Defalut = 0x0;
    };

    /// 游戏对象技术_纹理级别
    enum GameTech_Texture
    {
        /**

        */
        GT_T_DumpMap = 0x01,

        /** 通过纹理阴暗成分在片段着色器中对渲染文理进行颜色处理,从而模拟出更多的法线计算
            量,从而减少三角形而模拟更多的三角形,让模型性能和质量都得到提高。注意必须支持片
            段着色器的gpu
        */
        GT_T_NormalMap = 0x02,

        /** 继承DumpMap

        */
        GT_T_ParallaxMap = 0x04,

        /**

        */
        GT_T_DisplacementMap = 0x08,

        /// 默认的状态
        GT_T_Defalut = 0x0
    };

    /// 游戏对象技术_灯光级别
    enum GameTech_Light
    {
        /** 这种灯光类型合适那些离物体很远很远的灯光,对于一普通物体移动来说照射面角度没有
            变化影响的话,合适使用这种技术
        */
        GT_L_Static = 0x01,

        /** 标准图形库中的灯光类型,这种灯光通过计算三角形的法线从而推导出来的颜色成分,即使
            标准图形库中存在三角形正面和反面的拣选,但如果场景中存在多个这样的灯光,性能也难
            以保证
        */
        GT_L_Dynamic = 0x02,

        /** 继承GE_L_Dynamic但执行了射线查询,标准图形库中的三角形的光照度是按法线计算,但在
            仿真世界里物体的光照可能会被其他或自身物体所遮挡,所以不能单纯按普通的按法线计算.
        */
        GT_L_Dynamic_RayQuery = 0x04,

        /// 仅仅是把图形可见面都照亮而且亮度一样,不存在任何方位/方向型灯光
        GT_L_Env = 0x08,

        /// 不开启任何照明设置,会产生一片黑的效果,但物体实际是被渲染出来了
        GT_L_None = 0x10,

        /// 默认的状态
        GT_L_Defalut = 0x0
    };

    /// 游戏对象技术_几何级别
    enum GameTech_Model
    {
        /** 通过射线算法把模型的三角形分割成多个逻辑块,通过摄象机距离和方向求出哪个逻辑块
            是可见的,注意这个算法很合适那些静态物体作为一个整体而且不合适那些透明物体
        */
        GT_M_AI = 0x01,

        /** 对于全局空间而言这个对象是处于不可动,状态不可改变的(包括灯光,和所有颜色属性,
            还有顶点或片段着色程序),这种技术合适那些在场景中出现多次的对象
        */
        GT_M_Static = 0x02,

        /// 不对几何模型分割,很合适那些只有不到50个顶点的物体
        GT_M_Little = 0x04,

        /// 默认的状态
        GT_M_Defalut = 0x04
    };

    /// 游戏对象技术_事件级别
    enum GameTech_Event
    {
        GT_E_Refuse = 0x01,     ///< 对象拒绝接收任何外部的事件驱动
        GT_E_Self = 0x02,       ///< 对象接收外来对象发出的任何事件驱动
        GT_E_Influence = 0x04,  ///< 这个技术能使对象产生新的驱动事件
        GT_E_Defalut = 0x01     ///< 默认的状态
    };

    /// 游戏对象技术_声音级别
    enum GameTech_Voice
    {
        /**
        */
        GT_V_3D,

        /**
        */
        GT_V_BG,

        /**
        */
        GT_V_Event,

        /**
        */
        GT_V_None = 0x10,       ///< 无声音

        GT_V_Default = 0x10     ///< 默认的状态
    };

    /// 游戏对象技术_摄象机
    enum GameTech_Camera
    {
        GT_C_Vague = 0x01,          ///< 模糊效果
        GT_C_Depth = 0x02,          ///< 包含影响深度
        GT_C_Square_Effect = 0x04,  ///< 含概的影响形状(正方形)(组合可以产生刺猬状)
        GT_C_Circle_Effect = 0x08,  ///< 含概的影响形状(圆形)
        GT_C_Ellipse_Effect = 0x10, ///< 含概的影响形状(椭圆形)
        GT_C_Normal =               ///< 普通情况,无任何效果
        GT_C_Default = 0x10         ///< 默认
    };

    /// 处理器技术_输入设备控制器
    enum PatternTech_Control
    {
        PT_C_Refuse = 0x01,     ///< 拒绝所有输入设备的事件驱动
        PT_C_Keyboard = 0x02,   ///< 接收键盘输入设备的事件驱动
        PT_C_Mouse = 0x04,      ///< 接收鼠标输入设备的事件驱动
        PT_C_Joystick = 0x08,   ///< 接收摇杆输入设备的事件驱动
        PT_C_Touch = 0x10,      ///< 接收触屏输入设备的事件驱动
        PT_C_Defalut = 0x20     ///< 默认状态
    };

    /// 处理器技术_传输控制器
    enum PatternTech_Internet
    {
        PT_I_Refuse = 0x01,     ///< 拒绝所有网络传输的事件驱动
        PT_I_UDP_In = 0x02,     ///< 接收UDP型网络传输的事件驱动
        PT_I_TCP_In = 0x04,     ///< 接收TCP型网络传输的事件驱动
        PT_I_UDP_Out = 0x08,    ///< 发送UDP型网络传输的事件驱动
        PT_I_TCP_Out = 0x10,    ///< 发送TCP型网络传输的事件驱动
        PT_I_Defalut = 0x01     ///< 默认状态
    };

    /// 处理器技术_物理控制器
    enum PatternTech_Physical
    {
        PT_P_Refuse = 0x01,     ///< 拒绝所有物理碰撞的事件驱动
        PT_P_Self = 0x02,       ///< 这个对象接收其他物体的物理碰撞
        PT_P_Influence = 0x04,  ///< 这个对象影响其他物体的物理位置
        PT_P_X = 0x08,          ///< 这个对象受x坐标的所物体的位置影响(相对于世界空间)
        PT_P_Y = 0x10,          ///< 这个对象受y坐标的所物体的位置影响(相对于世界空间)
        PT_P_Z = 0x20,          ///< 这个对象受z坐标的所物体的位置影响(相对于世界空间)
        PT_P_Defalut = 0x01     ///< 默认状态
    };

    enum GameEffect
    {
        GE_Glow = 0x01,                 ///< 光辉效果
        GE_WaterWaves = 0x02,           ///< 水波效果
        GE_WaterCaustics = 0x04,        ///< 水刻蚀
        GE_Perlin_Simplex_Noise = 0x08, ///< Simplex噪声
        GE_Perlin_Fractal_Noise = 0x10, ///< 分形噪声
        GE_Diffraction = 0x20,          ///< 光衍射
        GE_SubsurfaceScatter = 0x40,    ///< 表面散射
        GE_SBRDF = 0x80,                ///< 角度表面光线呈像
    };
}
#endif