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

#ifndef _NII_MATERIAL_MANAGER_H_
#define _NII_MATERIAL_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiResourceManager.h"
#include "NiiMaterialMatchListener.h"
#include "NiiMaterial.h"

#define BaseMaterialWhite   2

namespace NII
{
    /** 材质管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MaterialManager : public ResourceManager, public Singleton<MaterialManager>
    {
        friend class Engine;
    public:
        MaterialManager();
        virtual ~MaterialManager();

        using ResourceManager::remove;
        using ResourceManager::destroy;

        /** 执行回收机制
        @version NIIEngine 3.0.0
        */
        inline void recover()   { ShaderFusion::recover(); }

        /** 设置当前使用的方案
        @version NIIEngine 3.0.0
        */
        void setCurrent(SchemeID sid);

        /** 获取当前使用的方案
        @version NIIEngine 3.0.0
        */
        SchemeID getCurrent() const;

        /** 返回空材质
        @remark 一般用于一个不存在的材质替代
        @version NIIEngine 3.0.0
        */
        Material * getNull() const;

        /** 匹配材质
        @version NIIEngine 3.0.0
        */
        ShaderFusion * match(const GeometryObj * geo, const Material * obj, Nidx lod);

        /** 通知对象改变
        @version NIIEngine 3.0.0
        */
        void notify(ShaderCh * obj);

        /** 通知对象改变
        @version NIIEngine 3.0.0
        */
        void notify(ShaderFusion * obj);

        /** 设置环境颜色
        @version NIIEngine 3.0.0
        */
        void setAmbientColour(Material * obj, const Colour & c);

        /** 设置环境颜色
        @version NIIEngine 3.0.0
        */
        void setAmbientColour(ShaderFusion * obj, const Colour & c);

        /** 设置深度检测
        @version NIIEngine 3.0.0
        */
        void setDepthCheck(Material * obj, bool set);

        /** 设置深度检测
        @version NIIEngine 3.0.0
        */
        void setDepthCheck(ShaderFusion * obj, bool set);

        /** 设置深度写入
        @version NIIEngine 3.0.0
        */
        void setDepthWrite(Material * obj, bool set);

        /** 设置深度写入
        @version NIIEngine 3.0.0
        */
        void setDepthWrite(ShaderFusion * obj, bool set);

        /** 设置层混合模式
        @version NIIEngine 3.0.0
        */
        void setBlend(Material * obj, FrameBlendMode fbm);

        /** 设置层混合模式
        @version NIIEngine 3.0.0
        */
        void setBlend(ShaderFusion * obj, FrameBlendMode fbm);

        /** 设置层混合模式
        @version NIIEngine 3.0.0
        */
        void setBlend(Material * obj, FrameBlendMode fbm, FrameBlendMode afbm);

        /** 设置层混合模式
        @version NIIEngine 3.0.0
        */
        void setBlend(ShaderFusion * obj, FrameBlendMode fbm, FrameBlendMode afbm);

        /** 设置层混合模式
        @version NIIEngine 3.0.0
        */
        void setBlend(Material * obj, ColourFactor src, ColourFactor dest);

        /** 设置层混合模式
        @versin NIIEngine 3.0.0
        */
        void setBlend(ShaderFusion * obj, ColourFactor src, ColourFactor dest);

        /** 创建纹理层动画.
        @param[in] unit 需要控制的对象
        @param[in] dur 时间总量(单位:毫秒)
        @version NIIEngine 3.0.0
        */
        DataEquation<TimeDurMS, NIIf> * createAnimation(ShaderChTextureUnit * unit, TimeDurMS dur);

        /** 创建基于时间纹理uv滚动
        @param[in] unit 需要控制的对象
        @param[in] speed
        @version NIIEngine 3.0.0
        */
        DataEquation<TimeDurMS, NIIf> * createUVScroll(ShaderChTextureUnit * unit, NIIf speed);

        /** 创建基于时间纹理u滚动
        @param[in] unit 需要控制的对象
        @param[in] speed 水平滚动速度
        @version NIIEngine 3.0.0
        */
        DataEquation<TimeDurMS, NIIf> * createUScroll(ShaderChTextureUnit * unit, NIIf speed);

        /** 创建基于时间纹理v滚动
        @param[in] unit 需要控制的对象
        @param[in] speed 垂直滚动速度
        @version NIIEngine 3.0.0
        */
        DataEquation<TimeDurMS, NIIf> * createVScroll(ShaderChTextureUnit * unit, NIIf speed);

        /** 创建基于时间纹理uv缩放
        @param[in] unit 需要控制的对象
        @param[in] speed
        @version NIIEngine 3.0.0
        */
        DataEquation<TimeDurMS, NIIf> * createUVScale(ShaderChTextureUnit * unit, NIIf speed);

        /** 创建基于时间纹理u缩放
        @param[in] unit 需要控制的对象
        @param[in] speed
        @version NIIEngine 3.0.0
        */
        DataEquation<TimeDurMS, NIIf> * createUScale(ShaderChTextureUnit * unit, NIIf speed);

        /** 创建基于时间纹理v缩放
        @param[in] unit 需要控制的对象
        @param[in] speed
        @version NIIEngine 3.0.0
        */
        DataEquation<TimeDurMS, NIIf> * createVScale(ShaderChTextureUnit * unit, NIIf speed);

        /** 创建基于时间纹理旋转.
        @param[in] unit 需要控制的对象
        @param[in] speed
        @version NIIEngine 3.0.0
        */
        DataEquation<TimeDurMS, NIIf> * createRotate(ShaderChTextureUnit * unit, NIIf speed);

        /** 删除时间等式
        @version NIIEngine 3.0.0
        */
        void destroy(DataEquation<TimeDurMS, NIIf> * obj);

        /** 设置纹理过滤模式
        @version NIIEngine 3.0.0
        */
        void setTextureFilter(TextureFilterMode mode);

        /** 设置纹理过滤模式
        @version NIIEngine 3.0.0
        */
        void setTextureFilter(TextureFilterType type, TextureFilterOP op);

        /** 设置纹理过滤模式
        @version NIIEngine 3.0.0
        */
        TextureFilterOP getTextureFilter(TextureFilterType type) const;

        /** 设置纹理各异向
        @version NIIEngine 3.0.0
        */
        void setTextureAniso(Nui32 aniso);

        /** 获取纹理各异向
        @version NIIEngine 3.0.0
        */
        Nui32 getTextureAniso() const;

        /** 添加材质匹配监听器
        @version NIIEngine 3.0.0
        */
        void add(SchemeID sid, MaterialMatchListener * l);

        /** 移去材质匹配监听器
        @version NIIEngine 3.0.0
        */
        void remove(SchemeID sid, MaterialMatchListener * l);

        /// @see ScriptParserSys::parse
        void parse(DataStream * stream, GroupID gid, PtrList & out);

        static const ShaderChColour * InvalidColour;
        static const ShaderChBlend  * InvalidBlend;
        static const ShaderChStencil * InvalidStencil;
        static const ShaderChTexture *InvalidTexture;
        static const ShaderChProgram *InvalidProgram;
        static const ShaderChPoint  * InvalidPoint;
        static const ShaderChFog    * InvalidFog;
        
        static const Material * WhiteLight;
        static const Material * WhiteColour;
        
        static SchemeID Default;
    protected:
        /// @copydetails ResourceManager::createImpl
        Resource * createImpl(ResourceID rid, GroupID gid, ResLoadScheme * ls, 
            ResResultScheme * rs,const PropertyData * params);

        /** 内部调用初始化
        @version NIIEngine 3.0.0 高级api
        */
        void init();
    protected:
        typedef list<MaterialMatchListener *>::type Listeners;
        typedef map<SchemeID, Listeners>::type ListenerMap;
        typedef map<Material *, NCount>::type MaterialLink;
    protected:
        Material * mNull;
        SchemeID mCurrent;
        MaterialLink mMaterialLink;
        ListenerMap mListenerMap;
        TextureFilterOP mMin;
        TextureFilterOP mMag;
        TextureFilterOP mMip;
        Nui32 mMaxAniso;
    };
}
#endif