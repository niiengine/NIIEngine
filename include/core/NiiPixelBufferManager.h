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
#ifndef _PIXEL_BUFFER_MANAGER_H_
#define _PIXEL_BUFFER_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiPixelBuffer.h"
#include "NiiException.h"

namespace NII
{
    /** 像素缓存管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelBufferManager : public Singleton<PixelBufferManager>, public BufferAlloc
    {
    public:
        PixelBufferManager();
        ~PixelBufferManager();

        /** 加载创建像素缓存
        @param[in] file 脚本文件名字
        @version NIIEngine 3.0.0
        */
        void load(const String & file, GroupID gid = GroupUnknow);

        /** 加载创建像素缓存
        @param[in] id 像素标识
        @param[in] file 文件名字
        @param[in] rgid 资源组标识
        @version NIIEngine 3.0.0
        */
        void load(PixelBufferID id, const String & file, GroupID rgid = GroupUnknow);

        /** 创建(或获取)像素组
        @version NIIEngine 3.0.0
        */
        PixelBufferGroup * createGroup(Texture * obj, ScaleMode mode = SM_Unknow, const Vector2f & oft = Vector2f::ZERO,
            const Vector2f & pixeloft = Vector2f::ZERO, const PlaneSizef & srcRes = PlaneSizef(640.0f, 480.0f));

        /** 获取(或创建)像素组
        @version NIIEngine 3.0.0
        */
        PixelBufferGroup * getGroup(GroupID gid);

        /** 删除像素组
        @version NIIEngine 3.0.0
        */
        void destroyGroup(PixelBufferGroup * obj);

        /** 创建像素缓存
        @param[in] id 像素标识
        @param[in] rid 像素纹理源
        @version NIIEngine 3.0.0
        */
        PixelBuffer * create(PixelBufferID id, PixelBufferGroup * pbg, const Rectf & area);

        /** 获取指定的像素缓存
        @param[in] pid 像素标识
        @version NIIEngine 3.0.0
        */
        PixelBuffer * get(PixelBufferID pid) const;

        /** 获取指定的像素缓存
        @param[in] rid 像素纹理源
        @param[in] id 像素标识
        @version NIIEngine 3.0.0
        */
        PixelBuffer * get(GroupID rid, PixelBufferID id) const;

        /** 是否存在定义的像素缓存
        @param[in] pid 像素标识
        @version NIIEngine 3.0.0
        */
        bool isExist(PixelBufferID id) const;

        /** 是否存在定义的像素缓存
        @param[in] id 像素标识
        @param[in] rid 像素纹理源
        @version NIIEngine 3.0.0
        */
        bool isExist(GroupID rid, PixelBufferID id) const;

        /** 删除像素组
        @param[in] rid 像素纹理源
        @param[in] deltex 删除纹理
        */
        void destroy(GroupID rid, PixelBufferID id, bool deltex = true);

        /** 删除像素组
        @param[in] rid 像素纹理源
        @param[in] deltex 删除纹理
        */
        void destroy(GroupID rid, bool deltex = true);

        /** 删除像素
        @version NIIEngine 3.0.0
        */
        void destroyAll(bool deltex = true);

        /** 获取像素组中的像素数量
        @param[in] rid 像素纹理源
        @version NIIEngine 3.0.0
        */
        NCount getCount(GroupID rid) const;

        /** 视图大小
        @param size 尺寸大小
        @version NIIEngine 3.0.0
        */
        void notifyView(const PlaneSizei & size);

        /** 设置资源组
        @version NIIEngine 3.0.0
        */
        static void setGroup(GroupID rgid);

        /** 获取资源组
        @version NIIEngine 3.0.0
        */
        static GroupID getGroup();
    protected:
        GroupID genValidID() const;
    private:
        typedef map<GroupID, PixelBufferGroup *>::type PixelGroupList;
        typedef multimap<ResourceID, PixelBufferGroup *>::type ResourceGroupList;
        PixelGroupList mPBGroupList;
        ResourceGroupList mPBRList;
        mutable GroupID mValidRID;
        N_mutex(mValidGIDMutex)
        static GroupID ResourceGroup;
    };
}
#endif