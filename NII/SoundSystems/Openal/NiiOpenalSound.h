/*
-----------------------------------------------------------------------------
面向游戏设计引擎

时间: 2012-8-3

作者: zelda_ok

格式: open it use utf-8

主页: www.nii-zelda.tk

授权方式:商业授权(www.nii-zelda.tk/license)
------------------------------------------------------------------------------
*/

#ifndef _NII_OPENAL_SOUND_H_
#define _NII_OPENAL_SOUND_H_


namespace NII
{
    /// NII专用的openal的Sound实现
    class OpenalSound : public Sound
    {
    public:
        enum SourceType
        {
            ST_SourceSound,
            ST_SourceMusic
        };
    public:
        OpenalSound(ResourceManager * creator, const String & name, ResourceHandle handle,
            const String & group, bool isManual = false, ManualResourceLoader * loader);

        ~OpenalSound();

        /// @copydetails Sound::addPartner
        void addPartner(VoiceBuffer * partner);

        /// @copydetails Sound::getPartnerCount
        Nui getPartnerCount() const;

        /** 获取当前存在的错误
        @remark 如果不潜在任何错误前调用这个方法,不会发生任何事情,但当存在错误的时候,调用这个
            方法会抛出一个异常
        */
        static void getError() const;
    protected;
        /// @copydetails Resource::loadImpl
        void loadImpl();

        /// @copydetails Resource::unloadImpl
        void unloadImpl();

        /// @copydetails Sound::setupImpl
        void setupImpl();

        /// @copydetails Sound::playImpl
        void playImpl();

        /// @copydetails Sound::stopImpl
        void stopImpl();

        /// @copydetails Sound::pauseImpl
        void pauseImpl();

        /// @copydetails Sound::bindImpl
        void bindImpl();
    private:
        void updateManualRolloff();
    private:
        ALuint mTarget;
        ALuint mDirectFilter;
        ALuint mEffectFilter;

    };
}
#endif