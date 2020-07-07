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

#ifndef _NII_SHADER_CH_PROGRAM_H_
#define _NII_SHADER_CH_PROGRAM_H_

#include "NiiPreInclude.h"
#include "NiiGpuProgram.h"

namespace NII
{
    class ShaderChShader;

    /** 应用着色程序
    @note 性质特殊,不能在任意渲染通路中切换
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShaderChProgram : public ShaderAlloc
    {
    public:
        /** 着色程序类型
        @see 注释
        @version NIIEngine 3.0.0
        */
        enum ShaderType
        {
            ST_VS = 0,       ///< 顶点(渲染物件时)
            ST_FS = 1,       ///< 片段(渲染物件时)
            ST_GS = 2,       ///< 几何(渲染物件时)
            ST_VS_SR = 3,    ///< 顶点阴影接收(渲染时段扩展)
            ST_FS_SR = 4,    ///< 片段阴影接收(渲染时段扩展)
            ST_VS_SC = 5,    ///< 顶点投射阴影(渲染时段扩展)
            ST_FS_SC = 6,    ///< 片段投射阴影(渲染时段扩展)
            ST_FS_N = 7,     ///< 法线片段(法线纹理用)(渲染时段扩展)
            ST_FS_F = 8,     ///< 镜面片段(倒影)(渲染时段扩展)
            ST_Count = 9
        };
    public:
        ShaderChProgram(ShaderCh * ch);
        ShaderChProgram(ShaderCh * ch, const ShaderChProgram & o);
        ShaderChProgram & operator =(const ShaderChProgram & o);
        ~ShaderChProgram();

        /** 是否存在着色程序
        @version NIIEngine 3.0.0
        */
        bool isExist() const;

        /** 是否存在此类型的着色程序
        @param[in] type 只能单个,不能组合
        @version NIIEngine 3.0.0
        */
        bool isExist(ShaderType type) const;

        /** 加入着色程序到队列中
        @remark 这些程序会按加入时的顺序运行,注意顺序
        @param[in] type 程序类型
        @param[in] gpid 资源ID
        @param[in] param 程序参数
        @version NIIEngine 3.0.0
        */
        void add(ShaderType type, GpuProgramID gpid, GpuProgramParam * param = 0);

        /** 移去指定类型的所有着色程序
        @param[in] type 程序类型
        @version NIIEngine 3.0.0
        */
        void remove(ShaderType type);

        /** 移去指定类型的ID着色程序
        @param[in] type 程序类型
        @param[in] gpid 资源ID
        @version NIIEngine 3.0.0
        */
        void remove(ShaderType type, GpuProgramID gpid);

        /** 移去所有类型的所有着色程序
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** 设置参数
        @param[in] type
        @param[in] index
        @param[in] gpid
        @param[in] param
        @version NIIEngine 3.0.0
        */
        void set(ShaderType type, GpuProgramID id, GpuProgramParam * param = 0, Nidx index = 0);

        /** 设置参数
        @param[in] type
        @param[in] index
        @param[in] gpid
        @param[in] param
        @version NIIEngine 3.0.0
        */
        void setParam(ShaderType type, GpuProgramParam * param, Nidx index = 0);

        /** 获取参数设置
        @param[in] index
        @param[in] type
        @param[in] out1
        @param[in] out2
        @version NIIEngine 3.0.0
        */
        void get(ShaderType type, GpuProgram * & outID, GpuProgramParam *& outParam, Nidx index = 0) const;
        
        /** 获取参数设置
        @param[in] index
        @param[in] type
        @param[in] out
        @version NIIEngine 3.0.0
        */
        GpuProgram * getProgram(ShaderType type, Nidx index = 0) const;

        /** 获取参数设置
        @param[in] index
        @param[in] type
        @param[in] out1
        @param[in] out2
        @version NIIEngine 3.0.0
        */
        GpuProgramParam * getParam(ShaderType type, Nidx index = 0) const;

        /** 局部化多线程
        @version NIIEngine 3.0.0
        */
        void plan();

        /** 局部化多线程
        @version NIIEngine 3.0.0
        */
        void unplan();

        /** 局部化多线程
        @version NIIEngine 3.0.0
        */
        void load();

        /** 局部化多线程
        @version NIIEngine 3.0.0
        */
        void unload();

        /** 自动更新参数
        @param[in] src
        @param[in] mark
        @version NIIEngine 3.0.0
        */
        void _update(const SysSyncParam * src, Nmark mark);
    private:
        typedef vector<ShaderChShader *>::type Shaders;
    private:
        ShaderCh * mParent;
        Shaders mShaders;
        Nui8 mMark[ST_Count];
    };
}
#endif