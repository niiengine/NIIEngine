/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-8

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

#ifndef _NII_TEXTUREMANAGER_H_
#define _NII_TEXTUREMANAGER_H_

#include "NiiPreInclude.h"
#include "NiiResourceManager.h"
#include "NiiTexture.h"

namespace NII
{
    /** 纹理管理器类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TextureManager : public ResourceManager, public Singleton<TextureManager>
    {
    public:
        TextureManager();
        virtual ~TextureManager();

        /** 设置默认纹理位深
        @param[in] bit 位深
        @param[in] reload 重载所有纹理
        @version NIIEngine 3.0.0
        */
        void setBitDepth(NCount bit, bool reload = true);

        /** 获取默认纹理位深
        @version NIIEngine 3.0.0
        */
        NCount getBitDepth() const;

        /** 设置默认纹理LOD数量
        @note 默认为 0.
        @param[in] c LOD数量
        @version NIIEngine 3.0.0
        */
        void setMipMapCount(NCount c);

        /** 获取默认纹理LOD数量
        @version NIIEngine 3.0.0
        */
        NCount getMipMapCount() const;

        /** 创建纹理
        @param rid 资源句柄,这个句柄很可能是无定义的
        @version NIIEngine 3.0.0 高级api
        */
        Texture * createTexture(ResourceID rid, GroupID gid);

        /** 创建纹理
        @param[in] rid 资源句柄
        @param[in] w 宽
        @param[in] h 高
        @param[in] pf 像素格式
        @version NIIEngine 3.0.0 高级api
        */
        Texture * createTexture(ResourceID rid, GroupID gid, NCount w, NCount h, PixelFormat pf = PF_A8B8G8R8);

        /** 创建纹理
        @param rid 句柄
        @param file 文件
        @param gid 资源组
        @version NIIEngine 3.0.0 高级api
        */
        Texture * createTexture(ResourceID rid, GroupID gid, const String & file);

        /** 创建纹理
        @param rid 句柄
        @param gid 资源组
        @param d 深度
        @version NIIEngine 3.0.0 高级api
        */
        Texture * createTexture(ResourceID rid, GroupID gid, Texture::Type type, PixelFormat pf, NCount w, NCount h, 
            NCount d = 1, NIIi mipmaps = -1, Nmark usage = Texture::MM_MIPMAP_AUTO | Buffer::M_GPU | Buffer::M_WRITE,
            ResourceLoadScheme * ls = 0, ResourceResultScheme * rs = 0, bool HWsRGB = false, 
            NCount fsaa = 0, const String & fsaaHint = StrUtil::WBLANK);

        /** 安排资源
        @version NIIEngine 3.0.0
        */
        virtual Texture * plan(ResourceID rid, GroupID gid, Texture::Type type = Texture::T_2D,
            PixelFormat pf = PF_UNKNOWN, NIIi mipmaps = -1, NIIf gamma = 1.0f,
            Nmark mark = Texture::MM_AGP_COLOUR | Texture::MM_AGP_ALPHA);

        /** 加载
        @version NIIEngine 3.0.0
        */
        virtual Texture * load(ResourceID rid, GroupID gid, Texture::Type type = Texture::T_2D,
            PixelFormat pf = PF_UNKNOWN, NIIi mipmaps = -1, NIIf gamma = 1.0f,
            Nmark mark = Texture::MM_AGP_COLOUR | Texture::MM_AGP_ALPHA);

        /** 从图片中加载
        @param[in] img 像素来源
        @version NIIEngine 3.0.0
        */
        virtual Texture * load(ResourceID rid, GroupID gid, const Image & img,
            Texture::Type type = Texture::T_2D, PixelFormat pf = PF_UNKNOWN, NIIi mipmaps = -1,
            NIIf gamma = 1.0f, Nmark mark = Texture::MM_AGP_COLOUR | Texture::MM_AGP_ALPHA);

        /** 从流中加载
        @param[in] stream 像素来源,未编码的数据流
        @version NIIEngine 3.0.0
        */
        virtual Texture * load(ResourceID rid, GroupID gid, DataStream * stream, NCount w, NCount h, 
            Texture::Type type = Texture::T_2D, PixelFormat pf = PF_UNKNOWN, NIIi mipmaps = -1,
            NIIf gamma = 1.0f, Nmark mark = Texture::MM_AGP_COLOUR | Texture::MM_AGP_ALPHA);

        /** 获取渲染系统实际使用的格式
        @version NIIEngine 3.0.0 高级api
        */
        virtual PixelFormat getFormat(Texture::Type type, PixelFormat pf, Nmark mark) = 0;

        /** 把纹理转换成图片
        @note 仅用纹理的顶层mimap作为转换
        @param[in] dst 目标图片
        @param[in] src 源纹理
        @version NIIEngine 3.0.0
        */
        static void convert(Image & dst, Texture * src);

        /** 计算纹理大小(辅助)
        @param[in] mipmaps LOD映射
        @param[in] faces 面数量
        @version NIIEngine 3.0.0
        */
        static NCount calcSize(NCount mipmaps, NCount faces, NCount w, NCount h, NCount d, PixelFormat pf);

        /** 获取错误加载时使用的纹理
        @version NIIEngine 3.0.0
        */
        Texture * getWarning() const;

        /// @copydetails Singleton::getOnly
        static TextureManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static TextureManager * getOnlyPtr();
    protected:
        /// @copydetails Resource::init
        void init();
    protected:
        NCount mBitDepth;
        NCount mMipMapCount;
        mutable Texture * mWarningTexture;
    };
}
#endif