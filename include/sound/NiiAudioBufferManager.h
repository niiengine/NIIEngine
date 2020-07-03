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
        GroupAudio * createGroup(GroupID gid);
        
        /** 
        @version NIIEngine 3.0.0
        */
        void removeGroup(GroupID gid);
        
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