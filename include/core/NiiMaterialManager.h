/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-2-1

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
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
        void recover();

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

        /** 关联对象
        @note 如果 ShaderFusion 是单独存在的,不使用这个方式
        @param[in] dst
        @param[in] obj
        @version NIIEngine 3.0.0
        */
        void attach(Material * dst, ShaderFusion * obj);

        /** 回收对象自动删除内存
        @note 如果 ShaderFusion 是单独存在的,不使用这个方式
        @param[in] dst
        @param[in] obj
        @version NIIEngine 3.0.0
        */
        void detach(Material * dst, ShaderFusion * obj);

        /** 关联对象
        @note 如果 ShaderCh 是单独存在的,不使用这个方式
        @param[in] dst
        @param[in] obj
        @version NIIEngine 3.0.0
        */
        void attach(ShaderFusion * dst, ShaderCh * obj);

        /** 回收对象自动删除内存
        @note 如果 ShaderCh 是单独存在的,不使用这个方式
        @param[in] dst
        @param[in] obj
        @version NIIEngine 3.0.0
        */
        void detach(ShaderFusion * dst, ShaderCh * obj);

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

        /// @copydetails Singleton::getOnly
        static MaterialManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static MaterialManager * getOnlyPtr();

        static const ShaderChColour * InvalidColour;
        static const ShaderChBlend  * InvalidBlend;
        static const ShaderChAlpha  * InvalidAlpha;
        static const ShaderChDepth  * InvalidDepth;
        static const ShaderChTexture *InvalidTexture;
        static const ShaderChProgram *InvalidProgram;
        static const ShaderChPoint  * InvalidPoint;
        static const ShaderChFog    * InvalidFog;
        
        static const Material * WhiteLight;
        static const Material * WhiteColour;
    protected:
        /// @copydetails ResourceManager::createImpl
        Resource * createImpl(ResourceID rid, GroupID gid, ResourceLoadScheme * ls, 
			ResourceResultScheme * rs,const PropertyData * params);

        /** 内部调用初始化
        @version NIIEngine 3.0.0 高级api
        */
        void init();
    protected:
        typedef list<MaterialMatchListener *>::type Listeners;
        typedef map<SchemeID, Listeners>::type ListenerMap;
        typedef map<ShaderCh *, ShaderFusionList>::type ShaderChLink;
        typedef map<ShaderFusion *, MaterialList>::type ShaderFusionLink;
        typedef map<Material *, NCount>::type MaterialLink;
    protected:
        Material * mNull;
        SchemeID mCurrent;
        ShaderChLink mChLink;
        ShaderFusionLink mFusionLink;
        MaterialLink mMaterialLink;
        ShaderChList mDetachList;
        ListenerMap mListenerMap;
        TextureFilterOP mMin;
        TextureFilterOP mMag;
        TextureFilterOP mMip;
        Nui32 mMaxAniso;
    };
}
#endif