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

#ifndef _NII_RENDERSYS_H_
#define _NII_RENDERSYS_H_

#include "NiiPreInclude.h"
#include "NiiSysOption.h"
#include "NiiDriverVersion.h"
#include "NiiRenderFeature.h"
#include "NiiExtFrameBuffer.h"
#include "NiiGeometryRaw.h"
#include "NiiShaderCh.h"
#include "NiiCommon.h"
#include "NiiDataType.h"
#include "NiiPlane.h"

namespace NII
{
    #define Independent N_StrBlank

    typedef vector<ViewWindow *>::type WindowList;
    /** 渲染系统
    @note 部分函数很接近底层
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderSys : public SysAlloc
    {
        friend class Engine;
        friend class SpaceManager;
    public:
        typedef map<String, FrameObj *>::type FrameList;
        typedef multimap<GroupID, FrameObj *>::type FrameLevelList;
    public:
        RenderSys();
        virtual ~RenderSys();

        /** 初始化系统
        @version NIIEngine 3.0.0
        */
        virtual void init();

        /** 重新初始化系统
        @version NIIEngine 3.0.0
        */
        virtual void reinit() = 0;

        /** 关闭系统
        @version NIIEngine 3.0.0
        */
        virtual void shutdown();

        /** 返回渲染系统名字.
        @version NIIEngine 3.0.0
        */
        virtual const String & getName() const = 0;

        /** 构建配置
        @version NIIEngine 3.0.0
        */
        virtual void setupConfig();

        /** 应用/检测自定义属性
        @version NIIEngine 3.0.0
        */
        virtual String checkConfig() = 0;

        /** 设置自定义属性
        @param[in] name 名字
        @param[in] value 值
        @version NIIEngine 3.0.0
        */
        virtual void setConfig(const String & name, const String & value);

        /** 获取自定义属性集
        @version NIIEngine 3.0.0
        */
        inline const SysOptionList & getConfig() const { return mOptions; }

        /** 获取自定义属性集数据
        @version NIIEngine 3.0.0
        */
        inline const PropertyData * getConfigData() const { return &mOptionData; }

        /** 构建渲染系统特性
        @version NIIEngine 3.0.0
        */
        virtual void setupFeature() = 0;

        /** 设置自定义渲染系统特性
        @remark 参数内存交由本类管理
        @version NIIEngine 3.0.0 高级api
        */
        virtual void setCustomFeature(RenderFeature * rsf);

        /** 获取渲染系统能力
        @version NIIEngine 3.0.0
        */
        inline RenderFeature * getFeature() const { N_assert(mFeature, "SYS Uninitialized");  return mFeature; }

        /** 构建固定编程管线着色程序参数
        @version NIIEngine 5.0.0
        */
        virtual void setupFFPParam();

        /** 获取固定编程管线着色程序参数
        @verison NIIEngine 5.0.0
        */
        inline GpuProgramParam * getFFPParam() const { N_assert(mFFGPP, "SYS Uninitialized"); return mFFGPP; }

        /** 设置动态灯光
        @version NIIEngine 3.0.0
        */
        virtual void setLight(bool b) = 0;

        /** 设置光体计算模式
        @version NIIEngine 3.0.0
        */
        virtual void setShadingType(ShadeMode sm) = 0;

        /** 设置环境(灯光)颜色
        @version NIIEngine 3.0.0 高级api
        */
        virtual void setAmbient(const Colour & c) = 0;

        /** 设置是否使用同步垂直
        @remark Has NO effec
        @version NIIEngine 3.0.0 高级api
        */
        void setVSync(bool b);

        /** 获取是否使用同步垂直
        @version NIIEngine 3.0.0 高级api
        */
        bool isVSync() const;

        /** 设置是否使用深度缓存
        @version NIIEngine 3.0.0 高级api
        */
        void setWBuffer(bool b);

        /** 获取是否使用深度缓存
        @version NIIEngine 3.0.0 高级api
        */
        bool getWBuffer() const;

        /** 设置投影矩阵
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _setProj(const Matrix4f & m) = 0;

        /** 设置视图矩阵
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _setView(const Matrix4f & m) = 0;

        /** 设置模型矩阵
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _setModel(const Matrix4f & m) = 0;

        /** 设置几何空间矩阵
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _setSpace(const Matrix4f * m, NCount cnt);

        /** 设置动态灯光
        @version NIIEngine 3.0.0
        */
        virtual void _setLight(NCount cnt) = 0;

        /** 创建一个新的渲染窗体
        @param[in] w 窗体的宽
        @param[in] h 窗体的高
        @version NIIEngine 3.0.0
        */
        virtual ViewWindow * createView(const String & name, NCount w, NCount h, bool full, const PropertyData * ext = 0) = 0;

        /** 加入渲染帧对象
        @version NIIEngine 3.0.0
        */
        virtual void add(FrameObj * obj);

        /** 移去渲染帧对象
        @version NIIEngine 3.0.0
        */
        virtual FrameObj * remove(const FrameObj * obj);

        /** 创建多个窗体
        @version NIIEngine 3.0.0
        */
        virtual bool createView(const WindowDefineList & dlist, WindowList & out);

        /** 创建复合渲染帧对象
        @version NIIEngine 3.0.0
        */
        virtual MultiTextureFrame * createMultiFrame(const String & name) = 0;

        /** 删除渲染帧对象
        @version NIIEngine 3.0.0
        */
        virtual void destroyFrame(const FrameObj * fo);

        /** 获取渲染帧对象
        @version NIIEngine 3.0.0
        */
        virtual FrameObj * getFrame(const String & name);

        /** 创建扩展缓存
        @version NIIEngine 3.0.0
        */
        virtual void createExtBuffer(FrameObj * obj);

        /** 删除扩展缓存
        @version NIIEngine 3.0.0
        */
        void destroyAllExtBuffer(bool all = true);

        /** 创建一个像素查询
        @version NIIEngine 3.0.0
        */
        virtual void create(PixelCountQuery *& obj) = 0;

        /** 删除一个像素查询
        @version NIIEngine 3.0.0
        */
        virtual void destroy(PixelCountQuery * obj);

        /** 设置顶点缓存
        @version NIIEngine 3.0.0
        */
        void setInstanceData(VertexBuffer * buf);

        /** 获取顶点缓存
        @version NIIEngine 3.0.0
        */
        VertexBuffer * getInstanceBuffer() const;

        /** 设置复用顶点
        @version NIIEngine 3.0.0
        */
        void setInstanceData(VertexData * vd);

        /** 获取复用顶点
        @version NIIEngine 3.0.0
        */
        VertexData * getInstanceData() const;

        /** 设置复用顶点次数
        @version NIIEngine 3.0.0
        */
        void setInstanceCount(NCount b);

        /** 获取复用顶点次数
        @version NIIEngine 3.0.0
        */
        NCount getInstanceCount() const;

        /** 设置视口类型
        @versin NIIEngine 4.0.0
        */
        virtual bool setViewportType(ViewportType type);

        /** 创建颜色属性
        @versin NIIEngine 4.0.0
        */
        virtual ShaderChColour * createColour(const String & name = Independent, bool Bindtosys = false) const;

        /** 获取颜色属性
        @versin NIIEngine 4.0.0
        */
        virtual ShaderChColour * getColour(const String & name) const;

        /** 创建渲染点的属性
        @version NIIEngine 3.0.0
        */
        virtual ShaderChPoint * createPoint(const String & name = Independent, bool Bindtosys = false) const;

        /** 创建渲染点的属性
        @version NIIEngine 3.0.0
        */
        virtual ShaderChPoint * getPoint(const String & name) const;

        /** 创建颜色缓存
        @version NIIEngine 3.0.0
        */
        virtual ShaderChBlend * createBlend(const String & name = Independent, bool Bindtosys = false) const;

        /** 创建颜色缓存
        @version NIIEngine 3.0.0
        */
        virtual ShaderChBlend * getBlend(const String & name) const;

        /** 创建透明度测试
        @version NIIEngine 3.0.0
        */
        virtual ShaderChAlpha * createAlpha(const String & name = Independent, bool Bindtosys = false) const;

        /** 创建透明度测试
        @version NIIEngine 3.0.0
        */
        virtual ShaderChAlpha * getAlpha(const String & name) const;

        /** 创建雾模式
        @version NIIEngine 3.0.0
        */
        virtual ShaderChFog * createFog(const String & name = Independent, bool Bindtosys = false) const;

        /** 创建雾模式
        @version NIIEngine 3.0.0
        */
        virtual ShaderChFog * getFog(const String & name) const;

        /** 创建深度测试
        @version NIIEngine 3.0.0
        */
        virtual ShaderChDepth * createDepth(const String & name = Independent, bool Bindtosys = false) const;

        /** 创建深度测试
        @version NIIEngine 3.0.0
        */
        virtual ShaderChDepth * getDepth(const String & name) const;

        /** 创建模板测试
        @version NIIEngine 3.0.0
        */
        virtual ShaderChStencil * createStencil(const String & name = Independent, bool Bindtosys = false) const;

        /** 创建模板测试
        @version NIIEngine 3.0.0
        */
        virtual ShaderChStencil * getStencil(const String & name) const;

        /** 创建纹理混合
        @version NIIEngine 3.0.0
        */
        virtual TextureBlend * createTexBlend(const String & name = Independent, bool Bindtosys = false) const;

        /** 获取纹理混合
        @version NIIEngine 3.0.0
        */
        virtual TextureBlend * getTexBlend(const String & name) const;

        /** 获取纹理样本
        @version NIIEngine 3.0.0
        */
        virtual TextureSample * createTexSample(const String & name = Independent, bool Bindtosys = false) const;

        /** 获取纹理样本
        @version NIIEngine 3.0.0
        */
        virtual TextureSample * getTexSample(const String & name) const;

        /** 设置当前颜色属性
        @version NIIEngine 3.0.0
        */
        virtual void setColour(const ShaderChColour & c, Nmark applymark = 0xFFFFFFFF) = 0;

        /** 设置当前颜色属性
        @version NIIEngine 3.0.0
        */
        void setColour(const String & name, Nmark applymark = 0xFFFFFFFF);

        /** 设置渲染点的属性
        @version NIIEngine 3.0.0
        */
        virtual void setPoint(const ShaderChPoint & p, Nmark applymark = 0xFFFFFFFF) = 0;

        /** 设置渲染点的属性
        @version NIIEngine 3.0.0
        */
        void setPoint(const String & name, Nmark applymark = 0xFFFFFFFF);

        /** 设置颜色缓存
        @version NIIEngine 3.0.0
        */
        virtual void setBlend(const ShaderChBlend & b, Nmark applymark = 0xFFFFFFFF) = 0;

        /** 设置颜色缓存
        @version NIIEngine 3.0.0
        */
        void setBlend(const String & name, Nmark applymark = 0xFFFFFFFF);

        /** 设置透明度测试
        @version NIIEngine 3.0.0
        */
        virtual void setAlpha(const ShaderChAlpha & a, Nmark applymark = 0xFFFFFFFF) = 0;

        /** 设置透明度测试
        @version NIIEngine 3.0.0
        */
        void setAlpha(const String & name, Nmark applymark = 0xFFFFFFFF);

        /** 设置雾模式
        @version NIIEngine 3.0.0
        */
        virtual void setFog(const ShaderChFog & f, Nmark applymark = 0xFFFFFFFF) = 0;

        /** 设置雾模式
        @version NIIEngine 3.0.0
        */
        void setFog(const String & name, Nmark applymark = 0xFFFFFFFF);

        /** 设置深度测试
        @version NIIEngine 3.0.0
        */
        virtual void setDepth(const ShaderChDepth & d, Nmark applymark = 0xFFFFFFFF) = 0;

        /** 设置深度测试
        @version NIIEngine 3.0.0
        */
        void setDepth(const String & name, Nmark applymark = 0xFFFFFFFF);

        /** 设置模版测试参数
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void setStencil(const ShaderChStencil & d, Nmark applymark = 0xFFFFFFFF) = 0;

        /** 设置模版测试
        @version NIIEngine 3.0.0
        */
        void setStencil(const String & name, Nmark applymark = 0xFFFFFFFF);

        /** 设置固定管线着色程序参数
        @version NIIEngine 4.0.0
        */
        virtual void setFFPParam(const GpuProgramParam & param, Nmark typemark) = 0;

        /** 设置深度测试(分离操作)
        @see ShaderChDepth
        @note 用于共面三角形的前后区分
        @version NIIEngine 3.0.0
        */
        virtual void _setDepthOpFunc(CmpMode func = CPM_LESS_EQUAL) = 0;

        /** 设置深度偏量(分离操作)
        @see ShaderChDepth
        @note 用于共面三角形的前后区分
        @version NIIEngine 3.0.0
        */
        virtual void _setDepthBias(NIIf bias, NIIf slopeScale = 0.0f) = 0;

        /** 设置深度偏量(分离操作)
        @param[in] derive 如果为真,深度偏差 = base + multi(current) * basefactor
        @see ShaderChDepth
        @note 用于共面三角形的前后区分
        @version NIIEngine 3.0.0
        */
        virtual void _setDepthBias(bool derive, NIIf base = 0.0f, NIIf basefactor = 0.0f, NIIf slopeScale = 0.0f);

        /** 获取深度缓存最小值
        @version NIIEngine 3.0.0
        */
        virtual NIIf getMinDepth() = 0;

        /** 获取深度缓存最大值
        @version NIIEngine 3.0.0
        */
        virtual NIIf getMaxDepth() = 0;

        /** 设置颜色掩码
        @param[in] mark ColourMark一个或多个组合
        @version NIIEngine 3.0.0
        */
        virtual void setColourWrite(Nmark mark) = 0;

        /** 设置模版测试
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void setStencilTest(bool set) = 0;

        /** 设置光栅模式
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _setPolygonMode(PolygonMode m) = 0;

        /** 设置顶点饶向拣选模式
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _setCullingMode(CullingMode m) = 0;

        /** 获取顶点饶向拣选模式
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual CullingMode _getCullingMode() const;

        /** 设置是否顶点反向绕
        @version NIIEngine 3.0.0
        */
        virtual void setInvertVertexWinding(bool b);

        /** 获取是否顶点反向绕
        @version NIIEngine 3.0.0
        */
        virtual bool isInvertVertexWinding();

        /** 绑定渲染帧
        @version NIIEngine 3.0.0
        */
        virtual void _setFrame(FrameObj * fo) = 0;

        /** 启用纹理
        @note 原始概念
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _enableTexture(Nidx index, const ShaderChTextureUnit * tl);

        /** 启用纹理单元数量
        @note 原始概念
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _enableTexture(NCount count);

        /** 关闭纹理单元
        @note 原始概念
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _disableTexture(Nidx index);

        /** 绑定纹理
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _bindTexture(Nidx index, ResourceID rid, bool enable);

        /** 绑定纹理
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _bindTexture(Nidx index, const Texture * tex, bool enable) = 0;

        /** 绑定纹理顶点
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _bindVertexTexture(Nidx index, const Texture * tex);

        /** 设置纹理单元使用的坐标
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _setTextureCoordSet(Nidx index, Nidx setidx) = 0;

        /** 设置纹理的变形空间
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _setTextureMatrix(Nidx index, const Matrix4f & space) = 0;

        /** 纹理混合模式
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _setTextureBlend(Nidx index, const TextureBlend & tb) = 0;

        /** 纹理样本模式
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _setTextureSample(Nidx indx, const TextureSample & ts) = 0;

        /** 设置纹理寻址计算
        @param[in] rel 投影(环境)纹理用
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _setTextureCoordPrc(Nidx index, TextureCoordPrc m, const Frustum * rel = 0) = 0;

        /** 通知渲染系统,相对于一个不同原点,它应该调整纹理投影
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _setTextureProjRel(bool enable, const Vector3f & pos);

        /** 获取实际纹理元素x偏移
        @version NIIEngine 3.0.0
        */
        virtual NIIf getTexelXOffset() = 0;

        /** 获取实际纹理元素y偏移
        @version NIIEngine 3.0.0
        */
        virtual NIIf getTexelYOffset() = 0;

        /** 设置法线是否自动单位化
        @version NIIEngine 3.0.0
        */
        virtual void setUnitNormals(bool set) = 0;

        /** 设置视口
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual void _setViewport(Viewport * vp) = 0;

        /** 获取视口
        @note 原始概念
        @version NIIEngine 3.0.0
        */
        virtual Viewport * _getViewport();

        /** 清除缓存
        @version NIIEngine 3.0.0
        */
        virtual void clearBuffer(Nmark buffers, const Colour & colour = Colour::Black, NIIf depth = 1.0f, Nui16 stencil = 0) = 0;

        /** 设置裁剪区域
        @version NIIEngine 3.0.0
        */
        virtual void setScissorTest(bool enable, NCount left = 0, NCount top = 0, NCount right = 0, NCount bottom = 0) = 0;

        /** 设置裁剪区域
        @param[in] clip
        @version NIIEngine 3.0.0
        */
        virtual void setClipPlanes(const PlaneList & clip);

        /** 添加裁剪面
        @version NIIEngine 3.0.0
        */
        virtual void addClipPlane(const Plane & clip);

        /** 添加裁剪面
        @version NIIEngine 3.0.0
        */
        virtual void addClipPlane(NIIf a, NIIf b, NIIf c, NIIf d);

        /** 清除裁剪区域
        @version NIIEngine 3.0.0
        */
        virtual void resetClipPlanes();

        /** 设置是否固定管线模式
        @note 如果启用,将关闭可编程管线
        @version NIIEngine 3.0.0
        */
        void setFixedPipelineMode(bool b);

        /** 获取是否固定管线模式
        @note 如果启用,将关闭可编程管线
        @version NIIEngine 3.0.0
        */
        bool isFixedPipelineMode() const;

        /** 启用着色程序
        @version NIIEngine 3.0.0
        */
        virtual void on(GpuProgram * prg);

        /** 绑定着色程序参数
        @version NIIEngine 3.0.0
        */
        virtual void on(GpuProgram::ShaderType type, GpuProgramParam * params, Nmark range) = 0;

        /** 禁用指定类型着色程序
        @version NIIEngine 3.0.0
        */
        virtual void off(GpuProgram::ShaderType type);

        /*** 是否启用了指定类型着色程序
        @version NIIEngine 3.0.0
        */
        virtual bool isOn(GpuProgram::ShaderType type);

        /** 设置渲染次数(多渲染)
        @param[in] count 渲染当前状态的次数
        */
        virtual void setRenderCount(NCount count);

        /** 渲染几何元
        @note 把所有状态设置好了才渲染几何元
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _render(const GeometryRaw & op);

        /** 初始化帧对象内容
        @version NIIEngine 3.0.0
        */
        virtual void _initAllFrame();

        /** 更新帧对象内容
        @param[in] swap 向渲染系统交换显示内存
        @version NIIEngine 3.0.0
        */
        virtual void _updateAllFrame(bool swap = true);

        /** 向渲染系统交换显示内存
        @version NIIEngine 3.0.0
        */
        virtual void _swapAllFrame(bool vsync = true);

        /** 重至几何数量
        @version NIIEngine 3.0.0
        */
        virtual void _resetGeoCount();

        /** 开始帧
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _beginFrame() = 0;

        /** 暂停渲染
        @version NIIEngine 3.0.0 高级api
        */
        virtual FrameObj * _pauseFrame();

        /** 恢复渲染
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _resumeFrame(FrameObj * fo);

        /** 结束当前视口的每帧渲染
        @version NIIEngine 3.0.0
        */
        virtual void _endFrame() = 0;

        /** 转换成系统原始颜色的格式
        @note DT_Colour_ARGB 或 DT_Colour_ABGR 之前的转换
        @version NIIEngine 3.0.0
        */
        virtual void convertType(const Colour & src, Nui32 * dest);

        /** 获取渲染系统原始的颜色格式
        @note 返回 DT_Colour_ARGB 或 DT_Colour_ABGR
        @version NIIEngine 3.0.0
        */
        virtual DataType getColourType() const = 0;

        /** 获取渲染系统专用的投影矩阵
        @version NIIEngine 3.0.0
        */
        virtual void getSysProj(const Matrix4f & in, Matrix4f & out, bool gpuprog = false) = 0;

        /** 获取渲染系统专用的投影矩阵
        @version NIIEngine 3.0.0
        */
        virtual void getSysProj(const Radian & fovy, NIIf aspect, NIIf near, NIIf far, Matrix4f & out, bool gpuprog = false) = 0;

        /** 获取渲染系统专用的投影矩阵
        @version NIIEngine 3.0.0
        */
        virtual void getSysProj(NIIf l, NIIf r, NIIf b, NIIf t, NIIf near, NIIf far, Matrix4f & out,  bool gpuprog = false) = 0;

        /** 获取渲染系统专用的正交投影矩阵
        @version NIIEngine 3.0.0
        */
        virtual void getSysOrtho(const Radian & fovy, NIIf aspect, NIIf near, NIIf far, Matrix4f & out,  bool gpuprog = false) = 0;

        /** 推导渲染系统专用的倾斜深度投影
        @param[in] 输入和输出
        @version NIIEngine 3.0.0
        */
        virtual void getObliqueDepthProj(const Plane & plane, Matrix4f & io, bool gpuprog) = 0;

        /** 获取显示器的数量
        @version NIIEngine 3.0.0
        */
        virtual NCount getDisplayMonitorCount() const = 0;

        /** 获取系统材质方案
        @note opengl 和 d3d 至少在着色程序上有所区别
        @version NIIEngine 3.0.0
        */
        virtual SchemeID getMaterialScheme() const;

        /** 返回驱动版本
        @version NIIEngine 3.0.0
        */
        virtual const DriverVersion & getDriverVersion() const;

        /** 获取当前渲染顶点数量
        @version NIIEngine 3.0.0
        */
        virtual NCount getVertexCount() const;

        /** 获取当前渲染三角形数量
        @version NIIEngine 3.0.0
        */
        virtual NCount getTriangleCount() const;

        /** 获取当前渲染批次数量
        @version NIIEngine 3.0.0
        */
        virtual NCount getBatchCount() const;

        /** 错误码代表的字符串
        @version NIIEngine 3.0.0
        */
        virtual String getErrorString(NIIl num) const = 0;
    protected:
        /** 下一重复渲染
        @return 是否还需要重复渲染
        @version NIIEngine 3.0.0 高级api
        */
        virtual bool updateParam();

        /** 创建帧扩展缓存
        @version NIIEngine 3.0.0 高级api
        */
        virtual ExtFrameBuffer * createExtFrameImpl(FrameObj * dst) = 0;

        /** 设置裁减面
        @version NIIEngine 3.0.0 高级api
        */
        virtual void setClipPlanesImpl(const PlaneList & clip) = 0;

        /** 初始化渲染系统
        @version NIIEngine 3.0.0 高级api
        */
        virtual void initImpl(ViewWindow * primary) = 0;

        /** 通知摄象机被删除了
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _notifyDestroy(const Camera * cam);
    public:
        static const Matrix4f ProjClipSpace2DToImageSpace;
    protected:
        typedef list<PixelCountQuery *>::type PixelCountQueryList;
        typedef map<GroupID, ExtFrameBufferList>::type ExtFrameList;
        typedef map<String, ShaderChColour *>::type ColourList;
        typedef map<String, ShaderChPoint *>::type PointList;
        typedef map<String, ShaderChBlend *>::type BlendList;
        typedef map<String, ShaderChAlpha *>::type AlphaList;
        typedef map<String, ShaderChFog *>::type FogList;
        typedef map<String, ShaderChDepth *>::type DepthList;
        typedef map<String, ShaderChStencil *>::type StencilList;
        typedef map<String, TextureBlend *>::type TexBlendList;
        typedef map<String, TextureSample *>::type TexSampleList;
    protected:
        String mName;
        DriverVersion mVersion;
        RenderFeature * mVendorFeature;
        Viewport * mActiveViewport;
        FrameObj * mActiveFrame;
        GpuProgramParam * mActiveVGPP;
        GpuProgramParam * mActiveGGPP;
        GpuProgramParam * mActiveFGPP;
        GpuProgramParam * mActiveTGPP;
        GpuProgramParam * mFFGPP;
        RenderFeature * mFeature;
        TextureManager * mTextureManager;
        ExtFrameList mExtFrameList;
        FrameList mFrameObjList;
        FrameLevelList mFrameLevelList;
        PixelCountQueryList mPixelQueryList;
        CullingMode mCullingMode;
        PlaneList mClipPlanes;
        Colour mTextureBlend[NII_MAX_TextureLayer][2];
        NCount mEnableTexCount;
        NCount mVertexCount;
        NCount mTriangleCount;
        NCount mBatchCount;
        NCount mRenderCount;
        NCount mCurrentRender;
        Vector3f mTextureProjRel;
        Nui32 mVSyncInterval;
        NIIf mDepthBiasBase;
        NIIf mDepthBiasFactor;
        NIIf mDepthBiasSlopeScale;
        NCount mInstanceCount;
        VertexBuffer * mInstanceBuffer;
        VertexData * mInstanceData;
        SysOptionList mOptions;
        PropertyData mOptionData;
        ColourList mColourList;
        PointList mPointList;
        BlendList mBlendList;
        AlphaList mAlphaList;
        FogList mFogList;
        DepthList mDepthList;
        StencilList mStencilList;
        TexBlendList mTexBlendList;
        TexSampleList mTexSampleList;
        bool mFixedPipelineMode;
        bool mVertexTextureShared;
        bool mVPMode;
        bool mGPMode;
        bool mFPMode;
        bool mClipValid;
        bool mAutoDepthBias;
        bool mInvertVertexWinding;
        bool mSysFeatureValid;
        bool mTexProjRelative;
        bool mWBuffer;
        bool mVSync;
    };
    typedef vector<RenderSys *>::type RenderSysList;
}
#endif