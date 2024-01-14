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
#ifndef _NII_PIXELBUFFER_H_
#define _NII_PIXELBUFFER_H_

#include "NiiPreInclude.h"
#include "NiiPlaneSize.h"
#include "NiiRect.h"

namespace NII
{
    /** 像素缓存
    @remark
        像素概念比面/片复杂点.像素的实际渲染效果是依赖缩放方式,实际渲染大小等参数
        确定的
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelBuffer : public ResourceAlloc
    {
        friend class GeometryPixel;
        friend class PixelManager;
    public:
        PixelBuffer(PixelBufferID pxid, PixelBufferGroup * pbg, const Rectf & area = Rectf::ZERO);
        PixelBuffer(PixelBufferID pxid, Texture * obj, const Rectf & area = Rectf::ZERO, ScaleMode mode = SM_Unknow,
            const Vector2f & oft = Vector2f::ZERO, const Vector2f & pixeloft = Vector2f::ZERO,  
                const PlaneSizef & srcRes = PlaneSizef(640.0f, 480.0f));
        ~PixelBuffer();

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        inline PixelBufferID getID() const { return mID;  }

        /** 设置像素区域
        @param[in] area 区域定义(像素级)
        @version NIIEngine 3.0.0
        */
        void setArea(const Rectf & area);

        /** 设置缩放模式
        @param[in] mode 模式
        @version NIIEngine 3.0.0
        */
        void setMode(ScaleMode mode);

        /** 获取资源原分辨率
        @note (像素级)
        @version NIIEngine 3.0.0
        */
        void setNativeRes(const PlaneSizef & area);

        /** 设置像素来源
        @param[in] obj 纹理
        @version NIIEngine 3.0.0
        */
        void setTexture(Texture * obj);

        /** 设置实际偏移
        @return 实际渲染到屏幕的位置偏移(屏幕级)
        @version NIIEngine 3.0.0
        */
        void setOffset(const Vector2f & oft);

        /** 设置渲染偏移
        @param[in] oft 像素位置偏移(像素级)
        @version NIIEngine 3.0.0
        */
        void setPixelOffset(const Vector2f & oft);

        /** 获取实际尺寸
        @return 实际渲染尺寸(屏幕级)
        @version NIIEngine 3.0.0
        */
        inline const PlaneSizef & getSize() const { return mSize;  }

        /** 设置缩放模式
        @param[in] mode 模式
        @version NIIEngine 3.0.0
        */
        ScaleMode getMode() const;

        /** 设置像素来源
        @param[in] obj 纹理
        @version NIIEngine 3.0.0
        */
        Texture * getTexture() const;

        /** 获取群组
        @version NIIEngine 3.0.0
        */
        GroupID getGroupID() const;

        /** 获取实际偏移
        @return 实际渲染偏移(屏幕级)
        @version NIIEngine 3.0.0
        */
        const Vector2f & getOffset() const;

        /** 设置渲染偏移
        @param[in] oft 像素位置偏移(像素级)
        @version NIIEngine 3.0.0
        */
        const Vector2f & getPixelOffset() const;

        /** 获取资源原分辨率
        @note (像素级)
        @version NIIEngine 3.0.0
        */
        const PlaneSizef & getNativeRes() const;

        /** 确定视图的实际大小
        @version NIIEngine 3.0.0
        */
        void notifyRatio(const Vector2f & aspect);

        /** 计算缩放比例
        @param[in] out 缩放比例
        @version NIIEngine 3.0.0
        */
        static void calcAspectRatio(ScaleMode mode, const PlaneSizei & dest, const PlaneSizef & src, Vector2f & out);
    protected:
        PixelBufferID mID;
        PixelBufferGroup * mGroup;
        Rectf mArea;
        PlaneSizef mSize;
    };

    /** 像素缓存群组
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelBufferGroup : public ResourceAlloc
    {
        friend class PixelManager;
    public:
        typedef map<PixelBufferID, PixelBuffer *>::type PBList;
    public:
        bool operator == (PixelBufferGroup & o);

        /** 添加像素单元
        @version NIIEngine 3.0.0
        */
        inline void add(PixelBuffer * pb)                   { mPBList.insert(Npair(pb->getID(), pb)); }

        /** 获取指定的像素单元
        @param[in] id 像素标识
        @version NIIEngine 3.0.0
        */
        inline PixelBuffer * get(PixelBufferID pid) const   { PBList::const_iterator i = mPBList.find(pid); return (i != mPBList.end()) ? i->second : 0; }

        /** 是否存在定义的像素单元
        @param[in] pid 像素标识
        @version NIIEngine 3.0.0
        */
        inline bool isExist(PixelBufferID pid) const        { return mPBList.find(pid) != mPBList.end(); }

        /** 移去像素单元
        @param[in] pid 像素标识
        @note 移去后,本类不管理参数内存
        @version NIIEngine 3.0.0
        */
        inline PixelBuffer * remove(PixelBufferID pid);

        /** 获取像素单元数量
        @version NIIEngine 3.0.0
        */
        inline NCount getCount() const                      { return mPBList.size(); }

        /** 获取群组
        @version NIIEngine 3.0.0
        */
        inline GroupID getGroupID() const                   { return mGroupID; }

        /** 设置像素来源
        @param[in] obj 纹理
        @version NIIEngine 3.0.0
        */
        inline void setTexture(Texture * obj)               { mTexture = obj; }

        /** 获取像素来源
        @param[in] obj 纹理
        @version NIIEngine 3.0.0
        */
        inline Texture * getTexture() const                 { return mTexture; }

        /** 设置缩放模式
        @param[in] mode 模式
        @version NIIEngine 3.0.0
        */
        inline void setMode(ScaleMode mode)                 { mMode = mode; }

        /** 获取缩放模式
        @param[in] mode 模式
        @version NIIEngine 3.0.0
        */
        inline ScaleMode getMode() const                    { return mMode; }

        /** 设置资源原分辨率
        @note (像素级)
        @version NIIEngine 3.0.0
        */
        inline void setNativeRes(const PlaneSizef & area)   { mNativeRes = area; }

        /** 获取资源原分辨率
        @note (像素级)
        @version NIIEngine 3.0.0
        */
        inline const PlaneSizef & getNativeRes() const      { return mNativeRes; }

        /** 设置渲染偏移
        @param[in] oft 像素在纹理上的位置偏移(像素级)
        @version NIIEngine 3.0.0
        */
        inline void setPixelOffset(const Vector2f & oft)    { mPixelOffset = oft; }

        /** 获取渲染偏移
        @param[in] oft 像素在纹理上的位置偏移(像素级)
        @version NIIEngine 3.0.0
        */
        inline const Vector2f & getPixelOffset() const      { return mPixelOffset; }

        /** 设置实际偏移
        @return 实际渲染到屏幕的位置偏移(屏幕级)
        @version NIIEngine 3.0.0
        */
        inline void setOffset(const Vector2f & oft)         { mOffset = oft; }

        /** 获取实际偏移
        @return 实际渲染到屏幕的位置偏移(屏幕级)
        @version NIIEngine 3.0.0
        */
        inline const Vector2f & getOffset() const           { return mOffset; }

        /** 确定视图的实际大小
        @version NIIEngine 3.0.0
        */
        void notifyView(const PlaneSizei & size);

        const PBList & getList() const                      { return mPBList; }
    protected:
        PixelBufferGroup();
        PixelBufferGroup(GroupID gid);
        PixelBufferGroup(GroupID gid, Texture * obj, ScaleMode mode = SM_Unknow, const Vector2f & oft = Vector2f::ZERO,
            const Vector2f & pixeloft = Vector2f::ZERO, const PlaneSizef & srcRes = PlaneSizef(640.0f, 480.0f));
        ~PixelBufferGroup();
    protected:
        PBList mPBList;
        GroupID mGroupID;
        Texture * mTexture;
        ScaleMode mMode;
        Vector2f mPixelOffset;
        Vector2f mOffset;
        PlaneSizef mNativeRes;
    };

    inline ScaleMode PixelBuffer::getMode() const { return mGroup->getMode(); }
    inline Texture * PixelBuffer::getTexture() const { return mGroup->getTexture(); }
    inline GroupID PixelBuffer::getGroupID() const { return mGroup->getGroupID(); }
    inline const Vector2f & PixelBuffer::getOffset() const { return mGroup->getOffset(); }
    inline const Vector2f & PixelBuffer::getPixelOffset() const { return mGroup->getPixelOffset(); }
    inline const PlaneSizef & PixelBuffer::getNativeRes() const { return mGroup->getNativeRes(); }

    typedef vector<PixelBufferGroup *>::type PixelBufferGroupList;
}
#endif