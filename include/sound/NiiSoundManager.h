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

#ifndef NII_SOUNDMANAGER_H
#define NII_SOUNDMANAGER_H

#include "NiiPreInclude.h"
#include "NiiSingleton.h"

namespace NII
{
    /**
    @version NIIEngine 4.0.0
    */
    class SoundManager : public ResourceManager, public Singleton<SoundManager>
    {
        friend class AudioBufferManager;
    public:
        virtual ~SoundManager();

        /**
        @version NIIEngine 4.0.0
        */
        void attach(Sound * sound, AudioBuffer * buffer);

        /**
        @version NIIEngine 4.0.0
        */
        void detach(Sound * sound, AudioBuffer * buffer);

        /// @copydetails Singleton::getOnly
        static SoundManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static SoundManager * getOnlyPtr();
    protected:
        SoundManager();
        
        /** 
        @version NIIEngine 4.0.0
        */
        void notifyLoad(AudioBuffer * buffer);

        /** 
        @version NIIEngine 4.0.0
        */
        void notifyUnload(const String & buf);
        
        /// @copydetails ResourceManager::init
        void init();
        
        /// @copydetails ResourceManager::createImpl
        virtual Resource * createImpl(ResourceID rid, GroupID gid, 
            ResLoadScheme * ls, ResResultScheme * rs, const PropertyData * params) = 0;
    private:
        typedef map<String, SoundList>::type AttachList;
        AttachList mAttachList;
    };
}
#endif