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

#ifndef _NII_PLUGIN_H_
#define _NII_PLUGIN_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 插件
    @remarks 可以有效的控制运行时库的总大小/性能/效果
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Plugin : public PluginAlloc
    {
    public:
        enum Type
        {
            T_Process_Main      = 1,
            T_Shadow_Shader     = 2,
            T_Process_Net       = 3,
            T_Process_Image     = 4,
            T_Input_System      = 5,
            T_Render_System     = 6,
            T_Sound_System      = 7,
            T_Xml_Process       = 8,
            T_Audio_Code        = 9,
            T_Video_Code        = 10,
            T_SPU_Code          = 11,
            T_Demux_Code        = 12,
            T_Image_Code        = 13,
            T_Compress_Data     = 14,
            T_Game_Model        = 15,
            T_ALL               = 16
        };
    public:
        Plugin(PluginID pid, Type type);
        virtual ~Plugin();

        /** 获取插件ID
        @version NIIEngine 3.0.0
        */
        PluginID getID() const;

        /** 获取插件的名字
        @remark 辅助名字
        @version NIIEngine 3.0.0
        */
        virtual String getName() const = 0;

        /** 获取NII对应的版本
        @version NIIEngine 3.0.0
        */
        virtual String Version() const = 0;

        /** 获取NII对应的授权协议
        @version NIIEngine 3.0.0
        */
        virtual String getLisence() const = 0;

        /** 获取这个插件的描述
        @version NIIEngine 3.0.0
        */
        virtual String getVerbose() const = 0;

        /** 安装插件
        @remark 运行时库挂靠到引擎中
        @version NIIEngine 3.0.0
        */
        virtual void install() = 0;

        /** 初始化这个插件
        @remark 插件正式在引擎中起作用
        @version NIIEngine 3.0.0
        */
        virtual void init() = 0;

        /** 停用这个插见
        @remark 插件在引擎中失去效果
        @verison NIIEngine 3.0.0
        */
        virtual void shutdown() = 0;

        /** 卸载插件
        @remark 运行时库从引擎中解除
        */
        virtual void uninstall() = 0;

        /** 获取第三方库的授权协议
        @remark
            一般使用了商业,LPGL等第三方存在约束的动态库时才存在,如果不存在这种
            情况的话,则和 getLisence() 函数结果一致
        @version NIIEngine 3.0.0
        */
        virtual String getOwnLisence() const = 0;

        /** 获取第三方库的版本
        @remark
            一般使用了商业,LPGL等第三方存在约束的动态库时才存在,如果不存在这种
            情况的话,则和 getLisence() 函数结果一致
        @version NIIEngine 3.0.0
        */
        virtual String getOwnVersion() const = 0;

        /** 插件性能
        @remark 和效果对立
        @version NIIEngine 3.0.0
        */
        virtual NIIb getPerformance() const = 0;

        /** 插件效果
        @remark 和性能对立
        @version NIIEngine 3.0.0
        */
        virtual NIIb getEffectiveness() const = 0;

        /** 插件复杂度
        @remark 和扩展度对立
        @version NIIEngine 3.0.0
        */
        virtual NIIb getComplex() const = 0;

        /** 插件扩展度
        @remark 和复杂度度对立
        @version NIIEngine 3.0.0
        */
        virtual NIIb getExtention() const = 0;

        /** 输出细节到记事文本中
        @version NIIEngine 3.0.0
        */
        void log() const;
    private:
        PluginID mID;
        Type mType;
    };
}
#endif