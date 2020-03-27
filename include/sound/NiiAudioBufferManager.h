/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-5-7

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

#ifndef NII_AUDIOBUFFERMANAGER_H
#define NII_AUDIOBUFFERMANAGER_H

#include "NiiPreInclude.h"
#include "NiiSoundUnit.h"

namespace NII
{
    /**
    @version NIIEngine 4.0.0
    */
    class AudioBufferManager : public Singleton<AudioBufferManager>, public SoundAlloc
    {
    public:
        /** 
        @version NIIEngine 4.0.0
        */
        class GroupAudio
        {
        public:
            GroupAudio(GroupID gid);
            inline GroupID getID() const { return mID; }

            /** 
            @version NIIEngine 4.0.0
            */
            bool isLoad() const;

            /** 
            @version NIIEngine 4.0.0
            */
            void load(bool async);

            /** 
            @version NIIEngine 4.0.0
            */
            bool unload();

            /** 
            @version NIIEngine 4.0.0
            */
            AudioBuffer * getBuffer(Nid id) const;

            /** 
            @version NIIEngine 4.0.0
            */
            NCount getSize() const;
        protected:
            /**
            @version NIIEngine 4.0.0
            */
            void update();
        protected:
            typedef map<BufferID, AudioBuffer *>::type BufferList;

            GroupID mID;
            BufferList mBufferList;
            bool mLoad;
            bool mComplete;
            bool mAsyncComplete;
        };
    public:
        AudioBufferManager();
        ~AudioBufferManager();

        /** 
        @version NIIEngine 3.0.0
        */
        void init();

        /** 
        @version NIIEngine 3.0.0
        */
        AudioBuffer * get(BufferID bid, GroupID gid = 0);

        /** 
        @version NIIEngine 3.0.0
        */
        GroupAudio * getGroup(GroupID gid);

        /** 
        @version NIIEngine 3.0.0
        */
        void reload(bool async);

        /** 
        @version NIIEngine 3.0.0
        */
        void releaseAll();

        /// @copydetails Singleton::getOnly
        static AudioBufferManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static AudioBufferManager * getOnlyPtr();
    protected:
        /**
        @version NIIEngine 4.0.0
        */
        void onUpdate();
    private:
        typedef map<GroupID, GroupAudio *>::type GroupList;
        typedef map<String, ResourceIDList>::type DefineList;

        GroupList mGroupList;
        DefineList mDefineList;
    };
}
#endif